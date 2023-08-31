#include <utility>
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>

#include "HuffmanParallel.h"
#include "../utils/utimer.cpp"
#include "../utils/huffman-commons.h"

HuffmanParallel::HuffmanParallel(size_t n_mappers, size_t n_encoders, string filename, size_t n_reducers=0) {
    this->n_mappers = n_mappers;
    this->n_encoders = n_encoders;
    this->n_reducers = n_reducers;
    this->filename = std::move(filename);
    this->seq = read_file(this->filename);
    this->tree = nullptr;
}

HuffmanParallel::~HuffmanParallel() {
    if(tree) free_tree(this->tree);
    free_encoding(*this->encoded);
    free_codes(this->codes);
}

/* sequential reduce version */
unordered_map<char, unsigned int> HuffmanParallel::generate_frequency() {

    unordered_map<char, unsigned> partial_freqs[n_mappers]; // map fusion
    unordered_map<char, unsigned> result;
    vector<thread> thread_mappers(n_mappers);

    auto map_executor = [&](size_t tid) {

        // delegate the computation of the partial frequencies to the mappers.
        // we split everything here in chunks in order to make it parallel computation
        // so, splitting phase
        auto start = tid * (seq.length() / n_mappers);
        auto end = (tid + 1) * (seq.length() / n_mappers);
        if (tid == n_mappers - 1) end = seq.length();

        // mapping phase.
        // note: instead of returning the tuple (char, 1) we return a map with the partial frequencies.
        // this will reduce the amount of data to be transferred to the reducers. (map fusion)
        for (size_t i = start; i < end; i++) partial_freqs[tid][seq[i]]++;
    };

    // start the threads
    for (size_t i = 0; i < n_mappers; i++) thread_mappers[i] = thread(map_executor, i);
    for (auto &t: thread_mappers) t.join();
    for (auto &partial_freq: partial_freqs) for (auto &it: partial_freq) result[it.first] += it.second;

    return result;
}


/** Parallel reduce version */
unordered_map<char, unsigned> HuffmanParallel::generate_frequency_gmr(){
    cout << "Generating frequencies with GMR" << endl;
    unordered_map<char, unsigned> partial_freqs[this->n_mappers];
    vector<mutex> red_mutexes(this->n_reducers);
    vector<condition_variable> red_conds(this->n_reducers);
    vector<thread> thread_mappers(n_mappers);
    vector<thread> thread_reducers(n_reducers);
    queue<pair<char, unsigned>> red_queues[n_reducers];

    mutex res_mutex;
    unordered_map<char, unsigned> result;

    auto map_executor = [&](size_t tid)
    {
        // delegate the computation of the partial frequencies to the mappers.
        // we split everything here in chunks in order to make it parallel computation
        // so, splitting phase
        auto length = this->seq.length();
        auto start = tid * (seq.length() / n_mappers);
        auto end = (tid + 1) * (seq.length() / n_mappers);
        if (tid == n_mappers - 1)
            end = seq.length();

        // mapping phase.
        // note: instead of returning the tuple (char, 1) we return a map with the partial frequencies.
        // this will reduce the amount of data to be transferred to the reducers.
        for (size_t i = start; i < end; i++)
        {
            partial_freqs[tid][seq[i]]++;
        }

        // push the partial frequencies to the reducers queues.
        for (auto &it : partial_freqs[tid])
        {
            auto red_id = it.first % n_reducers;
            unique_lock<mutex> lock(red_mutexes[red_id]);  // we need to lock the queues
            red_queues[red_id].emplace(it.first, it.second);
            red_conds[red_id].notify_one();
        }
    };

    auto reduce_executor = [&](size_t nred)
    {
        unordered_map<char, unsigned> partial_res;

        // reduce phase, until nullptr is received.
        while (true)
        {
            {
                unique_lock<mutex> lock(red_mutexes[nred]);
                red_conds[nred].wait(lock, [&](){ return !red_queues[nred].empty(); });
                auto pair = red_queues[nred].front();
                red_queues[nred].pop();    
                if (pair.first == '\0')
                break;
                partial_res[pair.first] += pair.second; 
            }
        }

        // merge the partial results
        unique_lock<mutex> lock(res_mutex);
        for (auto &it : partial_res)
            result[it.first] += it.second;
    };

    // start the threads
    for (size_t i = 0; i < n_reducers; i++)
        thread_reducers[i] = thread(reduce_executor, i);
    for (size_t i = 0; i < n_mappers; i++)
        thread_mappers[i] = thread(map_executor, i);

    // join mappers
    for (auto &t : thread_mappers)
        t.join();

    // push nullptr to reducers
    for (size_t i = 0; i < n_reducers; i++)
    {
        unique_lock<mutex> lock(red_mutexes[i]);
        red_queues[i].emplace('\0', 0);
        red_conds[i].notify_one();
    }

    // join reducers
    for (auto &t : thread_reducers)
        t.join();
    return result;
}


encoded_t* HuffmanParallel::encode() {
    vector<thread> thread_encoder(n_encoders);
    auto size = seq.length();
    auto results = new encoded_t (n_encoders);
    
    // executor body: it will compute chunk size and add the chunk to the vector of chunks
    // no lock needed since splits are independent.
    auto encode_executor = [&](size_t tid) {
        // split the sequence in chunks
        auto start = tid * (size / n_encoders);
        auto end = (tid + 1) * (size / n_encoders);
        if (tid == n_encoders - 1) end = size;

        // encode the chunk -> this will make memory allocation parallel
        results->at(tid) = new chunk_t();
        results->at(tid)->reserve(end - start);
        for (size_t i = start; i < end; i++) {
            results->at(tid)->push_back(codes[seq[i]]);
        }
    };

    // start and join the threads
    for (size_t i = 0; i < n_encoders; i++) thread_encoder[i] = thread(encode_executor, i);
    for (auto &t: thread_encoder) t.join();
    return results;
}

void HuffmanParallel::run() {

    /** frequency map generation **/
    long time_read;
    {
        utimer timer("read time", &time_read);
        this->seq = read_file(this->filename);
    }

    long time_freqs;
    unordered_map<char, unsigned> freqs;
    {
        utimer timer("freqs time", &time_freqs);
        if (n_reducers>0) freqs = generate_frequency_gmr();
        else freqs = generate_frequency();
    }

    /** huffman tree generation **/
    long time_tree_codes;
    {
        utimer timer("tree codes time", &time_tree_codes);
        this->tree = generate_huffman_tree(freqs);
        this->codes = generate_huffman_codes(tree);
    }


    /** encoding **/
    long time_encoding;
    {
        utimer timer("encoding time", &time_encoding);
        this->encoded = encode();
    }
    cout << "encoded size: " << encoded->size() << endl;
    /** writing **/
    long time_writing;
    {
        utimer timer("writing time", &time_writing);
        write_to_file(*encoded, OUTPUT_FILE);
    }

    //check file and print result in green if correct, red otherwise.
    #ifdef CHKFILE
       check_file(OUTPUT_FILE, seq, this->tree);
    #endif  
    auto type = n_reducers > 0 ? TYPE_GMR + to_string(n_reducers): TYPE_MAP;
    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, n_mappers, n_reducers, n_encoders, type);
}





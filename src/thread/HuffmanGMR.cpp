//
// Created by Luca Miglior on 24/07/23.
//

#include <utility>
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "HuffmanGMR.h"
#include "../utils/huffman-commons.h"
#include "../utils/utimer.cpp"


HuffmanGMR::HuffmanGMR(size_t n_mappers, size_t n_reducers, size_t n_encoders, string filename)
{
    this->n_mappers = n_mappers;
    this->n_reducers = n_reducers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    this->tree = nullptr;
}

HuffmanGMR::~HuffmanGMR()
{
    if (tree)free_tree(this->tree);
    free_codes(this->codes);
}

unordered_map<char, unsigned int> HuffmanGMR::generate_frequency()
{

    unordered_map<char, unsigned> partial_freqs[n_mappers];
    vector<mutex> red_mutexes(n_reducers);
    vector<condition_variable> red_conds(n_reducers);
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
            unique_lock<mutex> lock(red_mutexes[red_id]);
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

vector<vector<bool> *> HuffmanGMR::encode()
{
    vector<thread> thread_encoder(n_encoders);
    auto size = seq.length();
    auto buffer = vector<vector<bool> *>(size);

    // split sequence in chunks and delegate the encoding to the mappers.
    auto encode_executor = [&](size_t tid)
    {
        // make an equal split of the sequence
        auto start = tid * (size / n_encoders);
        auto end = (tid + 1) * (size / n_encoders);
        if (tid == n_encoders - 1)
            end = size;
        for (size_t i = start; i < end; i++)
            buffer[i] = codes[seq[i]];
    };

    for (size_t i = 0; i < n_encoders; i++)
        thread_encoder[i] = thread(encode_executor, i);
    for (auto &t : thread_encoder)
        t.join();

    return buffer;
}

void HuffmanGMR::run()
{

    /** frequency map generation **/
    long time_read, time_freqs, time_tree_codes, time_encoding, time_writing;
    {
        utimer timer("HuffmanGMR", &time_read);
        this->seq = read_file(this->filename);
    }

    {
        utimer timer("HuffmanGMR", &time_freqs);
        this->freq_map = generate_frequency();
    }


    /** huffman tree generation **/
    {
        utimer timer("HuffmanGMR", &time_tree_codes);
        this->tree = generate_huffman_tree(this->freq_map);
        this->codes = generate_huffman_codes(this->tree);
    }

    /** encoding **/
    vector<vector<bool> *> encoded;
    {
        utimer timer("HuffmanGMR", &time_encoding);
        this->encoded_seq = encode();
    }

    /** writing **/
    {
        utimer timer("HuffmanGMR", &time_writing);
        write_to_file(this->encoded_seq, OUTPUT_FILE);
    }

    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, n_mappers, n_reducers, n_encoders);


    #ifdef CHKFILE
        check_file(OUTPUT_FILE, this->seq, this->tree); 
    #endif
}
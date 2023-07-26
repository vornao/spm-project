//
// Created by Luca Miglior on 24/07/23.
//

#include "HuffmanParallel.h"
#include <utility>
#include <iostream>
#include <thread>
#include "../utils/huffman-commons.h"

HuffmanParallel::HuffmanParallel(size_t n_mappers, size_t n_reducers, size_t n_encoders, string filename) {
    this->n_mappers = n_mappers;
    this->n_reducers = n_reducers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    this->tree = nullptr;

    // read files
    this -> seq = read_file(this->filename);
}

HuffmanParallel::~HuffmanParallel() {
    if(tree) free_tree(this->tree);
}

unordered_map<char, unsigned int> HuffmanParallel::generate_frequency() {

    unordered_map<char, unsigned> partial_freqs[n_mappers];
    vector<mutex> red_mutexes(n_reducers);
    vector<condition_variable> red_conds(n_reducers);
    vector<thread> thread_mappers(n_mappers);
    vector<thread> thread_reducers(n_reducers);
    queue<pair<char, unsigned>> red_queues[n_reducers];

    mutex res_mutex;
    unordered_map<char, unsigned> result;

    auto map_executor = [&](size_t tid) {

        // delegate the computation of the partial frequencies to the mappers.
        // we split everything here in chunks in order to make it parallel computation
        // so, splitting phase
        auto start = tid * (seq.length() / n_mappers);
        auto end = (tid + 1) * (seq.length() / n_mappers);
        if (tid == n_mappers - 1) end = seq.length();

        // mapping phase.
        // note: instead of returning the tuple (char, 1) we return a map with the partial frequencies.
        // this will reduce the amount of data to be transferred to the reducers.
        for (size_t i = start; i < end; i++) {
            partial_freqs[tid][seq[i]]++;
        }

        // push the partial frequencies to the reducers queues.
        for (auto &it: partial_freqs[tid]) {
            auto red_id = it.first % n_reducers;
            unique_lock<mutex> lock(red_mutexes[red_id]);
            red_queues[red_id].emplace(it.first, it.second);
            red_conds[red_id].notify_one();
        }
    };

    auto reduce_executor = [&](size_t nred) {
        unordered_map<char, unsigned> partial_res;

        // reduce phase, until nullptr is received.
        while (true) {
            unique_lock<mutex> lock(red_mutexes[nred]);
            red_conds[nred].wait(lock, [&]() { return !red_queues[nred].empty(); });
            auto pair = red_queues[nred].front();
            red_queues[nred].pop();
            lock.unlock();

            if (pair.first == '\0') break;
            partial_res[pair.first] += pair.second;
        }

        // merge the partial results
        unique_lock<mutex> lock(res_mutex);
        for (auto &it: partial_res) result[it.first] += it.second;
    };

    // start the threads
    for (size_t i = 0; i < n_reducers; i++) thread_reducers[i] = thread(reduce_executor, i);
    for (size_t i = 0; i < n_mappers; i++) thread_mappers[i] = thread(map_executor, i);

    // join mappers
    for (auto &t: thread_mappers) t.join();

    // push nullptr to reducers
    for (size_t i = 0; i < n_reducers; i++) {
        unique_lock<mutex> lock(red_mutexes[i]);
        red_queues[i].emplace('\0', 0);
        red_conds[i].notify_one();
    }

    // join reducers
    for (auto &t: thread_reducers) t.join();
    return result;
}

unique_ptr<vector<vector<bool>>> HuffmanParallel::encode() {
    vector<thread> thread_encoder(n_encoders);
    unique_ptr<vector<vector<bool>>> buffer = make_unique<vector<vector<bool>>>(seq.size());

    // split sequence in chunks and delegate the encoding to the mappers.
    auto encode_executor = [&](size_t tid) {
        auto start = tid * (seq.size() / n_encoders);
        auto end = (tid + 1) * (seq.size() / n_encoders);
        if (tid == n_encoders - 1) end = seq.size();

        for (size_t i = start; i < end; i++) {
            auto c = seq[i];
            auto code = (*codes)[c];
            buffer->at(i) = code;
        }
    };

    for (size_t i = 0; i < n_encoders; i++) thread_encoder[i] = thread(encode_executor, i);
    for (auto &t: thread_encoder) t.join();

    return buffer;
}

void HuffmanParallel::run() {

    /** frequency map generation **/
    cout << "> Generating frequency map (parallel GMR)... ";
    auto start = chrono::high_resolution_clock::now();
    auto freqs = generate_frequency();
    auto end_freqs = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end_freqs - start).count();
    cout << "(took " << elapsed << "ms)" << endl;

    /** huffman tree generation **/
    this -> tree = generate_huffman_tree(freqs);
    this -> codes = generate_huffman_codes(tree);


    /** encoding **/
    cout << "> Encoding sequence...";
    auto start_encoding = chrono::high_resolution_clock::now();
    auto encoded = encode();

    auto end = chrono::high_resolution_clock::now();
    auto elapsed_encoding = chrono::duration_cast<chrono::milliseconds>(end - start_encoding).count();
    cout << "(took " << elapsed_encoding << "ms)" << endl;

    write_to_file(*encoded, OUTPUT_FILE);


    // check file and print result in green if correct, red otherwise.
    if (check_file(OUTPUT_FILE, seq, tree)) cout << "\033[1;32m> File is correct!\033[0m" << endl;
    else cout << "\033[1;31mWrong!\033[0m" << endl;

    auto total_elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "> Total time (GMR): " << total_elapsed << "ms" << endl;
}





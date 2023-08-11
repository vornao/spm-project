//
// Created by Luca Miglior on 24/07/23.
//

#include "HuffmanParallel.h"
#include <utility>
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <optional>
#include "../utils/utimer.cpp"


#include "../utils/huffman-commons.h"

HuffmanParallel::HuffmanParallel(size_t n_mappers, size_t n_encoders, string filename) {
    this->n_mappers = n_mappers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    this->seq = read_file(this->filename);
    this->tree = nullptr;
}

HuffmanParallel::~HuffmanParallel() {
    if(tree) free_tree(this->tree);
    free_codes(this->codes);
    free_encoding(this->encoded);

}

unordered_map<char, unsigned int> HuffmanParallel::generate_frequency() {

    unordered_map<char, unsigned> partial_freqs[n_mappers];
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
        // this will reduce the amount of data to be transferred to the reducers.
        for (size_t i = start; i < end; i++) partial_freqs[tid][seq[i]]++;
    };

    // start the threads
    for (size_t i = 0; i < n_mappers; i++) thread_mappers[i] = thread(map_executor, i);
    for (auto &t: thread_mappers) t.join();
    for (auto &partial_freq: partial_freqs) for (auto &it: partial_freq) result[it.first] += it.second;

    return result;
}

vector<vector<vector<bool>*>*>HuffmanParallel::encode() {
    vector<thread> thread_encoder(n_encoders);
    auto size = seq.length();
    auto results = vector<vector<vector<bool>*>*>(n_encoders);
    
    auto encode_executor = [&](size_t tid) {
        // split the sequence in chunks
        auto start = tid * (size / n_encoders);
        auto end = (tid + 1) * (size / n_encoders);
        if (tid == n_encoders - 1) end = size;

        // encode the chunk -> this will make memory allocation parallel
        results[tid] = new vector<vector<bool>*>();
        results[tid]->reserve(end - start);
        for (size_t i = start; i < end; i++) {
            results[tid]->push_back(codes[seq[i]]);
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
        freqs = generate_frequency();
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
    vector<vector<vector<bool>*>*> encoded;
    {
        utimer timer("encoding time", &time_encoding);
        encoded = encode();
    }

    /** writing **/
    long time_writing;
    {
        utimer timer("writing time", &time_writing);
        write_to_file( encoded, OUTPUT_FILE);
    }

    //check file and print result in green if correct, red otherwise.
    #ifdef CHKFILE
        check_file(OUTPUT_FILE, seq, this->tree);
    #endif

    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, n_mappers, 0, n_encoders);
}





//
// Created by Luca Miglior on 24/07/23.
//

#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <thread>
#include <utility>

#include "HuffmanFastFlow.h"
#include "../utils/huffman-commons.h"
#include "../utils/utimer.cpp"

using namespace ff;

HuffmanFastFlow::HuffmanFastFlow(size_t n_mappers, size_t n_reducers, size_t n_encoders, string filename) {
    this->n_mappers = n_mappers;
    this->n_reducers = n_reducers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    this -> seq = read_file(this->filename);
}

HuffmanFastFlow::~HuffmanFastFlow() {
    if (tree!=nullptr) free_tree(tree);
    free_codes(codes);
}

vector<vector<bool>*> HuffmanFastFlow::encode() {
    auto buffer = vector<vector<bool>*>(seq.length());
    auto body = [&](const long i){
        auto code = codes[seq[i]];
        buffer[i] = code;
    };

    auto pf = ParallelFor((long)n_encoders);
    pf.parallel_for(0, (long)seq.size(), 1, body);

    return buffer;
}

unordered_map<char, unsigned int> HuffmanFastFlow::generate_frequency() {
    auto res = unordered_map<char, unsigned int>();

    auto map_f = [&](const long i, unordered_map<char, unsigned> &tempsum){
        tempsum[seq[i]]++;
    };

    auto red_f = [&](unordered_map<char, unsigned> &a, const unordered_map<char, unsigned> &b){
        for(auto &it: b) a[it.first] += it.second;
    };

    auto pf = ParallelForReduce<unordered_map<char, unsigned>>(n_mappers);
    pf.parallel_reduce(res, unordered_map<char, unsigned>(), 0, (long)seq.size(), 1, map_f, red_f);
    return res;
}

void HuffmanFastFlow::run() {
    /** frequency map generation **/
    long time_read;
    {
        utimer timer("Reading file", &time_read);
        this -> seq = read_file(this->filename);
    }


    long time_freqs;
    unordered_map<char, unsigned int> freqs;
    {
        utimer timer("Frequency generation", &time_freqs);
        freqs = generate_frequency();
    }


    /** huffman tree generation **/
    long time_tree_codes;
    {
        utimer timer("Tree and codes generation", &time_tree_codes);
        this -> tree = generate_huffman_tree(freqs);
        this -> codes = generate_huffman_codes(tree);
    }


    long time_encoding;
    vector<vector<bool>*> encoded;
    {
        utimer timer("Encoding", &time_encoding);
        encoded = encode();
    }

    long time_writing;
    {
        utimer timer("Writing file", &time_writing);
        write_to_file(encoded, OUTPUT_FILE);
    }


    //check file and print result in green if correct, red otherwise.
    #ifdef CHKFILE
        check_file(OUTPUT_FILE, seq, this->tree);
    #endif

    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, n_mappers, n_reducers, n_encoders);
}


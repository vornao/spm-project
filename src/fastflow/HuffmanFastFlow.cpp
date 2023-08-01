//
// Created by Luca Miglior on 24/07/23.
//

#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <thread>
#include <utility>

#include "HuffmanFastFlow.h"
#include "../utils/huffman-commons.h"

using namespace ff;

HuffmanFastFlow::HuffmanFastFlow(size_t n_mappers, size_t n_reducers, size_t n_encoders, string filename) {
    this->n_mappers = n_mappers;
    this->n_reducers = n_reducers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);

    // read files
    this -> seq = read_file(this->filename);
}

HuffmanFastFlow::~HuffmanFastFlow() {
    if (tree!=nullptr) free_tree(tree);
    free_codes(codes);
}

vector<vector<bool>*> HuffmanFastFlow::encode() {
    // create
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

    auto pf = ParallelForReduce<unordered_map<char, unsigned>>(8);
    pf.parallel_reduce(res, unordered_map<char, unsigned>(), 0, (long)seq.size(), 1, map_f, red_f);
    return res;
}

void HuffmanFastFlow::run() {
    /** frequency map generation **/
    auto read_start = chrono::high_resolution_clock::now();
    this -> seq = read_file(this->filename);
    auto time_read = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - read_start).count();

    auto start = chrono::high_resolution_clock::now();
    auto freqs = generate_frequency();
    auto time_freqs = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start).count();

    /** huffman tree generation **/
    auto start_tree_codes = chrono::high_resolution_clock::now();
    this -> tree = generate_huffman_tree(freqs);
    this -> codes = generate_huffman_codes(tree);
    auto time_tree_codes = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_tree_codes).count();


    /** encoding **/
    auto start_encoding = chrono::high_resolution_clock::now();
    auto encoded = encode();
    auto time_encoding = chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - start_encoding).count();

    auto start_writing = chrono::high_resolution_clock::now();
    write_to_file(encoded, OUTPUT_FILE);
    auto end_writing = chrono::high_resolution_clock::now();
    auto time_writing = chrono::duration_cast<chrono::microseconds>(end_writing - start_writing).count();

    // check file and print result in green if correct, red otherwise.
    if (check_file(OUTPUT_FILE, seq, tree)) cout << "\033[1;32m> File is correct!\033[0m" << endl;
    else cout << "\033[1;31mWrong!\033[0m" << endl;

    //sum freqs, tree_codes, encoding
    auto total_elapsed_no_rw = time_freqs + time_tree_codes + time_encoding;
    auto total_elapsed_rw = total_elapsed_no_rw + time_writing + time_read;

    // write benchmark file with csv format n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_writing, total_elapsed_no_rw, total_elapsed_rw
    ofstream benchmark_file;
    benchmark_file.open(BENCHMARK_FILE, ios::out | ios::app);
    auto bench_string = to_string(n_mappers) + "," + to_string(n_reducers) + "," + to_string(n_encoders) + "," + to_string(time_freqs) + "," + to_string(time_tree_codes) + "," + to_string(time_encoding) + "," + to_string(time_read) + "," + to_string(time_writing) + "," + to_string(total_elapsed_no_rw) + "," + to_string(total_elapsed_rw) + "," + "fastflow" + "\n";
    benchmark_file << bench_string;
    benchmark_file.close();
}


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
}

unique_ptr<vector<vector<bool>>> HuffmanFastFlow::encode() {
    // create
    auto buffer = make_unique<vector<vector<bool>>>(seq.length());
    auto body = [&](const long i){
        auto code = codes->at(seq[i]);
        buffer->at(i) = code;
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
    cout << "> Generating frequency map (parallel FF)... ";
    auto start = chrono::high_resolution_clock::now();
    auto freqs = generate_frequency();
    auto end_freqs = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end_freqs - start).count();
    cout << "(took " << elapsed << "ms)" << endl;

    this -> tree = generate_huffman_tree(freqs);
    this -> codes = generate_huffman_codes(tree);

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


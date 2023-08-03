//
// Created by Luca Miglior on 24/07/23.
//
#include <fstream>
#include "HuffmanSequential.h"
#include "../utils/huffman-commons.h"

using namespace std;

HuffmanSequential::HuffmanSequential(const string &filename) {

    this->filename = filename;
    this->seq = read_file(filename);
    this->input = vector<char>(seq.begin(), seq.end());

    this->codes = unordered_map<char, vector<bool>*>();
    this->encoded_seq = nullptr;
}

HuffmanSequential::~HuffmanSequential() {
    free_tree(tree);
    free_codes(codes);
}

unordered_map<char, unsigned int> HuffmanSequential::generate_frequency() {
    this->freq_map = unordered_map<char, unsigned>();
    for (auto &c: seq) (freq_map)[c]++;
    return freq_map;
}

vector<vector<bool>*> HuffmanSequential::encode() {
    auto encoded = vector<vector<bool>*>();
    for (char i: seq) {
        encoded.push_back(codes[i]);
    }
    return encoded;
}

void HuffmanSequential::run() {

/** frequency map generation **/
    auto read_start = chrono::system_clock::now();
    this -> seq = read_file(this->filename);
    auto time_read = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - read_start).count();

    auto start = chrono::system_clock::now();
    auto freqs = generate_frequency();
    auto time_freqs = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count();

    /** huffman tree generation **/
    auto start_tree_codes = chrono::system_clock::now();
    this -> tree = generate_huffman_tree(freqs);
    this -> codes = generate_huffman_codes(tree);
    auto time_tree_codes = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start_tree_codes).count();


    /** encoding **/
    auto start_encoding = chrono::system_clock::now();
    auto encoded = encode();
    auto time_encoding = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start_encoding).count();

    auto start_writing = chrono::system_clock::now();
    write_to_file(encoded, OUTPUT_FILE);
    auto end_writing = chrono::system_clock::now();
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
    auto bench_string = to_string(0) + "," + to_string(0) + "," + to_string(0) + "," + to_string(time_freqs) + "," + to_string(time_tree_codes) + "," + to_string(time_encoding) + "," + to_string(time_read) + "," + to_string(time_writing) + "," + to_string(total_elapsed_no_rw) + "," + to_string(total_elapsed_rw) + "," + "seq" + "\n";
    benchmark_file << bench_string;
    benchmark_file.close();


}

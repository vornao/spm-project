//
// Created by Luca Miglior on 24/07/23.
//

#include "HuffmanSequential.h"
#include "../utils/huffman-commons.h"

using namespace std;

HuffmanSequential::HuffmanSequential(const string &filename) {

    this->filename = filename;
    this->seq = read_file(filename);
    this->input = vector<char>(seq.begin(), seq.end());

    this->codes = nullptr;
    this->encoded_seq = nullptr;
}

HuffmanSequential::~HuffmanSequential() {
    free_tree(tree);
}

unordered_map<char, unsigned int> HuffmanSequential::generate_frequency() {
    this->freq_map = unordered_map<char, unsigned>();
    for (auto &c: seq) (freq_map)[c]++;
    return freq_map;
}

unique_ptr<vector<vector<bool>>> HuffmanSequential::encode() {
    auto encoded = make_unique<vector<vector<bool>>>();
    for (char i: seq) {
        encoded->push_back(codes->at(i));
    }
    return encoded;
}

void HuffmanSequential::run() {

    /** frequency map generation, sequential bottleneck **/
    cout << "> Generating frequency map (sequentially)... ";
    auto start = chrono::high_resolution_clock::now();
    auto freqs = generate_frequency();
    auto end_freqs = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end_freqs - start).count();
    cout << "(took " << elapsed << "ms)" << endl;

    /** huffman tree generation **/
    this->tree = generate_huffman_tree(freqs);
    this->codes = generate_huffman_codes(tree);

    // encode the sequence using the codes into a vector of bits.
    // using a vector of bool for convenience and efficiency.
    cout << "> Encoding sequence...";
    auto start_encoded = chrono::high_resolution_clock::now();

    /** sequential bottleneck */
    auto encoded = encode();
    auto end = chrono::high_resolution_clock::now();

    cout << "(took " << chrono::duration_cast<chrono::milliseconds>(end - start_encoded).count()
         << "ms)" << endl;


    write_to_file(*encoded, OUTPUT_FILE);

    cout << "> Check correctness..." << endl;
    // check file and print result in green if correct, red otherwise.
    if (check_file(OUTPUT_FILE, seq, tree)) cout << "\033[1;32m> File is correct!\033[0m" << endl;
    else cout << "\033[1;31mWrong!\033[0m" << endl;

    cout << "> Total time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
}

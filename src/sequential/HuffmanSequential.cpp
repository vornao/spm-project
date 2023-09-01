#include <fstream>
#include "HuffmanSequential.h"
#include "../utils/huffman-commons.h"
#include "../utils/utimer.cpp"

using namespace std;

HuffmanSequential::HuffmanSequential(const string &filename) {

    this->filename = filename;
    this->seq = read_file(filename);
    this->input = vector<char>(seq.begin(), seq.end());
    this->codes = unordered_map<char, vector<bool>*>();
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
    encoded.reserve(seq.size());
    for (char i: seq) {
        encoded.push_back(codes[i]);
    }
    return encoded;
}

void HuffmanSequential::run() {

 /** frequency map generation **/
    long time_read, time_freqs, time_tree_codes, time_encoding, time_writing;
    {
        utimer timer("", &time_read);
        this->seq = read_file(this->filename);
    }

    {
        utimer timer("", &time_freqs);
        this->freq_map = generate_frequency();
    }

    /** huffman tree generation **/
    {
        utimer timer("", &time_tree_codes);
        this->tree = generate_huffman_tree(this->freq_map);
        this->codes = generate_huffman_codes(this->tree);
    }

    /** encoding **/
    {
        utimer timer("", &time_encoding);
        this->encoded_seq = encode();
    }

    /** writing **/
    {
        utimer timer("", &time_writing);
        write_to_file(this->encoded_seq, OUTPUT_FILE);
    }

    // check file and print result in green if correct, red otherwise.
    #ifdef CHKFILE
        check_file(OUTPUT_FILE, seq, this->tree);
    #endif

    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, 1, 1, 1, "sequential");

}

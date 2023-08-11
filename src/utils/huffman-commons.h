#ifndef SPM_PROJECT_HUFFMAN_COMMONS_H
#define SPM_PROJECT_HUFFMAN_COMMONS_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>
#include <bitset>

#define OUTPUT_FILE "./files/output.bin"
#define BENCHMARK_FILE "./files/benchmark.csv"
#define TYPE_SEQ "seq"
#define TYPE_MAP "map"
#define TYPE_GMR "gmr"
#define TYPE_FASTFLOW_PF "ff-pf"
#define TYPE_FASTFLOW_FARM "ff-farm"

#define BENCHMARK_HEADER "time_read,time_freqs,time_tree_codes,time_encode,time_write,n_mappers,n_reducers,n_encoders\n"

using namespace std;
/** Single Huffman code. */
typedef vector<bool> code_t;

/** Chunk of encoded sequence */
typedef vector<code_t*> chunk_t;

/** Full encoded sequence. (Vector of chunks) */
typedef vector<chunk_t*> encoded_t;

/** Node of the Huffman tree. */
struct Node {
    char c;
    unsigned freq;
    Node *left, *right;

    Node(char c, unsigned freq, Node *left, Node *right) {
        this->left = left;
        this->right = right;
        this->c = c;
        this->freq = freq;
    }
};

/** Comparator for the priority queue. */
struct Compare {
    bool operator()(Node *left, Node *right) {
        return (left->freq > right->freq);
    }
};


std::string decode(const vector<bool> &encoded, const Node *root);

std::string read_file(const std::string &filename);

std::vector<bool> *read_encoded_file(const std::string &filename);

Node *generate_huffman_tree(const unordered_map<char, unsigned> &freqs);

unordered_map<char, vector<bool>*> generate_huffman_codes(Node *root);

bool check_file(const string &filename, const string &seq, const Node *root);

void free_tree(Node *root);

void print_codes(std::unordered_map<char, code_t> &codes);

void print_encoded_sequence(code_t &encoded);

// legacy version that uses a vector of codes instead of a vector of chunks
void write_to_file(std::vector<code_t*> &encoded, const std::string &filename);

void write_to_file(encoded_t &encoded, const std::string &filename);

void free_tree(Node *root);

void free_codes(unordered_map<char, code_t*> &codes);

void free_encoding(encoded_t &encoded);

void write_benchmark(const long time_read, const long time_freqs, const long time_tree_codes, const long time_encode, const long time_write, const unsigned n_mappers, const unsigned n_reducers, const unsigned n_encoders, const string &type);


#endif //SPM_PROJECT_HUFFMAN_COMMONS_H


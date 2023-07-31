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

using namespace std;

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

bool check_file(const string &filename, const string &seq, const Node *root);

void free_tree(Node *root);

void print_codes(std::unordered_map<char, std::vector<bool>> &codes);

void print_encoded_sequence(std::vector<bool> &encoded);

void write_to_file(std::vector<std::vector<bool>> &encoded, const std::string &filename);

std::string decode(const vector<bool> &encoded, const Node *root);

std::string read_file(const std::string &filename);

std::vector<bool> *read_encoded_file(const std::string &filename);

Node *generate_huffman_tree(const unordered_map<char, unsigned> &freqs);

unordered_map<char, vector<bool>> generate_huffman_codes(Node *root);


#endif //SPM_PROJECT_HUFFMAN_COMMONS_H


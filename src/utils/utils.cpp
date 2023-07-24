#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bitset>
#include "utils.h"


void print_codes(std::unordered_map<char, std::vector<bool>> &codes) {
    for (auto &it: codes) {
        std::cout << it.first << ": ";
        for (auto b: it.second) std::cout << b;
        std::cout << std::endl;
    }
}

void print_encoded_sequence(std::vector<bool> &encoded) {
    for (auto b: encoded) std::cout << b;
    std::cout << std::endl;
}


std::string read_file(const std::string &filename) {
    std::ifstream in(filename);
    if (!in.is_open()) throw std::runtime_error("Could not open file.");

    std::string seq;
    getline(in, seq);
    in.close();
    return seq;
}


void write_to_file(std::vector<bool> &encoded, const std::string &filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) throw std::runtime_error("Could not open file.");

    // convert the sequence of bits to bytes and write them to the file.

    // get the number of bits to discard from the last byte.
    auto header = 8 - encoded.size() % 8;
    if (header == 8) header = 0;

    // writing a header which contains the number of bits to discard from the last byte.
    out << char(header);

    // convert the sequence of bits to bytes and write them to the file.
    for (int i = 0; i < encoded.size(); i += 8) {
        std::bitset<8> bits;
        for (int j = 0; j < 8; j++) {
            if (i + j < encoded.size()) bits[j] = encoded[i + j];
        }
        out << char(bits.to_ulong());
    }

    out.close();
}

/**
    * Reads the encoded file and returns a vector of bits.
    * @param filename the name of the file to read.
    * @return a vector of bits. The last n bits are discarded, where n is the header.
*/
std::vector<bool> *read_encoded_file(const std::string &filename) {

    std::ifstream in(filename, std::ios::binary);
    auto encoded = new std::vector<bool>();

    if (!in.is_open()) throw std::runtime_error("Could not open file.");

    // first byte is the header, which contains the number of bits to discard from the last byte.
    char header;
    in.get(header);

    char c;
    while (in.get(c)) {
        std::bitset<8> bits(c);
        for (int i = 0; i < 8; i++) encoded->push_back(bits[i]);
    }

    // discard the last n bits, where n is the header.
    encoded->erase(encoded->end() - int(header), encoded->end());
    in.close();
    return encoded;
}

void free_tree(Node *root) {
    if (root == nullptr) return;
    free_tree(root->left);
    free_tree(root->right);
    delete root;
}



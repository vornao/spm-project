//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANMONODE_H
#define SPM_PROJECT_HUFFMANMONODE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <ff/ff.hpp>
#include "../utils/huffman-commons.h"

using namespace std;
using namespace ff;


class HuffmanMonode {
private:
    size_t n_mappers;
    size_t n_encoders;
    string filename;
    string seq;
    Node* tree;
    unordered_map<char, unsigned> freq_map;
    unordered_map<char, code_t*> codes;
    encoded_t *encoded;
    encoded_t *encode();
    unordered_map<char, unsigned> generate_frequency();

public:
    HuffmanMonode(size_t n_mappers, size_t n_encoders, string filename);
    ~HuffmanMonode();
    void run();
};



#endif 

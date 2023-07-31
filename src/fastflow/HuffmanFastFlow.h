//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANFASTFLOW_H
#define SPM_PROJECT_HUFFMANFASTFLOW_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "../utils/huffman-commons.h"

using namespace std;


class HuffmanFastFlow {
private:
    size_t n_mappers;
    size_t n_reducers;
    size_t n_encoders;
    string filename;

    string seq;

    Node* tree;
    unordered_map<char, unsigned> freq_map;
    unordered_map<char, vector<bool>> codes;

    vector<vector<bool>> encode();
    unordered_map<char, unsigned> generate_frequency();

public:
    HuffmanFastFlow(size_t n_mappers, size_t n_reducers,size_t n_encoders, string filename);
    ~HuffmanFastFlow();
    void run();

};


#endif //SPM_PROJECT_HUFFMANFASTFLOW_H

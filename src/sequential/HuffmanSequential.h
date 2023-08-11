//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANSEQUENTIAL_H
#define SPM_PROJECT_HUFFMANSEQUENTIAL_H

#include <string>
#include <chrono>
#include <memory>
#include "../utils/huffman-commons.h"

using namespace std;

class HuffmanSequential {
private:
    string filename;
    string seq;
    vector<char> input;

    unordered_map<char, unsigned> freq_map;
    unordered_map<char, vector<bool>*> codes;
    vector<vector<bool> *> encoded_seq;

    Node* tree = nullptr;
    vector<vector<bool>*> encode();
    unordered_map<char, unsigned> generate_frequency();

public:
    HuffmanSequential(const string& filename);
    ~HuffmanSequential();
    void run();

};



#endif //SPM_PROJECT_HUFFMANSEQUENTIAL_H

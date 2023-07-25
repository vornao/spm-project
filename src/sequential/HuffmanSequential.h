//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANSEQUENTIAL_H
#define SPM_PROJECT_HUFFMANSEQUENTIAL_H

#include <string>
#include "../utils/huffman-commons.h"

using namespace std;

class HuffmanSequential {
private:
    string filename;
    string seq;
    vector<char> input;

    unordered_map<char, unsigned> freq_map;
    unique_ptr<unordered_map<char, vector<bool>>> codes;
    unique_ptr<vector<bool>> encoded_seq;

    Node* tree = nullptr;
    unique_ptr<vector<vector<bool>>> encode();
    unordered_map<char, unsigned> generate_frequency();

public:
    HuffmanSequential(const string& filename);
    ~HuffmanSequential();
    void run();

};



#endif //SPM_PROJECT_HUFFMANSEQUENTIAL_H

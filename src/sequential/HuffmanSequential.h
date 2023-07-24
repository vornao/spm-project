//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANSEQUENTIAL_H
#define SPM_PROJECT_HUFFMANSEQUENTIAL_H

#include <string>
#include "../utils/utils.h"

using namespace std;

class HuffmanSequential {
private:
    size_t n_mappers;
    size_t n_reducers;
    size_t n_encoders;
    string filename;
    string seq;
    vector<char> input;

    unordered_map<char, unsigned> freq_map;
    unique_ptr<unordered_map<char, vector<bool>>> codes;
    unique_ptr<vector<bool>> encoded_seq;

    Node* tree = nullptr;
    unique_ptr<vector<bool>> encode();
    unordered_map<char, unsigned> generate_frequency();

public:
    HuffmanSequential(size_t n_mappers, size_t n_reducers,size_t n_encoders, const string& filename);
    ~HuffmanSequential();
    void run();

};



#endif //SPM_PROJECT_HUFFMANSEQUENTIAL_H

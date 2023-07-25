//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANPARALLEL_H
#define SPM_PROJECT_HUFFMANPARALLEL_H

#include <string>
#include "../utils/huffman-commons.h"

using namespace std;

class HuffmanParallel {
    private:
        size_t n_mappers;
        size_t n_reducers;
        size_t n_encoders;
        string filename;

        string seq;
        vector<char> input;

        Node* tree;
        unordered_map<char, unsigned> freq_map;
        unique_ptr<unordered_map<char, vector<bool>>> codes;
        unique_ptr<vector<bool>> encoded_seq;

        unique_ptr<vector<vector<bool>>> encode();
        unordered_map<char, unsigned> generate_frequency();

    public:
        HuffmanParallel(size_t n_mappers, size_t n_reducers,size_t n_encoders, string filename);
        ~HuffmanParallel();
        void run();

};


#endif //SPM_PROJECT_HUFFMANPARALLEL_H

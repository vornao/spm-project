//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANGMR_H
#define SPM_PROJECT_HUFFMANGMR_H

#include <string>
#include <memory>
#include "../utils/huffman-commons.h"

using namespace std;

class HuffmanGMR {
    private:
        size_t n_mappers;
        size_t n_reducers;
        size_t n_encoders;
        string filename;
        string seq;

        Node* tree{};
        unordered_map<char, unsigned> freq_map;
        unordered_map<char, vector<bool>*> codes;
        vector<bool> encoded_seq;

        vector<vector<bool>*> encode();
        unordered_map<char, unsigned> generate_frequency();

    public:
        // for the parallel reducer version
        HuffmanGMR(size_t n_mappers, size_t n_reducers, size_t n_encoders, string filename);
        ~HuffmanGMR();
        void run();

};

#endif //SPM_PROJECT_HUFFMANPARALLEL_H

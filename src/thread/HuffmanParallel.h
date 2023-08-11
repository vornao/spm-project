//
// Created by Luca Miglior on 24/07/23.
//

#ifndef SPM_PROJECT_HUFFMANPARALLEL_H
#define SPM_PROJECT_HUFFMANPARALLEL_H

#include <string>
#include <memory>
#include "../utils/huffman-commons.h"

using namespace std;

class HuffmanParallel {
    private:
        size_t n_mappers;
        size_t n_reducers;
        size_t n_encoders;
        string filename;
        string seq;

        Node* tree{};
        unordered_map<char, unsigned> freq_map;
        unordered_map<char, code_t*> codes;
        encoded_t* encoded;
        encoded_t* encode();
        unordered_map<char, unsigned> generate_frequency();
        unordered_map<char, unsigned> generate_frequency_gmr();

    public:
        // for the sequential reducer version
        HuffmanParallel(size_t n_mappers, size_t n_encoders, string filename, size_t n_reducers);
        ~HuffmanParallel();
        void run();

};

#endif //SPM_PROJECT_HUFFMANPARALLEL_H

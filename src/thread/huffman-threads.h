//
// Created by Luca Miglior on 22/07/23.
//

#ifndef SPM_PROJECT_HUFFMAN_THREADS_H
#define SPM_PROJECT_HUFFMAN_THREADS_H

//
// Created by Luca Miglior on 20/07/23.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// map/reduce version of the sequential huffman algorithm
using namespace std;

namespace huffman_thread {
    /**
     * Generates a frequency map from a string. Parallel implementation with google map/reduce.
     * @param seq
     * @return unordered_map<char, unsigned>* a pointer to the frequency map.
     */
    unordered_map<char, unsigned> generate_frequency_gmr(vector<char> input, size_t n_mappers, size_t n_reducers);

    /**
     * Encodes a string msing a map of Huffman codes, parallel version.
     * @param seq the string to encode.
     * @param codes the map of Huffman codes.
     * @return vector<bool>* a pointer to the encoded sequence.
     */
    unique_ptr<vector<bool>> encode(string &seq, unordered_map<char, vector<bool>> &codes, size_t n_encoders);

    /**
     * Runs the parallel version of the Huffman algorithm.
     * @param filename the name of the file to read.
     */
    void run(string &filename, size_t n_mappers, size_t n_reducers, size_t n_encoders);

}
#endif //SPM_PROJECT_HUFFMAN_THREADS_H

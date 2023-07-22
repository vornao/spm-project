#ifndef SPM_PROJECT_HUFFMAN_SEQUENTIAL_H
#define SPM_PROJECT_HUFFMAN_SEQUENTIAL_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <chrono>

using namespace std;

namespace huffman_sequential {
/**
 * Generates a frequency map from a string.
 * @param seq
 * @return unordered_map<char, unsigned>* a pointer to the frequency map.
 */
    unordered_map<char, unsigned> *generate_frequency_map(const string &seq);

/**
 * Encodes a string msing a map of Huffman codes.
 * @param seq the string to encode.
 * @param codes the map of Huffman codes.
 * @return vector<bool>* a pointer to the encoded sequence.
 */
    vector<bool> *encode(const string &seq, unordered_map<char, vector<bool>> &codes);


/**
 * Runs the sequential version of the Huffman algorithm.
 * @param filename the name of the file to read.
 */
    void run(const string &filename);
}
#endif //SPM_PROJECT_HUFFMAN_SEQUENTIAL_H



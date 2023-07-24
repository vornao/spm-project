//
// Created by Luca Miglior on 21/07/23.
//

#ifndef SPM_PROJECT_HUFFMAN_COMMONS_H
#define SPM_PROJECT_HUFFMAN_COMMONS_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include "utils.h"

using namespace std;
/**
 * Decodes a sequence of bits using a Huffman tree.
 * @param encoded the sequence of bits to decode.
 * @param root the root of the Huffman tree.
 * @return string the decoded sequence.
 */
string decode(const vector<bool> &encoded, const Node *root);

/**
 * Generates a Huffman tree from a frequency map.
 * @param freqs the frequency map.
 * @return Node* the root of the Huffman tree.
 */
Node* generate_huffman_tree(const unordered_map<char, unsigned> &freqs);

/**
 * Generates a map of Huffman codes from a Huffman tree.
 * @param root the root of the Huffman tree.
 * @return unordered_map<char, vector<bool>>* a pointer to the map of Huffman codes.
 */
unique_ptr<unordered_map<char, vector<bool>>> generate_huffman_codes(Node *root);
bool check_file(const string &filename, const string &seq, const Node* root);

void free_tree(Node* root);

#endif //SPM_PROJECT_HUFFMAN_COMMONS_H


#ifndef SPM_PROJECT_HUFFMAN_COMMONS_H
#define SPM_PROJECT_HUFFMAN_COMMONS_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#define OUTPUT_FILE "output.bin"

using namespace std;

/** Node of the Huffman tree. */
struct Node {
    char c;
    unsigned freq;
    Node *left, *right;

    Node(char c, unsigned freq, Node *left, Node *right) {
        this->left = left;
        this->right = right;
        this->c = c;
        this->freq = freq;
    }
};


/** Comparator for the priority queue. */
struct Compare {
    bool operator()(Node *left, Node *right) {
        return (left->freq > right->freq);
    }
};


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
Node *generate_huffman_tree(const unordered_map<char, unsigned> &freqs);

/**
 * Generates a map of Huffman codes from a Huffman tree.
 * @param root the root of the Huffman tree.
 * @return unordered_map<char, vector<bool>>* a pointer to the map of Huffman codes.
 */
unique_ptr<unordered_map<char, vector<bool>>> generate_huffman_codes(Node *root);

bool check_file(const string &filename, const string &seq, const Node *root);

void free_tree(Node *root);

/**
    * Prints the codes to the console given .
    * @param freqs the frequency map to print.
*/
void print_codes(std::unordered_map<char, std::vector<bool>> &codes);


/**
    * Prints the encoded sequence to the console, given a vector of bits.
    * @param encoded the sequence of bits to print.
*/
void print_encoded_sequence(std::vector<bool> &encoded);


/**
    * Writes the encoded sequence to a file.
    * @param encoded the sequence of bits to write.
    */
void write_to_file(std::vector<bool> &encoded, const std::string &filename);

void write_to_file(std::vector<std::vector<bool>> &encoded, const std::string &filename);


/**
    * Reads a text file and returns its content as a string.
    * @param filename the name of the file to read.
    * @return the content of the file as a string.
*/
std::string read_file(const std::string &filename);

/**
    * Reads the encoded file and returns a vector of bits.
    * @param filename the name of the file to read.
    * @return a vector of bits. The last n bits are discarded, where n is the header.
*/
std::vector<bool> *read_encoded_file(const std::string &filename);

#endif //SPM_PROJECT_HUFFMAN_COMMONS_H


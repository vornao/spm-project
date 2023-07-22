//
// Created by Luca Miglior on 22/07/23.
//

#ifndef SPM_PROJECT_UTILS_H
#define SPM_PROJECT_UTILS_H

#define OUTPUT_FILE "./files/output.bin"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

/** Node of the Huffman tree. */
struct Node{
    char c;
    unsigned freq;
    Node *left, *right;

    Node(char c, unsigned freq, Node *left, Node *right){
        this->left = left;
        this->right = right;
        this->c = c;
        this->freq = freq;
    }
};


/** Comparator for the priority queue. */
struct Compare {
    bool operator()(Node *left, Node *right){
        return (left->freq > right->freq);
    }
};


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

#endif //SPM_PROJECT_UTILS_H

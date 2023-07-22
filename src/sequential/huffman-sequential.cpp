#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include "huffman-sequential.h"
#include "../utils/huffman-commons.h"

using namespace std;

/**
 * Generates a frequency map from a string.
 * @param seq
 * @return unordered_map<char, unsigned>* a pointer to the frequency map.
 */

namespace huffman_sequential {
    unordered_map<char, unsigned> *generate_frequency_map(const string &seq) {
        auto frequencies = new unordered_map<char, unsigned>();
        for (auto &c: seq) (*frequencies)[c]++;
        return frequencies;
    }

/**
 * Encodes a string msing a map of Huffman codes.
 * @param seq the string to encode.
 * @param codes the map of Huffman codes.
 * @return vector<bool>* a pointer to the encoded sequence.
 */
    vector<bool> *encode(const string &seq, unordered_map<char, vector<bool>> &codes) {
        auto encoded = new vector<bool>();
        for (auto &c: seq) {
            auto code = codes[c];
            encoded->insert(encoded->end(), code.begin(), code.end());
        }
        return encoded;
    }


    void run(const string &filename) {

        // sequential part, not parallelized.
        string seq = read_file(filename);

        /** frequency map generation, sequential bottleneck **/
        cout << "> Generating frequency map (sequentially)... ";
        auto start = chrono::high_resolution_clock::now();
        auto freqs = generate_frequency_map(seq);
        auto end_freqs = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end_freqs - start).count();
        cout << "(took " << elapsed << "ms)" << endl;

        /** huffman tree generation **/
        auto tree = generate_huffman_tree(*freqs);
        auto codes = generate_huffman_codes(tree);


        // encode the sequence msing the codes into a vector of bits.
        // msing a vector of bool for convenience and efficiency.
        cout << "> Encoding sequence...";
        auto start_encoded = chrono::high_resolution_clock::now();

        /** sequential bottleneck */
        auto encoded = encode(seq, *codes);

        auto end_encoded = chrono::high_resolution_clock::now();
        cout << "(took " << chrono::duration_cast<chrono::milliseconds>(end_encoded - start_encoded).count()
             << "ms)" << endl;
        auto end = chrono::high_resolution_clock::now();

        write_to_file(*encoded, OUTPUT_FILE);


        cout << "> Check correctness..." << endl;
        // check file and print result in green if correct, red otherwise.
        if (check_file(OUTPUT_FILE, seq, tree)) cout << "\033[1;32m> File is correct!\033[0m" << endl;
        else cout << "\033[1;31mWrong!\033[0m" << endl;

        cout << "> Total time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    }
}


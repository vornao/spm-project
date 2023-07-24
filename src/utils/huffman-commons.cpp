//
// Created by Luca Miglior on 22/07/23.
//

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
string decode(const vector<bool> &encoded, const Node *root) {
    auto decoded = new string();
    auto node = root;
    for (auto b: encoded) {
        if (b) node = node->right;
        else node = node->left;

        if (node->c != '\0') {
            // append the character to the decoded string and reset the node to the root.
            decoded->push_back(node->c);
            node = root;
        }
    }
    return *decoded;
}

/**
 * Generates a Huffman tree from a frequency map.
 * @param freqs the frequency map.
 * @return Node* the root of the Huffman tree.
 */
Node*generate_huffman_tree(const unordered_map<char, unsigned> &freqs) {

    // instantiating a priority queue to store the nodes, ordered by frequency.
    auto q = priority_queue<Node *, vector<Node *>, Compare>();

    for (auto &it: freqs)
        q.push(new Node(it.first, it.second, nullptr, nullptr));

    while (q.size() != 1) {
        auto left = q.top();
        q.pop();
        auto right = q.top();
        q.pop();

        // intermediate nodes won't have a character, just freq as sum of the two children.
        auto top = new Node('\0', left->freq + right->freq, left, right);
        q.push(top);
    }

    // returning the root of the tree
    return q.top();
}


/**
 * Generates a map of Huffman codes from a Huffman tree.
 * @param root the root of the Huffman tree.
 * @return unordered_map<char, vector<bool>>* a pointer to the map of Huffman codes.
 */
unique_ptr<unordered_map<char, vector<bool>>> generate_huffman_codes(Node *root) {
    auto codes = make_unique<unordered_map<char, vector<bool>>>();
    auto q = queue<pair<Node *, vector<bool>>>();

    q.emplace(root, vector<bool>());

    // traversing the tree iteratively, using a queue.
    while (!q.empty()) {
        auto elem = q.front();
        auto node = elem.first;
        auto code = elem.second;
        q.pop();

        if (node->c != '\0') (*codes)[node->c] = code;

        if (node->left != nullptr) {
            auto left_code = code;
            left_code.push_back(false);
            q.emplace(node->left, left_code);
        }

        if (node->right != nullptr) {
            auto right_code = code;
            right_code.push_back(true);
            q.emplace(node->right, right_code);
        }
    }

    return codes;
}

bool check_file(const string &filename, const string &seq, const Node* root) {
    // read the file and decode it to string
    auto encoded = read_encoded_file(filename);
    return seq == decode(*encoded, root);
}

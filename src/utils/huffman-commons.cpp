//
// Created by Luca Miglior on 22/07/23.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#include "../utils/huffman-commons.h"

using namespace std;

/**
 * Decodes a sequence of bits using a Huffman tree.
 * Clearly not built for efficiency, but for simplicity.
 * @param encoded the sequence of bits to decode.
 * @param root the root of the Huffman tree.
 * @return string the decoded sequence.
 */
string decode(const vector<bool> &encoded, const Node *root)
{
    std::string decoded = "";
    auto node = root;
    for (auto b : encoded)
    {
        if (b)
            node = node->right;
        else
            node = node->left;

        if (node->c != '\0')
        {
            // append the character to the decoded string and reset the node to the root.
            decoded.push_back(node->c);
            node = root;
        }
    }
    return decoded;
}

/**
 * Generates a Huffman tree from a frequency map.
 * @param freqs the frequency map.
 * @return Node* the root of the Huffman tree.
 */
Node *generate_huffman_tree(const unordered_map<char, unsigned> &freqs)
{

    // instantiating a priority queue to store the nodes, ordered by frequency.
    auto q = priority_queue<Node *, vector<Node *>, Compare>();

    for (auto &it : freqs)
        q.push(new Node(it.first, it.second, nullptr, nullptr));

    while (q.size() != 1)
    {
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
unordered_map<char, vector<bool> *> generate_huffman_codes(Node *root)
{
    auto codes = unordered_map<char, vector<bool> *>();
    auto q = queue<pair<Node *, vector<bool>>>();

    q.emplace(root, vector<bool>());

    // traversing the tree iteratively, using a queue.
    while (!q.empty())
    {
        auto elem = q.front();
        auto node = elem.first;
        auto code = elem.second;
        q.pop();

        if (node->c != '\0')
            (codes)[node->c] = new vector<bool>(code);

        if (node->left != nullptr)
        {
            auto left_code = code;
            left_code.push_back(false);
            q.emplace(node->left, left_code);
        }

        if (node->right != nullptr)
        {
            auto right_code = code;
            right_code.push_back(true);
            q.emplace(node->right, right_code);
        }
    }

    return codes;
}

/**
 * Reads the file and returns the sequence of characters.
 * @param filename the name of the file to read.
 * @return the sequence of characters.
 */
std::string read_file(const std::string &filename)
{
    std::ifstream in(filename);
    std::string seq;

    if (!in.is_open())
        throw std::runtime_error("Could not open file: " + filename);
    getline(in, seq);
    in.close();
    return seq;
}

/**
 * Reads the encoded file and returns a vector of bits.
 * @param filename the name of the file to read.
 * @return a vector of bits. The last n bits are discarded, where n is the header.
 */
std::vector<bool> *read_encoded_file(const std::string &filename)
{

    std::ifstream in(filename, std::ios::binary);
    auto encoded = new std::vector<bool>();

    if (!in.is_open())
        throw std::runtime_error("Could not open file: " + filename);

    // first byte is the header, which contains the number of bits to discard from the last byte.
    char header;
    in.get(header);

    char c;
    while (in.get(c))
    {
        std::bitset<8> bits(c);
        for (int i = 0; i < 8; i++)
            encoded->push_back(bits[i]);
    }

    // discard the last n bits, where n is the header.
    encoded->erase(encoded->end() - int(header), encoded->end());
    in.close();
    return encoded;
}

/**
 * Checks if the decoded sequence is equal to the original sequence.
 * @param filename the name of the file to read.
 * @param seq the original sequence.
 * @param root the root of the Huffman tree.
 * @return true if the decoded sequence is equal to the original sequence, false otherwise.
 */
bool check_file(const string &filename, const string &seq, const Node *root)
{
    // read the file and decode it to string
    auto encoded = read_encoded_file(filename);
    auto decoded = decode(*encoded, root);


    if (seq == decoded)
        cout << "\033[1;32m> File is correct!\033[0m" << endl;
    else
        cout << "\033[1;31mWrong!\033[0m" << endl;

    auto val = (seq == decoded);
    delete encoded;
    return val;
}


/**
 * Writes the encoded sequence to a file, grouping bits into bytes to write an effective compression.
 * First byte is the header, which contains the number of bits to discard from the last byte, if the number of bits
 * is not a multiple of 8.
 *
 * @param encoded the encoded sequence (vector of vectors of bools).
 * @param filename the name of the file to write to.
 */
void write_to_file(std::vector<std::vector<bool> *> &encoded, const std::string &filename)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Could not open file: " + filename);

    // convert the sequence of bits to bytes and write them to the file.
    // now we have a vector of vectors of bools, so we need to iterate over the vector of vectors.

    std::vector<unsigned char> bytes;
    int bitsWritten = 0;
    int totalWritten = 0;
    unsigned char byte = 0;

    for (const auto &vec : encoded)
    {
        for (auto bit : *vec)
        {
            byte |= (bit << bitsWritten);
            bitsWritten++;
            totalWritten++; // needed for final padding and header
            if (bitsWritten == 8)
            {
                bytes.push_back(byte);
                byte = 0;
                bitsWritten = 0;
            }
        }
    }

    // Handle padding
    if (bitsWritten > 0)
        bytes.push_back(byte);
    unsigned char header = 8 - (totalWritten % 8);
    if (header == 8)
        header = 0;

    // writing a header which contains the number of bits to discard from the last byte.
    out << char(header);
    out.write(reinterpret_cast<const char *>(bytes.data()), (long)bytes.size());
    out.close();
}



// same function but working on the nested version of the encoding (the one generated from the parallel code)
void write_to_file(encoded_t &encoded, const std::string &filename)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open())
        throw std::runtime_error("Could not open file: " + filename);

    // convert the sequence of bits to bytes and write them to the file.
    // now we have a vector of vectors of bools, so we need to iterate over the vector of vectors.

    std::vector<unsigned char> bytes;
    int bitsWritten = 0;
    int totalWritten = 0;
    unsigned char byte = 0;

    for (const auto &chunk : encoded)
    {
        for (const auto &vec : *chunk)
        {
            for (auto bit : *vec)
            {
                byte |= (bit << bitsWritten);
                bitsWritten++;
                totalWritten++;
                if (bitsWritten == 8)
                {
                    bytes.push_back(byte);
                    byte = 0;
                    bitsWritten = 0;
                }
            }
        }
    }

    // Handle padding
    if (bitsWritten > 0)
        bytes.push_back(byte);
    unsigned char header = 8 - (totalWritten % 8);
    if (header == 8)
        header = 0;

    // writing a header which contains the number of bits to discard from the last byte.
    out << char(header);
    out.write(reinterpret_cast<const char *>(bytes.data()), (long)bytes.size());
    out.close();
}


/**
 * Frees the memory allocated for the Huffman tree.
 * @param root the root of the Huffman tree.
 */
void free_tree(Node *root)
{
    // free tree iteratively, using a queue.
    if (!root)
        return;
    auto q = queue<Node *>();
    q.push(root);

    while (!q.empty())
    {
        auto current = q.front();
        q.pop();

        if (current->left != nullptr)
            q.push(current->left);
        if (current->right != nullptr)
            q.push(current->right);

        delete current;
    }
}

void free_codes(unordered_map<char, vector<bool> *> &codes)
{
    for (auto &it : codes){
        delete it.second;
    }
}

void free_encoding(encoded_t &encoding)
{
    for (auto &chunk : encoding){
        delete chunk;
    }

    delete &encoding;
}

void write_benchmark(
    const long time_read, 
    const long time_freqs, 
    const long time_tree_codes, 
    const long time_encoding, 
    const long time_writing, 
    unsigned const n_mappers, 
    unsigned const n_reducers, 
    unsigned const n_encoders,
    const string &type
    )
{
    // sum freqs, tree_codes, encoding
    auto total_elapsed_no_rw = time_freqs + time_tree_codes + time_encoding;
    auto total_elapsed_rw = total_elapsed_no_rw + time_writing + time_read;

    // write benchmark file with csv format n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_writing, total_elapsed_no_rw, total_elapsed_rw
    ofstream benchmark_file;
    benchmark_file.open(BENCHMARK_FILE, ios::out | ios::app);
    auto bench_string = 
        to_string(n_mappers) + ","
        + to_string(n_reducers) + "," 
        + to_string(n_encoders) + "," 
        + to_string(time_freqs) + "," 
        + to_string(time_tree_codes) + "," 
        + to_string(time_encoding) + "," 
        + to_string(time_read) + "," 
        + to_string(time_writing) + "," 
        + to_string(total_elapsed_no_rw) + "," 
        + to_string(total_elapsed_rw) + "," + type + "\n";
    benchmark_file << bench_string;
    benchmark_file.close();
}

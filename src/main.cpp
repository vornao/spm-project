#include <iostream>
#include <string>
#include <memory>
#include "thread/HuffmanParallel.h"
#include "sequential/HuffmanSequential.h"
#include "fastflow/HuffmanMonode.h"

using namespace std;
int main(int argc, char** argv) {
    // take filename, nmappers, nreducers, nthreads from command line

    string filename = argv[1];
    auto n_mappers = stoi(argv[2]);
    auto n_reducers = stoi(argv[3]);
    auto n_threads = stoi(argv[4]);
    auto exec_type = string(argv[5]); //seq gmr ff


    cout << "---------------------------------------------------------------" << endl;
    cout << "Filename: " << filename << endl;

    // translate to if else
    if (exec_type == "seq") {
        cout << "Running Huffman Sequential..." << endl;
        HuffmanSequential huffman_sequential(filename);
        huffman_sequential.run();
    }
    else if (exec_type == "ff") {
        cout << "Running Huffman FastFlow..." << endl;
        HuffmanMonode huffman_fastflow(n_mappers, n_threads, filename);
        huffman_fastflow.run();
    }
    else if (exec_type == "map") {
        cout << "Running Huffman Map-Parallel..." << endl;
        HuffmanParallel huffman_parallel(n_mappers, n_threads, filename, n_reducers);
        huffman_parallel.run();
    } else {
        cout << "Invalid execution type" << endl;
        return 1;
    }
}

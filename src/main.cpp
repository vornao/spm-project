#include <iostream>
#include <string>
#include "thread/HuffmanParallel.h"
#include "sequential/HuffmanSequential.h"
#include "fastflow/HuffmanFastFlow.h"

using namespace std;
int main(int argc, char** argv) {
    // take filename, nmappers, nreducers, nthreads from command line
    if (argc != 5) {
        cout << "Program usage: " << " ./spm_project <filename> <nmappers> <nreducers> <nthreads>" << endl;
        return 1;
    }

    string filename = argv[1];
    auto n_mappers = stoi(argv[2]);
    auto n_reducers = stoi(argv[3]);
    auto n_threads = stoi(argv[4]);

    cout << "Filename: " << filename << endl;

    {
        cout << "Running Huffman GMR..." << endl;
        HuffmanParallel huffman_parallel(n_mappers, n_reducers, n_threads, filename);
        huffman_parallel.run();
    }

    cout << "---------------------------------------------------------------" << endl;
    {
        //cout << "Running Huffman Sequential..." << endl;
        //HuffmanSequential huffman_sequential(filename);
        //huffman_sequential.run();
    }

    {
        cout << "Running Huffman FastFlow..." << endl;
        HuffmanFastFlow huffman_fastflow(n_mappers, n_reducers, n_threads, filename);
        huffman_fastflow.run();
    }

}

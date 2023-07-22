#include <iostream>
#include <string>
#include "thread/huffman-threads.h"
#include "sequential/huffman-sequential.h"

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
    cout << "Running Huffman GMR..." << endl;
    huffman_thread::run(filename, n_mappers, n_reducers, n_threads);
    cout << "---------------------------------------------------------------" << endl;
    cout << "Running Huffman Sequential..." << endl;
    huffman_sequential::run(filename);
}

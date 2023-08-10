#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <queue>
#include <chrono>

#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>

#include "HuffmanMonode.h"
#include "../utils/huffman-commons.h"

using namespace std;
using namespace ff;

struct Task{
    int task_id;
    int n_encoders;
    string* seq;
    vector<vector<bool> *> *chunk;
    unordered_map<char, std::vector<bool> *>* codes;

    // create constructor
    Task(int task_id, string* seq, int n_encoders, vector<vector<bool> *> *chunk, unordered_map<char, std::vector<bool> *>* codes){
        this->task_id = task_id;
        this->seq = seq;
        this->codes = codes;
        this->chunk = chunk;
        this->n_encoders = n_encoders;
    }
};

class Emitter : public ff_monode_t<Task>{
private:
    int n_encoders;
    string seq;
    unordered_map<char, std::vector<bool> *> codes;
    vector<vector<vector<bool>*>*> partial_res;

public:
    Emitter(
        int n_encoders,
        unordered_map<char, std::vector<bool> *> codes, const string &seq): n_encoders(n_encoders){
        this -> seq = seq;
        this->codes = std::move(codes);
    }
    Task *svc(Task*) override{
        for (int i = 0; i < n_encoders; i++){
            Task *t = new Task(i, &seq, n_encoders, nullptr, &codes);
            ff_send_out(t);
        }
        return EOS;
    }
};

class Collector : public ff_node_t<Task>{
private:
    vector<vector<vector<bool>*>*>* partial_res;
    public:
        Task *svc(Task *t) override{
            partial_res->at(t->task_id) = t->chunk;
            return GO_ON;
        }
        explicit Collector(vector<vector<vector<bool>*>*>* partial_res){
            this->partial_res = partial_res;
        }
};

Task *Worker(Task* t, ff::ff_node* nn){
    auto size = t->seq->length();
    auto start = t->task_id * (size / t->n_encoders);
    auto stop = (t->task_id == t->n_encoders - 1) ? size : (t->task_id + 1) * (size / t->n_encoders);
    t->chunk = new vector<vector<bool> *>();
    t->chunk->reserve(stop - start);

    for (auto i = start; i < stop; i++){
        t->chunk->push_back(t->codes->at(t->seq->at(i)));
    }
    return t;
}


HuffmanMonode::HuffmanMonode(size_t n_mappers, size_t n_encoders, string filename){
    this->n_mappers = n_mappers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    // read file
    this->seq = read_file(this->filename);
}

HuffmanMonode::~HuffmanMonode(){
    delete this->tree;
    for (auto &pair : this->codes){
        delete pair.second;
    }
}

unordered_map<char, unsigned int> HuffmanMonode::generate_frequency(){
    auto res = unordered_map<char, unsigned int>();

    auto map_f = [&](const long i, unordered_map<char, unsigned> &tempsum){
        tempsum[seq[i]]++;
    };

    auto red_f = [&](unordered_map<char, unsigned> &a, const unordered_map<char, unsigned> &b)
    {
        for (auto &it : b)
            a[it.first] += it.second;
    };

    auto pf = ParallelForReduce<unordered_map<char, unsigned>>(n_mappers);
    pf.parallel_reduce(res, unordered_map<char, unsigned>(), 0, (long)seq.size(), 1, map_f, red_f);
    return res;
}


vector<vector<vector<bool>*>*> HuffmanMonode::encode(){
    auto results = vector<vector<vector<bool>*>*>(n_encoders);
    auto emitter = Emitter(n_encoders, codes, seq);
    auto collector = Collector(&results);

    ff_Farm<Task> farm(Worker, n_encoders);
    farm.add_emitter(emitter);
    farm.add_collector(collector);
    farm.run_and_wait_end();
    return results;
}


void HuffmanMonode::run()
{
    /** frequency map generation **/
    auto read_start = chrono::system_clock::now();
    this->seq = read_file(this->filename);
    auto time_read = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - read_start).count();

    auto start = chrono::system_clock::now();
    auto freqs = generate_frequency();
    auto time_freqs = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count();

    /** huffman tree generation **/
    auto start_tree_codes = chrono::system_clock::now();
    this->tree = generate_huffman_tree(freqs);
    this->codes = generate_huffman_codes(tree);
    auto time_tree_codes = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start_tree_codes).count();

    /** encoding **/
    cout << "Encoding..." << endl;
    auto start_encoding = chrono::system_clock::now();
    auto encoded = encode();
    auto time_encoding = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start_encoding).count();
    cout << "End Encoding..." << endl;

    auto start_writing = chrono::system_clock::now();
    //write_to_file(encoded, OUTPUT_FILE);
    auto end_writing = chrono::system_clock::now();
    auto time_writing = chrono::duration_cast<chrono::microseconds>(end_writing - start_writing).count();

    // check file and print result in green if correct, red otherwise.
    //if (check_file(OUTPUT_FILE, seq, tree))
    //    cout << "\033[1;32m> File is correct!\033[0m" << endl;
    //else
     //   cout << "\033[1;31mWrong!\033[0m" << endl;

    // sum freqs, tree_codes, encoding
    auto total_elapsed_no_rw = time_freqs + time_tree_codes + time_encoding;
    auto total_elapsed_rw = total_elapsed_no_rw + time_writing + time_read;

    // write benchmark file with csv format n_mappers, n_reducers, n_encoders, time_freqs, time_tree_codes, time_encoding, time_writing, total_elapsed_no_rw, total_elapsed_rw
    ofstream benchmark_file;
    benchmark_file.open(BENCHMARK_FILE, ios::out | ios::app);
    auto bench_string = to_string(n_mappers) + "," + to_string(0) + "," + to_string(n_encoders) + "," + to_string(time_freqs) + "," + to_string(time_tree_codes) + "," + to_string(time_encoding) + "," + to_string(time_read) + "," + to_string(time_writing) + "," + to_string(total_elapsed_no_rw) + "," + to_string(total_elapsed_rw) + "," + "fastflow" + "\n";
    benchmark_file << bench_string;
    benchmark_file.close();
}

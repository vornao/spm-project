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
#include "../utils/utimer.cpp"

using namespace std;
using namespace ff;

struct Task{
    int task_id;
    int n_encoders;
    string* seq;
    chunk_t* chunk;
    unordered_map<char, code_t* >* codes;

    // create constructor
    Task(int task_id, string* seq, int n_encoders, chunk_t *chunk, unordered_map<char, code_t* >* codes){
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
    unordered_map<char, code_t*> codes;
    encoded_t partial_res;

public:
    Emitter(
        int n_encoders,
        unordered_map<char, code_t* > codes, const string &seq): n_encoders(n_encoders){
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
    encoded_t* partial_res;
    public:
        Task* svc(Task* t) override{
            partial_res->at(t->task_id) = t->chunk;
            delete t;
            return GO_ON;
        }
        explicit Collector(encoded_t* partial_res){
            // get pointer to result vector
            this->partial_res = partial_res;
        }
};

Task* Worker(Task* t, ff::ff_node* nn){
    auto n_encoders = t->n_encoders;
    auto tid   = t->task_id;
    auto size  = t->seq->length();
    auto start = tid * (size /n_encoders);
    auto stop  = (tid == n_encoders - 1) ? size : (tid+ 1) * (size / n_encoders);
    
    // allocating memory for chunk -> make memory allocation parallel time.
    t->chunk = new chunk_t();
    t->chunk->reserve(stop - start);

    for (auto i = start; i < stop; i++){
        auto seq = t->seq->at(i);
        t->chunk->push_back(t->codes->at(seq));
    }
    return t;
}


HuffmanMonode::HuffmanMonode(size_t n_mappers, size_t n_encoders, string filename){
    this->n_mappers = n_mappers;
    this->n_encoders = n_encoders;
    this->filename = std::move(filename);
    this->seq = read_file(this->filename);
    this->tree = nullptr;
}

HuffmanMonode::~HuffmanMonode(){
    free_tree(this->tree);
    free_encoding(*this->encoded);
    free_codes(this->codes);
    
}

unordered_map<char, unsigned int> HuffmanMonode::generate_frequency(){
    auto res = unordered_map<char, unsigned int>();

    auto map_f = [&](const long i, unordered_map<char, unsigned> &tempsum){
        tempsum[seq[i]]++;
    };

    auto red_f = [&](unordered_map<char, unsigned> &a, const unordered_map<char, unsigned> &b){
        for (auto &it : b)
            a[it.first] += it.second;
    };

    auto pf = ParallelForReduce<unordered_map<char, unsigned>>((long)n_mappers);
    pf.parallel_reduce(res, unordered_map<char, unsigned>(), 0, (long)seq.size(), 1, map_f, red_f, n_mappers);
    return res;
}


encoded_t* HuffmanMonode::encode(){
    auto results = new encoded_t(n_encoders);
    auto emitter = Emitter((int)n_encoders, codes, seq);
    auto collector = Collector(results);

    // create FF farm with n_encoders workers
    ff_Farm<Task> farm(Worker, (long)n_encoders);
    farm.add_emitter(emitter);
    farm.add_collector(collector);
    farm.run_and_wait_end();

    return results;
}


void HuffmanMonode::run()
{
    /** frequency map generation **/
    long time_read;
    {
        utimer timer("Reading file", &time_read);
        this -> seq = read_file(this->filename);
    }


    long time_freqs;
    unordered_map<char, unsigned int> freqs;
    {
        utimer timer("Frequency map generation", &time_freqs);
        freqs = generate_frequency();
    }


    /** huffman tree generation **/
    long time_tree_codes;
    {
        utimer timer("Huffman tree generation", &time_tree_codes);
        this->tree = generate_huffman_tree(freqs);
        this->codes = generate_huffman_codes(tree);
    }


    /** encoding **/
    long time_encoding;
    {
        utimer timer("Encoding", &time_encoding);
        this->encoded = encode();
    }

    /** writing **/
    long time_writing;
    {
        utimer timer("Writing", &time_writing);
        write_to_file(*encoded, OUTPUT_FILE);
    }

    //check file and print result in green if correct, red otherwise.
    #ifdef CHKFILE
        check_file(OUTPUT_FILE, seq, this->tree);
    #endif

    write_benchmark(time_read, time_freqs, time_tree_codes, time_encoding, time_writing, n_mappers, 0, n_encoders, TYPE_FASTFLOW_FARM);
}

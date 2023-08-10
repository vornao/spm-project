#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include "utimer.hpp"

#include <ff/ff.hpp>		// change 1: include fastflow library code

bool pf = false; 

using namespace std; 

typedef struct __task {
  int start; int stop; 
} TASK; 

vector<double> v,r;                 // input and result vectors
					  
double f(double x) { return x*x; }   // function to be mapped on v
inline double g(double x) { 
  for(int i=0; i<1000; i++) 
    x = sin(x); 
  return(x); 
} 


class emitter : public ff::ff_monode_t<TASK> {
private: 
	int nw; 
public:
	emitter(int nw):nw(nw) {}

	TASK * svc(TASK *) {
	   for(int i=0; i<nw; i++) {
             auto n = v.size();
             int delta = n/nw;
             int from  = i*delta;                    // compute assigned iteration
             int to    = (i == (nw-1) ? n : (i+1)*delta);  // (poor load balancing)
	     auto t = new TASK(from,to);
	     ff_send_out(t);
	   }
	   return(EOS);
	}
};

class collector : public ff::ff_node_t<TASK>
{
private:
    TASK *tt;

public:
    TASK *svc(TASK *t)
    {
       free(t);
       return (GO_ON);
    }
};

TASK *  worker(TASK * t, ff::ff_node* nn) {
    auto start = t->start; 
    auto stop = t->stop; 
    for(int i=start; i<stop; i++) {         // then apply function to the
      r[i] = g(v[i]);
    }
    return t;
  }
	
int main(int argc, char * argv[]) {

  // parameter parsing from command line : mapvec n seed nw
  if(argc == 2 && strcmp(argv[1],"-help")==0) {
    cout << "Usage is: " << argv[0] << " n seed printflag" << endl; 
    return(0);
  }

  int n = (argc > 1 ? atoi(argv[1]) : 10);  // len of the vector(s)
  int s = (argc > 2 ? atoi(argv[2]) : 123); // seed 
  int nw = (argc > 3 ? atoi(argv[3]) : 4);   // par degree
  pf=(argc > 4 ? (argv[4][0]=='t' ? true : false) : true);  
  
  v.resize(n);
  r.resize(n);

  const int max = 8; 
  srand(s); 
  for(int i=0; i<n; i++)                    // random init: same seed to 
    v[i] = (double) (rand() % max);         // run different par degree 

  // cout << "working with " << nw << " workers " << endl; 

  long usecs; 
  {
    utimer t0("parallel computation",&usecs); 
    auto e = emitter(nw);
    auto c = collector(); 
    // cout << "---> " << workers.size() << endl; 
    ff::ff_Farm<TASK> mf(worker, nw); 
    mf.add_emitter(e);
    mf.add_collector(c);

    mf.run_and_wait_end();
  } 

  if(pf)                                    // print results (if needed)
    for(int i=0; i<n; i++) cout <<i<<": "<<v[i]<<" -> "<<r[i]<< endl;

  cout << "End (spent " << usecs << " usecs with "<< nw << " threads)"  << endl;
  return(0); 
}
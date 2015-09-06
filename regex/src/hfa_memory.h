#include "stdinc.h"
#include "dfa.h"
#include "nfa.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <sstream>
#include <string>
#include "hybrid_fa.h"

#define MIN_BLOCK_SIZE 1

class hfa_memory
{
    int threshold ; //threshold for the head dfa

    DFA *head_dfa;  //head dfa

    map <state_t, nfa_set*> *border; //border

    long int head_dfa_memory;

    long int tail_dfa_memory;

    int no_hstates;

    int no_tstates;

    public :

    hfa_memory(HybridFA *hfa_in, int threshold_in);

    long int return_head_dfa_memory();

    long int return_tail_dfa_memory();

    int return_no_hstates();

    int return_no_tstates();

};


   
    inline long int hfa_memory::return_head_dfa_memory(){return head_dfa_memory;}

    inline long int hfa_memory::return_tail_dfa_memory(){return tail_dfa_memory;}

    inline int hfa_memory::return_no_hstates(){return no_hstates;}

    inline int hfa_memory::return_no_tstates(){return no_tstates;}



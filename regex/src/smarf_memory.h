#include "stdinc.h"
#include "dfa.h"
#include "nfa.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <sstream>
#include <string>
#define NO_ADDRESS 0xFFFFFFFF
#define MIN_BLOCK_SIZE 1                   //min. memory block size in terms of byte


class smarf_memory
{
    int threshold;        //threshold

    DFA *dfa;              // DFA

    long int *state_index;     //state index
 
    long int *SAS_index;       //SAS_state table index

    long int dfa_table_memory;        //total dfa table memory

    long int link_table_memory;       //total link table memory

    int no_states;                   //no of states

    int no_SAS_states;              // no of SAS states

    long int char_table_memory;
    
    public :

    smarf_memory(DFA *dfa_in , int threshold_in);  

    long int *return_state_index();


    long int *return_SAS_index();


    long int return_dfa_table_memory();


    long int return_link_table_memory();


    int return_no_states();


    int return_no_SAS_states();

    long int return_char_table_memory();
    

};


    inline long int *smarf_memory::return_state_index(){return state_index;}


    inline long int *smarf_memory::return_SAS_index(){return SAS_index;}


    inline long int smarf_memory::return_dfa_table_memory(){return dfa_table_memory;}


    inline long int smarf_memory::return_link_table_memory(){return link_table_memory;}


    inline int smarf_memory::return_no_states(){return no_states;}


    inline int smarf_memory::return_no_SAS_states(){return no_SAS_states;} 
    inline long int smarf_memory::return_char_table_memory(){return char_table_memory;}


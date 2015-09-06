#include "hfa_memory.h"

hfa_memory::hfa_memory(HybridFA *hfa_in, int threshold_in)
{
    head_dfa = hfa_in->get_head();

    border = hfa_in->get_border();

    nfa_set *border_state = new nfa_set();

    set<DFA *> *dfa_border = new set<DFA *>();

    set<DFA *>::iterator itd;

    DFA *dfat = NULL;

    no_tstates = 0;

    for(border_it it=border->begin();it!=border->end();it++){
        FOREACH_SET(it->second,it2)
        {
            if(!SET_MBR(border_state,(*it2)))
                border_state->insert((*it2));
        }
    }
    FOREACH_SET(border_state,it) 
    {
        dfat = new DFA();
        dfat = (*it)->nfa2dfa();
        dfat->minimize();
        no_tstates= no_tstates + dfat->size();
        dfat->fast_compression_algorithm(1,-1);
        dfa_border->insert(dfat);
        //delete dfat;
    } 

    no_hstates = head_dfa->size();

    head_dfa->fast_compression_algorithm(1,-1);

    int hstate_i_size = (int) ceil(log(no_hstates)/(MIN_BLOCK_SIZE*8*log(2)));

    int tstate_i_size = (int) ceil(log(no_tstates)/(MIN_BLOCK_SIZE*8*log(2)));

    state_t s = 0;
    head_dfa_memory = 0;
    //head_dfa->fast_compression_algorithm(1,1);

    for(s=0;s<head_dfa->size();s++)
    {

        state_t siz = head_dfa->get_lable_size(s);
        if(siz > threshold)
        {
            head_dfa_memory += hstate_i_size + (256/(MIN_BLOCK_SIZE*8)) + 256*hstate_i_size + hstate_i_size ; 
        }
        else
        {
            head_dfa_memory += hstate_i_size + (256/(MIN_BLOCK_SIZE*8)) + hstate_i_size*siz + hstate_i_size;
        }
        border_it ittemp = border->find(s);
        if(ittemp != border->end())
        {
            head_dfa_memory += tstate_i_size*(ittemp->second->size());
            //cout<<"Border match :"<<" "<<ittemp->second->size()<<endl;
        }
        if(!head_dfa->accepts(s)->empty())
            head_dfa_memory += head_dfa->accepts(s)->size() + 1;


    }

    tail_dfa_memory = 0;
    for(itd = dfa_border->begin(); itd != dfa_border->end(); itd++)
    {
        //(*itd)->fast_compression_algorithm(1,1);

        for(s = 0 ; s <(*itd)->size(); s++)
        {
            state_t sizt = (*itd)->get_lable_size(s);
            if(sizt > threshold)
            {
                tail_dfa_memory += tstate_i_size + (256/(MIN_BLOCK_SIZE*8)) + 256*tstate_i_size + tstate_i_size ; 
            }
            else
            {
                tail_dfa_memory += tstate_i_size + (256/(MIN_BLOCK_SIZE*8)) + sizt*tstate_i_size + tstate_i_size ; 
            }
            if(!(*itd)->accepts(s)->empty())
            {
                tail_dfa_memory += (*itd)->accepts(s)->size() + 1;
            }
        

        }
    }
}

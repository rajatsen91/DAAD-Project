/*
 * Copyright (c) 2007 Michela Becchi and Washington University in St. Louis.
 * All rights reserved
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The name of the author or Washington University may not be used
 *       to endorse or promote products derived from this source code
 *       without specific prior written permission.
 *    4. Conditions of any other entities that contributed to this are also
 *       met. If a copyright notice is present from another entity, it must
 *       be maintained in redistributions of the source code.
 *
 * THIS INTELLECTUAL PROPERTY (WHICH MAY INCLUDE BUT IS NOT LIMITED TO SOFTWARE,
 * FIRMWARE, VHDL, etc) IS PROVIDED BY  THE AUTHOR AND WASHINGTON UNIVERSITY
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR WASHINGTON UNIVERSITY
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS INTELLECTUAL PROPERTY, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * */

/*
 * File:   nfa.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 */

#include "nfa.h"

#include <iterator>

#include "subset.h"

#define LIMIT 254
#define UNDEFINED 0xFFFFFFFF

bool NFA::ignore_case = 0;

int NFA::num_rules = 0;

/* constructor */
NFA::NFA(){
	id=nstate_nr++;
	if (nstate_nr==NO_STATE) fatal("NFA:: Max number of states reached");
	transitions=new pair_set();
	epsilon=new nfa_list();
	accepting=new linked_set();	
	marked=0;
	visited=0;
	fully_visited=0;
	delete_only_me=0;
	depth=UNDEFINED;
	first=this;
	last=this;
	counting = false;
	count_forbidden = new int_set(CSIZE);
 	count_max=-1;
 	count_mark=0;
 	count_min=-1;
}

/* destructor */
NFA::~NFA(){
	if (!delete_only_me){
		nfa_set *queue=new nfa_set();
		traverse(queue);
		FOREACH_SET(queue,it){ 
			if (*it!=this){
				(*it)->delete_only_me=true;
				delete *it;
			}
		}
		delete queue;	
	}
	FOREACH_TRANSITION(it) delete (*it);
	delete transitions;
	if (epsilon!=NULL) delete epsilon;
	delete accepting;
}

/* Builds a queue of states which can be reached from the current one.
 * The current state is included, too
 */
void NFA::traverse(nfa_set *queue){
	nfa_list *q=new nfa_list();
	q->push_back(this);	
	queue->insert(this);
	this->marked=true;
	while(!q->empty()){
		NFA *qnfa=q->front(); q->pop_front();
		FOREACH_PAIRSET(qnfa->transitions,it){
			NFA *nfa=(*it)->second;
			if (!nfa->marked){
				nfa->marked=true;
				q->push_back(nfa);
				queue->insert(nfa);
			}		
		}
		if (qnfa->epsilon==NULL) printf("epsilon is NULL!\n");
		//if (!qnfa->epsilon->empty()) printf("epsilon is NOT empty!\n");
		FOREACH_LIST(qnfa->epsilon,it){
			NFA *nfa=*it;
			if (!nfa->marked){
				nfa->marked=true;
				q->push_back(nfa);
				queue->insert(nfa);
			}
		}
	}
	FOREACH_SET(queue,it) (*it)->marked=false;
	delete q;
}

bool NFA::check_occurence(symbol_t c)
{
	
		nfa_list *queue=new nfa_list();
		nfa_set  *nfas=new nfa_set();
		queue->push_back(this);
		nfas->insert(this);
		int flag = 0;
		while (!queue->empty()){
			
			NFA *state=queue->front(); queue->pop_front();
			pair_set *tx=state->get_transitions();
			FOREACH_PAIRSET(tx,it) 
			{
				
				if((*it)->first == c && (*it)->second != state)
				{
					
					//printf("c = %d state = %d",c,state->get_id());
					flag=1;
					break;
				}
				else
				{
					
						if (!SET_MBR(nfas,(*it)->second)){
							queue->push_back((*it)->second);
							nfas->insert((*it)->second);
						}
						
					
				}
				
			}
			if(flag == 1)
			break;
			
			
		}
		if(flag == 1)
		return true;
		else
		return false;
}



void NFA::set_state_id(state_t id1)
{
	id=id1;
}

void NFA::traverse(nfa_list *queue){
	nfa_list *q=new nfa_list();
	q->push_back(this);	
	queue->push_back(this);
	this->marked=true;
	while(!q->empty()){
		NFA *qnfa=q->front(); q->pop_front();
		FOREACH_PAIRSET(qnfa->transitions,it){
			NFA *nfa=(*it)->second;
			if (!nfa->marked){
				nfa->marked=true;
				q->push_back(nfa);
				queue->push_back(nfa);
			}		
		}
		FOREACH_LIST(qnfa->epsilon,it){
			NFA *nfa=*it;
			if (!nfa->marked){
				nfa->marked=true;
				q->push_back(nfa);
				queue->push_back(nfa);
			}
		}
	}
	FOREACH_LIST(queue,it) (*it)->marked=false;
	delete q;
}


unsigned int NFA::size(){
	nfa_list *queue=new nfa_list();
	traverse(queue);
	unsigned long size=queue->size();
	delete queue;
	return size;	
}

bool NFA::is_visited(){
    if (!visited) return false;
    if (fully_visited) return true;
	nfa_list *q=new nfa_list();
	q->push_back(this);	
	marked=true;
	while(!q->empty()){
		NFA *qnfa=q->front(); q->pop_front();
		FOREACH_PAIRSET(qnfa->transitions,it){
			NFA *nfa=(*it)->second;
			if (!nfa->marked){
				if (!nfa->visited){
					reset_marking();
					delete q;
					return false;
				}
				else if (!nfa->fully_visited){
					nfa->marked=true;
					q->push_back(nfa);
				}
			}		
		}
		FOREACH_LIST(qnfa->epsilon,it){
			NFA *nfa=*it;
			if (!nfa->marked){
				if (!nfa->visited){
					reset_marking();
					delete q;
					return false;
				}else if (!nfa->fully_visited){
					nfa->marked=true;
					q->push_back(nfa);
				}
			}
		}
	}
	reset_marking();
	delete q;
	fully_visited=true;
	return true;
}

void NFA::del_transitions()
{
	transitions->clear();
	epsilon->clear();
	
}


bool NFA::is_counting()                                                  //checks if counting and updates the counting
{
     			symbol_t c;
			NFA *nfar = new NFA();
			int_set *chars=new int_set(CSIZE);
			int_set *chars_back=new int_set(CSIZE);
			pair_set *tx=new pair_set();
			tx=this->get_transitions();
			FOREACH_PAIRSET(tx,it2) if ((*it2)->second==this) chars_back->insert((*it2)->first);
			if(chars_back->size()>MAX_TX)
			{
				chars_back->clear();
				printf("\nepic no");
				return false;
			}
			for(c=0;c<256;c++)
			{
				if(this->get_transitions(c)==NULL)
				{
					printf("\ntno to counting"); 
					return false;
				}
				if(this->get_transitions(c)->size()==1)
				break;
			}
			nfar=*(this->get_transitions(c)->begin());
			
			//tx=this->get_transitions();
	                FOREACH_PAIRSET(tx,it2) if ((*it2)->second==nfar) chars->insert((*it2)->first);
			if(chars->size()>MAX_TX && nfar!=this)
			{
				chars->clear();
				printf("\nyes to counting");
				return true;
			}
			printf("\nno to counting");
			chars->clear();
			return false;
			
}


void NFA::reset_visited(){
	nfa_set *queue=new nfa_set();
	traverse(queue);
	FOREACH_SET(queue,it) {
		(*it)->visited=false;
		(*it)->fully_visited=false;
	}
	delete queue;
}

void NFA::reset_marking(){
	if (!marked) return;
	marked=0;
	FOREACH_TRANSITION(it){
		NFA *nfa=(*it)->second;
		nfa->reset_marking();		
	}
	FOREACH_LIST(epsilon,it){
		NFA *nfa = (*it);
		nfa->reset_marking();
	}
}

bool NFA::has_transition(symbol_t c, NFA *nfa){
	FOREACH_TRANSITION(it){
		if ((*it)->first==c && (*it)->second==nfa) return true;
	}
	return false;
}

void NFA::add_transition(symbol_t c, NFA *nfa){
	if (has_transition(c,nfa)) return;
	pair<unsigned,NFA *> *tr=new pair<unsigned,NFA *>(c,nfa);
	transitions->insert(tr);
}

nfa_set *NFA::get_transitions(symbol_t c){
	if (transitions->empty()) return NULL;
	nfa_set *result=new nfa_set();
	FOREACH_TRANSITION(it){
		symbol_t ch=(*it)->first;
		NFA *nfa=(*it)->second;
		if (c==ch) result->insert(nfa);		
	}
	if(result->empty()){ 
		delete result;
		result=NULL;
	}
	return result;
}

pair_set *NFA::get_transition_pairs(symbol_t c){
	pair_set *result=new pair_set();
	FOREACH_TRANSITION(it){
		if ((*it)->first==c) result->insert(*it);		
	}
	return result; 
}
linked_set *NFA::return_accepting(){                                         //returns the accepting rules if there it is a accepting state and null otherwise
	return accepting;
}

/* exports a complete FA to DOT format - entry point */
void NFA::to_dot(FILE *file, const char *title){
	fprintf(file, "digraph \"%s\" {\n", title);
	reset_marking();
	to_dot(file);
	fprintf(file, "}\n");	
	reset_marking();
}

/* exports an FA state to DOT format - recursive function*/
void NFA::to_dot(FILE *file,bool blue){
	const char *color= (blue) ? "blue" : "black";
	marked=1;
	//print node data
	if (accepting->empty()) 
		{
			if(!this->counting)
			fprintf(file, " N%ld [shape=circle,label=\"%ld-%d\",color=\"%s\"];\n", id,id,depth,color);
			else
			fprintf(file, " N%ld [shape=circle,label=\"%ld-%d\",color=\"%s\"];\n", id,id,depth,"green");
		}
	else{ 
		fprintf(file, " N%ld [shape=doublecircle,label=\"%ld-%d/",id,id,depth);
		linked_set *ls=	accepting;
		while(ls!=NULL){
			if(ls->succ()==NULL)
				fprintf(file,"%ld",ls->value());
			else
				fprintf(file,"%ld,",ls->value());
			ls=ls->succ();
		}
		fprintf(file,"\",color=\"%s\"];\n",color);
	}	
	//print char-transitions
	linked_set *targets=new linked_set();
	linked_set *curr_target=targets;
	FOREACH_TRANSITION(it){
		targets->insert((*it)->second->get_id());
	}
	while (curr_target!=NULL && !curr_target->empty()){
		state_t target=curr_target->value();
		linked_set *chars=new linked_set();
		FOREACH_TRANSITION(it){
			if ((*it)->second->get_id()==target) chars->insert((unsigned char)(*it)->first);
		}
		char *label=(char *)malloc(100);
		char *temp=(char *)malloc(10);
		linked_set *characters=chars;
		unsigned char c=(unsigned char)characters->value();
		bool range=false;
		if ((c>='a' && c<='z') || (c>='A' && c<='Z')) 
			sprintf(label,"%c",c);
		else 
			sprintf(label,"%d",c);
		characters=characters->succ();	
		while(characters!=NULL && !characters->empty()){
			if(characters->value()==(c+1)) range=true;
			else{
			    if(range){
			    	if ((c>='a' && c<='z') || (c>='A' && c<='Z')) 
						sprintf(temp,"-%c",c);
					else 
						sprintf(temp,"-%d",c);
					label=strcat(label,temp);	
					range=false;	
			    }
				if ((characters->value()>='a' && characters->value()<='z') || (characters->value()>='A' && characters->value()<='Z')) 
					sprintf(temp,",%c",characters->value());
				else 
					sprintf(temp,",%d",characters->value());
				label=strcat(label,temp);	
			}
			c=characters->value();
			characters=characters->succ();
		}
		if(range){
			if ((c>='a' && c<='z') || (c>='A' && c<='Z')) 
				sprintf(temp,"-%c",c);
			else 
				sprintf(temp,"-%d",c);	
			label=strcat(label,temp);	
		}
		delete chars;
		curr_target=curr_target->succ();
		fprintf(file, "N%ld -> N%ld [label=\"%s\",color=\"%s\"];\n", id,target,label,color);
		free(label);
		free(temp);
	}
	delete targets;
	//epsilon transition
	FOREACH_LIST(epsilon,it)
		fprintf(file, "N%ld -> N%ld [color=\"red\"];\n", id,(*it)->id);
		
	//recursion on connected nodes
	FOREACH_TRANSITION(it){
		NFA *next=(*it)->second;
		if (!next->is_marked()) next->to_dot(file,blue);
	}		
	FOREACH_LIST(epsilon,it) if (!(*it)->is_marked()) (*it)->to_dot(file,blue);
}

/* convers a set of state pointers into the set of corresponding identifiers */ 
set<state_t> *NFA::set_NFA2ids(nfa_set *fas){
	set <state_t> *ids=new set<state_t>();
	FOREACH_SET(fas,it){
		ids->insert((*it)->id);
	}
	return ids;
}	

nfa_set *NFA::epsilon_closure(){
	nfa_set *closure = new nfa_set();
	nfa_list *queue = new nfa_list();
	queue->push_back(this);
	while (!queue->empty()){
		NFA *nfa = queue->front(); queue->pop_front();  
		closure->insert(nfa);
		FOREACH_LIST(nfa->epsilon,it){
			if (!SET_MBR(closure,*it)){
				queue->push_back(*it);
			}
		}
	}
	delete queue;
	return closure;
}
//DFA *NFA::nfa2cdfa() //creates a counting DFA from a tail counting NFA
DFA *NFA::nfa2dfa(){
	// DFA to be created
	DFA *dfa=new DFA();
	
	// contains mapping between DFA and NFA set of states
	subset *mapping=new subset(0);
	//queue of DFA states to be processed and of the set of NFA states they correspond to
	list <state_t> *queue = new list<state_t>();
	list <nfa_set*> *mapping_queue = new list<nfa_set*>();  
	//iterators used later on
	nfa_set::iterator set_it;
	//new dfa state id
	state_t target_state=NO_STATE;
	//set of nfas state corresponding to target dfa state
	nfa_set *target; //in FA form
	set <state_t> *ids=NULL; //in id form
	
	/* code begins here */
	//initialize data structure starting from INITIAL STATE
	target = this->epsilon_closure();
	ids=set_NFA2ids(target);
	mapping->lookup(ids,dfa,&target_state);                         //create a new dfa state if the mapped subset does not exist and returns the state created in target state
	delete ids;
	FOREACH_SET(target,set_it)  dfa->accepts(target_state)->add((*set_it)->accepting);
	queue->push_back(target_state);
	mapping_queue->push_back(target);
	
	// process the states in the queue and adds the not yet processed DFA states
	// to it while creating them
	while (!queue->empty()){
		//dequeue an element
		state_t state=queue->front(); queue->pop_front(); 
		nfa_set *cl_state=mapping_queue->front(); mapping_queue->pop_front();
		// each state must be processed only once
		if(!dfa->marked(state)){ 
			dfa->mark(state);
			//iterate other all characters and compute the next state for each of them
			for(symbol_t i=0;i<CSIZE;i++){
				target= new nfa_set();
				FOREACH_SET(cl_state,set_it){
					nfa_set *state_set=(*set_it)->get_transitions(i);
					if (state_set!=NULL){
						FOREACH_SET(state_set,it2){
							nfa_set *eps_cl = (*it2)->epsilon_closure();
							target->insert(eps_cl->begin(),eps_cl->end());
	                    	delete eps_cl;
						}
						delete state_set;
					}
				}
							
				//look whether the target set of state already corresponds to a state in the DFA
				//if the target set of states does not already correspond to a state in a DFA,
				//then add it
				ids=set_NFA2ids(target);
				bool found=mapping->lookup(ids,dfa,&target_state);
				delete ids;
				if (target_state==MAX_DFA_SIZE){
					delete queue;
					while (!mapping_queue->empty()){
						delete mapping_queue->front();
						mapping_queue->pop_front();
					}
					delete mapping_queue;
					delete mapping;
					delete dfa;
					delete target;
					delete cl_state;
					return NULL;
				}
				if (!found){
					queue->push_back(target_state);
					mapping_queue->push_back(target);
					FOREACH_SET(target,set_it){
						dfa->accepts(target_state)->add((*set_it)->accepting);
					}
					if (target->empty()) dfa->set_dead_state(target_state);
				}else{
					delete target;
				}
				dfa->add_transition(state,i,target_state); // add transition to the DFA
			}//end for on character i		
		}//end if state marked
		delete cl_state;
	}//end while
	//deallocate all the sets and the state_mapping data structure
	delete queue;
	delete mapping_queue;
	if (DEBUG) mapping->dump(); //dumping the NFA-DFA number of state information
	delete mapping;

	return dfa;
}

/* dumps the set of NFA - debugging function*/ 
void NFA::debug_set(nfa_set *s){
	if (s->empty()){
		printf("EMPTY");
		return;
	}
	FOREACH_SET(s,it){
		NFA *fa=*it;
		fprintf(stdout,"%ld ",fa->id);
	}	
}


DFA *NFA::nfa2cdfa()                                           //function to create a counting tail dfa from a counting tail NFA
{
	unsigned int no_states;
	DFA *dfa_init=new DFA();
	dfa_init = this->nfa2dfa();                           //to know the number of the states we have and to get the transition table for each character
	if(!this->counting)
	return dfa_init;
	no_states = dfa_init->size();                        //no of states in the initial DFA
	printf("\nno of states : %d",no_states);
	int r,c;
	unsigned int flag=0;
	state_t store;
	int flag3 = 0;
	
	set <state_t> *ids1 = NULL;
	linked_set *idtemp = new linked_set() ; //test id
	linked_set *idtemp1 = new linked_set() ;
	linked_set *idtemp2 = new linked_set() ;
	linked_set *idtemp3 = new linked_set() ;
	nfa_set *nfatmp = new nfa_set();
	nfa_set **temp ;
	nfa_set *nfar = new nfa_set();
	temp = (nfa_set **)malloc((no_states)*sizeof(nfa_set *));
	for(r=0;r<no_states;r++)
	temp[r] = new nfa_set();
	
	// DFA to be created
	DFA *dfa=new DFA(no_states);
	for(int i=0;i<4;i++)
	{
		dfa->state_table_count[i] = (state_t **)malloc((no_states+1)*sizeof(state_t *));
		for(int j=0;j<=no_states;j++)
		dfa->state_table_count[i][j] = (state_t *)malloc(CSIZE*sizeof(state_t));
	}
	dfa->correlation = (linked_set **)malloc((no_states)*sizeof(linked_set *));
	for(r=0;r<no_states;r++)
	{
		dfa->correlation[r] = new linked_set();
	}
	for(r=0;r<4;r++)                                           //the last state is the dead-state always pointing to itself
	{
		for(c=0;c<CSIZE;c++)
		dfa->state_table_count[r][no_states][c]=no_states;
	}
	// contains mapping between DFA and NFA set of states
	subset *mapping=new subset(0);
	int flag2;
	//queue of DFA states to be processed and of the set of NFA states they correspond to
	list <state_t> *queue = new list<state_t>();
	list <nfa_set*> *mapping_queue = new list<nfa_set*>();  
	//iterators used later on
	nfa_set::iterator set_it;
	//new dfa state id
	state_t target_state=NO_STATE;
	//set of nfas state corresponding to target dfa state
	nfa_set *target; //in FA form
	set <state_t> *ids=NULL; //in id form
	
	// code begins here
	//initialize data structure starting from INITIAL STATE
	target = this->epsilon_closure();
	ids=set_NFA2ids(target);
	ids1=set_NFA2ids(target);
	mapping->lookup(ids,dfa,&target_state);                         //create a new dfa state if the mapped subset does not exist and returns the state created in target state
	for(set<state_t>::iterator itr=ids1->begin();itr!=ids1->end();++itr)             //update the dfa->correlation
	{
	      dfa->correlation[target_state]->insert((*itr));					
        }
	
	delete ids;
	FOREACH_SET(target,set_it)  dfa->accepts(target_state)->add((*set_it)->accepting);
	temp[target_state]->insert(target->begin(),target->end());                                      //store the correlation as nfa_set in nfa_set **temp
	queue->push_back(target_state);
	mapping_queue->push_back(target);
	
	// process the states in the queue and adds the not yet processed DFA states
	// to it while creating them
	while (!queue->empty()){
		//dequeue an element
		state_t state=queue->front(); queue->pop_front(); 
		nfa_set *cl_state=mapping_queue->front(); mapping_queue->pop_front();
		// each state must be processed only once
		if(!dfa->marked(state)){ 
			dfa->mark(state);
			//iterate other all characters and compute the next state for each of them
			for(symbol_t i=0;i<CSIZE;i++){
				target= new nfa_set();
				FOREACH_SET(cl_state,set_it){
					nfa_set *state_set=(*set_it)->get_transitions(i);
					if (state_set!=NULL){
						FOREACH_SET(state_set,it2){
							nfa_set *eps_cl = (*it2)->epsilon_closure();
							target->insert(eps_cl->begin(),eps_cl->end());
	                    	delete eps_cl;
						}
						delete state_set;
					}
				}
							
				//look whether the target set of state already corresponds to a state in the DFA
				//if the target set of states does not already correspond to a state in a DFA,
				//then add it
				ids=set_NFA2ids(target);
				ids1=set_NFA2ids(target);
				bool found=mapping->lookup(ids,dfa,&target_state);
				
				
				delete ids;
				if (target_state==MAX_DFA_SIZE){
					delete queue;
					while (!mapping_queue->empty()){
						delete mapping_queue->front();
						mapping_queue->pop_front();
					}
					delete mapping_queue;
					delete mapping;
					delete dfa;
					delete target;
					delete cl_state;
					return NULL;
				}
				if (!found){
					for(set<state_t>::iterator itr=ids1->begin();itr!=ids1->end();++itr)             //update the dfa->correlation
					{
	     					 dfa->correlation[target_state]->insert((*itr));					
        				}
					temp[target_state]->insert(target->begin(),target->end());
					queue->push_back(target_state);
					mapping_queue->push_back(target);
					FOREACH_SET(target,set_it){
						dfa->accepts(target_state)->add((*set_it)->accepting);
					}
					if (target->empty()) dfa->set_dead_state(target_state);
				}else{
					delete target;
				}
				dfa->add_transition(state,i,target_state); // add transition to the DFA
			}//end for on character i		
		}//end if state marked
		delete cl_state;
	}//end while
	//deallocate all the sets and the state_mapping data structure
	delete queue;
	delete mapping_queue;
	if (DEBUG) mapping->dump(); //dumping the NFA-DFA number of state information
	delete mapping;
        
        if(this->count_max != -1 && this->count_min == -1)
        dfa->counting = 1;
        if(this->count_max != -1 && this->count_min != -1)
        dfa->counting = 2;
        if(this->count_max == -1 && this->count_min != -1)
        dfa->counting = 3;                                          //the three counting conditons .{n},.{n.m},.{n,} respectively
        /* we have the original state table in dfa->state_table and we have the correlation between dfa and nfa states in dfa->correlation
        *  Moreover we also have the counting NFA, the state 0 of which is the counting state(couting=true)
        *  Case 1 :  .{n}     state_tabel_count[0][][] :  for count != n
                              state_tabel_count[1][][] :  for count = n 
                              state_tabel_count[2][][] :  for count = |_
           
           Case 2 :  .{n,m}   state_tabel_count[0][][] :  for 0 in {0,n) and 1 or more in {n,m} , but if 1 then !=m
                              state_tabel_count[1][][] :  for 0 in {0,n) and 1 in {m} 
                              state_tabel_count[2][][] :  for non-zero in {0,n) and 0 in {n,m}                
                              state_tabel_count[3][][] :  for non-zero in {0,n) and non-zero in {n,m} 
                              
           Case 3 :  .{n,}    state_tabel_count[0][][] :  for 0 in {0,n)
                              state_tabel_count[1][][] :  for 0 in >=n
	                      state_tabel_count[2][][] :  for all other cases

*/	for(r=0;r<no_states;r++)
	{
		printf("\n %d :",r);
		for(c=0;c<this->size();c++)
		{
			if(dfa->correlation[r]->member(c))
			printf("%d ",c);
		}
		printf("\n");
	}
	for(r=0;r<no_states;r++)
	{
		printf("\n %d :",r);
		FOREACH_SET(temp[r],itstest)
		{
			printf("%d ",(*itstest)->get_id());
		}
		printf("\n");
		
	}

	switch(dfa->counting)
	{
		
		
		case 1 : 
		{
			//null-state is defined as -1 state id.
			for(r=0;r<dfa_init->size();r++)
			{
				
				if(dfa->correlation[r]->member(0))                        //if nfa state 0 is a member(special case)
				{
                                        for(c=0;c<CSIZE;c++)
				        {
                                        	idtemp->clear();
                                        	idtemp1->clear();
                                        	idtemp2->clear();
                                        	idtemp3->clear();
                                        	FOREACH_SET(temp[r],its)
                                        	{
                                        		if((*its)->get_id() != 0)
                                        		{
                                        			if((*its)->get_transitions(c))
                                        			{
                                        				nfar = (*its)->get_transitions(c);
                                        				FOREACH_SET(nfar,its2)
                                        				{
                                        					if(!idtemp->member((*its2)->get_id()))
                                        					{
                                        						idtemp->insert((*its2)->get_id());
                                        						idtemp1->insert((*its2)->get_id());
                                        						idtemp2->insert((*its2)->get_id());
                                        					
                                        					}
                                        				}
                                        			}
                                        		}
                                        		else
                                        		{
                                        			nfar=(*its)->get_transitions(c);
                                        			if(nfar->size()==1)
                                        			{
                                        				idtemp->insert(0);
                                        				idtemp2->insert(0);
                                        			}
                                        			if(nfar->size()==2)
                                        			{
                                        			     FOREACH_SET(nfar,its3)
                                        			     {
                                        			     	if((*its3)->get_id() != 0)
                                        			     	{store = (*its3)->get_id();break;}
                                        			     }
                                        			     idtemp->insert(0);
                                        			     idtemp1->insert(store);
                                        			     idtemp2->insert(0);
                                        			     idtemp2->insert(store);
                                        			     	
                                        		        }
                                        		}
                                        	}
                                        	flag3=0;
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		
                                        		if(idtemp->empty())
                                        		{
                                        			dfa->state_table_count[0][r][c] = no_states;
                                        			flag3=1;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp->member(i))
                                        				{
                                        					if(i==4 && c==99)
                                        					printf("\tis");
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[0][r][c] = p;
                                        				flag3=1;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp))
                                        		{
                                        			dfa->state_table_count[0][r][c] = p;
                                        			flag3=1;
                                        			break;
                                        		}
                                        	}
                                        	if(flag3==0)
                                        	{
                                        		printf("\tFatal\n");
                                        		for(int k=0;k<this->size();k++)
							{
								if(idtemp->member(k))
								printf("%d ",k);
							}
							printf("\n");
						}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp1->empty())
                                        		{
                                        			dfa->state_table_count[1][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp1->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[1][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp1))
                                        		{
                                        			dfa->state_table_count[1][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp2->empty())
                                        		{
                                        			dfa->state_table_count[2][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp2->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[2][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp2))
                                        		{
                                        			dfa->state_table_count[2][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        }
                                        					 					
				}
				else                                        //if 0 is not a member then state_table_count[1][][],state_table_count[0][][],state_table_count[1][][]
				{                                           // state_table_count[2][][] are all the same
					
					for( c=0;c<CSIZE;c++)
					{
						dfa->state_table_count[0][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[1][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[2][r][c] = dfa_init->state_table[r][c]; 	
					}
				}			      
				
				
				
			}
			break;
		}
		
		case 3 : 
		{
			//null-state is defined as -1 state id.
			for(r=0;r<dfa_init->size();r++)
			{
				
				if(dfa->correlation[r]->member(0))                        //if nfa state 0 is a member(special case)
				{
                                        for(c=0;c<CSIZE;c++)
				        {
                                        	idtemp->clear();
                                        	idtemp1->clear();
                                        	idtemp2->clear();
                                        	idtemp3->clear();
                                        	FOREACH_SET(temp[r],its)
                                        	{
                                        		if((*its)->get_id() != 0)
                                        		{
                                        			if((*its)->get_transitions(c))
                                        			{
                                        				nfar = (*its)->get_transitions(c);
                                        				FOREACH_SET(nfar,its2)
                                        				{
                                        					if(!idtemp->member((*its2)->get_id()))
                                        					{
                                        						idtemp->insert((*its2)->get_id());
                                        						idtemp1->insert((*its2)->get_id());
                                        						idtemp2->insert((*its2)->get_id());
                                        					
                                        					}
                                        				}
                                        			}
                                        		}
                                        		else
                                        		{
                                        			nfar=(*its)->get_transitions(c);
                                        			if(nfar->size()==1)
                                        			{
                                        				idtemp->insert(0);
                                        				idtemp1->insert(0);
                                        				idtemp2->insert(0);
                                        			}
                                        			if(nfar->size()==2)
                                        			{
                                        			     FOREACH_SET(nfar,its3)
                                        			     {
                                        			     	if((*its3)->get_id() != 0)
                                        			     	{store = (*its3)->get_id();break;}
                                        			     }
                                        			     idtemp->insert(store);
                                        			     idtemp1->insert(0);
                                        			     idtemp2->insert(0);
                                        			     idtemp2->insert(store);
                                        			     	
                                        		        }
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		
                                        		if(idtemp->empty())
                                        		{
                                        			dfa->state_table_count[0][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[0][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp))
                                        		{
                                        			dfa->state_table_count[0][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp1->empty())
                                        		{
                                        			dfa->state_table_count[1][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp1->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[1][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp1))
                                        		{
                                        			dfa->state_table_count[1][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp2->empty())
                                        		{
                                        			dfa->state_table_count[2][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp2->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[2][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp2))
                                        		{
                                        			dfa->state_table_count[2][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        }
                                        					 					
				}
				else                                        //if 0 is not a member then state_table_count[1][][],state_table_count[0][][],state_table_count[1][][]
				{                                           // state_table_count[2][][] are all the same
					
					for( c=0;c<CSIZE;c++)
					{
						dfa->state_table_count[0][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[1][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[2][r][c] = dfa_init->state_table[r][c]; 	
					}
				}			      
				
				
				
			}
			break;
		}
		
		
		case 2 : 
		{
			//null-state is defined as -1 state id.
			for(r=0;r<dfa_init->size();r++)
			{
				
				if(dfa->correlation[r]->member(0))                        //if nfa state 0 is a member(special case)
				{
                                        for(c=0;c<CSIZE;c++)
				        {
                                        	idtemp->clear();
                                        	idtemp1->clear();
                                        	idtemp2->clear();
                                        	idtemp3->clear();
                                        	FOREACH_SET(temp[r],its)
                                        	{
                                        		if((*its)->get_id() != 0)
                                        		{
                                        			if((*its)->get_transitions(c))
                                        			{
                                        				nfar = (*its)->get_transitions(c);
                                        				FOREACH_SET(nfar,its2)
                                        				{
                                        					if(!idtemp->member((*its2)->get_id()))
                                        					{
                                        						idtemp->insert((*its2)->get_id());
                                        						idtemp1->insert((*its2)->get_id());
                                        						idtemp2->insert((*its2)->get_id());
                                        						idtemp3->insert((*its2)->get_id());
                                        					}
                                        				}
                                        			}
                                        		}
                                        		else
                                        		{
                                        			nfar=(*its)->get_transitions(c);
                                        			if(nfar->size()==1)
                                        			{
                                        				idtemp->insert(0);
                                        				idtemp2->insert(0);
                                        				idtemp3->insert(0);
                                        			}
                                        			if(nfar->size()==2)
                                        			{
                                        			     FOREACH_SET(nfar,its3)
                                        			     {
                                        			     	if((*its3)->get_id() != 0)
                                        			     	{store = (*its3)->get_id();break;}
                                        			     }
                                        			     idtemp->insert(store);
                                        			     idtemp1->insert(store);
                                        			     idtemp2->insert(0);
                                        			     idtemp3->insert(0);
                                        			     idtemp3->insert(store);	
                                        		        }
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		
                                        		if(idtemp->empty())
                                        		{
                                        			dfa->state_table_count[0][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[0][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp))
                                        		{
                                        			dfa->state_table_count[0][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp1->empty())
                                        		{
                                        			dfa->state_table_count[1][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp1->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[1][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp1))
                                        		{
                                        			dfa->state_table_count[1][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp2->empty())
                                        		{
                                        			dfa->state_table_count[2][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp2->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[2][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp2))
                                        		{
                                        			dfa->state_table_count[2][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        	for(int p=0;p<no_states;p++)
                                        	{
                                        		if(idtemp3->empty())
                                        		{
                                        			dfa->state_table_count[3][r][c] = no_states;
                                        			break;
                                        		}
                                        		if(1)
                                        		{
                                        			flag2=0;
                                        			for(int i=0;i<this->size();i++)
                                        			{
                                        				if(idtemp3->member(i))
                                        				{
                                        					if(!dfa->correlation[p]->member(i))
                                        					{flag2=1;break;}
                                        				}
                                        			}
                                        			if(flag2==0)
                                        			{
                                        				dfa->state_table_count[3][r][c] = p;
                                        				break;
                                        			}
                                        		}
                                        		if(dfa->correlation[p]->equal(idtemp3))
                                        		{
                                        			dfa->state_table_count[3][r][c] = p;
                                        			break;
                                        		}
                                        	}
                                        }
                                        					 					
				}
				else                                        //if 0 is not a member then state_table_count[1][][],state_table_count[0][][],state_table_count[1][][]
				{                                           // state_table_count[2][][] are all the same
					
					for( c=0;c<CSIZE;c++)
					{
						dfa->state_table_count[0][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[1][r][c] = dfa_init->state_table[r][c];
						dfa->state_table_count[2][r][c] = dfa_init->state_table[r][c]; 
						dfa->state_table_count[3][r][c] = dfa_init->state_table[r][c];	
					}
				}			      
				
				
				
			}
			break;
		} 
		default : {break;}
			
        } 
	if(!this->count_forbidden->empty())
	{
		for(c=0;c<CSIZE;c++)
		{
			
			if(this->count_forbidden->mbr(c))
			{
				for(r=0;r<dfa_init->size();r++)
				{
					dfa->state_table_count[0][r][c] = no_states;	
					dfa->state_table_count[1][r][c] = no_states;
					dfa->state_table_count[2][r][c] = no_states;
					dfa->state_table_count[3][r][c] = no_states;
				}	
			}
		}
	}
	
        


            /////////////////////////////////////////////////INCOMPLETE////////////////////////////////////////////////////////////////////////////////
	dfa->dead_state_count = no_states;
	dfa->count_max = this->count_max;
	dfa->count_min = this->count_min;
	dfa->set_depth_count();
	dfa->fast_compression_algorithm_count(1,1);
	
	FILE *fp = fopen("..//debug_output//DFA_counting.txt","w");
	dfa->put_count(fp,NULL);
	fclose(fp);
	//dfa->print_table();             
	return dfa;
}
	
		


void NFA::analyze(FILE *file){
	nfa_set *queue=new nfa_set();
	traverse(queue);
	unsigned num_epsilon=0;
	dheap *heap=new dheap(256*10);
	FOREACH_SET(queue,it){
		NFA *nfa=*it;
		int size=nfa->transitions->size();
		num_epsilon+= nfa->epsilon->size();		
		if (heap->member(size)){
			heap->changekey(size,heap->key(size)+1);
		}else{
			heap->insert(size,1);
		}	
	}
	printf("\nANALYZING NFA\n====================\n");
	fprintf(file,"#-states #-outgoing_transitions\n");
	while(!heap->empty()){
		int size=heap->deletemin();
		fprintf(file,"%ld %ld\n",heap->key(size),size);
	}	
	fprintf(file,"\n");
	printf("states w/ epsilon: %d\n",num_epsilon);
	delete heap;
	delete queue;
}

void NFA::reset_depth(){
	nfa_set *queue=new nfa_set();
	traverse(queue);
	FOREACH_SET(queue,it) (*it)->depth=UNDEFINED;
	delete queue;	
}


void NFA::set_depth(unsigned d){
	if (depth==UNDEFINED || depth>d){
		depth=d;
		FOREACH_TRANSITION(it){
			((*it)->second)->set_depth(d+1);
		}
		FOREACH_LIST(epsilon,it){
			(*it)->set_depth(d+1);
		}
	}	
}

bool check_dep(unsigned *dep, state_t from,  state_t to){
	if (from==to) return false;
	state_t s=to;
	do{
		s=dep[s]; 
		if (s==from) return false;
	}while(s!=UNDEFINED);
	return true;
}

void NFA::set_increasing_depth(){
	reset_state_id();
	unsigned _size=size();
	unsigned *dependences = new unsigned[_size];
	for (unsigned s=0;s<_size;s++) dependences[s]=UNDEFINED;
	nfa_list *q=new nfa_list();
	this->depth=0;
	this->marked=true;
	q->push_back(this);
	while(!q->empty()){
		NFA *nfa=q->front();q->pop_front();
		FOREACH_PAIRSET(nfa->transitions,it){
			NFA *tx=(*it)->second;
			if (tx->depth==UNDEFINED || (tx->depth <= nfa->depth && check_dep(dependences,tx->id,nfa->id))){
				tx->depth = nfa->depth+1;
				dependences[tx->id]=nfa->id;
				if (!tx->marked) q->push_back(tx);
				tx->marked=true;
			}
		}
		FOREACH_LIST(nfa->epsilon,it){
			NFA *tx=*it;
			if (tx->depth==UNDEFINED || (tx->depth <= nfa->depth && check_dep(dependences,tx->id,nfa->id))){
				tx->depth = nfa->depth+1;
				dependences[tx->id]=nfa->id;
				if (!tx->marked) q->push_back(tx);
				tx->marked=true;
			}
		}
		nfa->marked=false;
	}
	delete q;
	delete [] dependences;
}

/* 
 * returns the first state of the FA.
 * (helper function needed because we don't update the first pointer in intermediate nodes 
 * in case of concatenation)
 */
NFA *NFA::get_first(){
	if (this->first==this)
		return this;
	else
		return this->first->get_first();	
}

/* 
 * returns the last state of the FA.
 * (helper function needed because we don't update the last pointer in intermediate nodes in case of
 * concatenation)
 */
NFA *NFA::get_last(){
	if (this->last==this)
		return this;
	else
		return this->last->get_last();	
}

/*
 * Functions used to build a NFA starting from a given
 * regular expression.
 */
  
NFA *NFA::add_epsilon(NFA *nfa){
	if (nfa==NULL) nfa = new NFA();
	nfa->first=this;
	this->last=nfa;
	epsilon->push_back(nfa);
	return nfa;
}

// adds only one transition for the given symbol 
NFA *NFA::add_transition(symbol_t c){
	NFA *newstate=new NFA();
	newstate->first=this->get_first();
	this->get_first()->last=newstate; // this is redundant, but speeds up later on
	this->last=newstate;
	this->add_transition(c,newstate);	
	if (ignore_case){
		if (c>='A' && c<='Z') this->add_transition(c+('a'-'A'),newstate); 
		if (c>='a' && c<='z') this->add_transition(c-('a'-'A'),newstate);
	}
	return newstate;
}

// adds a set of transitions (e.g.: no case)
NFA *NFA::add_transition(int_set *l){
	NFA *finalst=new NFA();
	for (int c=l->head();c!=UNDEF;c=l->suc(c)){
		this->add_transition(c,finalst);
		if (ignore_case){
			if (c>='A' && c<='Z') this->add_transition(c+('a'-'A'),finalst); 
			if (c>='a' && c<='z') this->add_transition(c-('a'-'A'),finalst);
		} 
	}	 
	finalst->first=this->get_first();
	this->get_first()->last=finalst; // this is redundant, but speeds up later on
	this->last=finalst;	
	return finalst;
}

// adds a range of transitions
NFA *NFA::add_transition(symbol_t from, symbol_t to, NFA *nfa){
	if (nfa==NULL) nfa=new NFA();
	for (int i=from;i<=to;i++){
		this->add_transition(i,nfa);
		if (ignore_case){
			if (i>='A' && i<='Z') this->add_transition(i+('a'-'A'),nfa); 
			if (i>='a' && i<='z') this->add_transition(i-('a'-'A'),nfa);
		} 
	}	 
	nfa->first=this->get_first();
	this->get_first()->last=nfa; // this is redundant, but speeds up later on
	this->last=nfa;	
	return nfa;
}

//adds a wildcard
NFA *NFA::add_any(NFA *nfa){
	return add_transition(0,CSIZE-1,nfa);
}

/* links the current machine with fa and returns the last */
NFA *NFA::link(NFA *fa){
	//simple case: return this
	if (fa==NULL) return this; //fa is null
	NFA *second=fa->get_first();
	if (!second->connected()){		//fa is not connected
		second->delete_only_me = 1;
		delete second;
		return this;
	}
	
	//modify "second" in fa nodes -> it will be replaced by "this"
	nfa_set *queue=new nfa_set();
	second->traverse(queue);
	FOREACH_SET(queue,it){
		NFA *node = *it;
		if (node->first==second) node->first=this;
		FOREACH_PAIRSET(node->transitions,it) if ((*it)->second==second) (*it)->second=this;
		bool exist=false;
		FOREACH_LIST(node->epsilon,it) if (*it==second) {exist=true;break;}
		if (exist){
			node->epsilon->remove(second);
			node->epsilon->push_back(this);
		}	
	}
	delete queue;
	
	//copy transitions
	FOREACH_PAIRSET(second->transitions,it){ 
		this->add_transition((*it)->first,(*it)->second);
	}
	FOREACH_LIST(second->epsilon,it) 
		this->epsilon->push_back(*it);	
	
	this->get_last()->last = second->get_last();
	this->last = second->get_last();
	
	second->delete_only_me=1;
	delete second;		
	
	return this->last;
}

/* 
 * makes a machine repeatable from lb (lower bound) to ub (upper bound) times. 
 * ub may be _INFINITY 
 */
NFA *NFA::make_rep(int lb,int ub){
	// upper bound is INIFINITY
	if (ub==_INFINITY){
		if(lb==0){
			this->get_first()->epsilon->push_back(this->get_last());
			this->get_last()->epsilon->push_back(this->get_first());
			this->get_last()->add_epsilon();
		}else if (lb==1){
			NFA *copy=this->make_dup();
			this->get_first()->epsilon->push_back(this->get_last());
			this->get_last()->epsilon->push_back(this->get_first());
			this->get_last()->add_epsilon();
			this->get_last()->link(copy);
		}else{
#ifdef COUNTING			
			fatal("{n,INF} counting constraint\n");
#else
			if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
			NFA *copies = this->make_mdup(lb-1);
			this->get_last()->epsilon->push_back(this->get_first());
			return copies->link(this);
#endif			
		}
	// upper bound is limited	
	}else{
		// a fixed number of repetitions
		if (lb==ub){
			if (lb==0) fatal("{0} counting constraint");
			if (lb==1) return this;
#ifdef COUNTING			
			fatal("{n} counting constraint\n");
#else
			if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
			NFA *copies = this->make_mdup(lb-1);
			return copies->link(this);
#endif					
		}else{
			//{0,1/n}	
			if (lb==0){
				if (ub==1){ //{0,1} 
					if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
					this->get_first()->epsilon->push_back(this->get_last());
					return this->get_last();					
				}else{  // {0,n}
#ifdef COUNTING			
					fatal("{0,n} counting constraint\n");
#else
					if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
					this->get_first()->epsilon->push_back(this->get_last());					
					NFA *copies = this->make_mdup(ub-1);
					return copies->link(this);
#endif					
				}
			//{1,n}	
			}else if (lb==1){
					if (ub==0) fatal("{1,0} counting constraint\n");
#ifdef COUNTING			
					fatal("{1,n} counting constraint\n");
#else
					if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
					NFA *copy = this->make_dup();
					copy->get_first()->epsilon->push_back(copy->get_last());					
					if (ub==2)
						return this->get_last()->link(copy);
					else	
						return (this->get_last()->link(copy))->link(copy->make_mdup(ub-2));
#endif				
			//{m,n}
			}else{
				if (lb>ub) fatal("{m,n} counting constraint with m>n");
#ifdef COUNTING			
					fatal("{m,n} counting constraint\n");
#else
					if (!epsilon->empty()) this->add_epsilon(); //to avoid critical situations with backward epsilon
					NFA *head = this->make_mdup(lb-1);
					NFA *tail = this->make_dup();
					tail->get_first()->epsilon->push_back(tail->get_last());
					if (ub-lb>1) tail=(tail->get_last())->link(tail->make_mdup(ub-lb-1));	
					return ((this->get_last())->link(head))->link(tail);				
#endif				
			}
		}
	}
	return this->get_last();
}

NFA *NFA::make_or(NFA *alt){
	if (alt==NULL)
       return this->get_last();
                
   	NFA *begin = new NFA();
    NFA *end = new NFA();
    //connect begin states to given FAs
    begin->epsilon->push_back(this->get_first());
    this->get_first()->first=begin;
    begin->epsilon->push_back(alt->get_first());
    alt->get_first()->first=begin;
    //connect FAs to end
    this->get_last()->epsilon->push_back(end);
    this->get_last()->last=end;
    alt->get_last()->epsilon->push_back(end);
     alt->get_last()->last=end;
     //correct fist and last of begin and end
    begin->last=end;
    end->first=begin;
    return end;
	
}

void NFA::remove_epsilon(){
	if (DEBUG) printf("remove_epsilon()-NFA size: %d\n",size());
	nfa_set *queue=new nfa_set();
	nfa_set *epsilon_states=new nfa_set();
	nfa_set *to_delete=new nfa_set();
	traverse(queue);
	FOREACH_SET(queue,it){		
		nfa_set *eps=(*it)->epsilon_closure();
		SET_DELETE(eps,*it);
		epsilon_states->insert(eps->begin(),eps->end());
		FOREACH_SET(eps,it2){
			FOREACH_PAIRSET((*it2)->transitions,it3) (*it)->add_transition((*it3)->first,(*it3)->second); //transitions
			(*it)->accepting->add((*it2)->accepting);
		}
		delete eps;
	}
	
	//compute the epsilon states which can be deleted (they are reached only upon epsilon transitions)
	FOREACH_SET(epsilon_states,it){
		NFA *eps_state=(*it);
		bool reachable=false;
		FOREACH_SET(queue,it2){
			FOREACH_PAIRSET((*it2)->transitions,it3){
				if ((*it3)->second==eps_state){
					reachable=true;
					break;
				}
			}
			if (reachable) break;
		}
		if (!reachable) to_delete->insert(eps_state);
	}
	
	//delete the epsilon transitions and the transitions to states to be deleted
	FOREACH_SET(queue,it){
		NFA *state=*it;
		if (!SET_MBR(to_delete,state)){
			state->epsilon->erase(state->epsilon->begin(),state->epsilon->end());
			FOREACH_PAIRSET(state->transitions,it2){
				if (SET_MBR(to_delete,(*it2)->second)){
					fatal("remove_epsilon:: case should not happen.");
				}
			}
		}
	}
	FOREACH_SET(to_delete,it) {(*it)->delete_only_me=1;delete (*it);}
	
	delete to_delete;
	delete queue;
	delete epsilon_states;
	
	if (DEBUG) printf("remove_epsilon()-NFA size: %d\n",size());
}

bool NFA::same_transitions(NFA *nfa1, NFA *nfa2){
	FOREACH_PAIRSET(nfa1->transitions, it){ 	 
	 	 if (!nfa2->has_transition((symbol_t)((*it)->first), (NFA*)((*it)->second))) return false;
	}
	if (nfa1->transitions->size()!=nfa2->transitions->size()) return false;
	return true;
}

void NFA::remove_duplicates(){
	nfa_set *queue=new nfa_set();
	nfa_set *to_del=new nfa_set();
	traverse(queue);
	FOREACH_SET(queue,it){
		NFA *source=*it;
		if (!SET_MBR(to_del,source)){
			FOREACH_PAIRSET(source->transitions, it2){
				NFA *target=(*it2)->second;
				if (!SET_MBR(to_del,target) && source!=target && source->accepting->equal(target->accepting) && same_transitions(source,target)){
					FOREACH_SET(queue,it3){
						FOREACH_PAIRSET((*it3)->transitions, it4) if ((*it4)->second==target) (*it4)->second=source;
						if (first==target) first=source;
						if (last==target)  last==source;
					}
					target->delete_only_me=1;
					to_del->insert(target); 
				}
			}
		}
	}
	FOREACH_SET(to_del,it) delete *it;
	delete queue;
	delete to_del; 

}

void NFA::reduce(){
	if (DEBUG) printf("reduce()-NFA size: %d\n",size());
	remove_duplicates();
	reset_state_id();
	// contains mapping between DFA and NFA set of states
	subset *mapping=new subset(this->id,this);
	//queue of NFA states to be processed and of the set of NFA states they correspond to
	nfa_list *queue = new nfa_list();
	list <nfa_set*> *mapping_queue = new list<nfa_set*>();
	NFA *nfa; 
	NFA *next_nfa;
	pair_set *this_tx= new pair_set();
	nfa_set *subset=new nfa_set();
	subset->insert(this);
	queue->push_back(this);
	mapping_queue->push_back(subset);
	
	while(!queue->empty()){
		nfa=queue->front(); queue->pop_front();
		subset=mapping_queue->front(); mapping_queue->pop_front();
#ifdef COMMON_TARGET_REDUCTION		
		//compute the common target and treat it in a special way
		nfa_set *common_target=new nfa_set();
		FOREACH_SET(subset,it){
			nfa_set *tr = (*it)->get_transitions(0);
			if (tr!=NULL){
				common_target->insert(tr->begin(), tr->end());
				delete tr;
			}
		}
		for(symbol_t c=1;c<CSIZE;c++){
			nfa_set *target=new nfa_set();
			FOREACH_SET(subset,it){
				nfa_set *tr = (*it)->get_transitions(c);
				if (tr!=NULL){
					target->insert(tr->begin(), tr->end());
					delete tr;
				}
			}
			nfa_set *ct = new nfa_set();
			FOREACH_SET(common_target,it) if (SET_MBR(target,*it)) ct->insert(*it);
			delete common_target;
			common_target=ct;
			delete target;
		}
#endif		
		//compute transitions not to common target
		for(symbol_t c=0;c<CSIZE;c++){
			nfa_set *target=new nfa_set();
		 	FOREACH_SET(subset,it){
		 		nfa_set *tr = (*it)->get_transitions(c);
		 		if (tr!=NULL){
		 			target->insert(tr->begin(), tr->end());
		 			delete tr;
		 		}
		 	}
#ifdef COMMON_TARGET_REDUCTION		 	
		 	FOREACH_SET(common_target,it) if (SET_MBR(target,*it)) SET_DELETE(target,(*it));
#endif
		 	if (!target->empty()){
		 		nfa_set *self_target=new nfa_set();
		 		FOREACH_SET(target,it) if (SET_MBR(subset,*it)) self_target->insert(*it);
		 		FOREACH_SET(subset,it) if (SET_MBR(target,*it)) SET_DELETE(target,(*it));
		 	
		 		//self_target
		 		if (self_target->empty()) delete self_target;
		 		else{
		 			set<state_t> *ids=set_NFA2ids(self_target);
					bool found=mapping->lookup(ids,&next_nfa);
					delete ids;
					if (!found){
						FOREACH_SET(self_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
						queue->push_back(next_nfa);
						mapping_queue->push_back(self_target);
					}else delete self_target;
					if (nfa==this) this_tx->insert(new pair<unsigned,NFA*>(c,next_nfa));
					else nfa->add_transition(c,next_nfa);
		 		}
		 		
#ifdef HYBRID_FA_REDUCTION
		 		nfa_set *special_target=new nfa_set();
		 		FOREACH_SET(target,it){
		 			pair_set *tx=(*it)->get_transitions();
		 			int_set *chars=new int_set(CSIZE);
		 			FOREACH_PAIRSET(tx,itx) if ((*itx)->second==(*it)) chars->insert((*itx)->first);
		 			if (chars->size()>MAX_TX) special_target->insert(*it);
		 			delete chars;
		 		}
		 		
		 		if (special_target->empty()) delete special_target;
		 		else{
		 			FOREACH_SET(special_target,it) SET_DELETE(target,*it);
		 			set<state_t> *ids=set_NFA2ids(special_target);
					bool found=mapping->lookup(ids,&next_nfa);
					delete ids;
					if (!found){
						FOREACH_SET(special_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
						queue->push_back(next_nfa);
						mapping_queue->push_back(special_target);
					}else delete special_target;
					if (nfa==this) this_tx->insert(new pair<unsigned,NFA*>(c,next_nfa));
					else nfa->add_transition(c,next_nfa);
		 		}
#endif		 		
		 		//target
		 		if (target->empty()) delete target;
		 		else{
		 			set<state_t> *ids=set_NFA2ids(target);
					bool found=mapping->lookup(ids,&next_nfa);
					delete ids;
					if (!found){
						FOREACH_SET(target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
						queue->push_back(next_nfa);
						mapping_queue->push_back(target);
					}else delete target;
					if (nfa==this) this_tx->insert(new pair<unsigned,NFA*>(c,next_nfa));
					else nfa->add_transition(c,next_nfa);
		 		}
		 	}else delete target;
		}
		
		if (nfa==this){
			pair_set *tmp = transitions;
			transitions = this_tx;
			this_tx = tmp;
		}
#ifdef COMMON_TARGET_REDUCTION		
		//process the common target
		if (!common_target->empty()){
	 		nfa_set *self_target=new nfa_set();
	 		FOREACH_SET(common_target,it) if (SET_MBR(subset,*it)) self_target->insert(*it);
	 		FOREACH_SET(subset,it) if (SET_MBR(common_target,*it)) SET_DELETE(common_target,(*it));
	 		//self_target
	 		if (self_target->empty()) delete self_target;
	 		else{
	 			set<state_t> *ids=set_NFA2ids(self_target);
				bool found=mapping->lookup(ids,&next_nfa);
				delete ids;
				if (!found){
					FOREACH_SET(self_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
					queue->push_back(next_nfa);
					mapping_queue->push_back(self_target);
				}else delete self_target;
				nfa->add_any(next_nfa);
	 		}
	 		//common_target
	 		if (common_target->empty()) delete common_target;
	 		else{
	 			set<state_t> *ids=set_NFA2ids(common_target);
				bool found=mapping->lookup(ids,&next_nfa);
				delete ids;
				if (!found){
					FOREACH_SET(common_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
					queue->push_back(next_nfa);
					mapping_queue->push_back(common_target);
				}else delete common_target;
				nfa->add_any(next_nfa);
	 		}
	 	}else delete common_target;
#endif	 	
		delete subset;
	}
	
	last=this;
	
	nfa_set *to_delete=new nfa_set();
	FOREACH_PAIRSET(this_tx,it) {
		if ((*it)->second!=this) ((*it)->second)->traverse(to_delete);
		delete (*it);
	}

	FOREACH_SET(to_delete,it) {(*it)->delete_only_me=true; delete *it;} 
	delete to_delete;
	delete this_tx;
	
	delete mapping;
	delete queue;
	delete mapping_queue;

	reset_state_id();
	if (DEBUG) printf("reduce()-NFA size: %d\n",size());
}

void NFA::transform(){
	if (DEBUG) printf("transform()-NFA size: %d\n",size());
	reset_state_id();
	// contains mapping between DFA and NFA set of states
	subset *mapping=new subset(0);
	//queue of NFA states to be processed and of the set of NFA states they correspond to
	nfa_list *queue = new nfa_list();
	list <nfa_set*> *mapping_queue = new list<nfa_set*>();
	pair_set *this_tx=new pair_set();
	NFA *new_this;
	NFA *nfa; 
	NFA *next_nfa;
	nfa_set *subset=this->epsilon_closure();
	set<state_t> *ids=set_NFA2ids(subset);
	mapping->lookup(ids,&new_this);
	delete ids; 
	queue->push_back(new_this);
	mapping_queue->push_back(subset);
	
	while(!queue->empty()){
		nfa=queue->front(); queue->pop_front();
		subset=mapping_queue->front(); mapping_queue->pop_front();
#ifdef COMMON_TARGET_REDUCTION		
		//compute the common target and treat it in a special way
		nfa_set *common_target=new nfa_set();
		FOREACH_SET(subset,it){
			nfa_set *tr = (*it)->get_transitions(0);
			if (tr!=NULL){
				FOREACH_SET(tr,it2){
					nfa_set *closure=(*it2)->epsilon_closure();
					common_target->insert(closure->begin(),closure->end());
					delete closure;
				}
				delete tr;
			}
		}
		for(symbol_t c=1;c<CSIZE;c++){
			nfa_set *target=new nfa_set();
			FOREACH_SET(subset,it){
				nfa_set *tr = (*it)->get_transitions(c);
				if (tr!=NULL){
					FOREACH_SET(tr,it2){
						nfa_set *closure=(*it2)->epsilon_closure();
						target->insert(closure->begin(),closure->end());
						delete closure;
					}
					delete tr;
				}
			}
			nfa_set *ct = new nfa_set();
			FOREACH_SET(common_target,it) if (SET_MBR(target,*it)) ct->insert(*it);
			delete common_target;
			common_target=ct;
			delete target;
		}
#endif		
		//compute transitions not to common target
		for(symbol_t c=0;c<CSIZE;c++){
			nfa_set *target=new nfa_set();
		 	FOREACH_SET(subset,it){
		 		nfa_set *tr = (*it)->get_transitions(c);
		 		if (tr!=NULL){
		 			FOREACH_SET(tr,it2){
		 				nfa_set *closure = (*it2)->epsilon_closure();
		 				target->insert(closure->begin(),closure->end());
		 				delete closure;
		 			}
		 			delete tr;
		 		}
		 	}
#ifdef COMMON_TARGET_REDUCTION		 	
		 	FOREACH_SET(common_target,it) if (SET_MBR(target,*it)) SET_DELETE(target,(*it));
#endif
		 	if (!target->empty()){
		 		nfa_set *self_target=new nfa_set();
		 		FOREACH_SET(target,it) if (SET_MBR(subset,*it)) self_target->insert(*it);
		 		FOREACH_SET(subset,it) if (SET_MBR(target,*it)) SET_DELETE(target,(*it));
		 	
		 		//self_target
		 		if (self_target->empty()) delete self_target;
		 		else{
		 			set<state_t> *ids=set_NFA2ids(self_target);
					bool found=mapping->lookup(ids,&next_nfa);
					delete ids;
					if (!found){
						FOREACH_SET(self_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
						queue->push_back(next_nfa);
						mapping_queue->push_back(self_target);
					}else delete self_target;
					if (next_nfa==new_this) nfa->add_transition(c,this);
					else nfa->add_transition(c,next_nfa);
		 		}
		 		//target
		 		if (target->empty()) delete target;
		 		else{
		 			set<state_t> *ids=set_NFA2ids(target);
					bool found=mapping->lookup(ids,&next_nfa);
					delete ids;
					if (!found){
						FOREACH_SET(target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
						queue->push_back(next_nfa);
						mapping_queue->push_back(target);
					}else delete target;
					if (next_nfa==new_this) nfa->add_transition(c,this);
					else nfa->add_transition(c,next_nfa);
		 		}
		 	}else delete target;
		}
		
#ifdef COMMON_TARGET_REDUCTION		
		nfa_set *self_target=new nfa_set();
 		FOREACH_SET(common_target,it) if (SET_MBR(subset,*it)) self_target->insert(*it);
 		FOREACH_SET(subset,it) if (SET_MBR(common_target,*it)) SET_DELETE(common_target,(*it));
 		//self_target
 		if (self_target->empty()) delete self_target;
 		else{
 			set<state_t> *ids=set_NFA2ids(self_target);
			bool found=mapping->lookup(ids,&next_nfa);
			delete ids;
			if (!found){
				FOREACH_SET(self_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
				queue->push_back(next_nfa);
				mapping_queue->push_back(self_target);
			}else delete self_target;
			if (next_nfa==new_this) nfa->add_any(this);
			else nfa->add_any(next_nfa);
 		}
 		//common_target
 		if (common_target->empty()) delete common_target;
 		else{
 			set<state_t> *ids=set_NFA2ids(common_target);
			bool found=mapping->lookup(ids,&next_nfa);
			delete ids;
			if (!found){
				FOREACH_SET(common_target,set_it)  next_nfa->accepting->add((*set_it)->accepting);
				queue->push_back(next_nfa);
				mapping_queue->push_back(common_target);
			}else delete common_target;
			if (next_nfa==new_this) nfa->add_any(this);
			else nfa->add_any(next_nfa);
 		}
#endif	 	
		delete subset;
	}
	
	//delete the states of the old NFA
	nfa_set *to_delete=new nfa_set();
	FOREACH_PAIRSET(transitions,it) {
		if ((*it)->second!=this) ((*it)->second)->traverse(to_delete);
		delete (*it);
	}
	FOREACH_LIST(epsilon,it) if (*it!=this) (*it)->traverse(to_delete);
	FOREACH_SET(to_delete,it) if (*it!=this) {(*it)->delete_only_me=true; delete *it;} 
	delete to_delete;
	
	//connet new transitions to "this" NFA 
	delete transitions;
	transitions = new_this->transitions;
	epsilon->erase(epsilon->begin(),epsilon->end());
	
	//delete new_this
	new_this->transitions=new pair_set(); //to allow deletion
	new_this->delete_only_me=true;
	delete new_this;
	
	delete mapping;
	delete queue;
	delete mapping_queue;

	reset_state_id();
	
	if (DEBUG) printf("transform()-NFA size: %d\n",size());
}

void NFA::split_states(){
	if (DEBUG) printf("split_states() - initial NFA size: %d\n",size());
	fflush(stdout);
	nfa_list *queue=new nfa_list();
	traverse(queue);
	bool exist_tx[CSIZE];
	while(!queue->empty()){
		NFA *nfa=queue->front(); queue->pop_front();
		NFA *new_nfa=new NFA();
		for (int c=0;c<CSIZE;c++) exist_tx[c]=false;
		FOREACH_PAIRSET(nfa->transitions,it){
			if (exist_tx[(*it)->first]) new_nfa->transitions->insert(*it);
			exist_tx[(*it)->first]=true;
		}
		FOREACH_PAIRSET(new_nfa->transitions,it) SET_DELETE(nfa->transitions,*it);
		if (new_nfa->transitions->empty()){
			delete new_nfa;
		}else{
			nfa->epsilon->push_back(new_nfa);
			queue->push_back(new_nfa);
		}
	}
	delete queue;
	if (DEBUG) printf("split_states() - final NFA size: %d\n",size());
	reset_state_id();
	reset_depth();
	fflush(stdout);
}

/* makes a copy of the current machine and returns it */
NFA *NFA::make_dup(){
	std::map <state_t,NFA *> *mapping = new std::map <state_t,NFA *>();
	NFA *begin=this->get_first();
	NFA *copy= new NFA();
	(*mapping)[begin->id]=copy;
	copy->accepting->add(begin->accepting);
	dup_state(begin,copy,mapping);
	begin->get_first()->reset_marking();
	delete mapping;
	copy->get_last();
	return copy;
}

/* makes a copy of the current state and recursively of its connected states */
void NFA::dup_state(NFA *nfa, NFA *copy,std::map <state_t,NFA *> *mapping){
	if (nfa->marked) return;
	nfa->marked=1;
	FOREACH_PAIRSET(nfa->transitions,it){
		NFA *next=(*it)->second;
		NFA *next_copy = (*mapping)[next->id];
		if (next_copy==NULL) {
			next_copy=new NFA();
			next_copy->accepting->add(next->accepting);
			(*mapping)[next->id]=next_copy;
			next_copy->first=copy;
		}
		copy->add_transition((*it)->first,next_copy);
		if (copy->last==copy) copy->last=next_copy;
	}
	FOREACH_LIST(nfa->epsilon,it){
		NFA *next=(*it);
		NFA *next_copy = (*mapping)[next->id];
		if (next_copy==NULL) {
			next_copy=new NFA();
			next_copy->accepting->add(next->accepting);
			(*mapping)[next->id]=next_copy;
			next_copy->first=copy;
		}
		copy->epsilon->push_back(next_copy);
		if (copy->last==copy) copy->last=next_copy;
	}
	
	// and now recursion on connected nodes
	FOREACH_PAIRSET(nfa->transitions,it){
		NFA *state=(*it)->second;
		dup_state(state,(*mapping)[state->id],mapping);
	}
	FOREACH_LIST(nfa->epsilon,it) dup_state((*it),(*mapping)[(*it)->id],mapping);
}

/* makes times concatenated copy of the current machine. Used when multiple
 * repetitions of a given subpattern are required */
NFA *NFA::make_mdup(int times){
	if(times<=0)
		fatal("FA::make_mdup: called with times<=0");	
	NFA *copy=this->make_dup();
	for (int i=0;i<times-1;i++) 
		copy=copy->get_last()->link(this->make_dup());
	return copy->get_last();
}


void NFA::reset_state_id(state_t first_id){
	nstate_nr=first_id;
	nfa_list *queue=new nfa_list();
	traverse(queue);
	while(!queue->empty()){
		NFA *nfa=queue->front(); queue->pop_front();
		nfa->id=nstate_nr++;
	}
	delete queue;
}



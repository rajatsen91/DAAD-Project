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
 * File:   main_tracegen.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 * 
 * Description: This is the main file used to generate traversal traces for a given NFA
 * 
 */

#include "stdinc.h"
#include "nfa.h"
#include "parser.h"
#include "trace.h"
#include "regex_common.h"



#ifndef CUR_VER
#define CUR_VER		"\n\tMichela  Becchi 1.4.1\n"
#endif

/* CONFIGURATION */
#define NUM_TRAFFIC 4
float p_m[NUM_TRAFFIC]={0.35,0.55,0.75,0.95}; //considered probabilities of malicious traffic 

/*
 * Functions restricted to this file
 */






/*
 *  Main - entry point
 */
int regex_tracegen(int argc, char **argv)
{
	//read configuration
	init_conf();
	while(!parse_arguments(argc,argv) || !config.regex_file || !config.trace_file_base) usage();
	print_conf();
	VERBOSE=config.verbose;
	DEBUG=config.debug; if (DEBUG) VERBOSE=1; 
		
	if (!check_file(config.regex_file,"r")) fatal("Could not open the regex-file");
	
	//parse Regex and create NFA
	FILE *re_file=fopen(config.regex_file,"r");
	regex_parser *parser=new regex_parser(config.i_mod, config.m_mod);
	NFA *nfa=parser->parse(re_file);
	fclose(re_file);
	delete parser;
	
	//reduce NFA
	nfa->remove_epsilon();
	nfa->reduce();
	
	FILE *trace_file=NULL;
	char trace_name[100];
	trace *trace_gen=new trace();
	
	//perform traversal
	for (int seed=0;seed<config.num_seeds;seed++){
		for (int i=0;i<NUM_TRAFFIC;i++){
			if (config.depth_mode){
				sprintf(trace_name,"%s_depth_s%d_p%.2f.trace",config.trace_file_base,seed,p_m[i]);
				trace_file = trace_gen->generate_trace(nfa, seed, p_m[i], true, trace_name);
				fclose(trace_file);
			}
			if (config.size_mode){
				sprintf(trace_name,"%s_size_s%d_p%.2f.trace",config.trace_file_base,seed,p_m[i]);
				trace_file = trace_gen->generate_trace(nfa, seed, p_m[i], false, trace_name);
				fclose(trace_file);
			}
		}
	}

	//de-allocates
	delete trace_gen;
	delete nfa;
				
	return 0;

}


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
 * File:   main.c
 * Author: Michela Becchi
 * Email:  mbecchi@cse.wustl.edu
 * Organization: Applied Research Laboratory
 * 
 * Description: This is the main entry file
 * 
 */

/* The modifications done are for the purpose of checking for overlaps,forbidden character occurences and then  removing the unwanted cases. The to_dot_tailnfa converts the tail nfas into DFA 
   and counts the total number of states as well. 
*/

#include "stdinc.h"
#include "nfa.h"
#include "dfa.h"
#include "hybrid_fa.h"
#include "parser.h"
#include "trace.h"
#include "string.h"
#include "stdlib.h"
#include "regex_common.h"
#include "smarf_memory.h"
#include "hfa_memory.h"
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
using namespace std;

/*
 * Program entry point.
 * Please modify the main() function to add custom code.
 * The options allow to create a DFA from a list of regular expressions.
 * If a single single DFA cannot be created because state explosion occurs, then a list of DFA
 * is generated (see MAX_DFA_SIZE in dfa.h).
 * Additionally, the DFA can be exported in proprietary format for later re-use, and be imported.
 * Moreover, export to DOT format (http://www.graphviz.org/) is possible.
 * Finally, processing a trace file is an option.
 */


#ifndef CUR_VER
#define CUR_VER		"Michela  Becchi 1.4.1"
#endif

int VERBOSE;
int DEBUG;

/*
 * Returns the current version string
 */
void version(){
    printf("version:: %s\n", CUR_VER);
}

NFA *set_of_nfa[10];
int it_nfa;

/* usage */
void usage() 
{

	if(u_option==0)
	{
		fprintf(stderr,"\n");
		fprintf(stderr, "Usage: regex [options]\n"); 
		fprintf(stderr, "             [--parse|-p <regex_file> [--m|--i] | --import|-i <in_file> ]\n");
		fprintf(stderr, "             [--export|-e  <out_file>][--graph|-g <dot_file>]\n");
		fprintf(stderr, "             [--trace|-t <trace_file>]\n");
		fprintf(stderr, "             [--hfa]\n\n");
		fprintf(stderr, "\nOptions:\n");
		fprintf(stderr, "    --help,-h       print this message\n");
		fprintf(stderr, "    --version,-r    print version number\n");				    
		fprintf(stderr, "    --verbose,-v    basic verbosity level \n");
		fprintf(stderr, "    --debug,  -d    enhanced verbosity level \n");
		fprintf(stderr, "\nOther:\n");
		fprintf(stderr, "    --parse,-p <regex_file>  process regex file\n");
		fprintf(stderr, "    --m,--i  m modifier, ignore case\n");
		fprintf(stderr, "    --import,-i <in_file>    import DFA from file\n");
		fprintf(stderr, "    --export,-e <out_file>   export DFA to file\n");    
		fprintf(stderr, "    --graph,-g <dot_file>    export DFA in DOT format into specified file\n");
		fprintf(stderr, "    --trace,-t <trace_file>  trace file to be processed\n");
		fprintf(stderr, "    --hfa                    generate the hybrid-FA\n");
		fprintf(stderr, "\n");
		exit(0);
	}
	if(u_option==1)
	{
		fprintf(stderr,"\n");
		fprintf(stderr, "Usage: regex_tracegen --parse|-p <regex_file> --trace|-t <trace_file_base> [options]\n"); 
		fprintf(stderr, "\nOptions:\n");
		fprintf(stderr, "    --help,-h       		   print this message\n");
		fprintf(stderr, "    --version,-r    		   print version number\n");				    
		fprintf(stderr, "    --verbose,-v    		   basic verbosity level \n");
		fprintf(stderr, "    --debug,  -d    		   enhanced verbosity level \n");
		fprintf(stderr, "    --m,--i         		   m modifier, ignore case\n");
		fprintf(stderr, "    --seed,-s <num_seed>      number of seeds for probabilistic traversal\n");
		fprintf(stderr, "    --mode <depth|size|both>  traversal mode - maximize depth(default)|size|do both\n");
		fprintf(stderr, "\nGenerated trace files:\n");
		fprintf(stderr, "    for each (seed,p_M): <trace_file_base>_<depth/size>_<seed>_<p_M>.trace\n");
		fprintf(stderr, "\n");
		exit(0);
	}

}

/* configuration */


/* initialize the configuration */
void init_conf(){
	config.in_file=NULL;
	config.out_file=NULL;
	config.dot_file=NULL;
	config.trace_file=NULL;
	config.i_mod=false;
	config.m_mod=false;
	config.debug=false;
	config.verbose=false;
	config.hfa=false;
	config.regex_file=NULL;
	config.trace_file_base=NULL;
	config.num_seeds=1;
	config.depth_mode=true;
	config.size_mode=false;
}

/* print the configuration */
void print_conf(){
	
	if(u_option==0)
	{
	fprintf(stderr,"\nCONFIGURATION: \n");
	if (config.regex_file) fprintf(stderr, "- RegEx file: %s\n",config.regex_file);
	if (config.in_file) fprintf(stderr, "- DFA import file: %s\n",config.in_file);
	if (config.out_file) fprintf(stderr, "- DFA export file: %s\n",config.out_file);
	if (config.dot_file) fprintf(stderr, "- DOT file: %s\n",config.dot_file);
	if (config.trace_file) fprintf(stderr,"- Trace file: %s\n",config.trace_file);
	if (config.i_mod) fprintf(stderr,"- ignore case selected\n");
	if (config.m_mod) fprintf(stderr,"- m modifier selected\n");
	if (config.verbose && !config.debug) fprintf(stderr,"- verbose mode\n");
	if (config.debug) fprintf(stderr,"- debug mode\n");
	if (config.hfa)   fprintf(stderr,"- hfa generation invoked\n");                 // print the configuration initially 
        }
        if(u_option==1)
        {
        fprintf(stderr,"\nCONFIGURATION: \n");
	if (config.regex_file) fprintf(stderr, "- Reg. Exp. file: %s\n",config.regex_file);
	if (config.trace_file_base) fprintf(stderr, "- Trace base file: %s\n",config.trace_file_base);
	if (config.verbose && !config.debug) fprintf(stderr,"- verbose mode\n");
	if (config.debug) fprintf(stderr,"- debug mode\n");
	if (config.i_mod) fprintf(stderr,"- ignore case selected\n");
	if (config.m_mod) fprintf(stderr,"- m modifier selected\n");
	fprintf(stderr,"- number of seeds: %d\n",config.num_seeds);
	if (config.depth_mode) fprintf(stderr,"- depth mode selected\n");
	if (config.size_mode) fprintf(stderr,"- size mode selected\n");
	}
}

/* parse the main call parameters */
int parse_arguments(int argc, char **argv)                //command line arguments of ./regex. Processes the arguments and sets the variables in the config structure in main.c
{
	
 if(u_option==0)	
 {
	int i=1;
    if (argc < 2) {
        usage();
		return 0;                                        //print the options available is only ./regex is executed
    }
    while(i<argc){
    	if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
    		usage();
    		return 0;
    	}else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--version") == 0){
    		version();
    		return 0;
    	}else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
    		config.verbose=1;
    	}else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0){
    		config.debug=1;
    	}else if(strcmp(argv[i], "--hfa") == 0){
    	    		config.hfa=1;	
    	}else if(strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--graph") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Dot file name missing.\n");
    			return 0;
    		}
    		config.dot_file=argv[i];
    	}else if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--import") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Import file name missing.\n");
    			return 0;
    		}
    		config.in_file=argv[i];	
    	}else if(strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--export") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Export file name missing.\n");
    			return 0;
    		}
    		config.out_file=argv[i];
    	}else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parse") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Regular expression file name missing.\n");
    			return 0;
    		}
    		config.regex_file=argv[i];
    	}else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--trace") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Trace file name missing.\n");
    			return 0;
    		}
    		config.trace_file=argv[i];		
    	}else if(strcmp(argv[i], "--m") == 0){
			config.m_mod=true;
		}else if(strcmp(argv[i], "--i") == 0){
			config.i_mod=true;	    		
    	}else{
    		fprintf(stderr,"Ignoring invalid option %s\n",argv[i]);
    	}
    	i++;
    }
	return 1;
 }
 if(u_option==1)
 {
      int i=1;
    if (argc < 2) {
        usage();
		return 0;
    }
    while(i<argc){
    	if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
    		usage();
    		return 0;
    	}else if(strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--version") == 0){
    		version();
    		return 0;
    	}else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
    		config.verbose=1;
    	}else if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0){
    		config.debug=1;
    	}else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parse") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Regular expression file name missing.\n");
    			return 0;
    		}
    		config.regex_file=argv[i];
    	}else if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--seed") == 0){
    	    		i++;
    	    		if(i==argc){
    	    			fprintf(stderr,"Number of seeds is missing.\n");
    	    			return 0;
    	    		}
    	    		config.num_seeds=atoi(argv[i]);
    	    		if (config.num_seeds==0){
    	    			printf("NO seed selected!");
    	    			return 0;
    	    		}
    	}else if(strcmp(argv[i], "--mode") == 0){
    		i++;
    	    if(i==argc){
    	    	fprintf(stderr,"Mode missing.\n");
    	    	return 0;
    	    }
    	    config.depth_mode=false;
    	    config.size_mode=false;
    	    if (strcmp(argv[i], "depth")==0) config.depth_mode=true;
    	    if (strcmp(argv[i], "size")==0) config.size_mode=true;
    	    if (strcmp(argv[i], "both")==0) {
    	    	config.depth_mode=true;
    	    	config.size_mode=true;
    	    }
    	    if (!config.depth_mode && !config.size_mode){
    	    	fprintf(stderr,"No mode selected.\n");
    	    	return 0;
    	    }
    	}else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--trace") == 0){
    		i++;
    		if(i==argc){
    			fprintf(stderr,"Trace file name missing.\n");
    			return 0;
    		}
    		config.trace_file_base=argv[i];		
    	}else if(strcmp(argv[i], "--m") == 0){
			config.m_mod=true;
		}else if(strcmp(argv[i], "--i") == 0){
			config.i_mod=true;	    		
    	}else{
    		fprintf(stderr,"Ignoring invalid option %s\n",argv[i]);
    	}
    	i++;
    }
	return 1;
  
  
  
 }
 
}


/* check that the given file can be read/written */
int check_file(char *filename, char *mode){
	if(u_option==0)
	{
		FILE *file=fopen(filename,mode);
		if (file==NULL){
			fprintf(stderr,"Unable to open file %s in %c mode",filename,mode);
			fatal("\n");
			return 0;
		}else {fclose(file);return 0;}
	}
	if(u_option==1)
	{
		FILE *file=fopen(filename,mode);
		if (file==NULL){
			fprintf(stderr,"Unable to open file %s\n",filename);
			return 0;
		}
		else{
			fclose(file);
			return 1;
		}
	}
}


/*
 *  MAIN - entry point
 */
void dump_nfa(NFA *nfa,char *str)                                              //dumps the nfa into file <str>.txt
{
		int k,flag;
		pair_set *pnfaset;
		nfa_list *queuenfa = new nfa_list();
		nfa->traverse(queuenfa);
		printf("Dumping nfa in text file \n");
		FILE *fp;
		fp=fopen(str,"w");
		linked_set *accept;
		fprintf(fp,"#NFA dump \n");
		FOREACH_LIST(queuenfa,it1)
		{
			pnfaset = (*it1)->get_transitions();
			accept=(*it1)->return_accepting();
			fprintf(fp,"%d : { ( ",(*it1)->get_id());
			for(k=0;k<255;k++)
			{		
				flag=0;
				FOREACH_PAIRSET(pnfaset,it)
				{
					if((*it)->first == k)
						{flag=1;break;}
				}
				if(flag==1)
				{	
					fprintf(fp,"%d( ",k);
					FOREACH_PAIRSET(pnfaset,it)
					{
					if((*it)->first == k)
						fprintf(fp,"%d  ",(*it)->second->get_id());
					}
					fprintf(fp,") ");
				}
			
			}
			fprintf(fp,") ");
			if(!accept->empty())
			{
				while(accept)
				{
					fprintf(fp,"%d ",accept->value());
					accept=accept->succ();
				}
			}
			fprintf(fp,"}\n");
		}
		fclose(fp);
}
bool check_if_overlap(char *re)                                            //checks for overlap as well as containment for strings of the form substring1.*substring2)
{
	char a[10][100];
	unsigned int i,flag=0,j=0,k=0,m,count = 0,t;
	ofstream outfile;                                                      //create a file for output
	outfile.open("temporary.re",ios::trunc|ios::out);
	unsigned int count1=0,count2=0,count_1=0,count_2=0;
	bool i1,i2;
	for(i=0;re[i]!=0;i++)                                                //checks for .* only as the checking for .+,[^ ]* are already done in function overlap which calls this function.
	{
		if(re[i] == '.' && re[i+1] == '*' )
		{
			a[flag][j] = 0;
			flag++;
			i++;
			i++;
			j=0;
		}



		a[flag][j++] = re[i];


	}
	a[flag][j]=0;
	for(i=0;i<=flag;i++)
	{
		//outfile.write("^",1);
		outfile.write(a[i],strlen(a[i]));
		outfile<<endl;
	}
	outfile.close();
	FILE *fp;
	fp = fopen("temporary.re","r");
	if(flag==1)
	{


		NFA *nfa1 = new NFA();
		NFA *nfa2 = new NFA();
		NFA *nfa3 = new NFA();
		DFA *dfa1=NULL;
		DFA *dfa2=NULL;
		DFA *dfa3=NULL;
		regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);               
		nfa1 = parse->parse(fp, 1, 1);                                         //line 1 contains the first substring
		nfa2 = parse->parse(fp, 2, 2);                                         //line 2 contains the second substring
		nfa3 = parse->parse(fp, 1, 2);                                         // this makes the combined nfa for both the substrings   
		nfa1->remove_epsilon();
		nfa1->reduce();
		nfa2->remove_epsilon();
		nfa2->reduce();
		nfa3->remove_epsilon();
		nfa3->reduce();
		FILE *dot_file1 = fopen("..//debug_dotty//dfa1test.dot","w");
		FILE *dot_file2 = fopen("..//debug_dotty//dfa2test.dot","w");
		FILE *dot_file3 = fopen("..//debug_dotty//dfa3test.dot","w");
		dfa1 = nfa1->nfa2dfa();
		dfa2 = nfa2->nfa2dfa();
		dfa3 = nfa3->nfa2dfa();
		dfa1->to_dot(dot_file1,"DFAC");
		dfa2->to_dot(dot_file2,"DFAC");
		dfa3->to_dot(dot_file3,"DFAC");
		for(i=0;i<dfa1->size();i++)
		{
			if(!dfa1->accepts(i)->empty())
				count1++;
		}
		for(i=0;i<dfa2->size();i++)
		{
			if(!dfa2->accepts(i)->empty())
				count2++;
		}
		for(i=0;i<dfa3->size();i++)
		{
			if(dfa3->accepts(i)->member(1))
				count_1++;
			if(dfa3->accepts(i)->member(2))
				count_2++;
		}	
		if(count_1>count1)
			i1 = true;
		fclose(fp);
		fclose(dot_file1);
		fclose(dot_file2);
		fclose(dot_file3);
		return ((dfa1->intersection_DFA(dfa2))|i1);                //intersection DFA creates the intersection DFA of prefix DFA and modified suffix DFA, returns true if
	}                                                              // there is a valid intersection DFA(this checks for overlap). The containment is checked by bool i1 as shwon above


}


bool overlap(char *re)                       //function to call check_if_overlap with all the strings that should to be checked for overlap and complete containment for the string re 
{
	char a[10][100];
	char *tempchar;
	string str;
	unsigned int i,flag=0,j=0,k=0,m,count = 0,t;
	for(i=0;re[i]!=0;i++)                                                //check for .* like cases and store all substrings as seperate strings in a[10]100]. There are no more than 10 substrings.
	{
		if(re[i] == '.' && (re[i+1] == '*' || re[i+1] == '+'))
		{
			a[flag][j] = 0;
			flag++;
			i++;
			i++;
			
			j=0;
		}
		if(re[i] == '[' && re[i+1] == '^')
		{
			count = 0;
			//printf("i+2 = %c ",re[i+2]);
			t=i;
			for(m=i+2;re[m]!=']';m++)
			{
				if(re[m+1] != '-')
				{
					if(re[m] == '\\')
					{
						if(re[m+1] == 'x')
						{
							count++;
							m++;
							m++;
							m++;
						}
						else
						{
							count++;
							m++;
						}
					}
					else
					count++;
				}
				else
				{
					count = count + (re[m+2] - re[m]) + 1;
					m++;
					m++;
				}
			 
			}
			if(count <= 5 && (re[m+1] == '*' || re[m+1] == '+'))
			{
				a[flag][j] = 0;
				flag++;
				i = m+2;
				
				j=0;
			}
			else
			i=t;
		}
		
		a[flag][j++] = re[i];
		
		
	}
	a[flag][j]=0;
	for(i=1;i<=flag;i++)
	{
		str.clear();
		for(j=0;j<i;j++)
		{
			str.append(a[j]);
		}
		str.append(".*");
		str.append(a[i]);
		tempchar = new char [str.size()+1];
  		strcpy (tempchar, str.c_str());
  		if(check_if_overlap(tempchar))
  		return true;
  		delete[] tempchar;
	}
	return false;
}	


int return_data(string temp,string str[10],int_set *forbidden[10],string &bitmap)      //returns(indirectly) the set of substrings,int_set array of forbidden characters,flag,(*/+)
{										                                              //indicator bitmap
	unsigned int i,j;
	//string str[10];                                                                  // holds the substring
	//int_set *forbidden[10];                                                         //holds the forbidden character for each special state
	for(i=0;i<10;i++)
	{
		forbidden[i]->clear();
	}
	bitmap.clear();
	for(i=0;i<10;i++)
		str[i].clear();
	int m,flag=0,t;
	int flag2 = 0;
	char c;
	int count;
	char hex[3];
	unsigned int x;
	NFA *nfa = new NFA();
	regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);               
		for(i=0;i<temp.size();i++)
		{
			
			if(temp[i] == '.' && temp[i+1] == '*')
			{
				bitmap.push_back('0');
				flag++;
				i++;
				
				
			}
			else if(temp[i] == '.' && temp[i+1] == '+')
			{
				bitmap.push_back('1');
				flag++;
				i++;
				
			}
			else if(temp[i] == '[' && temp[i+1] == '^')
			{
				
					count = 0;					
					t=i;
					for(m=i+2;temp[m]!=']';m++)
					{
						if(temp[m] != '\\' && temp[m+1] != '-')
						{
							forbidden[flag]->insert((unsigned int)temp[m]);
							count++;
						}
						else if(temp[m] == '\\' && temp[m+1] == 'x')
						{
							hex[0] = temp[m+2];
							hex[1] = temp[m+3];
							hex[2] = 0;
							sscanf(hex,"%x",&x);
							forbidden[flag]->insert(x);
							count++;
							m++;
							m++;
							m++;
						}
						else if(temp[m+1] == '-')
						{
							for(c=temp[m];c<=temp[m+2];c++)
							forbidden[flag]->insert((unsigned int)c);
							count = count + (temp[m+2] - temp[m]) + 1;
							m++;
							m++;
						}
						else if(temp[m] == '\\' && temp[m+1] != 'x')
						{
							switch(temp[m+1])
							{
								case 'n' : { forbidden[flag]->insert(10);break;}
								case 'a' : { forbidden[flag]->insert(7);break;}
								case 'b' : { forbidden[flag]->insert(8);break;}
								case 't' : { forbidden[flag]->insert(9);break;}
								case 'f' : { forbidden[flag]->insert(12);break;}
								case 'r' : { forbidden[flag]->insert(13);break;}
								default  : { forbidden[flag]->insert((unsigned int)temp[m+1]);}
							}
							count++;
							m++;
						}
					}
					if(count <= 5 && (temp[m+1] == '*' || temp[m+1] == '+'))
					{
						
						if(temp[m+1] == '*')
							bitmap.push_back('0');
						else
							bitmap.push_back('1');
						flag++;
						i=m+1;
				
						
					}
					else
					{
						i=t;
						forbidden[flag]->clear();
						str[flag].push_back(temp[i]);
					}
					
		
			}
			else
			str[flag].push_back(temp[i]);
		}
	
	return flag;	                                       //only the flag is returned directly, the rest by referenced arguments
	
}
void make_final_files()                                     //the final correlation and superbroken files are made using this function
{
	ifstream infile1;
	infile1.open("..//debug_dotty//new_broken.txt",ifstream::in);
	FILE *fin;
	fin = fopen("..//debug_dotty//new_label2d.txt","r");
	int idata[500][2];                                        //for taking input from label2d
	string cdata[500];                                        //for giving output to correlation
	int num=0,i,j;                                            //num is for keeping the number count
	while(!feof(fin))
	{
		fscanf(fin,"%d %d",&idata[num][0],&idata[num][1]);
		num++;
	}
	fclose(fin);
	string temp;
	string str[10];
	string bitmap;
	int_set *forbidden[10];
	for(i=0;i<10;i++)
		forbidden[i] = new int_set(256);
	int flag;
	int count=0;
	ofstream outfile1("..//debug_output//superbroken.txt",ios::trunc | ios::out);
	ofstream outfile2("..//debug_output//correlation.txt",ios::trunc | ios::out);
	while(!infile1.eof())
	{
		getline(infile1,temp);
		flag = return_data(temp,str,forbidden,bitmap);
		for(i=0;i<=flag;i++)
		{
			if(i==0)
			{
				outfile1<<str[i]<<endl;
				outfile2<<idata[count][0]<<" "<<idata[count][1]<<" "<<i+1<<" "<<0<<" ";
				for(j=0;j<forbidden[i]->return_max();j++)
				{
					if(forbidden[i]->mbr(j))
					{
						outfile2<<j<<" ";
					}
				}
				outfile2<<endl;
			}
			else if(i==flag)
			{
				outfile1<<str[i]<<endl;
				outfile2<<idata[count][0]<<" "<<idata[count][1]<<" "<<i+1<<" "<<0<<" "<<endl;
				if(bitmap[i-1] == '1')
				{
					outfile1<<str[i-1]<<str[i]<<endl;
					outfile2<<idata[count][0]<<" "<<idata[count][1]<<" "<<0<<" "<<1<<" "<<endl;
				}
			}
			else
			{
				outfile1<<str[i]<<endl;
				outfile2<<idata[count][0]<<" "<<idata[count][1]<<" "<<i+1<<" "<<0<<" ";
				for(j=0;j<forbidden[i]->return_max();j++)
				{
					if(forbidden[i]->mbr(j))
					{
						outfile2<<j<<" ";
					}
				}
				outfile2<<endl;
				if(bitmap[i-1] == '1')
				{
					outfile1<<str[i-1]<<str[i]<<endl;
					outfile2<<idata[count][0]<<" "<<idata[count][1]<<" "<<0<<" "<<1<<" "<<endl;
				}
			}
		}
		count++;
	
				
	}
		
		

}

void remove_unwanted_cases()                               //to remove overlap and forbidden cases from label2d and broken
{
	FILE *fin0;
	fin0 = fopen("..//debug_output//overlapcases.txt","r");
	FILE *fin1;
	fin1 = fopen("..//debug_output//forbiddencases.txt","r");
	FILE *fin;
	fin = fopen("..//label2d.txt","r");
	ifstream infile3;
	infile3.open("..//input.re",ifstream::in);
	ofstream outfile1("..//debug_dotty//new_label2d.txt",ios::trunc | ios::out);
	ofstream outfile2("..//debug_dotty//new_broken.txt", ios::trunc | ios::out);
	unsigned int num=0,i,j,k;
	int idata[200][2];
	string *temp;
	while(!feof(fin))
	{
		fscanf(fin,"%d %d",&idata[num][0],&idata[num][1]);
		num++;
	}
	fclose(fin);
	int_set *ucases = new int_set(num);
	temp = new string [num];
	int *bitmap;
	bitmap = new int [num];
	for(i=0;i<num;i++)
		bitmap[i] = 0; 
	while(!feof(fin0))
	{
		fscanf(fin0,"%d",&i);
		ucases->insert(i-1);
	}
	
	while(!feof(fin1))
	{
		fscanf(fin1,"%d",&i);
		ucases->insert(i-1);
		
	}
	fclose(fin0);
	fclose(fin1);
	i=0;
	while(!infile3.eof())
	{
		getline(infile3,temp[i]);
		i++;
	}
	infile3.close();
	for(i=0;i<num;i++)
	{
		if(ucases->mbr(i))
		{
			j = idata[i][0];
			for(k=i-5;k<=i+5;k++)
			{
				if(idata[k][0] == j)
				{
					bitmap[k] = 1;
				}
			}
		}
	}
	for(i=0;i<num;i++)
	{
		if(bitmap[i] != 1)
		{
			outfile1<<idata[i][0]<<" "<<idata[i][1]<<endl;
			outfile2<<temp[i]<<endl;
		}
		else 
		{
			cout<<i+1<<endl;
			cout<<idata[i][0]<<" "<<idata[i][1]<<" "<<temp[i]<<endl;
		}
	}
	outfile1.close();
	outfile2.close();
		
	
	
}	

int to_dot_tailnfa(HybridFA *hfa)                          
{	                                                                 // function to create all the tail dfas and return the total number of tail states(a bit of a misnomer)									
	map <state_t, nfa_set*> *border;
	NFA *nfac = new NFA();
	DFA *dfat = NULL;
	state_t t;
	char str[100];	
	char str2[100];
	char str1[100];
	border = hfa->get_border();
	nfa_set *border_state=new nfa_set();
 	nfa_set *tail=new nfa_set();
 	int count1 = 0;
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
 		count1 = count1 + dfat->size();
 		delete dfat;
 	} 
 	
 	delete tail;
 	delete border_state;
	return count1;	
}


void count_overlap()                                                           //count the number of overlap cases in a regex file and prints the line numbers in the text file
{
	int i,count,num=0;
	string str;
	ifstream fin;
	fin.open("..//input.re",ifstream::in);
	ofstream outfile("..//debug_output//overlapcases.txt",ios::trunc | ios::out);
	char *tempchar;
	count=0;
	int lno[200];
	i=0;
	while(!fin.eof())
	{
		
		num++;
		printf(" %d \n",num);
		str.clear();
		getline(fin,str);
		tempchar = new char [str.size()+1];
  		strcpy (tempchar, str.c_str());
  		if(overlap(tempchar))
  		{
  			lno[count++] = i+1;
  			
  		}
  		i++;
  	}
  	//outfile<<'#'<<count<<endl;
  	for(i=0;i<count;i++)
 	 	outfile<<lno[i]<<endl;
  	outfile.close();
  	
}

bool forbidden_character_occurence(string temp)                                        //checks for forbidden char occurence in substring
{
	unsigned int i,j;
	string str[10];                   // holds the substring
	int_set *forbidden[10];            //holds the forbidden character for each special state
	for(i=0;i<10;i++)
		forbidden[i] = new int_set(256);
	int m,flag=0,t;
	int flag2 = 0;
	char c;
	int count;
	char hex[3];
	unsigned int x;
	NFA *nfa = new NFA();
	regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);               
		for(i=0;i<temp.size();i++)
		{
			
			if(temp[i] == '.' && temp[i+1] == '*')
			{
				flag++;
				i++;
				
				
			}
			else if(temp[i] == '.' && temp[i+1] == '+')
			{
				flag++;
				i++;
				
			}
			else if(temp[i] == '[' && temp[i+1] == '^')
			{
				
					count = 0;					
					t=i;
					for(m=i+2;temp[m]!=']';m++)
					{
						if(temp[m] != '\\' && temp[m+1] != '-')
						{
							forbidden[flag]->insert((unsigned int)temp[m]);
							count++;
						}
						else if(temp[m] == '\\' && temp[m+1] == 'x')
						{
							hex[0] = temp[m+2];
							hex[1] = temp[m+3];
							hex[2] = 0;
							sscanf(hex,"%x",&x);
							forbidden[flag]->insert(x);
							count++;
							m++;
							m++;
							m++;
						}
						else if(temp[m+1] == '-')
						{
							for(c=temp[m];c<=temp[m+2];c++)
							forbidden[flag]->insert((unsigned int)c);
							count = count + (temp[m+2] - temp[m]) + 1;
							m++;
							m++;
						}
						else if(temp[m] == '\\' && temp[m+1] != 'x')
						{
							switch(temp[m+1])
							{
								case 'n' : { forbidden[flag]->insert(10);break;}
								case 'a' : { forbidden[flag]->insert(7);break;}
								case 'b' : { forbidden[flag]->insert(8);break;}
								case 't' : { forbidden[flag]->insert(9);break;}
								case 'f' : { forbidden[flag]->insert(12);break;}
								case 'r' : { forbidden[flag]->insert(13);break;}
								default  : { forbidden[flag]->insert((unsigned int)temp[m+1]);}
							}
							count++;
							m++;
						}
					}
					if(count <= 5 && (temp[m+1] == '*' || temp[m+1] == '+'))
					{
						
						flag++;
						i=m+1;
				
						
					}
					else
					{
						i=t;
						forbidden[flag]->clear();
						str[flag].push_back(temp[i]);
					}
					
		
			}
			else
			str[flag].push_back(temp[i]);
		}
		ofstream outfile("temporary2.re",ios::trunc | ios::out);
		for(i=1;i<=flag;i++)
		outfile<<str[i]<<endl;
		outfile.close();
		FILE *fp;
		fp = fopen("temporary2.re","r");
		flag2 = 0;
		for(i=1;i<=flag;i++)
		{
			nfa = parse->parse(fp, i, i);
			nfa->remove_epsilon();
			nfa->reduce();
			
			for(j=0;j<forbidden[i-1]->return_max();j++)
			{
				if(forbidden[i-1]->mbr(j))
				{
					//cout<<str[i]<<" "<<j<<endl;
					if(nfa->check_occurence(j))
					{
						flag2 = 1;
						break;
					}
				}
			}
			if(flag2 == 1)
			break;
		}
		fclose(fp);
		if(flag2 == 1)
		return true;
		else 
		return false;
		
	
}

void forbidden_character_cases()                        //reports the forbidden character cases in input.re
{
	ifstream infile;
	infile.open("..//input.re",ifstream::in);
	ofstream outfile("..//debug_output//forbiddencases.txt",ios::trunc | ios::out);
	string temp;
	int count = 0;
	while(!infile.eof())
	{
		count++;
		temp.clear();
		getline(infile,temp);		
		if(forbidden_character_occurence(temp))
		{
			outfile<<count<<endl;
		}
        cout<<count<<endl;
	}
	infile.close();
	outfile.close();
}

void remove_exactmatch(DFA *dfa)                      //function to take care of .+ cases
{
    ifstream infile;
    infile.open("..//debug_output//correlation.txt",ifstream::in);
    vector < vector<int> > data;
    vector < vector<int> >::iterator it;
    vector<int>::iterator iti;
    string line;
    while(getline(infile,line))
    {
        vector<int> linedata;
        stringstream linestream(line);
        int value;
        while(linestream>>value)
        {
            linedata.push_back(value);
        }
        //cout<<endl;		
        data.push_back(linedata);
    }
    unsigned int i = 1;
    int_set *lno = new  int_set(data.size());
    for(it = data.begin(); it <data.end() ;it++)
    {
        if((*it)[3] == 1)
            lno->insert(i);
        i++;
    }
    for(i=0;i<=lno->return_max();i++)
    {
        if(lno->mbr(i))
        {
            for(state_t s=0;s<dfa->size();s++)
            {
                if(dfa->accepts(s)->member(i) && dfa->accepts(s)->member(i-1))
                {
                    dfa->remove_accepted_rules(s,i);
                    dfa->remove_accepted_rules(s,i-1);
                }
                else if(dfa->accepts(s)->member(i))
                {
                    dfa->remove_accepted_rules(s,i);
                }
            }
        }
    }
    
    
}
				
void tracing_SMARF(DFA *dfa)           //function for tracing SMARF
{
    ifstream infile;
    infile.open("..//debug_output//correlation.txt",ifstream::in);
    vector < vector<int> > data;
    vector < vector<int> >::iterator it,itr;
    vector<int>::iterator iti;
    string line;
    linked_set *temp = NULL;
    while(getline(infile,line))
    {
        vector<int> linedata;
        stringstream linestream(line);
        int value;
        while(linestream>>value)
        {
            linedata.push_back(value);
        }
        //cout<<endl;		
        data.push_back(linedata);
    }
    infile.close();
    ifstream infile1;
    infile1.open("..//trace_in.txt",ifstream::in);    
    stringstream buffer; 
    buffer << infile1.rdbuf();
    string contents(buffer.str());
    infile1.close();
    unsigned int i,j;
    unsigned int sas_size = data.size();
    unsigned int *marker = new unsigned int [sas_size];
    unsigned int *on = new unsigned int [sas_size];
    for(i=0;i<sas_size;i++)
    {
        marker[i] = 0;
        on[i] = 0;
    }
    for(i=0;i<sas_size;i++)
    {
        it = data.begin() + i;
        if((*it)[3] != 1)
        {
            if(it != data.end() - 1)
            {
                if((*it)[1] != (*(it+1))[1] | (*it)[0] != (*(it+1))[0])
                    marker[i] = 1;
                else if((*(it+1))[3] == 1  && ( (*it)[1] != (*(it+2))[1] | (*it)[0] != (*(it+2))[0] ) )
                    marker[i] = 1;
            }
            else
                marker[i] = 1;
        }
        else
            marker[i] = 2;
    }
    //for(i=0;i<sas_size;i++)
        //cout<<marker[i]<<" ";
    //cout<<endl;
    int_set *char_set[256];
    for(i=0;i<256;i++)
        char_set[i] = new int_set[sas_size];
    i = 0;
    for(it = data.begin(); it < data.end() ;it++)
    {
        i++;
        if(it->size() >= 4)
        {
            for(j=4; j < it->size() ;j++)
                char_set[(*it)[j]]->insert(i);
        }
    }
    for(it = data.begin() ; it< data.end() ;it++)
    {
        //for(i=0;i<it->size();i++)
            //cout<<(*it)[i]<<" ";
        //cout<<endl;
    }
    state_t s = 0,s1;
    for(i=0;i<contents.size();i++)
    {
        s1 = s;
        s = dfa->get_next_state(s1,(symbol_t)contents[i]);
        cout<<"char : "<<(unsigned int)contents[i]<<" "<<s1<<"->"<<s<<endl;
        for(j=0;j<char_set[contents[i]]->return_max();j++)
        {
            if(char_set[contents[i]]->mbr(j))
            {
                 on[j-1] = 0;
                 cout<<j<<" "<<contents[i]<<" "<<endl;
            }
        }
        temp = dfa->accepts(s);
        for(j = 1; j <= sas_size; j++)
        {
            if(temp->member(j))
            {
                //cout<<"sub rule : "<<j<<endl;
                it = data.begin() + j - 1;
                //for(unsigned int k=0;k<it->size();k++)
                //cout<<(*it)[k]<<" ";
                //cout<<endl;
                if(j>=2)
                {
                    if(marker[j-2] != 2)
                    {
                        if(marker[j-2] == 1)
                            on[j-1] = 1;
                        else if(on[j-2] == 1)
                            on[j-1] = 1;
                    }
                    else
                    {
                        if(marker[j-3] == 1)
                            on[j-1] = 1;
                        else if(on[j-3] == 1)
                            on[j-1] = 1;
                    }
                    if(marker[j-1] == 1 && on[j-1] == 1)
                        cout<<"Rule : "<<(*it)[0]<<" matched"<<endl;
                }
                else
                {
                    on[j-1] = 1;
                    if(marker[j-1] == 1 && on[j-1] == 1 )
                        cout<<"Rule : "<<(*it)[0]<<" matched"<<endl;
                }
            }

        }
    }
}

                        
void print_no_SAS_FAS(DFA *dfa)   //function to print the no of SAS and FAS
{
    ifstream infile;
    infile.open("..//debug_output//correlation.txt",ifstream::in);
    vector < vector<int> > data;
    vector < vector<int> >::iterator it,itr;
    vector<int>::iterator iti;
    string line;
    linked_set *temp = NULL;
    while(getline(infile,line))
    {
        vector<int> linedata;
        stringstream linestream(line);
        int value;
        while(linestream>>value)
        {
            linedata.push_back(value);
        }
        //cout<<endl;		
        data.push_back(linedata);
    }
    infile.close();
    unsigned int count_SAS = 0;
    unsigned int count_FAS = 0;
    unsigned int i,j;
    unsigned int sas_size = data.size();
    unsigned int *marker = new unsigned int [sas_size];
    for(i=0;i<sas_size;i++)
    {
        marker[i] = 0;
    }
    for(i=0;i<sas_size;i++)
    {
        it = data.begin() + i;
        if((*it)[3] != 1)
        {
            if(it != data.end() - 1)
            {
                if((*it)[1] != (*(it+1))[1] | (*it)[0] != (*(it+1))[0])
                    marker[i] = 1;
                else if((*(it+1))[3] == 1  && ( (*it)[1] != (*(it+2))[1] | (*it)[0] != (*(it+2))[0] ) )
                    marker[i] = 1;
            }
            else
                marker[i] = 1;
        }
        else
            marker[i] = 2;
    }
    int max_accepts = 0;

    for(state_t s = 0; s < dfa->size() ; s++)
    {
        temp = dfa->accepts(s);
        if(temp->size() > max_accepts)
            max_accepts = temp->size();
        if(!temp->empty())
        {
            for(i=1;i<=sas_size;i++)
            {
                if(temp->member(i))
                {
                    if(marker[i] == 1)
                        count_FAS++;
                    else
                        count_SAS++;
                }
            }
        }
    }
    cout<<"No of SAS : "<<count_SAS<<endl;
    cout<<"No of FAS : "<<count_FAS<<endl;
    cout<<"Max bits to be turned on by a SAS : "<<max_accepts<<endl;
}




int regex_default(int argc, char **argv)
{
    // read configuration
    init_conf();
    it_nfa = 0;
    int k;
    int flag;
    DFA *dfatemp = NULL;
    while(!parse_arguments(argc,argv)) usage();
    print_conf();
    VERBOSE=config.verbose;
    DEBUG=config.debug; if (DEBUG) VERBOSE=1;                  //verbose is always one if debug in 1

    //check that it is possible to open the files
    if (config.regex_file!=NULL) check_file(config.regex_file,"r");
    if (config.in_file!=NULL) check_file(config.in_file,"r");
    if (config.out_file!=NULL) check_file(config.out_file,"w");
    if (config.dot_file!=NULL) check_file(config.dot_file,"w");
    if (config.trace_file!=NULL) check_file(config.trace_file,"r");

    // check that either a regex file or a DFA import file are given as input
    if (config.regex_file==NULL && config.in_file==NULL){
        fatal("No data file - please use either a regex or a DFA import file\n");
    }
    if (config.regex_file!=NULL && config.in_file!=NULL){
        printf("DFA will be imported from the Regex file. Import file will be ignored");
    }

    /* FA declaration */
    NFA *nfa= new NFA();  	// NFA
    DFA *dfa=NULL;		//DFA
    NFA *nfac=NULL;	
    dfa_set *dfas=NULL; // set of DFAs, in case a single DFA for all RegEx in the set is not possible
    HybridFA *hfa=NULL; // Hybrid-FA
    ///*
    // if regex file is provided, parses it and instantiate the corresponding NFA.
    // if feasible, convert the NFA to DFA





    if (config.regex_file!=NULL){
        FILE *regex_file=fopen(config.regex_file,"r");
        fprintf(stderr,"\nParsing the regular expression file %s ...\n",config.regex_file);
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);              //calling the constructor function with configuration arguments 
        nfa = parse->parse(regex_file);                                               //nfa created              
        nfa->remove_epsilon();
        nfa->reduce();
        nfa_set *tree = new nfa_set();
        nfa->reset_state_id();
        //pair_set *pnfaset;
        //nfac=nfa->make_dup();
        //nfac->reset_state_id();
        //dump_nfa(nfa,"..//debug_output//nfa.txt");
        //printf("\n");	
        FILE *dot_file1=fopen("..//debug_dotty//NFA.dot","w");		
        nfa->to_dot(dot_file1, "NFA");
        fclose(dot_file1);
        //dfa=nfa->nfa2dfa();
        //dfa->minimize();
        //printf("No of states in the nfa : %d \n",nfa->size());
        //printf("No of dfa states : %d \n",dfa->size());
        //if (dfa==NULL) printf("Max DFA size %ld exceeded during creation: the DFA was not generated\n",MAX_DFA_SIZE);
        //else dfa->minimize();	
        if(!config.hfa)
        {
            dfa = nfa->nfa2dfa();
            dfa->minimize();
            remove_exactmatch(dfa);
            //tracing_SMARF(dfa);
            print_no_SAS_FAS(dfa);
            dfa->fast_compression_algorithm(1,-1);
            smarf_memory mem(dfa,240);
            long int a,b,c;
            a = mem.return_dfa_table_memory();
            b = mem.return_link_table_memory();
            c = mem.return_char_table_memory();
            cout<<"mem in bytes : "<<a<<" "<<b<<" "<<c<<endl;
        }

        fclose(regex_file);
        delete parse;
    }
    //*/
    // if a regex file is not provided, import the DFA 
    if (config.regex_file==NULL && config.in_file!=NULL){
        FILE *in_file=fopen(config.in_file,"r");
        fprintf(stderr,"\nImporting from file %s ...\n",config.in_file);
        dfa=new DFA();
        dfa->get(in_file);
        fclose(in_file);
    }

    // DFA export 
    if (dfa!=NULL && config.out_file!=NULL){
        FILE *out_file=fopen(config.out_file,"w");
        fprintf(stderr,"\nExporting to file %s ...\n",config.out_file);
        dfa->put(out_file);
        fclose(out_file);
    }

    // DOT file generation
    if (dfa!=NULL && config.dot_file!=NULL){
        FILE *dot_file=fopen(config.dot_file,"w");
        FILE *dot1 = fopen("..//debug_dotty//DFAR.dot","w");
        fprintf(stderr,"\nExporting to DOT file %s ...\n",config.dot_file);
        char string[100];
        if (config.regex_file!=NULL) sprintf(string,"source: %s",config.regex_file);
        else sprintf(string,"source: %s",config.in_file);
        dfa->to_dot(dot_file, "DFA");  
        remove_exactmatch(dfa);
        dfa->to_dot(dot1,"DFA");
        fclose(dot1);                 
        fclose(dot_file);
    }

    // HFA generation
    if (config.hfa){
        if (nfa==NULL) fatal("Impossible to build a Hybrid-FA if no NFA is given.");
        hfa=new HybridFA(nfa);
        hfa->minimize();
        /*FILE *dot_file2=fopen("..//debug_dotty//HFA.dot","w");		
          hfa->to_dot(dot_file2, "HFA");
          fclose(dot_file2);*/
        int c = to_dot_tailnfa(hfa);
        //map <state_t, nfa_set*> *nfaborder;
        //nfaborder = hfa->get_border();
        //nfa_set *tmpnfa;
        //tmpnfa = nfaborder->begin()->second;
        //nfac=*(tmpnfa->begin());
        //FILE *dot_file3=fopen("..//debug_output//NFAC.dot","w");		
        //nfac->to_dot(dot_file3, "NFAC");
        //fclose(dot_file3);
        //to_dot_tailnfa(hfa);
        /*if (hfa->get_head()->size()<100000) hfa->minimize();
          hfa->createset();
          linked_set *sset = new linked_set();
          sset=hfa->getsborder();
          printf("\nThe sborders are :");	
          if(!sset->empty())
          {
          while(sset)
          {
          printf("%d ",sset->value());
          sset=sset->succ();
          }
          }
          printf("\n");
          hfa->counting_tails();
          to_dot_tailnfa(hfa);
        //FILE *dot_file2=fopen("..//debug_output//HFA.dot","w");		
        //hfa->to_dot(dot_file2, "HFA");
        //fclose(dot_file2);
        //nfa->nfa2cdfa();
        printf("\nis all right");
        //hfa->make_tail_DFA();
        //hfa->to_dot(dot_file2, "HFA");
        printf("\n");*/
        hfa_memory hmem(hfa,240);
        long int ha = hmem.return_head_dfa_memory();
        long int hb = hmem.return_tail_dfa_memory();
        cout<<ha<<"  "<<hb<<endl;
        printf("HFA:: head size=%d, tail size=%d, number of tails=%d, border size=%d\n",hfa->get_head()->size(),hfa->get_tail_size(),hfa->get_num_tails(),hfa->get_border()->size());
        printf("\n No of tail dfa states : %d ",c);
    }

    // trace file traversal
    if (config.trace_file){
        trace *tr=new trace(config.trace_file);
        if (nfa!=NULL) tr->traverse(nfa);
        if (dfa!=NULL){
            tr->traverse(dfa);	
            if (dfa->get_default_tx()!=NULL) tr->traverse_compressed(dfa);
        }		
        if (hfa!=NULL) tr->traverse(hfa);
        delete tr;
    }

    // if the DFA was not generated because of state blow-up during NFA-DFA transformation,
    // then generate multiple DFAs
    /*if (config.regex_file!=NULL && dfa==NULL){
      printf("\nCreating multiple DFAs...\n");
      FILE *re_file=fopen(config.regex_file,"r");
      regex_parser *parser=new regex_parser(config.i_mod,config.m_mod);
      dfas = parser->parse_to_dfa(re_file);
      printf("%d DFAs created\n",dfas->size());
      fclose(re_file);
      delete parser;
      int idx=0;
      FOREACH_DFASET(dfas,it) {
      printf("DFA #%d::  size=%ld\n",idx,(*it)->size());
      if (config.out_file!=NULL){
      char out_file[100];
      sprintf(out_file,"%s%d",config.out_file,idx);
      FILE *file=fopen(out_file,"w");
      fprintf(stderr,"Exporting DFA #%d to file %s ...\n",idx,out_file);
      (*it)->put(file);
      fclose(file);
      idx++;
      }
      }
      }*/

    /*
     * ADD YOUR CODE HERE 
     * This is the right place to call the compression algorithms (see dfa.h, nfa.h),
     * and analyze your data structures.
     */

    /* BEGIN USER CODE */

    // write your code here

    ///*
    //	dfa->D2FA(1,1);
    //	dfa->analyze_default_tx_red();
    //	dfa->analyze_default_tx_red();
    //	dfa->bitmap_compression();
    //	dfa->sub_bitmap_compression();
    //	dfa->adaptive_bitmap_compression();
    //	dfa->adaptive_bitmap_compression();
    //	dfa->fast_compression_algorithm(1,-1);
    //	dfa->RCDFA(0);
    //dfa->RCDFA(dfa->_size-1);
    //	dfa->myRCDFA(0);
    //*/

    //	printf("Exporting DFA here to file  ...\n");

    //	dfa->fast_compression_algorithm(1,12);

    //	short *classes=new short[CSIZE];
    //	short num_classes=dfa->build_char_classes(&classes);
    //	bool *class_covered=allocate_bool_array(num_classes);
    //	dfa->analyze_default_tx(class_covered,CSIZE,0,'output.txt');


    /* END USER CODE */

    /* Automata de-allocation */

    if (nfa!=NULL) delete nfa;
    if (dfa!=NULL) delete dfa;
    if (dfas!=NULL){
        FOREACH_DFASET(dfas,it) delete (*it);
        delete dfas;
    }
    if (hfa!=NULL) delete hfa;	

    /*char a[100] = "abc.*dv.*(mnop|abcde)pqr";
      if(overlap(a))
      printf("\nThere is overlap\n");
      else
      printf("\n No overlap\n");*/



    return 0;
}

int main(int argc, char **argv)                             // "-z default" for main method and "-z tracegen" for main_tracegen method
{
    // read arguments to understand if it is tracegen
    unsigned int i = 1;
    while(i<argc)
    {
        if(strcmp(argv[i], "-z") == 0) 
        {
            i++;
            if (i == argc) 
            {
                fprintf(stderr,"Regex Mode Missing.\n");
                return 0;
            } 
            else 
            {
                if (strcmp(argv[i], "tracegen") == 0) 
                {
                    u_option = 1;	
                    regex_tracegen(argc, argv);
                } 
                else if (strcmp(argv[i], "default") == 0) 
                {
                    u_option = 0; 	
                    regex_default(argc, argv);
                                       
                }
                else if(strcmp(argv[i], "preprocess") == 0)
                {
                    count_overlap();
                    forbidden_character_cases();
                    remove_unwanted_cases();
                    make_final_files();
                }

            }
        }
        i++;
    }                

    return 0;
}

#include<stdio.h>
#include<fstream.h>
#include<string.h>
#include<iostream>
using namespace std;
int main(int argc,char **argv)
{
	unsigned int i = 1;
    	while(i<argc)
    	{
        	if(strcmp(argv[i], "-i") == 0) 
        	{
            		i++;
            		if (i == argc) 
            		{
                		fprintf(stderr,"Input file name missing.\n");
                		return 0;
            		} 
            		else 
            		{	
               			ifstream infile(argv[i]);
               			if(!infile)
            			cout<<"Not a valid input file \n";
            		}
            
        	}
       		if(strcmp(argv[i],"-o") == 0)
       		{
       			i++;
            		if (i == argc) 
            		{
                		fprintf(stderr,"Output file name missing.\n");
                		return 0;
            		} 
            		else 
            		{	
               			ofstream outfile(argv[i]);
            		}
            	}
       		
       		
       		i++;
   	}
   	char strtemp[1000];
   	unsigned count = 0;
   	undigned int no_stars;
   	while(infile)
   	{
   		infile.getline(strtemp,1000);
   		count++;
   		no_stars = 0;
   		for(i=0;strtemp[i]!=NULL;i++)
   		{
   			if(strtemp[i]=='.' && strtemp[i+1]=='*')
   			no_stars++;
   			
   	
}
   	
   	

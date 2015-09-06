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
 			cout<<endl;		
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

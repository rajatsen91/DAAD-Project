/*
This code if for or-seperation, counting the number of .* like conditions per line, make the label file and the label2d file
*/

#include<stdio.h>
#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<set>
#include<list>
#include<utility>
#define FOREACH_SET(set_id,it) \
	for(set<string>::iterator it=set_id.begin();it!=set_id.end();++it)
using namespace std;

char *parse(char *re,char **forest,int *n)                              // parses the regular expression based on '|' and'(', ')'
{
	char *final = new char [2*strlen(re)];
	char *final2 = new char [2*strlen(re)];
	int i=0,j=0,count=0,k=0,dcount=0,flag=0,first;
	list <char> stack ;
	flag=0;
	for(i=0;;i++)
	{
		if(i!=0)
		{
			if(re[i] == '[' && flag == 0 && re[i-1]!='\\')
			flag=1;
			if(re[i] == ']' && flag == 1 && re[i-1]!='\\')
			flag = 0;
		}
		if(re[i] == '(' && flag == 0 && re[i-1]!='\\')
		{
			stack.push_front('(');
			if(i!=0)
			{
				forest[count][j] = 0;
				final[k++]=count+'A';
				count++;
			}
			final[k++]='(';
			j=0;
		}
		else if(re[i] == ')' && flag == 0 && re[i-1]!='\\')
		{
			
			if(re[i+1] != ')' && re[i+1] != '|')
			{
				final[k++]=count+'A';
				final[k++]=')';
				forest[count][j] = 0;
				if(count!=0)
				count++;
				j=0;
				stack.push_front(')');
			}
			else
			{
				final[k++]=count+'A';
				final[k++] = ')';
				final[k++] = re[i+1];
				i++;
				forest[count][j] = 0;
				count++;
				j=0;
			}
			
			
		}
		else if(re[i] == '|' && flag == 0 && re[i-1]!='\\')
		{
			
			
				
				final[k++]=count+'A';
				final[k++]='|';
				forest[count][j] = 0;
				//if(count!=0)
				count++;
				j=0;
				stack.push_front('|');
				
			
		}
		else if(re[i]==0)
		{
			if(re[i-1] != ')')
			{
				final[k++]=count+'A';
				forest[count][j] = 0;
				if(count!=0)
				count++;
				j=0;
			}
			break;
		}	
		else
		{
			forest[count][j++] = re[i];
		}
		
		
	}
	final[k]=0;
	final[k+1] = 0;
	final[k+2] = 0;
	j=0;
	for(i=0;i<k;i++)
	{
		if(final[i] != '|' && final[i] != '(')
		{
			if(final[i+1] != ')' && final[i+1] != '|' && final[i+1] != 0)
			{
				final2[j++] = final[i];
				final2[j++] = '.';
				
			}
			else
			final2[j++] = final[i];
			
		}
		else
		final2[j++] = final[i];
	}
	final2[j] = 0;
	*n = count;
	//cout<<final2<<endl;
	return final2;
	
}

char *postfixer(char *re_in)                                //creates the postfix expression from the parsed regex forest
{
	char *postfix = new char [strlen(re_in)+10];
	char *re = new char [strlen(re_in)+10];
	sprintf(re,"(%s)",re_in);
	//printf("\n%s\n",re);
	int i=0,j=0;
	list <char> stack ;
	for(i=0;re[i]!=0;i++)
	{
		if(re[i] == '(')
		stack.push_front('(');
		else if(re[i] == '|')
		{
			while(stack.front() == '|' || stack.front() == '.')
			{
				postfix[j++] = stack.front();
				stack.pop_front();
			}
			stack.push_front('|');
		}
		else if(re[i] == ')')
		{
			while(stack.front() != '(')
			{
				postfix[j++] = stack.front();
				stack.pop_front();
				
			}
			stack.pop_front();
		}
		else if(re[i] == '.')
		{
			while(stack.front() == '.')
			{
				postfix[j++] = stack.front();
				stack.pop_front();
			}
			stack.push_front('.');
		}
		else
		{
			postfix[j++] = re[i];
		}
	}
	postfix[j]=0;
	return postfix;
}
void count_to_label(FILE *fp)                                             //chnages a count file to a label file
{
	ofstream outfile("label.txt",ios::trunc | ios::out);
	int n,i;
	int count=0;
	while(!feof(fp))
	{
		count++;
		fscanf(fp,"%d",&n);
		cout<<n<<endl;
		for(i=1;i<=n;i++)
		{
			outfile<<count<<endl;
		}
	}
	outfile.close();
}
	
	
	
int *marker(char **forest,int n)                                     // marks a string if it has a .* like condition and returns the marked bitmap of the forest
{
	int i=0,j=0,m=0,count=0,flag=0;
	int *bitmap = new int [n];
	for(i=0;i<n;i++)
	{
		flag=0;
		for(j=0;forest[i][j]!=0;j++)
		{
			if(forest[i][j] == '.' && (forest[i][j+1] == '*' || forest[i][j+1] == '+'))
			{
				bitmap[i] = 1;
				flag=1;
				break;
			}
			if(forest[i][j] == '[' && forest[i][j+1] == '^')
			{
				count = 0;
				//printf("i+2 = %c ",re[i+2]);
				//t=i;
				for(m=j+2;forest[i][m]!=']';m++)
				{
					if(forest[i][m+1] != '-')
					{
						if(forest[i][m] == '\\')
						{
							if(forest[i][m+1] == 'x')
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
						count = count + (forest[i][m+2] - forest[i][m]) + 1;
						m++;
						m++;
					} 
				}
				if(count <= 5 && (forest[i][m+1] == '*' || forest[i][m+1] == '+'))
				{
					bitmap[i]=1;
					flag=1;
					break;
				}
				
			}
		}
		if(flag==0)
		bitmap[i] = 0;
		
	}
	return bitmap;
}
bool check_if_marked(string re,int *bitmap)                         //checks if the string has a .* 
{
	int i;
	for(i=0;i<re.length();i++)
	{
		if(re[i]!='|' && re[i]!='.')
		if(bitmap[re[i]-'A'] == 1)
		return true;
	}
	return false;
}
set<string> or_operation(set<string> A,set<string> B,int *bitmap)                           //perform A|B
{
	set<string> C;
	string temp;
	FOREACH_SET(A,ita)
	{
		FOREACH_SET(B,itb)
		{
			if((!check_if_marked(*ita,bitmap)) && (!check_if_marked(*itb,bitmap)))
			{
				temp.clear();
				temp.append("(");
				temp.append(*ita);						
				temp.append("|");
				temp.append(*itb);
				temp.append(")");
				C.insert(temp);
			}
			else
			{
				C.insert(*ita);
				C.insert(*itb);
			}
		}
	}
	return C;
}	
set<string> and_operation(set<string> A,set<string> B,int *bitmap)                    //performs the concatenation operation
{
	set<string> C;
	string temp;
	FOREACH_SET(A,ita)
	{
		FOREACH_SET(B,itb)
		{
			temp.clear();
			temp.append(*ita);
			temp.append(*itb);
			C.insert(temp);
		}
	}
	return C;
}		

list< set<string> > seperator(char *exp,int *bitmap)                             //creates the final broken strings
{
	list< set<string> > stack;
	set<string> operand;
	set<string> operand1;
	set<string> result;
	string singular;
	int i=0;
	for(i=0;i<strlen(exp);i++)
	{
		if((exp[i] != '|') && (exp[i] != '.'))
		{
			singular.clear();
			singular.push_back(exp[i]);
			operand.clear();
			operand.insert(singular);
			stack.push_front(operand);
		}
		else if(exp[i] == '|')
		{
			operand.clear();
			operand1.clear();
			operand = stack.front();
			stack.pop_front();
			operand1 = stack.front();
			stack.pop_front();
			result.clear();
			result = or_operation(operand1,operand,bitmap);
			stack.push_front(result);
		}
		else
		{
			operand.clear();
			operand1.clear();
			operand = stack.front();
			stack.pop_front();
			operand1 = stack.front();
			stack.pop_front();
			result.clear();
			result = and_operation(operand1,operand,bitmap);
			stack.push_front(result);
		}
	}
	return stack;
}
		
set<string> output(char *re)                                //returns the actual broken regular expressions                                                  
{
	char *final = NULL;
	char *exp = NULL;
	char **forest;
	list< set<string> > result;
	set<string> divided;
	string temp;
	int *n = new int [1] ;
	forest = new char* [100];
	for(int i=0;i<100;i++)
	forest[i] = new char [100];
	final = parse(re,forest,n);
	int *bitmap = NULL;
	exp = postfixer(final);
	bitmap = marker(forest,*n);
	result = seperator(exp,bitmap);
	FOREACH_SET(result.front(),it)
	{
		temp.clear();
		for(int i=0;i<(*it).length();i++)
		{
			if((*it)[i] == '(' || (*it)[i] == ')' || (*it)[i] == '|')
			{
				temp.push_back((*it)[i]);
			}
			else
			{
				temp.append(forest[(*it)[i] - 'A']);
			}
		}
		divided.insert(temp);
	}
	return divided;
}			

void label_to_label2d()                               //coverts label to label2d
{
	ifstream infile;
	infile.open("label.txt",ifstream::in);
	ofstream outfile("label2d.txt",ios::out | ios::trunc);
	list <int> record;
	unsigned int n,i,cnt=1;
	while(!infile.eof())
	{
		infile>>n;
		record.push_back(n);
	}
	while(!record.empty())
    {
        n = record.front();
        record.pop_front();
        i = record.front();
        if( i != n)
        {
            outfile<<n<<" "<<cnt<<endl;
            cnt = 1;
        }
        else
        {
            outfile<<n<<" "<<cnt<<endl;
            cnt++;
        }
    }
    infile.close();
	outfile.close();
}	

void specialstates_per_line()                    //counts the number of .* like conditions per file
{
	ifstream infile;
	infile.open("spy-put-nor.txt",ifstream::in);
	string temp;
	unsigned int count_array[11],i,j,m,t,count;
	for(i=0;i<=10;i++)
		count_array[i] = 0;
	unsigned int x;
	while(!infile.eof())
	{
		temp.clear();
		getline(infile,temp);
		x=0;
		for(j=0;j<temp.size();j++)
		{
			if(temp[j] == '.' && (temp[j+1] == '*' || temp[j+1] == '+'))
			{
				x++;
			}
			if(temp[j] == '[' && temp[j+1] == '^')
			{
				count = 0;
				t = j;
				for(m=j+2;temp[m]!=']';m++)
				{
					if(temp[m+1] != '-')
					{
						if(temp[m] == '\\')
						{
							if(temp[m+1] == 'x')
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
						count = count + (temp[m+2] - temp[m]) + 1;
						m++;
						m++;
					} 
				}
				if(count <= 5 && (temp[m+1] == '*' || temp[m+1] == '+'))
				{
					x++;
					j = m+2;
				}
				else
					j = t;
				
			}
		}
		count_array[x]++;
	}
	for(unsigned i = 0;i<11;i++)
		cout<<count_array[i]<<" ";
	cout<<endl;
}
	
			
		
int main()
{
	//char a[] = "(longaccoun|qsver2auth|\\x35[57]\\x30|\\+\\x10\\*)";
	//set<string> divided;
	//divided = output(a);
	/*set<string> store;
	string use;
	int c=0,flag=0;
	string str;
	char *temp;
	ifstream infile("snort_3.re");
	ofstream outfile("result.txt",ios::trunc | ios::out);
	ofstream outfile2("broken.txt",ios::trunc | ios::out);	
	ofstream outfile3("count.txt",ios::trunc | ios::out);*/
	//FILE *fp = fopen("count.txt","r");
    /*string load_str;
    while(infile)
	{
		getline(infile,str);
		temp = new char [str.size()+1];
		strcpy(temp,str.c_str());
		outfile<<temp<<endl;
		store.clear();
		use.clear();
		flag=0;
		if(temp[0] == '^')
		{
			for(int i=0;i<strlen(temp);i++)
			temp[i]=temp[i+1];
			flag=1;
		}		
		store = output(temp);
		FOREACH_SET(store,it1)
		{
			load_str.clear();
			load_str.assign(it1->begin(),it1->end());
            		if(flag==0)
			{
				outfile2<<load_str<<endl;
				cout<<load_str<<endl;
				cout<<load_str<<endl;
			}
			else
			{
				use.clear();
				use.push_back('^');
				use.append(load_str);
				outfile2<<use<<endl;
				cout<<use<<endl;
			}
			
				
		}
		outfile3<<store.size()<<endl;
		c+=store.size();
		delete temp;
	}
	infile.close();
	outfile.close();
	outfile2.close();
	outfile3.close();*/
	//count_to_label(fp);
	//fclose(fp);	
	//FOREACH_SET(divided,it)
	//{
	//	cout<<*it<<endl;
	//}
	
	//cout<<c<<endl;
	//label_to_label2d();
	specialstates_per_line();
	
	
}
			
	

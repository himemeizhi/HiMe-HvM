/*
 * GET 0 a: push direct_number(a)
 * GET 1 a: push memory[a]
 * SET a: set memory[a]=s0
 * CUS %s: direct HVM code
 * LABEL %s: set a label
 * JUMP %s: jump to a label
 * JIF [01] [<=>] %s: 0 for no, 1 for yes; jump to label if true
 * CALL %s: call a process with label name
 * RETURN return process
 */
#include<cstdio>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
#include<cstdlib>
#include<map>
#include<list>

#define MAX_NUMBER_LEN (9)

typedef std::pair<std::string,int>psi;

int line_count;
std::string out;
std::map<std::string,int>label_map;
std::list<psi>later;

inline void WRONG_CODE(int line_count=::line_count)
{
	std::cout<<"Error code in "<<line_count<<std::endl;
	exit(1);
}

std::string gen_number(int a)
{
	if(a>810)
	{
		//not supported yet
		WRONG_CODE();
	}
	if(a<0)
		return "0"+gen_number(-a)+"-";
	std::string re("");
	if(a<10)
	{
		re+='0'+a;
		return re;
	}
	if(a<19)
		return "9"+gen_number(a-9)+"+";
	if(a<90)
	{
		char b,c;
		b='0'+a/9;
		c='0'+a%9;
		return std::string("9")+b+"*"+c+"+";
	}
	return std::string("9")+gen_number(a/9)+"*"+std::string(1,'0'+a%9)+"+";
}

std::string trim(std::string a)
{
	return a+std::string(MAX_NUMBER_LEN-a.size(),' ');
}

int i,j,k,n,m;

int main(int argn,char *argv[])
{
	std::ifstream fin(argv[1]);
	if(!fin)
	{
		std::cout<<"File opening failed\n";
		return EXIT_FAILURE;
	}
	static std::string buff,lb;
	while(!fin.eof())
	{
		++line_count;
		std::getline(fin,buff);
		if(!buff[0])
			break; // eof() doesnt work??
		if(buff[0]=='#')
			continue;// ignore comment
		std::istringstream in(buff);
		in>>buff;
		if(buff=="GET")
		{
			in>>i>>j;
			if(i==0)
				out+=gen_number(j);
			else if(i==1)
				out+=gen_number(j)+"<";
			else
				WRONG_CODE();
		}
		else if(buff=="SET")
		{
			in>>j;
			out+=gen_number(j)+">";
		}
		else if(buff=="CUS")
		{
			in>>buff;
			out+=buff;
		}
		else if(buff=="LABEL")
		{
			in>>buff;
			if(label_map.count(buff))
				WRONG_CODE();
			label_map[buff]=out.size();
		}
		else if(buff=="JUMP")
		{
			in>>buff;
			if(label_map.count(buff))
				out+=trim(gen_number(label_map[buff]-(out.size()+MAX_NUMBER_LEN+1)))+"g";
			else
			{
				later.push_back(psi("JUMP "+buff,out.size()));
				out+=std::string(MAX_NUMBER_LEN+1,' ');// calculate number and a 'g'
			}
		}
		else if(buff=="JIF")
		{
			in>>i>>buff>>lb;
			if(!label_map.count(lb))
			{
				later.push_back(psi(std::string("JIF ")+(i?"1":"0")+" "+buff+" "+lb,out.size()));
				out+=std::string(2+MAX_NUMBER_LEN+1,' ');// 2 for shift, number, and 1 for g
				continue;
			}
			if(i==0)
			{
				//not supported yet
				WRONG_CODE();
			}
			else if(i==1)
			{
				if(buff[0]!='<' && buff[0]!='=' && buff[0]!='>')
					WRONG_CODE();
				if(buff[0]=='<')
					out+="1+";
				else if(buff[0]=='>')
					out+="1-";
				out+=trim(gen_number(label_map[lb]-(out.size()+MAX_NUMBER_LEN+1)))+"?";
			}
			else
				WRONG_CODE();
		}
		else if(buff=="CALL")
		{
			in>>lb;
			if(!label_map.count(lb))
			{
				later.push_back(psi(std::string("CALL ")+lb,out.size()));
				out+=std::string(MAX_NUMBER_LEN+1,' ');// calculate number and a 'c'
				continue;
			}
			out+=gen_number(label_map[lb])+"c";
		}
		else if(buff=="RETURN")
			out+="$";
		else
			WRONG_CODE();
	}
	puts(out.c_str());
	for(std::list<psi>::const_iterator it(later.begin());it!=later.end();++it)
	{
		//		printf("%s : %d\n",it->first.c_str(),it->second);
		std::istringstream in(it->first);
		j=it->second;
		in>>buff;
		if(buff=="JIF")
		{
			in>>i>>buff>>lb;
			if(i==0)
			{
				//not supported yet
				WRONG_CODE(it->second);
			}
			else if(i==1)
			{
				if(buff[0]!='<' && buff[0]!='=' && buff[0]!='>')
					WRONG_CODE(j);
				if(buff[0]=='<')
					out.replace(j,2,"1+");
				else if(buff[0]=='>')
					out.replace(j,2,"1-");
				j+=2;
//				printf("%d %d\n",j,label_map[lb]);
				out.replace(j,MAX_NUMBER_LEN+1,trim(gen_number(label_map[lb]-(j+MAX_NUMBER_LEN+1)))+"?");
				/*
				buff=gen_number(label_map[lb]-(j+8));
				for(k=0;k<buff.size();++k)
					out[j+k]=buff[k];
				out[j+7]='?';
				*/
			}
		}
		else if(buff=="JUMP")
		{
			//not supported yet
			WRONG_CODE(it->second);
		}
		else if(buff=="CALL")
		{
			in>>lb;
			out.replace(j,MAX_NUMBER_LEN+1,gen_number(label_map[lb])+"c");
		}
		else WRONG_CODE(it->second);
	}
	puts(out.c_str());
	return 0;
}

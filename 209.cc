/*
 * GET 0 a: push direct_number(a)
 * GET 1 a: push memory[a]
 * SET a: set memory[a]=s0
 * CUS %s: direct HVM code
 * LABEL %s: set a label
 * JUMP %s: jump to a label
 * JIF [01] [<=>] %s: 0 for no, 1 for yes; jump to label if true
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

typedef std::pair<std::string,int>psi;

int line_count;
std::string out;
std::map<std::string,int>label_map;
std::list<psi>later;

inline void WRONG_CODE()
{
	std::cout<<"Error code in "<<line_count<<std::endl;
	exit(1);
}

inline void WRONG_CODE(int line_count)
{
	std::cout<<"Error code in "<<line_count<<std::endl;
	exit(1);
}

std::string gen_number(int a)
{
	if(a>90)
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
	static char b,c;
	b='0'+a/9;
	c='0'+a%9;
	return std::string("9")+b+"*"+c+"+";
	return re;
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
	static std::string buff;
	while(!fin.eof())
	{
		++line_count;
		std::getline(fin,buff);
		if(!buff[0])
			break;
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
				out+=gen_number(label_map[buff])+"c";
			else
			{
				later.push_back(psi("JUMP "+buff,out.size()));
				out+=std::string(6,' ');// 6 space, 5 for calculate number, 1 for 'c'
			}
		}
		else if(buff=="JIF")
		{
			static std::string lb;
			in>>i>>buff>>lb;
			if(!label_map.count(lb))
			{
				later.push_back(psi(std::string("JIF ")+(i?"1":"0")+" "+buff+" "+lb,out.size()));
				out+=std::string(10,' ');// 10 space, 2 for shift, 7 for calculate number, 1 for c
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
				buff=gen_number(label_map[lb]-(out.size()+8));
				out+=buff+std::string(7-buff.size(),' ');
				out+="?";
			}
			else
				WRONG_CODE();
		}
		else
			WRONG_CODE();
	}
	puts(out.c_str());
	for(std::list<psi>::const_iterator it(later.begin());it!=later.end();++it)
	{
//		printf("%s : %d\n",it->first.c_str(),it->second);
		std::istringstream in(it->first);
		in>>buff;
		if(buff=="JIF")
		{
			static std::string lb;
			in>>i>>buff>>lb;
			if(i==0)
			{
				//not supported yet
				WRONG_CODE(it->second);
			}
			else if(i==1)
			{
				j=it->second;
				if(buff[0]!='<' && buff[0]!='=' && buff[0]!='>')
					WRONG_CODE();
				if(buff[0]=='<')
					out.replace(j,2,"1+");
				else if(buff[0]=='>')
					out.replace(j,2,"1-");
				j+=2;
				buff=gen_number(label_map[lb]-(j+8));
				for(k=0;k<buff.size();++k)
					out[j+k]=buff[k];
				out[j+7]='?';
			}
		}
		else//JUMP
		{
			//not supported yet
			WRONG_CODE(it->second);
		}
	}
	puts(out.c_str());
	return 0;
}

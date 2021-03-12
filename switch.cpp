#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
using namespace std;


int command(string line)
	{

	    int sep=line.find(" ");
	    string opn = line.substr(0,sep);
	    string cmd = line.substr(sep,line.length());
	    vector<string> oper;
	    boost::split(oper, cmd, [](char c)return c==",";);
	    /*
	    while (getline(ss,str,",")){
            oper.push_back(str);
	    }*/
        return 0;
	}
int main()
{


    vector<string> program;
    string fileName;
    cin>>fileName;
    ifstream file(fileName);
    string line;
    while (getline(file,line)){
        program.push_back(line);
    }
    file.close();

    for (int i=0; i<program.size();i++)
    {
        command(program[i]);

    }


    return 0;







}

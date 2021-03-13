#include <bits/stdc++.h>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

int command(string line)
{
	char_separator<char> sep(", \t");
	tokenizer<char_separator<char>> tokens(line, sep);
	// boost::split(oper, cmd, [](char c) { return c == ','; });
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
	cin >> fileName;
	ifstream file(fileName);
	string line;
	while (getline(file, line))
	{
		program.push_back(line);
	}
	file.close();

	for (int i = 0; i < program.size(); i++)
	{
		command(program[i]);
	}

	return 0;
}

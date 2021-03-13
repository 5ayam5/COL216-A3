#include <bits/stdc++.h>

using namespace std;

// struct to store the registers and the functions to be executed
struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr, PCnext;
	unordered_map<string, function<int(MIPS_Architecture &, string , string , string )>> instructions;
	unordered_map<string, int> registerMap, address, data;
	static const int MAX = (1 << 20);

	MIPS_Architecture()
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}};
	}

	// perform add operation
	int add(string r1, string r2, string r3)
	{
		if (!checkRegisters({r1, r2, r3}))
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] + registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform subtraction operation
	int sub(string r1, string r2, string r3)
	{
		if (!checkRegisters({r1, r2, r3}))
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] - registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform multiplication operation
	int mul(string r1, string r2, string r3)
	{
		if (!checkRegisters({r1, r2, r3}))
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] * registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the beq operation
	int beq(string r1, string r2, string label)
	{
		return bOP(r1, r2, label, [](int a, int b) { return a == b; });
	}

	// perform the bne operation
	int bne(string r1, string r2, string label)
	{
		return bOP(r1, r2, label, [](int a, int b) { return a != b; });
	}

	// implements beq and bne by taking the comparator
	int bOP(string r1, string r2, string label, function<bool(int, int)> comp)
	{
		if (address.find(label) == address.end())
			return 2;
		int r2Val;
		if (r2[0] == '$')
		{
			if (!checkRegister(r2))
				return 1;
			r2Val = registers[registerMap[r2]];
		}
		else
		{
			try
			{
				r2Val = stoi(r2);
			}
			catch (exception &e)
			{
				return 3;
			}
		}
		PCnext = comp(registers[registerMap[r1]], r2Val) ? address[label] : PCcurr + 1;
	}

	// implements slt operation
	int slt(string r1, string r2, string r3)
	{
		if (!checkRegisters({r1, r2, r3}))
			return 1;
		registers[registerMap[r3]] = registers[registerMap[r1]] < registers[registerMap[r2]];
		return 0;
	}

	// perform the jump operation
	int j(string label, string unused1 = "", string unused2 = "")
	{
		if (address.find(label) == address.end())
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(string r, string location, string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		if (data.find(location) == data.end())
			data[location] = 0;
		registers[registerMap[r]] = data[location];
		++PCnext;
		return 0;
	}

	// perform store word operation
	int sw(string r, string location, string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		data[location] = registers[registerMap[r]];
		++PCnext;
		return 0;
	}

	// perform add immediate operation
	int addi(string r1, string r2, string num)
	{
		if (!checkRegisters({r1, r2}))
			return 1;
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			return 0;
		}
		catch (exception &e)
		{
			return 3;
		}
	}

	// checks if the register is a valid one
	inline bool checkRegister(string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(vector<string> regs)
	{
		return all_of(regs.begin(), regs.end(), [&](string r) { return checkRegister(r); });
	}

	/*
		handle all possible errors:
		1: register provided is incorrect
		2: invalid label
		3: syntax error
	*/
	void handleErrors(int code)
	{
	}

	int command(string line)
	{

	    int sep=line.find(" ");
	    string opn = line.substr(0,sep);
	    string cmd = line.substr(sep,line.length());
	    vector<string> oper;
	    stringstream ss(cmd);
	    cout<<cmd<<"\n";
        /*
	    string str;
	    while (getline(ss,str,",")){
            oper.push_back(str);
	    }*/
        return 0;


	}
};








int main()
{
    vector<string> program;
    string fileName;
    cin>>fileName;
    ifstream file(fileName);
    if (file.is_open()){
        string line;
        while (getline(file,line)){
            program.push_back(line);
        }
        file.close();

        MIPS_Architecture* trial = new MIPS_Architecture();
        for (int i=0; i<program.size();i++)
        {
            trial.command(program[i]);
        }







    }






}

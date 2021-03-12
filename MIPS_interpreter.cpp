#include <bits/stdc++.h>

using namespace std;

// struct to store the registers and the functions to be executed
struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	unordered_map<string, function<int(MIPS_Architecture &, string, string, string)>> instructions;
	unordered_map<string, int> registerMap, address, data;
	static const int MAX = (1 << 18);
	vector<vector<string>> commands;

	// constructor to initialise the instruction set
	MIPS_Architecture()
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};

		for (int i = 0; i < 32; ++i)
			registerMap["$" + to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + to_string(i)] = i + 4;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + to_string(i)] = i + 8, registerMap["$s" + to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;
	}

	// perform add operation
	int add(string r1, string r2, string r3)
	{
		return op(r1, r2, r3, [&](int a, int b) { return a + b; });
	}

	// perform subtraction operation
	int sub(string r1, string r2, string r3)
	{
		return op(r1, r2, r3, [&](int a, int b) { return a - b; });
	}

	// perform multiplication operation
	int mul(string r1, string r2, string r3)
	{
		return op(r1, r2, r3, [&](int a, int b) { return a * b; });
	}

	// perform the operation
	int op(string r1, string r2, string r3, function<int(int, int)> operation)
	{
		if (!checkRegisters({r1, r2, r3}))
			return 1;
		int r3Val;
		if (r3[0] == '$')
		{
			if (!checkRegister(r3))
				return 1;
			r3Val = registers[registerMap[r3]];
		}
		else
		{
			try
			{
				r3Val = stoi(r3);
			}
			catch (exception &e)
			{
				return 3;
			}
		}
		registers[registerMap[r1]] = operation(registers[registerMap[r2]], r3Val);
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
		if (address[0] < 65)
			return 3;
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
		return 0;
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
		if (label[0] < 65)
			return 3;
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
		if (location[0] < 65)
			return 3;
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
		if (location[0] < 65)
			return 3;
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
		switch (code)
		{
		case 1:
			cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			cerr << "Label used not defined\n";
			break;
		case 3:
			cerr << "Syntax error encountered\n";
			break;
		default:
			break;
		}
		for (auto &str: commands[PCcurr])
			cerr << str << ' ';
		cerr << '\n';
	}
};

int main()
{
}

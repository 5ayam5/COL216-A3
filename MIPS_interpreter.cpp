#include <bits/stdc++.h>
#include <boost/tokenizer.hpp>

using namespace std;


// struct to store the registers and the functions to be executed
struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	unordered_map<string, function<int(MIPS_Architecture &, string, string, string)>> instructions;
	unordered_map<string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX] = {0};
	vector<vector<string>> commands;
	vector<int> commandCount;

	// constructor to initialise the instruction set
	MIPS_Architecture(ifstream &file)
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

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
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
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
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
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end())
			return 2;
		if (!checkRegisters({r1, r2}))
			return 1;
		PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCcurr + 1;
		return 0;
	}

	// implements slt operation
	int slt(string r1, string r2, string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r3]] = registers[registerMap[r1]] < registers[registerMap[r2]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the jump operation
	int j(string label, string unused1 = "", string unused2 = "")
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end())
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(string r, string location, string unused1 = "")
	{
		if (!checkRegister(r) || registerMap[r] == 0)
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		registers[registerMap[r]] = data[address];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform store word operation
	int sw(string r, string location, string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		data[address] = registers[registerMap[r]];
		PCnext = PCcurr + 1;
		return 0;
	}

	int locateAddress(string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				int address = registers[registerMap[reg]] + offset;
				if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
					return -3;
				return address / 4;
			}
			catch (exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address % 4)
				return -3;
			return address / 4;
		}
		catch (exception &e)
		{
			return -4;
		}
	}

	// perform add immediate operation
	int addi(string r1, string r2, string num)
	{
		if (!checkRegisters({r1, r2}))
			return 1;
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (exception &e)
		{
			return 4;
		}
	}

	// checks if label is valid
	inline bool checkLabel(string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c) { return (bool)isalnum(c); });
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
		handle all exit codes:
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(int code, int cycleCount)
	{
		cout << '\n';
		switch (code)
		{
		case 1:
			cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			cerr << "Label used not defined\n";
			break;
		case 3:
			cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			cerr << "Syntax error encountered\n";
			break;
		case 5:
			cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				cerr << s << ' ';
			cerr << '\n';
		}
		cout << "\nTotal number of cycles: " << cycleCount << '\n';
		cout << "Count of instructions executed:\n";
		for (int i = 0; i < (int)commands.size(); ++i)
		{
			cout << commandCount[i] << " times:\t";
			for (auto &s : commands[i])
				cout << s << ' ';
			cout << '\n';
		}
	}

	void parseCommand(string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		vector<string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
			address[command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?"] = commands.size();
		else if (command[0].back() == ':')
		{
			address[command[0].substr(0, command[0].size() - 1)] = commands.size();
			commands.push_back(vector<string>(command.begin() + 1, command.end()));
		}
		else if (command[0].find(':') != string::npos)
		{
			int idx = command[0].find(':');
			address[command[0].substr(0, idx)] = commands.size();
			command[0] = command[0].substr(idx + 1);
			commands.push_back(command);
		}
		else
			commands.push_back(command);
		return;
	}

	void constructCommands(ifstream &file)
	{
		string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}

	void executeCommands()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(5, 0);
			return;
		}

		int clockCycles = 0;
		while (PCcurr < commands.size())
		{
			++clockCycles;
			vector<string> &command = commands[PCcurr];
			while (command.size() < 4)
				command.push_back("");
			if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(4, clockCycles);
				return;
			}
			int ret = instructions[command[0]](*this, command[1], command[2], command[3]);
			if (ret != 0)
			{
				handleExit(ret, clockCycles);
				return;
			}
			++commandCount[PCcurr];
			PCcurr = PCnext;
			printRegisters(clockCycles);
		}
		handleExit(0, clockCycles);
	}

	void printRegisters(int clockCycle)
	{
		cout << "Cycle number: " << clockCycle << '\n';
		for (int i = 0; i < 32; ++i)
			cout << registers[i] << ' ';
		cout << '\n';
	}
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cerr << "Required argument: file_name\n./MIPS_interpreter <file name>\n";
		return 0;
	}
	ifstream file(argv[1]);
	MIPS_Architecture *mips;
	if (file.is_open())
		mips = new MIPS_Architecture(file);
	else
	{
		cerr << "File could not be opened. Terminating...\n";
		return 0;
	}

	mips->executeCommands();
	return 0;
}

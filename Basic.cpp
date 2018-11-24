/*
 * File: Basic.cpp
 * ---------------
 * Name: Zhang Zhicheng
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "statement.h"
#include "./StanfordCPPLib/error.h"
#include "./StanfordCPPLib/tokenscanner.h"

#include "./StanfordCPPLib/simpio.h"
#include "./StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

ResultType processLine(string line, Program & program, EvalState & state);

/* Main program */

int main()
{
	EvalState state;
	Program program;
	//cout << "Implementation of BASIC" << endl;
	while (true)
	{
		try
		{
			ResultType t = processLine(getLine(), program, state);
			if (t == EXECUTED) continue;
			else if (t == QUIT) break;
			else if (t == HELP)
			{
				cout << "Help functionality not provided" << endl;
				cout << "Please refer to the manual" << endl;
			}
		}
		catch (ErrorException &ex)
		{
			cout << "Error: " << ex.getMessage() << endl;
		}
	}
	return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

ResultType processLine(string line, Program & program, EvalState & state)
{
	if (line.length() == 0) return EXECUTED;
	StatementType curType = analyzeStatement(line);
	if (curType == COMMAND)
	{
		return program.runCommand(line);
	}
	else
	{
		size_t p = line.find(" ");
		if (line[0] >= '0' && line[0] <= '9')
		{
			int curLine = stringToInteger(line.substr(0, p));
			if (p == string::npos) program.removeSourceLine(curLine);
			else program.addSourceLine(curLine, line);
		}
		else
		{
			program.addSourceLine(0, line);
		}
		return EXECUTED;
	}
}

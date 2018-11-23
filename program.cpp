/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
using namespace std;

Program::Program()
{
	/* Empty */
}

Program::~Program()
{
	/* Empty */
}

void Program::clear()
{
	programTable.clear();
}

void Program::addSourceLine(int lineNumber, string line)
{
	int typeId = analyzeStatement(integerToString(lineNumber) + " " + line);
	if (typeId == 1)
	{
		auto cur = new SequentialStatement(line);
		programTable[lineNumber] = cur;
	}
	else if (typeId == 2)
	{
		auto cur = new ControlStatement(line);
		programTable[lineNumber] = cur;
	}
	else
	{
		auto cur = DirectlyExecutedStatement(line);
		cur.execute(state);
	}
}

void Program::removeSourceLine(int lineNumber)
{
	auto t = programTable[lineNumber];
	delete t;
	programTable.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber)
{
	return programTable[lineNumber]->getLine();
}

void Program::setParsedStatement(int lineNumber, Statement *stmt)
{
	/* 
	Method not needed
	Parsing already done in the constructor
	*/
}

Statement *Program::getParsedStatement(int lineNumber)
{
	return programTable[lineNumber];
}

int Program::getFirstLineNumber()
{
	return programTable.begin()->first;
}

int Program::getNextLineNumber(int lineNumber)
{
	auto t = programTable.lower_bound(lineNumber);
	return t->first;
}

/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.h"
using namespace std;

Program::Program()
{
	state = new EvalState();
}

Program::~Program()
{
	/* Empty */
}

void Program::clear()
{
	programTable.clear();
	delete state;
	state = new EvalState();
}

void Program::addSourceLine(int lineNumber, string line)
{
	StatementType typeId = analyzeStatement(line);
	if (typeId == SEQUENTIAL)
	{
		auto cur = new SequentialStatement(line);
		programTable[lineNumber] = cur;
	}
	else if (typeId == CONTROL)
	{
		auto cur = new ControlStatement(line);
		programTable[lineNumber] = cur;
	}
	else if (typeId == DIRECTLY_EXECUTED)
	{
		auto cur = DirectlyExecutedStatement(line);
		cur.execute(*state);
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
	auto t = programTable.upper_bound(lineNumber);
	if (t == programTable.end()) return -1;
	else return t->first;
}

void Program::execute()
{
	pair<int, Statement*> cur = *programTable.begin();
	while (cur.first != -1)
	{
		//cout << "Current line: " << cur.first << " " << cur.second->getLine() << endl;
		auto type = analyzeStatement(cur.second->getLine());
		if (type == SEQUENTIAL)
		{
			cur.second->execute(*state);
			if ((static_cast<SequentialStatement*>(cur.second))->hasEnd()) break;
			int t = getNextLineNumber(cur.first);
			cur = make_pair(t, programTable.count(t) ? programTable[t] : nullptr);
		}
		if (type == CONTROL)
		{
			int t = (static_cast<ControlStatement*>(cur.second))->getNextLine(*state);
			if (t == 0) t = getNextLineNumber(cur.first);
			else if (!programTable.count(t)) error("LINE NUMBER ERROR");
			cur = make_pair(t, programTable[t]);
		}
	}
}

ResultType Program::runCommand(string &line)
{
	if (line == "RUN") execute();
	else if (line == "LIST")
	{
		for (auto u : programTable)
		{
			cout << u.second->getLine() << endl;
		}
	}
	else if (line == "CLEAR") clear();

	if (line == "QUIT") return QUIT;
	else if (line == "HELP") return HELP;
	else return EXECUTED;
}

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
	programTable.clear();
	if (state != nullptr) delete state;
}

void Program::clear()
{
	programTable.clear();
	if (state != nullptr) delete state;
	state = new EvalState();
}

void Program::addSourceLine(int lineNumber, string line)
{
	StatementType typeId = analyzeStatement(line);
	shared_ptr<Statement> cur;

	if (typeId == SEQUENTIAL)
	{
		shared_ptr<SequentialStatement> tmp (new SequentialStatement(line));
		cur = dynamic_pointer_cast<Statement>(tmp);
		programTable[lineNumber] = cur;
	}
	else if (typeId == CONTROL)
	{
		shared_ptr<ControlStatement> tmp (new ControlStatement(line));
		cur = dynamic_pointer_cast<Statement>(tmp);
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
	//auto t = programTable[lineNumber];
	//delete t;
	programTable.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber)
{
	return programTable[lineNumber]->getLine();
}

void Program::setParsedStatement(int lineNumber, shared_ptr<Statement> &stmt)
{
	/*
	Method not needed
	Parsing already done in the constructor
	*/
}

shared_ptr<Statement> Program::getParsedStatement(int lineNumber)
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
	pair<int,shared_ptr<Statement>> cur = *programTable.begin();
	while (cur.first != -1)
	{
		//cout << "Current line: " << cur.first << " " << cur.second->getLine() << endl;
		auto type = analyzeStatement(cur.second->getLine());
		if (type == SEQUENTIAL)
		{
			cur.second->execute(*state);
			if ((dynamic_pointer_cast<SequentialStatement>(cur.second))->hasEnd()) break;
			int t = getNextLineNumber(cur.first);
			cur = make_pair(t, programTable.count(t) ? programTable[t] : nullptr);
		}
		if (type == CONTROL)
		{
			int t = (dynamic_pointer_cast<ControlStatement>(cur.second))->getNextLine(*state);
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

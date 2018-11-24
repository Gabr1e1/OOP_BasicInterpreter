/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.h"
using namespace std;

StatementType analyzeStatement(string &line) //line must be a complete line, nothing truncated
{
	if (line == "RUN" || line == "LIST" || line == "CLEAR" || line == "QUIT" || line == "HELP") return COMMAND;
	bool hasLineNum = (line[0] >= '0' && line[0] <= '9');
	if (hasLineNum)
	{
		string t = line.substr(line.find(" ") + 1, line.length() - line.find(" ") - 1);
		size_t p = t.find(" ");
		string identifier = t.substr(0, p);
		if (identifier == "IF" || identifier == "GOTO") return CONTROL;
		else return SEQUENTIAL;
	}
	else return DIRECTLY_EXECUTED;
}

/* Implementation of the Statement class */

Statement::Statement(string &_line) : line(_line), lineNumber(0)
{
	/* Empty */
}

Statement::~Statement()
{
	/* Empty */
}

string Statement::getLine()
{
	return line;
}

SequentialStatement::SequentialStatement(string &_line) : Statement(_line), terminated(false), exp(nullptr)
{
	string t = line.substr(line.find(" ") + 1, line.length() - line.find(" ") - 1);
	size_t p = t.find(" ");
	identifier = t.substr(0, p);
	statement = t.substr(p + 1, t.length() - p - 1);

	if (identifier == "REM") typeId = 1;
	else if (identifier == "LET") typeId = 2;
	else if (identifier == "PRINT") typeId = 3;
	else if (identifier == "INPUT") typeId = 4;
	else if (identifier == "END") typeId = 5;
}

SequentialStatement::~SequentialStatement()
{
	delete exp;
}

bool SequentialStatement::hasEnd()
{
	return terminated;
}

void SequentialStatement::execute(EvalState &state)
{
	if (typeId == 1) return;
	if (typeId == 5)
	{
		terminated = true;
		return;
	}
	if (typeId == 4)
	{
		string val;
		cout << "?";
		cin >> val;
		statement += " = " + stringToInteger(val); //Todo : exception handling
	}

	if (typeId == 2 || typeId == 3 || typeId == 4)
	{
		TokenScanner scanner;
		scanner.ignoreWhitespace();
		scanner.setInput(statement);
		exp = parseExp(scanner);
	}

	if (typeId == 2) exp->eval(state);
	if (typeId == 3) cout << (exp->eval(state)) << endl;
	if (typeId == 4) exp->eval(state);
}

ControlStatement::ControlStatement(string &_line) : Statement(_line), cmpId(0)
{
	string t = line.substr(line.find(" ") + 1, line.length() - line.find(" ") - 1);
	size_t p = t.find(" ");
	string identifier = t.substr(0, p);
	string statement = t.substr(p + 1, t.length() - p - 1);
	
	if (identifier == "GOTO")
	{
		typeId = 1;
		gotoLine = stringToInteger(statement);
	}
	else
	{
		typeId = 2;
		p = statement.find("THEN");
		gotoLine = stringToInteger(statement.substr(p + 4, statement.length() - p - 1));
		statement = statement.substr(0, p);
		p = statement.find('<'), cmpId = 1;
		if (p == string::npos) p = statement.find('>'), cmpId = 2;
		else p = statement.find('='), cmpId = 3;

		TokenScanner scanner;
		scanner.ignoreWhitespace();
		
		scanner.setInput(statement.substr(0, p));
		lhs = parseExp(scanner);

		scanner.setInput(statement.substr(p + 1, statement.size() - p - 1));
		rhs = parseExp(scanner);
	}
}

ControlStatement::~ControlStatement()
{
	delete lhs;
	delete rhs;
}

void ControlStatement::execute(EvalState &state)
{
	if (typeId == 1) return;
	int valLeft = lhs->eval(state);
	int valRight = rhs->eval(state);
	if (cmpId == 1) // <
	{
		if (valLeft >= valRight) gotoLine = 0; //Todo : negative or zero line number may cause Runtime Error
	}
	else if (cmpId == 2) // >
	{
		if (valLeft <= valRight) gotoLine = 0;
	}
	else
	{
		if (valLeft != valRight) gotoLine = 0;
	}
}

int ControlStatement::getNextLine()
{
	return gotoLine;
}

DirectlyExecutedStatement::DirectlyExecutedStatement(string &_line) : Statement(_line), exp(nullptr)
{
	size_t p = line.find(" ");
	string identifier = line.substr(0, p);
	statement = line.substr(p + 1, line.length() - p - 1);

	if (identifier == "LET") typeId = 1;
	else if (identifier == "PRINT") typeId = 2;
	else if (identifier == "INPUT") typeId = 3;
}

DirectlyExecutedStatement::~DirectlyExecutedStatement()
{
	delete exp;
}

void DirectlyExecutedStatement::execute(EvalState &state)
{
	if (typeId == 3)
	{
		string val;
		cout << "?";
		cin >> val;
		statement += " = " + stringToInteger(val);
	}
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.setInput(statement);
	exp = parseExp(scanner);
	if (typeId == 2) cout << exp->eval(state) << endl;
	else exp->eval(state);
}

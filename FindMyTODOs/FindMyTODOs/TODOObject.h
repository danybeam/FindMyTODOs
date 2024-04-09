#pragma once

#include <string>

// TODO: change structure to a N-Tree
// TODO: Each node is aware of their level of indentation
// TODO: Each node is in charge of printing only themselves with what they know
// TODO: insert operation is assumed to be performed in the root node
// TODO: needs to be aware of how deep we've gone to find its place
class TODOObject
{
public:
	TODOObject(int lineNumber, std::string line);
	~TODOObject();

	bool isFileName();

	bool operator<(TODOObject& other);
	operator std::string();

private:
	unsigned int m_lineNumber;
	std::string m_line;
};
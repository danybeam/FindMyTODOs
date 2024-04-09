#pragma once

#include <string>

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
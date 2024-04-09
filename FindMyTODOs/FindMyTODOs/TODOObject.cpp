#include "TODOObject.h"

#include <sstream>

TODOObject::TODOObject(int lineNumber, std::string line) :
	m_lineNumber(lineNumber),
	m_line(line)
{
}

TODOObject::~TODOObject()
{
}

bool TODOObject::isFileName()
{
	return this->m_lineNumber == -1;
}

bool TODOObject::operator<(TODOObject& other)
{
	return this->m_lineNumber < other.m_lineNumber;
}

TODOObject::operator std::string()
{
	std::stringstream builder;
	if (this->m_lineNumber != -1)
	{
		builder << "(" << this->m_lineNumber << ")\t: ";
	}
	 builder << this->m_line;

	return builder.str();
}

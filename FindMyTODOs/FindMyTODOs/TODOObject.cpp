#include "TODOObject.h"

#include <sstream>
#include <cassert>

TODOObject::TODOObject(std::string name, bool isDirectory) :
	name(name),
	isDirectory(isDirectory)
{
}

TODOObject::~TODOObject()
{

}

void TODOObject::insertSubdirectory(std::string directoryPath)
{
	directoryPath = directoryPath.substr(this->name.size());

	for (TODOObject* directory : this->childrenDirectories)
	{
		if (directoryPath.find(directory->name) == 0)
		{
			directory->insertSubdirectory(directoryPath);
			return;
		}
	}

	this->childrenDirectories.push_back(new TODOObject(directoryPath, true));
}

void TODOObject::insertLine(std::string filePath, std::string line, int lineNumber)
{
	filePath = filePath.substr(this->name.size());

	for (TODOObject* directory : this->childrenDirectories)
	{
		if (filePath.find(directory->name) == 0)
		{
			directory->insertLine(filePath, line, lineNumber);
			return;
		}
	}

	for (TODOObject* file : this->files)
	{
		if (filePath.find(file->name) == 0)
		{
			file->lines.emplace_back("(" + std::to_string(lineNumber) + ")\t: " + line);
			return;
		}
	}

	this->files.push_back(new TODOObject(filePath, false));
	this->files.back()->lines.emplace_back("(" + std::to_string(lineNumber) + ")\t: " + line);
}

std::string TODOObject::toString(int indentLevel)
{
	std::stringstream buffer;

	if (indentLevel == 0)
	{
		buffer << (unsigned char)218;
	}
	else if (indentLevel == 1)
	{
		buffer << (unsigned char)195;
	}
	else
	{
		buffer << "|";
	}

	for (int i = 0; i < indentLevel - 1; i++)
	{
		buffer << " ";
	}

	if (indentLevel > 0)
	{
		if (indentLevel > 1)
		{
			buffer << (unsigned char)192;
		}
		else if (indentLevel == 1)
		{
			buffer << "-";
		}
		buffer << (unsigned char)194;

		buffer << "->";

	}
	buffer << this->name << "\n";

	for (TODOObject* subItem : this->files)
	{
		buffer << subItem->toString(indentLevel + 1);
	}

	for (TODOObject* subItem : this->childrenDirectories)
	{
		bool isLastChild = subItem == this->childrenDirectories.back();
		buffer << subItem->toString(indentLevel + 1);
	}

	int index = 1;
	for (std::string line : this->lines)
	{
		char header = '|';
		buffer << header;
		for (int i = 0; i < indentLevel; i++)
		{
			buffer << " ";
		}
		if (index == this->lines.size())
		{
			buffer << (unsigned char)192;
		}
		else
		{
			buffer << (unsigned char)195;
		}
		buffer << "->";
		buffer << line;
		buffer << "\n";
		index++;
	}

	return buffer.str();
}

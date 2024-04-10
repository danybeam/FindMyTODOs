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

std::string TODOObject::toString(int indentLevel, int lastItemCount)
{
	std::stringstream buffer;
	int localLast = lastItemCount;
	bool atLeastOne = false;

	if (this->childrenDirectories.empty() && this->files.empty() && this->lines.empty())
	{
		return std::string();
	}

	// Tree stems

	for (int i = 0; i < indentLevel - localLast - 1; i++)
	{
		buffer << "|  ";
	}

	// padding

	for (int i = 0; i < lastItemCount - 1; i++)
	{
		buffer << "   ";
	}

	// bendy boy

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
		buffer << (unsigned char)192;
	}

	// other bendy boy
	if (indentLevel > 0)
	{
		buffer << "--";
		buffer << (unsigned char)194;
	}

	// arrow

	buffer << "-->";
	// File/directory name
	buffer << indentLevel << ", " << lastItemCount << " " << this->name << "\n";


	for (std::string& line : this->lines)
	{
		// Tree stems

		for (int i = 0; i < indentLevel - localLast + 1; i++)
		{
			buffer << "|  ";
		}

		// padding

		for (int i = 0; i < lastItemCount; i++)
		{
			buffer << "   ";
		}

		// bendy boy
		unsigned char bendiBoi = line == this->lines.back() ? 192 : 195;
		buffer << bendiBoi;
		// arrow
		buffer << "->";
		// line
		buffer << line << "\n";
		atLeastOne |= true;
	}

	for (TODOObject* subItem : this->files)
	{
		if (subItem == this->files.back() && this->childrenDirectories.empty())
		{
			localLast++;
		}

		std::string temp = subItem->toString(indentLevel + 1, localLast);
		atLeastOne |= !temp.empty();
		buffer << temp;
	}

	localLast = lastItemCount;

	for (TODOObject* subItem : this->childrenDirectories)
	{
		if (subItem == this->childrenDirectories.back())
		{
			localLast++;
		}

		std::string temp = subItem->toString(indentLevel + 1, localLast);
		atLeastOne |= !temp.empty();
		buffer << temp;
	}

	return atLeastOne ? buffer.str() : std::string();
}

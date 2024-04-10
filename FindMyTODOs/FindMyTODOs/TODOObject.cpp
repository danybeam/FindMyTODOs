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

// TODO: this doesn't take into account cases where it goes multiple levels deep
// need to update insert directory to be aware of last common ancestor for directories
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
			file->lines.emplace_back("(L" + std::to_string(lineNumber) + ")\t: " + line);
			return;
		}
	}

	this->files.push_back(new TODOObject(filePath, false));
	this->files.back()->lines.emplace_back("(L" + std::to_string(lineNumber) + ")\t: " + line);
}


std::string TODOObject::toString(int indentLevel, int lastItemCount)
{
	std::stringstream buffer;
	int localLast = lastItemCount;
	bool imLast = localLast < 0;
	if (imLast)
	{
		localLast *= -1;
	}
	bool atLeastOne = false;

	if (this->childrenDirectories.empty() && this->files.empty() && this->lines.empty())
	{
		return std::string();
	}

	// Tree stems
	for (int i = 1; i < indentLevel; i++)
	{
		char head = '|';
		if (
			indentLevel == localLast ||
			(localLast > 1 && i >= localLast)
			)
		{
			head = ' ';
		}
		buffer << head << "  ";
	}

	// arrow
	char head = (unsigned char)194;
	if (indentLevel == 0)
	{
		head = 218;
	}
	else
	{
		unsigned char subHead = 195;
		if (imLast)
		{
			subHead = 192;
		}
		buffer << subHead << "--";
	}
	buffer << head << "->";


	// File/directory name
	buffer << this->name << "\n";

	indentLevel++;

	for (std::string& line : this->lines)
	{
		// Tree stems
		for (int i = 1; i < indentLevel; i++)
		{
			char head = '|';
			if (i >= (indentLevel - localLast))
			{
				head = ' ';
			}
			buffer << head << "  ";
		}

		// arrow
		unsigned char head = 195;
		if (line == this->lines.back())
		{
			head = 192;
		}
		buffer << head << "---->";


		buffer << line << "\n";
		atLeastOne |= true;
	}

	for (TODOObject* subItem : this->files)
	{
		if (subItem == this->files.back() && this->childrenDirectories.empty())
		{
			localLast++;
			localLast *= -1;
		}

		std::string temp = subItem->toString(indentLevel, localLast);
		atLeastOne |= !temp.empty();
		buffer << temp;
	}

	localLast = lastItemCount;
	if (imLast)
	{
		localLast *= -1;
	}

	for (TODOObject* subItem : this->childrenDirectories)
	{
		if (subItem == this->childrenDirectories.back())
		{
			localLast++;
			localLast *= -1;
		}

		std::string temp = subItem->toString(indentLevel, localLast);
		atLeastOne |= !temp.empty();
		buffer << temp;
	}

	return atLeastOne ? buffer.str() : std::string();
}

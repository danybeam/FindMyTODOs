#include "TODOObject.h"

#include <sstream>
#include <cassert>

const unsigned char indent0Bend = 218;
const unsigned char bendAndContinue = 195;
const unsigned char bigPipe = 179;
const unsigned char lBend = 192;
const unsigned char bendyT = 194;
const unsigned char longDash = 196;

TODOObject::TODOObject(std::filesystem::path path) :
	currentPath(path)
{
}

TODOObject::~TODOObject()
{

}

void TODOObject::insertSubdirectory(std::filesystem::path directoryPath)
{
}

void TODOObject::insertLine(std::filesystem::path filePath, std::string line, int lineNumber)
{

	// if it's correct file. move to file and add
	for (TODOObject* file : this->files)
	{
		if (file->currentPath == filePath)
		{
			file->insertLine(filePath, line, lineNumber);
			return;
		}
	}

	// if it's correct directory move to directory and add
	for (TODOObject* directory : this->childrenDirectories)
	{
		auto filePathAsString = filePath.parent_path().string();
		auto directoryPathAsString = directory->currentPath.string();
		// if we're not the deepest we can go, enter subdirectory
		if ((directoryPathAsString.size() <= filePathAsString.size()) && filePathAsString.find(directoryPathAsString) == 0)
		{
			directory->insertLine(filePath, line, lineNumber);
			return;
		}


		for (auto path = directory->currentPath; path > this->currentPath; path = path.parent_path())
		{
			if (filePathAsString.find(path.string()) == 0 && path < directory->currentPath)
			{
				TODOObject* newNode = new TODOObject(path);
				std::swap(*newNode, *directory);
				directory->childrenDirectories.push_back(newNode);
				directory->insertLine(filePath, line, lineNumber);
				return;
			}
		}
	}

	// if it's here add and exit
	std::string newLine = "(L" + std::to_string(lineNumber) + ")\t: " + line;
	if (this->currentPath == filePath)
	{
		this->lines.push_back(newLine);
		return;
	}

	// if we are at the correct directory insert file and line
	if (this->currentPath == filePath.parent_path())
	{
		this->files.emplace_back(new TODOObject(filePath));
		this->files.back()->insertLine(filePath, line, lineNumber);
		return;
	}

	// if we're at the correct parent create subdirectory and move there
	this->childrenDirectories.emplace_back(new TODOObject(filePath.parent_path()));
	this->childrenDirectories.back()->insertLine(filePath, line, lineNumber);
}

std::string TODOObject::toString()
{
	return this->toString(0, this->currentPath.string(), 0, 0, false);
}

// TODO: Calculate how many tabs are needed and how many to use/not use at each level
std::string TODOObject::toString(int indentLevel, std::string rootFolder, int skipLeft, int skipRight, bool lastInLevel)
{
	std::stringstream buffer;
	bool atLeastOne = false;

	// Tree stems
	for (int i = 0; i < skipLeft; i++)
	{
		buffer << "   ";
	}

	for (int i = skipLeft + 1; i < indentLevel; i++)
	{
		buffer << bigPipe << "  ";
	}

	for (int i = 0; i < skipRight; i++)
	{
		buffer << "   ";
	}

	// bendy boy
	if (indentLevel == 0)
	{
		buffer << indent0Bend;
	}
	else if (lastInLevel)
	{
		buffer << lBend;
	}
	else
	{
		buffer << bendAndContinue;
	}

	buffer << longDash << longDash;// << "--";
	if (indentLevel > 0)
	{
		buffer << bendyT;
	}
	
	buffer << "-<<";
	buffer << " ";

	// Print the name
	if (this->currentPath.has_extension())
	{
		buffer << this->currentPath.filename().string() << "\n";
	}
	else if (indentLevel == 0)
	{
		buffer << this->currentPath.string() << "\n";
	}
	else
	{
		std::string miniName = this->currentPath.string().substr(rootFolder.size());
		buffer << miniName << "\n";
	}

	// Print all sub-directories
	for (auto directory : this->childrenDirectories)
	{
		bool isLastDirectory = directory == this->childrenDirectories.back();
		auto directoryString = directory->toString(indentLevel + 1, this->currentPath.string(), skipLeft, skipRight, isLastDirectory && this->files.empty());
		atLeastOne |= !directoryString.empty();

		buffer << directoryString;
	}

	// Print all sub-files
	for (auto file : this->files)
	{
		bool isLastFile = file == this->files.back();


		auto fileString = file->toString(indentLevel + 1, rootFolder, skipLeft, skipRight, isLastFile);
		atLeastOne |= !fileString.empty();

		buffer << fileString;
	}




	// Print lines
	if (lastInLevel)
	{
		skipRight++;
	}
	for (std::string& line : this->lines)
	{
		for (int i = 0; i < skipLeft; i++)
		{
			buffer << "   ";
		}
		for (int i = skipLeft; i < indentLevel - skipRight; i++)
		{
			buffer << bigPipe << "  ";
		}
		for (int i = 0; i < skipRight; i++)
		{
			buffer << "   ";
		}

		if (line == this->lines.back())
		{
			buffer << lBend;
		}
		else
		{
			buffer << bendAndContinue;
		}

		buffer << "----> " << line << "\n";
		atLeastOne |= true;
	}

	return atLeastOne ? buffer.str() : std::string();
}

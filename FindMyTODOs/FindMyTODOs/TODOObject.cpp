#include "TODOObject.h"

#include <sstream>
#include <cassert>
#include <iostream>

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
	// If we're at the correct file, insert directly
	if (this->currentPath == filePath)
	{
		std::string newLine = "(L" + std::to_string(lineNumber) + ")\t: " + line;
		this->lines.push_back(newLine);
		return;
	}

	// If there is no files and no children directories, we're in root.
	// Take ownership of root as if this was the deepest path
	// it prevents having a linear tree
	if (this->files.empty() && this->childrenDirectories.empty())
	{
		this->currentPath = filePath.parent_path();
	}

	// if correct file is present. move to file and add
	for (TODOObject* file : this->files)
	{
		if (file->currentPath == filePath)
		{
			file->insertLine(filePath, line, lineNumber);
			return;
		}
	}

	// If we are at the correct directory but the file was not present insert file and line
	if (this->currentPath == filePath.parent_path())
	{
		this->files.emplace_back(new TODOObject(filePath));
		this->files.back()->insertLine(filePath, line, lineNumber);
		return;
	}

	// Check if correct directory is present 
	// if it is move to directory and add
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

		// if we are correct-ish, find nearest common ancestor that is still bigger than the current path
		// and re-root the suspected directory under the common ancestor
		for (auto filePath2 = filePath.parent_path(); filePath2 > this->currentPath; filePath2 = filePath2.parent_path())
		{
			for (auto path = directory->currentPath; path > this->currentPath; path = path.parent_path())
			{
				// We know we found the common ancestor when both the shrunk file path and the shrunk directory path match
				if (filePath2 == path && path < directory->currentPath)
				{
					TODOObject* newNode = new TODOObject(path);
					std::swap(*newNode, *directory);
					directory->childrenDirectories.push_back(newNode);
					directory->insertLine(filePath, line, lineNumber);
					return;
				}
			}
		}
	}

	// If none of the subdirectories were good candidates, it means that this is the node that needs re-rooting
	auto filePathAsString = filePath.string();
	if (filePathAsString.find(this->currentPath.string()) != 0)
	{
		for (auto currentDirectory = this->currentPath; currentDirectory != currentDirectory.root_path(); currentDirectory = currentDirectory.parent_path())
		{
			if (filePathAsString.find(currentDirectory.string()) == 0)
			{
				TODOObject* newNode = new TODOObject(currentDirectory); // Make node with nearest common ancestor
				std::swap(*this, *newNode);                             // Swap current node with new node
				this->childrenDirectories.push_back(newNode);           // Save current node as nearest ancestor child
				break;
			}
		}
	}

	// If we're at the correct parent create subdirectory and move there
	this->childrenDirectories.emplace_back(new TODOObject(filePath.parent_path()));
	this->childrenDirectories.back()->insertLine(filePath, line, lineNumber);
}

std::string TODOObject::toString()
{
	return this->toString(0, this->currentPath.string(), 0, 0, false);
}

// TODO: Calculate how to keep TODOs aligned
std::string TODOObject::toString(int indentLevel, std::string rootFolder, int skipLeft, int skipRight, bool lastInLevel)
{
	std::stringstream buffer;
	bool atLeastOne = false;

	// Tree stems
	// Draw empty spaces to the left of the stem
	for (int i = 0; i < skipLeft; i++)
	{
		buffer << "   ";
	}

	// Draw pipes for the indentation level
	// take into account how many empty spaces have been drawn already
	// i = 1 so it draws one less stem
	// this makes indent 0 and indent 1 aligned to the left
	for (int i = 1; i < (indentLevel - skipLeft); i++)
	{
		buffer << bigPipe << "  ";
	}

	// Draw empty spaces after the stems
	// This is mostly for files on the last subdirectory of the last subdirectory on a level
	for (int i = 0; i < skipRight; i++)
	{
		buffer << "   ";
	}

	// bendy boy depending on whether it's the last folder or not
	// special case for the root of the file tree
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

	// two dashes for spacing
	buffer << longDash << longDash;

	// Fork in the file tree for subdirectories and files at this level
	if (indentLevel > 0)
	{
		buffer << bendyT;
	}

	// Make it pretty
	buffer << "-<<";
	buffer << " ";

	// Print the name
	// If at root, print everything
	// If on a subdirectory, only print the difference with root
	// If on a file, only print the file name
	if (indentLevel == 0)
	{
		buffer << this->currentPath.string() << "\n";
	}
	else if (!this->currentPath.has_extension())
	{
		std::string miniName = this->currentPath.string().substr(rootFolder.size());
		buffer << miniName << "\n";
	}
	else
	{
		buffer << this->currentPath.filename().string() << "\n";
	}

	// Print all sub-directories first
	// If we are the last subdirectory on a given level, 
	// skip one more tree stem
	if (lastInLevel)
	{
		skipLeft++;
	}
	for (auto directory : this->childrenDirectories)
	{
		bool isLastDirectory = directory == this->childrenDirectories.back() && this->files.empty();
		int localSkipLeft = skipLeft;
		auto directoryString = directory->toString(indentLevel + 1, this->currentPath.string(), localSkipLeft, skipRight, isLastDirectory);
		atLeastOne |= !directoryString.empty();

		buffer << directoryString;
	}

	// Print all files here next
	// if we're at last in level and there are no more subdirectories
	// we know we are at the last file of this part of the tree
	if (lastInLevel && this->childrenDirectories.empty())
	{
		// revert change made for subdirectories
		skipLeft--;


		// Draw one less pipe
		indentLevel--;

		// compensate for missing pipe
		skipRight++;
	}

	for (auto file : this->files)
	{
		bool isLastFile = file == this->files.back();
		auto fileString = file->toString(indentLevel + 1, rootFolder, skipLeft, skipRight, isLastFile);
		atLeastOne |= !fileString.empty();

		buffer << fileString;
	}


	// Print lines
	for (std::string& line : this->lines)
	{
		for (int i = 0; i < skipLeft; i++)
		{
			buffer << "   ";
		}
		for (int i = 0; i < indentLevel - skipLeft; i++)
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

		buffer << "----> ";

		// Debug data save for later
		// buffer << "la " << lastInLevel << " in " << indentLevel << " lf " << skipLeft << " r " << skipRight;

		buffer << line << "\n";
		atLeastOne |= true;
	}

	return atLeastOne ? buffer.str() : std::string();
}

std::string TODOObject::getPathAsString()
{
	return this->currentPath.string();
}

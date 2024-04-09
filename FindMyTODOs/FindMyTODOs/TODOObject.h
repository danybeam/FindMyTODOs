#pragma once

#include <string>
#include <vector>

// TODO: change structure to a N-Tree
// TODO: Each node is aware of their level of indentation
// TODO: Each node is in charge of printing only themselves with what they know
// TODO: insert operation is assumed to be performed in the root node
// TODO: needs to be aware of how deep we've gone to find its place
class TODOObject
{
public:
	TODOObject(std::string name, bool isDirectory);
	~TODOObject();

	void insertSubdirectory(std::string directoryPath);
	void insertLine(std::string filePath, std::string line, int lineNumber);
	std::string toString(int indentLevel);

private:
	bool isDirectory;
	std::string name;
	std::vector<std::string> lines;
	std::vector<TODOObject*> childrenDirectories;
	std::vector<TODOObject*> files;
};
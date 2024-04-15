#pragma once

#include <string>
#include <vector>
#include <filesystem>

class TODOObject {
public:
	TODOObject(std::filesystem::path path);
	~TODOObject();

	void insertSubdirectory(std::filesystem::path directoryPath);
	void insertLine(std::filesystem::path filePath, std::string line, int lineNumber);
	std::string toString();
	std::string toString(int indentLevel, std::string rootFolder, int skipLeft, int skipRight, bool lastInLevel);
	std::string getPathAsString();

private:
	std::filesystem::path currentPath;
	std::vector<std::string> lines;
	std::vector<TODOObject*> childrenDirectories;
	std::vector<TODOObject*> files;
};

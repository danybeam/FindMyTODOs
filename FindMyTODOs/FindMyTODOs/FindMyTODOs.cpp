#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>

#include "TODOObject.h"
#include <unordered_map>

// TODO: Extract all this stuff into different file
// trim from start (in place)
inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

inline void trim(std::string& s) {
	rtrim(s);
	ltrim(s);
}

// TODO: Exctract magic numbers into constants
// TODO: add config file and search that first if no arguments given
int main()
{
	// TODO: add arguments to be able to pick directory to check
	std::filesystem::path cwd = std::filesystem::current_path();

	std::unordered_map<std::string, std::vector<TODOObject>> directoryMap;

	directoryMap[cwd.string()].push_back(TODOObject(-1, cwd.string()));

	for (auto& path : std::filesystem::recursive_directory_iterator(cwd))
	{
		std::string currentPathAsString = path.path().string();

		if (path.is_directory())
		{
			directoryMap[path.path().string()].push_back(TODOObject(-1, path.path().string()));
			//std::cout << path.path().string() << std::endl;
		}
		else if (path.is_regular_file())
		{
			// TODO: add arguments to be able to chose one or multiple file types
			auto extension = path.path().extension();
			if (extension != ".cpp" && extension != ".h")
			{
				continue;
			}

			std::ifstream file(path.path().string());
			std::string currentLine;
			unsigned int lineNumber = 1;
			while (std::getline(file, currentLine))
			{
				// TODO: add support for FIXME comments
				// TODO: add support to define custom comments
				// TODO: make this check to be begins with
				if (currentLine.find("// TODO") != std::string::npos)
				{
					if (directoryMap[path.path().string()].empty())
					{
						directoryMap[path.path().string()].push_back(TODOObject(-1, path.path().string()));
					}

					trim(currentLine);
					directoryMap[path.path().string()].push_back(TODOObject(lineNumber, currentLine));
				}

				lineNumber++;
			}
			file.close();
		}

	}

	std::vector<std::string> keys;

	for (auto entry : directoryMap)
	{
		keys.push_back(entry.first);
	}

	// TODO: I'm sure this can be optimized/simplified by using index access instead of range loop
	std::sort(keys.begin(), keys.end());
	int indent = 1;
	int maxIndent = indent;
	int currentDirIndex;
	int currentKeyIndex = -1;
	bool resetLastTime = false;
	for (auto key : keys)
	{
		currentDirIndex = 0;
		currentKeyIndex++;
		if (directoryMap[key].size() == 1 && !resetLastTime)
		{
			indent = 1;
			maxIndent = indent;
			resetLastTime = true;
			std::cout << std::endl;
		}

		for (auto line : directoryMap[key])
		{
			currentDirIndex++;

			if (indent > 0)
			{
				unsigned char header = 195;
				if (indent == 1)
				{
					header = 218;
				}
				else if (
					(currentKeyIndex + 1) < keys.size() &&
					currentDirIndex == directoryMap[key].size() &&
					directoryMap[keys[currentKeyIndex + 1]].size() == 1 &&
					directoryMap[key].size() > 1
					)
				{
					header = 192;
				}
				else if (
					(currentKeyIndex + 1) == keys.size() &&
					currentDirIndex == directoryMap[key].size()
					)
				{
					header = 192;
				}

				std::cout << header;

				for (int i = 0; i < indent * 2; i++)
				{
					std::cout << "-";
				}

				std::cout << "> ";
			}

			std::cout << (std::string)line << std::endl;

			if (line.isFileName())
			{
				indent++;
				maxIndent = maxIndent > indent ? maxIndent : indent;
			}
		}
		if (directoryMap[key].size() != 1)
		{
			resetLastTime = false;
			indent--;
		}
	}
}
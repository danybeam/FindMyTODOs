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

	TODOObject root(cwd);

	for (auto& path : std::filesystem::recursive_directory_iterator(cwd))
	{
		std::string currentPathAsString = path.path().string();

		if (!path.is_regular_file())
		{
			continue;
		}
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
				trim(currentLine);
				if (currentLine.find("// TODO") == 0)
				{
					root.insertLine(path.path(), currentLine, lineNumber);
				}

				lineNumber++;
			}
			file.close();

	}

	std::cout << root.toString();
}
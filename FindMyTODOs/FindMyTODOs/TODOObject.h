#pragma once

#include <string>
#include <vector>

class TODOObject {
public:
  TODOObject(std::string name, bool isDirectory);
  ~TODOObject();

  void insertSubdirectory(std::string directoryPath);
  void insertLine(std::string filePath, std::string line, int lineNumber);
  std::string toString(int indentLevel, int lastItemCount = 0);

private:
  bool isDirectory;
  std::string name;
  std::vector<std::string> lines;
  std::vector<TODOObject *> childrenDirectories;
  std::vector<TODOObject *> files;
};

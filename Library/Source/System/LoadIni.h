#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

bool LoadIni(const char* file_path, std::map<unsigned, unsigned>& values) {
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open())
	{
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return false;
	}
	while (fileStream.good())
	{
		char buf[256];
		fileStream.getline(buf, 256);
		std::pair<unsigned, unsigned> value;
		sscanf_s(buf, "%u=%u", &value.first, &value.second);
		values.insert(value);
	}
	return true;
}

bool SaveIni(const char* file_path, std::map<unsigned, unsigned>& values) {
	std::fstream outputStream;
	outputStream.open(file_path, std::ios_base::out);
	if (!outputStream.is_open()) {
		std::cout << "Impossible to save " << file_path << "\n";
		return false;
	}
	for (auto& pair : values) {
		outputStream << pair.first << "=" << pair.second << std::endl;
	}
	outputStream.close();
	return true;
}
#pragma once
#include <regex>

class CStringHelper
{
public:
	CStringHelper();
	~CStringHelper();
	void ToLower(std::string& str);
	bool IsContains(std::string str, std::string find);
	void Replace(std::string& str, std::string find, std::string replace);
	void Replace(std::string& str, char find, char replace);
	void Split(std::string const& str, const char delim, std::vector<std::string>& out);
	void Split(std::string str, std::string delim, std::vector<std::string>& out);
	std::vector<std::string> Split(std::string str, std::string regex);
	void EraseUntil(std::string& str, std::string start, std::string end);
	std::string NumberFormat(int number, char splitter = '.');
	char* ToChar(std::string str);
	void ToChar2(std::string s);
	std::string NumberStringFormat(int number, char splitter = '.');

private:

};
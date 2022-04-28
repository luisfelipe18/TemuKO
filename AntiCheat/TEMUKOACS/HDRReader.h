#pragma once
#include "stdafx.h"

struct SRC
{
	uint32 nameLen;
	string name;
	uint32 offset;
	uint32 sizeInBytes;
	unsigned char* buff;
	SRC(uint32 nl, string n, uint32 o, uint32 s, unsigned char* b)
	{
		nameLen = nl;
		name = n;
		offset = o;
		sizeInBytes = s;
		buff = b;
	}
};

class HDRReader
{
	DWORD FindPattern(std::string search);
	string m_basePath;
public:
	static char* ReadAllBytes(const char* filename, int* read);
	DWORD FindPatternEx(char* mem, int size, std::string search);
	HDRReader(string basePath);
	char* m_hdr;
	int m_hdrSize;
	char* m_src;
	int m_srcSize;
	void Load();
	SRC GetSRC(string fileName);
	string CreateTMP(string fileName);
};

extern HDRReader* hdrReader;
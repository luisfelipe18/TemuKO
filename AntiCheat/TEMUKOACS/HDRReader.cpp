#include "HDRReader.h"
#include "TEMUKOACS Engine.h"

std::string basename(const std::string& filename);
bool fileexists(std::string& filename);

HDRReader::HDRReader(string basePath)
{
	m_basePath = basePath;
	m_hdr = ReadAllBytes(string(m_basePath + xorstr("UI\\ui.hdr")).c_str(), &m_hdrSize);
	m_src = ReadAllBytes(string(m_basePath + xorstr("UI\\ui.src")).c_str(), &m_srcSize);
}

void HDRReader::Load()
{
	
}

char* HDRReader::ReadAllBytes(const char* filename, int* read)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();
	int length = pos;
	char* pChars = new char[length];
	ifs.seekg(0, ios::beg);
	ifs.read(pChars, length);
	ifs.close();
	*read = length;
	return pChars;
}

DWORD HDRReader::FindPatternEx(char* mem, int size, std::string search)
{
	char* pattern = (char*)search.c_str();
	uint32 mask = search.size();
	if (mask > size) return NULL;

	for (int i = 0; i < size; i++)
	{
		bool found = true;
		for (int j = 0; j < mask; j++)
			if (mem[i + j] != pattern[j])
				found = false;
		if (found)
			return i;
	}

	return NULL;
}

DWORD HDRReader::FindPattern(std::string search)
{
	char* pattern = (char*)search.c_str();
	uint32 mask = strlen(pattern);
	if (mask > m_hdrSize) return NULL;

	for (int i = 0; i < m_hdrSize; i++)
	{
		bool found = true;
		for (int j = 0; j < mask; j++)
			if (m_hdr[i + j] != pattern[j])
				found = false;
		if (found)
			return i;
	}

	return NULL;
}

SRC HDRReader::GetSRC(string fileName)
{
	if(Engine->StringHelper->IsContains(fileName, xorstr("TEMUKOACS\\")))
		return SRC(0, "", 0, 0, NULL);

	fileName = basename(fileName);

	DWORD address = FindPattern(fileName);

	if (address == NULL)
		return SRC(0, "", 0, 0, NULL);

	uint32 nameLen = *(int*)&m_hdr[address - 4];

	if (!(nameLen > 0))
		return SRC(0, "", 0, 0, NULL);

	char* name = new char[nameLen];
	memcpy(&name[0], &m_hdr[address], nameLen);

	uint32 offset = *(int*)&m_hdr[address + nameLen];
	uint32 sizeInBytes = *(int*)&m_hdr[address + nameLen + sizeof(offset)];

	unsigned char* buff = new unsigned char[sizeInBytes];
	memcpy(&buff[0], &m_src[offset], sizeInBytes);

	return SRC(nameLen, name, offset, sizeInBytes, buff);
}

string HDRReader::CreateTMP(string fileName)
{
	fileName = basename(fileName);
	SRC src = hdrReader->GetSRC(fileName);
	if (src.nameLen == 0 || src.offset == 0 || src.sizeInBytes == 0)
		return fileName;

	CreateDirectoryA(string(m_basePath + xorstr("UI\\tmp")).c_str(), NULL);

	ofstream file;
	file.open(string(m_basePath + xorstr("UI\\tmp\\") + fileName).c_str(), ios::out | ios::binary);
	file.write((const char*)src.buff, src.sizeInBytes);
	file.close();
	return string(xorstr("UI\\tmp\\") + fileName).c_str();
}

std::string basename(const std::string& filename)
{
	if (filename.empty()) {
		return "";
	}

	auto len = filename.length();
	auto index = filename.find_last_of("/\\");

	if (index == std::string::npos) {
		return filename;
	}

	if (index + 1 >= len) {

		len--;
		index = filename.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filename;
		}

		if (index == 0) {
			return filename.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filename.substr(0, len);
		}

		return filename.substr(index + 1, len - index - 1);
	}

	return filename.substr(index + 1, len - index);
}

bool fileexists(std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return (bool)ifile;
}
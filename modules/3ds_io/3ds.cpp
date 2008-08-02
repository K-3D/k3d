#include "3ds.h"
#include <algorithm>

using namespace std;

C3dsParser::C3dsParser(void)
{
	header.id = 0;
	header.length = 0;
	m_file = NULL;
}

C3dsParser::C3dsParser(const char *filename)
{
	header.id = 0;
	header.length = 0;
	m_file = NULL;
	LoadFile(filename);
}

C3dsParser::~C3dsParser(void)
{
	if (m_file)
		fclose(m_file);
}

unsigned short C3dsParser::GetChunkName(void)
{
	return header.id;
}

void C3dsParser::SkipChunk(void)
{
	if (m_file == NULL)
		return;
	fseek(m_file, header.length - 6, SEEK_CUR);
	Read();
}

void C3dsParser::EnterChunk(long datalength)
{
	if (m_file == NULL)
		return;
	fseek(m_file, (signed long)datalength, SEEK_CUR);
	Read();
}

unsigned long C3dsParser::GetChunkLength(void)
{
	return header.length;
}

void C3dsParser::GetChunkData(void *buffer, unsigned long buffersize, unsigned long skip)
{
	fseek(m_file, skip, SEEK_CUR);
	unsigned long amt = min(buffersize, header.length - 6);
	amt = fread(buffer, 1, amt, m_file);
	fseek(m_file, (long)-1 * ((signed long)amt + (signed long)skip), SEEK_CUR);
}

void C3dsParser::Rewind(void)
{
	if (m_file == NULL)
		return;
	fseek(m_file, 0, SEEK_SET);
	Read();
}

unsigned long C3dsParser::StrDataLength(bool skipData)
{
	byte buf;
	long len = 0;
	do
	{
		fread(&buf, 1, 1, m_file);
		len++;
	} while (buf != 0x00);
	if (!skipData)
		fseek(m_file, -1 * len, SEEK_CUR);
	else
		Read();
	return len;
}

void C3dsParser::Read(void)
{
	fread(&header.id, 2, 1, m_file);
	fread(&header.length, 4, 1, m_file);
}

void C3dsParser::CloseFile(void)
{
	if (m_file != NULL)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

bool C3dsParser::Eof(void)
{
	if (m_file)
		return feof(m_file);
	else
		return true;
}

bool C3dsParser::LoadFile(const char *filename)
{
	if (m_file != NULL)
		fclose(m_file);
	m_file = fopen(filename, "rbR");
	if (m_file)
	{
		Read();
		return true;
	} else
		return false;
}
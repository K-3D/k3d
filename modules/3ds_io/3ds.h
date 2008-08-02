#include <cstdio>
#include <cstdlib>

typedef unsigned char byte;

struct ChunkHeader
{
	unsigned short id;
	unsigned long length;
};


class C3dsParser
{
public:
	C3dsParser(void);
	C3dsParser(const char *filename);
	~C3dsParser(void);

	unsigned short GetChunkName(void);
	void SkipChunk(void);
	void EnterChunk(long datalength = 0);
	unsigned long GetChunkLength(void);
	void GetChunkData(void *buffer, unsigned long buffersize, unsigned long skip = 0);
	void SkipStrData(void) { StrDataLength(true); }
	unsigned long GetStrDataLength(void) { return StrDataLength(false); }
	void Rewind(void);
	bool LoadFile(const char *filename);
	bool Eof(void);
	void CloseFile(void);
protected:
	unsigned long StrDataLength(bool skipData);
	void Read(void);

	ChunkHeader header;
	FILE *m_file;
};

#include <RWStreams.h>

//============================================================================
//============================================================================
//============================================================================

ReadStream::ReadStream()
{
	// autorelease();
}

ReadStream::ReadStream(const void *buf, int size)
{
	// autorelease();
	this->buf = (char*)buf;
	this->size = size;
	pos = 0;
	failed = false;
}

bool ReadStream::IsFailed() const
{
	return failed;
}

void ReadStream::ReadInternal(void * buf, int n)
{
	if(failed || pos + n > size){
		failed = true;
		memset(buf, 0, n);
		return;
	}
	memcpy(buf, this->buf+pos, n);
	pos += n;
}

XString ReadStream::ReadString()
{
	int len = ReadShort();
	XString s;
	s.SetLen(len, 0);
	for(int i = 0; i < len; i++)
	{
		s.SetChar(i, (TCHAR)ReadShort());
	}
	return s;
}

float ReadStream::ReadFloat()
{
	int32 value;
	ASSERT(sizeof(float) == sizeof(value));
	value = ReadInt32();
	return *(float*)&value;
}

int ReadStream::ReadInt32()
{
	int a = ReadByte() << 0;
	int b = ReadByte() << 8;
	int c = ReadByte() << 16;
	int d = ReadByte() << 24;
	return a | b | c | d;
}

int ReadStream::ReadInt()
{
	return ReadInt32();
}

uint8 ReadStream::ReadByte()
{
	uint8 val;
	ReadInternal(&val, sizeof(val));
	return val;
}

int8 ReadStream::ReadSignedByte()
{
	return ReadByte();
}

int16 ReadStream::ReadShort()
{
	int a = ReadByte() << 0;
	int b = ReadByte() << 8;
	return a | b;
}

//============================================================================
//============================================================================
//============================================================================

WriteStream::WriteStream()
{
	buf = NULL;
	bufSize = 0;
	dataSize = 0;
	failed = false;
}

WriteStream::~WriteStream()
{
	DDELETEARR(buf);
}

bool WriteStream::ReallocBuf(int newBufSize)
{
	if(failed)
	{
		return false;
	}
	newBufSize = (newBufSize + DATA_ALIGN_SIZE-1) & ~(DATA_ALIGN_SIZE-1);
	if(newBufSize != bufSize)
	{
		char * newBuf = DNEW char[newBufSize];
		ASSERT(newBuf);
		if(!newBuf)
		{
			failed = true;
			return false;
		}
		memcpy(newBuf, buf, sizeof(buf[0]) * bufSize);
		DDELETEARR(buf);
		buf = newBuf;
		bufSize = newBufSize;
	}
	return true;
}

void WriteStream::WriteInternal(const void *pBuf, int n)
{
	if(ReallocBuf(dataSize + (int)n))
	{
		memcpy(buf + dataSize, pBuf, n);
		dataSize += (int)n;
	}
}

bool WriteStream::WriteAnsi(const char * value)
{
	WriteInternal(value, strlen(value));
	return !failed;
}

bool WriteStream::WriteString(const XString& value)
{
	int len = value.Len();
	WriteShort((int16)len);
	for(int i = 0; i < len; i++)
	{
		WriteShort((int16)value.Char(i));
	}
	return !failed;
}

bool WriteStream::WriteFloat(float value)
{
	ASSERT(sizeof(float) == sizeof(value));
	WriteInt32(*(int32*)&value);
	return !failed;
}

bool WriteStream::WriteInt32(int value)
{
	WriteByte((uint8)(value >> 0));
	WriteByte((uint8)(value >> 8));
	WriteByte((uint8)(value >> 16));
	WriteByte((uint8)(value >> 24));
	return !failed;
}

bool WriteStream::WriteInt(int value)
{
	return WriteInt32(value);
}

bool WriteStream::WriteByte(int value)
{
	ASSERT(value >= 0 && value <= 0xff);
	WriteInternal(&value, 1);
	return !failed;
}

bool WriteStream::WriteSignedByte(int value)
{
	ASSERT(value >= (signed char)0x80 && value <= 0x7f);
	WriteInternal(&value, 1);
	return !failed;
}

bool WriteStream::WriteShort(int value)
{
	ASSERT(value >= (short)0x8000 && value <= 0x7fff);
	WriteByte((uint8)(value >> 0));
	WriteByte((uint8)(value >> 8));
	return !failed;
}

//============================================================================
//============================================================================
//============================================================================

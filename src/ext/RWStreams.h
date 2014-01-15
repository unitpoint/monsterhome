#ifndef __RW_STREAMS_H__
#define __RW_STREAMS_H__

#include <ExtCommon.h>
#include <xString.h>

class ReadStream: public cocos2d::CCObject
{
protected:

	char * buf;
	int size;
	int pos;
	bool failed;

	ReadStream();

	void ReadInternal(void * buf, int n);

public:

	ReadStream(const void *buf, int size);

	bool IsFailed() const;

	XString ReadString();
	float ReadFloat();
	int ReadInt32();
	int ReadInt();
	uint8 ReadByte();
	int8 ReadSignedByte();
	int16 ReadShort();
};

class WriteStream: public cocos2d::CCObject
{
protected:

	enum
	{
		DATA_ALIGN_SIZE = 64
	};

	char * buf;
	int bufSize;
	int dataSize;
	bool failed;

	bool ReallocBuf(int newBufSize);
	void WriteInternal(const void * buf, int n);

public:

	WriteStream();
	virtual ~WriteStream();

	const void * Buf() const { return buf; }
	int DataSize() const { return dataSize; }
	bool IsFailed() const { return failed; }

	bool WriteAnsi(const char * value);
	bool WriteString(const XString& value);
	bool WriteFloat(float);
	bool WriteInt32(int);
	bool WriteInt(int);
	bool WriteByte(int);
	bool WriteSignedByte(int);
	bool WriteShort(int);
};

#endif // __RW_STREAMS_H__

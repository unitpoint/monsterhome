#ifndef __RW_STREAMS_H__
#define __RW_STREAMS_H__

/******************************************************************************
* Copyright (C) 2012 Evgeniy Golovin (evgeniy.golovin@unitpoint.ru)
*
* Latest source code: https://github.com/unitpoint/monsterhome
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

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

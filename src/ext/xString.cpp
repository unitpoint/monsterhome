//============================================================================
//
// Author: Evgeniy Golovin, egolovin@superscape.com
//
//============================================================================

// #include "AppHeapManager.h"
#include "xString.h"

#ifdef __COCOS2D_H__
#include <CCObject.h>
#endif

// #include <CTextParser.h>

#define X_VA_BUF_SIZE (10*1024)

// =============================================

XString::Data * XString::Data::Alloc(int size, int pad)
{
	IW_CALLSTACK("XString::Data::Alloc");
#ifndef USE_APP_HEAP_MANAGER
	pad = 0;
#endif
	// TRACE(("[xString::Data::Alloc] size: %d, pad: %d", size, pad));
	int buf_size = size + pad + sizeof(TCHAR) + sizeof(TCHAR)/2;
	Data * d = (Data*)DNEW char[sizeof(Data) + buf_size];
	if(d)
	{
		d->numRefs = 1;
		d->size = size;
		// d->TerminateData();
		__MEMSET(d->ToChar(), 0, buf_size);
	}
	return d;
}

void XString::Data::Release()
{
	if(!--numRefs)
		DDELETEARR((char*)this);
}

void XString::Data::TerminateData()
{
	char * s = (char*)ToChar();
	if(sizeof(TCHAR) == 1)
	{
		s[size] = 0;
	}
	else
	{
		if(size&1)
		{
			s[size] = 0;
			*(TCHAR*)(s+size+1) = 0;
		}
		else
			*(TCHAR*)(s+size) = 0;
	}
}

// =============================================

#ifndef USE_EXT_VERSION
struct __str_start__
{
	__str_start__()
	{ 
		XString::Init();
	}
} __str_start__;
#endif

// =============================================
#define USE_ALLOC_NULL_STRING

void XString::Init()
{
	IW_CALLSTACK("XString::Init");

#ifndef XSTRING_ALLOC_ALWAYS

	DDD(("[XString::Init] begin"));
	XSTR_DEBUG_INFO();

#ifdef USE_ALLOC_NULL_STRING
	pEmpty = DNEW XString(_T("1"));
	XString::Data * d = pEmpty->ToStringData();
	d->size = 0;
	d->ToChar()[0] = 0;
#else
	static Data emptyStringData[2];
	static int emptyString[(sizeof(XString)+sizeof(int)-1)/sizeof(int)];

	// __MEMSET(emptyStringData, sizeof(emptyStringData));

	Data * d = (Data*)&emptyStringData;
	d->numRefs = 2;
	d->size = 0;

	XString& s = *(XString*)&emptyString;
	s.str = d->ToChar();
	s.str[0] = 0;
	pEmpty = &s;
	//*(XString**)&pEmpty = &s;
#endif

	DDD(("[XString::Init] PEmptyString: %x", PEmptyString()));
	DDD(("[XString::Init] PEmptyString: %x", pEmpty));
	//  DDD(("[XString::Init] pEmpty: %x", pEmpty));

#endif // XSTRING_ALLOC_ALWAYS

#ifdef USE_EXT_VERSION
	#ifdef __COCOS2D_H__
	cocos2d::CCSharedFinalizer::atexit(XString::Shutdown);
	#endif
#else
	atexit(Shutdown);
#endif
}

#ifdef USE_EXT_VERSION
void XString::Shutdown()
#else
void __cdecl XString::Shutdown()
#endif
{
#ifndef XSTRING_ALLOC_ALWAYS
	if(PEmptyString())
	{
		Data * d = PEmptyString()->ToStringData();
#ifdef USE_ALLOC_NULL_STRING
		ASSERT(d->numRefs == 1);
		DDELETE(PEmptyString());
#else
		ASSERT(d->numRefs == 2);
		// d->numRefs = 1;
		// DDELETE(PEmptyString());
		//*((XString**)(&pEmpty)) = NULL;
#endif
		pEmpty = 0;
	}
#endif // XSTRING_ALLOC_ALWAYS
}

// =============================================

XString::XString(char a)
{
#ifdef USE_EXT_VERSION
	char str[] = {a, 0};
	Init(str, 1);
#else
	return Format(_T("%c"), a);
#endif
}

XString::XString(int a)
{
#ifdef USE_EXT_VERSION
	TCHAR buf[24];
	__WSPRINTF(buf, sizeof(buf), (const TCHAR*)_T("%i"), a);
	Init(buf);
#else
	return Format(_T("%i"), a);
#endif
}

XString::XString(int64 a)
{
#ifdef USE_EXT_VERSION
	TCHAR buf[128];
	__WSPRINTF(buf, sizeof(buf), (const TCHAR*)_T("%li"), a);
	Init(buf);
#else
	return Format(_T("%li"), a);
#endif
}

XString::XString(float a, int precision)
{
	TCHAR s[64];
	ToString(s, a, precision);
	Init(s);
}

// =============================================

void XString::Init(int size)
{
#ifndef XSTRING_ALLOC_ALWAYS
	if(size <= 0)
	{
		Init(*PEmptyString());
		return;
	}
#endif
	IW_CALLSTACK("XString::Init(int size)");
	Data * d = Data::Alloc(size, 0);
	ASSERT(d);
#ifndef XSTRING_ALLOC_ALWAYS
	if(!d)
	{
		Init(*PEmptyString());
		return;
	}
#endif
	str = d->ToChar();
}

void XString::Init(const void * p, int size)
{
#ifndef XSTRING_ALLOC_ALWAYS
	if(size <= 0)
	{
		Init(*PEmptyString());
		return;
	}
#endif
	IW_CALLSTACK("XString::Init(const void * p, int size)");
	Data * d = Data::Alloc(size, 0);
	ASSERT(d);
#ifndef XSTRING_ALLOC_ALWAYS
	if(!d)
	{
		Init(*PEmptyString());
		return;
	}
#endif
	str = d->ToChar();
	__MEMCPY(str, p, size);
	// d->TerminateData(); alloc - fill mem to zero
}

void XString::Init(const void * p1, int size1, const void * p2, int size2)
{
	if(size1 <= 0) {
		Init(p2, size2);
		return;
	}
	if(size2 <= 0) {
		Init(p1, size1);
		return;
	}
	IW_CALLSTACK("XString::Init(const void * p1, int size1, const void * p2, int size2)");
	int newSize = size1 + size2;
	Data * d = Data::Alloc(newSize, (size1 + size2) / 4);
	ASSERT(d);
#ifndef XSTRING_ALLOC_ALWAYS
	if(!d) {
		Init(*PEmptyString());
		return;
	}
#endif
	str = d->ToChar();
	__MEMCPY(str, p1, size1);
	__MEMCPY((char*)str + size1, p2, size2);
	// d->TerminateData();
}

void XString::Init(const void * p1, int size1, 
	const void * p2, int size2, 
	const void * p3, int size3)
{
	if(size1 <= 0)
	{
		Init(p2, size2, p3, size3);
		return;
	}
	if(size2 <= 0)
	{
		Init(p1, size1, p3, size3);
		return;
	}
	if(size3 <= 0)
	{
		Init(p1, size1, p2, size2);
		return;
	}
	IW_CALLSTACK("XString::Init(const void * p1, int size1, const void * p2, int size2, const void * p3, int size3)");
	int newSize = size1 + size2 + size3;
	Data * d = Data::Alloc(newSize, (size1 + size2 + size3) / 4);
	ASSERT(d);
#ifndef XSTRING_ALLOC_ALWAYS
	if(!d)
	{
		Init(*PEmptyString());
		return;
	}
#endif
	str = d->ToChar();
	__MEMCPY(str, p1, size1);
	__MEMCPY((char*)str + size1, p2, size2);
	__MEMCPY((char*)str + size1 + size2, p3, size3);
	// d->TerminateData();
}

#ifndef ONE_BYTE_WCHAR
void XString::Init(const char * s)
{
#if 1
	Init(s, __STRLEN(s));
#else
	if(!s[len])
	{
		Init(s);
		return;
	}
	char * str = DNEWARR(char, len+1);
	if(!str)
	{
		Init(*PEmptyString());
		return;
	}
	__MEMCPY(str, s, len);
	str[len] = 0;
	Init(str);
	DDELETEARR(str);
#endif
}

void XString::Init(const char * s, int len)
{
#ifdef ONE_BYTE_WCHAR
	// if(sizeof(char) == sizeof(TCHAR)){
	Init((const TCHAR*)s, len);
	return;
	// }
#else
	int size = len * sizeof(TCHAR);
	Data * d = Data::Alloc(size, 0);
	if(!d) {
		Init(*PEmptyString());
		return;
	}
	str = d->ToChar();
#if 1
	__STRLENTOWSTR(str, s, len);
#else
#ifdef USE_EXT_VERSION
	ICStdUtil::StrToWcs((wchar*)str, s);
#else
	MultiByteToWideChar(CP_ACP, 0, (CHAR*)s, -1, (WCHAR*)str, (unsigned)size / (unsigned)sizeof(TCHAR));
#endif
#endif
#endif // ONE_BYTE_WCHAR
}
#endif // ONE_BYTE_WCHAR

XString& XString::SetSize(int size, int pad, bool keepData)
{
#ifndef XSTRING_ALLOC_ALWAYS
	if(size <= 0 && pad <= 0)
	{
		Assign(*PEmptyString());
		return *this;
	}
#endif
	Data * d2, * d = ToStringData(); 
	if(d->numRefs == 1)
	{
#ifdef USE_APP_HEAP_MANAGER
		int curMaxSize = AppHeapManager::Instance()->Size(d) - sizeof(Data) - sizeof(TCHAR);
#else
		int curMaxSize = d->size;
#endif
		if(curMaxSize >= size) {
			int ZeroFrom = size < d->size ? size : d->size;
			__MEMSET((char*)str + ZeroFrom, 0, curMaxSize - ZeroFrom + sizeof(TCHAR));
			d->size = size;
			return *this;
		}
	}
	if(!keepData)
	{
		d->Release();
		IW_CALLSTACK("XString::SetSize !keepData");
		d2 = Data::Alloc(size, pad);
		ASSERT(d2);
#ifndef XSTRING_ALLOC_ALWAYS
		if(!d2)
		{
			Init(*PEmptyString());
			return *this;
		}
#endif
		str = d2->ToChar();
		return *this;
	}

	IW_CALLSTACK("XString::SetSize");
	d2 = Data::Alloc(size, pad);
	ASSERT(d2);
#ifndef XSTRING_ALLOC_ALWAYS
	if(!d2)
	{
		d->Release();
		Init(*PEmptyString());
		return *this;
	}
#endif
	str = d2->ToChar();
	int copySize = d->size < size ? d->size : size;
	__MEMCPY(str, d->ToChar(), copySize);
	d->Release();
	return *this;
}

#if 0
Vector<XString>& XString::Split(Vector<XString>& out, int width, CFont * font) const
{
	out.setSize(0);
	out.trimToSize();

	const TCHAR * s = ToChar();

	CTextParser parser;
	parser.SetFont(font);
	parser.SetText((const wchar*)s);
	parser.SetWidth(width);
	parser.Parse();

	int numLines = parser.GetNumLines();
	for(int i = 0; i < numLines; i++)
	{
		int start = parser.GetIndexForLine(i);
		int numChars = parser.GetNumCharsForLine(i);
		out.addElement(XString(s + start, numChars));
	}
	return out;
}
#endif

Vector<XString>& XString::Split(Vector<XString>& out, int width) const
{
	out.clear();

	int len = Len();
	int i = 0, lineStart = 0, word_end = 0;
	int cur_width = 0;
	const TCHAR * s = ToChar();
	for(; i < len; i++)
	{
		if(s[i] == _T('\r') || s[i] == _T('\n'))
		{
			out.push_back(XString(s+lineStart, i-lineStart).Trim());
			if(s[i+1] == _T("\r\n")[s[i] == L'\r']) // i+1 is always valid, because data is null terminated
			{
				i++;
			}
			word_end = lineStart = i+1;
			cur_width = 0;
		}else if(++cur_width > width && word_end > lineStart){
			out.push_back(XString(s+lineStart, word_end-lineStart).Trim());
			word_end = lineStart = i+1;
			cur_width = 0;
		}else if(s[i] <= _T(' ')){
			word_end = i;
		}
	}
	if(i > lineStart)
	{
		out.push_back(SubString(lineStart, i-lineStart).Trim()); // XString(s+lineStart, i-lineStart).Trim());
	}
	return out;
}

Vector<XString>& XString::SplitLines(Vector<XString>& out) const
{
	out.clear();

	int len = Len();
	int i = 0, lineStart = 0;
	const TCHAR * s = ToChar();
	for(; s[i] && i < len; i++)
	{
		if(s[i] == L'\r' || s[i] == L'\n')
		{
			out.push_back(XString(s+lineStart, i-lineStart).Trim());
			if(s[i+1] == _T("\r\n")[s[i] == L'\r']) // i+1 is always valid, because data is null terminated
			{
				i++;
			}
			lineStart = i+1;
		}
	}
	if(i > lineStart)
	{
		out.push_back(SubString(lineStart, i-lineStart).Trim()); // XString(s+lineStart, i-lineStart).Trim());
	}
	return out;
}

Vector<XString>& XString::SplitWords(Vector<XString>& out) const
{
	out.clear();

	int len = Len();
	int i = 0, lineStart = 0;
	const TCHAR * s = ToChar();
	for(; s[i] && i < len; i++)
	{
		if(s[i] <= L' ')
		{
			XString str = XString(s+lineStart, i-lineStart).Trim();
			if(!str.IsEmpty())
			{
				out.push_back(str);
			}
			while(++i < len && s[i] <= L' '){}
			lineStart = i;
		}
	}
	if(i > lineStart)
	{
		XString str = SubString(lineStart, i-lineStart).Trim();
		if(!str.IsEmpty())
		{
			out.push_back(str);
		}
	}
	return out;
}

Vector<XString>& XString::Split(Vector<XString>& out, const TCHAR * separator, bool trim) const
{ 
#if 1
	out.clear();

	int sepLen = __WSTRLEN(separator);
	unsigned sepSize = (unsigned)sepLen * (unsigned)sizeof(TCHAR);

	int endPos = Len() - sepLen;
	int i = 0, lineStart = 0;
	const TCHAR * s = ToChar();
	for(; i < endPos; i++)
	{
		if(__MEMCMP(s+i, separator, sepSize) == 0)
		{
			XString str = XString(s+lineStart, i-lineStart);
			if(trim)
			{
				str = str.Trim();
				if(!str.IsEmpty())
				{
					out.push_back(str);
				}
				i += sepLen;
				while(i < endPos && s[i] <= L' '){}
			}
			else
			{
				out.push_back(str);
				i += sepLen;
			}
			lineStart = i--;
		}
	}
	if(i > lineStart)
	{
		XString str = SubString(lineStart, i-lineStart);
		if(trim)
		{
			str = str.Trim();
			if(!str.IsEmpty())
			{
				out.push_back(str);
			}
		}
		else
		{
			out.push_back(str);
		}
	}
	return out;
#else
	int s_pos = Find( separator, 0);
	if( s_pos == -1 )
	{    
		out.addElement( *this );    
	}
	else
	{ 
		if( s_pos != 0 )
		{
			out.addElement( SubString( 0, s_pos ) );
		}
		SubString( s_pos + 1, Len() ).Split( out, separator );        
	}
#endif
}

XString& XString::SetFormat(const TCHAR * fmt, va_list va)
{
#ifdef USE_EXT_VERSION
	Assign((const void*)NULL, 0); // release
	TCHAR * buf = DNEW TCHAR[X_VA_BUF_SIZE];
	if(buf)
	{
		__VSWPRINTF((TCHAR*)buf, (TCHAR*)fmt, va);
		Assign(buf);
		DDELETEARR(buf);
	}

	return *this;
#else
	TCHAR buf[X_VA_BUF_SIZE];
	_vstprintf(buf,fmt,va);
	return Assign(buf);
#endif
}

XString& XString::Separate()
{
	Data * d = ToStringData();
	if(d->numRefs == 1) 
		return *this;

	return Assign((void*)str, d->size);
}
XString XString::Clone() const
{
	return XString((const void*)str, Size());
}

int XString::FindData(const void * p, int p_size, int p_start) const
{
	const char * mem = (const char *)ToMemory() + p_start;
	int end = Size() - p_size;
	for(int i = p_start; i <= end; i++, mem++)
	{
		if(__MEMCMP(mem, p, p_size) == 0)
		{
			return i;
		}
	}
	return -1;
}

int XString::Find(const TCHAR * findStr, int start) const
{
	const TCHAR * str = ToChar() + start;
	int findLen = __WSTRLEN(findStr);
	unsigned findSize = (unsigned)findLen * (unsigned)sizeof(TCHAR);
	for(int i = start, end = Len() - findLen; i < end; i++, str++)
	{
		if(__MEMCMP(str, findStr, findSize) == 0)
		{
			return i;
		}
	}
	return -1;
}

int XString::Find(const XString& a, int start) const
{
	const TCHAR * str = ToChar() + start;
	const TCHAR * findStr = a.ToChar();
	int findLen = a.Len();
	unsigned findSize = (unsigned)a.Size();
	for(int i = start, end = Len() - findLen; i < end; i++, str++)
	{
		if(__MEMCMP(str, findStr, findSize) == 0)
		{
			return i;
		}
	}
	return -1;
}

XString XString::Replace(const TCHAR * p_find, const TCHAR * p_replace, bool all) const
{
	int findLen = __WSTRLEN(p_find);
	int findSize = findLen * sizeof(TCHAR);

	int replaceLen = __WSTRLEN(p_replace);
	int replaceSize = replaceLen * sizeof(TCHAR);

	XString tmp = *this;
	for(int start = 0;;)
	{
		int pos = tmp.FindData(p_find, findSize, start);
		if(pos >= 0)
		{
			if(findSize != replaceSize)
			{
				const char * s = (const char*)tmp.ToMemory();
				tmp = XString(s, pos, p_replace, replaceSize,
					s + pos + findSize, tmp.Size() - (pos + findSize));
			}
			else
			{
				tmp.Separate();
				char * s = (char*)tmp.ToMemory();
				__MEMCPY(s + pos, p_replace, replaceSize);
			}
			if(!all)
			{
				break;
			}
			start = pos + replaceSize;
		}
		else
		{
			break;
		}
	}
	return tmp;
}

TCHAR XString::Char(int i) const 
{ 
	return i >= 0 && i < Len() ? str[i] : 0;
}
XString& XString::SetChar(int i, TCHAR c)
{ 
	// Data * d = ToStringData();
	if(i >= 0 && i < Len()) {
		Separate(); 
		str[i] = c; 
	}
	return *this;
}

XString& XString::Assign(const XString& a)
{
	Data * d = ToStringData();
	Init(a);
	d->Release();
	return *this;
}
XString& XString::Assign(const void * p, int size)
{
	Data * d = ToStringData();
	Init(p, size);
	d->Release();
	return *this;
}
XString& XString::Assign(const void * p1, int size1, const void * p2, int size2)
{
	Data * d = ToStringData();
	Init(p1, size1, p2, size2);
	d->Release();
	return *this;
}

XString& XString::InsertData(int i, const XString& a)
{
	return InsertData(i, a.str, a.Size());
}
XString& XString::InsertData(int i, const void * p, int size)
{
	Data * d = ToStringData();
	if(i < 0 || i > d->size)
		return *this;

	Init((const void*)str, i, p, size, (char*)str + i, d->size-i);
	d->Release();
	return *this;
}

XString XString::SubString(int start,int len) const
{
	return SubData((unsigned)start * (unsigned)sizeof(TCHAR),
		(unsigned)len * (unsigned)sizeof(TCHAR));
}
XString XString::SubData(int start, int size) const
{
	Data * d = ((XString*)this)->ToStringData();
	if(start < 0)
	{
		start = d->size + start;
		if(start < 0)
			start = 0;
	}
	if(start >= d->size)
		return XString();

	if(size < 0)
	{
		size = d->size - start + size;
		if(size < 0)
			return XString();
	}
	if(start + size > d->size)
		size = d->size - start;

	if(!start && size == d->size)
		return XString(*this);

	return XString((const void*)((char*)str + start), size);
}

XString& XString::Append(const XString& a)
{
	return Append((void*)a.str, a.Size());
}
XString& XString::Append(const void * p, int size)
{
	if(size > 0)
	{
		int oldSize = this->Size();
		int newSize = oldSize + size;
		SetSize(newSize, newSize / 2, true);
		__MEMCPY((char*)str + oldSize, p, size);
	}
	return *this;
}
XString& XString::Append(const void * p1, int size1, const void * p2, int size2)
{
	if(size1 <= 0)
		return Append(p2, size2);

	if(size2 <= 0)
		return Append(p1, size1);

	int oldSize = this->Size();
	int newSize = oldSize + size1 + size2;
	SetSize(newSize, newSize / 2, true);
	__MEMCPY((char*)str + oldSize, p1, size1);
	__MEMCPY((char*)str + oldSize + size1, p2, size2);
	return *this;
}

XString XString::Lower() const
{
	XString str = Clone();
	TCHAR * s = str.str;
#if defined(USE_EXT_VERSION) && defined(__WSTRLOWER)
	__WSTRLOWER(s);
#else
	for(int len = str.Len(); len > 0; len--, s++)
		*s = tolower(*s);
#endif
	return str;
}
XString XString::Upper() const
{
	XString str = Clone();
	TCHAR * s = str.str;
#if defined(USE_EXT_VERSION) && defined(__WSTRUPPER)
	__WSTRUPPER(s);
#else
	for(int len = str.Len(); len > 0; len--, s++)
		*s = toupper(*s);
#endif 
	return str;
}
XString XString::Flower() const
{
	if(!Size())
		return XString();

	XString str = Clone();
	TCHAR * s = str.str;

#if defined(USE_EXT_VERSION) && defined(__WSTRUPPER)
	if(Len() > 0)
	{
		TCHAR c = s[1];
		s[1] = 0;
		__WSTRUPPER(s);
		s[1] = c;
		__WSTRLOWER(s+1);
	}
#else
	*s = toupper(*s);
	int len = str.Len()-1;
	for(s++; len > 0; len--, s++)
		*s = tolower(*s);
#endif
	return str;
}

XString XString::Trim(bool leftTrim, bool rightTrim) const
{
	const TCHAR * start = ToChar();
	const TCHAR * end = start + Len();
	bool realSub = false;
	if(leftTrim)
#ifdef USE_EXT_VERSION
		while(end > start && *start <= L' ') {
#else
		while(end > start && _istspace(*start)) {
#endif
			start++;
			realSub = true;
		}

		if(rightTrim)
#ifdef USE_EXT_VERSION
			while(end > start && end[-1] <= L' ') {
#else
			while(end > start && _istspace(end[-1])) {
#endif
				end--;
				realSub = true;
			}

			return realSub ? XString((void*)start, (int)end - (int)start) : *this;
}

int XString::Cmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n)
{
	return CmpData((const void*)a, (unsigned)aLen * sizeof(TCHAR),
		(const void*)b, (unsigned)bLen * sizeof(TCHAR),
		(unsigned)n * sizeof(TCHAR));
}
int XString::Icmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n)
{
	int len = aLen < bLen ? aLen : bLen;
	if(len > n)
		len = n;
	//else if(n > len)
	// n = len;

	TCHAR aBuf[2], bBuf[2];
	aBuf[1] = bBuf[1] = 0;

	for(int k, i = 0; i < len; i++) {
#if defined(USE_EXT_VERSION) && defined(__WSTRLOWER)
		aBuf[0] = *a++;
		bBuf[0] = *b++;
		__WSTRLOWER(aBuf);
		__WSTRLOWER(bBuf);
		k = aBuf[0] - bBuf[0];
#else
		k = tolower(*a++) - tolower(*b++);
#endif
		if(k)
			return k;
	}
	return len == n || aLen == bLen ? 0 : (aLen < bLen ? -1 : 1);
}

int XString::Cmp(const TCHAR * b, int n) const 
{
	return CmpData((const void*)str, Size(), (const void*)b, (int)__WSTRLEN(b) * sizeof(TCHAR),
		(unsigned)n * sizeof(TCHAR));
}
int XString::Icmp(const TCHAR * b, int n) const 
{
	return Icmp(str, Len(), b, (int)__WSTRLEN(b), n);
}

int XString::Cmp(const XString& b, int n) const 
{
	return CmpData((const void*)str, Size(), (const void*)b.str, b.Size(),
		(unsigned)n * sizeof(TCHAR));
}
int XString::Icmp(const XString& b, int n) const 
{
	return Icmp(str, Len(), b.str, b.Len(), n);
}

int XString::CmpData(const void * a, int aSize, const void * b, int bSize, int n)
{
	int size = aSize < bSize ? aSize : bSize;
	if(size > n)
		size = n;
	//else if(n > size)
	// n = size;
	/* int maxSize = aSize > bSize ? aSize : bSize;
	if(n > maxSize)
	n = maxSize; */

	int k = __MEMCMP(a, b, size);
	if(k || size == n)
		return k;

	if(aSize < bSize) return -1;
	if(aSize > bSize) return 1;
	return 0;
}

XString XString::ToString(int a)
{
	return XString(a);
}

XString XString::ToString(char a)
{
	return XString(a);
}

#if 0 && defined USE_EXT_VERSION
TCHAR * XString::ToString(TCHAR * dst, float a, int precision)
{
	if(precision <= 0)
	{
		if(precision < 0)
		{
			int p = 10;
			precision = -precision;
			if(precision > 5) precision = 5;
			for(int i = 1; i < precision; i++)
			{
				p *= 10;
			}
			__WSPRINTF(dst, sizeof(TCHAR)*32, (const TCHAR*)_T("%d"), int(a / p + 0.5f) * p);
			return dst;
		}
		__WSPRINTF(dst, sizeof(TCHAR)*32, (const TCHAR*)_T("%d"), int(a + 0.5f));
		return dst;
	}
	int p = 10;
	if(precision > 5) precision = 5;
	for(int j = 1; j < precision; j++)
	{
		p *= 10;
	}
	TCHAR maskBuf[32] = _T("-%d.%0");
	if(precision <= 9)
	{
		TCHAR * pos = maskBuf + __WSTRLEN(maskBuf);
		*pos++ = _T('0') + precision;
		*pos++ = _T('d');
		*pos++ = 0;
	}
	else
	{
		// __WSTRCPY(maskBuf, _T("-%d.%0"));
		__WSPRINTF(maskBuf + __WSTRLEN(maskBuf), sizeof(maskBuf), _T("%dd"), precision);
	}
	TCHAR * mask = maskBuf;
	if(a < 0)
	{
		a = -a;
	}
	else
	{
		mask++;
	}
	int val = int(a * p + 0.5f);
	int i = val / p; // int(a);
	int f = val % p; // int((a - (float)i) * p + 0.5f);

	// TCHAR dst[32];
	__WSPRINTF(dst, sizeof(TCHAR)*32, mask, i, f);
	if(precision > 0){
		int dst_len = __WSTRLEN(dst);
		while(dst_len >= 3 && dst[dst_len-1] == _T('0')){
			dst[--dst_len] = 0;
		}
		if(dst_len > 1 && dst[dst_len-1] == _T('.')){
			dst[--dst_len] = 0;
		}
	}
	return dst;
}

#else

TCHAR * XString::ToString(TCHAR * dst, float a, int precision)
{
	TCHAR fmt[32];
	if(precision <= 0) {
		if(precision < 0) {
			float p = powf(10.0, -precision);
			a = floorf(a / p + 0.5f) * p;
		}
		__WSPRINTF(dst, sizeof(fmt), _T("%.f"), a);
		return dst;
	}
	__WSPRINTF(fmt, sizeof(fmt), _T("%%.%df"), precision);
	int n = __WSPRINTF(dst, sizeof(fmt), fmt, a);

	while(n > 0 && dst[n-1] == '0') dst[--n] = (TCHAR)0;
	if(n > 0 && dst[n-1] == '.') dst[--n] = (TCHAR)0;

	return dst;
}
#endif

#if 0
XString XString::FloatArrayToString(const float * a, int Count, int precision)
{
	if(Count <= 0)
		return XString();

	TCHAR s[256];
	XString r = ToString(*a++, precision);
	for(Count--; Count > 0; Count--) {
		ToString(s, *a++, precision);
		r.Append(_T(" "), sizeof(TCHAR), s, (unsigned)__WSTRLEN(s) * (unsigned)sizeof(TCHAR));
	}
	return r; 
}
#endif

XString XString::ToString(float a, int precision)
{
	return XString(a, precision);
	// TCHAR s[64];
	// return ToString(s, a, precision);
}

bool XString::ParseDecSimple(const TCHAR *& str, int& val)
{
	int intValue = 0;
	const TCHAR * start = str;
	for(; *str >= _T('0') && *str <= _T('9'); str++)
	{
		intValue = intValue * 10 + (int8)(*str - _T('0'));
	}
	val = intValue;
	return str > start;
}

XString::ENumType XString::ParseNum(const TCHAR *& str, double& val) // , bool parseEndSpaces)
{
	const TCHAR * saveSrc = str;
	int sign = 1;
	if(*str == _T('-')){
		str++;
		saveSrc++;
		sign = -1;
	}

	int intValue = 0;
	bool escapeMode = false;
	/* 
	if(str[0] == _T('0') && str[1] != _T('.'))
	{
	bool octalNum = false;
	if(str[1] == _T('x') || str[1] == _T('X')) // parse hex
	{
	str += 2;
	ParseHexSimple(str, intValue);
	}
	else if(str[1] == _T('b') || str[1] == _T('B')) // parse hex
	{
	str += 2;
	ParseBinSimple(str, intValue);
	}
	else // parse octal
	{
	octalNum = true;
	ParseOctalSimple(str, intValue);
	}
	if(saveSrc+1 == str && !octalNum)
	{
	val = 0;
	return NUM_ERROR;
	}
	}
	else
	*/
	{
		// parse int or float
		ParseDecSimple(str, intValue);

		if(*str == _T('.')) // parse float
		{
			/*
			// parse 1.#INF ...
			if(sign == 1 && saveSrc+1 == str && *saveSrc == _T('1') && str[1] == _T('#'))
			{
			const TCHAR * spec[] = {_T("INF"), _T("IND"), _T("QNAN"), NULL};
			int i = 0;
			for(; spec[i]; i++)
			{
			if(_tcscmp(str, spec[i]) != 0)
			continue;

			size_t specLen = _tcslen(spec[i]);
			str += specLen;
			if(!*str || _istspace(*str) || _tcschr(_T("!@#$%^&*()-+={}[]\\|;:'\",<.>/?`~"), *str))
			{
			dword specVal;
			switch(i)
			{
			case 0:
			specVal = 0x7f800000;
			break;

			case 1:
			specVal = 0xffc00000;
			break;

			case 2:
			specVal = 0x7fc00000;
			break;

			default:
			ASSERT(false);
			}
			val = (double)*(float*)&specVal;
			if(parseEndSpaces)
			{
			ParseSpaces(str);
			}
			return NUM_FLOAT;
			}            
			}
			val = 0;
			return NUM_ERROR;
			}
			*/

			double floatValue = intValue, m = 0.1;
			ASSERT((int)floatValue == intValue);
			for(str++; *str >= _T('0') && *str <= _T('9'); str++, m *= 0.1)
			{
				floatValue += (int8)(*str - _T('0')) * m;
			}
			if(saveSrc == str)
			{
				val = 0;
				return NUM_ERROR;
			}
			if(*str == _T('e') || *str == _T('E'))
			{
				str++;
				bool div = false; // + for default
				if(*str == _T('-'))
				{
					div = true;
					str++;
				}
				else if(*str == _T('+'))
				{
					// div = false;
					str++;
				}
				int pow;
				ParseDecSimple(str, pow);
				m = 1.0;
				for(int i = 0; i < pow; i++)
				{
					m *= 10.0;
				}
				if(div)
				{
					floatValue /= m;
				}
				else
				{
					floatValue *= m;
				}
			}
			val = floatValue * sign;
			/* if(parseEndSpaces)
			{
			ParseSpaces(str);
			} */
			return NUM_FLOAT;
		}
		else if(saveSrc == str)
		{
			val = 0;
			return NUM_ERROR;
		}
	}
	intValue *= sign;
	val = intValue;
	ASSERT((int)val == intValue);
	/* if(parseEndSpaces)
	{
	ParseSpaces(str);
	} */
	return NUM_INT;
}

float XString::ToFloat() const
{
	double val;
	const TCHAR * str = ToChar();
	if(ParseNum(str, val) != NUM_ERROR)
	{
		return (float)val;
	}
	return 0.0f;
}

int XString::ToInt() const
{
	const TCHAR * str = ToChar();
	bool isNeg = str[0] == _T('-');
	if(isNeg)
		str++;

	int intValue = 0;
	ParseDecSimple(str, intValue);
	return isNeg ? -intValue : intValue;
}

int XString::HashData(const void * a, int size)
{
#define KeyHashAddNum KeyNum = ((KeyNum << 5) + KeyNum) + *Key++

	unsigned KeyNum = 5381;
	char * Key = (char*)a;
	for(; size >= 8; size -= 8) {
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
		KeyHashAddNum;
	}
	switch(size) {
	case 7: KeyHashAddNum;
	case 6: KeyHashAddNum;
	case 5: KeyHashAddNum;
	case 4: KeyHashAddNum;
	case 3: KeyHashAddNum;
	case 2: KeyHashAddNum;
	case 1: KeyHashAddNum;
	case 0: break;
	}
	return (int)KeyNum;

#undef KeyHashAddNum
}

#ifdef _DEBUG
void XString::printDebugInfo()
{
#ifndef XSTRING_ALLOC_ALWAYS
	DDD(("XString debug info pEmpty = %x | PEmptyString() = %d", pEmpty, PEmptyString()));
#endif
}
#endif

#ifndef XSTRING_ALLOC_ALWAYS
XString * XString::pEmpty = NULL;
#endif

#ifndef ONE_BYTE_WCHAR
XString::AnsiString::AnsiString(const TCHAR * str)
{
	uint32 len = __WSTRLEN(str);
	ansiStr = DNEWARR(char, len + 1);
	ICStdUtil::WcsToStr(ansiStr, str);
	ansiStr[len] = 0;
}
#endif
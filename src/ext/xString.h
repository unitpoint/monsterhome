#ifndef __X_STRING_H__
#define __X_STRING_H__

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

// #pragma once

// #define XSTRING_ALLOC_ALWAYS

// #if !defined(USE_EXT_VERSION) && (!defined(WIN32) || defined(AEE_SIMULATOR) || defined(PLATFORM_WIN32) || defined(PLATFORM_WINMOBILE))
#define USE_EXT_VERSION
// #endif

#ifdef USE_EXT_VERSION

// #include <StdDefs.h>
// #include <vector_ss.h>
// #include <CFont.h>

#include <ExtCommon.h>
#include <stdio.h>
#include <string.h>

#ifndef TCHAR
//#ifdef PLATFORM_BREW
//#define TCHAR AECHAR
//#else
#define TCHAR char
#define ONE_BYTE_WCHAR
//#endif
#endif // TCHAR

#ifndef _T
#if defined(ONE_BYTE_WCHAR)
#define _T(s) s
#else
#define _T(s) L ## s
#endif // ONE_BYTE_WCHAR
#endif // _T

#ifndef DDD
#define DDD
#endif

#ifndef __WSTRLEN
#define __WSTRLEN strlen
#endif // __WSTRLEN

#ifndef __STRLEN
#define __STRLEN strlen
#endif // __STRLEN

#ifndef __WSTRCPY
#define __WSTRCPY strcpy
#endif // __WSTRCPY

#ifndef __MEMCPY
#define __MEMCPY memcpy
#endif // __MEMCPY

#ifndef __MEMCMP
#define __MEMCMP memcmp
#endif // __MEMCMP

#ifndef __MEMSET
#define __MEMSET memset
#endif // __MEMSET

#ifndef __MEMMOVE
#define __MEMMOVE memmove
#endif // __MEMMOVE

#ifndef __WSTRLOWER
// #define __WSTRLOWER tolower
#endif // __WSTRLOWER

#ifndef __WSTRUPPER
// #define __WSTRUPPER toupper
#endif // __WSTRLOWER

#ifndef __STRLENTOWSTR
#define __STRLENTOWSTR __strlentowstr
/* inline int __strlentowstr( TCHAR *dst, const char *src, int len )
{
return ICStdUtil::StrLToWcs(dst, src, len+1);
} */
#endif // __STRTOWSTR

#ifndef __WSPRINTF
#if 1
#define __WSPRINTF snprintf
#else
#define __WSPRINTF __wsprintf
inline int __wsprintf(TCHAR *s, int size, const TCHAR * fmt, ...)
{
	(void)size;
	va_list va;
	va_start(va, fmt);
	int r = vsnprintf(s, size, fmt, va);
	va_end(va);
	return r;
}
#endif
#endif // __WSPRINTF

#ifndef __VSWPRINTF
#define __VSWPRINTF vsprintf
#endif // __VSWPRINTF

#else

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>

#ifndef _UNICODE
#error _UNICODE must be defined???
#endif

#error Current version is not implemented yet

#endif

/**
* String class implementation.
*/

#ifdef _DEBUG
#define XSTR_DEBUG_INFO() XString::printDebugInfo()
#else
#define XSTR_DEBUG_INFO() (void*)0
#endif

class XString
{
public:
#ifdef _DEBUG
	static void printDebugInfo();
#endif
protected:

#ifndef XSTRING_ALLOC_ALWAYS
	static XString * pEmpty;
#endif

	friend class XRefClass;

	union
	{
		TCHAR * str; ///< unicode string that can be visible from debuger
		char * c_str; ///< ansi string that can be visible from debuger
	};

#ifndef XSTRING_ALLOC_ALWAYS
	/// dont allow cache pEmpty
	static XString * PEmptyString(){ return pEmpty; }
#endif

	/// Internal string header data
	struct Data
	{
		int numRefs; ///< string refs number
		int size; ///< allocated buffer size in bytes

		inline TCHAR * ToChar() { return (TCHAR*)(this + 1); }

		static Data * Alloc(int size, int pad);
		void Release();
		void TerminateData();
	};

	inline Data * ToStringData() { return ((Data*)str)-1; }
	inline const Data * ToStringData() const { return ((Data*)str)-1; }
	inline void Release() { ToStringData()->Release(); }

#ifndef XSTRING_ALLOC_ALWAYS
	inline void Init(const XString& a) { ASSERT(&a); str = a.str; ToStringData()->numRefs++; }
#else
	inline void Init(const XString& a) { ASSERT(&a); Init(a.str, a.Len()); }
#endif

	void Init(const void * p, int size);
	void Init(const void * p1, int size1, const void * p2, int size2);
	void Init(const void * p1, int size1, const void * p2, int size2, const void * p3, int size3);
	inline void Init(const TCHAR * s) { Init((const void*)s, (unsigned)__WSTRLEN(s) * (unsigned)sizeof(TCHAR)); }
	inline void Init(const TCHAR * s, int len) 
	{ 
		Init((const void*)s, (int)((unsigned)len * (unsigned)sizeof(TCHAR))); 
	}
	inline void Init(TCHAR c) { Init((const void*)&c, sizeof(TCHAR)); }

#ifndef ONE_BYTE_WCHAR
	void Init(const char * s);
	void Init(const char * s, int len);
#endif

	void Init(int size);

public:

	// static const int Granularity = 16;

	static void Init();

#ifdef USE_EXT_VERSION
	static void Shutdown();
#else
	static void __cdecl Shutdown();
#endif

	inline operator const TCHAR * () const { return (const TCHAR*)str; }
	// inline operator TCHAR * () const { return (TCHAR*)str; }
	inline const TCHAR * ToChar() const { return (const TCHAR*)str; }
	inline const void * ToMemory() const { return (const void*)str; }
	inline int NumRefs() const { return ToStringData()->numRefs; }
	inline int Size() const { return ToStringData()->size; }
	inline int Len() const { return (unsigned)Size() / (unsigned)sizeof(TCHAR); }

#ifndef XSTRING_ALLOC_ALWAYS
	inline XString() { Init(*pEmpty); }
#else
	inline XString() { Init((void*)NULL, 0); }
#endif

	inline XString(const XString& s) { Init(s); }
	inline XString(const XString& a, const XString& b) 
	{ 
		Init((const void*)a.str, a.Size(), (const void*)b.str, b.Size()); 
	}
	inline XString(const void * p, int size) { Init(p, size); }
	inline XString(const void * p1, int size1, const void * p2, int size2) { Init(p1, size1, p2, size2); }
	inline XString(const void * p1, int size1, const void * p2, int size2, const void * p3, int size3) { Init(p1, size1, p2, size2, p3, size3); }

	inline XString(TCHAR * s) { Init((const TCHAR*)s); }
	inline XString(const TCHAR * s) { Init(s); }

	inline XString(TCHAR * s, int len) { Init((const TCHAR*)s, len); }
	inline XString(const TCHAR * s, int len) { Init(s, len); }

#ifndef ONE_BYTE_WCHAR
	inline XString(char * s) { Init((const char*)s); }
	inline XString(const char * s) { Init(s); }

	inline XString(char * s, int len) { Init((const char*)s, len); }
	inline XString(const char * s, int len) { Init(s, len); }
#endif

	XString(char a);
	XString(int a);
	XString(int64 a);
	XString(float a, int precision = 15);

	inline ~XString() { Release(); }

	inline bool IsEmpty() const { return Size() == 0; }

#ifndef XSTRING_ALLOC_ALWAYS
	inline XString& Clear() { return Assign(*pEmpty); }
#else
	inline XString& Clear() { return Assign((void*)NULL, 0); }
#endif

	XString& SetSize(int size, int pad = 0, bool keepData = false);
	inline XString& SetLen(int len, int pad = 0, bool keepData = false)
	{ 
		return SetSize((unsigned)len * (unsigned)sizeof(TCHAR), 
			(unsigned)pad * (unsigned)sizeof(TCHAR), keepData); 
	}

	Vector<XString>& SplitLines(Vector<XString>& out) const;
	Vector<XString>& SplitWords(Vector<XString>& out) const;
	Vector<XString>& Split(Vector<XString>& out) const;
	Vector<XString>& Split(Vector<XString>& out, int width) const;
	Vector<XString>& Split(Vector<XString>& out, const TCHAR * separator, bool trim = false) const;

	XString& SetFormat(const TCHAR * fmt, va_list va);
	inline XString& SetFormat(TCHAR * fmt, va_list va) { return SetFormat((const TCHAR*)fmt, va); }

	inline XString& SetFormat(const TCHAR * fmt, ...)
	{ 
		va_list va;
		va_start(va,fmt);
		SetFormat(fmt, va);
		va_end(va);
		return *this;
	}
	inline XString& SetFormat(TCHAR * fmt, ...)
	{ 
		va_list va;
		va_start(va,fmt);
		SetFormat(fmt, va);
		va_end(va);
		return *this;
	}

	static inline XString Format(const TCHAR * fmt, va_list va) { return XString().SetFormat(fmt, va); }
	static inline XString Format(TCHAR * fmt, va_list va) { return Format((const TCHAR*)fmt, va); }

	static inline XString Format(const TCHAR * fmt, ...)
	{ 
		va_list va;
		va_start(va,fmt);
		XString s = Format(fmt, va);
		va_end(va);
		return s;
	}
	static inline XString Format(TCHAR * fmt, ...)
	{ 
		va_list va;
		va_start(va,fmt);
		XString s = Format(fmt, va);
		va_end(va);
		return s;
	}

	int FindData(const void * p, int size, int start = 0) const;
	int Find(const TCHAR * a, int start = 0) const;
	int Find(const XString& a, int start = 0) const;

	XString Replace(const TCHAR * p_find, const TCHAR * p_replace, bool all = true) const;

	XString& Separate();
	XString Clone() const;

	XString& Assign(const XString& a);
	XString& Assign(const void * p, int size);
	XString& Assign(const void * p1, int size1, const void * p2, int size2);
	inline XString& Assign(const TCHAR * s) 
	{ 
		return Assign((const void*)s, (unsigned)__WSTRLEN(s) * (unsigned)sizeof(TCHAR)); 
	}
	inline XString& Assign(const TCHAR * s, int len) 
	{ 
		return Assign((const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); 
	}
	inline XString& Assign(TCHAR c) { return Assign((const void*)&c, sizeof(TCHAR)); }

	inline XString& operator=(const XString& a) { return Assign(a); }
	inline XString& operator=(TCHAR * a) { return Assign((const TCHAR*)a); }
	inline XString& operator=(const TCHAR * a) { return Assign(a); }
	inline XString& operator=(TCHAR c) { return Assign(c); }
	inline XString& operator=(int val) { return Assign(XString(val)); }
	inline XString& operator=(float val) { return Assign(XString(val, 10)); }

	TCHAR Char(int i) const;
	XString& SetChar(int i, TCHAR c);

	inline XString& Insert(int i, const XString& a) 
	{ 
		return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), a); 
	}

	inline XString& Insert(int i, const TCHAR * s) 
	{ 
		return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), 
			(const void*)s, (unsigned)__WSTRLEN(s) * (unsigned)sizeof(TCHAR)); 
	}

	inline XString& Insert(int i, const TCHAR * s, int len) 
	{ 
		return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), 
			(const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); 
	}

	inline XString& Insert(int i, TCHAR c) 
	{ 
		return InsertData((unsigned)i * (unsigned)sizeof(TCHAR), (const void*)&c, sizeof(TCHAR)); 
	}

	XString& InsertData(int i, const XString& a);
	XString& InsertData(int i, const void * p, int size);

	XString SubString(int start,int len) const;
	XString SubData(int start,int len) const;

	XString Lower() const;
	XString Upper() const;
	XString Flower() const; // 1st char upper, another lower

	XString Trim(bool leftTrim = true, bool rightTrim = true) const;

	XString& Append(const XString& a);
	XString& Append(const void * p, int size);
	XString& Append(const void * p1, int size1, const void * p2, int size2);
	inline XString& Append(const TCHAR * s) 
	{ 
		return Append((const void*)s, (unsigned)__WSTRLEN(s) * (unsigned)sizeof(TCHAR)); 
	}

	inline XString& Append(const TCHAR * s, int len) 
	{ 
		return Append((const void*)s, (unsigned)len * (unsigned)sizeof(TCHAR)); 
	}

	inline XString& Append(TCHAR c) { return Append((const void*)&c, sizeof(TCHAR)); }

	inline XString& operator+=(const XString& b) { return Append(b); }
	inline XString& operator+=(TCHAR * b) { return Append((const TCHAR*)b); }
	inline XString& operator+=(const TCHAR * b) { return Append(b); }
	inline XString& operator+=(TCHAR c) { return Append(c); }

	friend inline XString operator+(const XString& a,const XString& b) { return XString(a, b); }

	friend inline XString operator+(TCHAR * a,const XString& b) 
	{ 
		return XString((const void*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), b.str, b.Size()); 
	}

	friend inline XString operator+(const TCHAR * a,const XString& b) 
	{ 
		return XString((const void*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), b.str, b.Size()); 
	}

	friend inline XString operator+(const XString& a,TCHAR * b) 
	{ 
		return XString(a.str,a.Size(), (const void*)b, (unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)); 
	}

	friend inline XString operator+(const XString& a,const TCHAR * b) 
	{ 
		return XString(a.str, a.Size(), (const void*)b, (unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)); 
	}

	friend inline XString operator+(char c,const XString& b) 
	{ 
		return XString((const void*)&c, (unsigned)sizeof(TCHAR), b.str, b.Size()); 
	}

	friend inline XString operator+(const XString& a,char c) 
	{ 
		return XString(a.str, a.Size(), (const void*)&c, (unsigned)sizeof(TCHAR)); 
	}

	static int Cmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n = 0x7fffffff / sizeof(TCHAR));
	static int Icmp(const TCHAR * a, int aLen, const TCHAR * b, int bLen, int n = 0x7fffffff / sizeof(TCHAR));

	int Cmp (const TCHAR * b, int n = 0x7fffffff / sizeof(TCHAR)) const;
	int Icmp (const TCHAR * b, int n = 0x7fffffff / sizeof(TCHAR)) const;

	int Cmp (const XString& b, int n = 0x7fffffff / sizeof(TCHAR)) const;
	int Icmp (const XString& b, int n = 0x7fffffff / sizeof(TCHAR)) const;

	static int CmpData(const void * a, int aSize, const void * b, int bSize, int n = 0x7fffffff);
	static int HashData(const void * a, int size);

	inline int Hash() const { return HashData((const void*)str, Size()); }

	friend inline bool operator==(const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) == 0; 
	}

	friend inline bool operator==(TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) == 0; 
	}

	friend inline bool operator==(const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) == 0; 
	}

	friend inline bool operator==(const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) == 0; 
	}

	friend inline bool operator==(const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) == 0; 
	}

	friend inline bool operator!=(const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) != 0; 
	}

	friend inline bool operator!=(TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) != 0; 
	}

	friend inline bool operator!=(const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) != 0; 
	}

	friend inline bool operator!=(const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) != 0; 
	}

	friend inline bool operator!=(const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) != 0; 
	}

	friend inline bool operator<=(const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) <= 0; 
	}

	friend inline bool operator<=(TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) <= 0; 
	}

	friend inline bool operator<=(const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) <= 0; 
	}

	friend inline bool operator<=(const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) <= 0; 
	}

	friend inline bool operator<=(const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) <= 0; 
	}

	friend inline bool operator< (const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) < 0; 
	}

	friend inline bool operator< (TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) < 0; 
	}

	friend inline bool operator< (const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) < 0; 
	}

	friend inline bool operator< (const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) < 0; 
	}

	friend inline bool operator< (const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) < 0; 
	}

	friend inline bool operator>=(const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) >= 0; 
	}

	friend inline bool operator>=(TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) >= 0; 
	}

	friend inline bool operator>=(const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) >= 0; 
	}

	friend inline bool operator>=(const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) >= 0; 
	}

	friend inline bool operator>=(const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) >= 0; 
	}

	friend inline bool operator>(const XString& a, const XString& b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), b.ToMemory(), b.Size()) > 0; 
	}

	friend inline bool operator>(TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) > 0; 
	}

	friend inline bool operator>(const TCHAR * a, const XString& b) 
	{ 
		return CmpData((const TCHAR*)a, (unsigned)__WSTRLEN(a) * (unsigned)sizeof(TCHAR), 
			b.ToMemory(), b.Size()) > 0; 
	}

	friend inline bool operator>(const XString& a, TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) > 0; 
	}

	friend inline bool operator>(const XString& a, const TCHAR * b) 
	{ 
		return CmpData(a.ToMemory(), a.Size(), (const TCHAR*)b, 
			(unsigned)__WSTRLEN(b) * (unsigned)sizeof(TCHAR)) > 0; 
	}

	static XString ToString(int a);
	static XString ToString(char a);
	static XString ToString(float a, int precision = 15);
	static TCHAR * ToString(TCHAR * dst, float a, int precision = 15);

	enum ENumType
	{
		NUM_ERROR,
		NUM_FLOAT,
		NUM_INT
	};

	static bool ParseDecSimple(const TCHAR *& str, int& val);
	static ENumType ParseNum(const TCHAR *& str, double& val);

	float ToFloat() const;
	int ToInt() const;

#ifndef USE_EXT_VERSION
	static XString FloatArrayToString(const float * a, int Count, int precision = 15);
#endif

public:

#ifndef ONE_BYTE_WCHAR
	class AnsiString
	{
		friend class XString;

	private:

		char * ansiStr;

		AnsiString(const TCHAR * str);

	public:

		inline ~AnsiString() { SAFE_DELETE_ARRAY(ansiStr); }
		inline operator const char* () const { return ansiStr; }
	};

	inline AnsiString ToCChar() const { return str; }
#endif

};

#endif // __X_STRING_H__

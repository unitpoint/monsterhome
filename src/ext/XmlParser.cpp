#include <XmlParser.h>

#ifdef USE_APP_DATA_SHARING
#include "AppsSharedDataManager.h"
#endif
static const int DICT_CRYPT_HEADER = 0xAA55AA55;
static const int DICT_CRYPT_SEED   = 0x12345678;

//============================================================================
//============================================================================
//============================================================================
#ifdef USE_APP_DATA_SHARING
const char sharedDataGroupName[]="$(AppIdentifierPrefix)glu.ToyShop";
const bool setSharedGroupName=false;

bool XmlParser::SaveDictToTextFile(const XString& filename, const XDictionary& d, const XString& rootTagName, bool cryptData)
{
	if(setSharedGroupName)
		CAppsSharedDataManager::setAppName(sharedDataGroupName);
#ifdef TEST_DATA_SHARING
	static int testData[512];
	for(int i=0;i<512;i++)
#ifdef VERSION_LITE
		testData[i]=512-i;
#else
		testData[i]=1024-i;
#endif
	CAppsSharedDataManager::saveData("testData",testData,sizeof(testData));
#endif
	WriteStream stream;
	if(XmlParser::SaveDict(&stream, d, rootTagName))
	{
		CStrWChar filePath;
		CFileUtil::GetApplicationDataPathForFile( filePath, filename.ToChar() );

		if(cryptData)
		{
			byte * buf = (byte*)stream.Buf();
			int bufSize = stream.DataSize();
			int crc32 = CCrc32::GetInstance()->Crc32(buf, bufSize);

			WriteStream cryptedStream;
#ifdef TEST_DATA_SHARING
			// some other data test
#ifdef VERSION_LITE
			cryptedStream.WriteInt(123);
#else
			cryptedStream.WriteInt(321);
#endif
#endif
			cryptedStream.WriteInt(DICT_CRYPT_HEADER);
			cryptedStream.WriteInt(crc32);

			RandomValue r(DICT_CRYPT_SEED);
			for(int i = 0; i < bufSize; i++)
			{
				cryptedStream.WriteByte(buf[i] ^ (byte)r.Int());
			}
			return CAppsSharedDataManager::saveData("savefile",(uint8*)cryptedStream.Buf(), cryptedStream.DataSize() );
		}
		return CAppsSharedDataManager::saveData("savefile",(uint8*)stream.Buf(), stream.DataSize() );
	}
	return false;
}

XDictionary XmlParser::LoadDictFromTextFile(const XString& filename, const XString& rootTagName)
{
	if(setSharedGroupName)
		CAppsSharedDataManager::setAppName(sharedDataGroupName);

#ifdef TEST_DATA_SHARING
	//////////////////////////////////////////////
	// keychain test
	////////////
	int *testData=NULL;
	int testDataSize=0;
	CAppsSharedDataManager::loadData("testData",(void**)&testData,testDataSize);
	if(testData)
	{
		int test0=testData[0];
		int test1=testData[1];
		DDELETEARR(testData);
	}
#endif
	XDictionary dictionary = XDictionary(XDictionary::NoData());

	CStrWChar filePath;
	CFileUtil::GetApplicationDataPathForFile( filePath, filename.ToChar() );
	int fileSize=0;
	char *helperBuffer=NULL;
	CAppsSharedDataManager::loadData("savefile",(void**)&helperBuffer,fileSize);

	if(helperBuffer)
	{
		char * buf = helperBuffer;
		buf[fileSize] = 0;
#ifdef TEST_DATA_SHARING
		int test=((int*)buf)[0];
		buf+=sizeof(int);

		fileSize-=sizeof(int);
#endif
		if(fileSize >= sizeof(int)*2 && *(int*)buf == DICT_CRYPT_HEADER)
		{
			int originCrc32 = ((int*)buf)[1];
			buf += sizeof(int)*2;
			fileSize -= sizeof(int)*2;

			RandomValue r(DICT_CRYPT_SEED);
			for(int i = 0; i < fileSize; i++)
			{
				buf[i] ^= (byte)r.Int();
			}

			int crc32 = CCrc32::GetInstance()->Crc32((uint8*)buf, fileSize);
			if(crc32 != originCrc32)
			{
				ASSERT(false);
				buf = NULL;
			}
		}
		if(buf)
		{
			dictionary = LoadDict(buf, rootTagName);
		}
		DDELETEARR(helperBuffer);
	}

	return dictionary;
}
#else
//============================================================================

bool XmlParser::SaveDictToTextFile(const XString& filename, const XDictionary& d, const XString& rootTagName, bool cryptData)
{
	WriteStream stream;
	if(SaveDict(&stream, d, rootTagName))
	{
		/*
		CStrWChar filePath;
		CFileUtil::GetApplicationDataPathForFile( filePath, filename.ToChar() );

		if(cryptData)
		{
			byte * buf = (byte*)stream.Buf();
			int bufSize = stream.DataSize();
			int crc32 = CCrc32::GetInstance()->Crc32(buf, bufSize);

			WriteStream cryptedStream;
			cryptedStream.WriteInt(DICT_CRYPT_HEADER);
			cryptedStream.WriteInt(crc32);

			RandomValue r(DICT_CRYPT_SEED);
			for(int i = 0; i < bufSize; i++)
			{
				cryptedStream.WriteByte(buf[i] ^ (byte)r.Int());
			}

#ifdef PLATFORM_WIN32
			return CFileUtil::WriteFile( filePath.ToWCharPtr(), (uint8*)cryptedStream.Buf(), cryptedStream.DataSize() ) != 0;
#else
			return CFileUtil::SafeWriteFile( filePath.ToWCharPtr(), (uint8*)cryptedStream.Buf(), cryptedStream.DataSize() ) != 0;
#endif
		}

#ifdef PLATFORM_WIN32
		return CFileUtil::WriteFile( filePath.ToWCharPtr(), (uint8*)stream.Buf(), stream.DataSize() ) != 0;
#else
		return CFileUtil::SafeWriteFile( filePath.ToWCharPtr(), (uint8*)stream.Buf(), stream.DataSize() ) != 0;
#endif
		*/
	}
	return false;
}
//============================================================================

XDictionary XmlParser::LoadDictFromTextFile(const XString& filename, const XString& rootTagName)
{
	XDictionary dictionary = XDictionary(XDictionary::NoData());

	/*
	CStrWChar filePath;
	CFileUtil::GetApplicationDataPathForFile( filePath, filename.ToChar() );
	ICFile * f = ICFileMgr::GetInstance()->Open( filePath.ToWCharPtr(), ICFileMgr::FILE_ACCESS_READ );
	if(f)
	{
		f->Seek(0, ICFile::FILE_SEEK_END);
		int fileSize = (int)f->Tell();
		f->Seek(0, ICFile::FILE_SEEK_START);

		char * allocatedBuf = DNEW char[fileSize+1];
		char * buf = allocatedBuf;
		if(buf && f->Read((byte*)buf, fileSize) == fileSize)
		{
			buf[fileSize] = 0;

			if(fileSize >= sizeof(int)*2 && *(int*)buf == DICT_CRYPT_HEADER)
			{
				int originCrc32 = ((int*)buf)[1];
				buf += sizeof(int)*2;
				fileSize -= sizeof(int)*2;

				RandomValue r(DICT_CRYPT_SEED);
				for(int i = 0; i < fileSize; i++)
				{
					buf[i] ^= (byte)r.Int();
				}

				int crc32 = CCrc32::GetInstance()->Crc32((uint8*)buf, fileSize);
				if(crc32 != originCrc32)
				{
					ASSERT(false);
					buf = NULL;
				}
			}
			if(buf)
			{
				dictionary = LoadDict(buf, rootTagName);
			}
		}
		DDELETEARR(allocatedBuf);
		ICFileMgr::GetInstance()->Close(f);
	}
	*/
	return dictionary;
}
#endif
bool XmlParser::StrEq(const TCHAR * s, const TCHAR * pat)
{
	return strcmp(s, pat) == 0;
}

const TCHAR * XmlParser::StrStr(const TCHAR * s, const TCHAR * pat)
{
	return strstr(s, pat);
}

int XmlParser::StrLen(const TCHAR * s)
{
	return strlen(s);
}

const TCHAR * XmlParser::TokenTypeToName(TokenType tokenType)
{
	switch(tokenType)
	{
	case NOTHING: return _T("NOTHING");

	case NAME: return _T("NAME");
	case STRING: return _T("STRING");
	case VALUE: return _T("VALUE");

	case ASSIGN: return _T("ASSIGN");

	case OPEN: return _T("OPEN");
	case CLOSE: return _T("CLOSE");
	case SINGLE_TAG_CLOSED: return _T("SINGLE_TAG_CLOSED");
	case BEGIN_CLOSE_TAG: return _T("BEGIN_CLOSE_TAG");

	case OPEN_UNKNOWN: return _T("OPEN_UNKNOWN");
	case CLOSE_UNKNOWN: return _T("CLOSE_UNKNOWN");

	case OPEN_Q: return _T("OPEN_Q");
	case CLOSE_Q: return _T("CLOSE_Q");

	case ERROR_TOKEN: return _T("ERROR_TOKEN");
	}
	return _T("TOKENTYPE !!!");
}

XmlParser::TokenData::TokenData()
{
	type = NOTHING;
	line = pos = -1;
}

XmlParser::XmlParser()
{
	operators.push_back(OperatorDesc(SINGLE_TAG_CLOSED, _T("/>")));
	operators.push_back(OperatorDesc(BEGIN_CLOSE_TAG, _T("</")));
	operators.push_back(OperatorDesc(OPEN_UNKNOWN, _T("<!")));
	operators.push_back(OperatorDesc(CLOSE_UNKNOWN, _T("!>")));
	operators.push_back(OperatorDesc(OPEN_Q, _T("<?")));
	operators.push_back(OperatorDesc(CLOSE_Q, _T("?>")));
	operators.push_back(OperatorDesc(ASSIGN, _T("=")));
	operators.push_back(OperatorDesc(OPEN, _T("<")));
	operators.push_back(OperatorDesc(CLOSE, _T(">")));

	error = ERROR_NOTHING;
	curLine = 0;
	curPos = 0;
}

XmlParser::~XmlParser()
{
	DeleteTokens();
}

void XmlParser::DeleteTokens()
{
	for(int i = 0; i < (int)tokens.size(); i++)
	{
		DDELETE(tokens[i]);
	}
	tokens.clear();
}

void XmlParser::Reset()
{
	DeleteTokens();
	filename.Clear();
	lines.clear();
	curLine = curPos = 0;
}

void XmlParser::InsertToken(int i, TokenData * token)
{
	tokens.insert(tokens.begin()+i, token);
}

#if defined(_WINDOWS) && defined(_DEBUG)

#include <stdio.h>
#include <tchar.h>

void XmlParser::DebugPrintLines()
{
	FILE * f = fopen("xml_lines.txt", "wt");
	ASSERT(f);
	fprintf(f, "[XmlParser::PrintLines] lines: %d\n\n", lines.size());
	for(int i = 0; i < lines.size(); i++)
	{
		fprintf(f, "%s\n", lines[i].ToCChar());
	}
	fclose(f);
}

void XmlParser::DebugPrintTokens()
{
	FILE * f = fopen("xml_tokens.txt", "wt");
	ASSERT(f);
	fprintf(f, "[XmlParser::PrintTokens] tokens: %d\n\n", tokens.size());
	for(int i = 0; i < tokens.size(); i++)
	{
		const TokenData * token = tokens[i];
		fprintf(f, "token: %s, type: %s, line: %d, %d\n", 
			token->str.ToCChar(), 
			XString(TokenTypeToName(token->type)).ToCChar(),
			token->line, token->pos); //, lines[token.line].SubString(token.pos, 20));
	}
	fclose(f);
}

#endif

bool XmlParser::ParseText(const XString& text)
{
	text.SplitLines(lines);
	return ParseLines();
}

XmlParser::TokenData * XmlParser::AddToken(const XString& str, TokenType type, int line, int pos)
{
	TokenData * token = DNEW TokenData();
	token->str = str;
	token->type = type;
	token->line = line;
	token->pos = pos;
	tokens.push_back(token);
	return token;
}

void XmlParser::SkipSpaces(const TCHAR *& str)
{
	while(*str && *str <= _T(' '))
	{
		str++;
	}
}

bool XmlParser::ParseLines()
{
	curLine = curPos = 0;
	for(; curLine < (int)lines.size(); curLine++)
	{
		// parse line
		const TCHAR * lineStart = lines[curLine].ToChar();
		const TCHAR * str = lineStart;

		curPos = 0;
		for(;;)
		{
			// skip spaces
			SkipSpaces(str);
			if(!*str)
				break;

			if(*str == _T('"') || *str == _T('\'')) // begin string
			{
				// XString s;
				TCHAR closeChar = *str;
				const TCHAR * tokenStart = str;
				for(str++; *str && *str != closeChar;)
				{
					str++;
					/* TCHAR c = *str++;
					if(c == _T('\\'))
					{
					switch(*str)
					{
					case _T('r'): c = _T('\r'); str++; break;
					case _T('n'): c = _T('\n'); str++; break;
					case _T('t'): c = _T('\t'); str++; break;
					case _T('\"'): c = _T('\"'); str++; break;
					case _T('\''): c = _T('\''); str++; break;
					case _T('\\'): c = _T('\\'); str++; break;
					//case _T('x'): 
					default:
					{
					int val, maxVal;
					if(sizeof(TCHAR) == 2)
					maxVal = 0xFFFF;
					else
					maxVal = 0xFF;

					if(*str == _T('x') || *str == _T('X')) // parse hex
					{
					str++;
					if(!ParseHexSimple(str, val)){
					curPos = str - lineStart;
					error = ERROR_CONST_STRING_ESCAPE_CHAR;
					return false;
					}
					}
					else if(*str == _T('0')) // octal
					{
					if(!ParseOctalSimple(str, val))
					{
					curPos = str - lineStart;
					error = ERROR_CONST_STRING_ESCAPE_CHAR;
					return false;
					}
					}
					else if(*str >= _T('1') && *str <= _T('9'))
					{
					if(!ParseDecSimple(str, val))
					{
					curPos = str - lineStart;
					error = ERROR_CONST_STRING_ESCAPE_CHAR;
					return false;
					}
					}
					else
					{
					val = c;
					}
					c = (TCHAR)(val <= maxVal ? val : maxVal);
					}
					break;
					}
					}
					s.Append(c); */
				}
				if(*str != closeChar)
				{
					curPos = str - lineStart;
					error = ERROR_CONST_STRING;
					return false;
				}
				XString s = XString(tokenStart+1, str - (tokenStart+1))
					.Replace(_T("&amp;"), _T("&"))
					.Replace(_T("&quot;"), _T("\""))
					.Replace(_T("&apos;"), _T("'"))
					.Replace(_T("&lt;"), _T("<"))
					.Replace(_T("&gt;"), _T(">"))
					;
				AddToken(s, STRING, curLine, tokenStart - lineStart);
				str++;
				continue;
			}

			if(StrEq(str, _T("<!--")))
			{
				str += StrLen(_T("<!--"));
				int startLine = curLine;
				int startPos = str - lineStart;
				for(;;)
				{
					const TCHAR * end = StrStr(str, _T("-->"));
					if(end){
						str = end + StrLen(_T("-->"));
						break;
					}
					if(curLine >= (int)lines.size()){
						error = ERROR_MULTI_LINE_COMMENT;
						curPos = str - lineStart;
						return false;
					}
					str = lineStart = lines[++curLine].ToChar();
					continue;
				}
			}

			if(*str == _T('_') || (*str >= _T('a') && *str <= _T('z'))
				|| (*str >= _T('A') && *str <= _T('Z')) )
			{ // parse name
				const TCHAR * nameStart = str;
				for(str++; *str; str++)
				{
					if(*str == _T('_') || *str == _T('.') || *str == _T('-')
						|| (*str >= _T('a') && *str <= _T('z'))
						|| (*str >= _T('A') && *str <= _T('Z'))
						|| (*str >= _T('0') && *str <= _T('9')) )
					{
						continue;
					}
					break;
				}
				if((*nameStart == _T('_') /*|| *nameStart == _T('.') || *str == _T('-')*/)
					&& nameStart+1 == str)
				{
					error = ERROR_NAME;
					curPos = str - lineStart;
					return false;
				}
				XString name = XString(nameStart, str - nameStart);
				AddToken(name, NAME, curLine, nameStart - lineStart);
				continue;
			}
			// parse operator
			int i;
			for(i = 0; i < (int)operators.size(); i++)
			{
				const OperatorDesc& op = operators[i];
				if(StrEq(str, op.name))
				{
					int len = StrLen(op.name);
					XString s(str, len);
					AddToken(s, op.type, curLine, str - lineStart);
					str += len;
					break;
				}
			}
			if(i < (int)operators.size())
			{
				continue;
			}

			{ // parse value
				const TCHAR * valueStart = str;
				for(str++; *str && *str > _T(' '); str++) {}
				XString s(valueStart, str - valueStart);
				AddToken(s, VALUE, curLine, valueStart - lineStart);
				continue;
			}

			error = ERROR_SYNTAX;
			curPos = str - lineStart;
			return false;
		}
	}
	// PrintTokens();
	return true;
}



XDictionary XmlParser::LoadDict(const XString& text, const XString& rootTagName)
{
	struct Lib
	{
		XString text;
		XmlParser parser;
		XString rootTagName;
		int tokenCursor;
		// XmlParser::Token * errorToken;
		bool error;
		XString itemsNumberKey;

		Lib(const XString& p_text, const XString& p_rootTagName)
		{
			text = p_text;
			rootTagName = p_rootTagName;
			tokenCursor = 0;
			// errorToken = NULL;
			itemsNumberKey = _T("#itemsNumber");
			error = false;      
		}

		const XmlParser::TokenData * ReadToken()
		{
			return !error && tokenCursor < parser.TokensNumber() ? parser.Token(tokenCursor++) : NULL;
		}

		XDictionary Error()
		{
			error = true;
			return XDictionary(XDictionary::NoData());
		}

		const XmlParser::TokenData * ExpectType(XmlParser::TokenType type)
		{
			const XmlParser::TokenData * token = ReadToken();
			return token && token->type == type ? token : NULL;
		}

		const XmlParser::TokenData * ExpectTypeOr(XmlParser::TokenType type1, XmlParser::TokenType type2)
		{
			const XmlParser::TokenData * token = ReadToken();
			return token && (token->type == type1 || token->type == type2) ? token : NULL;
		}

		XDictionary UnSerializeOpenTag_r()
		{
			XDictionary d;
			int tagNameCursor = tokenCursor-1;
			for(;;)
			{
				const XmlParser::TokenData * token = ReadToken();
				if(!token)
				{
					return Error();
				}
				if(token->type == XmlParser::OPEN_UNKNOWN || token->type == XmlParser::OPEN_Q)
				{
					XmlParser::TokenType closeTokenType = 
						token->type == XmlParser::OPEN_UNKNOWN 
						? XmlParser::CLOSE_UNKNOWN
						: XmlParser::CLOSE_Q;

					for(;;)
					{
						token = ReadToken();
						if(!token)
						{
							return Error();
						}
						if(token->type == closeTokenType)
						{
							break;
						}
					}
					continue;
				}
				if(token->type == XmlParser::SINGLE_TAG_CLOSED)
				{
					return d;
				}
				if(token->type == XmlParser::NAME)
				{
					XString attrName = token->str;
					if(!ExpectType(XmlParser::ASSIGN)
						|| !(token = ExpectTypeOr(XmlParser::STRING, XmlParser::VALUE)))
					{
						return Error();
					}
					d.Set(attrName, token->str);
					continue;
				}
				if(token->type == XmlParser::CLOSE)
				{
					for(;;)
					{
						if(!(token = ReadToken()))
						{
							return Error();
						}
						if(token->type == XmlParser::BEGIN_CLOSE_TAG)
						{
							token = ExpectType(XmlParser::NAME);
							if(!token 
								|| parser.Token(tagNameCursor)->str != token->str
								|| !ExpectType(XmlParser::CLOSE))
							{
								return Error();
							}
							return d;
						}
						if(token->type == XmlParser::OPEN)
						{
							if(!(token = ExpectType(XmlParser::NAME)))
							{
								return Error();
							}
							XDictionary item = UnSerializeOpenTag_r();
							if(error)
							{
								return Error();
							}
							if(token->str == _T("item"))
							{
								int num = d.Int(itemsNumberKey);
								d.SetDict(num, item);
								d.SetInt(itemsNumberKey, num+1);
								// d[itemsNumberKey] = (int)d[itemsNumberKey] + 1;
							}
							else
							{
								d.SetDict(token->str, item);
							}
							continue;
						}
						return Error();
					}
					continue;
				}
			}
			return Error(); // shut up compiler
		}

		XDictionary UnSerialize()
		{
			if(!parser.ParseText(text))
			{
				return Error();
			}

			for(;;)
			{
				const XmlParser::TokenData * token = ReadToken();
				if(!token)
				{
					return Error();
				}
				if(token->type == XmlParser::OPEN_UNKNOWN || token->type == XmlParser::OPEN_Q)
				{
					XmlParser::TokenType closeTokenType = 
						token->type == XmlParser::OPEN_UNKNOWN 
						? XmlParser::CLOSE_UNKNOWN
						: XmlParser::CLOSE_Q;

					for(;;)
					{
						token = ReadToken();
						if(!token)
						{
							return Error();
						}
						if(token->type == closeTokenType)
						{
							break;
						}
					}
					continue;
				}
				if(token->type == XmlParser::OPEN)
				{
					token = ReadToken();
					if(!token)
					{
						return Error();
					}
					if(token->type == XmlParser::NAME)
					{
						XDictionary d = UnSerializeOpenTag_r();
						if(error)
						{
							return Error();
						}
						if(rootTagName.IsEmpty() || token->str == rootTagName)
						{
							return d;
						}
					}
					continue;
				}
				break;
			}
			return Error();
		}
	};

	return Lib(text, rootTagName).UnSerialize();
}

bool XmlParser::SaveDict(WriteStream * dest, const XDictionary& d, const XString& rootTagName)
{
	struct Lib
	{
		XDictionary dictionary;
		XString rootTagName;
		WriteStream * stream;

		Lib(WriteStream * p_stream, const XDictionary& p_dict, const XString& p_rootTagName)
			: dictionary(p_dict), rootTagName(p_rootTagName)
		{
			stream = p_stream;
		}

		bool Write(const char * str)
		{
			return stream->WriteAnsi(str);
		}

		bool Serialize()
		{
			return Write("<?xml version=\"1.0\"?>\r\n\r\n") 
				&& Serialize_r(rootTagName, dictionary);
		}

		bool WriteIndent(int depth)
		{
			for(int i = 0; i < depth; i++)
			{
				if(!Write("  "))
				{
					return false;
				}
			}
			return true;
		}

		bool Serialize_r(const XString& tagName, const XDictionary& node, int depth = 0)
		{
			Vector<XDictionaryKeyValue> values;      
			values.reserve(node.Count()+1); // +1 for '__index' attr
			node.ToValues(values);

			bool isIndexTag = true;
			{
				int len = tagName.Len();
				const TCHAR * tagStr = tagName.ToChar();
				for(int i = 0; i < len; i++)
				{
					if(tagStr[i] < _T('0') || tagStr[i] > _T('9'))
					{
						isIndexTag = false;
						break;
					}
				}
			}

			// XString::AnsiString ansiTagName = tagName.ToCChar();
			const char * ansiTagName = tagName;

			WriteIndent(depth);
			Write("<");
			Write(isIndexTag ? "item" : ansiTagName);

			/* if(isIndexTag)
			{
			values.push_back(XDictionaryKeyValue(_T("__index"), tagName));
			} */

			int dictsNumber = 0;
			int scalarsNumber = 0;

			int i;
			for(i = 0; i < (int)values.size(); i++)
			{
				const XDictionaryKeyValue& value = values[i];
				if(value.ValueType() == XDictionary::VT_DICTIONARY)
				{
					dictsNumber++;
				}
				else
				{
					scalarsNumber++;
				}
			}

			for(i = 0; i < (int)values.size(); i++)
			{
				const XDictionaryKeyValue& value = values[i];
				if(value.ValueType() == XDictionary::VT_SCALAR)
				{
					if(scalarsNumber > 1)
					{
						Write("\r\n"); 
						WriteIndent(depth+1);
					}
					else
					{
						Write(" "); 
					}

					Write(value.key);
					Write("=\"");
					Write(value.str
						.Replace(_T("&"), _T("&amp;"))
						.Replace(_T("\""), _T("&quot;"))
						.Replace(_T("'"), _T("&apos;"))
						.Replace(_T("<"), _T("&lt;"))
						.Replace(_T(">"), _T("&gt;"))
						);
					Write("\"");
				}
			}

			if(!dictsNumber)
			{
				Write(" />\r\n");
			}
			else
			{
				Write(">\r\n");
				for(int i = 0; i < (int)values.size(); i++)
				{
					const XDictionaryKeyValue& value = values[i];
					if(value.ValueType() == XDictionary::VT_DICTIONARY)
					{
						if(!Serialize_r(value.key, value.dictionary, depth+1))
						{
							return false;
						}
					}
				}
				WriteIndent(depth);
				Write("</");
				Write(isIndexTag ? "item" : ansiTagName);
				Write(">\r\n");
			}

			return !stream->IsFailed();
		}
	};

	return Lib(dest, d, rootTagName).Serialize();
}

//============================================================================
//============================================================================
//============================================================================

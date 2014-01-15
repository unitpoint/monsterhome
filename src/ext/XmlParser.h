#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

//============================================================================
//
// Author: Evgeniy Golovin, evgeniy.golovin@glu.com
// Description: xml parser, it's used to un/serialize dictionary
//
//============================================================================

#include <ExtCommon.h>
#include <HashTable.h>
#include <xString.h>
#include <xDictionary.h>
#include <RWStreams.h>

class XmlParser
{
public:

  enum Error
  {
    ERROR_NOTHING,
    ERROR_MULTI_LINE_COMMENT, // multi line comment not end
    ERROR_CONST_STRING,             // string not end
    ERROR_CONST_STRING_ESCAPE_CHAR, // string escape error
    ERROR_NAME,               // error name, _ - this is error
    ERROR_SYNTAX
  };

  enum TokenType
  {
    NOTHING,

    NAME,           // [a..z][a..z0..9_\.]*
    STRING,         // ["].*?["]|['].*?[']
    VALUE,          // like string but without " or '
    
    ASSIGN, // =

    OPEN, // <
    CLOSE,   // >
    SINGLE_TAG_CLOSED,   // />
    BEGIN_CLOSE_TAG, // </

    OPEN_UNKNOWN, // <!
    CLOSE_UNKNOWN, // !>

    OPEN_Q, // <?
    CLOSE_Q, // ?>

    ERROR_TOKEN
  };

  struct TokenData
  {
    XString str;
    TokenType type;
    int line, pos;

    TokenData();
  };

protected:

  struct OperatorDesc
  {
    TokenType type;
    const TCHAR * name;

    OperatorDesc(){ type = NOTHING; name = NULL; }
    OperatorDesc(TokenType p_type, const TCHAR * p_name)
    {
      type = p_type;
      name = p_name;
    }
  };

  Vector<OperatorDesc> operators;

  XString filename;

  Vector<XString> lines;
  int curLine, curPos;

  Vector<TokenData*> tokens;
  Error error;

  bool ParseLines();

  static bool StrEq(const TCHAR * s, const TCHAR * pat);
  static const TCHAR * StrStr(const TCHAR * s, const TCHAR * pat);
  static int StrLen(const TCHAR * s);

private:

  TokenData * AddToken(const XString& token, TokenType type, int line, int pos);
  
  void SkipSpaces(const TCHAR *& str);

  void DeleteTokens();

public:

  XmlParser();
  ~XmlParser();

  void Reset();

  bool IsError() const { return error != ERROR_NOTHING; }
  Error ErrorCode() const { return error; }
  int ErrorLine() const { return curLine; }
  int ErrorPos() const { return curPos; }

  static const TCHAR * TokenTypeToName(TokenType tokenType);

  XString LineString(int i) const { return lines[i]; }
  int LinesNumber() const { return lines.size(); }

  bool ParseText(const XString& text);

  int TokensNumber() const { return tokens.size(); }
  const TokenData * Token(int i) const { return tokens[i]; }
  void InsertToken(int i, TokenData * token);

#if defined(_WINDOWS) && defined(_DEBUG)
  void DebugPrintLines();
  void DebugPrintTokens();
#endif

  static bool SaveDict(WriteStream * dest, const XDictionary& d, const XString& rootTagName = _T("settings"));
  static bool SaveDictToTextFile(const XString& filename, const XDictionary& d, const XString& rootTagName = _T("settings"), bool cryptData = false);

  static XDictionary LoadDict(const XString& text, const XString& rootTagName = _T("settings"));
  static XDictionary LoadDictFromTextFile(const XString& filename, const XString& rootTagName = _T("settings"));
};


#endif
#ifndef __X_DICTIONARY_H__
#define __X_DICTIONARY_H__

//============================================================================
//
// Author: Evgeniy Golovin, evgeniy.golovin@glu.com
// Description: ref counted dictionary
//
//============================================================================

#include <ExtCommon.h>
#include <HashTable.h>
#include <xString.h>

class XDictionaryData;
struct XDictionaryKeyValue;
class XDictionaryValueLink;
class XDictionaryReadonlyValueLink;
class XDictionary
{
protected:

  friend class XDictionaryData;

  XDictionaryData * data;

  XDictionaryData * RequireData();

public:

  enum EValueType
  {
    VT_NOT_EXIST,
    VT_SCALAR,
    VT_DICTIONARY
  };

  enum
  {
    ITERATE_CONTINUE     = 0,
    ITERATE_BREAK        = 1,
    ITERATE_DELETE_ITEM  = 2,
  };

  struct NoData {}; // int dummy; NoData(){ dummy = 0; } };
  struct CopyData {}; // int dummy; CopyData(){ dummy = 0; } };

  typedef int (*IterateFunc)(const XDictionaryKeyValue&, void*);

  XDictionary(const NoData&);
  XDictionary();
  XDictionary(const XDictionary&);
  XDictionary(const XDictionary&, const CopyData&);

  ~XDictionary();

  XDictionary& operator=(const XDictionary&);

  XDictionary& Separate();
  XDictionary Clone() const;

  int Count() const;
  bool IsAllocated() const { return data != NULL; }

  bool IsExist(const XString& key) const;
  EValueType ValueType(const XString& key) const;

  XDictionaryKeyValue GetAt(int i) const;

  XString Get(const XString& key) const;
  XString Get(const XString& key, const XString& defValue) const;
  XDictionary& Set(const XString& key, const XString& value);
  XDictionary& Add(const XString& key, const XString& value);

  int Int(const XString& key) const;
  int Int(const XString& key, int defValue) const;
  XDictionary& SetInt(const XString& key, int value);
  XDictionary& AddInt(const XString& key, int value);

  bool Bool(const XString& key) const;
  bool Bool(const XString& key, bool defValue) const;
  XDictionary& SetBool(const XString& key, bool value);
  XDictionary& AddBool(const XString& key, bool value);

  float Float(const XString& key) const;
  float Float(const XString& key, float defValue) const;
  XDictionary& SetFloat(const XString& key, float value, int precision = 5);
  XDictionary& AddFloat(const XString& key, float value, int precision = 5);

  XDictionary Dict(const XString& key) const;
  XDictionary Dict(const XString& key, const XDictionary& defValue) const;
  XDictionary& SetDict(const XString& key, const XDictionary& value);
  XDictionary& AddDict(const XString& key, const XDictionary& value);
  XDictionary& AddDict(const XDictionary& value);

  XDictionaryKeyValue GetValue(const XString& key) const;
  XDictionary& SetValue(const XDictionaryKeyValue& value);
  XDictionary& AddValue(const XDictionaryKeyValue& value);

  void ToKeys(Vector<XString>& out) const;
  void ToValues(Vector<XDictionaryKeyValue>& out) const;
  // void ToIntValues(Vector<int>& out);
  // void ToFloatValues(Vector<float>& out);
  
  XDictionary& SetKeyValues(const Vector<XDictionaryKeyValue>&);
  XDictionary& AddKeyValues(const Vector<XDictionaryKeyValue>&);

  void ForEach(IterateFunc f, void * params);

  XDictionaryReadonlyValueLink operator[](const XString& key) const;
  XDictionaryValueLink operator[](const XString& key);
};

struct XDictionaryKeyValue
{
  XString key;
  XString str;
  XDictionary dictionary;

  XDictionaryKeyValue(): dictionary(XDictionary::NoData()){}
  XDictionaryKeyValue(const XString& k, const XString& s): key(k), str(s), dictionary(XDictionary::NoData()){}
  XDictionaryKeyValue(const XString& k, const XString& s, const XDictionary& d): key(k), str(s), dictionary(d){}
  XDictionaryKeyValue(const XDictionaryKeyValue& b): key(b.key), str(b.str), dictionary(b.dictionary){}

  XDictionary::EValueType ValueType() const
  {
    return dictionary.IsAllocated() ? XDictionary::VT_DICTIONARY : XDictionary::VT_SCALAR;
  }
};

class XDictionaryValueLink
{
  XDictionary& dictionary;
  XString key;

  XDictionaryValueLink& operator=(const XDictionaryValueLink&);

public:

  XDictionaryValueLink(const XDictionaryValueLink&); // no body really, avoid compiler error
  XDictionaryValueLink(XDictionary& d, const XString& k): dictionary(d), key(k){}

  operator int(){ return dictionary.Int(key); }
  XDictionaryValueLink& operator =(int b)
  {
    dictionary.SetInt(key, b);
    return *this;
  }

  operator float(){ return dictionary.Float(key); }
  XDictionaryValueLink& operator =(float b)
  {
    dictionary.SetFloat(key, b);
    return *this;
  }

  operator bool(){ return dictionary.Bool(key); }
  XDictionaryValueLink& operator =(bool b)
  {
    dictionary.SetBool(key, b);
    return *this;
  }

  operator XString(){ return dictionary.Get(key); }
  XDictionaryValueLink& operator =(const XString& b)
  {
    dictionary.Set(key, b);
    return *this;
  }
  XDictionaryValueLink& operator =(const TCHAR * b)
  {
    dictionary.Set(key, b);
    return *this;
  }

  operator XDictionary(){ return dictionary.Dict(key); }
  XDictionaryValueLink& operator =(const XDictionary& b)
  {
    dictionary.SetDict(key, b);
    return *this;
  }
};

class XDictionaryReadonlyValueLink
{
  const XDictionary& dictionary;
  XString key;

  XDictionaryReadonlyValueLink& operator=(const XDictionaryReadonlyValueLink&);

public:

  XDictionaryReadonlyValueLink(const XDictionaryReadonlyValueLink&); // no body really, avoid compiler error
  XDictionaryReadonlyValueLink(const XDictionary& d, const XString& k): dictionary(d), key(k){}

  operator int(){ return dictionary.Int(key); }
  operator float(){ return dictionary.Float(key); }
  operator bool(){ return dictionary.Bool(key); }
  operator XString(){ return dictionary.Get(key); }
  operator XDictionary(){ return dictionary.Dict(key); }
};

#endif // __X_DICTIONARY_H__

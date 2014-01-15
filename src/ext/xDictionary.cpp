#include <xDictionary.h>

//============================================================================
//
// Author: Evgeniy Golovin, evgeniy.golovin@glu.com
// Description: ref counted dictionary
//
//============================================================================

class XDictionaryData
{
	friend class XDictionary;

protected:

	struct Value
	{
		XString str;
		XDictionary dictionary;

		Value(): dictionary(XDictionary::NoData()){}
		Value(const XString& s): str(s), dictionary(XDictionary::NoData()){}
		Value(const XDictionary& d): dictionary(d){}
		Value(const Value& b): str(b.str), dictionary(b.dictionary){}
		Value(const XString& s, const XDictionary& d): str(s), dictionary(d){}
	};

	HashTable<XString, Value> values;
	int refCount;

	XDictionaryData()
	{
		refCount = 1;
	}

	void AddRefSafely()
	{
		if(this)
		{
			++refCount;
		}
	}

	void ReleaseSafely()
	{
		if(this && --refCount <= 0)
		{
			DDELETE(this);
		}
	}

private:

	~XDictionaryData()
	{
	}
};

XDictionary::XDictionary(const NoData&)
{
	data = NULL;
}

XDictionary::XDictionary()
{
	data = DNEW XDictionaryData();
}

XDictionary::XDictionary(const XDictionary& b)
{
	data = b.data;
	data->AddRefSafely();
}

XDictionary::XDictionary(const XDictionary& b, const CopyData&)
{
	data = DNEW XDictionaryData();
	if(b.data)
	{
		data->values = b.data->values;
	}
}

XDictionary::~XDictionary()
{
	data->ReleaseSafely();
}

XDictionary& XDictionary::operator=(const XDictionary& b)
{
	XDictionaryData * oldData = data;
	data = b.data;
	data->AddRefSafely();
	oldData->ReleaseSafely();
	return *this;
}

bool XDictionary::IsExist(const XString& key) const
{
	return data && data->values.Get(key) != NULL;
}

XDictionary::EValueType XDictionary::ValueType(const XString& key) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? (val->dictionary.IsAllocated() ? VT_DICTIONARY : VT_SCALAR) : VT_NOT_EXIST;
	}
	return VT_NOT_EXIST;
}

XDictionaryKeyValue XDictionary::GetAt(int i) const
{
	if(data)
	{
		XString * key;
		XDictionaryData::Value * val = data->values.GetAt(i, &key);
		return val ? XDictionaryKeyValue(*key, val->str, val->dictionary) : XDictionaryKeyValue();
	}
	return XDictionaryKeyValue();
}

XDictionary& XDictionary::Separate()
{
	if(RequireData()->refCount > 1)
	{
		*this = Clone();
	}
	return *this;
}

XDictionary XDictionary::Clone() const
{
	if(data)
	{
		return XDictionary(*this, CopyData());
	}
	return XDictionary(); // NoData());
}

int XDictionary::Count() const
{
	return data ? data->values.Count() : 0;
}

inline XDictionaryData * XDictionary::RequireData()
{
	if(!data)
	{
		data = DNEW XDictionaryData();
	}
	return data;
}

//============================================================================

XString XDictionary::Get(const XString& key) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->str : XString();
	}
	return XString();
}

XString XDictionary::Get(const XString& key, const XString& defValue) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->str : defValue;
	}
	return defValue;
}

XDictionary& XDictionary::Set(const XString& key, const XString& value)
{
	RequireData()->values.Set(key, value);
	return *this;
}

XDictionary& XDictionary::Add(const XString& key, const XString& value)
{
	RequireData()->values.Add(key, value);
	return *this;
}

//============================================================================

int XDictionary::Int(const XString& key) const
{
	return Int(key, 0);
}

int XDictionary::Int(const XString& key, int defValue) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->str.ToInt() : defValue;
	}
	return defValue;
}

XDictionary& XDictionary::SetInt(const XString& key, int value)
{
	return Set(key, XString(value));
}

XDictionary& XDictionary::AddInt(const XString& key, int value)
{
	return Add(key, XString(value));
}

//============================================================================

bool XDictionary::Bool(const XString& key) const
{
	return Bool(key, false);
}

bool XDictionary::Bool(const XString& key, bool defValue) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->str.ToInt() != 0 : defValue;
	}
	return defValue;
}

XDictionary& XDictionary::SetBool(const XString& key, bool value)
{
	return SetInt(key, value);
}

XDictionary& XDictionary::AddBool(const XString& key, bool value)
{
	return AddInt(key, value);
}

//============================================================================

float XDictionary::Float(const XString& key) const
{
	return Float(key, 0.0f);
}

float XDictionary::Float(const XString& key, float defValue) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->str.ToFloat() : defValue;
	}
	return defValue;
}

XDictionary& XDictionary::SetFloat(const XString& key, float value, int precision)
{
	return Set(key, XString(value, precision));
}

XDictionary& XDictionary::AddFloat(const XString& key, float value, int precision)
{
	return Add(key, XString(value, precision));
}

//============================================================================

XDictionary XDictionary::Dict(const XString& key) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->dictionary : XDictionary(NoData());
	}
	return XDictionary(NoData());
}

XDictionary XDictionary::Dict(const XString& key, const XDictionary& defValue) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? val->dictionary : defValue;
	}
	return defValue;
}

XDictionary& XDictionary::SetDict(const XString& key, const XDictionary& value)
{
	// ASSERT(value.data);
	RequireData()->values.Set(key, value);
	return *this;
}

XDictionary& XDictionary::AddDict(const XString& key, const XDictionary& value)
{
	// ASSERT(value.data);
	RequireData()->values.Add(key, value);
	return *this;
}

XDictionary& XDictionary::AddDict(const XDictionary& value)
{
	return AddDict(Count(), value);
}

//============================================================================

XDictionaryKeyValue XDictionary::GetValue(const XString& key) const
{
	if(data)
	{
		XDictionaryData::Value * val = data->values.Get(key);
		return val ? XDictionaryKeyValue(key, val->str, val->dictionary) : XDictionaryKeyValue();
	}
	return XDictionaryKeyValue();
}

XDictionary& XDictionary::SetValue(const XDictionaryKeyValue& value)
{
	// ASSERT(value.data);
	RequireData()->values.Set(value.key, XDictionaryData::Value(value.str, value.dictionary));
	return *this;
}

XDictionary& XDictionary::AddValue(const XDictionaryKeyValue& value)
{
	// ASSERT(value.data);
	RequireData()->values.Add(value.key, XDictionaryData::Value(value.str, value.dictionary));
	return *this;
}

//============================================================================

void XDictionary::ToKeys(Vector<XString>& out) const
{
	struct Lib
	{
		static int Iter(const XString& k, XDictionaryData::Value& v, void * params)
		{
			((Vector<XString>*)params)->push_back(k);

			return HashTable<XString, XDictionaryData::Value>::ITERATE_CONTINUE;
		}
	};

	out.clear();
	if(data)
	{
		out.reserve(data->values.Count());
		data->values.ForEach(Lib::Iter, &out);
	}
}

void XDictionary::ToValues(Vector<XDictionaryKeyValue>& out) const
{
	struct Lib
	{
		static int Iter(const XString& k, XDictionaryData::Value& v, void * params)
		{
			((Vector<XDictionaryKeyValue>*)params)->push_back(
				XDictionaryKeyValue(k, v.str, v.dictionary));

			return HashTable<XString, XDictionaryData::Value>::ITERATE_CONTINUE;
		}
	};

	out.clear();
	if(data)
	{
		out.reserve(data->values.Count());
		data->values.ForEach(Lib::Iter, &out);
	}
}

XDictionary& XDictionary::SetKeyValues(const Vector<XDictionaryKeyValue>& src)
{
	for(int i = 0; i < (int)src.size(); i++)
	{
		SetValue(src[i]);
	}
	return *this;
}

XDictionary& XDictionary::AddKeyValues(const Vector<XDictionaryKeyValue>& src)
{
	for(int i = 0; i < (int)src.size(); i++)
	{
		AddValue(src[i]);
	}
	return *this;
}

void XDictionary::ForEach(IterateFunc f, void * params)
{
	struct Data
	{
		IterateFunc func;
		void * params;
	} iterParams = { f, params };

	struct Lib
	{
		static int Iter(const XString& k, XDictionaryData::Value& v, void * params)
		{
			return ((Data*)params)->func(
				XDictionaryKeyValue(k, v.str, v.dictionary),
				((Data*)params)->params);
		}
	};

	if(data)
	{
		data->values.ForEach(Lib::Iter, &iterParams);
	}
}

//============================================================================

XDictionaryReadonlyValueLink XDictionary::operator[](const XString& key) const
{
	return XDictionaryReadonlyValueLink(*this, key);
}

XDictionaryValueLink XDictionary::operator[](const XString& key)
{
	return XDictionaryValueLink(*this, key);
}

//============================================================================
//============================================================================
//============================================================================

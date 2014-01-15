#ifndef __JSON_H__
#define __JSON_H__

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

#include <AppCommon.h>

struct cJSON;
class JSON: public CCObject
{
public:

	enum EType {
		TYPE_FALSE,
		TYPE_TRUE,
		TYPE_NULL,
		TYPE_NUMBER,
		TYPE_STRING,
		TYPE_ARRAY,
		TYPE_OBJECT,
	};

protected:

	String name;
	EType type;

	String value_str;
	double value_float;
	int value_int;

	CCMutableArray<JSON*> * children;

	void reset();
	void loadFrom(cJSON*);

public:

	JSON();
	JSON(EType);
	JSON(const String& name, EType);
	JSON(const String& name, const String& value);
	JSON(const String& name, const String& value, EType);
	JSON(const String& name, int value);
	JSON(const String& name, float value, int precision = 15);
	JSON(const String& name, double value, int precision = 15);
	JSON(const String& filename);
	virtual ~JSON();

	EType getType(){ return type; }
	void setType(EType value){ type = value; }

	const String& getName(){ return name; }
	void setName(const String& value);

	const String& getString(){ return value_str; }
	void setValue(const String& value);

	double getDouble(){ return value_float; }
	void setValue(double value, int precision = 15);

	float getFloat(){ return (float)value_float; }
	void setValue(float value, int precision = 15);

	int getInt(){ return value_int; }
	void setValue(int value);

	CCMutableArray<JSON*> * getChildren(){ return children; };
	void setChildren(CCMutableArray<JSON*>*);

	int getChildrenCount();
	JSON * getChildAtIndex(int i);
	JSON * getChild(const String& name);
	void addChild(JSON*);

	const String& getChildString(const String& name, const String& def = "");
	double getChildDouble(const String& name, double def = 0);
	float getChildFloat(const String& name, float def = 0);
	int getChildInt(const String& name, int def = 0);

	const String& getSubChildString(const String& name, const String& sub_name, const String& def = "");
	double getSubChildDouble(const String& name, const String& sub_name, double def = 0);
	float getSubChildFloat(const String& name, const String& sub_name, float def = 0);
	int getSubChildInt(const String& name, const String& sub_name, int def = 0);
};


#endif // __JSON_H__
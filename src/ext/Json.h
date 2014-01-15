#ifndef __JSON_H__
#define __JSON_H__

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
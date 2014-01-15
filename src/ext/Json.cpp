#include <App.h>
#include <JSON.h>
#include <cJSON.h>

JSON::JSON()
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_NULL;
}

JSON::JSON(EType type)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	this->type = type;
}

JSON::JSON(const String& name, EType type)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	this->type = type;
	setName(name);
}

JSON::JSON(const String& name, const String& value)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_STRING;
	setName(name);
	setValue(value);
}

JSON::JSON(const String& name, const String& value, EType type)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	this->type = type;
	setName(name);
	setValue(value);
}

JSON::JSON(const String& name, int value)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_NUMBER;
	setName(name);
	setValue(value);
}

JSON::JSON(const String& name, float value, int precision)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_NUMBER;
	setName(name);
	setValue(value, precision);
}

JSON::JSON(const String& name, double value, int precision)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_NUMBER;
	setName(name);
	setValue(value, precision);
}

JSON::JSON(const String& filename)
{
	autorelease();
	value_float = 0;
	value_int = 0;
	children = NULL;
	type = TYPE_NULL;
	String text = App::readFile(filename);
	const char * str = text;
	if(strncmp(str, "\xef\xbb\xbf", 3) == 0){ // utf8 prefix
		str += 3;
	}
	cJSON * root = cJSON_Parse(str);
	if(root){
		loadFrom(root);
		cJSON_Delete(root);
	}	
}

JSON::~JSON()
{
	if(children){
		children->release();
	}
}

void JSON::setName(const String& value)
{
	name = value;
}

void JSON::setValue(const String& value)
{
	value_str = value;
	value_float = value.ToFloat();
	value_int = (int)value_float;
}

void JSON::setValue(double value, int precision)
{
	value_str = String((float)value, precision);
	value_float = value;
	value_int = (int)value;
}

void JSON::setValue(float value, int precision)
{
	value_str = String(value, precision);
	value_float = value;
	value_int = (int)value;
}

void JSON::setValue(int value)
{
	value_str = value;
	value_float = (double)value;
	value_int = value;
}

void JSON::reset()
{
	name = "";

	value_str = "";
	value_float = 0;
	value_int = 0;

	if(children){
		children->release();
		children = NULL;
	}
}

void JSON::loadFrom(cJSON * item)
{
	if(item){
		switch(item->type){
		case cJSON_False:
			type = TYPE_FALSE;
			break;

		case cJSON_True:
			type = TYPE_TRUE;
			break;

		default:
		case cJSON_NULL:
			type = TYPE_NULL;
			break;

		case cJSON_Number:
			type = TYPE_NUMBER;
			break;

		case cJSON_String:
			type = TYPE_STRING;
			break;

		case cJSON_Array:
			type = TYPE_ARRAY;
			break;

		case cJSON_Object:
			type = TYPE_OBJECT;
			break;
		}

		name = item->string ? item->string : "";
		value_str = item->valuestring ? item->valuestring : "";
		value_float = item->valuedouble;
		value_int = item->valueint;

		if(item->child){
			if(!children){
				children = new CCMutableArray<JSON*>();
				// children->retain();
			}else{
				children->removeAllObjects();
			}
			for(item = item->child; item; item = item->next){
				JSON * child = new JSON();
				children->addObject(child);
				child->loadFrom(item);
			}
		}else if(children){
			children->release();
			children = NULL;
		}	
	}else{
		reset();
	}
}

void JSON::setChildren(CCMutableArray<JSON*> * p_children)
{
	if(p_children) p_children->retain();
	if(children) children->release();
	children = p_children;
}

int JSON::getChildrenCount()
{
	return children ? children->count() : 0;
}

JSON * JSON::getChildAtIndex(int i)
{
	if(children){
		return children->getObjectAtIndex(i);
	}
	return NULL;
}

void JSON::addChild(JSON * child)
{
	if(!children){
		children = new CCMutableArray<JSON*>();
	}
	children->addObject(child);
}

JSON * JSON::getChild(const String& name)
{
	if(children){		
		int count = children->count();
		for(int i = 0; i < count; i++){
			JSON * child = children->getObjectAtIndex(i);
			if(child->name == name){
				return child;
			}
		}
	}
	return NULL;
}

const String& JSON::getChildString(const String& name, const String& def)
{
	JSON * child = getChild(name);
	return child ? child->getString() : def;
}

double JSON::getChildDouble(const String& name, double def)
{
	JSON * child = getChild(name);
	return child ? child->getDouble() : def;
}

float JSON::getChildFloat(const String& name, float def)
{
	JSON * child = getChild(name);
	return child ? child->getFloat() : def;
}

int JSON::getChildInt(const String& name, int def)
{
	JSON * child = getChild(name);
	return child ? child->getInt() : def;
}

const String& JSON::getSubChildString(const String& name, const String& sub_name, const String& def)
{
	JSON * child = getChild(name);
	return child ? child->getChildString(sub_name, def) : def;
}

double JSON::getSubChildDouble(const String& name, const String& sub_name, double def)
{
	JSON * child = getChild(name);
	return child ? child->getChildDouble(sub_name, def) : def;
}

float JSON::getSubChildFloat(const String& name, const String& sub_name, float def)
{
	JSON * child = getChild(name);
	return child ? child->getChildFloat(sub_name, def) : def;
}

int JSON::getSubChildInt(const String& name, const String& sub_name, int def)
{
	JSON * child = getChild(name);
	return child ? child->getChildInt(sub_name, def) : def;
}


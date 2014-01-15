#ifndef __PATH_MOVE_ACTION_H__
#define __PATH_MOVE_ACTION_H__

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
#include <Curve.h>

typedef CurveSpline<vec2> CurveSpline2d;

class PathObject: public cocos2d::CCObject
{
protected:

	void copySpline(PathObject * origin, float start_time);

public:

	String name;
	CurveCatmullRomSpline<vec2> spline;
	// CurveSpline<vec2> spline;

	PathObject(PathObject * origin = NULL, float start_time = 0.0f);
	PathObject(const String& name, PathObject * origin = NULL, float start_time = 0.0f);
	virtual ~PathObject();

	int AddPoint(const vec2&, float min_offs = 0.0f);
	void RemoveIndex(int);
};

class PathMoveAction: public cocos2d::CCActionInterval
{
	typedef cocos2d::CCActionInterval super;

protected:

	PathObject * path;
	float speed;
	float side;
	float t;

public:

	float last_t;
	vec2 last_move_dir;

	virtual ~PathMoveAction();

	bool initWithPath(PathObject * path, float speed, float side, float t);

	virtual void update(cocos2d::ccTime time);

	static PathMoveAction * actionWithPath(PathObject * path, float speed, float side = 0, float t = 0);
};

#endif // __PATH_MOVE_ACTION_H__
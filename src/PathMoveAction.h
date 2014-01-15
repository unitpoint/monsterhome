#ifndef __PATH_MOVE_ACTION_H__
#define __PATH_MOVE_ACTION_H__

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
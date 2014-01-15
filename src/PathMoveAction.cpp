#include <PathMoveAction.h>
#include <ccTypes.h>

// ===============================================================================
// ===============================================================================
// ===============================================================================

PathObject::PathObject(PathObject * origin, float start_time)
{
	IW_CALLSTACK("PathObject::PathObject");
	autorelease();
	copySpline(origin, start_time);
}

PathObject::PathObject(const String& name, PathObject * origin, float start_time)
{
	IW_CALLSTACK("PathObject::PathObject 2");
	autorelease();
	this->name = name;
	copySpline(origin, start_time);
}

PathObject::~PathObject()
{
}

void PathObject::copySpline(PathObject * origin, float start_time)
{
	if(origin){
		int count = origin->spline.GetNumValues();
		if(count > 0 && start_time > origin->spline.GetTime(0)){
			spline.AddValue(0.0f, origin->spline.GetCurrentValue(start_time));
		}
		for(int i = 0; i < count; i++){
			float time = origin->spline.GetTime(i);
			if(time >= start_time){
				spline.AddValue(time - start_time, origin->spline.GetValue(i));
			}
		}
	}			
}

int PathObject::AddPoint(const vec2& p, float min_offs)
{
	IW_CALLSTACK("PathObject::AddPoint");

	int count = spline.GetNumValues();
	if(count > 0){
		#if 0
			struct __check__ {
				PathObject * path;
				int old_count;
				vec2 p;

				__check__(PathObject * p_path, vec2 pp)
				{
					path = p_path;
					old_count = path->spline.GetNumValues();
					p = pp;
					check();
				}

				~__check__()
				{
					check();
				}

				void check()
				{
					for(int i = 0; i < path->spline.GetNumValues(); i++){
						float t = path->spline.GetTime(i);
						if(isnanf(t)){
							int i = 0;
							t = (p - p).Len();
							t = path->spline.GetTime(i);
							int j = 0;
						}
					}
				}
			} __check__(this, p);
		#endif

		vec2 last = spline.GetValue(count-1);
		if(count >= 2 && min_offs > 0.0f){
			vec2 last2 = spline.GetValue(count-2);
			float dist = (last - last2).Len();
			if(dist < min_offs){
				spline.RemoveIndex(--count);
				last = last2;
			}
		}
		float dist = (last - p).Len();
		return spline.AddValue(spline.GetTime(count-1) + dist, p);
	}
	return spline.AddValue(0.0f, p);
}

void PathObject::RemoveIndex(int i)
{
	IW_CALLSTACK("PathObject::RemoveIndex");
	spline.RemoveIndex(i);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

PathMoveAction::~PathMoveAction()
{
	IW_CALLSTACK("PathMoveAction::~PathMoveAction");
	if(path){
		path->release();
	}
}

PathMoveAction * PathMoveAction::actionWithPath(PathObject * path, float speed, float side, float t)
{
	IW_CALLSTACK("PathMoveAction::actionWithPath");

	PathMoveAction * action = new PathMoveAction();
	action->initWithPath(path, speed, side, t);
	action->autorelease();

	return action;
}

bool PathMoveAction::initWithPath(PathObject * path, float speed, float side, float t)
{
	IW_CALLSTACK("PathMoveAction::actionWithPath");
	this->path = path;
	path->retain();
	float duration = (path->spline.GetTime(path->spline.GetNumValues()-1) - t) * 1000 / (speed * 20);
	if(super::initWithDuration(duration)){
		this->path = path;
		this->speed = speed;
		this->side = side;
		this->t = t;
		return true;
	}
	return false;
}

void PathMoveAction::update(cocos2d::ccTime time)
{
	IW_CALLSTACK("PathMoveAction::update");
	if (m_pTarget){
		last_t = (path->spline.GetTime(path->spline.GetNumValues()-1) - this->t) * time + this->t;
		vec2 p = path->spline.GetCurrentValue(last_t);

		cocos2d::CCNode * parent = m_pTarget->getParent();
		CCAssert(parent, "");
		vec2 size = parent->getContentSize();
		p = p * size;

		vec2 speed = path->spline.GetCurrentFirstDerivative(last_t);
		last_move_dir = speed.Norm();

		if(side != 0.0f){
			vec2 right_side = vec2(last_move_dir.y, -last_move_dir.x);
			p = p + right_side * side;
		}

		// entity->updateMoveDir(move_dir);
		m_pTarget->setPosition(p);
	}
}

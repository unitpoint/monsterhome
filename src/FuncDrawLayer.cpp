#include <FuncDrawLayer.h>

FuncDrawLayer * FuncDrawLayer::createWithTarget(cocos2d::CCObject * target, SEL_CallFuncUP draw_func, void * user_param)
{
	FuncDrawLayer * layer = new FuncDrawLayer();
	if(layer && layer->initWithTarget(target, draw_func, user_param)){
		layer->autorelease();
		return layer;
	}
	delete layer;
	return NULL;
}

bool FuncDrawLayer::initWithTarget(cocos2d::CCObject * target, SEL_CallFuncUP draw_func, void * user_param)
{
	if(!super::init()){
		return false;
	}

	this->target = target;
	this->draw_func = draw_func;
	this->user_param = user_param;

	return true;
}

void FuncDrawLayer::draw()
{
	if(draw_func){
		draw_func(target, user_param);
	}
}

void DrawRect::draw()
{
// #if CC_SPRITE_DEBUG_DRAW == 1
	// draw bounding box
	cocos2d::CCSize s = getContentSize();
	cocos2d::CCPoint vertices[4] = {
		ccp(0,0), ccp(s.width,0),
		ccp(s.width,s.height), ccp(0,s.height)
	};
	ccDrawPoly(vertices, 4, true);
// #endif
}
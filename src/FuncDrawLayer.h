#ifndef __FUNC_DRAW_LAYER_H__
#define __FUNC_DRAW_LAYER_H__

#include <Layer.h>

class FuncDrawLayer: public Layer
{
	typedef Layer super;

	CC_SYNTHESIZE(CCObject*, target, Target);
	CC_SYNTHESIZE(SEL_CallFuncUP, draw_func, DrawFunc);
	CC_SYNTHESIZE(void*, user_param, UserParam);

public:

	// LAYER_INIT_FUNC(FuncDrawLayer, initWithTarget, (CCObject* target, SEL_CallFuncN selector), (target, selector));

	static FuncDrawLayer * createWithTarget(cocos2d::CCObject * target, SEL_CallFuncUP draw_func, void * user_param = NULL);

	virtual bool initWithTarget(cocos2d::CCObject * target, SEL_CallFuncUP draw_func, void * user_param = NULL);

	virtual void draw();
};

class DrawRect: public Layer
{
public:

	LAYER_NODE_FUNC(DrawRect);

	virtual void draw();
};

#endif // __FUNC_DRAW_LAYER_H__
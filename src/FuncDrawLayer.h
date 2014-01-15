#ifndef __FUNC_DRAW_LAYER_H__
#define __FUNC_DRAW_LAYER_H__

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
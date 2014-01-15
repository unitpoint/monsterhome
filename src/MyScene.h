#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "App.h"

class LineLayer: public CCLayer
{
public:

	LAYER_NODE_FUNC(LineLayer);

	virtual void draw();
};

class MyWorld : public CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommand to return the exactly class pointer
	static cocos2d::CCScene* scene();

	// a selector callback
	virtual void menuCloseCallback(CCObject* pSender);

	void actionFinished(CCNode * sender);

	// implement the "static node()" method manually
	LAYER_NODE_FUNC(MyWorld);
};

#endif // __HELLOWORLD_SCENE_H__

#include "MyScene.h"
#include "MonsterHome.h"

CCScene* MyWorld::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::node();
	
	// 'layer' is an autorelease object
	MyWorld *layer = MyWorld::node();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MyWorld::init()
{
	{
		// Curve<CIwFVec2> curve;

	}
	//////////////////////////////
	// 1. super init first
	if ( !CCLayer::init() )
	{
		return false;
	}

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	CCMenuItemImage *pCloseItem = CCMenuItemImage::itemFromNormalImage(
										"CloseNormal.png",
										"CloseSelected.png",
										this,
										menu_selector(MyWorld::menuCloseCallback) );
	pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::menuWithItems(pCloseItem, NULL);
	pMenu->setPosition( CCPointZero );
	this->addChild(pMenu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::labelWithString("Hello World", "Arial", 24);
	pLabel->setOpacity(0);
	// ask director the window size
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	// position the label on the center of the screen
	pLabel->setPosition( ccp(size.width / 2, size.height - 50) );

	// add the label as a child to this layer
	this->addChild(pLabel, 1);

	{
		// Create the actions
		CCNode * target = pLabel;
		CCActionInterval * actionFadeIn = CCFadeIn::actionWithDuration(0.1f);
		// CCActionInterval * actionSkew = CCSkewTo::actionWithDuration(1.0, 10, 0);
		CCActionInterval * actionScale = CCScaleTo::actionWithDuration(1.0f, 2.0f, 2.0f);
		actionScale = CCEaseBounceOut::actionWithAction(actionScale);
		CCActionInterval * actionMove = CCMoveTo::actionWithDuration(1.0f, ccp(size.width / 2, size.height *0.3));
		actionMove = CCEaseBounceOut::actionWithAction(actionMove);
		CCActionInterval * actionRotate = CCRotateTo::actionWithDuration(1.0, 180.0);
		CCFiniteTimeAction * actionDone = CCCallFuncN::actionWithTarget(this, callfuncN_selector(MyWorld::actionFinished));
		target->runAction(CCSequence::actions(actionFadeIn, actionMove, actionDone, NULL));	
	}

	// add "MyWorld" splash screen"
	// CCSprite* pSprite = CCSprite::spriteWithFile("MyWorld.png");
	CCSprite* pSprite = CCSprite::spriteWithFile("level-01.jpg");

	// position the sprite on the center of the screen
	pSprite->setPosition( ccp(size.width/2, size.height/2) );

	// add the sprite as a child to this layer
	this->addChild(pSprite, 0);

	LineLayer * line = LineLayer::node();
	this->addChild(line, 10);
	
	return true;
}

void LineLayer::draw()
{
	// glEnable(GL_LINE_SMOOTH);
	// glColor4ub(255, 0, 0, 255);
	glColor4f(1.0, 0, 0, 1);
	glLineWidth(6.0);
	// glPointSize();
	ccDrawLine(ccp(10, 10), ccp(100, 50));
}

void MyWorld::actionFinished(CCNode * sender)
{
	// int i = 0;
	CCTransitionScene * transition = CCTransitionFlipAngular::transitionWithDuration(1.0, MonsterHome::scene()); 
	// CCEaseElasticInOut
	CCDirector::sharedDirector()->replaceScene(transition);
}

void MyWorld::menuCloseCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

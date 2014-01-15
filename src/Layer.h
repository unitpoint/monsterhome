#ifndef __LAYER_H__
#define __LAYER_H__

#include <cocos2d.h>
#include <EasyLib.h>
#include <MathLib.h>

typedef void (*SEL_CallFuncUP)(cocos2d::CCObject * target, void * user_param);

class CallFuncUP : public cocos2d::CCCallFunc
{
	typedef cocos2d::CCCallFunc super;

public:

	/** creates the action with the callback and the data to pass as an argument */
	static CallFuncUP * actionWithTarget(CCObject* pSelectorTarget, SEL_CallFuncUP selector, void * user_param);
	/** initializes the action with the callback and the data to pass as an argument */
	virtual bool initWithTarget(CCObject* pSelectorTarget, SEL_CallFuncUP selector, void * user_param);
	// super methods
	virtual CCObject* copyWithZone(cocos2d::CCZone *pZone);
	virtual void execute();

protected:

	SEL_CallFuncUP func;
	void * user_param;
};

class EasyAction: public cocos2d::CCActionEase, public EasyLib
{
	typedef cocos2d::CCActionEase super;

protected:

	EMethod method;
	EDirType dir_type;

public:

	virtual void update(cocos2d::ccTime time);
	virtual CCActionInterval* reverse(void);
	virtual CCObject* copyWithZone(cocos2d::CCZone* pZone);

public:

	/** creates the action */
	static EasyAction * actionWithAction(CCActionInterval * pAction, EMethod method, EDirType dir_type = EasyLib::FORWARD);
};

class Layer: public cocos2d::CCLayer
{
	typedef cocos2d::CCLayer super;

public:

	LAYER_NODE_FUNC(Layer);

	virtual bool init();

	template <class T> cocos2d::CCFiniteTimeAction * getFuncAction(T func)
	{
		return cocos2d::CCCallFuncN::actionWithTarget(this, (cocos2d::SEL_CallFuncN)func);
	}
	// cocos2d::CCFiniteTimeAction * getFuncAction(void (Layer::*func)(cocos2d::CCNode * sender));
	cocos2d::CCFiniteTimeAction * getFuncAction(SEL_CallFuncUP func, void * user_param = NULL);

	template <class T> void scheduleOnce(T func, float delay = 0.00001f)
	{
		cocos2d::CCFiniteTimeAction * action = getFuncAction(func);
		if(delay > 0.0f){
			action = cocos2d::CCSequence::actionOneTwo(
				cocos2d::CCDelayTime::actionWithDuration(delay),
				action);
		}
		runAction(action);
	}

	cocos2d::CCFiniteTimeAction * scheduleOnce(SEL_CallFuncUP func, float delay = 0.00001f, void * user_param = NULL);

	virtual Rect2 getRect();

	static void pauseAll(cocos2d::CCNode*);
	static void resumeAll(cocos2d::CCNode*);

	static void setOpacityAll(cocos2d::CCNode*, GLubyte opacity, bool set_this_opacity = true);

	enum {
		ALIGN_LEFT = 1<<0,
		ALIGN_RIGHT = 1<<1,
		ALIGN_CENTER = 1<<2,
		ALIGN_TOP = 1<<3,
		ALIGN_BOTTOM = 1<<4,
		ALIGN_MIDDLE = 1<<5,
	};

	static void tableLayout(cocos2d::CCNode*, int cols, int rows, float padding, int align = ALIGN_LEFT | ALIGN_TOP, bool locate_unvisible = true);

	void pauseAll();
	void resumeAll();

	static void tileRectWithSprites(cocos2d::CCNode*, const Rect2& rect, cocos2d::CCTexture2D*, const Rect2& texture_rect, const vec2& scale = vec2(1,1));
	static void tileRectWithSprites(cocos2d::CCNode*, const Rect2& rect, cocos2d::CCTexture2D*);
	static void tileRectWithSprites(cocos2d::CCNode*, const Rect2& rect, cocos2d::CCTexture2D*, const vec2& scale);
};

class WindowLayer: public Layer
{
	typedef Layer super;

protected:

	Layer * background_layer;
	cocos2d::CCTexture2D * background_texture;
	vec2 left_top_size;
	vec2 right_bottom_size;

	vec2 min_content_size;

	WindowLayer();
	virtual ~WindowLayer();

	void resetBackground();
	void populateBackground();

public:

	LAYER_NODE_FUNC(WindowLayer);

	Layer * getBackgroundLayer(){ return background_layer; }
	cocos2d::CCTexture2D * getBackgroundTexture(){ return background_texture; }
	vec2 getLeftTopFrameSize(){ return left_top_size; }
	vec2 getRightBottomSize(){ return right_bottom_size; }
	vec2 getMinContentSize(){ return min_content_size; }

	virtual bool init();
	virtual void setFrames(cocos2d::CCTexture2D*, const vec2& left_top_size, const vec2& right_bottom_size, const vec2& min_content_size);
	void setFrames(cocos2d::CCTexture2D*, const vec2& left_top_size, const vec2& right_bottom_size);

	virtual void setContentSize(cocos2d::CCSize);
	
	float getBackgroundScale();
	virtual void setBackgroundScale(float scale);
};

class RGBAProtocolDummy: public cocos2d::CCRGBAProtocol
{
	typedef cocos2d::CCRGBAProtocol super;

public:

	RGBAProtocolDummy(){}

    virtual void setColor(const cocos2d::ccColor3B& color);
	virtual const cocos2d::ccColor3B& getColor();
    virtual GLubyte getOpacity();
	virtual void setOpacity(GLubyte opacity);
	virtual void setIsOpacityModifyRGB(bool bValue);
	virtual bool getIsOpacityModifyRGB();
};

class SmartLabel: public Layer, public cocos2d::CCLabelProtocol, public cocos2d::CCRGBAProtocol
{
	typedef Layer super;

protected:

	cocos2d::CCLabelBMFont * label;
	cocos2d::CCLabelBMFont * shadow_label;

	SmartLabel();

public:

	LAYER_NODE_FUNC(SmartLabel);

	// static SmartLabel * labelWithString(const char *label, const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const vec2& offs, const cocos2d::CCSize& dimensions, cocos2d::CCTextAlignment alignment, const char *fontName);
	static SmartLabel * labelWithString(const char *label, const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontName, 
		float width = cocos2d::kCCLabelAutomaticWidth, cocos2d::CCTextAlignment alignment = cocos2d::CCTextAlignmentLeft, const vec2& offs = vec2(0,0));

	virtual const char * getString();
	virtual void setString(const char *label);

    virtual void setColor(const cocos2d::ccColor3B& color);
	virtual const cocos2d::ccColor3B& getColor();
    virtual GLubyte getOpacity();
	virtual void setOpacity(GLubyte opacity);
	virtual void setIsOpacityModifyRGB(bool bValue);
	virtual bool getIsOpacityModifyRGB();

	cocos2d::ccColor3B getShadowColor();
	void setShadowColor(const cocos2d::ccColor3B&);

	vec2 getShadowOffs();
	void setShadowOffs(const vec2& offs);
};

class SmartButton: public Layer, public cocos2d::CCRGBAProtocol
{
	typedef Layer super;

protected:

	cocos2d::CCSprite * sprite;
	SmartLabel * label;
	float label_scale_x;

	SmartButton();

public:

	// LAYER_NODE_FUNC(SmartButton);

	static SmartButton * createButton(const char * label, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale);

	virtual bool init(const char * label, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale);

	cocos2d::CCSprite * getSprite(){ return sprite; }
	SmartLabel * getLabel(){ return label; }

	void setLabelScale(float);

    virtual void setColor(const cocos2d::ccColor3B& color);
	virtual const cocos2d::ccColor3B& getColor();
    virtual GLubyte getOpacity();
	virtual void setOpacity(GLubyte opacity);
	virtual void setIsOpacityModifyRGB(bool bValue);
	virtual bool getIsOpacityModifyRGB();
};

class ItemsWindow: public WindowLayer
{
	typedef WindowLayer super;

protected:

	cocos2d::CCMutableArray<cocos2d::CCNode*> * items;
	// SmartLabel * title;
	// SmartLabel * message;
	// cocos2d::CCMenu * buttons_menu;
	float padding;

	ItemsWindow();
	virtual ~ItemsWindow();

public:

	static SmartButton * createLabel(const char * label, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale = 1.0f);

	static cocos2d::CCSprite * createLabelBM(const char * label, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const char *fontname);

	static cocos2d::CCMenuItemSprite * createLabelButton(const char * label,
		cocos2d::CCTexture2D * texture_normal, cocos2d::CCTexture2D * texture_pressed,
		const cocos2d::ccColor3B& color_normal, const cocos2d::ccColor3B& color_pressed, 
		const cocos2d::ccColor3B& shadow_color, 
		const char * fontname, const char * fontname_pressed, float sprite_scale,
		// float fontsize_normal, float fontsize_pressed,
		cocos2d::CCObject * target, cocos2d::SEL_MenuHandler selector);

	static cocos2d::CCMenu * createButtonMenu(cocos2d::CCMenuItem * button);
	// static cocos2d::CCMenu * createButtonsMenu(cocos2d::CCMenuItem * button, ...);
	// static cocos2d::CCMenu * prepareMenu(cocos2d::CCMenu * buttons_menu);

	static Rect2 getNodeChildrenRect(cocos2d::CCNode*);

	static ItemsWindow * create(float window_width, float padding,
		cocos2d::CCTexture2D*, const vec2& left_top_size, const vec2& right_bottom_size,
		cocos2d::CCMutableArray<cocos2d::CCNode*> * items);

	virtual bool initWithItems(float window_width, float padding,
		cocos2d::CCTexture2D*, const vec2& left_top_size, const vec2& right_bottom_size,
		cocos2d::CCMutableArray<cocos2d::CCNode*> * items);

	virtual void layout();
};

#endif // __LAYER_H__
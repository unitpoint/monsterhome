#include <Layer.h>
#include <MathLib.h>

// ===============================================================================
// ===============================================================================
// ===============================================================================

CallFuncUP * CallFuncUP::actionWithTarget(CCObject* pSelectorTarget, SEL_CallFuncUP selector, void* d)
{
	CallFuncUP* ret = new CallFuncUP();

	if (ret && ret->initWithTarget(pSelectorTarget, selector, d)) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return NULL;
}

bool CallFuncUP::initWithTarget(CCObject* pSelectorTarget, SEL_CallFuncUP selector, void* d)
{
	if (super::initWithTarget(pSelectorTarget)) {
		user_param = d;
		func = selector;
		return true;
	}

	return false;
}

cocos2d::CCObject * CallFuncUP::copyWithZone(cocos2d::CCZone* zone)
{
	cocos2d::CCZone* pNewZone = NULL;
	CallFuncUP* ret = NULL;

	if (zone && zone->m_pCopyObject) {
		//in case of being called at sub class
		ret = (CallFuncUP*) (zone->m_pCopyObject);
	} else {
		ret = new CallFuncUP();
		zone = pNewZone = new cocos2d::CCZone(ret);
	}

	CCCallFunc::copyWithZone(zone);
	ret->initWithTarget(m_pSelectorTarget, func, user_param);
	CC_SAFE_DELETE(pNewZone);
	return ret;
}

void CallFuncUP::execute()
{
	if (func) {
		// m_pTarget
		func(m_pSelectorTarget, user_param);
	}
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

EasyAction * EasyAction::actionWithAction(CCActionInterval* pAction, EMethod method, EDirType dir_type)
{
	EasyAction * action = new EasyAction();
	if (action)
	{
		if (action->initWithAction(pAction))
		{
			action->autorelease();
			action->method = method;
			action->dir_type = dir_type;
		}
		else
		{
			CC_SAFE_RELEASE_NULL(action);
		}
	}

	return action;      
}

cocos2d::CCObject * EasyAction::copyWithZone(cocos2d::CCZone *pZone)
{
	cocos2d::CCZone* pNewZone = NULL;
	EasyAction* pCopy = NULL;
	if(pZone && pZone->m_pCopyObject) 
	{
		//in case of being called at sub class
		pCopy = (EasyAction*)(pZone->m_pCopyObject);
	}
	else
	{
		pCopy = new EasyAction();
		pZone = pNewZone = new cocos2d::CCZone(pCopy);
	}

	pCopy->initWithAction((CCActionInterval *)(m_pOther->copy()->autorelease()));
	pCopy->method = method;
	pCopy->dir_type = dir_type;
		
	CC_SAFE_DELETE(pNewZone);
	return pCopy;
}

void EasyAction::update(cocos2d::ccTime time)
{
	// b + c * t / d;
	time = EasyLib::Run(dir_type, method, time, 0.0f, 1.0f, 1.0f);
	m_pOther->update(time);
}

cocos2d::CCActionInterval* EasyAction::reverse(void)
{
	EDirType dir_type = this->dir_type == FORWARD ? BACK : FORWARD;
	return EasyAction::actionWithAction(m_pOther->reverse(), method, dir_type);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

class LabelBMFontConfig: public cocos2d::CCLabelBMFont
{
public:

	cocos2d::CCBMFontConfiguration * getFontConfig(){ return m_pConfiguration; }
};

SmartLabel::SmartLabel()
{
	label = NULL;
	shadow_label = NULL;
}

SmartLabel * SmartLabel::labelWithString(const char *label, const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontName,
	float width, cocos2d::CCTextAlignment alignment, const vec2& offs)
{
	SmartLabel * ret = SmartLabel::node();
	if(ret){
		ret->label = cocos2d::CCLabelBMFont::labelWithString(label, fontName, width, alignment, offs);
		ret->label->setColor(color);
		ret->addChild(ret->label, 1);
		ret->setContentSize(ret->label->getContentSize());

		ret->setAnchorPoint(ret->label->getAnchorPoint());
		ret->label->setAnchorPoint(vec2(0,0));

		ret->setIsRelativeAnchorPoint(ret->label->getIsRelativeAnchorPoint());
		
		ret->shadow_label = cocos2d::CCLabelBMFont::labelWithString(label, fontName, width, alignment, offs);
		ret->shadow_label->setColor(shadow_color);
		ret->shadow_label->setAnchorPoint(vec2(0, 0));
		cocos2d::CCBMFontConfiguration * font_config = ((LabelBMFontConfig*)ret->shadow_label)->getFontConfig();
		ret->shadow_label->setPosition((vec2(0.05f, 0.05f) * (float)font_config->m_uCommonHeight).Max(vec2(1.0f, 1.0f)) * vec2(1.0f, -1.0f));
		ret->addChild(ret->shadow_label, 0);

		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

const char * SmartLabel::getString()
{
	return label->getString();
}

void SmartLabel::setString(const char *str)
{
	label->setString(str);
	if(shadow_label){
		shadow_label->setString(str);
	}
	setContentSize(label->getContentSize());
}

void SmartLabel::setColor(const cocos2d::ccColor3B& color)
{
	label->setColor(color);
}

const cocos2d::ccColor3B& SmartLabel::getColor()
{
	return label->getColor();
}

GLubyte SmartLabel::getOpacity()
{
	return label->getOpacity();
}

void SmartLabel::setOpacity(GLubyte opacity)
{
	label->setOpacity(opacity);
	if(shadow_label){
		shadow_label->setOpacity(opacity);
	}
}

void SmartLabel::setIsOpacityModifyRGB(bool bValue)
{
	label->setIsOpacityModifyRGB(bValue);
	if(shadow_label){
		shadow_label->setIsOpacityModifyRGB(bValue);
	}
}

bool SmartLabel::getIsOpacityModifyRGB()
{
	return label->getIsOpacityModifyRGB();
}

cocos2d::ccColor3B SmartLabel::getShadowColor()
{
	return shadow_label ? shadow_label->getColor() : cocos2d::ccc3(0,0,0);
}

void SmartLabel::setShadowColor(const cocos2d::ccColor3B& color)
{
	if(shadow_label){
		shadow_label->setColor(color);
	}
}

vec2 SmartLabel::getShadowOffs()
{
	return shadow_label ? vec2(shadow_label->getPosition()) : vec2(0, 0);
}

void SmartLabel::setShadowOffs(const vec2& offs)
{
	if(shadow_label){
		shadow_label->setPosition(offs);
	}
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

bool Layer::init()
{
	if(super::init()){
		// setIsRelativeAnchorPoint(true);
		return true;
	}
	return false;
}

cocos2d::CCFiniteTimeAction * Layer::getFuncAction(SEL_CallFuncUP func, void * user_param)
{
	return CallFuncUP::actionWithTarget(this, func, user_param);
}

cocos2d::CCFiniteTimeAction * Layer::scheduleOnce(SEL_CallFuncUP func, float delay, void * user_param)
{
	cocos2d::CCFiniteTimeAction * action = getFuncAction(func, user_param);
	if(delay > 0.0f){
		action = cocos2d::CCSequence::actionOneTwo(
			cocos2d::CCDelayTime::actionWithDuration(delay),
			action);
	}
	runAction(action);
	return action;
}

Rect2 Layer::getRect()
{
	vec2 size = getContentSize();
#if 1
	return Rect2(vec2(getPosition()) - size * getAnchorPoint(), size);
#else
	Rect2 rect = Rect2(vec2(getPosition()) - size * getAnchorPoint(), size);
	if(isnanf(rect.origin.x)){
		CCPoint p = getPosition();
		int i = 0;
	}
	return rect;
#endif
}

void Layer::pauseAll(cocos2d::CCNode * node)
{
	node->pauseSchedulerAndActions();
	CCObject* obj = NULL;
	CCARRAY_FOREACH(node->getChildren(), obj){
		cocos2d::CCNode * sub_node = dynamic_cast<cocos2d::CCNode*>(obj);
		if(sub_node){
			pauseAll(sub_node);
		}
	}
}

void Layer::resumeAll(cocos2d::CCNode * node)
{
	node->resumeSchedulerAndActions();
	CCObject* obj = NULL;
	CCARRAY_FOREACH(node->getChildren(), obj){
		cocos2d::CCNode * sub_node = dynamic_cast<cocos2d::CCNode*>(obj);
		if(sub_node){
			resumeAll(sub_node);
		}
	}
}

void Layer::setOpacityAll(cocos2d::CCNode * node, GLubyte opacity, bool set_this_opacity)
{
	if(set_this_opacity){
		cocos2d::CCRGBAProtocol * rgba = dynamic_cast<cocos2d::CCRGBAProtocol*>(node);
		if(rgba){
			rgba->setOpacity(opacity);
		}
	}
	CCObject* obj = NULL;
	CCARRAY_FOREACH(node->getChildren(), obj){
		cocos2d::CCNode * sub_node = dynamic_cast<cocos2d::CCNode*>(obj);
		if(sub_node){
			setOpacityAll(sub_node, opacity, true);
		}
	}
}

void Layer::tableLayout(cocos2d::CCNode * node, int cols, int rows, float padding, int align, bool locate_unvisible)
{
	struct Lib
	{
		struct Pos {
			int x, y;

			Pos(int ax, int ay)
			{
				x = ax;
				y = ay;
			}
		};

		cocos2d::CCNode * node;
		int cols;
		int rows;
		float padding;
		int align;
		bool locate_unvisible;

		cocos2d::CCArray * children;

		Vector<float> widths;
		Vector<float> heights;

		Lib(cocos2d::CCNode * node, int cols, int rows, float padding, int align, bool locate_unvisible)
		{
			this->node = node;
			this->cols = cols;
			this->rows = rows;
			this->padding = padding;
			this->align = align;
			this->locate_unvisible = locate_unvisible;
			children = node->getChildren();
			layout();
		}

		void reserve(Vector<float>& vec, int count)
		{
			while((int)vec.size() < count){
				vec.push_back(0);
			}
		}

		void layout()
		{
			int i, real_index, count = (int)children->count();
			if(cols > 0){
				reserve(widths, cols);
				reserve(heights, (int)((float)count / cols + 0.5f));
			}else{
				reserve(heights, rows);
				reserve(widths, (int)((float)count / rows + 0.5f));
			}
			real_index = 0;
			for(i = 0; i < count; i++){
				cocos2d::CCNode * child = dynamic_cast<cocos2d::CCNode*>(children->objectAtIndex(i));
				if(!locate_unvisible && !child->getIsVisible()){
					continue;
				}
				Pos pos = indexToPos( real_index++ );
				vec2 size = child->getContentSize();
				size.x *= child->getScaleX();
				size.y *= child->getScaleY();

				reserve(widths, pos.x+1);
				reserve(heights, pos.y+1);

				widths[pos.x] = MathLib::Max(widths[pos.x], size.x);
				heights[pos.y] = MathLib::Max(heights[pos.y], size.y);
			}

			Vector<float> col_offs;
			Vector<float> row_offs;
			reserve(col_offs, widths.size());
			reserve(row_offs, heights.size());

			float width = 0, height = 0;
			for(i = 0; i < (int)widths.size(); i++){
				float pad = i > 0 ? padding : 0;
				col_offs[i] = width + pad;
				width += widths[i] + pad;
			}
			for(i = 0; i < (int)heights.size(); i++){
				float pad = i > 0 ? padding : 0;
				row_offs[i] = height + pad;
				height += heights[i] + pad;
			}
			real_index = 0;
			for(i = 0; i < count; i++){
				cocos2d::CCNode * child = dynamic_cast<cocos2d::CCNode*>(children->objectAtIndex(i));
				if(!locate_unvisible && !child->getIsVisible()){
					continue;
				}
				Pos pos = indexToPos( real_index++ );
				pos.y = heights.size()-1 - pos.y;
				vec2 size = child->getContentSize();
				size.x *= child->getScaleX();
				size.y *= child->getScaleY();
				vec2 anchor = child->getAnchorPoint();
				float x = -width/2 + col_offs[pos.x];
				if(align & ALIGN_CENTER){
					x += (widths[pos.x] - size.x)/2;
				}else if(align & ALIGN_RIGHT){
					x += widths[pos.x] - size.x;
				}
				float y = -height/2 + row_offs[pos.y];
				if(align & ALIGN_MIDDLE){
					y += (heights[pos.y] - size.y)/2;
				}else if(align & ALIGN_BOTTOM){
					y += heights[pos.y] - size.y;
				}
				child->setPosition(vec2(x, y) + size * anchor);
			}
		}

		Pos indexToPos(int i)
		{
			int x, y;
			if(cols > 0){
				x = i % cols;
				y = i / cols;
			}else{
				y = i % rows;
				x = i / rows;
			}
			return Pos(x, y);
		}

	} make_layout(node, cols, rows, padding, align, locate_unvisible);
}

void Layer::tileRectWithSprites(cocos2d::CCNode * node, const Rect2& rect, cocos2d::CCTexture2D * texture)
{
	Layer::tileRectWithSprites(node, rect, texture, Rect2(vec2(0,0), texture->getContentSize()));
}

void Layer::tileRectWithSprites(cocos2d::CCNode * node, const Rect2& rect, cocos2d::CCTexture2D * texture, const vec2& scale)
{
	Layer::tileRectWithSprites(node, rect, texture, Rect2(vec2(0,0), texture->getContentSize()), scale);
}

void Layer::tileRectWithSprites(cocos2d::CCNode * node, const Rect2& rect, cocos2d::CCTexture2D * texture, const Rect2& texture_rect, const vec2& scale)
{
	vec2 real_sprite_size = texture_rect.size * scale;
	for(float y = 0; y < rect.size.y; y += real_sprite_size.y){
		for(float x = 0; x < rect.size.x; x += real_sprite_size.x){
			Rect2 sprite_rect(texture_rect.origin, texture_rect.size.Min(vec2((rect.size.x - x) / scale.x, (rect.size.y - y) / scale.y)));
			cocos2d::CCSprite * sprite = cocos2d::CCSprite::spriteWithTexture(texture, sprite_rect);
			sprite->setAnchorPoint(vec2(0, 1));
			sprite->setPosition(vec2(x + rect.origin.x, rect.origin.y + rect.size.y - y)); // - real_sprite_size.y));
			sprite->setScaleX(scale.x);
			sprite->setScaleY(scale.y);
			node->addChild(sprite);
		}
	}
}

void Layer::pauseAll()
{
	pauseAll(this);
}

void Layer::resumeAll()
{
	resumeAll(this);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

WindowLayer::WindowLayer()
{
	background_layer = NULL;
	background_texture = NULL;
}

WindowLayer::~WindowLayer()
{
	CC_SAFE_RELEASE(background_texture);
}

bool WindowLayer::init()
{
	if(super::init()){		
		setIsRelativeAnchorPoint(true);
		background_layer = Layer::node();
		background_layer->setAnchorPoint(vec2(0, 0));
		addChild(background_layer);
		return true;
	}
	return false;
}

void WindowLayer::setFrames(cocos2d::CCTexture2D * texture, const vec2& left_top_size, const vec2& right_bottom_size)
{
	setFrames(texture, left_top_size, right_bottom_size, min_content_size);
}

void WindowLayer::resetBackground()
{
	if(background_layer){
		background_layer->removeAllChildrenWithCleanup(true);
	}
}

void WindowLayer::setFrames(cocos2d::CCTexture2D * texture, const vec2& left_top_size, const vec2& right_bottom_size, const vec2& min_content_size)
{
	bool changed = false;
	if(background_texture != texture){
		CC_SAFE_RELEASE_NULL(background_texture);
		background_texture = texture;
		CC_SAFE_RETAIN(texture);
		changed = true;
	}
	if(this->left_top_size != left_top_size){
		this->left_top_size = left_top_size;
		changed = true;
	}
	if(this->right_bottom_size != right_bottom_size){
		this->right_bottom_size = right_bottom_size;
		changed = true;
	}
	if(this->min_content_size != min_content_size){
		this->min_content_size = min_content_size;
		changed = true;
	}

	ASSERT(background_layer);
	if(!changed && background_layer->getChildrenCount() > 0){
		return;
	}

	populateBackground();
}

void WindowLayer::populateBackground()
{
	ASSERT(background_layer);
	background_layer->removeAllChildrenWithCleanup(true);

	if(!background_texture){
		return;
	}

	vec2 size = background_layer->getContentSize();
	vec2 texture_size = background_texture->getContentSize();

	Rect2 left_top(vec2(0,0), left_top_size);
	Rect2 right_top(texture_size.x - right_bottom_size.x, 0, right_bottom_size.x, left_top_size.y);
	Rect2 horiz_top(left_top_size.x, 0, texture_size.x - left_top_size.x - right_bottom_size.x, left_top_size.y);

	Rect2 left_middle(0, left_top_size.y, left_top_size.x, texture_size.y - left_top_size.y - right_bottom_size.y);
	Rect2 horiz_middle(left_top_size.x, left_top_size.y, horiz_top.size.x, left_middle.size.y);
	Rect2 right_middle(right_top.origin.x, left_top_size.y, right_bottom_size.x, left_middle.size.y);

	Rect2 left_bottom(0, texture_size.y - right_bottom_size.y, left_top_size.x, right_bottom_size.y);
	Rect2 horiz_bottom(left_top_size.x, left_bottom.origin.y, horiz_top.size.x, right_bottom_size.y);
	Rect2 right_bottom(texture_size.x - right_bottom_size.x, left_bottom.origin.y, right_bottom_size.x, right_bottom_size.y);

	vec2 middle_size(size.x - left_top_size.x - right_bottom_size.x, size.y - left_top_size.y - right_bottom_size.y);
	if(middle_size.x < 0){
		int i;
		float width_offs = MathLib::Floor(-middle_size.x / 2);
		Rect2 * left[] = {&left_top, &left_middle, &left_bottom};
		for(i = 0; i < 3; i++){
			left[i]->size.x -= width_offs;
		}
		width_offs = -middle_size.x - width_offs;
		Rect2 * right[] = {&right_top, &right_middle, &right_bottom};
		for(i = 0; i < 3; i++){
			right[i]->origin.x += width_offs;
			right[i]->size.x -= width_offs;
		}
	}
	if(middle_size.y < 0){
		int i;
		float height_offs = MathLib::Floor(-middle_size.y / 2);
		Rect2 * top[] = {&left_top, &right_top, &horiz_top};
		for(i = 0; i < 3; i++){
			top[i]->size.y -= height_offs;
		}
		height_offs = -middle_size.y - height_offs;
		Rect2 * bottom[] = {&left_bottom, &horiz_bottom, &right_bottom};
		for(i = 0; i < 3; i++){
			bottom[i]->origin.y += height_offs;
			bottom[i]->size.y -= height_offs;
		}
	}
	
	cocos2d::CCSprite * sprite = cocos2d::CCSprite::spriteWithTexture(background_texture, left_top);
	sprite->setAnchorPoint(vec2(0,0));
	sprite->setPosition(vec2(0, size.y - left_top.size.y));
	background_layer->addChild(sprite);

	sprite = cocos2d::CCSprite::spriteWithTexture(background_texture, right_top);
	sprite->setAnchorPoint(vec2(0,0));
	sprite->setPosition(vec2(size.x - right_top.size.x, size.y - right_top.size.y));
	background_layer->addChild(sprite);

	sprite = cocos2d::CCSprite::spriteWithTexture(background_texture, left_bottom);
	sprite->setAnchorPoint(vec2(0,0));
	sprite->setPosition(vec2(0,0));
	background_layer->addChild(sprite);

	sprite = cocos2d::CCSprite::spriteWithTexture(background_texture, right_bottom);
	sprite->setAnchorPoint(vec2(0,0));
	sprite->setPosition(vec2(size.x - right_bottom.size.x,0));
	background_layer->addChild(sprite);

	tileRectWithSprites(background_layer, Rect2(left_top.size.x, size.y - left_top.size.y, middle_size.x, left_top.size.y), background_texture, horiz_top);
	tileRectWithSprites(background_layer, Rect2(0, left_top.size.y, left_top.size.x, middle_size.y), background_texture, left_middle);
	tileRectWithSprites(background_layer, Rect2(left_top.size.x, 0, middle_size.x, right_bottom.size.y), background_texture, horiz_bottom);
	tileRectWithSprites(background_layer, Rect2(size.x - right_bottom.size.x, right_bottom.size.y, right_bottom.size.x, middle_size.y), background_texture, right_middle);
	tileRectWithSprites(background_layer, Rect2(left_top.size.x, right_bottom.size.y, middle_size.x, middle_size.y), background_texture, horiz_middle);
}

void WindowLayer::setContentSize(cocos2d::CCSize size)
{
	size.width = MathLib::Max(size.width, min_content_size.x);
	size.height = MathLib::Max(size.height, min_content_size.y);
	super::setContentSize(size);
	background_layer->setContentSize(vec2(size) / vec2(background_layer->getScaleX(), background_layer->getScaleY()));
	populateBackground();
}

float WindowLayer::getBackgroundScale()
{
	return background_layer->getScale();
}

void WindowLayer::setBackgroundScale(float scale)
{
	if(background_layer->getScale() != scale){
		background_layer->setScale(scale);
		background_layer->setContentSize(vec2(getContentSize()) / vec2(background_layer->getScaleX(), background_layer->getScaleY()));
		populateBackground();
	}
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

void RGBAProtocolDummy::setColor(const cocos2d::ccColor3B&)
{
}

const cocos2d::ccColor3B& RGBAProtocolDummy::getColor()
{
	static cocos2d::ccColor3B color = {255, 255, 255};
	return color;
}

GLubyte RGBAProtocolDummy::getOpacity()
{
	return 255;
}

void RGBAProtocolDummy::setOpacity(GLubyte)
{
}
		
void RGBAProtocolDummy::setIsOpacityModifyRGB(bool)
{
}

bool RGBAProtocolDummy::getIsOpacityModifyRGB()
{
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

SmartButton::SmartButton()
{
	sprite = NULL;
	label = NULL;
	label_scale_x = 1.0f;
}

SmartButton * SmartButton::createButton(const char * label, cocos2d::CCTexture2D * texture, 
	const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale)
{
	SmartButton * button = new SmartButton();
	if(button && button->init(label, texture, color, shadow_color, fontname, sprite_scale)){
		button->autorelease();
		return button;
	}
	delete button;
	return NULL;
}

bool SmartButton::init(const char * str, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale)
{
	if(super::init()){
		setIsRelativeAnchorPoint(true);
		setAnchorPoint(vec2(0.5f, 0.5f));

		sprite = cocos2d::CCSprite::spriteWithTexture(texture);
		sprite->setScale(sprite_scale);
		sprite->setAnchorPoint(vec2(0, 0));
		sprite->setPosition(vec2(0, 0));
		addChild(sprite);
		setContentSize(vec2(sprite->getContentSize()) * sprite_scale);
	
		label = SmartLabel::labelWithString(str, color, shadow_color, fontname);
		label->setPosition( vec2(getContentSize()) * 0.5f );
		addChild(label);

		label_scale_x = 1.0f;
		float max_label_width = getContentSize().width * 0.85f;
		float label_width = label->getContentSize().width;
		if(label_width > max_label_width){
			label->setScaleX(label_scale_x = max_label_width / label_width);
		}

		return true;
	}
	return false;
}

void SmartButton::setLabelScale(float scale)
{
	label->setScaleX(label_scale_x * scale);
	label->setScaleY(scale);
}

void SmartButton::setColor(const cocos2d::ccColor3B& color)
{
	sprite->setColor(color);
}

const cocos2d::ccColor3B& SmartButton::getColor()
{
	return sprite->getColor();
}

GLubyte SmartButton::getOpacity()
{
	return sprite->getOpacity();
}

void SmartButton::setOpacity(GLubyte opacity)
{
	sprite->setOpacity(opacity);
}

void SmartButton::setIsOpacityModifyRGB(bool bValue)
{
	sprite->setIsOpacityModifyRGB(bValue);
}

bool SmartButton::getIsOpacityModifyRGB()
{
	return sprite->getIsOpacityModifyRGB();
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

ItemsWindow::ItemsWindow()
{
	// title = NULL;
	// message = NULL;
}

ItemsWindow::~ItemsWindow()
{
	CC_SAFE_RELEASE_NULL(items);
}

ItemsWindow * ItemsWindow::create(float window_width, float padding,
		cocos2d::CCTexture2D * texture, const vec2& left_top_size, const vec2& right_bottom_size,
		cocos2d::CCMutableArray<cocos2d::CCNode*> * items)
{
	ItemsWindow * window = new ItemsWindow();
	if(window->initWithItems(window_width, padding, texture, left_top_size, right_bottom_size, items)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

bool ItemsWindow::initWithItems(float window_width, float padding,
		cocos2d::CCTexture2D * texture, const vec2& left_top_size, const vec2& right_bottom_size,
		cocos2d::CCMutableArray<cocos2d::CCNode*> * items
	)
{
	if(super::init()){
		this->items = items;
		items->retain();
		for(int i = 0; i < (int)items->count(); i++){
			addChild(items->getObjectAtIndex(i));
		}
		// this->title = title;
		// this->message = message;
		// this->buttons_menu = buttons_menu;
		this->padding = padding;

		setContentSize(vec2(window_width, 100));
		layout();
		setFrames(texture, left_top_size, right_bottom_size);

		return true;
	}
	return false;
}

void ItemsWindow::layout()
{
	if(!items){
		return;
	}
	vec2 size = getContentSize();
	float y_bottom = padding;
	
	int count = items->count();
	for(int i = count-1; i >= 0; i--){
		cocos2d::CCNode * item = items->getObjectAtIndex(i);
		if(dynamic_cast<cocos2d::CCMenu*>(item)){
			Rect2 rect = getNodeChildrenRect(item);
			// buttons_menu->setAnchorPoint(vec2(0.5f, 0));
			// buttons_menu->setIsRelativeAnchorPoint(true);
			// buttons_menu->setPosition(vec2(size.x/2, y_bottom));
			item->setPosition(vec2(size.x/2, y_bottom + rect.size.y/2));
			// addChild(buttons_menu);
			// y_bottom += buttons_menu->getContentSize().height + padding;
			y_bottom += rect.size.y + padding;
		}else{
			float anchor_x = 0.5f;
			/* if(dynamic_cast<cocos2d::CCLabelTTF*>(item)){
				// float width = item->getContentSize().width;
				anchor_x = 0.49f; // (width/2+1) / width;
			} */
			item->setAnchorPoint(vec2(anchor_x, 0));
			item->setPosition(vec2(size.x/2, y_bottom));
			// addChild(message);
			y_bottom += item->getContentSize().height + padding;
		}
	}
	/*
	if(buttons_menu){
		Rect2 rect = getNodeChildrenRect(buttons_menu);
		// buttons_menu->setAnchorPoint(vec2(0.5f, 0));
		// buttons_menu->setIsRelativeAnchorPoint(true);
		// buttons_menu->setPosition(vec2(size.x/2, y_bottom));
		buttons_menu->setPosition(vec2(size.x/2, y_bottom + rect.size.y/2));
		addChild(buttons_menu);
		// y_bottom += buttons_menu->getContentSize().height + padding;
		y_bottom += rect.size.y + padding;
	}
	if(message){
		message->setAnchorPoint(vec2(0.5f, 0));
		message->setPosition(vec2(size.x/2, y_bottom));
		addChild(message);
		y_bottom += message->getContentSize().height + padding;
	}
	if(title){
		title->setAnchorPoint(vec2(0.5f, 0));
		title->setPosition(vec2(size.x/2, y_bottom));
		addChild(title);
		y_bottom += title->getContentSize().height + padding;
	}
	*/
	setContentSize(vec2(size.x, y_bottom));
}

SmartButton * ItemsWindow::createLabel(const char * label, cocos2d::CCTexture2D * texture, 
	const cocos2d::ccColor3B& color, const cocos2d::ccColor3B& shadow_color, const char *fontname, float sprite_scale)
{
	return SmartButton::createButton(label, texture, color, shadow_color, fontname, sprite_scale);
}

cocos2d::CCSprite * ItemsWindow::createLabelBM(const char * label, cocos2d::CCTexture2D * texture, 
		const cocos2d::ccColor3B& color, const char *fontname)
{
	cocos2d::CCSprite * sprite = cocos2d::CCSprite::spriteWithTexture(texture);
	
	cocos2d::CCLabelBMFont * labelBM = cocos2d::CCLabelBMFont::labelWithString(label, fontname);
	labelBM->setPosition( vec2(sprite->getContentSize()) * 0.5f );
	labelBM->setColor(color);
	sprite->addChild(labelBM);

	return sprite;
}

cocos2d::CCMenuItemSprite * ItemsWindow::createLabelButton(const char * label,
		cocos2d::CCTexture2D * texture_normal, cocos2d::CCTexture2D * texture_pressed,
		const cocos2d::ccColor3B& color_normal, const cocos2d::ccColor3B& color_pressed, 
		const cocos2d::ccColor3B& shadow_color, const char * fontname, const char * fontname_pressed, float sprite_scale,
		cocos2d::CCObject * target, cocos2d::SEL_MenuHandler selector)
{
	SmartButton * button_normal = createLabel(label, texture_normal, color_normal, shadow_color, fontname, sprite_scale);
	SmartButton * button_pressed = createLabel(label, texture_pressed, color_pressed, shadow_color, fontname, sprite_scale);
	button_pressed->setLabelScale(0.9f);

	cocos2d::CCMenuItemSprite * button_item = cocos2d::CCMenuItemSprite::itemFromNormalSprite(
			button_normal,
			button_pressed,
			target,
			selector);

	return button_item;
}

cocos2d::CCMenu * ItemsWindow::createButtonMenu(cocos2d::CCMenuItem * button_item)
{
	cocos2d::CCMenu * buttons_menu = cocos2d::CCMenu::menuWithItem(button_item);
	// button_item->setAnchorPoint(vec2(0, 0));
	// button_item->setPosition(vec2(0, 0));
	// buttons_menu->setContentSize( vec2(button_item->getContentSize()) );
	buttons_menu->alignItemsHorizontally();
	return buttons_menu;
}

Rect2 ItemsWindow::getNodeChildrenRect(cocos2d::CCNode * node)
{
	Bounds2 b;
	bool first = true;
	// Rect2 rect;
    CCObject * obj = NULL;
    CCARRAY_FOREACH(node->getChildren(), obj){
        CCNode * child = dynamic_cast<CCNode*>(obj);
        if(child){
			vec2 pos = child->getPosition();
			vec2 size = vec2(child->getContentSize()) * vec2(child->getScaleX(), child->getScaleY());
			if(first){
				first = false;
				b.Init(pos);
			}else{
				b.Add(pos);
			}
			b.Add(pos + size - vec2(1,1));
		}
	}
	return Rect2(b.b[0], b.b[1] - b.b[0] + vec2(1,1));
}

/*
cocos2d::CCMenu * ItemsWindow::createButtonsMenu(cocos2d::CCMenuItem * button, ...)
{
	va_list args;
	va_start(args, button);
	cocos2d::CCMenu * buttons_menu = cocos2d::CCMenu::menuWithItems(button, args);
	buttons_menu->alignItemsHorizontallyWithPadding(button->getContentSize().height * 0.5f);
	va_end(args);
	return buttons_menu;
}
*/

/*
cocos2d::CCMenu * ItemsWindow::prepareMenu(cocos2d::CCMenu * buttons_menu)
{
    CCObject * obj = NULL;
    CCARRAY_FOREACH(buttons_menu->getChildren(), obj){
        CCNode * item = dynamic_cast<CCNode*>(obj);
        if(item){

		}
	}
	return buttons_menu;
}
*/
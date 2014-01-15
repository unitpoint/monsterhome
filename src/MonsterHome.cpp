#include "MonsterHome.h"
#include "FuncDrawLayer.h"
#include "JSON.h"
#include "CCAutoreleasePool.h"

// ===============================================================================
// ===============================================================================
// ===============================================================================

EntityInfo::Animation::Animation()
{
	autorelease();
	scale = 1.0f;
	animation = NULL;
	actor_rect_populated = false;
	anchor_populated = false;
	anchor = vec2(0.5f, 0.5f);
}
EntityInfo::Animation::~Animation()
{
	if(animation){
		animation->release();
	}
}

void EntityInfo::initWithJSON(const char * p_name, JSON * config)
{
	IW_CALLSTACK("EntityInfo::initWithJSON");
	name = p_name;
	health = 0.0f;
	speed = 1.0f;
	animations.removeAllObjects();

	JSON * root = config; // new JSON((name + ".txt").c_str());
	if(root->getChildrenCount()){
		String global_texture = App::changeExt(name, ".png");
		if(!App::isFileExist(global_texture)){
			global_texture = App::changeExt(name, ".jpg");
		}

		health = root->getChildFloat("health", 0.0f);
		speed = root->getChildFloat("speed", 1.0f);
		float global_scale = root->getChildFloat("scale", 1.0f);

		vec2 actor_rect_center, actor_rect_size, anchor;
		bool actor_rect_populated = false;
		bool anchor_populated = false;
		JSON * rect = root->getChild("rect"), * anchor_item = NULL;
		if(rect){
			actor_rect_center.x = rect->getSubChildFloat("center", "x", 0.5f);
			actor_rect_center.y = rect->getSubChildFloat("center", "y", 0.5f);
			actor_rect_size.x = rect->getChildFloat("width", 1.0f);
			actor_rect_size.y = rect->getChildFloat("height", 1.0f);
			actor_rect_populated = true;

			if(anchor_item = rect->getChild("anchor")){
				anchor.x = anchor_item->getChildFloat("x", 0.5f);
				anchor.y = anchor_item->getChildFloat("y", 0.5f);
				anchor_populated = true;

				anchor = actor_rect_center + actor_rect_size * (anchor - vec2(0.5f, 0.5f));
			}else{
				anchor = actor_rect_center;
				anchor_populated = true;
			}
		}
		if(!anchor_populated && (anchor_item = root->getChild("anchor"))){
			anchor.x = anchor_item->getChildFloat("x", 0.5f);
			anchor.y = anchor_item->getChildFloat("y", 0.5f);
			anchor_populated = true;
		}
		if(!anchor_populated){
			anchor = vec2(0.5f, 0.5f);
			anchor_populated = true;
		}

		int global_cols = root->getChildInt("cols", 1);
		int global_rows = root->getChildInt("rows", 1);
		int global_unused_frames = root->getChildInt("unused_frames", 0);
		JSON * animations = root->getChild("animations");
		int count = animations->getChildrenCount();
		for(int i = 0; i < count; i++){
			JSON * animation = animations->getChildAtIndex(i); 
			String texture = animation->getChildString("texture", global_texture);
			int cols = animation->getChildInt("cols", global_cols);
			int rows = animation->getChildInt("rows", global_rows);
			float delay = animation->getChildFloat("delay", 0.1f);
			int start = animation->getChildInt("start", 0);
			int frames = animation->getChildInt("frames", cols * rows);
			int unused_frames = animation->getChildInt("unused_frames", global_unused_frames);

			Animation * anim_info = new Animation;
			if(texture == global_texture){
				anim_info->actor_rect_center = actor_rect_center;
				anim_info->actor_rect_size = actor_rect_size;
				anim_info->actor_rect_populated = actor_rect_populated;
				anim_info->anchor = anchor;
				anim_info->anchor_populated = anchor_populated;
			}else{
				int i = 0;
			}

			if(unused_frames > 0 && start + frames > cols * rows - unused_frames){
				frames = cols * rows - unused_frames - start;
				frames = MAX(0, frames);
			}

			JSON * rect = animation->getChild("rect"), * anchor_item = NULL;
			if(rect){
				anim_info->actor_rect_center.x = rect->getSubChildFloat("center", "x", 0.5f);
				anim_info->actor_rect_center.y = rect->getSubChildFloat("center", "y", 0.5f);
				anim_info->actor_rect_size.x = rect->getChildFloat("width", 1.0f);
				anim_info->actor_rect_size.y = rect->getChildFloat("height", 1.0f);
				anim_info->actor_rect_populated = true;

				if(anchor_item = rect->getChild("anchor")){
					anim_info->anchor.x = anchor_item->getChildFloat("x", 0.5f);
					anim_info->anchor.y = anchor_item->getChildFloat("y", 0.5f);
					anim_info->anchor_populated = true;

					anim_info->anchor = anim_info->actor_rect_center + anim_info->actor_rect_size * (anim_info->anchor - vec2(0.5f, 0.5f));
				}else{
					anim_info->anchor = anim_info->actor_rect_center;
					anim_info->anchor_populated = true;
				}
			}
			if(!anim_info->anchor_populated && (anchor_item = animation->getChild("anchor"))){
				anim_info->anchor.x = anchor_item->getChildFloat("x", 0.5f);
				anim_info->anchor.y = anchor_item->getChildFloat("y", 0.5f);
				anim_info->anchor_populated = true;
			}
			if(!anim_info->anchor_populated){
				anim_info->anchor = vec2(0.5f, 0.5f);
				anim_info->anchor_populated = true;
			}
			anim_info->animation = App::initAnimation(delay, texture, cols, rows, start, frames);
			anim_info->animation->retain();

			anim_info->scale = animation->getChildFloat("scale", global_scale);

			setAnimation(animation->getName(), anim_info);
		}
	}
}

EntityInfo::Animation * EntityInfo::getAnimation(const char * name)
{
	IW_CALLSTACK("EntityInfo::getAnimation");
	return animations.objectForKey(name);
}

void EntityInfo::setAnimation(const char * name, Animation * anim_info)
{
	IW_CALLSTACK("EntityInfo::setAnimation");
	animations.setObject(anim_info, name);
	anim_info->animation->setName(name);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

SmartSprite::SmartSprite()
{
	flip_x = false;
	sprite = NULL;
	sprite_animation = NULL;
	cur_animation = NULL;
	cur_animate = NULL;
	change_animation_timeout = NULL;
}

SmartSprite::~SmartSprite()
{
	if(sprite_animation){
		sprite_animation->release();
	}
}

SmartSprite * SmartSprite::create(JSON * config, float scale)
{
	IW_CALLSTACK("SmartSprite::create");
	SmartSprite * sprite = new SmartSprite();
	if(sprite && sprite->initWithConfig(config, scale)){
		sprite->autorelease();
		return sprite;
	}
	delete sprite;
	return NULL;
}

bool SmartSprite::init()
{
	IW_CALLSTACK("SmartSprite::init");
	if(super::init()){
		setIsRelativeAnchorPoint(true);
		// setAnchorPoint(ccp(0.0f, -0.26f));
		// setAnchorPoint(ccp(0.5f, 0.26f));
		return true;
	}
	return false;
}

bool SmartSprite::initWithConfig(JSON * config, float scale)
{
	IW_CALLSTACK("SmartSprite::initWithName");
	if(init()){
		info.initWithJSON(App::changeExt(config->getName(), ""), config);
		
		EntityInfo::Animation * anim_info;
		info.animations.begin();
		while(anim_info = info.animations.next()){
			anim_info->scale *= scale;
		}

		initSpriteByAnimation();
		return true;
	}
	return false;
}

bool SmartSprite::isFlipX()
{
	return flip_x;
}

void SmartSprite::setFlipX(bool value)
{
	flip_x = value;
	if(sprite){
		sprite->setFlipX(value);
		if(cur_animation && cur_animation->anchor_populated){
			if(!value){
				sprite->setAnchorPoint(cur_animation->anchor);
			}else{
				sprite->setAnchorPoint(vec2(1.0f - cur_animation->anchor.x, cur_animation->anchor.y));
			}
		}
		updateRectCache();
	}
}

void SmartSprite::runAnimation(EntityInfo::Animation * animation, bool unsync, int repeat, bool check_timeout, SEL_CallFuncUP finished, void * user_param)
{
	IW_CALLSTACK("MonsterHome::Entity::runAnimation");
	// ASSERT(state != DEAD);
	/* if(!repeat){
		return;
	} */
	if(check_timeout && change_animation_timeout){
		return;
	}
	if(cur_animation == animation){
		return;
	}
	if(animation && repeat){
		if(cur_animation && strcmp(cur_animation->getName(), animation->getName()) == 0){
			return;
		}
		stopAnimation();

		cur_animation = animation;
		initSpriteByAnimation(cur_animation);

		CCAnimation * cocos_animation = cur_animation->animation;
		if(unsync){
			cocos_animation = App::unsyncAnimation(cocos_animation);
		}
		cur_animate = CCAnimate::actionWithAnimation(cocos_animation, false);
		if(finished){
			cur_animate = CCSequence::actionOneTwo(cur_animate, getFuncAction(finished, user_param));
		}
		if(repeat < 0){
			cur_animate = CCRepeatForever::actionWithAction(cur_animate);
		}else{
			struct Lib {
				static void finished(CCObject * target, void * user_data)
				{
					SmartSprite * ent = dynamic_cast<SmartSprite*>(target);
					if(ent->cur_animation == user_data){
						ent->cur_animation = NULL;
						ent->cur_animate = NULL;
						if(ent->change_animation_timeout){
							ent->stopAction(ent->change_animation_timeout);
							ent->change_animation_timeout = NULL;
						}
					}else{
						int i = 0;
					}
				}
			};
			if(repeat > 1){
				cur_animate = CCRepeat::actionWithAction(cur_animate, repeat);
			}
			cur_animate = CCSequence::actionOneTwo(cur_animate, getFuncAction(Lib::finished, cur_animation));
		}
		sprite->runAction(cur_animate);

		struct Lib {
			static void clearChangeAnimationTimeout(CCObject * target, void*)
			{
				SmartSprite * ent = dynamic_cast<SmartSprite*>(target);
				ent->change_animation_timeout = NULL;
			}
		};
		change_animation_timeout = CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(0.3f),
			getFuncAction(Lib::clearChangeAnimationTimeout)
		);
		runAction(change_animation_timeout);
	}else{
		stopAnimation();
	}
}

void SmartSprite::runAnimation(const char * name, bool unsync, int repeat, bool check_timeout, SEL_CallFuncUP finished, void * user_param)
{
	IW_CALLSTACK("SmartSprite::runAnimation 2");
	if(check_timeout && change_animation_timeout){
		return;
	}
	EntityInfo::Animation * animation = info.animations.objectForKey(name);
	runAnimation(animation, unsync, repeat, false, finished, user_param);
}

void SmartSprite::stopAnimation()
{
	IW_CALLSTACK("SmartSprite::stopAnimation");
	// ASSERT(state != DEAD);
	if(cur_animate){
		sprite->stopAction(cur_animate);
		cur_animate = NULL;
		cur_animation = NULL;
	}
	if(change_animation_timeout){
		stopAction(change_animation_timeout);
		change_animation_timeout = NULL;
	}
}

void SmartSprite::initSpriteByAnimation(EntityInfo::Animation * anim_info)
{
	IW_CALLSTACK("SmartSprite::initSpriteByAnimation");
	if(!anim_info){
		info.animations.begin();
		anim_info = info.animations.next(NULL);
	}
	if(anim_info && (!sprite || anim_info != sprite_animation)){
		if(sprite_animation){
			sprite_animation->release();
		}
		sprite_animation = anim_info;
		sprite_animation->retain();

		if(sprite){
			removeChild(sprite, true);
		}

		float scale = anim_info->scale;

		CCSpriteFrame * frame = anim_info->animation->getFrames()->getObjectAtIndex(0);
		sprite = CCSprite::spriteWithTexture(frame->getTexture(), frame->getRect());
		sprite->setScale(scale);
		addChild(sprite); // , type == HOLE ? 0 : 2);
	
		if(anim_info->actor_rect_populated){
			setContentSize(vec2(sprite->getContentSize()) * anim_info->actor_rect_size * scale);
		}else{
			setContentSize(vec2(sprite->getContentSize()) * scale);
		}
		if(anim_info->anchor_populated){
			sprite->setAnchorPoint(anim_info->anchor);
		}
		sprite->setPosition(vec2(getContentSize()) * 0.5f);

		setFlipX(flip_x);

		updateWarningSprite();
	}
}

void SmartSprite::updateRectCache()
{
}

void SmartSprite::updateWarningSprite()
{
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::Entity::Entity()
{
	warning_sprite = NULL;
	warning_action = NULL;
	decal = NULL;
	owner = NULL;
	path = NULL;
	path_action = NULL;
	path_sequence_action = NULL;
	path_time = 0.0f;
	fight_with = NULL;
	ignore_fight = NULL;
	damaged = 0.0f;
	state = NORMAL;
	index = -1;
	type = OBJECT;
	game = NULL;
}

MonsterHome::Entity::~Entity()
{
	IW_CALLSTACK("MonsterHome::Entity::~Entity");
	CCAssert(!owner, "");
	CCAssert(!decal, "");
	
	// resetPathPoints();
	if(path){
		path->release();
	}
	if(decal){
		decal->release();
	}
	if(fight_with){
		fight_with->release();
	}
}

/*
void MonsterHome::Entity::setSprite(CCSprite * sprite, EntityInfo::Animation * anim_info)
{
	IW_CALLSTACK("MonsterHome::Entity::setSprite");
	if(this->sprite == sprite){
		return;
	}
	if(this->sprite){
		removeChild(this->sprite, true);
	}
	this->sprite = sprite;
	addChild(sprite);
	
	if(anim_info->actor_rect_populated){
		setContentSize(vec2(sprite->getContentSize()) * anim_info->actor_rect_size);
	}else{
		setContentSize(sprite->getContentSize());
	}
	if(anim_info->anchor_populated){
		sprite->setAnchorPoint(anim_info->anchor);
	}
	sprite->setPosition(vec2(getContentSize()) * 0.5f);
}
*/

void MonsterHome::Entity::draw()
{
	IW_CALLSTACK("MonsterHome::Entity::draw");
	super::draw();

#if CC_SPRITE_DEBUG_DRAW == 1
    // draw bounding box
    CCSize s = m_tContentSize;
    CCPoint vertices[4] = {
        ccp(0,0), ccp(s.width,0),
        ccp(s.width,s.height), ccp(0,s.height)
    };
    ccDrawPoly(vertices, 4, true);
#elif CC_SPRITE_DEBUG_DRAW == 2
    // draw texture box
    const CCSize& s = m_obRect.size;
    const CCPoint& offsetPix = getOffsetPositionInPixels();
    CCPoint vertices[4] = {
        ccp(offsetPix.x,offsetPix.y), ccp(offsetPix.x+s.width,offsetPix.y),
        ccp(offsetPix.x+s.width,offsetPix.y+s.height), ccp(offsetPix.x,offsetPix.y+s.height)
    };
    ccDrawPoly(vertices, 4, true);
#endif // CC_SPRITE_DEBUG_DRAW

	if(isResponsible() && info.health > 0 && damaged > 0){
		Rect2 rect = Rect2(vec2(0, 0), getContentSize());
		rect.origin.y -= 5;
		rect.size.y = 5;

		if(isFlipX()){
			rect.origin.x = getContentSize().width - rect.size.x - rect.origin.x;
		}

		{
			float health_t = 1.0f - MathLib::Clamp(damaged / info.health, 0.0f, 1.0f);
			CCPoint vertices[4] = {
				ccp(rect.origin.x, rect.origin.y), 
				ccp(rect.origin.x + rect.size.x * health_t, rect.origin.y),
				ccp(rect.origin.x + rect.size.x * health_t, rect.origin.y + rect.size.y), 
				ccp(rect.origin.x, rect.origin.y + rect.size.y)
			};
			if(health_t > 0.7f){
				glColor4f(0, 0.78f, 0, 1.0f);
			}else if(health_t > 0.35f){
				glColor4f(0.78f, 0.78f, 0, 1.0f);
			}else{
				glColor4f(0.78f, 0, 0, 1.0f);
			}
			ccDrawPoly(vertices, 4, true, true);
		}
		{
			CCPoint vertices[4] = {
				ccp(rect.origin.x, rect.origin.y), 
				ccp(rect.origin.x + rect.size.x, rect.origin.y),
				ccp(rect.origin.x + rect.size.x, rect.origin.y + rect.size.y), 
				ccp(rect.origin.x, rect.origin.y + rect.size.y)
			};
			glColor4f(0, 0, 0, 1.0f);
			glLineWidth(1.0f);
			ccDrawPoly(vertices, 4, true, false);
		}
	}
}

EntityPath::~EntityPath()
{
	resetPathPoints();
}

void EntityPath::setPathVisible(bool visible)
{
	std::vector<Point>::iterator it = points.begin();
	for(; it != points.end(); ++it){
		Point& p = *it;
		if(p.sprite){
			p.sprite->setIsVisible(visible);
		}
	}
}

void EntityPath::createPathPoints(Layer * path_layer, bool path_visible, bool start_from_begin)
{
	resetPathPoints();

	CCTexture2D * texture = App::getTexture("path-seg.png");
	vec2 size = path_layer->getContentSize();

	int count = spline.GetNumValues();
	float end_time = spline.GetTime(count-1);
	// float dt = 1.0f / MathLib::Max((float)count * 4.0f, end_time / 0.1f);
	float start_time = start_from_begin ? 0.0f : end_time;
	float dt = (start_from_begin ? 1.0f : -1.0f) * 0.05f;
	float entity_scale = App::getEntityScale();
	for(float t = start_time; t >= 0.0f && t <= end_time; t += dt){
		CCSprite * sprite = CCSprite::spriteWithTexture(texture);
		sprite->setIsVisible(path_visible);
		sprite->setScale(entity_scale);
			
		vec2 pos = spline.GetCurrentValue(t) * size;
		sprite->setPosition(pos);

		vec2 speed = spline.GetCurrentFirstDerivative(t).Norm();
		float angle = MathLib::ArcTan(-speed.y, speed.x);
		sprite->setRotation(angle);
			
		path_layer->addChild(sprite);

		points.push_back(Point(t, sprite));
	}	
}

void EntityPath::resetPathPoints()
{
	std::vector<Point>::iterator it = points.begin();
	for(; it != points.end(); ++it){
		Point& p = *it;
		if(p.sprite){
			CCNode * parent = p.sprite->getParent();
			if(parent){
				parent->removeChild(p.sprite, true);
			}
			p.sprite->release();
			p.sprite = NULL;
		}
	}
	points.clear();
}

void MonsterHome::Entity::setPathVisible(bool visible)
{
	if(path){
		path->setPathVisible(visible);
	}
}

void MonsterHome::Entity::resetPathPoints()
{
	if(path){
		path->resetPathPoints();
	}
}

void MonsterHome::Entity::createPathPoints(bool path_visible)
{
	if(path){
		path->createPathPoints(game->path_layer, path_visible);
	}
}

void MonsterHome::Entity::updateRectCache()
{
	rect_cache.expaned = rect_cache.rect = getRect();
	rect_cache.expaned.Expand(rect_cache.expaned.size * 0.5f);
}

void MonsterHome::Entity::updateWarningSprite()
{
	if(warning_sprite){
		if(sprite){
			App::setZOrder(sprite, type == HOLE ? 0 : 2);
		}
		warning_sprite->setPosition(vec2(getContentSize()) * 0.5f);
		warning_sprite->setScale((vec2(getContentSize()) * (type == HOLE ? 2.5f : 1.4f) / warning_sprite->getContentSize()).Max());
		// warning_sprite->setIsVisible(false);
	}
}

void MonsterHome::Entity::startWarning()
{
	if(/*isAlive() &&*/ !warning_action){ // && !fight_with){
		if(!warning_sprite){
			const char * texture = "dist-warn.png";
			if(type == HOLE){
				texture = "hole-select.png";
			}
			warning_sprite = CCSprite::spriteWithTexture(App::getTexture(texture));
			// warning_sprite->retain();
			addChild(warning_sprite, 1);
		}
		updateWarningSprite();

		warning_sprite->setIsVisible(true);
		warning_sprite->setOpacity(0);

		struct Lib {
			static void finished(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				if(ent->warning_sprite != user_param){
					return;
				}
				ent->warning_sprite->setIsVisible(false);
				ent->warning_action = NULL;
				if(ent->fight_with){
					// ent->startWarning();
					ent->runAction(CCSequence::actionOneTwo(
						CCDelayTime::actionWithDuration(App::getRandFloat(0.1f, 0.5f)),
						ent->getFuncAction(Lib::checkFight)
					));
				}
			}
			static void checkFight(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				if(ent->fight_with){
					ent->startWarning();
				}
			}
		};
		warning_action = dynamic_cast<CCSequence*>(CCSequence::actions(
			CCFadeIn::actionWithDuration(0.2f),
			CCFadeOut::actionWithDuration(0.2f),
			getFuncAction(Lib::finished, warning_sprite),
			NULL
		));
		warning_sprite->runAction(warning_action);
	}
}

bool MonsterHome::Entity::isDead()
{
	return state == DEAD || state == DYING;
}

bool MonsterHome::Entity::isHomePos()
{
	return state == HOME_INSIDE_MOVE || state == HOME_MOVE;
}

/*
bool MonsterHome::Entity::isAlive()
{
	return state != DYING && state != DEAD;
}
*/

bool MonsterHome::Entity::isResponsible()
{
	// return state != DYING && state != DEAD && state != HOME_MOVE;
	return state == NORMAL;
}

bool MonsterHome::Entity::isInFight()
{
	return fight_with != NULL;
}

bool MonsterHome::Entity::checkDie()
{
	if(isResponsible() && info.health > 0 && damaged >= info.health){
		die();
	}
	return isDead();
}

void onAllActionsRemoved(CCObject * obj, CCAction * action)
{
	MonsterHome::Entity * ent = dynamic_cast<MonsterHome::Entity*>(obj);
	if(ent){
		ent->onAllActionsRemoved();
	}else{
		CCNode * node = dynamic_cast<CCNode*>(obj);
		if(node){
			ent = dynamic_cast<MonsterHome::Entity*>(node->getParent());
			if(ent){
				ent->onAllActionsRemoved();
			}
		}
	}
}

void MonsterHome::Entity::onAllActionsRemoved()
{
	int count = numberOfRunningActions();
	{
		CCObject* obj = NULL;
		CCARRAY_FOREACH(getChildren(), obj){
			CCNode * node = dynamic_cast<CCNode*>(obj);
			count += node->numberOfRunningActions();
			if(count > 0){
				int i = 0;	
			}
		}
	}

	if(state == DEAD && count == 0){
		int i = 0;
	}
	if(state != BEFORE_FREE){
		int i = 0;
	}
}

void MonsterHome::Entity::die()
{
	if(isResponsible()){
		state = DYING;
		stopCurrentAction();

		struct Lib {
			static void finished(CCObject * target, void * user_param)
			{
#if 1
				Entity * ent = dynamic_cast<Entity*>((Entity*)user_param);
				if(ent->state != DEAD){
					return;
				}
				ent->sprite->runAction(CCSequence::actions(
					CCDelayTime::actionWithDuration(0.5f),
					CCFadeOut::actionWithDuration(0.5f), // relevant for sprite
					ent->getFuncAction(Lib::removeEntity),
					NULL
				));
				// ent->scheduleOnce(Lib::removeEntity, 0.5f);
#else
				removeEntity(target, user_param);
#endif
			}
			static void removeEntity(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				if(ent->state != DEAD){
					return;
				}
				ent->state = BEFORE_FREE;
				ent->game->onMonsterDead(ent);
				ent->getParent()->removeChild(ent, true);				
			}
		};
		EntityInfo::Animation * animation = getAnimation("die_right");
		if(animation){
			runAnimation(animation, false, 1, false, Lib::finished, this);
		}else{
			Lib::finished(this, this);
		}
		state = DEAD;

		if(App::settings()->isSoundEnabled()){
			App::playEntitySound(info.name, "death");
		}
	}
}

void MonsterHome::Entity::fight(Entity * ent)
{
	IW_CALLSTACK("MonsterHome::Entity::fight");
	if(ignore_fight || ent->ignore_fight){
		return;
	}
	if(fight_with && ent->fight_with){
		return;
	}
	if(game->selected_monster == this || game->selected_monster == ent){
		return;
	}
	if(!isResponsible() || !ent->isResponsible()){
		return;
	}
	float side_scale = 0.45f;
	if(!fight_with){
		if(!ent->fight_with){
			// NOT fight_with
			// NOT ent->fight_with
			vec2 fight_center = (vec2(getPosition()) + ent->getPosition()) * 0.5f;
			float side = getPosition().x < ent->getPosition().x ? 1.0f : -1.0f;
			// setFlipX(side < 0);
			// ent->setFlipX(side > 0);
			vec2 fight_pos = fight_center + vec2(rect_cache.rect.size.x * side_scale * -side, 0);
			vec2 fight_pos2 = fight_center + vec2(
				ent->rect_cache.rect.size.x * side_scale * side, 
				(ent->getPositionY() - ent->rect_cache.rect.origin.y) - (getPositionY() - rect_cache.rect.origin.y) 
					+ MathLib::Min(rect_cache.rect.size.y, ent->rect_cache.rect.size.y) * App::getRandFloat(-0.2f, 0.2f));
			startFightWith(ent, fight_pos);
			ent->startFightWith(this, fight_pos2);
		}else{
			// NOT fight_with
			// ent->fight_with
			float side = getPosition().x < ent->getPosition().x ? 1.0f : -1.0f;
			vec2 fight_pos2 = ent->getPosition();
			vec2 fight_pos = fight_pos2 + vec2(
				(rect_cache.rect.size.x * side_scale + ent->rect_cache.rect.size.x * side_scale) * -side, 
				(getPositionY() - rect_cache.rect.origin.y) - (ent->getPositionY() - ent->rect_cache.rect.origin.y) 
					+ MathLib::Min(rect_cache.rect.size.y, ent->rect_cache.rect.size.y) * App::getRandFloat(-0.2f, 0.2f));
			startFightWith(ent, fight_pos);
		}		
	}else{
		// fight_with
		// NOT ent->fight_with
		float side = getPosition().x < ent->getPosition().x ? 1.0f : -1.0f;
		vec2 fight_pos = getPosition();
		vec2 fight_pos2 = fight_pos + vec2(
			(rect_cache.rect.size.x * side_scale + ent->rect_cache.rect.size.x * side_scale) * side, 
			(ent->getPositionY() - ent->rect_cache.rect.origin.y) - (getPositionY() - rect_cache.rect.origin.y) 
				+ MathLib::Min(rect_cache.rect.size.y, ent->rect_cache.rect.size.y) * App::getRandFloat(-0.2f, 0.2f));
		ent->startFightWith(this, fight_pos2);
	}
}

void MonsterHome::Entity::stopIgnoreFight()
{
	if(ignore_fight){
		stopAction(ignore_fight);
		ignore_fight = NULL;
	}
}

void MonsterHome::Entity::startIgnoreFight(float time)
{
	stopIgnoreFight();
	if(time > 0.0f){
		struct Lib {
			static void enableFight(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				ent->ignore_fight = NULL;
			}
		};
		ignore_fight = CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(time),
			getFuncAction(Lib::enableFight)
		);
		runAction(ignore_fight);
	}
}

void MonsterHome::Entity::stopFight(bool start_ignore_timeout)
{
	// ASSERT(isResponsible());
	IW_CALLSTACK("MonsterHome::Entity::stopFight");
	if(fight_with){
		Entity * enemy = fight_with;

		fight_with->release();
		fight_with = NULL;

		stopAnimation();
		stopPathMove();
		stopIgnoreFight();

		if(start_ignore_timeout){
			startIgnoreFight();
		}

		enemy->stopFight(false);
		enemy->unpauseMove();

		CCObject* obj = NULL;
		CCARRAY_FOREACH(getParent()->getChildren(), obj){
			Entity * ent = dynamic_cast<Entity*>(obj);
			if(ent && ent != this && ent->fight_with == this){
				ent->stopFight(false);
				ent->unpauseMove();
			}
		}
		checkDie();
	}
}

void MonsterHome::Entity::startFightWith(Entity * enemy, const vec2& pos)
{
	ASSERT(fight_with != enemy);
	ASSERT(type == MONSTER);
	ASSERT(enemy->type == MONSTER);
	ASSERT(isResponsible());
	ASSERT(enemy->isResponsible());
	
	IW_CALLSTACK("MonsterHome::Entity::startFightWith");
	/* if(!isResponsible()){
		return;
	} */
	if(enemy && path){
		EntityPath * new_path = new EntityPath();
		vec2 p = pos / getParent()->getContentSize();
		new_path->AddPoint(p);
		int count = path->spline.GetNumValues();
		for(int i = 0; i < count; i++){
			if(path->spline.GetTime(i) > path_time){
				for(; i < count; i++){
					new_path->AddPoint(path->spline.GetValue(i));
				}
				break;
			}			
		}
		if(new_path->spline.GetNumValues() < 2){
			new_path->AddPoint(p + vec2(App::getRandSignFloat(), App::getRandSignFloat()).Norm() * 0.01f);
		}
		setPath(new_path, path && path->points.size() > 0 && path->points[0].sprite->getIsVisible());
	}
	stopFight();
	CCAssert(!fight_with, "");
	CCAssert(enemy != this, "");
	fight_with = enemy;
	if(fight_with){
		fight_with->retain();
		startIgnoreFight();

		stopAnimation();
		stopPathMove();
		startWarning();

		struct Lib {
			static void nextAnimation(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				ASSERT(ent->fight_with);
				if(!ent->isResponsible() || ent->path_action){
					return;
				}
				if(ent->fight_with->info.health > 0){
					ent->fight_with->damaged += MathLib::Min(ent->fight_with->info.health*0.95f, ent->info.health / 3.0f);
					if(App::settings()->isSoundEnabled()){
						App::playEntitySound(ent->fight_with->info.name, "pain");
					}
				}
				// ent->stopIgnoreFight();
				ent->stopAnimation();
				ent->stopPathMove();
				// ent->path_action = NULL;
				ent->path_sequence_action = CCSequence::actionOneTwo(
					CCDelayTime::actionWithDuration(App::getRandFloat(0.1f, 0.5f)),
					ent->getFuncAction(Lib::startAnimation, NULL)
				);
				ent->runAction(ent->path_sequence_action);
				ent->startWarning();
			}
			static void startAnimation(CCObject * target, void * user_param)
			{
				Entity * ent = dynamic_cast<Entity*>(target);
				if(!ent->isResponsible() || ent->path_action != user_param){
					return;
				}
				ent->stopPathMove();
				// ent->path_action = NULL;
				// ent->path_sequence_action = NULL;
				if(!ent->checkDie() && !ent->fight_with->checkDie()){
					ent->startWarning();
					ent->runAnimation("fight_right", false, 1, false, Lib::nextAnimation);
					ent->setFlipX(ent->getPosition().x > ent->fight_with->getPosition().x);

					/* if(App::settings()->isSoundEnabled()){
						App::playEntitySound(ent->info.name, "pain");
					} */
				}
			}
		};
		EntityPath * move_path = new EntityPath();
		move_path->AddPoint(vec2(getPosition()) / getParent()->getContentSize());
		move_path->AddPoint(pos / getParent()->getContentSize());
		path_action = createPathMoveAction(move_path, 0.0f);
		path_sequence_action = CCSequence::actionOneTwo(
			// CCMoveTo::actionWithDuration(0.2f, pos),
			path_action,
			getFuncAction(Lib::startAnimation, path_action)
			// getFuncAction(Lib::nextAnimation)
		);
		runAction(path_sequence_action);
	}
}

MonsterHome::Entity * MonsterHome::Entity::create(MonsterHome * game, EType type, JSON * config)
{
	IW_CALLSTACK("MonsterHome::Entity::create");
	Entity * entity = new Entity();
	if(entity && entity->initWithName(game, type, config)){
		entity->autorelease();
		return entity;
	}
	delete entity;
	return NULL;
}

bool MonsterHome::Entity::initWithName(MonsterHome * game, EType type, JSON * config)
{
	IW_CALLSTACK("MonsterHome::Entity::initWithName");
	if(initWithConfig(config, App::getEntityScale())){
		this->game = game;
		this->type = type;
		this->info.speed *= game->level_info.monster_speed_scale;
		return true;
	}
	return false;
}

void MonsterHome::Entity::setPath(EntityPath * value, bool path_visible)
{
	IW_CALLSTACK("MonsterHome::Entity::setPath");
	/* if(!isResponsible()){
		int i = 0;
		// return;
	} */

	stopFight();
	// stopAnimation();
	stopPathMove();

	resetPathPoints();

	if(value) value->retain();
	if(path) path->release();
	path = value;
	path_time = 0.0f;

#if 0
	for(int i = 0; i < path->spline.GetNumValues(); i++){
		float t = path->spline.GetTime(i);
		if(isnanf(t)){
			int i = 0;
			int j = 0;
		}
	}
#endif

	createPathPoints(path_visible);
}

PathMoveAction * MonsterHome::Entity::createPathMoveAction(EntityPath * path, float start_time)
{
	IW_CALLSTACK("MonsterHome::Entity::createPathMoveAction");
	return PathMoveAction::actionWithPath(path, (info.speed /*+ 0.05f * App::getRandSignFloat()*/) * 3.0f, 0.0f, start_time);
}

void MonsterHome::Entity::startHomeMove(Entity * target)
{
	IW_CALLSTACK("MonsterHome::Entity::startHomeMove");
	/* if(!isResponsible()){
		return;
	} */
	stopCurrentAction(false);

	vec2 size = getParent()->getContentSize();
	home_pos = target->getPosition();

	EntityPath * path = new EntityPath();
	vec2 p = vec2(getPosition()) / size;
	vec2 target_pos = home_pos / size;
	vec2 dir = target_pos - p;
	float len = dir.LenNorm();
	float skip_len = target->getContentSize().width * 0.5f / size.x;
	target_pos = p + dir * (len - skip_len);

	path->AddPoint(p);
	path->AddPoint(target_pos);
	setPath(path);

	struct Lib {
		static void moveFinished(CCObject * target, void * user_param)
		{
			Entity * ent = dynamic_cast<Entity*>(target);
			if(ent->state != HOME_MOVE || ent->path_action != user_param){
				return;
			}
			ent->startHomeInside();
		}
	};

	path_action = createPathMoveAction(path, 0.0f);
	path_sequence_action = CCSequence::actionOneTwo(path_action, getFuncAction(Lib::moveFinished, path_action));
	runAction(path_sequence_action);

	// startPathMove();

	state = HOME_MOVE;
}

void MonsterHome::Entity::startHomeInside()
{
	ASSERT(state == HOME_MOVE);
	/* if(!isResponsible()){
		return;
	} */
	stopCurrentAction(false);

	vec2 size = getParent()->getContentSize();

	EntityPath * path = new EntityPath();
	path->AddPoint(vec2(getPosition()) / size);
	path->AddPoint(home_pos / size);
	setPath(path);

	struct Lib {
		static void moveFinished(CCObject * target, void * user_param)
		{
			Entity * ent = dynamic_cast<Entity*>(target);
			if(ent->state != HOME_INSIDE_MOVE || ent->path_action != user_param){
				return;
			}
			ent->game->onMonsterAtHome(ent);
			ent->getParent()->removeChild(ent, true);
		}
	};

	path_action = createPathMoveAction(path, 0.0f);

	float duration = MathLib::Max(path_action->getDuration(), 0.5f);
	path_sequence_action = CCSequence::actionOneTwo(
		CCSpawn::actions(path_action, 
			CCFadeOut::actionWithDuration(duration * 1.1f),
			CCSequence::actionOneTwo(
				EasyAction::actionWithAction(CCScaleTo::actionWithDuration(duration * 0.4f, 1.2f), EasyAction::OUT_QUINT),
				EasyAction::actionWithAction(CCScaleTo::actionWithDuration(duration * 0.6f, 0.5f), EasyAction::IN_QUINT)
			),
			NULL
		), getFuncAction(Lib::moveFinished, path_action));
	runAction(path_sequence_action);

	state = HOME_INSIDE_MOVE;
}

void MonsterHome::Entity::startPathMove(float delay)
{
	IW_CALLSTACK("MonsterHome::Entity::startPathMove");
	/* if(!isResponsible()){
		return;
	} */
	stopCurrentAction();

	float start_time = 0.0f;
	path_action = createPathMoveAction(path, start_time);

	struct Lib {
		static void moveFinished(CCObject * target, void * user_param)
		{
			IW_CALLSTACK("MonsterHome::Entity::startPathMove::moveFinished");
			Entity * ent = dynamic_cast<Entity*>(target);
			if(!ent->isResponsible() || ent->path_action != user_param){
				return;
			}
			/* ent->stopIgnoreFight();
			if(ent->state == HOME_MOVE){
				ent->startHomeInside();
				return;
			} */
			if(ent->path_action){
				vec2 map_size = ent->getParent()->getContentSize();
				Rect2 rect(vec2(0, 0), map_size);
				vec2 start_pos = ent->getPosition();
				if(rect.ContainsPoint(start_pos)){
					vec2 ent_size = ent->getContentSize();
					rect.origin = rect.origin - ent_size;
					rect.size = rect.size + ent_size * 2.0f;
					EntityPath * path = new EntityPath();
					path->AddPoint(start_pos / map_size);
					vec2 end_pos = start_pos;
					vec2 offs = ent->path_action->last_move_dir;
					if(offs.Len() < 0.1f){
						offs = vec2(App::getRandSignFloat(), App::getRandSignFloat()).Norm();
					}
					offs = offs * (ent_size.Len() * 0.5f);
					for(int i = 0; i < 20; i++){
						end_pos += offs;
						if(!rect.ContainsPoint(end_pos)){
							// end_pos += offs;
							break;
						}
					}
					path->AddPoint(end_pos / map_size);
					ent->setPath(path);
					ent->startPathMove();
					return;
				}
			}
			MonsterHome * game = ent->game;
			ent->game->onMonsterMissed(ent);
			ent->getParent()->removeChild(ent, true);
			game->checkMonsters();
		}
	};
	path_sequence_action = CCSequence::actionOneTwo(path_action, getFuncAction(Lib::moveFinished, path_action));
	if(delay > 0.0f){
		path_sequence_action = CCSequence::actionOneTwo(CCDelayTime::actionWithDuration(delay), path_sequence_action);
	}
	runAction(path_sequence_action);
}

void MonsterHome::Entity::stopPathMove()
{
	IW_CALLSTACK("MonsterHome::Entity::stopPathMove");
	if(path_sequence_action){
		stopAction(path_sequence_action);
		path_sequence_action = NULL;
		path_action = NULL;
	}
}

void MonsterHome::Entity::stopCurrentAction(bool stop_animation, bool start_ignore_timeout)
{
	stopFight(start_ignore_timeout);
	stopPathMove();

	if(stop_animation){
		stopAnimation();
	}
}

void MonsterHome::Entity::pauseMove()
{
	IW_CALLSTACK("MonsterHome::Entity::pauseMove");
	// ASSERT(!fight_with);
	
	stopCurrentAction(false);
}

void MonsterHome::Entity::unpauseMove()
{
	IW_CALLSTACK("MonsterHome::Entity::unpauseMove");
	if(!isResponsible() || fight_with){
		return;
	}
	// CCAssert(!fight_with, "");
	// stopFight();
	if(!path){
		EntityPath * path = new EntityPath();
		vec2 p = vec2(getPosition()) / getParent()->getContentSize();
		path->AddPoint(p);
		path->AddPoint(p + vec2(App::getRandSignFloat(), App::getRandSignFloat()).Norm() * 0.01f);
		setPath(path); // , path && path->points.size() > 0 && path->points[0].sprite->getIsVisible());
	}else if(path_time > 0.0f){
		EntityPath * new_path = new EntityPath();
		vec2 p = vec2(getPosition()) / getParent()->getContentSize();
		new_path->AddPoint(p);
		int count = path->spline.GetNumValues();
		for(int i = 0; i < count; i++){
			if(path->spline.GetTime(i) > path_time){
				for(; i < count; i++){
					new_path->AddPoint(path->spline.GetValue(i));
				}
				break;
			}			
		}
		// EntityPath * new_path = new EntityPath(path, path_time);
		setPath(new_path, path && path->points.size() > 0 && path->points[0].sprite->getIsVisible());
	}
	startPathMove();
}

float MonsterHome::Entity::getDistToLocalPoint(const vec2& local)
{
	Rect2 rect = Rect2(vec2(0, 0), getContentSize());
	if(rect.ContainsPoint(local)){
		return 0.0f;
	}
	float dist_x = local.x < rect.origin.x ? (rect.origin.x - local.x) : (local.x >= rect.origin.x + rect.size.x ? local.x - rect.origin.x - rect.size.x : 0.0f);
	float dist_y = local.y < rect.origin.y ? (rect.origin.y - local.y) : (local.y >= rect.origin.y + rect.size.y ? local.y - rect.origin.y - rect.size.y : 0.0f);
	return dist_x != 0.0f && dist_y != 0.0f ? MathLib::Min(dist_x, dist_y) : MathLib::Max(dist_x, dist_y);
}

bool MonsterHome::Entity::checkTouchAtLocalPoint(const vec2& local)
{
	return true;
}

bool MonsterHome::Entity::checkTouchAtLocalRect(const Rect2& local_rect)
{
	return true;
}

void MonsterHome::Entity::setPosition(const CCPoint& value)
{
	IW_CALLSTACK("MonsterHome::Entity::setPosition");
	ASSERT(!isDead());

	super::setPosition(value);
	updateRectCache();
	updateOrder();

	if(path_action){
		path_time = path_action->last_t;
		updateMoveDir(path_action->last_move_dir);

		if(path){
			std::vector<EntityPath::Point>::iterator it = path->points.begin();
			for(; it != path->points.end(); ++it){
				EntityPath::Point& p = *it;
				if(p.time < path_time && p.sprite->getIsVisible()){
					p.sprite->setIsVisible(false);
				}
			}
		}
	}

	if(decal){
		decal->setPosition(value);
	}
}

void MonsterHome::Entity::onEnter()
{
	IW_CALLSTACK("MonsterHome::Entity::onEnter");
	super::onEnter();
	/* struct Lib {
		static void scheduledUpdateOrder(CCObject * target, void*)
		{
			Entity * self = dynamic_cast<Entity*>(target);
			CCAssert(self, "");
			self->updateOrder();
		}
	};
	scheduleOnce(Lib::scheduledUpdateOrder); */
}

void MonsterHome::Entity::onExit()
{
	IW_CALLSTACK("MonsterHome::Entity::onExit");
	state = FREE;
	stopCurrentAction();
	if(owner){
		CCAssert(owner->decal == this, "");
		owner->decal->release();
		owner->decal = NULL;
		owner->release();
		owner = NULL;
	}
	setDecal(NULL);
	game->onEntityRemoved(this);
	super::onExit();
}

void MonsterHome::Entity::setDecal(Entity * ent)
{
	IW_CALLSTACK("MonsterHome::Entity::setDecal");
	if(decal == ent){
		return;
	}
	if(decal){
		CCAssert(decal->owner == this, "");
		decal->owner->release();
		decal->owner = NULL;
		CCNode * decal_parent = decal->getParent();
		if(decal_parent){
			decal_parent->removeChild(decal, true);
		}
		decal->release();
		decal = NULL;
	}
	if(ent){
		decal = ent;
		decal->retain();
		decal->owner = this;
		decal->owner->retain();
	}
}

void MonsterHome::Entity::updateOrder()
{
	IW_CALLSTACK("MonsterHome::Entity::updateOrder");
	// float parent_height = parent->getContentSize().height;
	// Rect2 rect = getRect();
	int z = 100000 - (int)(rect_cache.rect.origin.y * 100);  // getPositionY(); // - getAnchorPoint().y * getContentSize().height;
	App::setZOrder(this, z);
}

void MonsterHome::Entity::updateMoveDir(const vec2& dir)
{
	IW_CALLSTACK("MonsterHome::Entity::updateMoveDir");
	EntityInfo::Animation * animation = NULL;
	if(dir.y > 0.85f && (animation = info.animations.objectForKey("walk_up"))){
		runAnimation(animation);
		setFlipX(false);
	}else if(dir.y < -0.85f && (animation = info.animations.objectForKey("walk_down"))){
		runAnimation(animation);
		setFlipX(false);
	}else{
		if(dir.y > 0.4f){
			animation = info.animations.objectForKey("walk_up_right");
		}else if(dir.y < -0.4f){
			animation = info.animations.objectForKey("walk_down_right");
		}
		if(animation){
			runAnimation(animation);
			setFlipX(dir.x < 0);
		}else{			
			if(!cur_animation || strcmp(cur_animation->getName(), "walk_right") != 0){
				runAnimation("walk_right");
				setFlipX(dir.x < 0);
			}else{
				runAnimation("walk_right");
			}
		}
	}
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

// static const char fontname[] = "DejaVuSans.ttf";

MonsterHome::BaseItemsWindow::BaseItemsWindow()
{
	// game = NULL;
	open_duration = 0.5f;
	close_duration = 0.3f;
	open_action = NULL;
	close_action = NULL;
	pos_initialized = false;
}

CCAction * MonsterHome::BaseItemsWindow::createOpenAction()
{
	struct Lib {
		static void finished(CCObject * target, void *)
		{
			BaseItemsWindow * self = dynamic_cast<BaseItemsWindow*>(target);
			self->open_action = NULL;
		}
	};

	float duration = open_duration;
	return CCSequence::actionOneTwo(
		CCSpawn::actionOneTwo(
			EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration, window_pos), EasyLib::OUT_BACK),
			CCFadeIn::actionWithDuration(duration)),
		getFuncAction(Lib::finished));
}

CCAction * MonsterHome::BaseItemsWindow::createCloseAction()
{
	struct Lib {
		static void remove(CCObject * target, void *)
		{
			BaseItemsWindow * self = dynamic_cast<BaseItemsWindow*>(target);
			self->close_action = NULL;
			self->getParent()->removeChild(self, true);
		}
	};
	float duration = close_duration;
	vec2 pos = window_pos + vec2(0, getContentSize().height * 0.2f);
	return CCSequence::actionOneTwo(
		CCSpawn::actionOneTwo(
			EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration, pos), EasyLib::IN_BACK),
			CCFadeOut::actionWithDuration(duration)
		),
		getFuncAction(Lib::remove)
	);
}

void MonsterHome::BaseItemsWindow::open()
{
	if(!open_action){
		ASSERT(pos_initialized);
		if(close_action){
			stopAction(close_action);
			close_action = NULL;
		}
	
		setPosition(window_pos + vec2(0, getContentSize().height * 0.3f));
		setOpacity(0);

		runAction(open_action = createOpenAction());
	}
}

void MonsterHome::BaseItemsWindow::close()
{
	if(!close_action){
		ASSERT(pos_initialized);
		if(open_action){
			stopAction(open_action);
			open_action = NULL;
		}
		runAction(close_action = createCloseAction());
	}
}

void MonsterHome::BaseItemsWindow::savePosition()
{
	window_pos = getPosition();
}

void MonsterHome::BaseItemsWindow::onEnter()
{
	super::onEnter();

	if(!pos_initialized){
		pos_initialized = true;

		if(App::isAdmobVisible()){
			float admob_height = (float)App::getAdmobDesc()->height;
			// vec2 parent_size = getParent()->getContentSize();
			vec2 size = getContentSize();
			vec2 pos = getPosition();
			vec2 anchor = getAnchorPoint();
			float low_y = pos.y - size.y * anchor.y;
			if(low_y < admob_height){
				setPosition(pos + vec2(0, admob_height - low_y));
			}
		}

		savePosition();
	}
	open();
}

GLubyte MonsterHome::BaseItemsWindow::getOpacity()
{
	if(background_layer && background_layer->getChildrenCount() > 0){
		CCSprite * sprite = dynamic_cast<CCSprite*>(background_layer->getChildren()->objectAtIndex(0));
		if(sprite){
			return sprite->getOpacity();
		}
	}
	return 255;
}

void MonsterHome::BaseItemsWindow::setOpacity(GLubyte opacity)
{
	setOpacityAll(this, opacity, false);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::MenuWindow::MenuWindow()
{
	game = NULL;
	mode_button_items[0] = mode_button_items[1] = mode_button_items[2] = NULL;
	ribon = NULL;
}

MonsterHome::MenuWindow * MonsterHome::MenuWindow::create(MonsterHome * game)
{
	MenuWindow * window = new MenuWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::MenuWindow::levelButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	CCNode * node = dynamic_cast<CCNode*>(obj);
	int level = node->getZOrder();
	if(App::settings()->isLevelEnabled(level)){
		game->gotoLevelScene(level);		
	}
}

void MonsterHome::MenuWindow::playButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	for(int i = Settings::LEVELS_NUMBER-1; i >= 0; i--){
		if(App::settings()->isLevelEnabled(i)){
			game->gotoLevelScene(i);
			break;
		}
	}
}

void MonsterHome::MenuWindow::buyButtonPressed(CCObject*)
{
	App::buyGame();
}

void MonsterHome::MenuWindow::exitButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->addChild(MenuExitWindow::create(this), WINDOW);
}

void MonsterHome::MenuWindow::showModeWindow(int mode)
{
	game->addChild(MenuModeWindow::create(this, mode), WINDOW);
}

void MonsterHome::MenuWindow::mode1ButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	showModeWindow(0);
}

void MonsterHome::MenuWindow::mode2ButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	showModeWindow(1);
}

void MonsterHome::MenuWindow::mode3ButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	showModeWindow(2);
}

void MonsterHome::MenuWindow::settingsButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->addChild(MenuSettingsWindow::create(this), WINDOW);
}

void MonsterHome::MenuWindow::savePosition()
{
	super::savePosition();
	ribon_pos = ribon->getPosition();
}

void MonsterHome::MenuWindow::open()
{
	super::open();

	if(ribon){
		float duration = open_duration;
		vec2 pos = ribon_pos;
		ribon->setPosition(pos + vec2(0, ribon->getContentSize().height * ribon->getScaleY() * 1.5f));
		ribon->runAction(
			CCSequence::actionOneTwo(
				CCDelayTime::actionWithDuration(duration * 0.0f),
				EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration * 1.3f, pos), EasyLib::OUT_BACK)
			));
	}
}

void MonsterHome::MenuWindow::onEnter()
{
	game->state = MENU;
	super::onEnter();
}

void MonsterHome::MenuWindow::onExit()
{
	super::onExit();
}

bool MonsterHome::MenuWindow::initWithGame(MonsterHome * game)
{
	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("GAME_NAME"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont()); // fontname, title_height);

	// float button_height = game_size.y * 0.05f;

	float entity_scale = App::getEntityScale();
	CCMenu * level_menu = CCMenu::node();
	CCTexture2D * level_button_texture = App::getTexture("level-button.png");
	CCTexture2D * level_button_pressed_texture = App::getTexture("level-button-pressed.png");
	CCTexture2D * level_button_disabled_texture = App::getTexture("level-button-disabled.png");
	for(int level = 0; level < Settings::LEVELS_NUMBER; level++){
		Settings::LevelAward level_award = App::settings()->getLevelAward(level);
		bool is_enabled = App::settings()->isLevelEnabled(level);
		// float scale = 1.0f;
		String label = level+1;
		// float button_height = level_button_texture->getContentSize().height * 0.5f;
		SmartButton * normal, * pressed = NULL, * disabled = NULL;
		normal = createLabel(label, level_button_texture, vec3(1,1,1), vec3(0,0,0), App::getFont(), entity_scale); // fontname, button_height);
		// normal = createLabelBM(label, level_button_texture, vec3(1,1,1), App::getFont());
		// normal->setScale(scale);
		if(is_enabled){
			pressed = createLabel(label, level_button_pressed_texture, vec3(1,1,0), vec3(0,0,0), App::getFont(), entity_scale); // fontname, button_height * 0.95f);
			// dynamic_cast<CCNode*>(pressed->getChildren()->objectAtIndex(0))->setScale(0.9f);
			pressed->setLabelScale(0.9f);
			// pressed = createLabelBM(label, level_button_pressed_texture, vec3(1,1,0), App::getFont());
			// pressed->setScale(scale);
		}else{
			disabled = createLabel(label, level_button_disabled_texture, vec3(0.5f,0.5f,0.5f), vec3(0,0,0), App::getFont(), entity_scale); // fontname, button_height * 0.8f);
			// disabled = createLabelBM(label, level_button_disabled_texture, vec3(0.5f,0.5f,0.5f), App::getFont());
			// disabled->setScale(scale);
		}
		CCMenuItem * level_button_item = CCMenuItemSprite::itemFromNormalSprite(
			normal, pressed, disabled, this, menu_selector(MenuWindow::levelButtonPressed));
		level_button_item->setIsEnabled(is_enabled);
		// level_button_item->setScale(scale);

		String time_str = level_award.sec > 0 ? App::timeToStr(level_award.sec) : "--:--";

#if 1
		for(int i = 0; i < 3; i++){
			Layer * node = i == 0 ? normal : (i == 1 ? pressed : disabled);
			if(!node){
				continue;
			}
			SmartLabel * time_label = SmartLabel::labelWithString(time_str, 
				i == 0 ? vec3(1,1,1) : (i == 1 ? vec3(1,1,0) : vec3(0.5f,0.5f,0.5f)), 
				vec3(0,0,0), // vec2(1,-1)*button_height*0.05f, fontname, i == 0 ? button_height : (i == 1 ? button_height * 0.95f : button_height * 0.8f));
				App::getFont());
			time_label->setAnchorPoint(vec2(0, 0.5f));
			time_label->setPosition(vec2(node->getContentSize().width + time_label->getContentSize().height*0.3f, node->getContentSize().height*0.5f));
			if(i == 1){
				time_label->setScale(0.9f);
			}
			node->addChild(time_label);
			node->setContentSize(vec2(time_label->getPositionX() + time_label->getContentSize().width, node->getContentSize().height));
		}
		level_button_item->setContentSize(vec2(normal->getContentSize())); // * vec2(normal->getScaleX(), normal->getScaleY()));
#endif

		/* CCMenuItem * level_button_item = createLabelButton(String(level), 
			level_button_texture, level_button_pressed_texture,
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), fontname, button_height, button_height * 0.97f,
			this, menu_selector(MenuWindow::levelButtonPressed)); */
		level_menu->addChild(level_button_item, level);
	}
	// level_menu->alignItemsInColumns(3, 3, 3, 0);
	Layer::tableLayout(level_menu, Settings::LEVEL_MENU_COLS, 0, game_size.y * 0.025f,
		Layer::ALIGN_LEFT | Layer::ALIGN_TOP);

	CCMenuItem * play_button_item = NULL;
	if(App::isAdmobVersion()){
		play_button_item = createLabelButton(App::getLangStr("BUY"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), entity_scale,
			this, menu_selector(MenuWindow::buyButtonPressed));
	}else{
		play_button_item = createLabelButton(App::getLangStr("PLAY"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), entity_scale,
			this, menu_selector(MenuWindow::playButtonPressed));
	}

	CCMenuItem * settings_button_item = createLabelButton(App::getLangStr("SETTINGS"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), entity_scale,
		this, menu_selector(MenuWindow::settingsButtonPressed));

	for(int mode = 0; mode < Settings::MODES_NUMBER; mode++){
		bool is_selected = App::settings()->getMode() == mode;
		bool is_enabled = App::settings()->isModeEnabled(mode);
		mode_button_items[mode] = CCMenuItemSprite::itemFromNormalSprite(
			CCSprite::spriteWithFile(String::Format(is_selected ? "mode-%d-selected.png" : "mode-%d.png", mode+1)), // (is_enabled ? "mode-%d.png" : "mode-%d-disabled.png"), mode+1))),
			CCSprite::spriteWithFile(String::Format("mode-%d-pressed.png", mode+1)), // is_enabled ? "mode-%d-pressed.png" : "mode-disabled-pressed.png", mode+1))),
			this, 
			mode == 0 ? menu_selector(MenuWindow::mode1ButtonPressed) : (mode == 1 ? menu_selector(MenuWindow::mode2ButtonPressed) : menu_selector(MenuWindow::mode3ButtonPressed))
			);
		mode_button_items[mode]->setScale(entity_scale);
	}

	// CCMenu * buttons_menu = createButtonsMenu(ok_button_item, menu_button_item, NULL);
	CCMenu * buttons_menu = CCMenu::menuWithItems(settings_button_item, mode_button_items[0], mode_button_items[1], mode_button_items[2], play_button_item, NULL);
	buttons_menu->alignItemsHorizontallyWithPadding(game_size.y * 0.05f);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	// items->addObject(message);
	items->addObject(level_menu);
	items->addObject(buttons_menu);

	if(super::initWithItems(770 * entity_scale, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		CCSprite * exit_sprite = CCSprite::spriteWithFile("exit.png");
		CCSprite * exit_sprite_pressed = CCSprite::spriteWithFile("exit-pressed.png");
		exit_sprite->setScale(entity_scale);
		exit_sprite_pressed->setScale(entity_scale);
		CCMenuItemSprite * exit_item = CCMenuItemSprite::itemFromNormalSprite(
			exit_sprite, exit_sprite_pressed,
			this,
			menu_selector(MenuWindow::exitButtonPressed)
			);
		// exit_item->setAnchorPoint(vec2(0,0));

		CCMenu * exit_menu = CCMenu::menuWithItem(exit_item);
		exit_item->setAnchorPoint(vec2(0, 0));
		// exit_menu->alignItemsHorizontally();
		exit_menu->setIsRelativeAnchorPoint(true);
		exit_menu->setContentSize(vec2(exit_item->getContentSize()) * entity_scale);
		exit_menu->setAnchorPoint(vec2(0.5f, 0.5f));
		exit_menu->setPosition(vec2(getContentSize().width - exit_menu->getContentSize().width, title->getPositionY() + title->getContentSize().height/2));
		exit_menu->setPositionX(getContentSize().width - (getContentSize().height - exit_menu->getPositionY()) * 1.1f);
		// exit_menu->setPosition(vec2(getContentSize()) - exit_menu->getContentSize());
		// ribon->setAnchorPoint(vec2(0, 0.5));
		// ribon->setPosition(vec2(padding, getContentSize().height - padding));
		addChild(exit_menu);

#if defined GAME_FREE_VERSION && 0
		ribon = CCSprite::spriteWithFile("free.png");
#else
		ribon = CCSprite::spriteWithFile("full.png");
#endif
		ribon->setScale(App::getEntityScale());
		ribon->setAnchorPoint(vec2(0, 0.5f));
		ribon->setPosition(vec2(0, title->getPositionY() + title->getContentSize().height*0.4f));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::MenuModeWindow::MenuModeWindow()
{
	menu = NULL;
	ribon = NULL;
}

MonsterHome::MenuModeWindow::~MenuModeWindow()
{
	menu->release();
}

MonsterHome::MenuModeWindow * MonsterHome::MenuModeWindow::create(MenuWindow * menu, int mode)
{
	MenuModeWindow * window = new MenuModeWindow();
	if(window->initWithMenu(menu, mode)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::MenuModeWindow::close()
{
	getParent()->removeChild(this, true);
	// super::close();
}

void MonsterHome::MenuModeWindow::selectButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	if(App::settings()->isModeEnabled(mode) && App::settings()->getMode() != mode){
		App::settings()->setMode(mode);
		App::settings()->save();

		struct Lib {
			static void recreateMenu(CCObject * target, void*)
			{
				MenuModeWindow * self = dynamic_cast<MenuModeWindow*>(target);
				// self->close();
				self->getParent()->removeChild(self, true);
				// self->menu->close();
				self->menu->getParent()->removeChild(self->menu, true);
				self->menu->getGame()->addChild(MenuWindow::create(self->menu->getGame()), WINDOW);
			}
		};
		scheduleOnce(Lib::recreateMenu);
	}else{
		close();
	}
}

void MonsterHome::MenuModeWindow::cancelButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	close();
}

void MonsterHome::MenuModeWindow::buyButtonPressed(CCObject * obj)
{
	App::buyGame();
	close();
}

void MonsterHome::MenuModeWindow::onEnter()
{
	// game->state = MENU_MODE;
	menu->setIsVisible(false);
	super::onEnter();
}

void MonsterHome::MenuModeWindow::onExit()
{
	super::onExit();

	if(menu->getIsRunning()){
		menu->setIsVisible(true);
		menu->open();
	}
}

void MonsterHome::MenuModeWindow::open()
{
	super::open();

	/*
	float duration = open_duration;
	ribon->setPosition(ribon_pos);
	ribon->setOpacity(0);
	ribon->runAction(
		CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(duration * 0.9f),
			CCFadeIn::actionWithDuration(duration * 2)));
	*/
}

bool MonsterHome::MenuModeWindow::initWithMenu(MenuWindow * menu, int mode)
{
	MonsterHome * game = menu->getGame();
	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;
	float width = 560 * App::getEntityScale();

	bool is_selected = App::settings()->getMode() == mode;
	bool is_enabled = App::settings()->isModeEnabled(mode);
	bool is_mode_for_buy = App::isAdmobVersion() && mode == Settings::MODES_NUMBER-1;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr(String::Format("MENU_MODE_%d", mode+1)), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	SmartLabel * message = SmartLabel::labelWithString(App::getLangStr(String::Format("MENU_MODE_%d_INFO", mode+1)), vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);
	SmartLabel * message2 = is_enabled || is_mode_for_buy ? NULL : SmartLabel::labelWithString(App::getLangStr(String::Format("MENU_MODE_%d_LOCKED_INFO", mode+1)), vec3(1, 0, 0), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);

	CCMenuItem * select_button_item = NULL;
	if(is_mode_for_buy){
		select_button_item = createLabelButton(App::getLangStr("BUY"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
			this, menu_selector(MenuModeWindow::buyButtonPressed));
	}else if(App::settings()->getMode() != mode && App::settings()->isModeEnabled(mode)){
		select_button_item = createLabelButton(App::getLangStr("SELECT"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
			this, menu_selector(MenuModeWindow::selectButtonPressed));
	}

	CCMenuItem * cancel_button_item = createLabelButton(App::getLangStr(App::settings()->getMode() == mode ? "OK" : "CANCEL"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(MenuModeWindow::cancelButtonPressed));

	CCMenu * buttons_menu = select_button_item ? CCMenu::menuWithItems(cancel_button_item, select_button_item, NULL) : CCMenu::menuWithItem(cancel_button_item);
	buttons_menu->alignItemsHorizontallyWithPadding(game_size.y * 0.05f);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	if(message2) items->addObject(message2);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->mode = mode;
		this->menu = menu;
		menu->retain();

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile(String::Format(is_selected ? "mode-%d-selected.png" : (is_enabled ? "mode-%d.png" : "mode-locked.png"), mode+1));
		ribon->setScale(App::getEntityScale());
		ribon->setAnchorPoint(vec2(0, 0.5f));
		ribon->setPosition(vec2(padding*2, title->getPositionY() + title->getContentSize().height/2));
		// ribon->setAnchorPoint(vec2(0, 0.5));
		// ribon->setPosition(vec2(padding, getContentSize().height - padding));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::MenuSettingsWindow::MenuSettingsWindow()
{
	menu = NULL;
	ribon = NULL;
	sound_enabled = true;
	music_enabled = true;
	reset_records = false;
}

MonsterHome::MenuSettingsWindow::~MenuSettingsWindow()
{
	menu->release();
}

MonsterHome::MenuSettingsWindow * MonsterHome::MenuSettingsWindow::create(MenuWindow * menu)
{
	MenuSettingsWindow * window = new MenuSettingsWindow();
	if(window->initWithMenu(menu)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::MenuSettingsWindow::close()
{
	getParent()->removeChild(this, true);
	// super::close();
}

void MonsterHome::MenuSettingsWindow::normalButtonPressed(CCObject*)
{
}

void MonsterHome::MenuSettingsWindow::checkedButtonPressed(CCObject*)
{
}

void MonsterHome::MenuSettingsWindow::toggleButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	CCMenuItemToggle * item = dynamic_cast<CCMenuItemToggle*>(obj);
	int i = item->getZOrder();
	switch(i){
	case 0: sound_enabled = !sound_enabled; break;
	case 1: music_enabled = !music_enabled; break;
	case 2: reset_records = !reset_records; break;
	case 3:
		lang_name = lang_names[item->getSelectedIndex()];
		break;
	}
}

void MonsterHome::MenuSettingsWindow::saveButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	bool recreate_menu = false;

	if(App::settings()->isSoundEnabled() != sound_enabled){
		App::settings()->setSoundEnabled(sound_enabled);
		if(sound_enabled){
			// App::playMusic(App::getMusicFilename());
			menu->getGame()->startAmbientSounds();
		}else{
			menu->getGame()->stopAmbientSounds();
			App::stopAllSounds();
		}
	}

	if(App::settings()->isMusicEnabled() != music_enabled){
		App::settings()->setMusicEnabled(music_enabled);
		if(music_enabled){
			App::playMusic(App::getMusicFilename());
		}else{
			App::stopMusic();
		}
	}

	if(App::settings()->getLangName() != lang_name){
		App::settings()->setLangName(lang_name);
		recreate_menu = true;
	}

	if(reset_records){
		App::settings()->resetLevelAwards();
		recreate_menu = true;
	}

	App::settings()->save();

	if(recreate_menu){
		struct Lib {
			static void recreateMenu(CCObject * target, void*)
			{
				MenuSettingsWindow * self = dynamic_cast<MenuSettingsWindow*>(target);
				// self->close();
				self->getParent()->removeChild(self, true);
				// self->menu->close();
				self->menu->getParent()->removeChild(self->menu, true);
				self->menu->getGame()->addChild(MenuWindow::create(self->menu->getGame()), WINDOW);
			}
		};
		scheduleOnce(Lib::recreateMenu);
	}else{
		close();
	}
}

void MonsterHome::MenuSettingsWindow::cancelButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	close();
}

void MonsterHome::MenuSettingsWindow::onEnter()
{
	// game->state = MENU_MODE;
	menu->setIsVisible(false);
	super::onEnter();
}

void MonsterHome::MenuSettingsWindow::onExit()
{
	super::onExit();

	if(menu->getIsRunning()){
		menu->setIsVisible(true);
		menu->open();
	}
}

void MonsterHome::MenuSettingsWindow::open()
{
	super::open();

	/*
	float duration = open_duration;
	ribon->setPosition(ribon_pos);
	ribon->setOpacity(0);
	ribon->runAction(
		CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(duration * 0.9f),
			CCFadeIn::actionWithDuration(duration * 2)));
	*/
}

bool MonsterHome::MenuSettingsWindow::initWithMenu(MenuWindow * menu)
{
	MonsterHome * game = menu->getGame();
	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;
	float width = 560 * App::getEntityScale();
	float entity_scale = App::getEntityScale();

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("SETTINGS"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	// SmartLabel * message = SmartLabel::labelWithString(App::getLangStr(String::Format("MENU_MODE_%d_INFO", mode+1)), vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*2, CCTextAlignmentCenter);

	sound_enabled = App::settings()->isSoundEnabled();
	music_enabled = App::settings()->isMusicEnabled();

	class LayerRGBA: public Layer, public RGBAProtocolDummy
	{
	public:

		LAYER_NODE_FUNC(LayerRGBA);

		virtual void setOpacity(GLubyte opacity)
		{
			setOpacityAll(this, opacity, false);
		}
	};

	CCMenuItem * menu_items[] = {NULL, NULL, NULL};
	float item_padding = padding;
	if(App::isAdmobVersion()){
		if(App::director()->getWinSize().height <= 360){
			item_padding *= 0.25f;
		}else{
			item_padding *= 0.5f;
		}
	}
	int i;
	for(i = 0; i < 3; i++){
		CCSprite * checkbox_empty = CCSprite::spriteWithTexture(App::getTexture("checkbox-empty.png"));
		CCSprite * checkbox_checked = CCSprite::spriteWithTexture(App::getTexture(i == 2 ? "checkbox-red.png" : "checkbox-checked.png"));

		CCSprite * checkbox_empty2 = CCSprite::spriteWithTexture(App::getTexture("checkbox-empty.png"));
		CCSprite * checkbox_checked2 = CCSprite::spriteWithTexture(App::getTexture(i == 2 ? "checkbox-red.png" : "checkbox-checked.png"));

		checkbox_empty->setScale(entity_scale);
		checkbox_checked->setScale(entity_scale);
		checkbox_empty2->setScale(entity_scale);
		checkbox_checked2->setScale(entity_scale);
	
		String label = App::getLangStr(i == 0 ? "SOUND" : (i == 1 ? "MUSIC" : "RESET_RECORDS"));
		float height;
		
		SmartLabel * label_normal = SmartLabel::labelWithString(label, vec3(1,1,1), vec3(0,0,0), App::getFont());
		height = MathLib::Max(checkbox_empty->getContentSize().height*entity_scale, label_normal->getContentSize().height) + item_padding;
		label_normal->setPosition( vec2(checkbox_empty->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
		label_normal->setAnchorPoint(vec2(0, 0.5f));

		checkbox_empty->setAnchorPoint(vec2(0, 0.5f));
		checkbox_empty2->setAnchorPoint(vec2(0, 0.5f));
		checkbox_checked->setAnchorPoint(vec2(0, 0.5f));
		checkbox_checked2->setAnchorPoint(vec2(0, 0.5f));

		checkbox_empty->setPosition(vec2(0, height * 0.5f));
		checkbox_empty2->setPosition(vec2(0, height * 0.5f));
		checkbox_checked->setPosition(vec2(0, height * 0.5f));
		checkbox_checked2->setPosition(vec2(0, height * 0.5f));

		SmartLabel * label_normal_pressed = SmartLabel::labelWithString(label, vec3(1,1,0), vec3(0,0,0), App::getFont());
		label_normal_pressed->setPosition( vec2(checkbox_empty->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
		label_normal_pressed->setAnchorPoint(vec2(0, 0.5f));
		label_normal_pressed->setScale(0.9f);

		SmartLabel * label_checked = SmartLabel::labelWithString(label, vec3(1,1,1), vec3(0,0,0), App::getFont());
		label_checked->setPosition( vec2(checkbox_checked->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
		label_checked->setAnchorPoint(vec2(0, 0.5f));

		SmartLabel * label_checked_pressed = SmartLabel::labelWithString(label, vec3(1,1,0), vec3(0,0,0), App::getFont());
		label_checked_pressed->setPosition( vec2(checkbox_checked->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
		label_checked_pressed->setAnchorPoint(vec2(0, 0.5f));
		label_checked_pressed->setScale(0.9f);

		Layer * normal = LayerRGBA::node();
		normal->addChild(checkbox_empty);
		normal->addChild(label_normal);
		normal->setContentSize(vec2(label_normal->getPositionX() + label_normal->getContentSize().width, height));

		Layer * normal_pressed = LayerRGBA::node();
		normal_pressed->addChild(checkbox_empty2);
		normal_pressed->addChild(label_normal_pressed);
		normal_pressed->setContentSize(vec2(label_normal_pressed->getPositionX() + label_normal_pressed->getContentSize().width, height));

		CCMenuItemSprite * normal_item = CCMenuItemSprite::itemFromNormalSprite(normal, normal_pressed, this, menu_selector(MenuSettingsWindow::normalButtonPressed));
		// menu_items[i] = normal_item; continue;

		Layer * checked = LayerRGBA::node();
		checked->addChild(checkbox_checked);
		checked->addChild(label_checked);
		checked->setContentSize(vec2(label_checked->getPositionX() + label_checked->getContentSize().width, height));

		Layer * checked_pressed = LayerRGBA::node();
		checked_pressed->addChild(checkbox_checked2);
		checked_pressed->addChild(label_checked_pressed);
		checked_pressed->setContentSize(vec2(label_checked_pressed->getPositionX() + label_checked_pressed->getContentSize().width, height));

		CCMenuItemSprite * checked_item = CCMenuItemSprite::itemFromNormalSprite(checked, checked_pressed, this, menu_selector(MenuSettingsWindow::checkedButtonPressed));

		if((i == 0 && App::settings()->isSoundEnabled()) || (i == 1 && App::settings()->isMusicEnabled())){
			CCMenuItemSprite * temp = normal_item;
			normal_item = checked_item;
			checked_item = temp;
		}

		CCMenuItemToggle * toggle = CCMenuItemToggle::itemWithTarget(this, menu_selector(MenuSettingsWindow::toggleButtonPressed), normal_item, checked_item, NULL);
		menu_items[i] = toggle;
	}

	CCMenuItemToggle * lang_toggle = NULL;
	{
		float max_width = 0;
		JSON * langs = App::getJSON("langs.txt");
		for(i = 0; i < langs->getChildrenCount(); i++){
			JSON * lang = langs->getChildAtIndex(i);
			String name = lang->getName();
			String label = lang->getString();
			String flag_texture_name = String::Format("checkbox-lang-%s.png", name.ToChar());

			lang_names.push_back(name);

			CCSprite * checkbox = CCSprite::spriteWithTexture(App::getTexture(flag_texture_name));
			CCSprite * checkbox2 = CCSprite::spriteWithTexture(App::getTexture(flag_texture_name));

			checkbox->setScale(entity_scale);
			checkbox2->setScale(entity_scale);

			SmartLabel * label_normal = SmartLabel::labelWithString(label, vec3(1,1,1), vec3(0,0,0), App::getFont());
			float height = MathLib::Max(checkbox->getContentSize().height*entity_scale, label_normal->getContentSize().height) + item_padding;
			label_normal->setPosition( vec2(checkbox->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
			label_normal->setAnchorPoint(vec2(0, 0.5f));

			checkbox->setAnchorPoint(vec2(0, 0.5f));
			checkbox2->setAnchorPoint(vec2(0, 0.5f));

			checkbox->setPosition(vec2(0, height * 0.5f));
			checkbox2->setPosition(vec2(0, height * 0.5f));

			SmartLabel * label_normal_pressed = SmartLabel::labelWithString(label, vec3(1,1,0), vec3(0,0,0), App::getFont());
			label_normal_pressed->setPosition( vec2(checkbox->getContentSize().width*entity_scale + padding/2, height * 0.5f) );
			label_normal_pressed->setAnchorPoint(vec2(0, 0.5f));
			label_normal_pressed->setScale(0.9f);

			Layer * normal = LayerRGBA::node();
			normal->addChild(checkbox);
			normal->addChild(label_normal);
			normal->setContentSize(vec2(label_normal->getPositionX() + label_normal->getContentSize().width, height));

			Layer * normal_pressed = LayerRGBA::node();
			normal_pressed->addChild(checkbox2);
			normal_pressed->addChild(label_normal_pressed);
			normal_pressed->setContentSize(vec2(label_normal_pressed->getPositionX() + label_normal_pressed->getContentSize().width, height));

			CCMenuItemSprite * item = CCMenuItemSprite::itemFromNormalSprite(normal, normal_pressed, this, menu_selector(MenuSettingsWindow::normalButtonPressed));
			max_width = MathLib::Max(max_width, item->getContentSize().width);

			if(!lang_toggle){
				lang_toggle = CCMenuItemToggle::itemWithTarget(this, menu_selector(MenuSettingsWindow::toggleButtonPressed), item, NULL);
			}else{
				lang_toggle->addSubItem(item);
			}
		}
		lang_name = App::settings()->getLangName();
		int selected_index = 0;
		CCMutableArray<CCMenuItem*> * sub_items = lang_toggle->getSubItems();
		for(i = 0; i < (int)sub_items->count(); i++){
			CCMenuItemSprite * item = dynamic_cast<CCMenuItemSprite*>(sub_items->getObjectAtIndex(i));
			item->setContentSize(vec2(max_width, item->getContentSize().height));
			item->getNormalImage()->setContentSize(vec2(max_width, item->getContentSize().height));
			item->getSelectedImage()->setContentSize(vec2(max_width, item->getContentSize().height));
			if(lang_names[i] == lang_name){
				selected_index = i;
			}
		}
		lang_toggle->setSelectedIndex((selected_index+1) % (int)sub_items->count()); // it fixes bug of toggle position
		lang_toggle->setSelectedIndex(selected_index);
	}
	CCMenu * checkbox_menu = CCMenu::menuWithItems(menu_items[0], menu_items[1], menu_items[2], lang_toggle, NULL);
	// checkbox_menu->alignItemsVerticallyWithPadding(padding);
	Layer::tableLayout(checkbox_menu, 1, 0, padding * 0.5f);

	CCMenuItem * save_button_item = createLabelButton(App::getLangStr("OK"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(MenuSettingsWindow::saveButtonPressed));

	CCMenuItem * cancel_button_item = createLabelButton(App::getLangStr("CANCEL"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(MenuSettingsWindow::cancelButtonPressed));

	CCMenu * buttons_menu = CCMenu::menuWithItems(cancel_button_item, save_button_item, NULL);
	buttons_menu->alignItemsHorizontallyWithPadding(game_size.y * 0.05f);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(checkbox_menu);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->menu = menu;
		menu->retain();

		setBackgroundScale(App::getEntityScale());

		/*
		ribon = CCSprite::spriteWithFile(String::Format(is_selected ? "mode-%d-selected.png" : (is_enabled ? "mode-%d.png" : "mode-locked.png"), mode+1));
		ribon->setAnchorPoint(vec2(0, 0.5f));
		ribon->setPosition(vec2(padding*2, title->getPositionY() + title->getContentSize().height/2));
		// ribon->setAnchorPoint(vec2(0, 0.5));
		// ribon->setPosition(vec2(padding, getContentSize().height - padding));
		addChild(ribon);
		*/

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::MenuExitWindow::MenuExitWindow()
{
	menu = NULL;
}

MonsterHome::MenuExitWindow::~MenuExitWindow()
{
	menu->release();
}

MonsterHome::MenuExitWindow * MonsterHome::MenuExitWindow::create(MenuWindow * menu)
{
	MenuExitWindow * window = new MenuExitWindow();
	if(window->initWithMenu(menu)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::MenuExitWindow::close()
{
	getParent()->removeChild(this, true);
	// super::close();
}

void MonsterHome::MenuExitWindow::okButtonPressed(CCObject * obj)
{
	// MonsterHome::playHudClickSound();
	App::director()->end();
}

void MonsterHome::MenuExitWindow::cancelButtonPressed(CCObject * obj)
{
	MonsterHome::playHudClickSound();
	close();
}

void MonsterHome::MenuExitWindow::onEnter()
{
	// game->state = MENU_MODE;
	menu->setIsVisible(false);
	super::onEnter();
}

void MonsterHome::MenuExitWindow::onExit()
{
	super::onExit();

	if(menu->getIsRunning()){
		menu->setIsVisible(true);
		menu->open();
	}
}

bool MonsterHome::MenuExitWindow::initWithMenu(MenuWindow * menu)
{
	MonsterHome * game = menu->getGame();
	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;
	float width = 560 * App::getEntityScale();

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("CONFIRMATION"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	SmartLabel * message = SmartLabel::labelWithString(App::getLangStr("EXIT_WARNING"), vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);

	CCMenuItem * ok_button_item = createLabelButton(App::getLangStr("OK"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(MenuExitWindow::okButtonPressed));

	CCMenuItem * cancel_button_item = createLabelButton(App::getLangStr("CANCEL"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(MenuExitWindow::cancelButtonPressed));

	CCMenu * buttons_menu = CCMenu::menuWithItems(cancel_button_item, ok_button_item, NULL);
	buttons_menu->alignItemsHorizontallyWithPadding(game_size.y * 0.05f);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->menu = menu;
		menu->retain();

		setBackgroundScale(App::getEntityScale());

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::ObjectiveWindow::ObjectiveWindow()
{
	game = NULL;
	replay_button_item = NULL;
	buttons_menu = NULL;
	ribon = NULL;
}

MonsterHome::ObjectiveWindow * MonsterHome::ObjectiveWindow::create(MonsterHome * game)
{
	ObjectiveWindow * window = new ObjectiveWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::ObjectiveWindow::okButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	close();
}

void MonsterHome::ObjectiveWindow::replayButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->loadLevel(game->level);
}

void MonsterHome::ObjectiveWindow::menuButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::ObjectiveWindow::savePosition()
{
	super::savePosition();
	ribon_pos = ribon->getPosition();
}

void MonsterHome::ObjectiveWindow::open()
{
	super::open();

	float duration = open_duration;
	vec2 pos = ribon_pos;
	ribon->setPosition(pos - vec2(ribon->getContentSize().width * ribon->getScaleX() * 0.5f, 0));
	ribon->runAction(
		CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(duration * 0.0f),
			EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration * 1.3f, pos), EasyLib::OUT_BACK)
		));
}

void MonsterHome::ObjectiveWindow::onEnter()
{
	game->pauseAll();
	game->state = OBJECTIVE;

	super::onEnter();
}

void MonsterHome::ObjectiveWindow::onExit()
{
	super::onExit();
	
	if(game->getIsRunning() && game->state == OBJECTIVE){
		game->resumeAll();
		game->state = GAME;
	}
}

bool MonsterHome::ObjectiveWindow::initWithGame(MonsterHome * game)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	float message_height = game_size.y * 0.05f;
	// String str = App::splitString(App::getLangStr("OBJECTIVE_1"), 32);
	String str = game->level_info.objective_str;
	SmartLabel * message = SmartLabel::labelWithString(str, vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter); // vec2(1, -1) * message_height * 0.05f, fontname, message_height);

	float title_height = game_size.y * 0.1f;
	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("OBJECTIVE"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont()); // vec2(1, -1) * title_height * 0.05f, fontname, title_height);

	float button_height = game_size.y * 0.05f;

	CCMenuItem * ok_button_item = createLabelButton(App::getLangStr("OK"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(ObjectiveWindow::okButtonPressed));

	CCMenuItem * menu_button_item = createLabelButton(App::getLangStr("MENU"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(ObjectiveWindow::menuButtonPressed));

	replay_button_item = createLabelButton(App::getLangStr("REPLAY"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(ObjectiveWindow::replayButtonPressed));

	// CCMenu * buttons_menu = createButtonsMenu(ok_button_item, menu_button_item, NULL);
	buttons_menu = CCMenu::menuWithItems(menu_button_item, replay_button_item, ok_button_item, NULL);
	// buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	Layer::tableLayout(buttons_menu, 0, 1, button_height, 0, false);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile("ribon-flag.png");
		ribon->setScale(MathLib::Min(App::getEntityScale(), (getContentSize().height-title->getPositionY()+padding)/ribon->getContentSize().height));
		ribon->setAnchorPoint(vec2(-0.3f, 0.93f));
		ribon->setPosition(vec2(0, getContentSize().height));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

void MonsterHome::ObjectiveWindow::setReplayVisible(bool value)
{
	vec2 game_size = game->getContentSize();
	float button_height = game_size.y * 0.05f;
	replay_button_item->setIsVisible(value);
	// buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	Layer::tableLayout(buttons_menu, 0, 1, button_height, 0, false);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::LevelCompletedWindow::LevelCompletedWindow()
{
	game = NULL;
	ribon = NULL;
}

MonsterHome::LevelCompletedWindow * MonsterHome::LevelCompletedWindow::create(MonsterHome * game)
{
	LevelCompletedWindow * window = new LevelCompletedWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::LevelCompletedWindow::nextButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	// close();
	game->loadNextLevel();
}

void MonsterHome::LevelCompletedWindow::replayButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->loadLevel(game->level);
}

void MonsterHome::LevelCompletedWindow::menuButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::LevelCompletedWindow::savePosition()
{
	super::savePosition();
	ribon_pos = ribon->getPosition();
}

void MonsterHome::LevelCompletedWindow::open()
{
	super::open();

	float duration = open_duration;
	vec2 pos = ribon_pos;
	ribon->setPosition(pos - vec2(ribon->getContentSize().width * ribon->getScaleX() * 0.5f, 0));
	ribon->runAction(
		CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(duration * 0.0f),
			EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration * 1.3f, pos), EasyLib::OUT_BACK)
		));
}

void MonsterHome::LevelCompletedWindow::onEnter()
{
	game->pauseAll();
	game->state = LEVEL_COMPLETED;

	super::onEnter();
}

void MonsterHome::LevelCompletedWindow::onExit()
{
	super::onExit();

	// it's all done
}

bool MonsterHome::LevelCompletedWindow::initWithGame(MonsterHome * game)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("LEVEL_COMPLETED"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	SmartLabel * message = SmartLabel::labelWithString(App::getLangStr("LEVEL_COMPLETED_INFO"), vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);
	SmartLabel * old_record = SmartLabel::labelWithString(App::timeToStr(App::settings()->getLevelAward(game->level).sec), vec3(0.5f, 0.9f, 0.5f), vec3(0, 0, 0), App::getBigFont());

	float button_height = game_size.y * 0.05f;

	CCMenuItem * next_button_item = NULL;
	if(!game->isLastGameLevel()){
		next_button_item = createLabelButton(App::getLangStr("NEXT"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
			this, menu_selector(LevelCompletedWindow::nextButtonPressed));
	}

	CCMenuItem * menu_button_item = createLabelButton(App::getLangStr("MENU"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelCompletedWindow::menuButtonPressed));

	CCMenuItem * replay_button_item = createLabelButton(App::getLangStr("REPLAY"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelCompletedWindow::replayButtonPressed));

	CCMenu * buttons_menu = CCMenu::menuWithItems(menu_button_item, replay_button_item, next_button_item, NULL);
	// buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	Layer::tableLayout(buttons_menu, 0, 1, button_height, 0, false);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(old_record);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile("level-completed-flag.png");
		ribon->setScale(MathLib::Min(App::getEntityScale(), (getContentSize().height-title->getPositionY()+padding)/ribon->getContentSize().height));
		ribon->setAnchorPoint(vec2(-0.3f, 0.93f));
		ribon->setPosition(vec2(0, getContentSize().height));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::LevelFailedWindow::LevelFailedWindow()
{
	game = NULL;
	ribon = NULL;
	type = (ELevelFailedType)0;
}

MonsterHome::LevelFailedWindow * MonsterHome::LevelFailedWindow::create(MonsterHome * game, ELevelFailedType type)
{
	LevelFailedWindow * window = new LevelFailedWindow();
	if(window->initWithGame(game, type)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::LevelFailedWindow::replayButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->loadLevel(game->level);
}

void MonsterHome::LevelFailedWindow::menuButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::LevelFailedWindow::onEnter()
{
	game->pauseAll();
	game->state = LEVEL_FAILED;

	super::onEnter();

	if(App::settings()->isSoundEnabled()){
		if(type == LEVEL_FAILED_LIVES){
			int index = App::getRand(game->level_info.active_monsters.size());
			App::playEntitySound(String::Format("monster-%02d", game->level_info.active_monsters[index]), "death");
		}else{
			App::playEntitySound("hud", "missed");
		}
	}
}

void MonsterHome::LevelFailedWindow::onExit()
{
	super::onExit();

	// it's all done
}

bool MonsterHome::LevelFailedWindow::initWithGame(MonsterHome * game, ELevelFailedType type)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("LEVEL_FAILED"), vec3(1, 0, 0), vec3(0, 0, 0), App::getBigFont());

	String message_str = App::getLangStr(type == LEVEL_FAILED_LIVES ? "LEVEL_FAILED_LIVES_INFO" : "LEVEL_FAILED_MISSED_INFO");
	SmartLabel * message = SmartLabel::labelWithString(message_str, vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);

	float button_height = game_size.y * 0.05f;

	CCMenuItem * menu_button_item = createLabelButton(App::getLangStr("MENU"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelFailedWindow::menuButtonPressed));

	CCMenuItem * replay_button_item = createLabelButton(App::getLangStr("REPLAY"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelFailedWindow::replayButtonPressed));

	CCMenu * buttons_menu = CCMenu::menuWithItems(menu_button_item, replay_button_item, NULL);
	// buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	Layer::tableLayout(buttons_menu, 0, 1, button_height, 0, false);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;
		this->type = type;

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile(type == LEVEL_FAILED_LIVES ? "lives-icon2.png" : "missed-icon2.png");
		ribon->setScale(App::getEntityScale());
		ribon->setAnchorPoint(vec2(0, 0.5f));
		ribon->setPosition(vec2(padding*2, title->getPositionY() + title->getContentSize().height/2));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::ModeUnlockedWindow::ModeUnlockedWindow()
{
	game = NULL;
	ribon = NULL;
}

MonsterHome::ModeUnlockedWindow * MonsterHome::ModeUnlockedWindow::create(MonsterHome * game)
{
	ModeUnlockedWindow * window = new ModeUnlockedWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::ModeUnlockedWindow::close()
{
	// getParent()->removeChild(this, true);
	super::close();
}

void MonsterHome::ModeUnlockedWindow::okButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	// close();

	game->state = GAME;
	game->resumeAll();

	game->showStartObjectiveWindow();

	if(game->getChildren()->containsObject(game->objective_window)){
		getParent()->removeChild(this, true);
	}else{
		close();
	}
}

void MonsterHome::ModeUnlockedWindow::menuButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::ModeUnlockedWindow::onEnter()
{
	game->pauseAll();
	game->state = MODE_UNLOCKED;

	super::onEnter();
}

void MonsterHome::ModeUnlockedWindow::onExit()
{
	super::onExit();

	// it's all done
}

bool MonsterHome::ModeUnlockedWindow::initWithGame(MonsterHome * game)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("CONGRATULATIONS"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());

	String message_str = App::getLangStr(String::Format("MODE_%d_UNLOCKED_INFO", App::settings()->getMode()+1));
	SmartLabel * message = SmartLabel::labelWithString(message_str, vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);

	float button_height = game_size.y * 0.05f;

	CCMenuItem * menu_button_item = createLabelButton(App::getLangStr("MENU"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(ModeUnlockedWindow::menuButtonPressed));

	CCMenuItem * ok_button_item = createLabelButton(App::getLangStr("OK"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(ModeUnlockedWindow::okButtonPressed));

	CCMenu * buttons_menu = CCMenu::menuWithItems(menu_button_item, ok_button_item, NULL);
	// buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	Layer::tableLayout(buttons_menu, 0, 1, button_height, 0, false);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile(String::Format("mode-%d-selected.png", App::settings()->getMode()+1));
		ribon->setScale(App::getEntityScale());
		ribon->setAnchorPoint(vec2(0, 0.5f));
		ribon->setPosition(vec2(padding*2, title->getPositionY() + title->getContentSize().height/2));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::LevelRecordWindow::LevelRecordWindow()
{
	game = NULL;
	ribon = NULL;
}

MonsterHome::LevelRecordWindow * MonsterHome::LevelRecordWindow::create(MonsterHome * game)
{
	LevelRecordWindow * window = new LevelRecordWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::LevelRecordWindow::nextButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	if(game->showGameFinishedWindow()){
		game->addChild(GameFinishedWindow::create(game), WINDOW);
		getParent()->removeChild(this, true); // it destroyes LevelRecordWindow

		App::settings()->setGameFinished(true);
		App::settings()->save();
	}else{
		game->loadNextLevel();
	}
}

void MonsterHome::LevelRecordWindow::replayButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	game->loadLevel(game->level);
}

void MonsterHome::LevelRecordWindow::menuButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::LevelRecordWindow::savePosition()
{
	super::savePosition();
	ribon_pos = ribon->getPosition();
}

void MonsterHome::LevelRecordWindow::open()
{
	super::open();

	float duration = open_duration;
	ribon->setRotation(0);
	ribon->runAction(CCSequence::actions(
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -15.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 15.0f), EasyLib::IN_OUT_QUAD)), 5),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -10.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 10.0f), EasyLib::IN_OUT_QUAD)), 4),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -5.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 5.0f), EasyLib::IN_OUT_QUAD)), 3),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -4.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 4.0f), EasyLib::IN_OUT_QUAD)), 2),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -3.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 3.0f), EasyLib::IN_OUT_QUAD)), 2),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -2.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 2.0f), EasyLib::IN_OUT_QUAD)), 2),
		CCRepeat::actionWithAction(CCSequence::actionOneTwo(
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, -1.0f), EasyLib::IN_OUT_QUAD),
			EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 1.0f), EasyLib::IN_OUT_QUAD)), 5),
		EasyAction::actionWithAction(CCRotateTo::actionWithDuration(duration / 2, 0.0f), EasyLib::IN_OUT_QUAD),
		NULL
	));

	/*
	vec2 pos = ribon_pos;
	ribon->setPosition(pos - vec2(ribon->getContentSize().width * 0.5f, 0));
	ribon->runAction(
		CCSequence::actionOneTwo(
			CCDelayTime::actionWithDuration(duration * 0.0f),
			EasyAction::actionWithAction(CCMoveTo::actionWithDuration(duration * 1.3f, pos), EasyLib::OUT_BACK)
		));
	*/
}

void MonsterHome::LevelRecordWindow::onEnter()
{
	game->pauseAll();
	game->state = LEVEL_NEW_RECORD;

	super::onEnter();
}

void MonsterHome::LevelRecordWindow::onExit()
{
	super::onExit();

	/* struct Lib {
		static 
	};	
	game->scheduleOnce( */

#if 1
	// everything is done inside of button click function
#else
	if(game->getIsRunning()){ // !App::isEnding()){
		game->resumeAll();
		// game->state = GAME;
		game->loadNextLevel();
	}
#endif
}

bool MonsterHome::LevelRecordWindow::initWithGame(MonsterHome * game)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("LEVEL_RECORD"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	SmartLabel * message = SmartLabel::labelWithString(App::timeToStr(game->level_time_sec), vec3(0.5f, 0.9f, 0.5f), vec3(0, 0, 0), App::getBigFont());

	float button_height = game_size.y * 0.05f;

	CCMenuItem * next_button_item = NULL;
	if(!game->isLastGameLevel() || game->showGameFinishedWindow()){
		next_button_item = createLabelButton(App::getLangStr("NEXT"), 
			App::getTexture("button.png"), App::getTexture("button-pressed.png"),
			vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
			this, menu_selector(LevelRecordWindow::nextButtonPressed));
	}

	CCMenuItem * menu_button_item = createLabelButton(App::getLangStr("MENU"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelRecordWindow::menuButtonPressed));

	CCMenuItem * replay_button_item = createLabelButton(App::getLangStr("REPLAY"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(LevelRecordWindow::replayButtonPressed));

	// CCMenu * buttons_menu = createButtonsMenu(ok_button_item, menu_button_item, NULL);
	CCMenu * buttons_menu = CCMenu::menuWithItems(menu_button_item, replay_button_item, next_button_item, NULL);
	buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		ribon = CCSprite::spriteWithFile("award-first.png");
		// ribon->setScale(MathLib::Min(App::getEntityScale(), (getContentSize().height-title->getPositionY()+padding)/ribon->getContentSize().height));
		ribon->setScale(App::getEntityScale());
		ribon->setAnchorPoint(vec2(0.5f, 0.65f));
		ribon->setPosition(vec2(ribon->getContentSize().width * ribon->getScaleX() * 0.05f, getContentSize().height));
		addChild(ribon);

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

MonsterHome::GameFinishedWindow::GameFinishedWindow()
{
	game = NULL;
}

MonsterHome::GameFinishedWindow * MonsterHome::GameFinishedWindow::create(MonsterHome * game)
{
	GameFinishedWindow * window = new GameFinishedWindow();
	if(window->initWithGame(game)){
		window->autorelease();
		return window;
	}
	delete window;
	return NULL;
}

void MonsterHome::GameFinishedWindow::okButtonPressed(CCObject*)
{
	MonsterHome::playHudClickSound();
	MonsterHome::gotoMenuScene();
}

void MonsterHome::GameFinishedWindow::onEnter()
{
	game->pauseAll();
	game->state = GAME_FINISHED;

	super::onEnter();
}

bool MonsterHome::GameFinishedWindow::initWithGame(MonsterHome * game)
{
	float width = 650 * App::getEntityScale();

	vec2 game_size = game->getContentSize();
	float padding = game_size.x * 0.025f;

	SmartLabel * title = SmartLabel::labelWithString(App::getLangStr("CONGRATULATIONS"), vec3(1, 1, 0.5f), vec3(0, 0, 0), App::getBigFont());
	SmartLabel * message = SmartLabel::labelWithString(App::getLangStr("GAME_FINISHED_INFO"), vec3(1, 1, 1), vec3(0, 0, 0), App::getFont(), width - padding*4, CCTextAlignmentCenter);

	float button_height = game_size.y * 0.05f;

	CCMenuItem * ok_button_item = createLabelButton(App::getLangStr("OK"), 
		App::getTexture("button.png"), App::getTexture("button-pressed.png"),
		vec3(1,1,1), vec3(1,1,0), vec3(0,0,0), App::getFont(), App::getFont(), App::getEntityScale(),
		this, menu_selector(GameFinishedWindow::okButtonPressed));

	// CCMenu * buttons_menu = createButtonsMenu(ok_button_item, menu_button_item, NULL);
	CCMenu * buttons_menu = CCMenu::menuWithItems(ok_button_item, NULL);
	buttons_menu->alignItemsHorizontallyWithPadding(button_height);
	// buttons_menu->alignItemsVerticallyWithPadding(button_height*0.5f);
	// buttons_menu = prepareMenu(buttons_menu);

	CCMutableArray<CCNode*> * items = new CCMutableArray<CCNode*>();
	items->autorelease();
	items->addObject(title);
	items->addObject(message);
	items->addObject(buttons_menu);

	if(super::initWithItems(width, padding, App::getTexture("window.png"), vec2(128,128), vec2(128,128), items)){
		this->game = game;

		setBackgroundScale(App::getEntityScale());

		setPosition(vec2(game->getContentSize()) * 0.5f);
		return true;
	}
	return false;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

SplashScreen::LoadStep::LoadStep(float weight)
{
	this->weight = weight;
	autorelease();
}

SplashScreen::LoadResourceStep::LoadResourceStep(const char * filename, float weight): LoadStep(weight)
{
	this->filename = filename;
}

void SplashScreen::LoadImageStep::step()
{
	App::getTexture(filename);
}

void SplashScreen::LoadJSONStep::step()
{
	App::getJSON(filename);
}

void SplashScreen::LoadSoundStep::step()
{
	App::preloadSound(filename);
}

void SplashScreen::LoadMusicStep::step()
{
	App::preloadMusic(filename);
}

// ===============================================================================

CCScene * SplashScreen::scene()
{
	IW_CALLSTACK("SplashScreen::scene");
	SplashScreen * splash = SplashScreen::node();
	if(splash){
		CCScene * scene = CCScene::node();
		scene->addChild(splash);
		return scene;
	}
	return NULL;
}

SplashScreen::SplashScreen()
{
	sum_weight = 0;
	cur_step = 0;
	cur_weight = 0;
	logo = NULL;
	progressbar_border = NULL;
	progress = NULL;
	waiting_time_sec = 0;
}

void SplashScreen::schedulerWaitLogoMusic(ccTime dt)
{
	waiting_time_sec += dt;
	if( // waiting_time_sec > 0.3f 
		progressbar_border->getOpacity() == 0
		&& (!App::isMusicPlaying() || App::getMusicFilename() == App::settings()->getModeMusicFilename())){
		unschedule(schedule_selector(SplashScreen::schedulerWaitLogoMusic));
		
		struct Lib {
			static void finished(CCObject*, void*)
			{
				MonsterHome::gotoMenuScene();
			}
		};
		scheduleOnce(Lib::finished, 0.1f);
		return;
	}
}

void SplashScreen::schedulerLoadStep(ccTime)
{
	if(cur_step >= (int)steps.count()){
		unschedule(schedule_selector(SplashScreen::schedulerLoadStep));
		schedule(schedule_selector(SplashScreen::schedulerWaitLogoMusic), 1.0f / 10.0f);

		cur_weight = sum_weight;
		progress->setPercentage(100);

		struct Lib {
			static void finished(CCObject * target, void*)
			{
				SplashScreen * self = dynamic_cast<SplashScreen*>(target);
				self->progress->getParent()->removeChild(self->progress, true);
				self->progress = NULL;

				CCSprite * progressbar_border = self->progressbar_border;
				CCSprite * progress = CCSprite::spriteWithFile("progressbar.png");
				progress->setAnchorPoint(progressbar_border->getAnchorPoint());
				progress->setPosition(progressbar_border->getPosition());
				progress->setScale(progressbar_border->getScale());
				self->addChild(progress, 1);

				float duration = 0.2f;
				progressbar_border->runAction(CCFadeOut::actionWithDuration(duration));
				progress->runAction(CCFadeOut::actionWithDuration(duration));
			}
		};
		scheduleOnce(Lib::finished, 0.001f);
		return;
	}
	LoadStep * step = steps.getObjectAtIndex(cur_step++);
	step->step();
	cur_weight += step->weight;
	progress->setPercentage(cur_weight * 100.0f / sum_weight);
}

void SplashScreen::startLoading()
{
	vec2 size = getContentSize();
	
	progressbar_border = CCSprite::spriteWithFile("progressbar-border.png");
	progressbar_border->setAnchorPoint(vec2(0.5f, 0));
	progressbar_border->setPosition(vec2(size.x * 0.5f, size.y * 0.05f));
	progressbar_border->setScale(logo->getScale());
	addChild(progressbar_border, 1);

	if(App::isAdmobVisible()){
		float admob_height = (float)App::getAdmobDesc()->height + size.y * 0.03f;
		// vec2 parent_size = getParent()->getContentSize();
		vec2 size = progressbar_border->getContentSize();
		vec2 pos = progressbar_border->getPosition();
		vec2 anchor = progressbar_border->getAnchorPoint();
		float low_y = pos.y - size.y * anchor.y;
		if(low_y < admob_height){
			progressbar_border->setPosition(pos + vec2(0, admob_height - low_y));
		}
	}

	progress = CCProgressTimer::progressWithFile("progressbar.png");
	progress->setAnchorPoint(progressbar_border->getAnchorPoint());
	progress->setPosition(progressbar_border->getPosition());
	progress->setScale(progressbar_border->getScale());
	progress->setType(kCCProgressTimerTypeHorizontalBarLR);
	progress->setPercentage(0);
	addChild(progress, 1);

	progressbar_border->setOpacity(0);
	progressbar_border->runAction(CCFadeIn::actionWithDuration(0.3f));

	{
		int monsters[] = {1,2,3,7};
		const char * sound_types[] = {"pain", "death"};
		for(int i = 0; i < ARR_SIZE(monsters); i++){
			steps.addObject(new LoadImageStep(String::Format("monster-%02d.png", monsters[i]), 2.0f));
			steps.addObject(new LoadJSONStep(String::Format("monster-%02d.txt", monsters[i])));
			
			for(int j = 0; j < ARR_SIZE(sound_types); j++){
				for(int k = 1; k <= 3; k++){
					String filename = String::Format("monster-%02d-%s-%d.wav", monsters[i], sound_types[j], k);
					if(App::isFileExist(filename)){
						steps.addObject(new LoadSoundStep(filename));
						App::registerEntitySound(String::Format("monster-%02d", monsters[i]), sound_types[j], filename);
					}
				}
			}
		}
		steps.addObject(new LoadImageStep("die-09.png")); // used by monster-02
	}
	{
		steps.addObject(new LoadSoundStep("missed.wav"));
		App::registerEntitySound("hud", "missed", "missed.wav");

		steps.addObject(new LoadSoundStep("click.wav"));
		App::registerEntitySound("hud", "click", "click.wav");

		for(int i = 1; i <= 20; i++){
			String filename = String::Format("ambient-%02d.wav", i);
			if(App::isFileExist(filename)){
				steps.addObject(new LoadSoundStep(filename));
				App::registerEntitySound("game", "ambient", filename);
			}
		}		
	}
	{
		for(int i = 1; i <= 15; i++){
			steps.addObject(new LoadImageStep(String::Format("tiled-%02d.jpg", i), 2));
		}
	}
	steps.addObject(new LoadImageStep(String(App::getBigFont()).Replace(".fnt", "_0.png")));
	steps.addObject(new LoadImageStep(String(App::getFont()).Replace(".fnt", "_0.png")));

	steps.addObject(new LoadImageStep("free.png"));
	steps.addObject(new LoadImageStep("full.png"));
	steps.addObject(new LoadImageStep("award-first.png"));
	steps.addObject(new LoadImageStep("button.png"));
	steps.addObject(new LoadImageStep("button-pressed.png"));
	steps.addObject(new LoadImageStep("button-icon.png"));
	steps.addObject(new LoadImageStep("button-icon-pressed.png"));
	steps.addObject(new LoadImageStep("checkbox-checked.png"));
	steps.addObject(new LoadImageStep("checkbox-empty.png"));
	steps.addObject(new LoadImageStep("checkbox-red.png"));
	steps.addObject(new LoadImageStep("cloud-1.png"));
	steps.addObject(new LoadImageStep("cloud-2.png"));
	steps.addObject(new LoadImageStep("dist-warn.png"));
	steps.addObject(new LoadImageStep("exit.png"));
	steps.addObject(new LoadImageStep("exit-pressed.png"));
	steps.addObject(new LoadImageStep("hole-select.png"));
	steps.addObject(new LoadImageStep("home-01.png"));
	steps.addObject(new LoadJSONStep("home-01.txt"));
	steps.addObject(new LoadImageStep("home-02.png"));
	steps.addObject(new LoadJSONStep("home-02.txt"));
	steps.addObject(new LoadImageStep("hud-fill.png"));
	steps.addObject(new LoadImageStep("level-button.png"));
	steps.addObject(new LoadImageStep("level-button-disabled.png"));
	steps.addObject(new LoadImageStep("level-button-pressed.png"));
	steps.addObject(new LoadImageStep("level-completed-flag.png"));
	steps.addObject(new LoadImageStep("lives-icon.png"));
	steps.addObject(new LoadJSONStep("lives-icon.txt"));
	steps.addObject(new LoadImageStep("lives-icon2.png"));
	steps.addObject(new LoadImageStep("missed-icon.png"));
	steps.addObject(new LoadJSONStep("missed-icon.txt"));
	steps.addObject(new LoadImageStep("missed-icon2.png"));
	steps.addObject(new LoadImageStep("mode-locked.png"));
	// steps.addObject(new LoadImageStep("mode-disabled-pressed.png"));
	steps.addObject(new LoadImageStep("monsters-icon.png"));
	steps.addObject(new LoadJSONStep("monsters-icon.txt"));
	steps.addObject(new LoadImageStep("path-seg.png"));
	steps.addObject(new LoadImageStep("pause-icon.png"));
	steps.addObject(new LoadImageStep("pause-icon-pressed.png"));
	steps.addObject(new LoadImageStep("points-icon.png"));
	steps.addObject(new LoadJSONStep("points-icon.txt"));
	steps.addObject(new LoadImageStep("ribon-flag.png"));
	steps.addObject(new LoadImageStep("trava.png"));
	steps.addObject(new LoadImageStep("window.png"));
	// steps.addObject(new LoadImageStep(""));

	{
		for(int i = 1; i <= Settings::MODES_NUMBER; i++){
			steps.addObject(new LoadImageStep(String::Format("mode-%d.png", i)));
			steps.addObject(new LoadImageStep(String::Format("mode-%d-icon.png", i)));
			steps.addObject(new LoadImageStep(String::Format("mode-%d-pressed.png", i)));
			steps.addObject(new LoadImageStep(String::Format("mode-%d-selected.png", i)));
		}
	}

	{
		JSON * langs = App::getJSON("langs.txt");
		for(int i = 0; i < langs->getChildrenCount(); i++){
			JSON * lang = langs->getChildAtIndex(i);
			String name = lang->getName();
			steps.addObject(new LoadImageStep(String::Format("checkbox-lang-%s.png", name.ToChar())));
			steps.addObject(new LoadJSONStep(String::Format("lang-%s.txt", name.ToChar())));
		}
	}

	if(App::settings()->isMusicEnabled()){
	#if 0
		steps.addObject(new LoadMusicStep(App::settings()->getModeMusicFilename(), 10));
	#else
		class LoadAndPlayMusic: public LoadMusicStep
		{
		public:

			LoadAndPlayMusic(float weight = 10.0f): LoadMusicStep(App::settings()->getModeMusicFilename(), weight){}
			virtual void step()
			{
				LoadMusicStep::step();
				if(App::settings()->isMusicEnabled()){
					App::playMusic(filename);
				}
			}
		};
		// steps.addObject(new LoadAndPlayMusic());
		steps.insertObjectAtIndex(new LoadAndPlayMusic(), steps.count() * 8 / 10);
	#endif
	}

	if(1){
		class GameLogo: public LoadStep
		{
		public:

			SplashScreen * screen;

			GameLogo(SplashScreen * p_screen): LoadStep(3.0f)
			{
				screen = p_screen;
			}

			void step()
			{
				screen->showGameDraftLogo();
			}
		};
		steps.insertObjectAtIndex(new GameLogo(this), (int)(steps.count() * 0.32));
	}

	sum_weight = 0.0001f;
	for(int i = 0; i < (int)steps.count(); i++){
		sum_weight += steps.getObjectAtIndex(i)->weight;
	}

	float dt = 1.0f / 20.0f, max_load_time = 4.0f;
	if(dt * (float)steps.count() > max_load_time){
		dt = max_load_time / (float)steps.count();
	}
	schedule(schedule_selector(SplashScreen::schedulerLoadStep), 1.0f/20.0f);
}

void SplashScreen::showGameLogo()
{
	vec2 size = getContentSize();
	CCSprite * logo = CCSprite::spriteWithFile(String::Format("splash-game-%d.jpg", size.x >= 600 ? 1024 : 512));
	logo->setAnchorPoint(vec2(0.5f, 0.5f));
	logo->setPosition(size * vec2(0.5f, 0.5f));

	vec2 scale = size / logo->getContentSize();
	logo->setScale(scale.Max());
	addChild(logo);

#if 0
	screen->removeChild(screen->logo, true);
	screen->logo = logo;
#else
	struct Lib {
		static void finished(CCObject * target, void * logo)
		{
			SplashScreen * screen = dynamic_cast<SplashScreen*>(target);
			screen->removeChild(screen->logo, true);
			screen->logo = dynamic_cast<CCSprite*>((CCSprite*)logo);
		}
	};

	logo->setOpacity(0);
	logo->runAction(CCSequence::actionOneTwo(
		CCFadeIn::actionWithDuration(0.9f),
		getFuncAction(Lib::finished, logo)
	));

	// screen->logo->stopAllActions();
	// screen->logo->runAction(CCFadeOut::actionWithDuration(0.3f));
#endif
}

void SplashScreen::showGameDraftLogo()
{
	vec2 size = getContentSize();
	CCSprite * logo = CCSprite::spriteWithFile(String::Format("splash-game-%d-draft.jpg", size.x >= 600 ? 1024 : 512));
	logo->setAnchorPoint(vec2(0.5f, 0.5f));
	logo->setPosition(size * vec2(0.5f, 0.5f));

	vec2 scale = size / logo->getContentSize();
	logo->setScale(scale.Max());
	addChild(logo);

#if 0
	screen->removeChild(screen->logo, true);
	screen->logo = logo;
#else
	struct Lib {
		static void nextLogo(CCObject * target, void * logo)
		{
			SplashScreen * screen = dynamic_cast<SplashScreen*>(target);
			screen->removeChild(screen->logo, true);
			screen->logo = dynamic_cast<CCSprite*>((CCSprite*)logo);
			screen->showGameLogo();
		}
	};

	logo->setOpacity(0);
	logo->runAction(CCSequence::actions(
		CCFadeIn::actionWithDuration(0.9f),
		CCDelayTime::actionWithDuration(0.4f),
		getFuncAction(Lib::nextLogo, logo),
		NULL
	));

	// screen->logo->stopAllActions();
	// screen->logo->runAction(CCFadeOut::actionWithDuration(0.3f));
#endif
}

bool SplashScreen::init()
{
	if(super::init()){
		
		if(App::isDummyAdmob()){
			struct Lib {
				static void drawAdmob(CCObject * target, void*)
				{
					App::drawAdmob();
				}
			};
			addChild(FuncDrawLayer::createWithTarget(this, Lib::drawAdmob), 100);
		}

		vec2 size = getContentSize();
		float admob_height = App::isAdmobVersion() ? (float)App::getAdmobDesc()->height : 0;
		size.y -= admob_height;

		logo = CCSprite::spriteWithFile("splash-company-512.png");
		logo->setAnchorPoint(vec2(0.5f, 0.5f));
		logo->setPosition(size * vec2(0.5f, 0.55f) + vec2(0, admob_height));

		float entity_scale = App::getEntityScale();
		float eps = 0.1f; // entity_scale >= 1.0f ? 0.5f : 0.1f;
		float fill_t = entity_scale >= 1.0f ? 0.0f : MathLib::Min(1.0f, 0.6f/entity_scale);
		float fill = MathLib::Lerp(0.7f, 0.9f, fill_t);
		logo->setScale(MathLib::EpsFloor((size * fill / vec2(logo->getContentSize())).Min(), eps));
		addChild(logo);

		logo->setOpacity(0);
		logo->runAction(CCFadeIn::actionWithDuration(0.3f));

		if(App::settings()->isMusicEnabled() || App::settings()->isSoundEnabled()){
			App::playMusic("logo.mp3", false);
		}

		struct Lib {
			static void start(CCObject * target, void*)
			{
				SplashScreen * self = dynamic_cast<SplashScreen*>(target);
				self->startLoading();
			}
		};
		scheduleOnce(Lib::start, 0.3f);

		return true;
	}
	return false;
}

void SplashScreen::draw()
{
	Rect2 rect(vec2(0,0), getContentSize());
	CCPoint vertices[4] = {
		ccp(rect.origin.x, rect.origin.y), 
		ccp(rect.origin.x + rect.size.x, rect.origin.y),
		ccp(rect.origin.x + rect.size.x, rect.origin.y + rect.size.y), 
		ccp(rect.origin.x, rect.origin.y + rect.size.y)
	};
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	ccDrawPoly(vertices, 4, true, true);
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

CCScene * MonsterHome::scene(int level)
{
	IW_CALLSTACK("MonsterHome::scene");
	MonsterHome * game = MonsterHome::node();
	if(game){
		// game->state
		// game->level = level;
		game->loadLevel(level);
		game->level_info.show_start_objective = true;

		CCScene * scene = CCScene::node();
		scene->addChild(game);
		return scene;
	}
	return NULL;
}

MonsterHome::MonsterHome()
{
	// entity_scale = 0;
	background_layer = NULL;
	hud_layer = NULL;
	hud_mode_icon = NULL;
	hud_monsters_icon = NULL;
	hud_monsters_number = NULL;
	hud_monsters_number_req = NULL;
	monsters_number = 0;
	hud_lives_icon = NULL;
	hud_lives_number = NULL;
	lives_number = 0;
	hud_missed_icon = NULL;
	hud_max_missed_monsters = NULL;
	max_missed_monsters_number = 0;
	hud_points_icon = NULL;
	hud_points_number = NULL;
	points_number = 0;
	start_level_points_number = 0;
	level_time_sec = 0;
	path_layer = NULL;
	monsters = NULL;
	homes = NULL;
	selected_monster = NULL;
	path = NULL;
	state = LOADING;
	objective_window = NULL;
	// menu_window = NULL;
	trava = NULL;
	clouds = NULL;
	ambient_sound_action = NULL;
	top_padding = 0;
	bottom_padding = 0;
}

MonsterHome::~MonsterHome()
{
	CC_SAFE_RELEASE_NULL(path);
	CC_SAFE_RELEASE_NULL(objective_window);
	// CC_SAFE_RELEASE_NULL(menu_window);
	// CC_SAFE_RELEASE_NULL(trava);
	// CC_SAFE_RELEASE_NULL(clouds);
}

bool MonsterHome::init()
{
	if(super::init()){
		setIsTouchEnabled(true);
		// setIsRelativeAnchorPoint(true);
		// setAnchorPoint(ccp(0.5f, 0.5f));
		return true;
	}
	return false;
}

void MonsterHome::onEnter()
{
	super::onEnter();
}

void MonsterHome::onEnterTransitionDidFinish()
{
	super::onEnterTransitionDidFinish();
	startLevel();
}

void MonsterHome::setLevelTime(float time_sec)
{
	int old_vis_time_sec = (int)level_time_sec;
	level_time_sec = time_sec;
	int vis_time_sec = (int)level_time_sec;
	if(state != GAME || old_vis_time_sec != vis_time_sec){
		hud_level_time->setString(App::timeToStr(level_time_sec));
	}
}

void MonsterHome::update(ccTime dt)
{
	// IW_CALLSTACK("MonsterHome::update");	
	// App::updateEntitySounds();

	float time_sec = App::getTimeSec() - 3.0f;
	for(int i = 0; i < (int)ignore_fight_rects.size();){
		if(ignore_fight_rects[i].time_sec < time_sec){
			ignore_fight_rects.erase(ignore_fight_rects.begin()+i, ignore_fight_rects.begin()+i+1);
		}else{
			i++;
		}
	}
	if(state != GAME){
		return;
	}
	setLevelTime(level_time_sec + dt);
}

void MonsterHome::schedulerUpdate(ccTime)
{
	// IW_CALLSTACK("MonsterHome::schedulerUpdate");
	if(state != GAME && state != MENU){
		return;
	}

	checkMonsters();

	Entity * ent;
	int i, j, count = monsters->getChildrenCount();
	CCArray * children = monsters->getChildren();

	int home_count = homes->getChildrenCount();
	CCArray * home_children = homes->getChildren();

	Rect2 display_rect(vec2(0, 0), monsters->getContentSize());
	float display_size = display_rect.size.Max();

	Rect2 fight_rect(vec2(0, 0), monsters->getContentSize());
	if(state != MENU){
		fight_rect.origin.y += top_padding;
		fight_rect.size.y -= top_padding + bottom_padding;
		float edge = fight_rect.size.Min() * -0.1f;
		fight_rect.Expand(vec2(edge, edge));			
	}else{
		fight_rect.size.y -= bottom_padding * 1.5f;
	}

	for(i = 0; i < count; i++){
		ent = dynamic_cast<Entity*>(children->objectAtIndex(i));
		if(!ent->isResponsible()){
			continue;
		}
		for(j = 0; j < home_count; j++){
			Entity * home = dynamic_cast<Entity*>(home_children->objectAtIndex(j));
			if(isMonsterHome(ent, home)){
				if(selected_monster == ent){
					home->startWarning();
				}else if(ent->rect_cache.rect.IntersectRect(home->rect_cache.expaned)){
					ent->startHomeMove(home);
				}
			}
		}
		if(!ent->isResponsible()){
			continue;
		}
		if(level_info.max_missed_monsters > 0 && !fight_rect.ContainsRect(ent->rect_cache.rect) && ent->path_action){
			vec2 offs = ent->path_action->last_move_dir * (display_size * 0.1f);
			vec2 pos = vec2(ent->getPosition()) + offs;
			for(int i = 1; i < 3; i++){
				if(!display_rect.ContainsPoint(pos)){
					ent->startWarning();
					break;
				}
				if(fight_rect.ContainsPoint(pos)){
					break;
				}
				pos = pos + offs;
			}
		}
		for(j = i+1; j < count; j++){
			Entity * ent2 = dynamic_cast<Entity*>(children->objectAtIndex(j));
			if(!ent2->isResponsible()){
				continue;
			}
			if(ent->rect_cache.rect.IntersectRect(ent2->rect_cache.rect)){
				if(fight_rect.ContainsRect(ent->rect_cache.rect) && fight_rect.ContainsRect(ent2->rect_cache.rect)){
					bool ignore_fight = false;
					for(int i = 0; i < (int)ignore_fight_rects.size(); i++){
						if(ent->rect_cache.rect.IntersectRect(ignore_fight_rects[i].rect)){
							ignore_fight = true;
							break;
						}
					}
					if(!ignore_fight){
						fightMonsters(ent, ent2);
					}
				}else{
					distanceWarningMonsters(ent, ent2);
				}
				continue;
			}
			if(ent->rect_cache.expaned.IntersectRect(ent2->rect_cache.expaned)){
				distanceWarningMonsters(ent, ent2);
				continue;
			}
		}
	}
	if(state != GAME){
		return;
	}
	if(level_info.req_monsters > 0 && monsters_number >= level_info.req_monsters){
		Settings::LevelAward award = App::settings()->getLevelAward(level);
		if(award.sec == 0 || (int)level_time_sec < award.sec){
			award.sec = (int)level_time_sec;
			App::settings()->setLevelAward(level, award);
			App::settings()->save();
			// loadLevel(level + 1);
			addChild(LevelRecordWindow::create(this), WINDOW);
		}else{
			// loadNextLevel();
			addChild(LevelCompletedWindow::create(this), WINDOW);
		}
	}else if(level_info.lives > 0 && lives_number < 1){
		// no lives
		// restartLevel(RESTART_BY_NO_LIVES);
		addChild(LevelFailedWindow::create(this, LEVEL_FAILED_LIVES), WINDOW);
	}else if(level_info.max_missed_monsters > 0 && max_missed_monsters_number < 1){
		// missed over
		// restartLevel(RESTART_BY_MAX_MISSED);
		addChild(LevelFailedWindow::create(this, LEVEL_FAILED_MISSES), WINDOW);
	}
}

void MonsterHome::distanceWarningMonsters(Entity * ent, Entity * ent2)
{
#if 0
	IW_CALLSTACK("MonsterHome::distanceWarningMonsters");
	ASSERT(ent->isResponsible() && ent2->isResponsible());
	if(0 && ent->info.name == ent2->info.name){
		return;
	}
	ent->startWarning();
	ent2->startWarning();
#endif
}

void MonsterHome::fightMonsters(Entity * ent, Entity * ent2)
{
	IW_CALLSTACK("MonsterHome::fightMonsters");
	ASSERT(ent->isResponsible() && ent2->isResponsible());
	if(level_info.attack_type == MONSTER_ATTACK_DISABLED){
		return;
	}
	if(level_info.attack_type == MONSTER_ATTACK_DIFFERENT && ent->info.name == ent2->info.name){
		return;
	}
	ent->fight(ent2);
}

/*
void MonsterHome::precacheLevel(int level)
{
	static bool is_cached = false;
	if(is_cached){
		return;
	}
	is_cached = true;

	App::getLangStr("dummy");
	{
		const char * precache_images[] = {
			"window.png",
			"ribon-flag.png",
			"path-seg.png", 
			"dist-warn.png",
			"hole-select.png",
			"home-01.png",
			"home-02.png",
			"monster-01.png",
			"monster-02.png",
			"monster-03.png",
			"monster-07.png",
		};
		for(int i = 0; i < ARR_SIZE(precache_images); i++){
			App::getTexture(precache_images[i]);
		}
	}

	{
		const char * precache_json[] = {
			"home-01.txt",
			"home-02.txt",
			"monster-01.txt",
			"monster-02.txt",
			"monster-03.txt",
			"monster-07.txt",
		};
		for(int i = 0; i < ARR_SIZE(precache_json); i++){
			App::getJSON(precache_json[i]);
		}
	}
#if 0
	CCLabelBMFont::labelWithString("123abc", App::getBigFont()); // auto release
	CCLabelBMFont::labelWithString("123abc", App::getFont());
	CCLabelBMFont::labelWithString("123abc", App::getFont());

	CCPoolManager::deletePoolManager();
#endif
}
*/

/*
void MonsterHome::restartLevel(ERestartLevelType)
{
	points_number = start_level_points_number;
	loadLevel(level);
}
*/

void MonsterHome::pauseButtonPressed(CCObject * obj)
{
	if(state != GAME){
		return;
	}
	MonsterHome::playHudClickSound();
	ASSERT(objective_window);
	objective_window->setReplayVisible(level_time_sec > 1);
	addChild(objective_window, WINDOW);
}

void MonsterHome::buyButtonPressed(CCObject*)
{
	App::buyGame();
}

void MonsterHome::showStartWindow()
{
	ASSERT(state == LOADING);
	state = GAME;
	if(0 && App::settings()->isModeJustUnlocked(App::settings()->getMode())){
		App::settings()->resetModeJustUnlocked();
		App::settings()->save();
		addChild(ModeUnlockedWindow::create(this), WINDOW);
	}else{
		showStartObjectiveWindow();
	}
}

void MonsterHome::showStartObjectiveWindow()
{
	if(level_info.show_start_objective){
		// pauseButtonPressed(NULL);
		ASSERT(objective_window);
		objective_window->setReplayVisible(level_time_sec > 1);
		addChild(objective_window, WINDOW);
	}
}

void MonsterHome::loadNextLevel()
{
	loadLevel(level+1);
}

void MonsterHome::createTrava()
{
	ASSERT(trava);
	trava->removeAllChildrenWithCleanup(true);
	// trava->setIsVisible(true);

#if 1
	CCTexture2D * texture = App::getTexture("trava.png");
	vec2 size = getContentSize();
	float scale = App::getEntityScale();
	tileRectWithSprites(trava, Rect2(vec2(0,0), vec2(size.x, texture->getContentSize().height * scale)), texture, vec2(scale, scale));
#else
	CCTexture2D * texture = App::getTexture("trava.png");
	vec2 size = getContentSize();
	float scale = App::getEntityScale();
	for(float x = 0; x < size.x; x += texture->getContentSize().width * scale){
		CCSprite * sprite = CCSprite::spriteWithTexture(texture);
		sprite->setAnchorPoint(vec2(0,0));
		sprite->setPosition(vec2(x,0));
		sprite->setScale(scale);
		trava->addChild(sprite);
	}
#endif
}

void MonsterHome::addCloud()
{
	ASSERT(clouds);
	vec2 size = getContentSize();
	
	CCTexture2D * textures[] = { App::getTexture("cloud-1.png"), App::getTexture("cloud-2.png") };
	CCSprite * sprite = CCSprite::spriteWithTexture(textures[App::getRand(ARR_SIZE(textures))]);
	sprite->setAnchorPoint(vec2(0.5f, 0.5f));
	sprite->setRotation((float)App::getRand(360));
	sprite->setFlipX(App::getRandFloat() < 0.5f);
	sprite->setScale(App::getEntityScale());
	sprite->setOpacity(MathLib::Clamp((int)((level_info.clouds_opacity + 0.05f * App::getRandSignFloat()) * 255), 0, 255));
	
	struct Lib {
		static void moveFinished(CCObject * target, void * user_param)
		{
			CCNode * node = dynamic_cast<CCNode*>((CCObject*)user_param);
			node->getParent()->removeChild(node, true);
		}
	};

	if(level_info.clouds_circular){
		float angle = (float)App::getRand(360);
		float y = App::getRandFloat(level_info.clouds_rel_pos[0], level_info.clouds_rel_pos[1]) * size.Min();
		// float y = size.Min()*0.5f*0.9f;

		// vec2 sprite_size = sprite->getContentSize();
		sprite->setScale(sprite->getScale() * 0.7f);
		sprite->setAnchorPoint(vec2(0.5f, 0.5f));
		sprite->setPosition(vec2(0, y));

		class RGBALayer: public Layer, public RGBAProtocolDummy
		{
		public:

			LAYER_NODE_FUNC(RGBALayer);

			virtual GLubyte getOpacity()
			{
				CCSprite * sprite = dynamic_cast<CCSprite*>(getChildren()->objectAtIndex(0));
				return sprite->getOpacity();
			}

			virtual void setOpacity(GLubyte opacity)
			{
				setOpacityAll(this, opacity, false);
			}
		};
		
		RGBALayer * layer = RGBALayer::node();
		layer->setAnchorPoint(vec2(0, 0));
		layer->setIsRelativeAnchorPoint(false);
		layer->setPosition(level_info.clouds_rel_center * size);
		layer->setRotation(angle);
		layer->addChild(sprite);
		clouds->addChild(layer);

		float move_time_sec = 120.0f * App::getRandFloat(0.5f, 1.0f);

		int opacity = sprite->getOpacity();
		layer->setOpacity(0);
		layer->runAction(CCSpawn::actions(
			CCFadeTo::actionWithDuration(2.0f, opacity),
			CCRotateBy::actionWithDuration(move_time_sec, App::getRandFloat(360.0f, 360.0f * 1.5f)), // * (App::getRand(2) ? 1.0f : -1.0f)),
			CCSequence::actions(
				CCDelayTime::actionWithDuration(move_time_sec * (App::getRandFloat() < 0.7f ? App::getRandFloat(0.4f, 0.8f) : 1.0f) - 2.0f),
				CCFadeTo::actionWithDuration(2.0f, 0),
				getFuncAction(Lib::moveFinished, layer),
				NULL
			),
			NULL
		));
	}else{
		// int pos_count = 5;
		float x = size.x * App::getRandFloat(0.1f, 0.9f); // (float)(App::getRand(pos_count)+1)/(pos_count+1) + size.x * 0.05f * App::getRandSignFloat();
		float y = App::getRandFloat(level_info.clouds_rel_pos[0], level_info.clouds_rel_pos[1]) * size.y;
		sprite->setPosition(vec2(x, y));

		clouds->addChild(sprite);

		float speed = (size.x / 60.f) * App::getRandFloat(0.6f, 1.4f);
		float dest_x = App::getRand(2) * size.x; // x > size.x/2 ? 0.0f : size.x;
		float move_time_sec = MathLib::Abs(x - dest_x) / speed;
		if(move_time_sec < 5.0f){
			move_time_sec = 5.0f;
		}

		int opacity = sprite->getOpacity();
		sprite->setOpacity(0);
		sprite->runAction(CCSpawn::actions(
			CCFadeTo::actionWithDuration(2.0f, opacity),
			CCMoveTo::actionWithDuration(move_time_sec, vec2(dest_x, y)),
			CCSequence::actions(
				CCDelayTime::actionWithDuration(move_time_sec * (move_time_sec > 20.0f && App::getRandFloat() < 0.3f ? App::getRandFloat(0.4f, 0.8f) : 1.0f) - 2.0f),
				CCFadeTo::actionWithDuration(2.0f, 0),
				getFuncAction(Lib::moveFinished, sprite),
				NULL
			),
			NULL
		));
	}
}

/*
void MonsterHome::createMenuClouds()
{
	ASSERT(clouds);
	clouds->removeAllChildrenWithCleanup(true);
	// clouds->setIsVisible(true);

	CCTexture2D * textures[] = { App::getTexture("cloud-1.png"), App::getTexture("cloud-2.png") };
	vec2 size = getContentSize();
	int count = 5;
	for(int i = 0; i < count; i++){
		CCSprite * sprite = CCSprite::spriteWithTexture(textures[App::getRand(ARR_SIZE(textures))]);
		sprite->setAnchorPoint(vec2(0.5f, 0.5f));
		sprite->setPosition(vec2(size.x * (float)(i+1)/(count+1) + size.x * 0.05f * App::getRandSignFloat(), size.y));
		sprite->setRotation((float)App::getRand(360));
		sprite->setFlipX(App::getRandFloat() < 0.5f);
		clouds->addChild(sprite);
	}
}
*/

bool MonsterHome::isLastGameLevel()
{
	return level == Settings::LEVELS_NUMBER-1 && App::settings()->getMode() == Settings::MODES_NUMBER-1;
}

bool MonsterHome::showGameFinishedWindow()
{
	return isLastGameLevel() && !App::settings()->isGameFinished();
}

void MonsterHome::loadLevel(int level)
{
	IW_CALLSTACK("MonsterHome::loadLevel");
	CC_SAFE_RELEASE_NULL(path);

	state = LOADING;

	if(level >= Settings::LEVELS_NUMBER && App::isAdmobVersion() && App::settings()->getMode()+1 == Settings::MODES_NUMBER-1){
		gotoMenuScene();
		return;
	}else if(level >= Settings::LEVELS_NUMBER && App::settings()->isModeEnabled(App::settings()->getMode()+1)){ // App::settings()->isModeJustUnlocked(App::settings()->getMode()+1)){
		App::settings()->setMode(App::settings()->getMode()+1);
		level = 0;
	}else if(level >= 0){
		level %= Settings::LEVELS_NUMBER;
	}

	String music_filename = App::settings()->getModeMusicFilename();
	if(App::settings()->isMusicEnabled() && (!App::isMusicPlaying() || App::getMusicFilename() != music_filename)){
		App::playMusic(music_filename);
	}else{
		App::setMusicFilename(music_filename);
	}

	stopAllActions();
	unscheduleAllSelectors();

	if(objective_window){
		removeChild(objective_window, true);
		CC_SAFE_RELEASE_NULL(objective_window);
	}
	/* if(menu_window){
		removeChild(menu_window, true);
		menu_window->release();
		menu_window = NULL;
	} */

	{
		unselectMonster();
		CC_SAFE_RELEASE_NULL(path);
		removeAllChildrenWithCleanup(true);

		hud_layer = NULL;
		hud_mode_icon = NULL;
		hud_level_time = NULL;
		hud_pause_button = NULL;
		hud_monsters_icon = NULL;
		hud_monsters_number = NULL;
		hud_monsters_number_req = NULL;
		hud_lives_icon = NULL;
		hud_lives_number = NULL;
		hud_missed_icon = NULL;
		hud_max_missed_monsters = NULL;
		hud_points_icon = NULL;
		hud_points_number = NULL;
		background_layer = NULL;
		path_layer = NULL;
		monsters = NULL;
		homes = NULL;
		trava = NULL;
		clouds = NULL;

		ambient_sound_action = NULL;
		ignore_fight_rects.clear();
	}

	scheduleUpdate();
	schedule(schedule_selector(MonsterHome::schedulerUpdate), 0.25f);

	bool restarted = false;
	if(this->level == level){ // restart
		points_number = start_level_points_number;
		restarted = true;
	}else{
		this->level = level;
		start_level_points_number = points_number;
	}

	initLevelInfo(level);
	// level_info.req_monsters = 1; // debug

	if(restarted){
		level_info.show_start_objective = false;
	}

	vec2 display_size = getContentSize();

	if(background_layer){
		removeChild(background_layer, true);
	}
	background_layer = Layer::node();
	background_layer->setContentSize(display_size);
	addChild(background_layer, BACKGROUND);

	{
		CCTexture2D * texture = App::getTexture(level_info.background_texture);
		Layer::tileRectWithSprites(background_layer, Rect2(vec2(0,0), display_size), texture, vec2(1,1) * App::getEntityScale());
	}

	/*
	if(!background || background_texture_name != level_info.background_texture){
		if(background){
			removeChild(background, true);
			background = NULL;
		}
		background_texture_name = level_info.background_texture;
		background = CCSprite::spriteWithFile(level_info.background_texture);
		vec2 map_size = background->getContentSize();
		background->setAnchorPoint(vec2(0.5f, 1.0f));
		background->setPosition(display_size * vec2(0.5f, 1.0f));
		background->setScaleX(MathLib::Max(1.0f, display_size.x / map_size.x));
		background->setScaleY(MathLib::Max(1.0f, display_size.y / map_size.y));
		background->setScale(MathLib::Max(background->getScaleX(), background->getScaleY()));
		addChild(background, BACKGROUND);
	}
	*/

	if(trava){
		removeChild(trava, true);
	}
	trava = Layer::node();
	addChild(trava, TRAVA);

	if(clouds){
		removeChild(clouds, true);
	}
	clouds = Layer::node();
	addChild(clouds, CLOUDS);

	top_padding = 0;
	bottom_padding = App::isAdmobVersion() ? (float)App::getAdmobDesc()->height : 0.0f;

	if(!hud_layer){
		if(hud_layer){
			removeChild(hud_layer, true);
		}
		hud_layer = Layer::node();
		addChild(hud_layer, HUD);
		hud_layer->setContentSize(hud_layer->getParent()->getContentSize());

		CCSprite * pause_normal_sprite = CCSprite::spriteWithTexture(App::getTexture("pause-icon.png"));
		CCSprite * pause_pressed_sprite = CCSprite::spriteWithTexture(App::getTexture("pause-icon-pressed.png"));		
		
		float pause_scale = 3.0f;
		Layer * pause_normal_node = Layer::node();
		pause_normal_node->setAnchorPoint(vec2(0,0));
		pause_normal_node->setContentSize(vec2(pause_normal_sprite->getContentSize()) * pause_scale);
		pause_normal_node->addChild(pause_normal_sprite);
		pause_normal_sprite->setAnchorPoint(vec2(-0.5f * (pause_scale-1.0f),-0.5f * (pause_scale-1.0f)));
		pause_normal_sprite->setPosition(vec2(0,0));

		Layer * pause_pressed_node = Layer::node();
		pause_pressed_node->setAnchorPoint(vec2(0,0));
		pause_pressed_node->setContentSize(vec2(pause_pressed_sprite->getContentSize()) * pause_scale);
		pause_pressed_node->addChild(pause_pressed_sprite);
		pause_pressed_sprite->setAnchorPoint(vec2(-0.5f * (pause_scale-1.0f),-0.5f * (pause_scale-1.0f)));
		pause_pressed_sprite->setPosition(vec2(0,0));

		CCMenuItem * hud_pause_button_item = CCMenuItemSprite::itemFromNormalSprite(
			pause_normal_node,
			pause_pressed_node,
			// NULL,
			this,
			menu_selector(MonsterHome::pauseButtonPressed));
		hud_pause_button_item->setScale(App::getEntityScale());

		hud_pause_button = CCMenu::menuWithItem(hud_pause_button_item);
		hud_pause_button->setContentSize( vec2(pause_pressed_sprite->getContentSize()) * App::getEntityScale() );
		// hud_pause_button->setAnchorPoint(vec2(-0.5f, 0.5f));
		// hud_pause_button->setIsRelativeAnchorPoint(true);
		hud_layer->addChild(hud_pause_button, 1);

		hud_monsters_icon = SmartSprite::create(App::getJSON("monsters-icon.txt"), App::getEntityScale());
		hud_monsters_icon->setAnchorPoint(vec2(0, 0.5f));
		hud_layer->addChild(hud_monsters_icon, 1);
		top_padding = MathLib::Max(hud_pause_button->getContentSize().height, hud_monsters_icon->getContentSize().height);

		hud_pause_button->setPosition( vec2(hud_layer->getContentSize()) - vec2(hud_pause_button->getContentSize().width * 0.55f, top_padding * 0.5f) );
		
		CCMenu * hud_buy_button = NULL;
		if(App::isAdmobVersion()){
			const char * font = App::getEntityScale() >= 1.0f || App::getEntityScale() < 0.61f ? App::getSmallFont() : App::getFont();
			CCMenuItemSprite * hud_buy_button_item = ItemsWindow::createLabelButton(App::getLangStr("BUY"), 
				App::getTexture("button-icon.png"), App::getTexture("button-icon-pressed.png"),
				vec3(1.0f,1.0f,0.8f), vec3(1,1,0), vec3(0,0,0), font, font, App::getEntityScale(),
				this, menu_selector(MonsterHome::buyButtonPressed));
			// hud_buy_button_item->setAnchorPoint(vec2(0.5f, 0.5f));
			
			hud_buy_button = CCMenu::menuWithItem(hud_buy_button_item);
			hud_buy_button->setContentSize(hud_buy_button_item->getContentSize());

			hud_layer->addChild(hud_buy_button, 1);

			float height = hud_buy_button->getContentSize().height;
			hud_buy_button->setPosition( vec2(hud_layer->getContentSize()) - vec2(hud_buy_button->getContentSize().width * 0.5f + height*0.02f, top_padding * 0.5f) );
			hud_pause_button->setPositionX(hud_pause_button->getPositionX() - hud_buy_button->getContentSize().width);
		}

		hud_monsters_icon->setPosition( vec2(0, hud_layer->getContentSize().height - top_padding * 0.5f) );

		// monsters_number = 0;
		float label_height = (float)MathLib::Round(MathLib::Min(top_padding * 0.6f, top_padding - 4.0f));
		vec2 shadow_offs = vec2(1, -1)*label_height*0.05f;

		// hud_monsters_number = CCLabelTTF::labelWithString("0", vec2(hud_layer->getContentSize().width, label_height), CCTextAlignmentLeft, "fonts/Arial.ttf", label_height);
		hud_monsters_number = SmartLabel::labelWithString("123", vec3(1,1,1), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_monsters_number->setAnchorPoint(vec2(0, 0.5f));
		hud_monsters_number->setPosition( hud_monsters_icon->getPosition() ); // vec2(hud_monsters_icon->getPosition()) + vec2(label_height * 0.5f + hud_monsters_icon->getContentSize().width, 0) );
		// hud_monsters_number->setColor(vec3(1,1,1));
		hud_layer->addChild(hud_monsters_number, 1);
		min_number_width = 0; // hud_monsters_number->getContentSize().width;

		hud_monsters_number_req = SmartLabel::labelWithString("", vec3(1,0,0), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_monsters_number_req->setAnchorPoint(vec2(0, 0.5f));
		hud_monsters_number_req->setPosition( hud_monsters_icon->getPosition() ); // vec2(hud_monsters_number->getPosition()) + vec2(label_height * 0.5f + hud_monsters_number->getContentSize().width, 0) );
		// hud_monsters_number_req->setColor(vec3(1,0,0));
		hud_layer->addChild(hud_monsters_number_req, 1);

		hud_lives_icon = SmartSprite::create(App::getJSON("lives-icon.txt"), App::getEntityScale());
		hud_lives_icon->setAnchorPoint(vec2(0, 0.5f));
		hud_lives_icon->setPosition( hud_monsters_icon->getPosition() );
		hud_layer->addChild(hud_lives_icon, 1);

		hud_lives_number = SmartLabel::labelWithString("5", vec3(1,1,1), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_lives_number->setAnchorPoint(vec2(0, 0.5f));
		hud_lives_number->setPosition( hud_monsters_icon->getPosition() ); // vec2(hud_lives_icon->getPosition()) + vec2(label_height * 0.5f + hud_lives_icon->getContentSize().width, 0) );
		// hud_lives_number->setColor(vec3(1,1,1));
		hud_layer->addChild(hud_lives_number, 1);

		hud_missed_icon = SmartSprite::create(App::getJSON("missed-icon.txt"), App::getEntityScale());
		hud_missed_icon->setAnchorPoint(vec2(0, 0.5f));
		hud_missed_icon->setPosition( hud_monsters_icon->getPosition() );
		hud_layer->addChild(hud_missed_icon, 1);

		hud_max_missed_monsters = SmartLabel::labelWithString("5", vec3(1,1,1), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_max_missed_monsters->setAnchorPoint(vec2(0, 0.5f));
		hud_max_missed_monsters->setPosition( hud_monsters_icon->getPosition() ); // vec2(hud_lives_icon->getPosition()) + vec2(label_height * 0.5f + hud_lives_icon->getContentSize().width, 0) );
		// hud_max_missed_monsters->setColor(vec3(1,1,1));
		hud_layer->addChild(hud_max_missed_monsters, 1);

		hud_points_icon = SmartSprite::create(App::getJSON("points-icon.txt"), App::getEntityScale());
		hud_points_icon->setAnchorPoint(vec2(0, 0.5f));
		hud_points_icon->setPosition( hud_monsters_icon->getPosition() );
		hud_layer->addChild(hud_points_icon, 1);

		hud_points_number = SmartLabel::labelWithString("5", vec3(1,1,1), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_points_number->setAnchorPoint(vec2(0, 0.5f));
		hud_points_number->setPosition( hud_monsters_icon->getPosition() ); // vec2(hud_lives_icon->getPosition()) + vec2(label_height * 0.5f + hud_lives_icon->getContentSize().width, 0) );
		// hud_points_number->setColor(vec3(1,1,1));
		hud_layer->addChild(hud_points_number, 1);

		hud_level_time = SmartLabel::labelWithString("", vec3(0.5f, 0.9f, 0.5f), vec3(0,0,0), App::getFont()); // shadow_offs, vec2(0, 0), CCTextAlignmentLeft, fontname, label_height);
		hud_level_time->setAnchorPoint(vec2(1, 0.5f));
		hud_level_time->setPosition( vec2(hud_pause_button->getPositionX() - hud_pause_button->getContentSize().width*0.5f - label_height*0.5f, hud_monsters_icon->getPosition().y) );
		// hud_level_time->setColor(vec3(0.5f, 0.9f, 0.5f));
		hud_layer->addChild(hud_level_time, 1);

		CCSprite * fill = CCSprite::spriteWithTexture(App::getTexture("hud-fill.png"));
		fill->setAnchorPoint(vec2(0, 1));
		fill->setPosition( vec2(0, hud_layer->getContentSize().height) );
		fill->setScaleX(hud_layer->getContentSize().width / fill->getContentSize().width);
		fill->setScaleY(hud_monsters_icon->getContentSize().height / fill->getContentSize().height);
		hud_layer->addChild(fill, 0);		
	}else{
		top_padding = hud_monsters_icon->getContentSize().height;
	}
	setMonstersNumber(0);

	if(hud_mode_icon){
		hud_layer->removeChild(hud_mode_icon, true);
	}
	hud_mode_icon = CCSprite::spriteWithFile(String::Format("mode-%d-icon.png", App::settings()->getMode()+1));
	hud_mode_icon->setAnchorPoint(vec2(0, 0.5f));
	hud_mode_icon->setPosition( vec2(0, hud_layer->getContentSize().height - top_padding * 0.5f) );
	// hud_mode_icon->setIsVisible(App::settings()->getMode() > 0);
	hud_mode_icon->setScale(App::getEntityScale());
	hud_layer->addChild(hud_mode_icon, 1);

	hud_lives_icon->setIsVisible(level_info.lives > 0);
	hud_lives_number->setIsVisible(level_info.lives > 0);
	setLivesNumber(level_info.lives); // MathLib::Max(lives_number, level_info.lives));

	hud_missed_icon->setIsVisible(level_info.max_missed_monsters > 0);
	hud_max_missed_monsters->setIsVisible(level_info.max_missed_monsters > 0);
	setMissedNumber(level_info.max_missed_monsters); // MathLib::Max(max_missed_monsters_number, level_info.max_missed_monsters));

	setLevelTime(0);

	hud_layer->setIsVisible(level >= 0);

	if(homes){
		removeChild(homes, true);
	}
	homes = Layer::node();
	addChild(homes, HOMES);
	homes->setContentSize(homes->getParent()->getContentSize());
	for(int i = 0; i < (int)level_info.rel_homes.size(); i++){
		char name[64];
		sprintf(name, "home-%02d.txt", (i % 2) + 1);
		Entity * home = Entity::create(this, Entity::HOLE, App::getJSON(name));
		home->index = i;
		homes->addChild(home);

		vec2 size = homes->getContentSize();
		home->setPosition(level_info.rel_homes[i] * size);
			
		const Rect2& rect = home->rect_cache.rect;
		float padding = rect.size.Max() * 0.5f;
		vec2 new_pos = home->getPosition();
		new_pos.x = MathLib::Clamp(new_pos.x, rect.size.x * 0.5f + padding, size.x - rect.size.x * 0.5f - padding);
		new_pos.y = MathLib::Clamp(new_pos.y, rect.size.y * 0.5f + MathLib::Max(padding, bottom_padding), size.y - rect.size.y * 0.5f - padding - top_padding);
		home->setPosition(new_pos);
	}

	if(monsters){
		removeChild(monsters, true);
	}
	monsters = Layer::node();
	addChild(monsters, MONSTERS);
	monsters->setContentSize(monsters->getParent()->getContentSize());

	struct Lib {
		static void drawPath(CCObject * target, void*)
		{
			MonsterHome * self = dynamic_cast<MonsterHome*>(target);
			self->drawPaths();
		}
	};
	if(path_layer){
		removeChild(path_layer, true);
	}
	path_layer = FuncDrawLayer::createWithTarget(this, Lib::drawPath);
	addChild(path_layer, PATH_SEGS);
	path_layer->setContentSize(path_layer->getParent()->getContentSize());

	if(App::isDummyAdmob()){
		struct Lib {
			static void drawAdmob(CCObject * target, void*)
			{
				App::drawAdmob();
			}
		};
		addChild(FuncDrawLayer::createWithTarget(this, Lib::drawAdmob), ADMOB);
	}

#if 0 // CC_SPRITE_DEBUG_DRAW == 1
	DrawRect * debug = DrawRect::node();
	debug->setContentSize(grid->getContentSize());
	grid->addChild(debug);
#endif

#if 0
	struct Lib2 {
		static void endGame(cocos2d::CCObject * target, void*)
		{
			App::director()->end();
		}
	};
	scheduleOnce(Lib2::endGame, 20.0f);
#endif

	if(level_info.trava){
		createTrava();
	}

	if(App::settings()->isSoundEnabled()){
		startAmbientSounds();
	}

	if(level < 0){
		// createMenuClouds();
		addMenuMonsters();

#if 1
		SmartLabel * version = SmartLabel::labelWithString(String("v")+App::getVersion(), vec3(0,0,0), vec3(0.5f,1,0.5f), App::getSmallFont());
		version->setAnchorPoint(vec2(1,0));
		version->setPosition(vec2(getContentSize().width - version->getContentSize().height*0.4f, version->getContentSize().height*0.3f));
		addChild(version, VERSION);
#else
		CCLabelBMFont * version = CCLabelBMFont::labelWithString(IW_APP_VERSION, App::getFont());
		version->setColor(vec3(0,0,0));
		version->setIsRelativeAnchorPoint(true);
		version->setAnchorPoint(vec2(1,0));
		version->setPosition(vec2(getContentSize().width - version->getContentSize().height, version->getContentSize().height));
		addChild(version, VERSION);
#endif
	}

	if(getIsRunning()){
		startLevel();
	}
}

void MonsterHome::startLevel()
{
	ASSERT(state == LOADING);
	ASSERT(/*!menu_window && */!objective_window);
	if(level < 0){
		addMenuMonsters();
	}
	struct Lib {
		static void start(cocos2d::CCObject * target, void*)
		{
			MonsterHome * game = dynamic_cast<MonsterHome*>(target);
			if(game->level < 0){
				MenuWindow * menu_window = MenuWindow::create(game);
				// menu_window->retain();
				game->addChild(menu_window, WINDOW);
			}else{
				game->objective_window = ObjectiveWindow::create(game);
				game->objective_window->retain();
				game->objective_window->setReplayVisible(false);
				// game->addChild(game->objective_window, WINDOW);
				game->showStartWindow();
				// game->addChild(LevelRecordWindow::create(game), WINDOW);
			}
		}
	};
	scheduleOnce(Lib::start, 0.1f);
}

void MonsterHome::startAmbientSounds()
{
	stopAmbientSounds();
	struct Lib {
		static void playAmbientSound(CCObject * target, void * is_start)
		{
			App::playEntitySound("game", "ambient");

			MonsterHome * game = dynamic_cast<MonsterHome*>(target);
			game->ambient_sound_action = game->scheduleOnce(Lib::playAmbientSound, App::getRandFloat(10.0f, 40.0f));
		}
	};
	ambient_sound_action = scheduleOnce(Lib::playAmbientSound, App::getRandFloat(5.0f, 10.0f));
}

void MonsterHome::stopAmbientSounds()
{
	if(ambient_sound_action){
		stopAction(ambient_sound_action);
		ambient_sound_action = NULL;
	}
}

void MonsterHome::checkMonsters()
{
	if(state != GAME && state != MENU){
		return;
	}
	IW_CALLSTACK("MonsterHome::checkMonsters");
	int max_count = level_info.max_soft_monsters;
	if(level_info.max_hard_monsters > max_count){
		CCObject* obj = NULL;
		CCARRAY_FOREACH(monsters->getChildren(), obj){
			Entity * ent = dynamic_cast<Entity*>(obj);
			if(ent && ent->path && ent->path->points.size() > 0 && ent->path->points[0].sprite->getIsVisible()){
				if(++max_count >= level_info.max_hard_monsters){
					break;
				}
			}
		}
	}
	for(int i = 0; i < 1 && (int)monsters->getChildrenCount() < max_count; i++){
		if(!addRandomMonster()){
			break;
		}
	}
	if(clouds && level_info.max_clouds > 0){
		for(int i = 0; i < 1 && (int)clouds->getChildrenCount() < level_info.max_clouds; i++){
			addCloud();
		}
	}
}

MonsterHome::LevelInfo::LevelInfo()
{
	show_start_objective = true;
	monster_speed_scale = 1.0f;
	max_clouds = 20;
	clouds_opacity = 0.6f;
	clouds_rel_pos[0] = 0.2f;
	clouds_rel_pos[1] = 1.0f;
	clouds_circular = false;
	max_soft_monsters = 5;
	max_hard_monsters = 10;
	max_missed_monsters = -1;
	lives = -1;
	req_monsters = 5;
	any_home = false;
	trava = false;
	attack_type = MONSTER_ATTACK_ANY;
}

MonsterHome::IgnoreFightRect::IgnoreFightRect()
{
	time_sec = 0;
}

MonsterHome::IgnoreFightRect::IgnoreFightRect(const Rect2& rect, float time_sec)
{
	this->rect = rect;
	this->time_sec = time_sec;
}

template <class T, class T2>
static void assignVector(T& vec, const T2 * map, int count)
{
	vec.clear();
	for(int i = 0; i < count; i++){
		vec.push_back(map[i]);
	}
}

template <class T, class T2>
static void assignVector(T& vec, const T2& p1)
{
	const T2 list[] = {p1};
	assignVector(vec, list, ARR_SIZE(list));
}

template <class T, class T2>
static void assignVector(T& vec, const T2& p1, const T2& p2)
{
	const T2 list[] = {p1, p2};
	assignVector(vec, list, ARR_SIZE(list));
}

template <class T, class T2>
static void assignVector(T& vec, const T2& p1, const T2& p2, const T2& p3)
{
	const T2 list[] = {p1, p2, p3};
	assignVector(vec, list, ARR_SIZE(list));
}

template <class T, class T2>
static void assignVector(T& vec, const T2& p1, const T2& p2, const T2& p3, const T2& p4)
{
	const T2 list[] = {p1, p2, p3, p4};
	assignVector(vec, list, ARR_SIZE(list));
}

template <class T, class T2>
static void assignVector(T& vec, const T2& p1, const T2& p2, const T2& p3, const T2& p4, const T2& p5)
{
	const T2 list[] = {p1, p2, p3, p4, p5};
	assignVector(vec, list, ARR_SIZE(list));
}

void MonsterHome::initLevelInfo(int level)
{
	initLevelInfo(level_info, level);
}

void MonsterHome::initLevelInfo(LevelInfo& level_info, int level, bool recursive)
{
	if(level < 0){
		// 10 - black
		int map[] = {5, 7, 8, 9, 10};
		// int map[] = {10};
		level_info.background_texture = String::Format("tiled-%02d.jpg", map[App::getRand(ARR_SIZE(map))]);
		level_info.rel_homes.clear();
		assignVector(level_info.active_monsters, 1, 2, 3, 7);
		level_info.any_home = true;
		level_info.trava = true;
		level_info.attack_type = MONSTER_ATTACK_ANY;
		level_info.max_soft_monsters = 10;
		level_info.max_hard_monsters = 12;
		level_info.max_missed_monsters = 0;
		level_info.lives = 0;
		level_info.req_monsters = 0;
		level_info.max_clouds = 20;
		level_info.clouds_opacity = 0.4f;
		level_info.clouds_rel_pos[0] = 0.95f;
		level_info.clouds_rel_pos[1] = 1.0f;
		level_info.show_start_objective = false;
		return;
	}

	// 5, 7
	// level_info.background_texture = "tiled-15.jpg"; // String::Format("level-%02d.jpg", 1); // level+1);
	// level_info.background_texture = String::Format("tiled-%02d.jpg", level+1);
	int tiled_map[] = {5, 7, 8, 11, 1, 6, 9, 12, 13, 14, 2, 3, 4, 15, 10};
	level_info.background_texture = String::Format("tiled-%02d.jpg", tiled_map[level % ARR_SIZE(tiled_map)]);
		
	int req_monsters[Settings::LEVELS_NUMBER] = {
		5, 15, 30, 45, 60, 
		75, 90, 105, 120, 135,
		150, 170, 190, 220, 250
	};
	int soft_monsters[Settings::LEVELS_NUMBER] = {
		2, 5, 5, 6, 6,
		7, 7, 8, 8, 9,
		9, 9, 10, 10, 12
	};

	level_info.objective_str = "";
	level_info.show_start_objective = false;

	float t = (float)level / (Settings::LEVELS_NUMBER-1);
	bool is_last_level = level+1 == Settings::LEVELS_NUMBER;
	int mode = App::settings()->getMode();
	switch(mode){
	case 0:
		assignVector(level_info.rel_homes, 
			vec2(0, 0),
			vec2(1, 0),
			vec2(1, 1),
			vec2(0, 1));

		assignVector(level_info.active_monsters, 3, 7);

		level_info.monster_speed_scale = MathLib::Lerp(0.8f, 1.9f, t); // is_last_level ? 1.1f : 1.0f + level;
		level_info.trava = false; // is_last_level;
		level_info.any_home = true;
		level_info.attack_type = MONSTER_ATTACK_DISABLED;
		level_info.lives = 0; // level / 5;
		level_info.max_missed_monsters = level >= 5 ? level*3 : 0;

		level_info.max_clouds = level >= 10 ? 10+level : 0;
		level_info.clouds_opacity = 0.4f;
		level_info.clouds_rel_pos[0] = 0.8f;
		level_info.clouds_rel_pos[1] = 1.0f;

		level_info.max_soft_monsters = soft_monsters[level];
		level_info.max_hard_monsters = level_info.max_soft_monsters*2;
		level_info.req_monsters = req_monsters[level];

		if(level == 0){
			level_info.objective_str = App::getLangStr("OBJECTIVE_START_INFO");
			level_info.show_start_objective = true;
		}
		break;

	case 1:
		assignVector(level_info.rel_homes, 
			// vec2(0, 0),
			vec2(1, 0),
			vec2(1, 1),
			vec2(0, 1));

		assignVector(level_info.active_monsters, 1, 3, 7);

		level_info.monster_speed_scale = MathLib::Lerp(1.0f, 1.2f, t); // is_last_level ? 1.2f : 1.1f;
		level_info.trava = level >= 10;
		level_info.any_home = false;
		level_info.attack_type = MONSTER_ATTACK_DIFFERENT;
		level_info.lives = (level+1) * 2;
		level_info.max_missed_monsters = level >= 5 ? 33 + level*33/5 : 0;
		
		level_info.max_clouds = level >= 5 ? 15+level : 0;
		level_info.clouds_opacity = 0.3f;
		level_info.clouds_rel_pos[0] = 0.6f;
		level_info.clouds_rel_pos[1] = 1.0f;

		level_info.max_soft_monsters = soft_monsters[level]*3/2;
		level_info.max_hard_monsters = level_info.max_soft_monsters*3/2 + level/5;
		level_info.req_monsters = req_monsters[level]*3/2;
		break;

	case 2:
		if(is_last_level){
			assignVector(level_info.rel_homes, 
				vec2(0.5f, 0.5f));
		}else if(level >= 10){
			assignVector(level_info.rel_homes, 
				// vec2(0, 0),
				// vec2(1, 0),
				vec2(1, 1),
				vec2(0, 1));
		}else if(level >= 10){
			assignVector(level_info.rel_homes, 
				// vec2(0, 0),
				vec2(1, 0),
				vec2(1, 1),
				vec2(0, 1));
		}else{
			assignVector(level_info.rel_homes, 
				vec2(0, 0),
				vec2(1, 0),
				vec2(1, 1),
				vec2(0, 1));
		}

		assignVector(level_info.active_monsters, 1, 2, 3, 7);

		level_info.monster_speed_scale = MathLib::Lerp(0.9f, 0.7f, t); // is_last_level ? 1.3f : 1.2f;
		level_info.trava = !is_last_level;
		level_info.any_home = false;
		level_info.attack_type = MONSTER_ATTACK_ANY;
		level_info.lives = is_last_level ? 2 : (level+1) * 3;
		level_info.max_missed_monsters = is_last_level ? 2 : 10 + level*2;
		
		level_info.max_clouds = 20 + level;
		level_info.clouds_opacity = 0.3f; // MathLib::Lerp(0.3f, 0.6f, (level+1.0f)/Settings::LEVELS_NUMBER);
		level_info.clouds_rel_pos[0] = is_last_level ? 0.45f : 0.25f;
		level_info.clouds_rel_pos[1] = is_last_level ? 0.55f : 1.0f;
		level_info.clouds_circular = is_last_level;
		level_info.clouds_rel_center = vec2(0.5f, 0.5f);

		level_info.max_soft_monsters = soft_monsters[level]*2;
		level_info.max_hard_monsters = level_info.max_soft_monsters*2; // (int)(level_info.max_soft_monsters*1.2 + 0.5); // + level/3;
		level_info.req_monsters = (int)MathLib::EpsRound(MathLib::Pow((float)req_monsters[level], 0.96f), 5.0f);

		if(is_last_level){
			level_info.objective_str = App::getLangStr("OBJECTIVE_END_INFO");
			level_info.show_start_objective = true;
		}
		break;
	}
	// level_info.req_monsters = 1; // debug
	if(recursive){
		return;
	}
	if(level_info.objective_str == ""){
		LevelInfo temp_level_info;
		if(level > 0){
			initLevelInfo(temp_level_info, level-1, true);
		}else{
			temp_level_info = level_info;
		}

		if(level > 0 && level_info.rel_homes.size() != temp_level_info.rel_homes.size()){
			level_info.objective_str += " " + App::getLangStr("OBJECTIVE_HOMES_CHNAGED_INFO");
			level_info.show_start_objective = true;
		}

		level_info.objective_str += " " + App::getLangStr("OBJECTIVE_GENERIC_INFO");
		if(!level_info.any_home){
			level_info.objective_str += " " + App::getLangStr("OBJECTIVE_OWN_HOME_INFO");
			if(!level_info.show_start_objective){
				level_info.show_start_objective = !level || temp_level_info.any_home;
			}
		}
		String additional_info;
		if(level_info.lives > 0){
			additional_info += " " + App::getLangStr("OBJECTIVE_LIVES_INFO");
			if(!level_info.show_start_objective){
				level_info.show_start_objective = !level || temp_level_info.lives <= 0;
			}
		}
		if(level_info.max_missed_monsters > 0){
			additional_info += " " + App::getLangStr("OBJECTIVE_MISSED_INFO");
			if(!level_info.show_start_objective){
				level_info.show_start_objective = !level || temp_level_info.max_missed_monsters <= 0;
			}
		}
		if(level_info.max_clouds > 0 && (!level || temp_level_info.max_clouds <= 0)){
			additional_info += " " + App::getLangStr("OBJECTIVE_CLOUNDS_INFO");
			if(!level_info.show_start_objective){
				level_info.show_start_objective = !level || temp_level_info.max_clouds <= 0;
			}
		}
		additional_info = additional_info.Trim();
		if(additional_info != ""){
			level_info.objective_str += " " + additional_info;
		}		
	}
	level_info.objective_str = level_info.objective_str.Replace("{req_monsters}", App::caseNumber(level_info.req_monsters, "MONSTER"));
	level_info.objective_str = level_info.objective_str.Replace("{lives}", App::caseNumber(level_info.lives, "LIVE"));
	level_info.objective_str = level_info.objective_str.Replace("{max_missed_monsters}", App::caseNumber(level_info.max_missed_monsters, "MONSTER"));
	level_info.objective_str = level_info.objective_str.Trim();
}

void MonsterHome::addMenuMonsters()
{
	ASSERT(level < 0);
#if 0
	addRandomMonster();
#else
	int max_count = level_info.max_soft_monsters;
	while((int)monsters->getChildrenCount() < max_count){
		if(!addRandomMonster()){
			break;
		}
	}
#endif
}

MonsterHome::Entity * MonsterHome::addRandomMonster()
{
	IW_CALLSTACK("MonsterHome::addRandomMonster");

	char name[64];
	int index = App::getRand(level_info.active_monsters.size());
	sprintf(name, "monster-%02d.txt", level_info.active_monsters[index]);
	JSON * config = App::getJSON(name);
	Entity * monster = Entity::create(this, Entity::MONSTER, config);
	monster->index = index;
	monsters->addChild(monster);
	
	vec2 points[2];
	if(state != LOADING){
		if(App::getRandFloat() < 0.5f){
			float y = state == MENU ? App::getRandFloat(0.1f, 0.9f) : App::getRandFloat(App::isAdmobVersion() ? 0.42f : 0.3f, 0.6f);
			points[0] = vec2(-0.1f, y + 0.05f * App::getRandSignFloat());
			points[1] = vec2(1.1f, y + 0.05f * App::getRandSignFloat());
		}else{
			float x = state == MENU ? App::getRandFloat(0.1f, 0.9f) : App::getRandFloat(0.3f, 0.7f);
			points[0] = vec2(x + 0.05f * App::getRandSignFloat(), -0.1f);
			points[1] = vec2(x + 0.05f * App::getRandSignFloat(), 1.1f);
		}
	
		Bounds2 bounds;
		vec2 map_size = monsters->getContentSize();
		if(state != MENU){
			vec2 half_size = vec2(monster->getContentSize()); // * 0.5f;
			bounds.Init(monsters->convertToWorldSpace(points[0] * map_size - half_size));
			bounds.Add(monsters->convertToWorldSpace(points[1] * map_size + half_size));
			// bounds.Expand(half_size.Len() * 0.5f);
		}

		if(App::getRandFloat() < 0.5f){
			vec2 temp = points[0];
			points[0] = points[1];
			points[1] = temp;
		}
		monster->setPosition(points[0] * map_size);

		if(state != MENU){
			Rect2 rect(bounds.b[0], bounds.b[1] - bounds.b[0] + vec2(1, 1));
			float rel_size = 0.2f;
			if(rect.size.x > rect.size.y){
				rect.size.x *= rel_size;
				if(points[0].x > points[1].x){
					rect.origin.x += rect.size.x * (1.0f - rel_size);
				}
			}else{
				rect.size.y *= rel_size;
				if(points[0].y > points[1].y){
					rect.origin.y += rect.size.y * (1.0f - rel_size);
				}
			}
			CCMutableArray<CCNode*> * found = findEntitiesAtRect(monsters, rect);
			if(found && found->count()){
				int count = found->count();
				for(int i = 0; i < count; i++){
					Entity * ent = dynamic_cast<Entity*>(found->getObjectAtIndex(i));
					if(ent && ent != monster){
						monsters->removeChild(monster, true);
						found->removeAllObjects();
						return NULL;
					}
				}
			}
		}
	}else{
		// vec2 size = getContentSize();
		points[0] = vec2(App::getRandFloat(), App::getRandFloat());
		points[1] = points[0] + vec2(App::getRandSignFloat(), App::getRandSignFloat()).Norm() * 0.05f;
	}

	EntityPath * path = new EntityPath();
	path->AddPoint(points[0]);
	path->AddPoint(points[1]);
	monster->setPath(path);
	monster->startPathMove(); // monsters->getChildrenCount() > 1 ? 3.0f : 0.0f);
	return monster;
}

MonsterHome::Entity * MonsterHome::findEntityAtPoint(Layer * layer, const vec2& p)
{
	IW_CALLSTACK("MonsterHome::findEntityAtPoint");
	struct Data
	{
		Entity * best_ent;
		float best_dist;

		static bool check(CCNode * node, const vec2& local, void * user_data)
		{
			IW_CALLSTACK("MonsterHome::findEntityAtPoint::check");
			Entity * ent = dynamic_cast<Entity*>(node);
			if(ent && ent->isResponsible()){
				float dist = ent->getDistToLocalPoint(local);
				if(dist < vec2(ent->getContentSize()).Max()){
					Data * data = (Data*)user_data;
					if(!data->best_ent || data->best_dist > dist){
						data->best_ent = ent;
						data->best_dist = dist;
					}
				}
			}
			return false;
		}
	} data = {NULL, 0.0f};
	App::collectNodesAtPoint(&touch_cache, layer, 0, layer->getContentSize().width*0.1f, p, Data::check, &data);
	return data.best_ent;
}

MonsterHome::Entity * MonsterHome::findMonsterAtPoint(const vec2& p)
{
	return findEntityAtPoint(monsters, p);
}

CCMutableArray<CCNode*> * MonsterHome::findEntitiesAtRect(Layer * layer, const Rect2& rect)
{
	IW_CALLSTACK("MonsterHome::findEntitiesAtRect");
	struct Lib {
		static bool check(CCNode * node, const Rect2& local_rect, void*)
		{
			IW_CALLSTACK("MonsterHome::findEntitiesAtRect::check");
			Entity * ent = dynamic_cast<Entity*>(node);
			if(ent){
				return ent->checkTouchAtLocalRect(local_rect);
			}
			return false;
		}
	};
	return App::collectNodesAtRect(&touch_cache, layer, 0, 0, rect, Lib::check);
}

void MonsterHome::onEntityRemoved(Entity * ent)
{
	IW_CALLSTACK("MonsterHome::onEntityRemoved");
	if(selected_monster == ent){
		selected_monster = NULL;
		if(path){
			path->release();
			path = NULL;
		}
	}
}

void MonsterHome::updateHudItemsPos()
{
	struct Item {
		SmartSprite * icon;
		SmartLabel * number;
	};
	Item items[] = {
		{hud_points_icon, hud_points_number},
		{hud_monsters_icon, hud_monsters_number},
		{hud_lives_icon, hud_lives_number},
		{hud_missed_icon, hud_max_missed_monsters},
	};

	float padding = hud_monsters_number->getContentSize().height * 0.5f;
	float x = 0;

	if(hud_mode_icon && hud_mode_icon->getIsVisible()){
		// x = padding/2;
		if(x != hud_mode_icon->getPositionX()){
			hud_mode_icon->setPositionX(x);
		}
		x += hud_mode_icon->getContentSize().width * hud_mode_icon->getScaleX();
		x += padding * 0.5f;
	}

	for(int i = 0; i < ARR_SIZE(items); i++){
		SmartSprite * icon = items[i].icon;
		SmartLabel * number = items[i].number;
		if(icon->getIsVisible()){
			x = MathLib::Max(x, icon->getPositionX());
			if(icon->getPositionX() != x){
				icon->setPositionX(x);
			}
			x += icon->getContentSize().width + padding;
		
			if(number->getPositionX() != x){
				number->setPositionX(x);
			}
			x += number->getContentSize().width + padding;

			if(number == hud_monsters_number && hud_monsters_number_req->getIsVisible()){
				if(hud_monsters_number_req->getPositionX() != x){
					hud_monsters_number_req->setPositionX(x);
				}
				x += hud_monsters_number_req->getContentSize().width + padding;
			}

			x = MathLib::Max(x, number->getPosition().x + min_number_width + padding);
		}	
	}
}

void MonsterHome::setMonstersNumber(int value)
{
	points_number = MathLib::Max(points_number, points_number + value - monsters_number);
	bool points_visible = false; // points_number != value;
	if(points_visible){
		hud_points_number->setString(XString(points_number));
	}
	if(points_visible != hud_points_number->getIsVisible()){
		hud_points_icon->setIsVisible(points_visible);
		hud_points_number->setIsVisible(points_visible);
	}

	monsters_number = value;
	hud_monsters_number->setString(XString(monsters_number));

	bool monsters_req_visible = false;
	if(level_info.req_monsters > 0 && monsters_number < level_info.req_monsters){
		vec2 monsters_label_size = hud_monsters_number->getContentSize();
		// hud_monsters_number_req->setPosition( vec2(hud_monsters_number->getPosition()) + vec2(monsters_label_size.x + monsters_label_size.y*0.5f, 0) );
		hud_monsters_number_req->setString(String::Format("(%d)", level_info.req_monsters - monsters_number));
		monsters_req_visible = true;
	}
	if(monsters_req_visible != hud_monsters_number_req->getIsVisible()){
		hud_monsters_number_req->setIsVisible(monsters_req_visible);
	}

	hud_monsters_icon->runAnimation("idle", false, 3, false);
	updateHudItemsPos();
}

void MonsterHome::setLivesNumber(int value)
{
	lives_number = value;
	hud_lives_number->setString(XString(lives_number));
	hud_lives_icon->runAnimation("idle", false, 3, false);
	updateHudItemsPos();
}

void MonsterHome::setMissedNumber(int value)
{
	max_missed_monsters_number = value;
	hud_max_missed_monsters->setString(XString(max_missed_monsters_number));
	hud_missed_icon->runAnimation("idle", false, 3, false);
	updateHudItemsPos();
}

void MonsterHome::onMonsterAtHome(Entity * ent)
{
	ASSERT(ent->type == Entity::MONSTER);
	if(state == GAME){
		setMonstersNumber(monsters_number + 1);
	}
}

void MonsterHome::onMonsterDead(Entity * ent)
{
	ASSERT(ent->type == Entity::MONSTER);
	if(state == GAME && level_info.lives > 0 && lives_number > 0){
		setLivesNumber(lives_number - 1);
	}
}

void MonsterHome::onMonsterMissed(Entity * ent)
{
	ASSERT(ent->type == Entity::MONSTER);
	if(state == GAME && level_info.max_missed_monsters > 0 && max_missed_monsters_number > 0){
		setMissedNumber(max_missed_monsters_number - 1);

		if(App::settings()->isSoundEnabled()){
			App::playEntitySound("hud", "missed");
		}
	}
}

void MonsterHome::playHudClickSound()
{
	if(App::settings()->isSoundEnabled()){
		App::playEntitySound("hud", "click");
	}
}

bool MonsterHome::isMonsterHome(Entity * monster, Entity * home)
{
	if(level_info.any_home){
		return true;
	}
	int count = homes->getChildrenCount();
	return count > 0 && (monster->index % count) == home->index;
}

void MonsterHome::selectMonster(Entity * monster)
{
	IW_CALLSTACK("MonsterHome::selectMonster");
	if(selected_monster != monster){
		unselectMonster();
		selected_monster = monster;
		if(selected_monster){
			selected_monster->pauseMove();
		}
	}
}

void MonsterHome::unselectMonster()
{
	IW_CALLSTACK("MonsterHome::unselectMonster");
	if(selected_monster){
		selected_monster->unpauseMove();
		selected_monster = NULL;
	}
}

void MonsterHome::drawPaths()
{
	App::drawAdmob();

#ifdef IW_DEBUG
	IW_CALLSTACK("MonsterHome::drawPaths");

	if(ignore_fight_rects.size() > 0){
		glColor4f(0, 0, 1, 1.0f);
		glLineWidth(2.0);
		for(int i = 0; i < (int)ignore_fight_rects.size(); i++){
			const Rect2& rect = ignore_fight_rects[i].rect;

			CCPoint vertices[4] = {
				ccp(rect.origin.x, rect.origin.y), 
				ccp(rect.origin.x + rect.size.x, rect.origin.y),
				ccp(rect.origin.x + rect.size.x, rect.origin.y + rect.size.y), 
				ccp(rect.origin.x, rect.origin.y + rect.size.y)
			};
			ccDrawPoly(vertices, 4, true, false);
		}
		glColor4f(0, 0, 0, 1.0f);
		glLineWidth(1.0);
	}

	if(path){
		drawPath(path);
	}
	CCObject* obj = NULL;
	CCARRAY_FOREACH(monsters->getChildren(), obj){
		Entity * ent = dynamic_cast<Entity*>(obj);
		if(ent){
			drawPath(ent->getPath());
		}
	}		
#endif
}

void MonsterHome::drawPath(EntityPath * path)
{
#if 0
	IW_CALLSTACK("MonsterHome::drawPath");
	vec2 size = path_layer->getContentSize();

	glColor4f(1.0, 0, 0, 1);
	glLineWidth(1.0);
	glPointSize(3.0);
	// ccDrawLine(ccp(0, 0), ccp(size.x*0.5f, size.y));

	int count = path->spline.GetNumValues();
#if 1
	float end_time = path->spline.GetTime(count-1);
	vec2 prev = path->spline.GetCurrentValue(0);
	// float dt = 0.01f;
	float dt = 1.0f / (float)(count * 4);
	for(float t = dt; t <= end_time; t += dt){
		vec2 p = path->spline.GetCurrentValue(t);
		ccDrawLine(prev * size, p * size);
		prev = p;
	}
#endif
	CCPoint * points = new CCPoint[count];
	for(int i = 0; i < count; i++){
		vec2 p = path->spline.GetValue(i);
		points[i] = p * size;

		if(0 && i > 0){
			ccDrawLine(points[i-1], points[i]);
		}
	}
	ccDrawPoints(points, count);
	delete [] points;
#endif
}

// default implements are used to call script callback if exist
bool MonsterHome::ccTouchBegan(CCTouch * touch, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchBegan");
	// CCAssert(!selected_monster, "");
	if(0 && !super::ccTouchBegan(touch, event)){
		return false;
	}
	unselectMonster();

	if(state != GAME && state != MENU){
		return false;
	}

	vec2 p = App::getTouchPoint(touch);

	{
		vec2 size = monsters->getContentSize();
		float used_size = size.Max()*0.2f;
		vec2 rect_size = vec2(used_size, used_size);
		Rect2 rect(vec2(monsters->convertToNodeSpace(p)) - rect_size*0.5f, rect_size);

		bool fight_found = false;
		CCObject* obj = NULL;
		CCARRAY_FOREACH(monsters->getChildren(), obj){
			Entity * ent = dynamic_cast<Entity*>(obj);
			if(ent && ent->isInFight() && ent->rect_cache.rect.IntersectRect(rect)){
				ent->stopFight();
				fight_found = true;
			}
		}
		if(fight_found){
			if(ignore_fight_rects.size() >= 5){
				ignore_fight_rects.erase(ignore_fight_rects.begin(), ignore_fight_rects.begin()+1);
			}
			ignore_fight_rects.push_back(IgnoreFightRect(rect, App::getTimeSec()));
		}
	}

	Entity * ent = findMonsterAtPoint(p);
	if(ent){
		selectMonster(ent);

		if(path){
			path->resetPathPoints();
			path->release();
		}
		path = new EntityPath();
		path->retain();
		vec2 size = path_layer->getContentSize();
		vec2 path_point = vec2(path_layer->convertToNodeSpace(p)) / size;
		path->AddPoint(vec2(ent->getPosition()) / size);
		path->AddPoint(path_point);

		return true;
	}

	return false;
}

void MonsterHome::ccTouchMoved(CCTouch * touch, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchMoved");
	// super::ccTouchMoved(touch, event);

	if(state != GAME && state != MENU){
		return;
	}

	if(selected_monster && path){
		vec2 p = path_layer->convertToNodeSpace(App::getTouchPoint(touch));
		if(selected_monster->rect_cache.rect.ContainsPoint(p)){
			EntityPath * new_path = new EntityPath();
			new_path->AddPoint(path->spline.GetValue(0));
			path->resetPathPoints();
			path->release();
			path = new_path;
			path->retain();
		}
		vec2 size = path_layer->getContentSize();
		vec2 path_point = p / size;
		path->AddPoint(path_point, 0.03f);
		path->createPathPoints(path_layer, true);

		if(path->getTimeLen() > 5.0f){
			ccTouchEnded(touch, event);
		}
	}	
}

void MonsterHome::ccTouchEnded(CCTouch * touch, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchEnded");
	// super::ccTouchEnded(touch, event);

	if(state != GAME && state != MENU){
		return;
	}

	if(selected_monster && path){
		vec2 p = App::getTouchPoint(touch);	
		vec2 size = path_layer->getContentSize();
		vec2 path_point = vec2(path_layer->convertToNodeSpace(p)) / size;
		path->AddPoint(path_point, 0.03f);

		if(path->getTimeLen() > 0.07f){
			selected_monster->setPath(path, true);
		}
		unselectMonster();
	}
	if(path){
		path->release();
		path = NULL;
	}
}

void MonsterHome::ccTouchCancelled(CCTouch * touch, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchCancelled");
	unselectMonster();
	if(path){
		path->release();
		path = NULL;
	}
}

// default implements are used to call script callback if exist
void MonsterHome::ccTouchesBegan(CCSet *touches, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchesBegan");
	super::ccTouchesBegan(touches, event);
	CCSetIterator it = touches->begin(), end = touches->end();
	for(; it != end; ++it){
		CCTouch * touch = (CCTouch*)(*it);
		ccTouchBegan(touch, event);
	}
}

void MonsterHome::ccTouchesMoved(CCSet *touches, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchesMoved");
	super::ccTouchesMoved(touches, event);
	CCSetIterator it = touches->begin(), end = touches->end();
	for(; it != end; ++it){
		CCTouch * touch = (CCTouch*)(*it);
		ccTouchMoved(touch, event);
	}
}

void MonsterHome::ccTouchesEnded(CCSet *touches, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchesEnded");
	super::ccTouchesEnded(touches, event);
	CCSetIterator it = touches->begin(), end = touches->end();
	for(; it != end; ++it){
		CCTouch * touch = (CCTouch*)(*it);
		ccTouchEnded(touch, event);
	}
}

void MonsterHome::ccTouchesCancelled(CCSet *touches, CCEvent * event)
{
	IW_CALLSTACK("MonsterHome::ccTouchesCancelled");
	super::ccTouchesCancelled(touches, event);
	CCSetIterator it = touches->begin(), end = touches->end();
	for(; it != end; ++it){
		CCTouch * touch = (CCTouch*)(*it);
		ccTouchCancelled(touch, event);
	}
}

#define USE_SCENE_TRANSITION

void MonsterHome::gotoMenuScene()
{
#ifdef USE_SCENE_TRANSITION
	CCScene * scene = MonsterHome::scene(-1);
	scene = CCTransitionFadeTR::transitionWithDuration(1.0, scene); 
	// CCEaseElasticInOut
	App::director()->replaceScene(scene);
#else
	loadLevel(-1);
#endif
}

void MonsterHome::gotoLevelScene(int level)
{
#ifdef USE_SCENE_TRANSITION
	CCScene * scene = MonsterHome::scene(level);
	scene = CCTransitionFadeBL::transitionWithDuration(1.0, scene); 
	// transition = CCEaseElasticInOut::actionWithAction(transition);
	App::director()->replaceScene(scene);
#else
	loadLevel(level);
#endif
}

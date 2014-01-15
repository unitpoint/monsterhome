#ifndef __LEVEL_SCENE_H__
#define __LEVEL_SCENE_H__

#include <App.h>
#include <Layer.h>
#include <JSON.h>
#include <PathMoveAction.h>

#define ENT_TAG_MONSTER 1

struct EntityInfo
{
	struct Animation: public CCObject
	{
		CCAnimation * animation;

		float scale;
		vec2 actor_rect_center;
		vec2 actor_rect_size;
		bool actor_rect_populated;

		vec2 anchor;
		bool anchor_populated;

		Animation();
		~Animation();

		const char * getName(){ return animation->getName(); }
		operator CCAnimation*(){ return animation; }
	};

	String name;
	CCDictionary<String, Animation*> animations;

	float health;
	float speed;

	void initWithJSON(const char * name, JSON * config);

	Animation * getAnimation(const char * name);
	void setAnimation(const char * name, Animation*);
};

class SmartSprite: public Layer
{
	typedef Layer super;

protected:

	EntityInfo info;

	bool flip_x;
	CCSprite * sprite;
	EntityInfo::Animation * sprite_animation;

	EntityInfo::Animation * cur_animation;
	CCActionInterval * cur_animate;
	CCActionInterval * change_animation_timeout;

	SmartSprite();
	~SmartSprite();

public:

	LAYER_NODE_FUNC(SmartSprite);

	virtual bool init();

	static SmartSprite * create(JSON*, float scale = 1.0f);
	bool initWithConfig(JSON*, float scale = 1.0f);

	CCSprite * getSprite(){ return sprite; }
	virtual void initSpriteByAnimation(EntityInfo::Animation * = NULL);

	virtual bool isFlipX();
	virtual void setFlipX(bool);

	EntityInfo::Animation * getAnimation(const char * name){ return info.getAnimation(name); }
	// void setAnimation(const char * name, CCAnimation * animation){ info.setAnimation(name, animation); }

	virtual void runAnimation(EntityInfo::Animation*, bool unsync = true, int repeat = -1, bool check_timeout = true, SEL_CallFuncUP finished = NULL, void * user_param = NULL);
	virtual void runAnimation(const char * name, bool unsync = true, int repeat = -1, bool check_timeout = true, SEL_CallFuncUP finished = NULL, void * user_param = NULL);
	virtual void stopAnimation();

	virtual void updateRectCache();
	virtual void updateWarningSprite();
};

class EntityPath: public PathObject
{
public:

	struct Point
	{
		float time;
		CCSprite * sprite;

		Point(){ time = 0; sprite = NULL; }
		Point(float p_time, CCSprite * p_sprite)
		{
			time = p_time;
			sprite = p_sprite;
			if(sprite){
				sprite->retain();
			}
		}
		Point(const Point& b)
		{
			time = b.time;
			sprite = b.sprite;
			if(sprite){
				sprite->retain();
			}
		}
		~Point()
		{
			if(sprite){
				sprite->release();
			}
		}
		Point& operator=(const Point& b)
		{
			time = b.time;
			if(sprite){
				sprite->release();
			}
			sprite = b.sprite;
			if(sprite){
				sprite->retain();
			}
			return *this;
		}
	};

	std::vector<Point> points;

	EntityPath(PathObject * origin = NULL, float start_time = 0.0f): PathObject(origin, start_time){}

	virtual ~EntityPath();

	float getTimeLen()
	{
		int count = spline.GetNumValues();
		return count > 0 ? spline.GetTime(count-1) : 0.0f;
	}

	void setPathVisible(bool);
	void createPathPoints(Layer * path_layer, bool path_visible, bool start_from_begin = false);
	void resetPathPoints();
};

class SplashScreen: public Layer
{
	typedef Layer super;

protected:

	class LoadStep: public CCObject
	{
	public:

		float weight;

		LoadStep(float weight = 1.0f);
		virtual void step() = 0;
	};

	class LoadResourceStep: public LoadStep
	{
	public:

		String filename;

		LoadResourceStep(const char * filename, float weight = 1.0f);
	};

	class LoadImageStep: public LoadResourceStep
	{
	public:
		LoadImageStep(const char * filename, float weight = 1.0f): LoadResourceStep(filename, weight){}
		virtual void step();
	};

	class LoadJSONStep: public LoadResourceStep
	{
	public:
		LoadJSONStep(const char * filename, float weight = 1.0f): LoadResourceStep(filename, weight){}
		virtual void step();
	};

	class LoadSoundStep: public LoadResourceStep
	{
	public:
		LoadSoundStep(const char * filename, float weight = 1.0f): LoadResourceStep(filename, weight){}
		virtual void step();
	};

	class LoadMusicStep: public LoadResourceStep
	{
	public:
		LoadMusicStep(const char * filename, float weight = 1.0f): LoadResourceStep(filename, weight){}
		virtual void step();
	};

	CCMutableArray<LoadStep*> steps;
	float sum_weight;
	int cur_step;
	float cur_weight;
	float waiting_time_sec;

	CCSprite * logo;
	CCSprite * progressbar_border;
	CCProgressTimer * progress;

	SplashScreen();

	void showGameDraftLogo();
	void showGameLogo();

	void startLoading();
	void schedulerLoadStep(ccTime);
	void schedulerWaitLogoMusic(ccTime);

public:

	LAYER_NODE_FUNC(SplashScreen);

	static CCScene * scene();

	virtual bool init();
	virtual void draw();
};

class MonsterHome: public Layer
{
	typedef Layer super;

	CC_SYNTHESIZE_READONLY(int, level, Level);

public:

	class Entity: public SmartSprite
	{
		typedef SmartSprite super;

		friend class MonsterHome;

	public:

		enum EType {
			MONSTER,
			HOLE,
			OBJECT,
		};

		void onAllActionsRemoved();

	protected:

		enum EState {
			NORMAL,
			// FIGHT,
			DYING,
			DEAD,
			HOME_MOVE,
			HOME_INSIDE_MOVE,
			BEFORE_FREE,
			FREE,
		};

		MonsterHome * game;

		EType type;
		EState state;

		// EntityInfo info;
		float damaged;

		// bool flip_x;
		// CCSprite * sprite;
		// EntityInfo::Animation * sprite_animation;

		CCSprite * warning_sprite;
		CCActionInterval * warning_action;

		EntityPath * path;
		PathMoveAction * path_action;
		CCFiniteTimeAction * path_sequence_action;
		float path_time;

		// EntityInfo::Animation * cur_animation;
		// CCActionInterval * cur_animate;
		// CCActionInterval * change_animation_timeout;

		vec2 home_pos;

		// CCDictionary<String, Entity*> linked_entities;
		Entity * owner;
		Entity * decal;

		Entity * fight_with;
		CCActionInterval * ignore_fight;
		// CCFiniteTimeAction * fight_sequence_action;

	public:

		struct RectCache
		{
			Rect2 rect;
			Rect2 expaned;
		} rect_cache;

		int index; // home index or monster type index

		LAYER_NODE_FUNC(Entity);

		Entity();
		virtual ~Entity();

		virtual void draw();

		void setPathVisible(bool visible);
		void createPathPoints(bool path_visible);
		void resetPathPoints();

		virtual void updateRectCache();
		virtual void updateWarningSprite();
		
		void startWarning();

		void fight(Entity*);
		void startFightWith(Entity*, const vec2& pos);
		void stopFight(bool start_ignore_timeout = true);
		
		void startIgnoreFight(float time = 10.0f);
		void stopIgnoreFight();

		bool isDead();
		bool isHomePos();
		// bool isAlive();
		bool isResponsible();
		bool isInFight();

		bool checkDie();
		void die();

		static Entity * create(MonsterHome*, EType, JSON*);
		bool initWithName(MonsterHome*, EType, JSON*);

		EntityPath * getPath(){ return path; }
		void setPath(EntityPath * value, bool path_visible = false);

		PathMoveAction * createPathMoveAction(EntityPath*, float start_time);
		void startPathMove(float delay = 0.0f);
		void stopPathMove();

		void startHomeMove(Entity * target);
		void startHomeInside();

		// EntityInfo::Animation * getAnimation(const char * name){ return info.getAnimation(name); }

		// void runAnimation(EntityInfo::Animation*, bool unsync = true, int repeat = -1, bool check_timeout = true, SEL_CallFuncUP finished = NULL, void * user_param = NULL);
		// void runAnimation(const char * name, bool unsync = true, int repeat = -1, bool check_timeout = true, SEL_CallFuncUP finished = NULL, void * user_param = NULL);
		// void stopAnimation();

		void stopCurrentAction(bool stop_animation = true, bool start_ignore_timeout = true);

		void pauseMove();
		void unpauseMove();

		bool checkTouchAtLocalPoint(const vec2&);
		float getDistToLocalPoint(const vec2&);

		bool checkTouchAtLocalRect(const Rect2&);

		virtual void onEnter();
		virtual void onExit();

		// virtual void setAnchorPoint(const CCPoint& value);
		virtual void setPosition(const CCPoint& value);

		// virtual bool isFlipX();
		// virtual void setFlipX(bool);

		void updateOrder();
		void updateMoveDir(const vec2& dir);

		Entity * getDecal(){ return decal; }
		void setDecal(Entity*);

		// Rect2 getRect();
	};

	class BaseItemsWindow: public ItemsWindow, public RGBAProtocolDummy
	{
		typedef ItemsWindow super;

	protected:

		float open_duration;
		float close_duration;

		CCAction * open_action;
		CCAction * close_action;

		bool pos_initialized;
		vec2 window_pos;

		BaseItemsWindow();

		virtual void savePosition();

		virtual CCAction * createOpenAction();
		virtual CCAction * createCloseAction();

	public:

		virtual void open();
		virtual void close();

		virtual void onEnter();

		// CCRGBAProtocol
		virtual GLubyte getOpacity();
		virtual void setOpacity(GLubyte opacity);
	};

	class MenuWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		CCMenuItem * mode_button_items[3];

		CCSprite * ribon;
		vec2 ribon_pos;

		MenuWindow();

		void levelButtonPressed(CCObject*);
		void playButtonPressed(CCObject*);
		void buyButtonPressed(CCObject*);
		void settingsButtonPressed(CCObject*);
		void exitButtonPressed(CCObject*);

		void showModeWindow(int mode);

		void mode1ButtonPressed(CCObject*);
		void mode2ButtonPressed(CCObject*);
		void mode3ButtonPressed(CCObject*);

		virtual void savePosition();		

	public:

		static MenuWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		MonsterHome * getGame(){ return game; }

		virtual void open();

		virtual void onEnter();
		virtual void onExit();
	};

	class MenuModeWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MenuWindow * menu;
		int mode;

		CCSprite * ribon;

		MenuModeWindow();
		virtual ~MenuModeWindow();

		void selectButtonPressed(CCObject*);
		void cancelButtonPressed(CCObject*);
		void buyButtonPressed(CCObject*);

	public:

		static MenuModeWindow * create(MenuWindow * menu, int mode);
		virtual bool initWithMenu(MenuWindow * menu, int mode);

		virtual void open();
		virtual void close();

		virtual void onEnter();
		virtual void onExit();
	};

	class MenuSettingsWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MenuWindow * menu;
		bool sound_enabled;
		bool music_enabled;
		bool reset_records;

		String lang_name;
		Vector<String> lang_names;

		CCSprite * ribon;

		MenuSettingsWindow();
		virtual ~MenuSettingsWindow();

		void normalButtonPressed(CCObject*);
		void checkedButtonPressed(CCObject*);
		void toggleButtonPressed(CCObject*);

		void saveButtonPressed(CCObject*);
		void cancelButtonPressed(CCObject*);

	public:

		static MenuSettingsWindow * create(MenuWindow * menu);
		virtual bool initWithMenu(MenuWindow * menu);

		virtual void open();
		virtual void close();

		virtual void onEnter();
		virtual void onExit();
	};

	class MenuExitWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MenuWindow * menu;

		MenuExitWindow();
		virtual ~MenuExitWindow();

		void okButtonPressed(CCObject*);
		void cancelButtonPressed(CCObject*);

	public:

		static MenuExitWindow * create(MenuWindow * menu);
		virtual bool initWithMenu(MenuWindow * menu);

		virtual void close();

		virtual void onEnter();
		virtual void onExit();
	};

	class ObjectiveWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		// SmartLabel * title;
		// SmartLabel * message;
		
		CCMenuItem * replay_button_item;
		CCMenu * buttons_menu;

		CCSprite * ribon;
		vec2 ribon_pos;

		ObjectiveWindow();

		virtual void savePosition();

		void okButtonPressed(CCObject*);
		void replayButtonPressed(CCObject*);
		void menuButtonPressed(CCObject*);

	public:

		static ObjectiveWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		virtual void open();

		void setReplayVisible(bool);

		virtual void onEnter();
		virtual void onExit();
	};

	class LevelRecordWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		// SmartLabel * title;
		// SmartLabel * message;
		CCSprite * ribon;
		vec2 ribon_pos;

		LevelRecordWindow();

		virtual void savePosition();

		void nextButtonPressed(CCObject*);
		void replayButtonPressed(CCObject*);
		void menuButtonPressed(CCObject*);

	public:

		static LevelRecordWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		virtual void open();

		virtual void onEnter();
		virtual void onExit();
	};

	class GameFinishedWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;

		GameFinishedWindow();

		void okButtonPressed(CCObject*);

	public:

		static GameFinishedWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		virtual void onEnter();
	};

	class LevelCompletedWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		// SmartLabel * title;
		// SmartLabel * message;
		CCSprite * ribon;
		vec2 ribon_pos;

		LevelCompletedWindow();

		virtual void savePosition();

		void nextButtonPressed(CCObject*);
		void replayButtonPressed(CCObject*);
		void menuButtonPressed(CCObject*);

	public:

		static LevelCompletedWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		virtual void open();

		virtual void onEnter();
		virtual void onExit();
	};

	enum ELevelFailedType {
		LEVEL_FAILED_LIVES,
		LEVEL_FAILED_MISSES,
	};

	class LevelFailedWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		CCSprite * ribon;
		ELevelFailedType type;

		LevelFailedWindow();

		void replayButtonPressed(CCObject*);
		void menuButtonPressed(CCObject*);

	public:

		static LevelFailedWindow * create(MonsterHome * game, ELevelFailedType);
		virtual bool initWithGame(MonsterHome * game, ELevelFailedType);

		// virtual void close();

		virtual void onEnter();
		virtual void onExit();
	};

	class ModeUnlockedWindow: public BaseItemsWindow
	{
		typedef BaseItemsWindow super;

	protected:

		MonsterHome * game;
		CCSprite * ribon;

		ModeUnlockedWindow();

		void okButtonPressed(CCObject*);
		void menuButtonPressed(CCObject*);

	public:

		static ModeUnlockedWindow * create(MonsterHome * game);
		virtual bool initWithGame(MonsterHome * game);

		virtual void close();

		virtual void onEnter();
		virtual void onExit();
	};

	enum EState {
		LOADING,
		OBJECTIVE,
		GAME,
		LEVEL_NEW_RECORD,
		LEVEL_COMPLETED,
		LEVEL_FAILED,
		MODE_UNLOCKED,
		GAME_FINISHED,
		MENU,
		// MENU_MODE,
	};

protected:

	CCMutableArray<CCNode*> touch_cache;

	enum EMonsterAttackType {
		MONSTER_ATTACK_DISABLED,
		MONSTER_ATTACK_DIFFERENT,
		MONSTER_ATTACK_ANY
	};

	struct LevelInfo
	{
		// int level;
		String objective_str;
		bool show_start_objective;

		std::vector<int> active_monsters;
		std::vector<int> responsible_monsters;
		std::vector<vec2> rel_homes;
		// int spawn_time_interval;
		float monster_speed_scale;
		int max_soft_monsters;
		int max_hard_monsters;
		int max_missed_monsters;
		int max_clouds;
		float clouds_rel_pos[2];
		float clouds_opacity;
		bool clouds_circular;
		vec2 clouds_rel_center;
		int lives;
		int req_monsters;
		bool any_home;
		bool trava;

		EMonsterAttackType attack_type;
		
		String background_texture;

		LevelInfo();
	};

	struct IgnoreFightRect
	{
		Rect2 rect;
		float time_sec;

		IgnoreFightRect();
		IgnoreFightRect(const Rect2& rect, float time_sec);
	};

	// float entity_scale;
	LevelInfo level_info;

	EState state;

	SmartLabel * hud_level_time;
	float level_time_sec;

	Layer * hud_layer;

	CCMenu * hud_pause_button;
	CCSprite * hud_mode_icon;

	SmartSprite * hud_monsters_icon;
	SmartLabel * hud_monsters_number;
	SmartLabel * hud_monsters_number_req;
	int monsters_number;
	float min_number_width;

	SmartSprite * hud_lives_icon;
	SmartLabel * hud_lives_number;
	int lives_number;

	SmartSprite * hud_missed_icon;
	SmartLabel * hud_max_missed_monsters;
	int max_missed_monsters_number;

	SmartSprite * hud_points_icon;
	SmartLabel * hud_points_number;
	int points_number;
	int start_level_points_number;

	float top_padding;
	float bottom_padding;

	// CCSprite * background;
	Layer * background_layer;
	// String background_texture_name;
	Layer * path_layer;
	Layer * monsters;
	Layer * homes;
	Layer * trava;
	Layer * clouds;

	Entity * selected_monster;

	EntityPath * path;
	Vector<IgnoreFightRect> ignore_fight_rects;

	// MenuWindow * menu_window;
	ObjectiveWindow * objective_window;

	CCAction * ambient_sound_action;

	MonsterHome();

	void createTrava();
	// void createMenuClouds();
	void createGameClouds(); // int count, float y1, float y2);
	void addCloud();

	void distanceWarningMonsters(Entity*, Entity*);
	void fightMonsters(Entity*, Entity*);

	Entity * addRandomMonster();
	void addMenuMonsters();

	Entity * findEntityAtPoint(Layer*, const vec2& p);
	Entity * findMonsterAtPoint(const vec2& p);

	CCMutableArray<CCNode*> * findEntitiesAtRect(Layer*, const Rect2& rect);

	void onEntityRemoved(Entity*);
	void onMonsterAtHome(Entity*);
	void onMonsterDead(Entity*);
	void onMonsterMissed(Entity*);

	bool isMonsterHome(Entity * monster, Entity * home);
	void selectMonster(Entity*);
	void unselectMonster();

	void schedulerUpdate(ccTime);

	void updateHudItemsPos();
	void setMonstersNumber(int value);
	void setLivesNumber(int value);
	void setMissedNumber(int value);
	void setLevelTime(float time_sec);

	void pauseButtonPressed(CCObject*);
	void buyButtonPressed(CCObject*);

	void showStartWindow();
	void showStartObjectiveWindow();

	void startLevel();

	bool isLastGameLevel();
	bool showGameFinishedWindow();

	void initLevelInfo(LevelInfo& level_info, int level, bool recursive = false);

	void startAmbientSounds();
	void stopAmbientSounds();

public:

	enum ELayer
	{
		BACKGROUND,
		HOMES,
		PATH_SEGS,
		MONSTERS,
		TRAVA,
		CLOUDS,
		HUD,
		WINDOW,
		VERSION,
		ADMOB,
	};

	LAYER_NODE_FUNC(MonsterHome);

	static CCScene * scene(int level = -1);

	virtual ~MonsterHome();

	virtual bool init();
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();

	virtual void update(ccTime);

	void initLevelInfo(int level);

	// void precacheLevel(int level);
	void loadLevel(int level);
	void loadNextLevel();

	/*
	enum ERestartLevelType {
		RESTART_BY_USER,
		RESTART_BY_NO_LIVES,
		RESTART_BY_MAX_MISSED,
	};
	void restartLevel(ERestartLevelType);
	*/

	void checkMonsters();
	void drawPath(EntityPath * path);
	void drawPaths();

	// default implements are used to call script callback if exist
	virtual bool ccTouchBegan(CCTouch * touch, CCEvent * event);
	virtual void ccTouchMoved(CCTouch * touch, CCEvent * event);
	virtual void ccTouchEnded(CCTouch * touch, CCEvent * event);
	virtual void ccTouchCancelled(CCTouch * touch, CCEvent * event);

	// default implements are used to call script callback if exist
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent * event);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent * event);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent * event);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent * event);

	static void gotoMenuScene();
	static void gotoLevelScene(int level);

	static void playHudClickSound();
};

#endif

#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

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

#define USE_SOUND_ENGINE

#include <AppCommon.h>
#include <JSON.h>

#ifdef USE_SOUND_ENGINE
// #include <IwSound.h>
#include <WavReader.h>
#endif // USE_SOUND_ENGINE

typedef bool (*CheckNodeFunc)(CCNode*, const vec2& local, void * user_data);
typedef bool (*CheckNodeRectFunc)(CCNode*, const Rect2& local_rect, void * user_data);
// typedef std::function<bool (CCNode*, const CCPoint& local)> CheckNodeFunc;

class JSON;
class App;

struct AdmobBannerDesc
{
	const char * const id;
	int width;
	int height;
};

class Settings
{
	friend class App;

public:

	enum {
		LEVEL_MENU_COLS = 5,
		LEVELS_NUMBER = LEVEL_MENU_COLS*3,
		MODES_NUMBER = 3,
	};

	struct LevelAward
	{
		// int level;
		int sec;

		LevelAward();
	};

protected:

	String lang_name;
	
	int mode;
	LevelAward level_awards[MODES_NUMBER][LEVELS_NUMBER];

	enum EModeAccessType {
		MODE_DISABLED,
		MODE_JUST_ENABLED,
		MODE_ENABLED,
	};
	EModeAccessType mode_access_type[MODES_NUMBER];

	bool sound_enabled;
	bool music_enabled;
	bool game_finished;

	Settings();
	virtual ~Settings();

	void reset();

	bool checkModeEnabled(int mode);

	String dumpJsonToString(JSON * json, int depth = 0);
	void safeToFile(JSON * json, const char * filename);

public:

	void load();
	void save();

	String getLangName(){ return lang_name; }
	void setLangName(const String& value);

	String getLangFilename();

	// zero based
	int getMode();
	void setMode(int mode);

	String getModeMusicFilename(int mode = -1);

	bool isGameFinished();
	void setGameFinished(bool);

	bool isModeEnabled(int mode);

	bool isModeJustUnlocked(int mode);
	void resetModeJustUnlocked();

	bool isSoundEnabled();
	void setSoundEnabled(bool);

	bool isMusicEnabled();
	void setMusicEnabled(bool);

	// zero based
	LevelAward getLevelAward(int level);
	void setLevelAward(int level, const LevelAward& award);
	void resetLevelAwards();

	bool isLevelEnabled(int level);
};

class  App: private CCApplication
{
	typedef CCApplication super;

protected:

	CCDictionary<String, JSON*> jsons;
	// CCDictionary<String, String> lang;

	Settings app_settings;
	JSON * lang_json;

	RandomValue rand;

	// String preloaded_music_filename;
	String music_filename;

	float entity_scale;

#ifdef USE_SOUND_ENGINE

	// CIwResHandlerWAV * wav_handler;

	typedef std::map<String, WavReader::SoundData*> SoundsData;
	SoundsData sounds_data;

#endif // USE_SOUND_ENGINE

	typedef std::map<String, Vector<String> > EntitySounds;
	typedef std::map<String, EntitySounds> EntitySoundsMap;
	EntitySoundsMap entity_sounds;

	std::map<String, int> entity_playing_sounds;

	struct DelayEntitySound
	{
		float start_time_sec;
		String entity_name;
		String channel;
	};
	Vector<DelayEntitySound> delay_entity_sounds;
	float recent_sound_time_sec;

	String version;
	String admob_banner_id;
	const AdmobBannerDesc * admob_banner;
	bool admob_available;
	bool admob_visible;

	CCObject * update_obj;
	// CCTimer * update_timer;

	void schedulerUpdate(ccTime);
	static void updateEntitySounds();

public:

	App();
	virtual ~App();

	static App * app(){ return (App*)&CCApplication::sharedApplication(); }
	static CCDirector * director(){ return CCDirector::sharedDirector(); }
	static SimpleAudioEngine * audio(){ return SimpleAudioEngine::sharedEngine(); }
	static Settings * settings(){ return &(app()->app_settings); }

	static String getVersion();
	static bool isEnding();

	static void updateEntityScale();
	static float getEntityScale();

	static void resetEntitySounds();
	static void registerEntitySound(const String& entity, const String& type, const String& sound);
	static String getEntityRandSound(const String& entity, const String& type);
	static void playEntitySound(const String& entity, const String& type);

	static const char * getBigFont();
	static const char * getFont();
	static const char * getSmallFont();

	static float getTimeSec();
	static int getTimeMS();

	static String timeToStr(int sec);
	static String timeToStr(float sec);

	static bool isAdmobVersion();
	static bool isDummyAdmob();
	static bool initAdmob();
	static bool showAdmob();
	static bool hideAdmob();
	static bool isAdmobVisible();
	static const AdmobBannerDesc * getAdmobDesc(const char * id);
	static const AdmobBannerDesc * getAdmobDesc();
	static void drawAdmob();
	static void buyGame();

	static void preloadMusic(const char * filename);
	static String getMusicFilename();
	static void setMusicFilename(const char * filename);
	static void playMusic(const char * filename, bool loop = true);
	static void stopMusic();
	static float getMusicVolume();
	static void setMusicVolume(float volume);
	static bool isMusicPlaying();

	static void preloadSound(const char * filename);
	static int playSound(const char * filename, bool loop = false);
	static void stopSound(int channel);
	static float getSoundVolume(int channel);
	static void setSoundVolume(int channel, float volume);
	static bool isSoundPlaying(int channel);
	static void stopAllSounds();

	static bool isFileExist(const char * filename);
	static String readFile(const char * filename, const char * mode = "rb");
	static String changeExt(const char * filename, const char * new_ext);
	static bool writeFile(const char * filename, const void * data, int data_len);
	static bool writeFile(const char * filename, const String& data);

	static vec2 getTouchPoint(CCTouch*);

	static CCMutableArray<CCNode*> * collectNodesAtPoint(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, CCTouch * touch, CheckNodeFunc func, void * user_data = NULL);
	static CCMutableArray<CCNode*> * collectNodesAtPoint(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, const vec2& point, CheckNodeFunc func, void * user_data = NULL);
	static CCMutableArray<CCNode*> * collectNodesAtRect(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, const Rect2& rect, CheckNodeRectFunc func, void * user_data = NULL);

	static void setRandSeed(int);
	static int getRand(int range);
	static int getRand(int min, int max);
	static float getRandFloat();
	static float getRandFloat(float min, float max);
	static float getRandSignFloat();

	// static JSON * getJSON(const char * name);
	static JSON * getJSON(const String& name);

	static void resetLangStrings();
	static String getLangStr(const String& string);
	static String caseNumber(int number, const String& base_string);
	static bool findLang(const String& lang_name);
	static String getSystemLang();

	static String splitString(const String& text, int max_width);

	static CCTexture2D * getTexture(const char* fileimage);
	static CCMutableArray<CCSpriteFrame*> * initSpriteFrames(CCTexture2D*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCMutableArray<CCSpriteFrame*> * initSpriteFrames(CCSpriteBatchNode*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCMutableArray<CCSpriteFrame*> * unsyncSpriteFrames(CCMutableArray<CCSpriteFrame*>*);
	
	static CCAnimation * unsyncAnimation(CCAnimation*);

	static CCAnimation * initAnimation(float delay, const char * tex_name, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCAnimation * initAnimation(float delay, const String& tex_name, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCAnimation * initAnimation(float delay, CCTexture2D*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCAnimation * initAnimation(float delay, CCSpriteBatchNode*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCAnimation * initUnsyncAnimation(float delay, CCTexture2D*, int cols, int rows = 1, int start = 0, int count = -1);
	static CCAnimation * initUnsyncAnimation(float delay, CCSpriteBatchNode*, int cols, int rows = 1, int start = 0, int count = -1);

	static CCSprite * initAnimatedSprite(float delay, CCTexture2D*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCSprite * initAnimatedSprite(float delay, CCSpriteBatchNode*, int cols, int rows = 1, int start = 0, int count = -1, bool unsync = false);
	static CCSprite * initUnsyncAnimatedSprite(float delay, CCTexture2D*, int cols, int rows = 1, int start = 0, int count = -1);
	static CCSprite * initUnsyncAnimatedSprite(float delay, CCSpriteBatchNode*, int cols, int rows = 1, int start = 0, int count = -1);

	static void setZOrder(CCNode*, int z);

	static int runGame();

    /**
    @brief	Implement for initialize OpenGL instance, set source path, etc...
    */
    virtual bool initInstance();

    /**
    @brief	Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();

};

#endif // _APP_DELEGATE_H_


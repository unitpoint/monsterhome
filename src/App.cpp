#include <App.h>

// #define DEBUG_UNLOCK_LEVELS
// #define DEBUG_NO_MUSIC

#if !defined _MSC_VER
#undef DEBUG_UNLOCK_LEVELS
#undef DEBUG_NO_MUSIC
#endif

#include "cocos2d.h"
#include "CCAutoreleasePool.h"
#include "MonsterHome.h"
#include "MyScene.h"

#include "CCEGLView.h"
#include "SimpleAudioEngine.h"
#include "JSON.h"
#include "platform/platform.h"
// #include "IwGx.h"
#include "s3eOSExec.h"

USING_NS_CC;
using namespace CocosDenshion;

#define __FONT_BIG "arial-en-ru-64.fnt"
#define __FONT_NORMAL "arial-en-ru-32.fnt"
#define __FONT_SMALL "arial-en-ru-16.fnt"
#define __FONT_SUPER_SMALL "arial-en-ru-8.fnt"

App::App()
{
#ifdef IW_DEBUG
	// IwDebugTraceDisable(true);
	// IwDebugTraceChannelSwitch("FILE", false);
#endif

	version = String(GAME_VERSION_MAJOR) + "." + String(GAME_VERSION_MINOR);
	if(GAME_VERSION_REVISION){
		version += "." + String(GAME_VERSION_REVISION);
	}
#ifdef GAME_FREE_VERSION
	version += "-free";
#endif

	// update_timer = NULL;
	update_obj = NULL;

#ifdef USE_SOUND_ENGINE
	// IwGxInit();
	// IwResManagerInit();
	// IwSoundInit();

#ifdef IW_BUILD_RESOURCES
	// IwGetResManager()->AddHandler(res_handler_wav = new CIwResHandlerWAV());
	// IwGetResManager()->AddHandler(new CIwResHandlerWAV());
#endif // IW_BUILD_RESOURCES

	// CIwResGroup * group = IwGetResManager()->GetGroupNamed("main");
	// wav_handler = new CIwResHandlerWAV();
#endif // USE_SOUND_ENGINE

	recent_sound_time_sec = 0;

	lang_json = NULL;

	cc_timeval time_val;
	CCTime::gettimeofdayCocos2d(&time_val, NULL);
	int seed = (int)(time_val.tv_sec + time_val.tv_usec);
	setRandSeed(seed);

	entity_scale = 1.0f;

	admob_visible = false;
	admob_banner = NULL;
#ifdef GAME_FREE_VERSION
	admob_available = AdmobAdsAvailable() ? true : false;
#else
	admob_available = false;
#endif
}

App::~App()
{
	// CC_SAFE_RELEASE_NULL(update_timer);
	CC_SAFE_RELEASE_NULL(update_obj);

	CC_SAFE_RELEASE_NULL(lang_json);
	settings()->save();

#ifdef USE_SOUND_ENGINE
	resetEntitySounds();
	// delete wav_handler;
	
	// IwSoundTerminate();
	// IwResManagerTerminate();
	// IwGxTerminate();
#else
#endif // USE_SOUND_ENGINE
}

String App::getVersion()
{
	return app()->version;
}

bool App::initInstance()
{
	IW_CALLSTACK("App::initInstance");
	bool bRet = false;
	do {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The HelloWorld is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create(TEXT("cocos2d: Hello World"), 480, 320));

#endif  // CC_PLATFORM_WIN32
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

		// OpenGLView initialized in testsAppDelegate.mm on ios platform, nothing need to do here.

#endif  // CC_PLATFORM_IOS
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

		// OpenGLView initialized in HelloWorld/android/jni/helloworld/main.cpp
		// the default setting is to create a fullscreen view
		// if you want to use auto-scale, please enable view->create(320,480) in main.cpp
		// if the resources under '/sdcard" or other writeable path, set it.
		// warning: the audio source should in assets/
		// cocos2d::CCFileUtils::setResourcePath("/sdcard");

#endif  // CC_PLATFORM_ANDROID
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WOPHONE)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The HelloWorld is designed as HVGA.
		CCEGLView* pMainWnd = new CCEGLView(this);
		CC_BREAK_IF(! pMainWnd || ! pMainWnd->Create(320,480, WM_WINDOW_ROTATE_MODE_CW));

#ifndef _TRANZDA_VM_  
		// on wophone emulator, we copy resources files to Work7/NEWPLUS/TDA_DATA/Data/ folder instead of zip file
		cocos2d::CCFileUtils::setResource("HelloWorld.zip");
#endif

#endif  // CC_PLATFORM_WOPHONE
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MARMALADE)
		// MaxAksenov said it's NOT a very elegant solution. I agree, haha
		CCDirector::sharedDirector()->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

		// Initialize OpenGLView instance, that release by CCDirector when application terminate.
		// The HelloWorld is designed as HVGA.
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd
				|| ! pMainWnd->Create("cocos2d: Hello World", 480, 320 ,480, 320));

		CCFileUtils::setResourcePath("./assets/");

#endif  // CC_PLATFORM_LINUX

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BADA)

		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(this, 480, 320));
		pMainWnd->setDeviceOrientation(Osp::Ui::ORIENTATION_LANDSCAPE);
		CCFileUtils::setResourcePath("/Res/");

#endif  // CC_PLATFORM_BADA

#if (CC_TARGET_PLATFORM == CC_PLATFORM_QNX)
		CCEGLView * pMainWnd = new CCEGLView();
		CC_BREAK_IF(! pMainWnd|| ! pMainWnd->Create(1024, 600));
		CCFileUtils::setResourcePath("app/native/Resources");
#endif // CC_PLATFORM_QNX
		bRet = true;
	} while (0);
	return bRet;
}

bool App::applicationDidFinishLaunching()
{
	IW_CALLSTACK("App::applicationDidFinishLaunching");

	// initialize director
	CCDirector * director = CCDirector::sharedDirector();

	director->setOpenGLView(&CCEGLView::sharedOpenGLView());

	// enable High Resource Mode(2x, such as iphone4) and maintains low resource on other devices.
//     director->enableRetinaDisplay(true);

	// turn on display FPS
#ifdef IW_DEBUG
	director->setDisplayFPS(true);
#else
	director->setDisplayFPS(false);
#endif

	// director->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

#if !defined _MSC_VER // defined S3E_ANDROID
	director->setDeviceOrientation(kCCDeviceOrientationPortrait);
#else
	director->setDeviceOrientation(kCCDeviceOrientationLandscapeLeft);
	// director->setDeviceOrientation(kCCDeviceOrientationPortraitUpsideDown);
	// director->setDeviceOrientation(kCCDeviceOrientationLandscapeRight);
#endif

	// setMusicVolume(0.5f);
	// playMusic("1-epic-theme.mp3");

	initAdmob();

	updateEntityScale();

	settings()->load();
	// settings()->save();
	
#ifdef IW_DEBUG
	{
		struct Lib {
			static void addChars(Vector<wchar_t>& chars, const char * utf8_text)
			{
				int text_len = strlen(utf8_text);
				wstring wtext;
				wtext.reserve(text_len+1);
				text_len = UTF8ToUCS2(utf8_text, text_len+1, (ucs2char*)wtext.c_str(), sizeof(wchar_t)*(text_len+1))-1;
				const wchar_t * wstr = wtext.c_str();
				for(int i = 0; i < text_len; i++){
					wchar_t c = wstr[i];
					if(std::find(chars.begin(), chars.end(), c) == chars.end()){
						chars.push_back(c);
					}
				}
			}
		};
		extern CCBMFontConfiguration* FNTConfigLoadFile( const char *fntFile);
		const char * fonts[] = { __FONT_BIG, __FONT_NORMAL, __FONT_SMALL, __FONT_SUPER_SMALL };

		for(int j, i = 0; i < ARR_SIZE(fonts); i++){
			Vector<wchar_t> chars;
			JSON * langs = App::getJSON("langs.txt");
			for(j = 0; j < langs->getChildrenCount(); j++){
				JSON * lang = langs->getChildAtIndex(j);
				Lib::addChars(chars, lang->getString());
				wchar_t * dbg = &*chars.begin();

				lang = App::getJSON(String::Format("lang-%s.txt", lang->getName().ToChar()));
				for(int k = 0; k < lang->getChildrenCount(); k++){
					JSON * item = lang->getChildAtIndex(k);
					Lib::addChars(chars, item->getString());
					dbg = &*chars.begin();
				}
			}
#if 1
			int text_len = chars.size();
			string text;
			text.reserve(text_len*5+1);
			text_len = UCS2ToUTF8((ucs2char*)&*chars.begin(), text_len, (char*)text.c_str(), sizeof(char)*(text_len*5));
			// text = text.substr(0, text_len);
			text = string(text.c_str(), text_len);
			const char * str = text.c_str();
			// str[text_len] = 0;
			CCLabelBMFont::labelWithString(str, fonts[i]); // auto released
#else
			CCBMFontConfiguration * font = FNTConfigLoadFile(fonts[i]);
			ASSERT(font);
			for(j = 0; j < (int)chars.size(); j++){
				unsigned int key = (unsigned int)chars[j];
				tKerningHashElement * element = NULL;
				HASH_FIND_INT(font->m_pFontDefDictionary, &key, element);
				CCAssert(element, "FontDefinition could not be found!");
			}
#endif
		}
	}
#endif

	update_obj = new CCObject();
	update_obj->autorelease();
	update_obj->retain();

	// update_timer = CCTimer::timerWithTarget(update_obj, schedule_selector(App::schedulerUpdate), 0.1f);
	// update_timer->retain();
	CCScheduler::sharedScheduler()->scheduleSelector(schedule_selector(App::schedulerUpdate), update_obj, 0.1f, false);

	// create a scene. it's an autorelease object
	// CCScene *pScene = MonsterHome::scene();
	CCScene *pScene = SplashScreen::scene();

	// run
	director->runWithScene(pScene);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void App::applicationDidEnterBackground()
{
	IW_CALLSTACK("App::applicationDidEnterBackground");

	CCDirector::sharedDirector()->pause();

	// if you use SimpleAudioEngine, it must be pause
	SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void App::applicationWillEnterForeground()
{
	IW_CALLSTACK("App::applicationWillEnterForeground");

	CCDirector::sharedDirector()->resume();

	// if you use SimpleAudioEngine, it must resume here
	SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

int App::runGame()
{
	s3eSurfaceSetInt(S3E_SURFACE_DEVICE_ORIENTATION_LOCK, 4);

	IwMemBucketInit();
	// IwMemBucketDebugSetBreakpoint(2331);
	IW_CALLSTACK("App::runGame");

	XString::Init();

	int32 checkpoint = IwMemBucketCheckpoint();

	App * app = new App;
	// App::setRandSeed(13);
	int ret = app->Run();
	delete app;

	// CocosDenshion::SimpleAudioEngine::deleteEngine();
	CCSharedFinalizer::finalizeAll();

	IwMemBucketDebugCheck(0, checkpoint, "check-app-leak.txt");

	XString::Shutdown();
	IwMemBucketTerminate();
	return ret;

}

bool App::isEnding()
{
	return s3eDeviceCheckQuitRequest() ? true : false;
}

float App::getTimeSec()
{
#if 0
	return (float)((double)s3eTimerGetUSTNanoseconds() * 0.000001);
#else
	return (float)((double)s3eTimerGetMs() * 0.001);
#endif
}

int App::getTimeMS()
{
	return (int)s3eTimerGetMs();
}

String App::timeToStr(float p_sec)
{
	return timeToStr((int)p_sec);
}

String App::timeToStr(int p_sec)
{
	int sec = p_sec % 60;
	int min = (p_sec / 60) % 60;
	int hour = p_sec / 3660;
	char str[128];
	if(hour > 0){
		sprintf(str, "%d:%02d:%02d", hour, min, sec);
	}else{
		sprintf(str, "%d:%02d", min, sec);
	}
	return str;
}

#ifdef GAME_FREE_VERSION
AdmobBannerDesc admob_banner_desc[] = {
	{"you-admob-codefor-468x60", 468, 60},
	{"you-admob-codefor-728x90", 728, 90}
};
#else
// const AdmobBannerDesc admob_banner_desc[] = {{""}};
#endif

bool App::isAdmobVersion()
{
	return app()->admob_available;
}

bool App::isDummyAdmob()
{
#ifdef _MSC_VER
	return isAdmobVersion() && true;
#else
	return false;
#endif
}

bool App::initAdmob()
{
#ifdef GAME_FREE_VERSION
	if(isAdmobVersion()){
		App * app = App::app();
		app->admob_banner_id = director()->getWinSize().height >= 640 ? "6276a8c77aea45f3" : "3860e8c895a74938";
		app->admob_visible = (InitAds(app->admob_banner_id) == S3E_RESULT_SUCCESS);
		app->admob_banner = getAdmobDesc(app->admob_banner_id);

		int edge = 14;
		for(int i = 0; i < ARR_SIZE(admob_banner_desc); i++){
			admob_banner_desc[i].width += edge;
			admob_banner_desc[i].height += edge;
		}
		return app->admob_visible;
	}
#endif
	return false;
}

bool App::showAdmob()
{
#ifdef GAME_FREE_VERSION
	if(isAdmobVersion()){
		return app()->admob_visible = (ShowAds() == S3E_RESULT_SUCCESS);
	}
#endif
	return false;
}

bool App::hideAdmob()
{
#ifdef GAME_FREE_VERSION
	if(isAdmobVersion()){
		app()->admob_visible = false;
		return HideAds() == S3E_RESULT_SUCCESS;
	}
#endif
	return false;
}

bool App::isAdmobVisible()
{
	return app()->admob_visible;
}

const AdmobBannerDesc * App::getAdmobDesc(const char * id)
{
#ifdef GAME_FREE_VERSION
	for(int i = 0; i < ARR_SIZE(admob_banner_desc); i++){
		if(strcmp(admob_banner_desc[i].id, id) == 0){
			return &admob_banner_desc[i];
		}
	}
#endif
	return NULL;
}

const AdmobBannerDesc * App::getAdmobDesc()
{
	return app()->admob_banner; // getAdmobDesc(app()->admob_banner_id);
}

void App::buyGame()
{
	static float processed_time_sec = 0;
	if(getTimeSec() - processed_time_sec > 2.0f){
		s3eOSExecExecute("market://details?id=com.unitpoint.monsterhome", false);
		processed_time_sec = getTimeSec();
	}else{
		int i = 0;
	}
}

void App::drawAdmob()
{
	if(isDummyAdmob() && app()->admob_visible){
		const AdmobBannerDesc * banner = getAdmobDesc();
		if(banner){
			glColor4f(0, 0.5, 0.5, 1.0f);
			Rect2 rect(vec2(0, 0), vec2((float)banner->width, (float)banner->height));

#if 1
			rect.origin.x = (director()->getWinSize().width - rect.size.x)/2;
#else
			rect.origin.y = director()->getWinSize().height - rect.size.y;
#endif

			CCPoint vertices[4] = {
				ccp(rect.origin.x, rect.origin.y), 
				ccp(rect.origin.x + rect.size.x, rect.origin.y),
				ccp(rect.origin.x + rect.size.x, rect.origin.y + rect.size.y), 
				ccp(rect.origin.x, rect.origin.y + rect.size.y)
			};
			ccDrawPoly(vertices, 4, true, true);			
			glColor4f(0, 0, 0, 1.0f);
		}
	}
}

void App::preloadMusic(const char * filename)
{
	IW_CALLSTACK("App::preloadMusic");
	// app()->preloaded_music_filename = filename;
#if 0 // doesn't work properly, audio()->stopBackgroundMusic() doesn't set NULL ptr of preloaded buffer 
	app()->music_filename = filename;
	audio()->preloadBackgroundMusic(filename);
#endif
}

String App::getMusicFilename()
{
	return app()->music_filename;
}

void App::setMusicFilename(const char * filename)
{
	app()->music_filename = filename;
}

void App::playMusic(const char * filename, bool loop)
{
	IW_CALLSTACK("App::playMusic");
	/* if(app()->music_filename != filename){
		audio()->stopBackgroundMusic();
		app()->music_filename = filename;
	} */
	stopMusic();
	app()->music_filename = filename;
	// audio()->preloadBackgroundMusic(filename);
#ifndef DEBUG_NO_MUSIC
	audio()->playBackgroundMusic(filename, loop);
#endif
}

void App::stopMusic()
{
	IW_CALLSTACK("App::stopMusic");
	audio()->stopBackgroundMusic();
}

float App::getMusicVolume()
{
	return audio()->getBackgroundMusicVolume();
}

void App::setMusicVolume(float volume)
{
	audio()->setBackgroundMusicVolume(volume);
}

bool App::isMusicPlaying()
{
	return audio()->isBackgroundMusicPlaying();
}

void App::preloadSound(const char * p_filename)
{
	IW_CALLSTACK("App::preloadSound");
#ifdef USE_SOUND_ENGINE
	App * app = App::app();

#if 1
	String filename = p_filename;
	if(app->sounds_data.find(filename) == app->sounds_data.end()){
		if(isFileExist(filename)){
			// CIwSoundData * res = dynamic_cast<CIwSoundData*>(app->wav_handler->Build((const char*)filename));
			// app->sounds_data[filename] = res;
			WavReader::SoundData * data = WavReader::load(filename);
			app->sounds_data[filename] = data;
		}else{
			app->sounds_data[filename] = NULL;
		}
	}
#else
	String filename = p_filename;
	// CIwResource * res = app->res_handler_wav->Build(filename);
	CIwResource * res3 = IwGetResManager()->LoadRes(filename);
	CIwSoundSpec * res2 = (CIwSoundSpec*)IwGetResManager()->GetResNamed(changeExt(filename, ""), "CIwSoundSpec");;
	// app->loaded_sounds[filename] = res2;
#endif
#else
	audio()->preloadEffect(p_filename);
#endif
}

int App::playSound(const char * filename, bool loop)
{
	IW_CALLSTACK("App::playSound");
	if(!filename[0]){
		return -1;
	}
#ifdef USE_SOUND_ENGINE
	App * app = App::app();
	SoundsData::iterator it = app->sounds_data.find(filename);
	if(it == app->sounds_data.end()){
		preloadSound(filename);
		it = app->sounds_data.find(filename);
	}

#if 1
	WavReader::SoundData * data = it->second;
	if(!data || !data->buffer_size){
		return -1;
	}
#else
	class OpenSoundData: public CIwSoundData
	{
	public:

		int16 * GetBuffer()
		{
			return (int16*)m_Samples;
		}
	};

	OpenSoundData * res = (OpenSoundData*)it->second;
	if(!res || res->GetBufferSize() == 0){
		return -1;
	}
#endif

	int channel = s3eSoundGetFreeChannel();		
	// s3eSoundChannelPlay(channel, res->GetBuffer(), res->GetBufferSize()/2, loop ? 0 : 1, 0);
	s3eSoundChannelPlay(channel, (int16*)data->samples, data->buffer_size/2, loop ? 0 : 1, 0);
	if(s3eSoundGetError() != S3E_SOUND_ERR_NONE){
		IwAssertMsg(GAME, false, ("Play sound %s Failed. Error Code : %s", filename, s3eSoundGetErrorString()));	
	}
	return channel;
#else
	return (int)audio()->playEffect(filename, loop);
#endif
}

void App::stopSound(int channel)
{
#ifdef USE_SOUND_ENGINE
	s3eSoundChannelStop(channel);
#else
	audio()->stopEffect((unsigned int)channel);
#endif
}

void App::stopAllSounds()
{
#ifdef USE_SOUND_ENGINE
	s3eSoundStopAllChannels();
#else
	audio()->stopAllEffects();
#endif
}

float App::getSoundVolume(int channel)
{
#ifdef USE_SOUND_ENGINE
	return (float)s3eSoundChannelGetInt(channel, S3E_CHANNEL_VOLUME) / (float)S3E_SOUND_MAX_VOLUME;
#else
	return audio()->getEffectsVolume();
#endif
}

void App::setSoundVolume(int channel, float volume)
{
#ifdef USE_SOUND_ENGINE
	s3eSoundChannelSetInt(channel, S3E_CHANNEL_VOLUME, (int)(volume * (float)S3E_SOUND_MAX_VOLUME));
#else
	audio()->setEffectsVolume(volume);
#endif
}

bool App::isSoundPlaying(int channel)
{
#ifdef USE_SOUND_ENGINE
	return s3eSoundChannelGetInt(channel, S3E_CHANNEL_STATUS) != 0;
#else
	// audio has no that function
	return s3eSoundChannelGetInt(channel, S3E_CHANNEL_STATUS) != 0;
#endif
}

CCTexture2D * App::getTexture(const char* fileimage)
{
	IW_CALLSTACK("App::getTexture");
	return CCTextureCache::sharedTextureCache()->addImage(fileimage);
}

CCMutableArray<CCSpriteFrame*> * App::initSpriteFrames(CCTexture2D * tex, int cols, int rows, int start, int count, bool unsync)
{
	IW_CALLSTACK("App::initSpriteFrames");

	CCSize tex_size = tex->getContentSize();
	CCSize rect_size(tex_size.width / cols, tex_size.height / rows);
	CCMutableArray<CCSpriteFrame*> * frames = new CCMutableArray<CCSpriteFrame*>();
	frames->autorelease();
	int end = start + (count < 0 ? cols * rows : count) - 1;
	int y_start = start / cols;
	int y_end = MIN(rows-1, end / cols);
	for(int y = y_start; y <= y_end; y++){
		for(int x = 0; x < cols; x++){
			int i = y*cols + x;
			if(i >= start && i <= end){
				CCRect rect(rect_size.width * x, rect_size.height * y, rect_size.width, rect_size.height);
				CCSpriteFrame * frame = CCSpriteFrame::frameWithTexture(tex, rect);
				frames->addObject(frame);
			}
		}
	}
	if(unsync){
		int count = App::getRand(frames->count());
		for(; count > 0; count--){
			CCSpriteFrame * frame = frames->getLastObject();
			frames->insertObjectAtIndex(frame, 0);
			frames->removeLastObject();
		}
	}
	return frames;
}

CCMutableArray<CCSpriteFrame*> * App::initSpriteFrames(CCSpriteBatchNode * sheet, int cols, int rows, int start, int count, bool unsync)
{
	return initSpriteFrames(sheet->getTexture(), cols, rows, start, count, unsync);
}

CCMutableArray<CCSpriteFrame*> * App::unsyncSpriteFrames(CCMutableArray<CCSpriteFrame*> * frames)
{
	IW_CALLSTACK("App::unsyncSpriteFrames");
	CCMutableArray<CCSpriteFrame*> * new_frames = new CCMutableArray<CCSpriteFrame*>();
	new_frames->autorelease();
	int count = frames->count();
	int i = App::getRand(count);
	for(int j = 0; j < count; j++){
		new_frames->addObject(frames->getObjectAtIndex((j+i) % count));
	}
	return new_frames;
}

CCAnimation * App::unsyncAnimation(CCAnimation * animation)
{
	IW_CALLSTACK("App::unsyncAnimation");
	CCAnimation * new_animation = CCAnimation::animationWithFrames(App::unsyncSpriteFrames(animation->getFrames()), animation->getDelay());
	new_animation->setName(animation->getName());
	return new_animation;
}

CCAnimation * App::initAnimation(float delay, const char * tex_name, int cols, int rows, int start, int count, bool unsync)
{
	return initAnimation(delay, getTexture(tex_name), cols, rows, start, count, unsync);
}

CCAnimation * App::initAnimation(float delay, const String& tex_name, int cols, int rows, int start, int count, bool unsync)
{
	return initAnimation(delay, getTexture(tex_name), cols, rows, start, count, unsync);
}

CCAnimation * App::initAnimation(float delay, CCTexture2D * tex, int cols, int rows, int start, int count, bool unsync)
{
	CCMutableArray<CCSpriteFrame*> * frames = initSpriteFrames(tex, cols, rows, start, count, unsync);
	return CCAnimation::animationWithFrames(frames, delay);
}

CCAnimation * App::initAnimation(float delay, CCSpriteBatchNode * sheet, int cols, int rows, int start, int count, bool unsync)
{
	return initAnimation(delay, sheet->getTexture(), cols, rows, start, count, unsync);
}

CCAnimation * App::initUnsyncAnimation(float delay, CCTexture2D * tex, int cols, int rows, int start, int count)
{
	return initAnimation(delay, tex, cols, rows, start, count, true);
}

CCAnimation * App::initUnsyncAnimation(float delay, CCSpriteBatchNode * sheet, int cols, int rows, int start, int count)
{
	return initAnimation(delay, sheet, cols, rows, start, count, true);
}

CCSprite * App::initAnimatedSprite(float delay, CCTexture2D * tex, int cols, int rows, int start, int count, bool unsync)
{
	IW_CALLSTACK("App::initAnimatedSprite");
	CCAnimation * animation = App::initAnimation(delay, tex, cols, rows, start, count, unsync);

	CCSprite * sprite = CCSprite::spriteWithTexture(tex, animation->getFrames()->getObjectAtIndex(0)->getRect());

	CCActionInterval * animate = CCAnimate::actionWithAnimation(animation, false);
	animate = CCRepeatForever::actionWithAction(animate);
	sprite->runAction(animate);

	return sprite;
}

CCSprite * App::initAnimatedSprite(float delay, CCSpriteBatchNode * sheet, int cols, int rows, int start, int count, bool unsync)
{
	IW_CALLSTACK("App::initAnimatedSprite 2");
	CCAnimation * animation = App::initAnimation(delay, sheet, cols, rows, start, count, unsync);

	CCSprite * sprite = CCSprite::spriteWithBatchNode(sheet, animation->getFrames()->getObjectAtIndex(0)->getRect());
	sheet->addChild(sprite);

	CCActionInterval * animate = CCAnimate::actionWithAnimation(animation, false);
	animate = CCRepeatForever::actionWithAction(animate);
	sprite->runAction(animate);

	return sprite;
}

CCSprite * App::initUnsyncAnimatedSprite(float delay, CCTexture2D * tex, int cols, int rows, int start, int count)
{
	return initAnimatedSprite(delay, tex, cols, rows, start, count, true);
}

CCSprite * App::initUnsyncAnimatedSprite(float delay, CCSpriteBatchNode * sheet, int cols, int rows, int start, int count)
{
	return initAnimatedSprite(delay, sheet, cols, rows, start, count, true);
}

void App::setZOrder(CCNode * node, int z)
{
	if(node->getZOrder() != z){
		CCNode * parent = node->getParent();
		// CCAssert(parent, "");
		if(parent){
			parent->reorderChild(node, z);
		}
	}
}

bool App::isFileExist(const char * filename)
{
	return s3eFileCheckExists(filename) ? true : false;
}

String App::readFile(const char * filename, const char * mode)
{
	IW_CALLSTACK("App::readFile");
#if 1
	s3eFile * file = s3eFileOpen(filename, mode);
    if(!file){ // && getIsPopupNotify()){    
        IwAssertMsg(GAME, file, ("Open file %s Failed. s3eFileError Code : %i", filename, s3eFileGetError()));
    }
    if(!file){
        return String();
    }
	String str;
	int file_size = (int)s3eFileGetSize(file);
	str.SetSize(file_size);
	s3eFileRead((void*)str.ToMemory(), file_size, 1, file);
	s3eFileClose(file);
	return str;
#else
	unsigned long size;
	unsigned char * buf = CCFileUtils::getFileData(filename, "r", &size);
	String str = String((char*)buf, size);
	delete [] buf;
	return str;
#endif
}

bool App::writeFile(const char * filename, const void * data, int data_len)
{
	IW_CALLSTACK("App::writeFile");
	s3eFile * file = s3eFileOpen(filename, "wb");
    if(!file){ // && getIsPopupNotify()){    
        IwAssertMsg(GAME, file, ("Open file %s Failed. s3eFileError Code : %i", filename, s3eFileGetError()));
    }
    if(!file){
        return false;
    }
	s3eFileWrite(data, data_len, 1, file);
	s3eFileClose(file);
	return true;
}

bool App::writeFile(const char * filename, const String& data)
{
	return writeFile(filename, data.ToMemory(), data.Size());
}

String App::changeExt(const char * full_filename, const char * new_ext)
{
	IW_CALLSTACK("App::changeExt");
	char * filename = strrchr(full_filename, '/');
	char * filename2 = strrchr(full_filename, '\\');
	if(filename && filename2){
		filename = MAX(filename, filename2);
	}else if(!filename){
		filename = filename2;
	}
	if(!filename){
		filename = (char*)full_filename;
	}
	char * ext = strrchr(filename, '.');
	if(!ext){
		return String(full_filename, strlen(full_filename), new_ext, strlen(new_ext));
	}
	return String(full_filename, ext - full_filename, new_ext, strlen(new_ext));
}

vec2 App::getTouchPoint(CCTouch * touch)
{
	return director()->convertToGL(touch->locationInView());
}

CCMutableArray<CCNode*> * App::collectNodesAtPoint(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, CCTouch * touch, CheckNodeFunc func, void * user_data)
{
	return collectNodesAtPoint(result, parent, tag, padding, getTouchPoint(touch), func, user_data);
}

CCMutableArray<CCNode*> * App::collectNodesAtPoint(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, const vec2& point, CheckNodeFunc func, void * user_data)
{
	IW_CALLSTACK("App::collectNodesAtPoint");
	if(!result){
		result = new CCMutableArray<CCNode*>();
		result->autorelease();
	}else{
		result->removeAllObjects();
	}
	bool is_padding_used = padding != 0.0f;
	bool func_used = func ? true : false;
	float padding_x2 = padding * 2;
	CCArray * children = parent->getChildren();
	if(children && children->count() > 0){
		CCObject* obj = NULL;
		CCARRAY_FOREACH(children, obj){
			CCNode* child = dynamic_cast<CCNode*>(obj);
			if(child && child->getIsVisible() && (!tag || child->getTag() == tag)){
				vec2 local = child->convertToNodeSpace(point);
				vec2 size = child->getContentSize();
#if 0
				const CCPoint& pos = child->getPosition();
				const CCPoint& anchor = child->getAnchorPoint();
				// CCRect r(pos.x - size.width * anchor.x, pos.y - size.height * anchor.y, size.width, size.height);
				CCRect r(size.width * anchor.x, size.height * anchor.y, size.width, size.height);
				// r.origin = CCPointZero;
#else
				Rect2 r(vec2(0, 0), size);
#endif
				if(is_padding_used){
					r.origin.x -= padding;
					r.origin.y -= padding;
					r.size.x += padding_x2;
					r.size.y += padding_x2;
				}

				if(r.ContainsPoint(local)){
					if(!func_used || func(child, local, user_data)){
						result->addObject(child);
					}
				}
			}
		}
	}
	if(result->count() == 0){
		return NULL;
	}
	return result;
}

CCMutableArray<CCNode*> * App::collectNodesAtRect(CCMutableArray<CCNode*> * result, CCNode * parent, int tag, float padding, const Rect2& rect, CheckNodeRectFunc func, void * user_data)
{
	IW_CALLSTACK("App::collectNodesAtRect");
	if(!result){
		result = new CCMutableArray<CCNode*>();
		result->autorelease();
	}else{
		result->removeAllObjects();
	}
	bool is_padding_used = padding != 0.0f;
	bool func_used = func ? true : false;
	float padding_x2 = padding * 2;
	// Rect2 local_rect;
	Bounds2 local_bounds;
	CCArray * children = parent->getChildren();
	if(children && children->count() > 0){
		CCObject* obj = NULL;
		CCARRAY_FOREACH(children, obj){
			CCNode* child = dynamic_cast<CCNode*>(obj);
			if(child && child->getIsVisible() && (!tag || child->getTag() == tag)){
				local_bounds.Init(child->convertToNodeSpace(rect.origin));
				local_bounds.Add(child->convertToNodeSpace(rect.origin + rect.size));

				Rect2 local_rect(local_bounds.b[0], local_bounds.b[1] - local_bounds.b[0]);

				vec2 size = child->getContentSize();
#if 0
				const CCPoint& pos = child->getPosition();
				const CCPoint& anchor = child->getAnchorPoint();
				// CCRect r(pos.x - size.width * anchor.x, pos.y - size.height * anchor.y, size.width, size.height);
				CCRect r(size.width * anchor.x, size.height * anchor.y, size.width, size.height);
				// r.origin = CCPointZero;
#else
				Rect2 r(vec2(0, 0), size);
#endif
				if(is_padding_used){
					r.origin.x -= padding;
					r.origin.y -= padding;
					r.size.x += padding_x2;
					r.size.y += padding_x2;
				}

				if(r.IntersectRect(local_rect)){
					if(!func_used || func(child, local_rect, user_data)){
						result->addObject(child);
					}
				}
			}
		}
	}
	if(result->count() == 0){
		return NULL;
	}
	return result;
}

void App::setRandSeed(int seed)
{
	App * app = App::app();
	app->rand.SetSeed(seed);
}

int App::getRand(int range)
{
	// return IwRandRange(range);
	App * app = App::app();
	return app->rand.Int(range);
}

int App::getRand(int min, int max)
{
	// return IwRandMinMax(min, max);
	App * app = App::app();
	return app->rand.Int(max - min + 1) + min;
}

float App::getRandFloat()
{
#if 1
	App * app = App::app();
	return app->rand.Float();
#else
	int rand1 = IwRand(); // 0x7fff;
	int rand2 = IwRand(); // 0x7fff;
#if 1
	int rand3 = (rand1 << 15) | rand2;
	return (float)((double)rand3 / (double)(((0x7fff<<15)|0x7fff)+1));
#else
	int rand3 = rand1 * rand2;
	return (float)((double)rand3 / (double)(0x7fff * 0x7fff + 1));
#endif
#endif
}

float App::getRandFloat(float min, float max)
{
	return min + getRandFloat() * (max - min);
}

float App::getRandSignFloat()
{
	// return (getRandFloat() * 2.0f) - 1.0f;
	App * app = App::app();
	return app->rand.CFloat();
}

JSON * App::getJSON(const String& name)
{
	IW_CALLSTACK("App::getJSON");
	App * app = App::app();
	JSON * config = app->jsons.objectForKey(name);
	if(!config){
		config = new JSON(name);
		CCAssert(config, "");
		config->setName(name);
		app->jsons.setObject(config, name);
	}
	return config;
}

void App::resetLangStrings()
{
	App * app = App::app();
	if(app->lang_json){
		// app->jsons.removeObjectForKey(app->lang_json->getName()); // settings()->getLangFilename());
		CC_SAFE_RELEASE_NULL(app->lang_json);
	}
}

String App::getLangStr(const String& string)
{
	// IW_CALLSTACK("App::getLangStr");
	App * app = App::app();
	if(!app->lang_json){
		app->lang_json = getJSON(settings()->getLangFilename());
		CC_SAFE_RETAIN(app->lang_json);
	}
	return app->lang_json->getChildString(string, string);
}

String App::caseNumber(int number, const String& base_string)
{
	String lang_name = settings()->getLangName();
	if(lang_name == "ru" || lang_name == "uk"){
		String name;
		int i = number % 10;
		int j = number % 100;
		if(i == 1 && j/10 != 1){
			name = base_string+"_1";
		}else if(i >= 2 && i <= 4 && j/10 != 1){
			name = base_string+"_2";
		}else{
			name = base_string+"_3";
		}
		return String(number)+" "+getLangStr(name);
	}
	return String(number)+" "+getLangStr(base_string+(number == 1 ? "_1" : "_2"));
}

bool App::findLang(const String& lang_name)
{
	JSON * langs = App::getJSON("langs.txt");
	for(int i = 0; i < langs->getChildrenCount(); i++){
		JSON * lang = langs->getChildAtIndex(i);
		String name = lang->getName();
		if(name == lang_name){
			return true;
		}
	}
	return false;
}

String App::getSystemLang()
{
	struct Lib {
		static String returnLang(const String& lang_name)
		{
			if(App::findLang(lang_name)){
				return lang_name;
			}
			return "en";
		}
	};
	int lang_id = s3eDeviceGetInt(S3E_DEVICE_LANGUAGE);
	switch(lang_id){
	case S3E_DEVICE_LANGUAGE_RUSSIAN:
		return Lib::returnLang("ru");

	case S3E_DEVICE_LANGUAGE_UKRAINIAN:
		return Lib::returnLang("uk");

	case S3E_DEVICE_LANGUAGE_ITALIAN:
		return Lib::returnLang("it");

	case S3E_DEVICE_LANGUAGE_FRENCH:
		return Lib::returnLang("fr");

	case S3E_DEVICE_LANGUAGE_GERMAN:
		return Lib::returnLang("de");

	case S3E_DEVICE_LANGUAGE_SPANISH:
		return Lib::returnLang("es");

	// case S3E_DEVICE_LANGUAGE_ENGLISH:
	}
	return "en";
}

String App::splitString(const String& text, int max_width)
{
	struct Lib {
		static String toUtf8Trim(const wchar_t * s, int len)
		{
			String out;
			out.SetSize(len*4);
			int out_len = UCS2ToUTF8((ucs2char*)s, len, (char*)out.ToChar(), out.Size());
			out.SetSize(out_len, 0, true);
			return out.Trim();
		}

		static void Append(String& out, const String& s)
		{
			if(out.Len() > 0){
				out.Append("\n");
			}
			out.Append(s);
		}

		static void Append(String& out, const wchar_t * s, int len)
		{
			Append(out, toUtf8Trim(s, len));
		}
	};

	int text_len = text.Len();
	wstring wtext;
	wtext.reserve(text_len+1);
	int len = UTF8ToUCS2(text, text_len+1, (ucs2char*)wtext.c_str(), sizeof(wchar_t)*(text_len+1))-1;
	const wchar_t * s = wtext.c_str();

	String out;

	int i = 0, line_start = 0, word_end = 0, cur_width = 0;
	for(; i < len; i++)
	{
		if(s[i] == L'\r' || s[i] == L'\n')
		{		
			Lib::Append(out, s+line_start, i-line_start);
			if(s[i+1] == L"\r\n"[s[i] == L'\r']) // i+1 is always valid, because data is null terminated
			{
				i++;
			}
			word_end = line_start = i+1;
			cur_width = 0;
		}else if(++cur_width > max_width && word_end > line_start){
			Lib::Append(out, s+line_start, word_end-line_start);
			line_start = ++word_end;
			cur_width = i-line_start+1;
		}else if(s[i] <= L' '){
			word_end = i;
		}
	}
	if(i > line_start)
	{
		Lib::Append(out, s+line_start, i-line_start);
	}
	return out;
}

void App::updateEntityScale()
{
	vec2 size = director()->getWinSize();
	float entity_scale = (vec2(size.Max(), size.Min()) / vec2(800.0f, 480.0f)).Min();
	if(entity_scale > 1.0f){
		// entity_scale = MathLib::EpsFloor(entity_scale, 0.1f);
		entity_scale = MathLib::EpsRound(entity_scale, 0.1f);
	}
	app()->entity_scale = entity_scale;
}

float App::getEntityScale()
{
	return app()->entity_scale;
}

void App::resetEntitySounds()
{
	App * app = App::app();

#ifdef USE_SOUND_ENGINE
	SoundsData::iterator it = app->sounds_data.begin();
	for(; it != app->sounds_data.end(); ++it){
		delete it->second;
	}
	app->sounds_data.clear();
#endif // USE_SOUND_ENGINE
}

void App::registerEntitySound(const String& entity_name, const String& type, const String& sound)
{
	App * app = App::app();
	EntitySoundsMap::iterator it = app->entity_sounds.find(entity_name);
	if(it == app->entity_sounds.end()){
		EntitySounds entity_sounds;
		Vector<String> sounds;
		sounds.push_back(sound);
		entity_sounds[type] = sounds;
		app->entity_sounds[entity_name] = entity_sounds;
		return;
	}
	EntitySounds& entity_sounds = it->second;
	EntitySounds::iterator type_it = entity_sounds.find(type);
	if(type_it == entity_sounds.end()){
		Vector<String> sounds;
		sounds.push_back(sound);
		entity_sounds[type] = sounds;
		return;
	}
	Vector<String>& sounds = type_it->second;
	if(std::find(sounds.begin(), sounds.end(), sound) == sounds.end()){
		sounds.push_back(sound);
	}
}

String App::getEntityRandSound(const String& entity_name, const String& type)
{
	App * app = App::app();
	EntitySoundsMap::iterator it = app->entity_sounds.find(entity_name);
	if(it == app->entity_sounds.end()){
		return "";
	}
	EntitySounds& entity_sounds = it->second;
	EntitySounds::iterator type_it = entity_sounds.find(type);
	if(type_it == entity_sounds.end()){
		return "";
	}
	Vector<String>& sounds = type_it->second;
	if(sounds.size() == 0){
		return "";
	}
	return sounds[App::getRand(sounds.size())];
}

void App::playEntitySound(const String& entity, const String& type)
{
	App * app = App::app();

	Vector<DelayEntitySound>::iterator it = app->delay_entity_sounds.begin();
	for(; it != app->delay_entity_sounds.end(); ++it){
		DelayEntitySound& item = *it;
		if(item.entity_name == entity && item.channel == type){
			return;
		}
	}
	if(app->delay_entity_sounds.size() > 5){
		app->delay_entity_sounds.erase(app->delay_entity_sounds.begin());
	}
	DelayEntitySound item;
	item.entity_name = entity;
	item.channel = type;
	item.start_time_sec = getTimeSec();
	app->delay_entity_sounds.push_back(item);
}

void App::schedulerUpdate(ccTime)
{
	App::updateEntitySounds();
}

void App::updateEntitySounds()
{
	App * app = App::app();

	float cur_time_sec = getTimeSec();
	if(cur_time_sec - app->recent_sound_time_sec < 0.05f){
		return;
	}
	for(int i = 0; i < (int)app->delay_entity_sounds.size(); i++){
		DelayEntitySound& item = app->delay_entity_sounds[i];
		if(cur_time_sec - item.start_time_sec > 0.3f){
			app->delay_entity_sounds.erase(app->delay_entity_sounds.begin()+i);
			i--;
			continue;
		}

		String channel_name = item.entity_name+"###"+item.channel;
		std::map<String, int>::iterator it = app->entity_playing_sounds.find(channel_name);
		if(it != app->entity_playing_sounds.end()){
			int channel = it->second;
			if(App::isSoundPlaying(channel)){
				app->delay_entity_sounds.erase(app->delay_entity_sounds.begin()+i);
				i--;
				continue;
			}
			app->entity_playing_sounds.erase(it);
		}
		int channel = playSound(getEntityRandSound(item.entity_name, item.channel));
		if(channel >= 0){
			for(it = app->entity_playing_sounds.begin(); it != app->entity_playing_sounds.end(); ++it){
				if(it->second == channel){
					app->entity_playing_sounds.erase(it);
					break;
				}
			}
			app->entity_playing_sounds[channel_name] = channel;
			app->recent_sound_time_sec = cur_time_sec;
		}
		app->delay_entity_sounds.erase(app->delay_entity_sounds.begin()+i);
		return;
	}
}

const char * App::getBigFont()
{
	float scale = getEntityScale();
	return scale >= 1.0f ? __FONT_BIG : __FONT_NORMAL;
}

const char * App::getFont()
{
	float scale = getEntityScale();
	return scale >= 1.0f ? __FONT_NORMAL : (scale >= 0.55f ? __FONT_SMALL : __FONT_SUPER_SMALL);
}

const char * App::getSmallFont()
{
	float scale = getEntityScale();
	return scale >= 1.0f ? __FONT_SMALL : __FONT_SUPER_SMALL;
}

// ===============================================================================
// ===============================================================================
// ===============================================================================

static const char settings_filename[] = "settings.txt";

Settings::LevelAward::LevelAward()
{
	sec = 0;
}

Settings::Settings()
{
	reset();
}

Settings::~Settings()
{
}

void Settings::setLangName(const String& value)
{
	if(lang_name != value){
		lang_name = value;
		App::resetLangStrings();
	}
}

String Settings::getLangFilename()
{
	return "lang-"+lang_name+".txt";
}

Settings::LevelAward Settings::getLevelAward(int level)
{
	if(level >= 0 && level < LEVELS_NUMBER){
		return level_awards[mode][level];
	}
	return LevelAward(); 
}

void Settings::setLevelAward(int level, const LevelAward& award)
{
	ASSERT(mode >= 0 && mode < MODES_NUMBER);
	if(level >= 0 && level < LEVELS_NUMBER){
		level_awards[mode][level] = award;

		if(mode+1 < MODES_NUMBER && mode_access_type[mode+1] == MODE_DISABLED){ // != MODE_ENABLED){
			if(!App::isAdmobVersion() || mode != Settings::MODES_NUMBER-1){
				mode_access_type[mode+1] = checkModeEnabled(mode+1) ? MODE_JUST_ENABLED : MODE_DISABLED;
			}
		}
	}
}

bool Settings::isLevelEnabled(int level)
{
#ifdef DEBUG_UNLOCK_LEVELS
	return true;
#endif

	if(level < 0){
		return true;
	}
	if(level >= LEVELS_NUMBER){
		return false;
	}
	if(level_awards[mode][level].sec > 0){
		return true;
	}
	if(level == 0){
		if(mode > 0){
			return isModeEnabled(mode); // level_awards[mode-1][LEVELS_NUMBER-1].sec > 0;
		}
		return true;
	}
	return level_awards[mode][level-1].sec > 0;
}

int Settings::getMode()
{
	return mode;
}

void Settings::setMode(int mode)
{
	this->mode = MathLib::Clamp(mode, 0, (int)MODES_NUMBER-1 - (int)App::isAdmobVersion());
}

String Settings::getModeMusicFilename(int mode)
{
	if(mode < 0){
		mode = this->mode;
	}
	const char * musics[] = {
		"music-1.mp3",
		"music-2.mp3",
		"music-3.mp3"
	};
	return musics[mode % ARR_SIZE(musics)];
}

bool Settings::isGameFinished()
{
	return game_finished;
}

void Settings::setGameFinished(bool value)
{
	game_finished = value;
}

bool Settings::isModeEnabled(int mode)
{
#ifdef DEBUG_UNLOCK_LEVELS
	return true;
#endif
	if(mode <= 0){
		return true;
	}
	if(mode >= MODES_NUMBER){
		return false;
	}
	return mode_access_type[mode] != MODE_DISABLED;
}

bool Settings::checkModeEnabled(int mode)
{
	// return true; // debug
	if(mode <= 0){
		return true;
	}
	if(mode >= MODES_NUMBER){
		return false;
	}
	// return mode_access_type[mode] != MODE_DISABLED;
	return true; // mode_access_type[mode] != MODE_DISABLED || level_awards[mode-1][LEVELS_NUMBER-1].sec > 0;
}

bool Settings::isModeJustUnlocked(int mode)
{
	if(mode >= 0 && mode < MODES_NUMBER){
		return mode_access_type[mode] == MODE_JUST_ENABLED;
	}
	return false;
}

void Settings::resetModeJustUnlocked()
{
	if(mode_access_type[mode] == MODE_JUST_ENABLED){
		mode_access_type[mode] = MODE_ENABLED;
	}
}

bool Settings::isSoundEnabled()
{
	return sound_enabled;
}

void Settings::setSoundEnabled(bool value)
{
	sound_enabled = value;
}

bool Settings::isMusicEnabled()
{
	return music_enabled;
}

void Settings::setMusicEnabled(bool value)
{
	music_enabled = value;
}

void Settings::reset()
{
	lang_name = App::getSystemLang();
	sound_enabled = true;
	music_enabled = true;
	game_finished = false;
	resetLevelAwards();
}

void Settings::resetLevelAwards()
{
#if 1
	for(int i = 0; i < MODES_NUMBER; i++){
		mode_access_type[i] = MODE_ENABLED;
	}
#else
	mode_access_type[0] = MODE_ENABLED;
	for(int i = 1; i < MODES_NUMBER; i++){
		mode_access_type[i] = MODE_DISABLED;
	}
#endif
	mode = 0;
	for(int i = 0; i < MODES_NUMBER; i++){
		for(int j = 0; j < LEVELS_NUMBER; j++){
			level_awards[i][j] = LevelAward();
		}
	}
}

String Settings::dumpJsonToString(JSON * json, int depth)
{
	if(json){
		switch(json->getType()){
		case JSON::TYPE_NULL: return "null";
		case JSON::TYPE_FALSE: return "false";
		case JSON::TYPE_TRUE: return "true";
		case JSON::TYPE_NUMBER: return json->getString(); // String(json->getFloat(), 10);
		case JSON::TYPE_STRING: return "\""+json->getString().Replace("\"", "\\\"")+"\"";
		}
		String out;
		String depth_str;
		for(int i = 0; i < depth; i++){
			depth_str.Append("  ");
		}
		bool is_array = json->getType() == JSON::TYPE_ARRAY;
		out.Append(is_array ? "[\n" : "{\n");
		{
			int count = json->getChildrenCount();
			for(int i = 0; i < count; i++){
				JSON * child = json->getChildAtIndex(i);
				out.Append(depth_str);
				out.Append("  ");
				if(!is_array){
					out.Append( "\""+child->getName().Replace("\"", "\\\"")+"\": " );
				}
				out.Append( dumpJsonToString(child, depth+1) );
				out.Append(i+1 < count ? ",\n" : "\n");
			}
		}
		out.Append(depth_str);
		out.Append(is_array ? "]" : "}");
		return out;
	}
	return String();
}

void Settings::safeToFile(JSON * json, const char * filename)
{
	String data = dumpJsonToString(json);
	App::writeFile(filename, data);
}

void Settings::load()
{
	reset();
	String filename = String(CCFileUtils::getWriteablePath().c_str()) + settings_filename;
	if(!App::isFileExist(filename)){
		return;
	}
	JSON * config = App::getJSON(filename);
	if(config){
		lang_name = config->getChildString("lang", lang_name);
		mode = config->getChildInt("mode", 0);
		sound_enabled = config->getChildInt("sound", 1) != 0;
		music_enabled = config->getChildInt("music", 1) != 0;
		game_finished = config->getChildInt("game_finished", 0) != 0;

		for(int i = 0; i < MODES_NUMBER; i++){
			mode_access_type[i] = (EModeAccessType)config->getChildInt(String::Format("mode-%d-access-type", i+1), (int)mode_access_type[i]);
			JSON * levels = config->getChild(String::Format("mode-%d-levels", i+1));
			if(levels){
				for(int j = 0; j < LEVELS_NUMBER; j++){
					LevelAward level_award;
					level_award.sec = levels->getSubChildInt(String(j), "sec");
					level_awards[i][j] = level_award;
				}
			}
		}
	}
	for(int i = 1; i < MODES_NUMBER; i++){
		if(!isModeEnabled(i) && checkModeEnabled(i)){
			mode_access_type[i] = MODE_JUST_ENABLED;
		}
	}
	if(App::isAdmobVersion() && mode == Settings::MODES_NUMBER-1){
		setMode(mode-1);
	}
}

void Settings::save()
{
	JSON * config = new JSON(JSON::TYPE_OBJECT);
	config->addChild(new JSON("lang", lang_name));
	config->addChild(new JSON("mode", mode));
	config->addChild(new JSON("sound", (int)sound_enabled));
	config->addChild(new JSON("music", (int)music_enabled));
	config->addChild(new JSON("game_finished", (int)game_finished));
	{
		for(int i = 0; i < MODES_NUMBER; i++){
			config->addChild(new JSON(String::Format("mode-%d-access-type", i+1), (int)mode_access_type[i]));
			JSON * levels = new JSON(String::Format("mode-%d-levels", i+1), JSON::TYPE_OBJECT);
			for(int j = 0; j < LEVELS_NUMBER; j++){
				const LevelAward& level_award = level_awards[i][j];
				JSON * level_row = new JSON(String(j), JSON::TYPE_OBJECT);
				level_row->addChild(new JSON("sec", level_award.sec));
				levels->addChild(level_row);
			}
			config->addChild(levels);
		}
	}
	safeToFile(config, String(CCFileUtils::getWriteablePath().c_str()) + settings_filename);
}

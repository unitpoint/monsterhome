#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>

#include <IwGeomFVec2.h>
#include <cocos2d.h>
#include <CCApplication.h>
#include <CCTransition.h>
#include <CCActionEase.h>
#include <CCFileUtils.h>
#include <SimpleAudioEngine.h>
#include <Curve.h>
// #include <cJSON.h>
// #include <Json.h>

#include <ExtCommon.h>
#include <xString.h>
#include <MathLib.h>
#include <EasyLib.h>

#ifdef GAME_FREE_VERSION
#include <AdmobAds.h>
#endif

// using namespace std;
USING_NS_CC;
using namespace CocosDenshion;

// #define fvec2 CIwFVec2
// #define vec2 CIwVec2
// #define vec2 CIwFVec2
#define ivec2 CIwVec2
// #define String XString
typedef XString String;

#endif // __APP_COMMON_H__
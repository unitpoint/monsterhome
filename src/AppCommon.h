#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

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
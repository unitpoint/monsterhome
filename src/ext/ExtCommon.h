#ifndef __EXT_COMMON_H__
#define __EXT_COMMON_H__

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

#define ARR_SIZE(a) (int)(sizeof(a)/sizeof((a)[0]))

#ifndef ASSERT
#define ASSERT(a) CCAssert((a), "")
#endif

#ifndef DNEW
#define DNEW new
#endif

#ifndef DDELETE
#define DDELETE(a) delete (a)
#endif

#ifndef DNEWARR
#define DNEWARR(t, l) new t[l]
#endif

#ifndef DDELETEARR
#define DDELETEARR(a) delete [] (a)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY DDELETEARR
#endif

#ifndef Vector
#define Vector std::vector
#endif

#endif // __EXT_COMMON_H__
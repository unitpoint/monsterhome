#ifndef __MATH_LIB_H__
#define __MATH_LIB_H__

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

#include <ExtCommon.h>

// #define MATHLIB_SIMULATE

#define IS_FLOAT_SIGN_BIT_SET(f) ((*(const unsigned int*)&(f)) & (1U << 31U))
// #define FLOAT_ZERO 0.000000001f

// from std lib float.h
#ifndef DBL_DIG

#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */

#define FLT_DIG         6                       /* # of decimal digits of precision */
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_GUARD       0
#define FLT_MANT_DIG    24                      /* # of bits in mantissa */
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MAX_10_EXP  38                      /* max decimal exponent */
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MIN_10_EXP  (-37)                   /* min decimal exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_NORMALIZE   0
#define FLT_RADIX       2                       /* exponent radix */
#define FLT_ROUNDS      1                       /* addition rounding: near */

#endif

#define	SIDE_ON					0
#define	SIDE_FRONT				1
#define	SIDE_BACK				2
#define	SIDE_CROSS				3

#define	SIDE_ON_EPSILON		0.001f

struct vec2;
struct vec3;
struct Plane;

/**
*  Math lib class.
*/
class MathLib
{
public:

	/*
	static const float PI;
	static const float TWO_PI;
	static const float HALF_PI;
	*/

	static const float DEG2RAD;
	static const float RAD2DEG;

	static float InvSqrt(float x);
	static float Sqrt(float x) { return x * InvSqrt(x); }

	static float Sqr(float x) { return x * x; }

	static float NormAngle(float angle);
	static float NormAngle180(float angle);
	static float AnglesDiff(float a, float b);

	static float Sin(float angle);
	static float Cos(float angle);
	static void SinCos(float angle, float& sv, float& cv);

	static float Tan(float angle);

	static float ArcSin(float a);
	static float ArcCos(float a);

	static float ArcTan(float a);
	static float ArcTan(float x, float y);

	static float Pow(float x, float y);

	static float Exp(float f);
	static float Log(float f);

	static float Mod(float x, float y);

	static float Frexp(float x, int * eptr);
	static float Ldexp(float x, int exp);

	static float Floor(float x);
	static float Ceil(float x);

	static bool IsNan(float x);

	static inline float Abs(float f)
	{
		int32 tmp = *(int32*)&f;
		tmp &= 0x7FFFFFFF;
		return *(float*)&tmp;
	}

	static float EpsRound(float f, float eps); // floor(f / eps + 0.5f) * eps
	static float EpsFloor(float f, float eps); // floor(f / eps) * eps
	static int Round(float f);

	static int FloorPowerOfTwo(int);
	static int CeilPowerOfTwo(int);
	static bool IsPowerOfTwo(int);

	template <class T>
	static inline T Clamp(const T& f, const T& a, const T& b)
	{
		if(f < a)
			return a;
		if(f > b)
			return b;
		return f;
	}

	template <class T>
	static inline T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template <class T>
	static inline T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	static inline float Sign(float f)
	{
		if(f < 0.0f) return -1.0f;
		if(f > 0.0f) return 1.0f;
		return 0.0f;
	}

	// static inline float Min(float a, float b) { return a < b ? a : b; }
	// static inline float Max(float a, float b) { return a > b ? a : b; }

	template <class T>
	static inline T Lerp(const T& a, const T& b, float t) { return a * (1.0f - t) + b * t; }

	static bool RayTriangleIntersection(const vec3& start, const vec3& dir, float rayLen, 
		const Plane& triPlane, const vec3& p1, const vec3& p2, const vec3& p3,
		vec3& contact, float& len, 
		float fronsideEpsilon, float backsideEpsilon,
		float frontsideStartBorder, float frontsideEndBorder);

	static bool RayTriangleIntersection(const vec3& start, const vec3& end,
		const Plane& triPlane, const vec3& p1, const vec3& p2, const vec3& p3,
		vec3& contact, float& dot);

	static bool SphereRayTriangleIntersection(
		const vec3& start, const vec3& end, const vec3& dir, float radius, 
		const Plane& triPlane, 
		const vec3& p1, const vec3& p2, const vec3& p3,
		vec3& contact, float& dot);
};

// ===================================================================================

struct vec3;

/**
*  2d vector class.
*/

struct vec2
{
	float x; ///< x position
	float y; ///< y position

	vec2(){ x = y = 0; }
	vec2(float ax, float ay)
	{
		x = ax;
		y = ay;
	}
	vec2(const vec3&);
	vec2(const cocos2d::CCPoint& p)
	{
		x = p.x;
		y = p.y;
	}
	vec2(const cocos2d::CCSize& p)
	{
		x = p.width;
		y = p.height;
	}

	float At(int i) const
	{
		ASSERT(i >= 0 && i < 2);
		return (&x)[i];
	}
	void Set(int i, float f)
	{
		ASSERT(i >= 0 && i < 2);
		(&x)[i] = f;
	}

	operator cocos2d::CCPoint () const
	{
		return ccp(x, y);
	}

	operator cocos2d::CCSize() const
	{
		return cocos2d::CCSize(x, y);
	}

	bool operator==(const vec2& b) const
	{
		return x == b.x && y == b.y;
	}

	bool operator!=(const vec2& b) const
	{
		return x != b.x || y != b.y;
	}

	vec2 operator+(const vec2& b) const
	{
		return vec2(x+b.x, y+b.y);
	}
	vec2 operator+(float b) const
	{
		return vec2(x+b, y+b);
	}
	vec2& operator+=(const vec2& b)
	{
		x += b.x;
		y += b.y;
		return *this;
	}

	vec2 operator-(const vec2& b) const
	{
		return vec2(x-b.x, y-b.y);
	}
	vec2 operator-(float b) const
	{
		return vec2(x-b, y-b);
	}
	/* friend class vec2 operator-(float a, const vec2& b) const
	{
	return vec2(a-b.x, a-b.y, a-b.z);
	} */

	vec2 operator*(const vec2& b) const
	{
		return vec2(x*b.x, y*b.y);
	}

	vec2 operator*(float b) const
	{
		return vec2(x*b, y*b);
	}
	friend struct vec2 operator * (float a, const vec2& b)
	{
		return vec2(b.x*a, b.y*a);
	}

	vec2 operator*(int b) const
	{
		return *this * (float)b;
	}
	friend struct vec2 operator * (int a, const vec2& b)
	{
		return b * (float)a;
	}

	vec2 operator/(const vec2& b) const
	{
		return vec2(x/b.x, y/b.y);
	}
	vec2 operator/(float b) const
	{
		b = 1.0f / b;
		return vec2(x*b, y*b);
	}

	vec2 operator-() const
	{
		return vec2(-x, -y);
	}

	float Dot(const vec2& b) const
	{
		return x*b.x + y*b.y;
	}

	float InvLen() const
	{
		return MathLib::InvSqrt(Dot(*this));
	}
	float Len() const
	{
		return MathLib::Sqrt(Dot(*this));
	}
	float SqrLen() const
	{
		return Dot(*this);
	}

	vec2 Norm() const
	{
		return *this * InvLen();
	}

	float LenNorm()
	{
		float sqrLen = Dot(*this);
		float invLen = MathLib::InvSqrt(sqrLen);
		x *= invLen;
		y *= invLen;
		return sqrLen * invLen;
	}

	float LenNorm(vec2& dir)
	{
		float sqrLen = Dot(*this);
		float invLen = MathLib::InvSqrt(sqrLen);
		dir.x = x * invLen;
		dir.y = y * invLen;
		return sqrLen * invLen;
	}

	vec2 Min(const vec2& b) const
	{
		return vec2(MathLib::Min(x, b.x), MathLib::Min(y, b.y));
	}
	float Min() const
	{
		return MathLib::Min(x ,y);
	}

	vec2 Max(const vec2& b) const
	{
		return vec2(MathLib::Max(x, b.x), MathLib::Max(y, b.y));
	}
	float Max() const
	{
		return MathLib::Max(x ,y);
	}
};

// ===================================================================================

/**
*  3d vector class.
*/

struct vec3
{
	float x;  ///< x pos
	float y;  ///< y pos
	float z;  ///< z pos

	vec3(){ x = y = z = 0; }
	vec3(float ax, float ay, float az)
	{
		x = ax;
		y = ay;
		z = az;
	}
	vec3(const vec2& b)
	{
		x = b.x;
		y = b.y;
		z = 0.0f;
	}

	operator cocos2d::ccColor3B() const
	{
		return cocos2d::ccc3(
			MathLib::Clamp((int)(x * 255), 0, 255),
			MathLib::Clamp((int)(y * 255), 0, 255),
			MathLib::Clamp((int)(z * 255), 0, 255)
		);
	}

	float At(int i) const
	{
		ASSERT(i >= 0 && i < 3);
		return (&x)[i];
	}
	void Set(int i, float f)
	{
		ASSERT(i >= 0 && i < 3);
		(&x)[i] = f;
	}

	bool FixDegenerateNormal();

	bool operator==(const vec3& b) const
	{
		return x == b.x && y == b.y && z == b.z;
	}

	bool operator!=(const vec3& b) const
	{
		return x != b.x || y != b.y || z != b.z;
	}

	vec3 operator+(const vec3& b) const
	{
		return vec3(x+b.x, y+b.y, z+b.z);
	}
	vec3 operator+(float b) const
	{
		return vec3(x+b, y+b, z+b);
	}

	vec3 operator-(const vec3& b) const
	{
		return vec3(x-b.x, y-b.y, z-b.z);
	}
	vec3 operator-(float b) const
	{
		return vec3(x-b, y-b, z-b);
	}
	/* friend class vec3 operator-(float a, const vec3& b) const
	{
	return vec3(a-b.x, a-b.y, a-b.z);
	} */

	vec3 operator*(const vec3& b) const
	{
		return vec3(x*b.x, y*b.y, z*b.z);
	}
	vec3 operator*(float b) const
	{
		return vec3(x*b, y*b, z*b);
	}

	vec3 operator/(const vec3& b) const
	{
		return vec3(x/b.x, y/b.y, z/b.z);
	}
	vec3 operator/(float b) const
	{
		b = 1.0f / b;
		return vec3(x*b, y*b, z*b);
	}

	vec3 operator-() const
	{
		return vec3(-x, -y, -z);
	}

	float Dot(const vec3& b) const
	{
		return x*b.x + y*b.y + z*b.z;
	}
	vec3 Cross(const vec3& b) const
	{
		return vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	}

	float InvLen() const
	{
		return MathLib::InvSqrt(Dot(*this));
	}
	float Len() const
	{
		return MathLib::Sqrt(Dot(*this));
	}
	float SqrLen() const
	{
		return Dot(*this);
	}

	vec3 Norm() const
	{
		return *this * InvLen();
	}

	float LenNorm(vec3& dir) const
	{
		float sqrLen = Dot(*this);
		float invLen = MathLib::InvSqrt(sqrLen);
		dir.x = x * invLen;
		dir.y = y * invLen;
		dir.z = z * invLen;
		return sqrLen * invLen;
	}

	vec3 Min(const vec3& b)
	{
		return vec3(MathLib::Min(x, b.x), MathLib::Min(y, b.y), MathLib::Min(z, b.z));
	}

	vec3 Max(const vec3& b)
	{
		return vec3(MathLib::Max(x, b.x), MathLib::Max(y, b.y), MathLib::Max(z, b.z));
	}

	vec3 Neg() const
	{
		return vec3( -x, -y, -z );
	}
};

inline vec2::vec2(const vec3& p)
{
	x = p.x;
	y = p.y;
}

// ===================================================================================

/**
*  4d vector class.
*/

struct vec4
{
	float x;  ///< x pos
	float y;  ///< y pos
	float z;  ///< z pos
	float w;  ///< w pos

	vec4(){ x = y = z = w = 0; }
	vec4(float ax, float ay, float az, float aw)
	{
		x = ax;
		y = ay;
		z = az;
		w = aw;
	}

	float At(int i) const
	{
		ASSERT(i >= 0 && i < 4);
		return (&x)[i];
	}
	void Set(int i, float f)
	{
		ASSERT(i >= 0 && i < 4);
		(&x)[i] = f;
	}
};

// ===================================================================================

struct Rect2
{
	vec2 origin;
	vec2 size;

	Rect2(){}
	Rect2(float x, float y, float width, float height): origin(x, y), size(width, height){}
	Rect2(const vec2& p_origin, const vec2& p_size): origin(p_origin), size(p_size){}
	Rect2(const cocos2d::CCRect& rect): origin(rect.origin), size(rect.size){}

	Rect2& Expand(const vec2& s)
	{
		origin = origin - s;
		size = size + s * 2.0f;
		return *this;
	}

	operator cocos2d::CCRect() const 
	{
		return cocos2d::CCRect(origin.x, origin.y, size.x, size.y);
	}

	bool ContainsPoint(const vec2& p) const
	{
		return p.x >= origin.x && p.y >= origin.y
			&& p.x < origin.x + size.x && p.y < origin.y + size.y;
	}

	bool ContainsRect(const Rect2& rect) const 
	{
		return rect.origin.x >= origin.x && rect.origin.y >= origin.y
			&& rect.origin.x + rect.size.x <= origin.x + size.x
			&& rect.origin.y + rect.size.y <= origin.y + size.y;
	}

	bool IntersectRect(const Rect2& rect) const 
	{
		if(origin.x >= rect.origin.x + rect.size.x || origin.x + size.x <= rect.origin.x
			|| origin.y >= rect.origin.y + rect.size.y || origin.y + size.y <= rect.origin.y)
		{
			return false;
		}
		return true;
	}
};

/**
*  2d bounds class.
*/

struct Bounds2
{
	vec2 b[2];  ///< min & max bounds

	Bounds2(){}
	Bounds2(const vec2& p_a, const vec2& p_b) { b[0] = p_a; b[1] = p_b; }

	Bounds2& Init(const vec2& p)
	{
		b[0] = p;
		b[1] = p;
		return *this;
	}

	Bounds2& Add(const vec2& p)
	{
		b[0] = b[0].Min(p);
		b[1] = b[1].Max(p);
		return *this;
	}

	Bounds2& Add(const Bounds2& a)
	{
		b[0] = b[0].Min(a.b[0]);
		b[1] = b[1].Max(a.b[1]);
		return *this;
	}

	void Expand(float f)
	{
		b[0] = b[0] - f;
		b[1] = b[1] + f;
	}

	bool ContainsPoint(const vec2& p) const
	{
		return p.x >= b[0].x && p.y >= b[0].y
			&& p.x <= b[1].x && p.y <= b[1].y;
	}

	bool ContainsBounds(const Bounds2& a) const
	{
		if(a.b[0].x >= b[0].x && a.b[0].y >= b[0].y
			&& a.b[1].y <= b[1].y && a.b[1].y <= b[1].y
			)
		{
			return true;
		}
		return false;
	}

	bool IntersectsBounds(const Bounds2& a) const
	{
		if(a.b[1].x < b[0].x || a.b[1].y < b[0].y 
			|| a.b[0].x > b[1].x || a.b[0].y > b[1].y)
		{
			return false;
		}
		return true;
	}
};

// ===================================================================================

/**
*  3d bounds class.
*/

struct Bounds3
{
	vec3 b[2];  ///< min & max bounds

	Bounds3(){}
	Bounds3(const vec3& p_a, const vec3& p_b) { b[0] = p_a; b[1] = p_b; }

	Bounds3& Init(const vec3& p)
	{
		b[0] = p;
		b[1] = p;
		return *this;
	}

	Bounds3& Add(const vec3& p)
	{
		b[0] = b[0].Min(p);
		b[1] = b[1].Max(p);
		return *this;
	}

	Bounds3& Add(const Bounds3& a)
	{
		b[0] = b[0].Min(a.b[0]);
		b[1] = b[1].Max(a.b[1]);
		return *this;
	}

	void Expand(float f)
	{
		b[0] = b[0] - f;
		b[1] = b[1] + f;
	}

	bool ContainsPoint(const vec3& p) const
	{
		return p.x >= b[0].x && p.y >= b[0].y && p.z >= b[0].z
			&& p.x <= b[1].x && p.y <= b[1].y && p.z <= b[1].z;
	}

	bool ContainsBounds(const Bounds3& a) const
	{
		if(a.b[0].x >= b[0].x && a.b[0].y >= b[0].y && a.b[0].z >= b[0].z
			&& a.b[1].y <= b[1].y && a.b[1].y <= b[1].y && a.b[1].z <= b[1].z
			)
		{
			return true;
		}
		return false;
	}

	bool IntersectsBounds(const Bounds3& a) const
	{
		if(a.b[1].x < b[0].x || a.b[1].y < b[0].y || a.b[1].z < b[0].z
			|| a.b[0].x > b[1].x || a.b[0].y > b[1].y || a.b[0].z > b[1].z)
		{
			return false;
		}
		return true;
	}

	int PlaneSide(const Plane& plane, float epsilon = SIDE_ON_EPSILON) const;
};

// ===================================================================================

struct Plane
{
	vec3 normal;
	float dist;

	Plane(){ dist = 0; }
	Plane(float a, float b, float c, float d): normal(a,b,c), dist(d){}
	Plane(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal = true);
	Plane(const vec3& n, const vec3& p){ Init(n, p); }

	void Init(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal = true);
	void Init(const vec3& n, const vec3& p);

	int PlaneAxeNum() const;
	float Dist(const vec3& p) const;
	int Side(const vec3& p) const;

	bool CompareEps(const Plane& b, float distEps, float minNormalDot = 0.98f) const;

	Plane Neg() const { return Plane(-normal.x, -normal.y, -normal.z, -dist); }

	bool RayIntersection(const vec3& start, const vec3& dir, float& scale) const;
	bool SplitLine(const vec3& start, const vec3& end, vec3& mid, float& dot) const;

	static vec3 Reflect(const vec3& dirToPlane, const vec3& normal);
};

// ===================================================================================

struct Angles;

/**
*  3d normalized vectors class.
*/

struct Vectors
{
	vec3 forward, right, up;

	Vectors(){}
	Vectors(const vec3& f, const vec3& r, const vec3& u)
	{
		forward = f;
		right = r;
		up = u;
	}
	Vectors(const vec3& f, const vec3& u);
	Vectors(const Angles& a);

	vec3& At(int i)
	{
		ASSERT(i >= 0 && i < 3);
		return (&forward)[i];
	}
	const vec3& At(int i) const
	{
		ASSERT(i >= 0 && i < 3);
		return (&forward)[i];
	}
	float At(int i, int j) const { return At(i).At(j); }

	bool FixDegenerateNormal();

	Angles& ToAngles(Angles& out) const;
};

// ===================================================================================

#define VEC3_ZERO     vec3(0.0f, 0.0f, 0.0f)  ///< 3d zero vector

#define VEC3_FORWARD  vec3(0.0f, 1.0f, 0.0f)  ///< 3d forward vector
#define VEC3_RIGHT    vec3(1.0f, 0.0f, 0.0f)  ///< 3d right vector
#define VEC3_UP       vec3(0.0f, 0.0f, 1.0f)  ///< 3d up vector

/**
*  Angles class.
*/

struct Angles
{
	float yaw;    ///< yaw angle in grad
	float pitch;  ///< pitch angle in grad
	float roll;   ///< roll angle in grad

	Angles(float p_yaw = 0, float p_pitch = 0, float p_roll = 0)
	{
		yaw = p_yaw;
		pitch = p_pitch;
		roll = p_roll;
	}
	Angles(const vec3& forward, const vec3& up);

	Angles& SetDiff(const Angles& a, const Angles& b);

	vec3 Forward() const;

	Vectors& ToVectors(Vectors& out) const;

	float& At(int i)
	{
		ASSERT(i >= 0 && i < 3);
		return (&yaw)[i];
	}
	const float At(int i) const
	{
		ASSERT(i >= 0 && i < 3);
		return (&yaw)[i];
	}

	Angles SlerpTo(const Angles& to, float t) const;
	Angles SlerpByDiff(const Angles& diff, float t) const;
};

// ===================================================================================

/**
*  4x4 matrix class.
*/

struct Mat4x4
{
	vec4 rows[4]; ///< matrix rows

	Mat4x4(float s = 1);
	Mat4x4(const Vectors& v, const vec3& pos);
	Mat4x4(const Angles& a, const vec3& pos);

	void Ident(float s = 1);

	Vectors& ToVectors(Vectors& out) const;
	void SetVectors(const Vectors& v);
	void SetVectors(const Vectors& v, const vec3& pos);

	Angles& ToAngles(Angles& out) const;
	void SetAngles(const Angles& a);
	void SetAngles(const Angles& a, const vec3& pos);

	vec3 Translation() const;
	void SetTranslation(const vec3& pos);

	void Scale(const vec3& s);

	vec3 RotateFast(const vec3& v) const;
	vec3 TransformFast(const vec3& p) const;
};

// ===================================================================================

class RandomValue
{
public:

	enum
	{
		MAX_RAND = 0x7fffffff
	};

	RandomValue(uint32 s = 0){ seed = s; }

	int Seed() const { return (int)seed; }
	void SetSeed(int s){ seed = (uint32)s; }

	int Int();        // random integer in the range [0, MAX_RAND] 
	int Int(int max); // random integer in the range [0, max[ 
	float Float();    // random float in the range [0, 1.0f] 
	float CFloat();   // random float in the range [-1.0f, 1.0f] 

protected:

	uint32 seed;
};

// ===================================================================================

#endif // __MATH_LIB_H__

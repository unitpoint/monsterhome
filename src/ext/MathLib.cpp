//============================================================================
//
// Author: Evgeniy Golovin, egolovin@superscape.com
//
//============================================================================

#include <MathLib.h>
// #include <SwerveHelper.h>

#define IEEE_FLT_MANTISSA_BITS  23
#define IEEE_FLT_EXPONENT_BITS  8
#define IEEE_FLT_EXPONENT_BIAS  127
#define IEEE_FLT_SIGN_BIT       31

// const float PI = 3.14159265358979323846f;
// const float MathLib::TWO_PI = 6.28318530718f; // PI * 2
// const float (PI/2.0f) = 1.570796326795f; // PI / 2

const float MathLib::DEG2RAD = 0.01745329251994f; // PI / 180.0f
const float MathLib::RAD2DEG = 57.29577951308f; // 180.0f / PI

float MathLib::InvSqrt(float x)
{
	float y = x * 0.5f;
	long i = *(long*)&x;
	i = 0x5f3759df - (i >> 1);
	float r = *(float*)&i;
	r = r * (1.5f - r * r * y);
#if 0 && defined MATHLIB_SIMULATE
	// if x == 0 then return huge number
	r = r * (1.5f - r * r * y);
#endif
	return r;
}

float MathLib::NormAngle(float angle)
{
	if(IS_FLOAT_SIGN_BIT_SET(angle))
	{
		return 360.0f + angle - (float)(int(angle / 360.0f) * 360);
	}
	if(angle >= 360.0f)
	{
		return angle - (float)(int(angle / 360.0f) * 360);
	}
	return angle;
}

float MathLib::NormAngle180(float angle)
{
	angle = NormAngle(angle);
	return angle > 180.0f ? angle - 360.0f : angle;
}

float MathLib::AnglesDiff(float a, float b)
{
	a = NormAngle(a);
	b = NormAngle(b);
	float diff = b - a;
	if(diff > 180.0f)
	{
		return diff - 360.0f;
	}
	if(diff < -180.0f)
	{
		return diff + 360.0f;
	}
	return diff;
}

float MathLib::Sin(float angle)
{
#ifndef MATHLIB_SIMULATE
	return sinf(angle * DEG2RAD);
#else
	angle = NormAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
		}
		else
		{
			angle = 180 - angle;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;

#ifdef DEVICE_LOW_SPEED
	return a * ((8.3333315e-03f * s - 1.666666664e-01f) * s + 1.0f); 
#else
	return a * (((((-2.39e-08f * s + 2.7526e-06f) * s - 1.98409e-04f) * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	// return a * (((1.98409e-04f * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
#endif
#endif // MATHLIB_SIMULATE
}

float MathLib::Cos(float angle)
{
#ifndef MATHLIB_SIMULATE
	return cosf(angle * DEG2RAD);
#else
	float d;
	angle = NormAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
			d = -1.0f;
		}
		else
		{
			d = 1.0f;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
			d = 1.0f;
		}
		else
		{
			angle = 180 - angle;
			d = -1.0f;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;
#ifdef DEVICE_LOW_SPEED
	return d * ((4.16666418e-02f * s - 4.999999963e-01f) * s + 1.0f);
#else
	return d * (((((-2.605e-07f * s + 2.47609e-05f) * s - 1.3888397e-03f) * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
	// return d * (((1.3888397e-03f * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
#endif
#endif // MATHLIB_SIMULATE
}

void MathLib::SinCos(float angle, float& sv, float& cv)
{
#ifndef MATHLIB_SIMULATE
	float t = angle * DEG2RAD;
	sv = sinf(t);
	cv = cosf(t);
#else
	float d;
	angle = NormAngle(angle);
	if(angle < 180)
	{
		if(angle > 90)
		{
			angle = 180 - angle;
			d = -1.0f;
		}
		else
		{
			d = 1.0f;
		}
	}
	else
	{
		if(angle > 270)
		{
			angle -= 360;
			d = 1.0f;
		}
		else
		{
			angle = 180 - angle;
			d = -1.0f;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;

#ifdef DEVICE_LOW_SPEED
	sv = a * ((8.3333315e-03f * s - 1.666666664e-01f) * s + 1.0f); 
	cv = d * ((4.16666418e-02f * s - 4.999999963e-01f) * s + 1.0f);
#else
	sv = a * (((((-2.39e-08f * s + 2.7526e-06f) * s - 1.98409e-04f) * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	cv = d * (((((-2.605e-07f * s + 2.47609e-05f) * s - 1.3888397e-03f) * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
	// sv = a * (((1.98409e-04f * s + 8.3333315e-03f) * s - 1.666666664e-01f) * s + 1.0f);
	// cv = d * (((1.3888397e-03f * s + 4.16666418e-02f) * s - 4.999999963e-01f) * s + 1.0f);
#endif
#endif // MATHLIB_SIMULATE
}

float MathLib::Tan(float angle)
{
#ifndef MATHLIB_SIMULATE
	return tanf(angle * DEG2RAD);
#else
	// return Sin(angle) / Cos(angle);
	bool reciprocal;
	angle = NormAngle(angle);
	if(angle < 90)
	{
		if(angle > 45)
		{
			angle = 90 - angle;
			reciprocal = true;
		}
		else
		{
			reciprocal = false;
		}
	}
	else
	{
		if(angle > 90+45)
		{
			angle -= 180;
			reciprocal = false;
		}
		else
		{
			angle = 90 - angle;
			reciprocal = true;
		}
	}
	float a = angle * DEG2RAD;
	float s = a * a;
#ifdef DEVICE_LOW_SPEED
	s = a * ((1.333923995e-01f * s + 3.333314036e-01f) * s + 1.0f);
#else
	// s = a * ((((((9.5168091e-03f * s + 2.900525e-03f) * s + 2.45650893e-02f) * s + 5.33740603e-02f) * s + 1.333923995e-01f) * s + 3.333314036e-01f) * s + 1.0f);
	s = a * (((5.33740603e-02f * s + 1.333923995e-01f) * s + 3.333314036e-01f) * s + 1.0f);
#endif
	return reciprocal ? 1.0f / s : s;
#endif // MATHLIB_SIMULATE
}

float MathLib::ArcSin(float a)
{
#ifndef MATHLIB_SIMULATE
	return asinf(a) * RAD2DEG;
#else
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
		if(a <= -1.0f)
		{
			return -90;
		}
		a = -a;
#ifdef DEVICE_LOW_SPEED
		return ((0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a) - (PI/2.0f)) * RAD2DEG;
#else
		return ((((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a) - (PI/2.0f)) * RAD2DEG;
		// return (((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a) - (PI/2.0f)) * RAD2DEG;
#endif
	}
	if(a >= 1.0f)
	{
		return 90;
	}
#ifdef DEVICE_LOW_SPEED
	return ((PI/2.0f) - (0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
	return ((PI/2.0f) - (((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
	// return ((PI/2.0f) - ((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#endif
#endif // MATHLIB_SIMULATE
}

float MathLib::ArcCos(float a)
{
#ifndef MATHLIB_SIMULATE
	return acosf(a) * RAD2DEG;
#else
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
		if(a <= -1.0f)
		{
			return 180;
		}
		a = -a;
#ifdef DEVICE_LOW_SPEED
		return (PI - (0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
		return (PI - (((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
		// return (PI - ((0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a) * RAD2DEG;
#endif
	}
	if(a >= 1.0f)
	{
		return 0.0f;
	}
#ifdef DEVICE_LOW_SPEED
	return ((0.2121144f * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#else
	return ((((-0.0187293f * a + 0.0742610f) * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
	// return (((0.0742610f * a - 0.2121144f) * a + 1.5707288f) * Sqrt(1.0f - a)) * RAD2DEG;
#endif
#endif // MATHLIB_SIMULATE
}

// =============================================================================
// =============================================================================
// =============================================================================

#define FLT_ZERO_EPSILON (1e-06f)

#ifdef MATHLIB_SIMULATE

inline static bool util_fcmp_g (float a, float b)  {return a - b > FLT_ZERO_EPSILON;}
inline static bool util_fcmp_ge(float a, float b) {return a - b > -FLT_ZERO_EPSILON;}
inline static bool util_fcmp_l (float a, float b)  {return util_fcmp_g(b, a);}
inline static bool util_fcmp_le(float a, float b) {return util_fcmp_ge(b, a);}
inline static bool util_fcmp_e (float a, float b)  {return MathLib::Abs(a - b) < FLT_ZERO_EPSILON;}
inline static bool util_fcmp_ne(float a, float b) {return util_fcmp_e(a, b) == false;}

//= snake ======================================================================
// defined on [0..1]
inline static float util_fastatan(float x)
{
	return x*(1.0f + x*(0.010413f - x*(0.411333f - x*(0.21537f - x*0.028779f))));
}

//= snake ======================================================================
inline static float util_atan(float a)
{
	float result;
	float aa = MathLib::Abs(a);
	if(a >= -1 && a <= 1)
	{
		result = util_fastatan(aa);
	}
	else
	{
		result = (PI/2.0f) - util_fastatan(1 / aa);
	}
	result *= MathLib::RAD2DEG;
	return a >= 0 ? result : -result;
}

//= snake ======================================================================
// returns angle in (-180..180)
inline static float util_atan2(float t_y, float t_x)
{
	if(util_fcmp_e(t_x, 0))
	{
		if(util_fcmp_e(t_y, 0))
		{
			return 0;
		}
		else if(t_y > 0)
		{
			return 90;
		}
		else
		{
			return -90;
		}
	}
	else if(util_fcmp_e(t_y, 0))
	{
		if(t_x > 0)
		{
			return 0;
		}
		else
		{
			return 180;
		}
	}
	else
	{
		float ax = MathLib::Abs(t_x);
		float ay = MathLib::Abs(t_y);

		float result;

		if(ax > ay)
		{
			result = util_fastatan(ay / ax);
			if (t_x < 0)
			{
				result = PI - result;
			}
		}
		else
		{
			if(t_x >= 0)
			{
				result = (PI/2.0f) - util_fastatan(ax / ay);
			}
			else
			{
				result = (PI/2.0f) + util_fastatan(ax / ay);
			}
		}

		result *= MathLib::RAD2DEG;
		return t_y >= 0 ? result : -result;
	}
}
#endif // MATHLIB_SIMULATE

// =============================================================================
// =============================================================================
// =============================================================================

float MathLib::ArcTan(float a)
{
#ifndef MATHLIB_SIMULATE
	return atanf(a) * RAD2DEG;
#else
	return util_atan(a);
	/*
	float s;
	if(Abs(a) > 1.0f)
	{
	a = 1.0f / a;
	s = a * a;
	#ifdef DEVICE_LOW_SPEED
	s = - ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	#else
	s = - (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
	* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	// s = - (((((((0.0429096138f * s - 0.0752896400f)
	//     * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
	#endif
	if(IS_FLOAT_SIGN_BIT_SET(a))
	{
	return (s - (PI/2.0f)) * RAD2DEG;
	}
	return (s + (PI/2.0f)) * RAD2DEG;
	}
	s = a * a;
	#ifdef DEVICE_LOW_SPEED
	return (((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	#else
	return ((((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
	* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	// return ((((((((0.0429096138f * s - 0.0752896400f)
	//    * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a) * RAD2DEG;
	#endif
	*/
#endif // MATHLIB_SIMULATE
}

/*
float MathLib::__ArcTan(float x, float y)
{
float a, s;
if(Abs(y) > Abs(x))
{
a = x / y;
s = a * a;
#ifdef DEVICE_LOW_SPEED
s = ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#else
s = (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
// s = - (((((((0.0429096138f * s - 0.0752896400f)
//      * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#endif
return s * RAD2DEG;
}
a = y / x;
s = a * a;
#ifdef DEVICE_LOW_SPEED
s = - ((((0.1420889944f * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
#else
s = - (((((((((0.0028662257f * s - 0.0161657367f) * s + 0.0429096138f) * s - 0.0752896400f)
* s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;
// s = (((((((0.0429096138f * s - 0.0752896400f)
//   * s + 0.1065626393f) * s - 0.1420889944f) * s + 0.1999355085f) * s - 0.3333314528f) * s) + 1.0f) * a;

if(IS_FLOAT_SIGN_BIT_SET(a))
{
return (s - (PI/2.0f)) * RAD2DEG;
}
return (s + (PI/2.0f)) * RAD2DEG;
#endif
}
*/

float MathLib::ArcTan(float x, float y)
{
#ifndef MATHLIB_SIMULATE
	return atan2f(x, y) * RAD2DEG;
#else
	/*
	y = 0.5f;
	x = 1.0f;
	float f2 = atan2(x, y) * RAD2DEG;
	float f = __ArcTan(x, y);
	ASSERT(Abs(f-f2) < 0.1);
	return f;
	*/
	return util_atan2(x, y);
#endif // MATHLIB_SIMULATE
}

// =========================================================================================
// =========================================================================================
// =========================================================================================
// http://www.raspberryginger.com/jbailey/minix/html/dir_c621c1a38526b1de0afafbfeaed1eb2c.html

#ifdef MATHLIB_SIMULATE

struct f64
{
	uint32 low_word;
	uint32 high_word;
};

#define F64_SIGN_SHIFT  31
#define F64_SIGN_MASK   1

#define F64_EXP_SHIFT   20
#define F64_EXP_MASK    0x7ff
#define F64_EXP_BIAS    1023
#define F64_EXP_MAX     2047

#define F64_MANT_SHIFT  0
#define F64_MANT_MASK   0xfffff

#define F64_GET_SIGN(fp)        (((fp)->high_word >> F64_SIGN_SHIFT) & \
	F64_SIGN_MASK)
#define F64_GET_EXP(fp)         (((fp)->high_word >> F64_EXP_SHIFT) & \
	F64_EXP_MASK)
#define F64_SET_EXP(fp, val)    ((fp)->high_word= ((fp)->high_word &    \
	~(F64_EXP_MASK << F64_EXP_SHIFT)) |     \
	(((val) & F64_EXP_MASK) << F64_EXP_SHIFT))

#define F64_GET_MANT_LOW(fp)            ((fp)->low_word)
#define F64_SET_MANT_LOW(fp, val)       ((fp)->low_word= (val))
#define F64_GET_MANT_HIGH(fp)   (((fp)->high_word >> F64_MANT_SHIFT) & \
	F64_MANT_MASK)
#define F64_SET_MANT_HIGH(fp, val)      ((fp)->high_word= ((fp)->high_word & \
	~(F64_MANT_MASK << F64_MANT_SHIFT)) |   \
	(((val) & F64_MANT_MASK) << F64_MANT_SHIFT))

#endif // MATHLIB_SIMULATE

float MathLib::Frexp(float x, int * eptr)
{
#ifndef MATHLIB_SIMULATE
	return frexpf(x, eptr);
#else
	struct f64 *f64p;
	int exp, exp_bias;
	double factor;

	double value = x;
	f64p= (struct f64 *)&value;
	exp_bias= 0;

	exp= F64_GET_EXP(f64p);
	if (exp == F64_EXP_MAX)
	{       /* Either infinity or Nan */
		*eptr= 0;
		return (float)value;
	}
	if (exp == 0)
	{
		/* Either 0 or denormal */
		if (F64_GET_MANT_LOW(f64p) == 0 && F64_GET_MANT_HIGH(f64p) == 0)
		{
			*eptr= 0;
			return (float)value;
		}

		/* Multiply by 2^64 */
		factor= 65536.0;        /* 2^16 */
		factor *= factor;       /* 2^32 */
		factor *= factor;       /* 2^64 */
		value *= factor;
		exp_bias= 64;
		exp= F64_GET_EXP(f64p);
	}

	exp= exp - F64_EXP_BIAS - exp_bias + 1;
	*eptr= exp;
	F64_SET_EXP(f64p, F64_EXP_BIAS-1);

	return (float)value;
#endif // MATHLIB_SIMULATE
}

#ifdef MATHLIB_SIMULATE
#ifndef HUGE_VAL
#define HUGE_VAL __huge_val()
static double __huge_val()
{
	return DBL_MAX; // 1.0e+1000; /* This will generate a warning */
}
#endif // HUGE_VAL
#endif // MATHLIB_SIMULATE

float MathLib::Ldexp(float x, int exp)
{
#ifndef MATHLIB_SIMULATE
	return ldexpf(x, exp);
#else
	int sign = 1;
	int currexp;

	if(IsNan(x))
	{
		// errno = EDOM;
		return x;
	}
	if (x == 0.0f) return 0.0f;
	/*
	if (fl<0) {
	fl = -fl;
	sign = -1;
	}
	if (fl > DBL_MAX) { // for infinity
	// errno = ERANGE;
	return (float)(sign * fl);
	}
	*/

	if (IS_FLOAT_SIGN_BIT_SET(x)) { // fl<0) {
		x = -x;
		sign = -1;
	}
	if (x > FLT_MAX) {             /* for infinity */
		// errno = ERANGE;
		return sign * x;
	}

	double fl = Frexp(x, &currexp);
	exp += currexp;
	if (exp > 0) {
		if (exp > DBL_MAX_EXP) {
			// errno = ERANGE;
			return (float)(sign * HUGE_VAL);
		}
		while (exp>30) {
			fl *= (double) (1L << 30);
			exp -= 30;
		}
		fl *= (double) (1L << exp);
	}
	else
	{
		/* number need not be normalized */
		if (exp < DBL_MIN_EXP - DBL_MANT_DIG)
		{
			return 0.0;
		}
		while (exp<-30)
		{
			fl /= (double) (1L << 30);
			exp += 30;
		}
		fl /= (double) (1L << -exp);
	}
	return (float)(sign * fl);
#endif // MATHLIB_SIMULATE
}

bool MathLib::IsNan(float f)
{
	return (*((long*)&f) & 0x7f800000) == 0x7f800000; // && (*((long*)&f) & 0x007fffff) != 0;
}

#ifdef MATHLIB_SIMULATE
static double __modf(double value, double * iptr)
{
	struct f64 *f64p;
	double tmp;
	int exp;
	int mask_bits;
	unsigned long mant;

	f64p= (struct f64 *)&value;

	exp= F64_GET_EXP(f64p);
	exp -= F64_EXP_BIAS;
	if (exp < 0)
	{
		*iptr= 0;
		return value;
	}
	mask_bits= 52-exp;
	if (mask_bits <= 0)
	{
		*iptr= value;
		return 0;
	}
	tmp= value;
	if (mask_bits >= 32)
	{
		F64_SET_MANT_LOW(f64p, 0);
		mask_bits -= 32;
		mant= F64_GET_MANT_HIGH(f64p);
		mant &= ~((1 << mask_bits)-1);
		F64_SET_MANT_HIGH(f64p, mant);
	}
	else
	{
		mant= F64_GET_MANT_LOW(f64p);
		mant &= ~((1 << mask_bits)-1);
		F64_SET_MANT_LOW(f64p, mant);
	}
	*iptr= value;
	return tmp-value;
}
#endif // MATHLIB_SIMULATE

float MathLib::Floor(float x)
{
#ifndef MATHLIB_SIMULATE
	return floorf(x);
#else
	double val;
	return __modf(x, &val) < 0 ? (float)val - 1.0f : (float)val;
	/*
	this also works if modf always returns a positive
	fractional part
	*/
#endif // MATHLIB_SIMULATE
}

float MathLib::Ceil(float x)
{
#ifndef MATHLIB_SIMULATE
	return ceilf(x);
#else
	double val;
	return __modf(x, &val) > 0 ? (float)val + 1.0f : (float)val;
	/*
	this also works if modf always returns a positive
	fractional part
	*/
#endif // MATHLIB_SIMULATE
}

float MathLib::EpsRound(float f, float eps)
{
	return Floor(f / eps + 0.5f) * eps;
}

float MathLib::EpsFloor(float f, float eps)
{
	return Floor(f / eps) * eps;
}

int MathLib::Round(float f)
{
	return (int)(f + 0.5f);
}

float MathLib::Mod(float x, float y)
{
#ifndef MATHLIB_SIMULATE
	if(y == 0)
	{
		// errno = EDOM;
		return 0;
	}
	double val;
	float frac = (float)modf(x / y, &val);
	return frac * y;
#else
#if 1
	if(y == 0)
	{
		// errno = EDOM;
		return 0;
	}
	double val;
	float frac = (float)__modf(x / y, &val);
	return frac * y;

	/*
	val = x / y;
	if (val > LONG_MIN && val < LONG_MAX) {
	i = val;
	return x - i * y;
	}
	*/
#else
	return x - int(x / y) * y;
#endif
#endif // MATHLIB_SIMULATE
}

float MathLib::Pow(float x, float y)
{
#ifndef MATHLIB_SIMULATE
	return powf(x, y);
#else
	if(x == 0.0f)
	{
		return 0.0f;
	}
	if(y == 0.0f)
	{
		return 1.0f;
	}
	return Exp(y * Log(x));
#endif // MATHLIB_SIMULATE
}

float MathLib::Exp(float f)
{
#ifndef MATHLIB_SIMULATE
	return expf(f);
#else
	int i, s, e, m, exponent;
	float x, x2, y, p, q;

	x = f * 1.44269504088896340f;    // multiply with (1 / log(2))
#if 1
	i = *(int*)&x;
	s = (i >> IEEE_FLT_SIGN_BIT);
	e = ((i >> IEEE_FLT_MANTISSA_BITS) & ((1 << IEEE_FLT_EXPONENT_BITS) - 1)) - IEEE_FLT_EXPONENT_BIAS;
	m = (i & ((1 << IEEE_FLT_MANTISSA_BITS) - 1)) | (1 << IEEE_FLT_MANTISSA_BITS);
	i = ((m >> (IEEE_FLT_MANTISSA_BITS - e)) & ~(e >> 31)) ^ s;
#else
	i = (int) x;
	if (x < 0.0f) {
		i--;
	}
#endif
	exponent = (i + IEEE_FLT_EXPONENT_BIAS) << IEEE_FLT_MANTISSA_BITS;
	y = *(float*)&exponent;
	x -= (float) i;
	if(x >= 0.5f)
	{
		x -= 0.5f;
		y *= 1.4142135623730950488f;  // multiply with sqrt(2)
	}
	x2 = x * x;
	p = x * (7.2152891511493f + x2 * 0.0576900723731f);
	q = 20.8189237930062f + x2;
	x = y * (q + p) / (q - p);
	return x;
#endif // MATHLIB_SIMULATE
}

#ifndef MATHLIB_SIMULATE
float MathLib::Log(float x)
{
	return logf(x);
}
#else
#if 1

#define M_1_SQRT2   0.70710678118654752440084436210484904

#define POLYNOM1(x, a) ((a)[1]*(x)+(a)[0])
#define POLYNOM2(x, a) (POLYNOM1((x),(a)+1)*(x)+(a)[0])
#define POLYNOM3(x, a) (POLYNOM2((x),(a)+1)*(x)+(a)[0])

float MathLib::Log(float x)
{
	/*
	Algorithm and coefficients from:
	"Software manual for the elementary functions"
	by W.J. Cody and W. Waite, Prentice-Hall, 1980
	*/
	static double a[] = {
		-0.64124943423745581147e2,
		0.16383943563021534222e2,
		-0.78956112887491257267e0
	};
	static double b[] = {
		-0.76949932108494879777e3,
		0.31203222091924532844e3,
		-0.35667977739034646171e2,
		1.0
	};

	double  znum, zden, z, w;
	int     exponent;

	if (IsNan(x)) {
		// errno = EDOM;
		return (float)x;
	}
	if (x < 0) {
		// errno = EDOM;
		return (float)-HUGE_VAL;
	}
	else if (x == 0) {
		// errno = ERANGE;
		return (float)-HUGE_VAL;
	}
	if (x <= DBL_MAX) {
	}
	else return x;  /* for infinity and Nan */
	x = Frexp(x, &exponent);
	if (x > M_1_SQRT2) {
		znum = (x - 0.5) - 0.5;
		zden = x * 0.5 + 0.5;
	}
	else {
		znum = x - 0.5;
		zden = znum * 0.5 + 0.5;
		exponent--;
	}
	z = znum/zden; w = z * z;
	double ax = z + z * w * (POLYNOM2(w,a)/POLYNOM3(w,b));
	z = exponent;
	ax += z * (-2.121944400546905827679e-4);
	return (float)(ax + z * 0.693359375);
}
#else
float MathLib::Log(float f)
{
	int i, exponent;
	float y, y2;

	i = *(int*)&f;
	exponent = ((i >> IEEE_FLT_MANTISSA_BITS) & ((1 << IEEE_FLT_EXPONENT_BITS) - 1)) - IEEE_FLT_EXPONENT_BIAS;
	i -= (exponent + 1) << IEEE_FLT_MANTISSA_BITS;  // get value in the range [.5, 1>
	y = *(float*)&i;
	y *= 1.4142135623730950488f;            // multiply with sqrt(2)
	y = (y - 1.0f) / (y + 1.0f);
	y2 = y * y;
	y = y * (2.000000000046727f + y2 * (0.666666635059382f + y2 * (0.4000059794795f + y2 * (0.28525381498f + y2 * 0.2376245609f))));
	y += 0.693147180559945f * ((float)exponent + 0.5f);
	return y;
}
#endif
#endif // MATHLIB_SIMULATE

int MathLib::FloorPowerOfTwo(int x)
{
	return CeilPowerOfTwo(x) >> 1;
}

int MathLib::CeilPowerOfTwo(int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

bool MathLib::IsPowerOfTwo(int x)
{
	return (x & (x - 1)) == 0 && x > 0;
}

// =========================================================================================
// =========================================================================================
// =========================================================================================

#define IEEE_ONE    0x3f800000
#define IEEE_MASK   0x007fffff

int RandomValue::Int()
{
	seed = 1664525L * seed + 1013904223L;
	return ((int)seed & MAX_RAND);
}

int RandomValue::Int(int max)
{
	if(max == 0)
	{
		return 0; // avoid divide by zero error 
	}
#if 0
	return Int() % max;
#else
	return MathLib::Round(Float() * (float)(max - 1));
#endif
}

float RandomValue::Float()
{
	seed = 1664525L * seed + 1013904223L;
	uint32 i = IEEE_ONE | (seed & IEEE_MASK);
	return ((*(float*)&i) - 1.0f);
}

float RandomValue::CFloat()
{
	seed = 1664525L * seed + 1013904223L;
	uint32 i = IEEE_ONE | (seed & IEEE_MASK);
	return (2.0f * (*(float*)&i) - 3.0f);
}

// =========================================================================================
// =========================================================================================
// =========================================================================================

bool vec3::FixDegenerateNormal()
{
	if (x == 0.0f)
	{
		if (y == 0.0f)
		{
			if (z > 0.0f)
			{
				if (z != 1.0f)
				{
					z = 1.0f;
					return true;
				}
			} 
			else
			{
				if (z != -1.0f)
				{
					z = -1.0f;
					return true;
				}
			}
			return false;
		} else if (z == 0.0f)
		{
			if (y > 0.0f)
			{
				if (y != 1.0f)
				{
					y = 1.0f;
					return true;
				}
			}
			else
			{
				if (y != -1.0f)
				{
					y = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	else if (y == 0.0f)
	{
		if (z == 0.0f)
		{
			if (x > 0.0f)
			{
				if (x != 1.0f)
				{
					x = 1.0f;
					return true;
				}
			}
			else
			{
				if (x != -1.0f)
				{
					x = -1.0f;
					return true;
				}
			}
			return false;
		}
	}
	if (MathLib::Abs(x) == 1.0f)
	{
		if (y != 0.0f || z != 0.0f)
		{
			y = z = 0.0f;
			return true;
		}
		return false;
	}
	else if (MathLib::Abs(y) == 1.0f)
	{
		if (x != 0.0f || z != 0.0f)
		{
			x = z = 0.0f;
			return true;
		}
		return false;
	}
	else if (MathLib::Abs(z) == 1.0f)
	{
		if (x != 0.0f || y != 0.0f)
		{
			x = y = 0.0f;
			return true;
		}
		return false;
	}
	return false;
}

// =========================================================================================

int Bounds3::PlaneSide(const Plane& plane, float epsilon) const
{
	vec3 center = vec3
		(
		(b[0].x + b[1].x) * 0.5f,
		(b[0].y + b[1].y) * 0.5f,
		(b[0].z + b[1].z) * 0.5f 
		);

	float d1 = plane.Dist(center);
	float d2 = MathLib::Abs((b[1].x - center.x) * plane.normal.x)
		+ MathLib::Abs((b[1].y - center.y) * plane.normal.y)
		+ MathLib::Abs((b[1].z - center.z) * plane.normal.z);

	if(d1 - d2 > epsilon)
	{
		return SIDE_FRONT;
	}
	if(d1 + d2 < -epsilon)
	{
		return SIDE_BACK;
	}
	return SIDE_CROSS;
}

// =========================================================================================

Plane::Plane(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal)
{
	Init(p1, p2, p3, fixDegenerateNormal);
}

void Plane::Init(const vec3& p1, const vec3& p2, const vec3& p3, bool fixDegenerateNormal)
{
	vec3 n = (p1 - p2).Cross(p3 - p2).Norm();
	if(fixDegenerateNormal)
	{
		n.FixDegenerateNormal();
	}
	Init(n, p2);
}

void Plane::Init(const vec3& n, const vec3& p)
{
#ifdef _DEBUG 
	float sl = n.SqrLen();
	ASSERT(sl >= 0.993f && sl <= 1.001f);
#endif

	normal = n;
	dist = n.Dot(p);
}

int Plane::PlaneAxeNum() const
{
	float n[] = {MathLib::Abs(normal.x), MathLib::Abs(normal.y), MathLib::Abs(normal.z)};
	if(n[0] > n[1])
	{
		return n[0] > n[2] ? 0 : 2;
	}
	return n[1] > n[2] ? 1 : 2;
}

float Plane::Dist(const vec3& p) const
{
	return normal.Dot(p) - dist;
}

int Plane::Side(const vec3& p) const
{
	float dist = Dist(p);
	if(dist > SIDE_ON_EPSILON)
		return SIDE_FRONT;
	if(dist < -SIDE_ON_EPSILON)
		return SIDE_BACK;

	return SIDE_ON;
}

bool Plane::CompareEps(const Plane& b, float distEps, float minNormalDot) const
{
	if(MathLib::Abs(dist - b.dist) > distEps || normal.Dot(b.normal) < minNormalDot)
		return false;

	return true;
}

bool Plane::RayIntersection(const vec3& start, const vec3& dir, float& scale) const
{
	float d1 = Dist(start);
	float d2 = normal.Dot(dir);
	if(d2 == 0.0f)
	{
		return false;
	}
	scale = -d1 / d2;
	return true;
}

bool Plane::SplitLine(const vec3& start, const vec3& end, vec3& mid, float& dot) const
{
	float dists[] = { Dist(start), Dist(end) };
	int side[] = { dists[0] >= 0.0f, dists[1] >= 0.0f };
	if(side[0] ^ side[1])
	{
		float n;
		int j;

		// always calculate the split going from the same side
		// or minor epsilon issues can happen
		if(side[0])
		{
			dot = dists[0] / (dists[0] - dists[1]);
			for(j = 0; j < 3; j++){
				// avoid round off error when possible
				n = normal.At(j);
				if(n == 1.0f)
					mid.Set(j, dist);
				else if(n == -1.0f)
					mid.Set(j, -dist);
				else
					mid.Set(j, start.At(j) + dot * (end.At(j) - start.At(j)));
			}
		}
		else
		{
			dot = dists[1] / (dists[1] - dists[0]);
			for(j = 0; j < 3; j++) {	
				// avoid round off error when possible
				n = normal.At(j);
				if(n == 1.0f)
					mid.Set(j, dist);
				else if(n == -1.0f)
					mid.Set(j, -dist);
				else
					mid.Set(j, end.At(j) + dot * (start.At(j) - end.At(j)));
			}
		}
		return true;
	}
	return false;
}

vec3 Plane::Reflect(const vec3& dirToPlane, const vec3& normal)
{
	/*
	N
	R       V
	\  ^  /
	\ | /
	\|/
	.
	O

	если построить Vproj - проекцию вектора V на N, то можно увидеть, что

	R + V = Vproj * 2 

	Vproj = N * длину проекции V на N, т.е.
	Vproj = N * dot(V, N) - при условии, что N нормализована

	R = Vproj * 2 - V = 2 * N * dot(V, N) - V

	В данном случае V направлен от точки O, в твоем случае V = -I (I - направление луча), и формула выглядит как 
	R = I - 2 * N * dot(I, N)
	*/
	return (dirToPlane - normal * ( 2.0f * normal.Dot(dirToPlane) ) ).Norm();
}

#define UV_EPSILON 0.001

bool MathLib::RayTriangleIntersection(const vec3& start, const vec3& dir, float rayLen, 
	const Plane& triPlane, const vec3& p1, const vec3& p2, const vec3& p3,
	vec3& contact, float& len, 
	float fronsideEpsilon, float backsideEpsilon,
	float frontsideStartBorder, float frontsideEndBorder)
{
	float d1 = triPlane.Dist(start);
	float d2 = triPlane.normal.Dot(dir);
	if((d1 <= fronsideEpsilon && d1 >= -backsideEpsilon)
		|| (d1 >= frontsideStartBorder && d1 <= frontsideEndBorder))
	{
		if(MathLib::Abs(d2) > 0.001f)
		{
			contact = start - triPlane.normal * d1;
			len = d1; // * (d2 < 0.0f ? -1.0f : 1.0f);
		}
		else
		{
			contact = start + dir * rayLen;
			d1 = triPlane.Dist(contact);
			contact = contact - triPlane.normal * d1;
			len = rayLen;
		}
	}
	else
	{    
		if(d2 == 0.0f)
		{
			return false;
		}
		float scale = -d1 / d2;
		if(scale < -SIDE_ON_EPSILON || scale > rayLen + SIDE_ON_EPSILON)
			return false;

		contact = start + dir * scale;
		len = scale;
	}  

	vec3 axe1 = p2 - p1;
	vec3 axe2 = p3 - p1;
	vec3 proj = contact - p1;

	static const int minorAxes[3][2] =
	{
		{1, 2}, // 0
		{0, 2}, // 1
		{0, 1}, // 2
	};
	int planeAxeNum = triPlane.PlaneAxeNum();
	int i1 = minorAxes[planeAxeNum][0];
	int i2 = minorAxes[planeAxeNum][1];

	float u, v;
	if(MathLib::Abs(axe2.At(i2)) < SIDE_ON_EPSILON)
	{
		u = (proj.At(i2)*axe2.At(i1) - proj.At(i1)*axe2.At(i2)) / (axe1.At(i2)*axe2.At(i1) - axe1.At(i1)*axe2.At(i2));
		v = (proj.At(i1) - u*axe1.At(i1)) / axe2.At(i1);
	}
	else
	{
		u = (proj.At(i1)*axe2.At(i2) - proj.At(i2)*axe2.At(i1)) / (axe1.At(i1)*axe2.At(i2) - axe1.At(i2)*axe2.At(i1));
		v = (proj.At(i2) - u*axe1.At(i2)) / axe2.At(i2);
	}
	if(u < -UV_EPSILON)
	{
		return false;
	}
	if(v < -UV_EPSILON)
	{
		return false;
	}
	float sum_uv = u + v;
	if(sum_uv < -UV_EPSILON)
	{
		return false;
	}
	if(sum_uv > 1.0f + UV_EPSILON)
	{
		return false;
	}
	return true;
}

bool MathLib::RayTriangleIntersection(const vec3& start, const vec3& end,
	const Plane& triPlane, const vec3& p1, const vec3& p2, const vec3& p3,
	vec3& contact, float& dot)
{
	if(!triPlane.SplitLine(start, end, contact, dot))
		return false;

	const float BIG_EPSILON = SIDE_ON_EPSILON; // * 20.0f;

	vec3 axe1 = p2 - p1;
	vec3 axe2 = p3 - p1;
	vec3 proj = contact - p1;

	static const int minorAxes[3][2] =
	{
		{1, 2}, // 0
		{0, 2}, // 1
		{0, 1}, // 2
	};
	int planeAxeNum = triPlane.PlaneAxeNum();
	int i1 = minorAxes[planeAxeNum][0];
	int i2 = minorAxes[planeAxeNum][1];

	float u, v;
	if(MathLib::Abs(axe2.At(i2)) < SIDE_ON_EPSILON)
	{
		u = (proj.At(i2)*axe2.At(i1) - proj.At(i1)*axe2.At(i2)) / (axe1.At(i2)*axe2.At(i1) - axe1.At(i1)*axe2.At(i2));
		v = (proj.At(i1) - u*axe1.At(i1)) / axe2.At(i1);
	}
	else
	{
		u = (proj.At(i1)*axe2.At(i2) - proj.At(i2)*axe2.At(i1)) / (axe1.At(i1)*axe2.At(i2) - axe1.At(i2)*axe2.At(i1));
		v = (proj.At(i2) - u*axe1.At(i2)) / axe2.At(i2);
	}
	if(u < -UV_EPSILON)
	{
		return false;
	}
	if(v < -UV_EPSILON)
	{
		return false;
	}
	float sum_uv = u + v;
	if(sum_uv < -UV_EPSILON)
	{
		return false;
	}
	if(sum_uv > 1.0f + UV_EPSILON)
	{
		return false;
	}
	return true;
}

bool MathLib::SphereRayTriangleIntersection(
	const vec3& start, const vec3& end, const vec3& dir, float radius, 
	const Plane& triPlane, const vec3& p1, const vec3& p2, const vec3& p3,
	vec3& contact, float& dot)
{
#if 0
	float d = triPlane.normal.Dot(dir);
	if(d >= 0.0f)
	{
		return false;
	}
	vec3 r;
	if(d < -0.99f)
	{
		r = dir.Mul(-radius);
	}
	else
	{
		r = dir.Cross(dir.Cross(triPlane.normal)).Norm().Mul(-radius);
		volatile float f = r.Dot(triPlane.normal);
		ASSERT(f > 0.0f);
	}
#else
	vec3 r = triPlane.normal * radius;
#endif
	vec3 a = start - r;
	vec3 b = end - r;

	if(RayTriangleIntersection(start, a, triPlane, p1, p2, p3, contact, dot))
	{
		contact = start; // contact.Add(r);
		dot = 0.0f;
		return true;
	}

	if(RayTriangleIntersection(a, b, triPlane, p1, p2, p3, contact, dot))
	{
		contact = contact + r;
		return true;
	}
	return false;

	/*
	vec3 a = start.Sub( dir.Mul(radius) );
	vec3 b = start.Add( dir.Mul(rayLen + radius) );

	float scale;
	if(!triPlane.RayIntersection(a, dir, rayLen + radius * 2.0f, scale))
	return false;

	vec3 p = a.Add( dir.Mul(scale) );
	vec3 axes[2];
	axes[0] = dir;
	axes[1] = dir.Cross(triPlane.normal).Cross(dir).Norm();
	volatile float ff = axe0.Dot(axe1);

	const vec3 * srcPoints[] = { &p1, &p2, &p3 };

	vec2 center = vec2(start.Dot(axes[0]), start.Dot(axes[1]));
	vec2 p[3];
	for(int i = 0; i < 3; i++)
	{
	p[i] = vec2(srcPoints[i].Dot(axes[0]) - center.x, srcPoints[i].Dot(axes[1]) - center.y);
	}
	*/
}

// =========================================================================================

Vectors::Vectors(const vec3& __forward, const vec3& __up)
{
#ifdef _DEBUG 
	float sl = __forward.SqrLen();
	ASSERT(sl >= 0.992f && sl <= 1.001f);
	sl = __up.SqrLen();
	ASSERT(sl >= 0.992f && sl <= 1.001f);
#endif

	forward = __forward;
	right = __forward.Cross(MathLib::Abs(__forward.Dot(__up)) < 0.99f ? __up : VEC3_FORWARD).Norm();
	up = right.Cross(forward).Norm();
}

Vectors::Vectors(const Angles& a)
{
	a.ToVectors(*this);
}

bool Vectors::FixDegenerateNormal()
{
	bool f = forward.FixDegenerateNormal();
	bool r = right.FixDegenerateNormal();
	bool u = up.FixDegenerateNormal();
	return f | r | u;
}

Angles& Vectors::ToAngles(Angles& out) const
{
	float sp = forward.z;
	if(sp > 1)
	{
		sp = 1;
	}
	else if(sp < -1)
	{
		sp = -1;
	}

	// float pitch = NormAngle180(90-ArcSin(sp));
	out.pitch = MathLib::NormAngle180(-MathLib::ArcSin(sp));
	if(out.pitch >= -89 && out.pitch <= 89)
	{
		out.yaw  = MathLib::NormAngle180(MathLib::ArcTan(-forward.x, forward.y));
		out.roll = MathLib::NormAngle180(-MathLib::ArcTan(right.z, up.z));
	}
	else
	{
		out.yaw  = MathLib::NormAngle180(-MathLib::ArcTan(right.x, right.y));
		out.roll = 0;
	}
	return out;
}

// =========================================================================================

Angles::Angles(const vec3& forward, const vec3& up)
{
	Vectors(forward, up).ToAngles(*this);
}

Angles& Angles::SetDiff(const Angles& a, const Angles& b)
{
	yaw   = MathLib::AnglesDiff(a.yaw,   b.yaw);
	pitch = MathLib::AnglesDiff(a.pitch, b.pitch);
	roll  = MathLib::AnglesDiff(a.roll,  b.roll);
	return *this;
}

vec3 Angles::Forward() const
{
	float sy, cy, sp, cp;

	MathLib::SinCos(yaw, sy, cy);
	MathLib::SinCos(pitch, sp, cp);

	vec3 v = vec3(-cp * sy, cp * cy, -sp);
	v.FixDegenerateNormal();
	return v;
}

Angles Angles::SlerpTo(const Angles& to, float t) const
{
	Angles diff;
	diff.SetDiff(*this, to);
	return SlerpByDiff(diff, t);
}

Angles Angles::SlerpByDiff(const Angles& diff, float t) const
{
	return Angles(yaw + diff.yaw * t,
		pitch + diff.pitch * t,
		roll + diff.roll * t);
}

Vectors& Angles::ToVectors(Vectors& out) const
{
	float sy, cy, sp, cp, sr, cr;

	MathLib::SinCos(yaw, sy, cy);
	MathLib::SinCos(pitch, sp, cp);
	MathLib::SinCos(roll, sr, cr);

	out.forward = vec3(-cp * sy, cp * cy, -sp);
	out.right   = vec3(sr * sp * sy + cr * cy, -sr * sp * cy + cr * sy, -sr * cp);
	out.up      = vec3(sr * cy - cr * sp * sy, cr * sp * cy + sr * sy, cr * cp);

	out.forward.FixDegenerateNormal();
	out.right.FixDegenerateNormal();
	out.up.FixDegenerateNormal();

	return out;
}

// =========================================================================================

Mat4x4::Mat4x4(float s)
{
	Ident(s);
}

Mat4x4::Mat4x4(const Vectors& v, const vec3& pos)
{
	rows[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	SetVectors(v, pos);
}

Mat4x4::Mat4x4(const Angles& a, const vec3& pos)
{
	rows[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	SetAngles(a, pos);
}

void Mat4x4::Ident(float s)
{
	memset(this, 0, sizeof(*this));
	rows[0].x = s;
	rows[1].y = s;
	rows[2].z = s;
	rows[3].w = 1.0f;
}

Vectors& Mat4x4::ToVectors(Vectors& out) const
{
	out.forward = vec3(rows[0].y, rows[1].y, rows[2].y);
	out.right   = vec3(rows[0].x, rows[1].x, rows[2].x);
	out.up      = vec3(rows[0].z, rows[1].z, rows[2].z);
	return out;
}

void Mat4x4::SetVectors(const Vectors& v)
{
	rows[0] = vec4(v.right.x, v.forward.x, v.up.x, rows[0].w);
	rows[1] = vec4(v.right.y, v.forward.y, v.up.y, rows[1].w);
	rows[2] = vec4(v.right.z, v.forward.z, v.up.z, rows[2].w);
}

void Mat4x4::SetVectors(const Vectors& v, const vec3& pos)
{
	rows[0] = vec4(v.right.x, v.forward.x, v.up.x, pos.x);
	rows[1] = vec4(v.right.y, v.forward.y, v.up.y, pos.y);
	rows[2] = vec4(v.right.z, v.forward.z, v.up.z, pos.z);
}

Angles& Mat4x4::ToAngles(Angles& out) const
{
	Vectors temp;
	return ToVectors(temp).ToAngles(out);
}

void Mat4x4::SetAngles(const Angles& a)
{
	Vectors temp;
	SetVectors(a.ToVectors(temp));
}

void Mat4x4::SetAngles(const Angles& a, const vec3& pos)
{
	Vectors temp;
	SetVectors(a.ToVectors(temp), pos);
}

vec3 Mat4x4::Translation() const
{
	return vec3(rows[0].w, rows[1].w, rows[2].w);
}

void Mat4x4::SetTranslation(const vec3& pos)
{
	rows[0].w = pos.x;
	rows[1].w = pos.y;
	rows[2].w = pos.z;
}

void Mat4x4::Scale(const vec3& s)
{
	rows[0].x *= s.x;
	rows[1].y *= s.y;
	rows[2].z *= s.z;
}

vec3 Mat4x4::RotateFast(const vec3& p) const
{
	/*
	assert(mat[ 3 ].x * p.x + mat[ 3 ].y * p.y + mat[ 3 ].z * p.z + mat[ 3 ].w == 1.0f);
	return xVec3(
	mat[ 0 ].x * p.x + mat[ 0 ].y * p.y + mat[ 0 ].z * p.z,
	mat[ 1 ].x * p.x + mat[ 1 ].y * p.y + mat[ 1 ].z * p.z,
	mat[ 2 ].x * p.x + mat[ 2 ].y * p.y + mat[ 2 ].z * p.z);
	*/
	return vec3(
		rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z,
		rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z,
		rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z);
}

vec3 Mat4x4::TransformFast(const vec3& p) const
{
	/* float s = rows[ 3 ].x * p.x + rows[ 3 ].y * p.y + rows[ 3 ].z * p.z + rows[ 3 ].w;
	if(s == 0.0f)
	{
	return VEC3_ZERO;
	}
	if(s == 1.0f)
	{
	*/
	return vec3(
		rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z + rows[ 0 ].w,
		rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z + rows[ 1 ].w,
		rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z + rows[ 2 ].w);
	/*}
	else
	{
	float invS = 1.0f / s;
	return vec3(
	(rows[ 0 ].x * p.x + rows[ 0 ].y * p.y + rows[ 0 ].z * p.z + rows[ 0 ].w) * invS,
	(rows[ 1 ].x * p.x + rows[ 1 ].y * p.y + rows[ 1 ].z * p.z + rows[ 1 ].w) * invS,
	(rows[ 2 ].x * p.x + rows[ 2 ].y * p.y + rows[ 2 ].z * p.z + rows[ 2 ].w) * invS);
	}*/
}

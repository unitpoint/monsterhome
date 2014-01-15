#include <EasyLib.h>
#include <MathLib.h>

float EasyLib::Linear(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	return b + c * t / d;
}

float EasyLib::InQuad(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d;
	return c * t * t + b;
}

float EasyLib::OutQuad(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d;
	return (0 - c) * t * (t - 2) + b;
}

float EasyLib::InOutQuad(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return c / 2 * t * t + b;
	}
	--t;
	return (0 - c) / 2 * (t * (t - 2) - 1) + b;
}

float EasyLib::OutInQuad(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutQuad(t * 2, b, c / 2, d);
	}
	return InQuad(t * 2 - d, b + c / 2, c / 2, d);
}

float EasyLib::InCubic(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t /= d;
	return c * t * t * t + b;
}

float EasyLib::OutCubic(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d - 1;
	return c * (t * t * t + 1) + b;
}

float EasyLib::InOutCubic(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return c / 2 * t * t * t + b;
	}
	t -= 2;
	return c / 2 * (t * t * t + 2) + b;
}

float EasyLib::OutInCubic(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutCubic(t * 2, b, c / 2, d);
	}
	return InCubic(t * 2 - d, b + c / 2, c / 2, d);
}

float EasyLib::InQuart(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t /= d;
	return c * t * t * t * t + b;
}

float EasyLib::OutQuart(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d - 1;
	return (0 - c) * (t * t * t * t - 1) + b;
}

float EasyLib::InOutQuart(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return c / 2 * t * t * t * t + b;
	}
	t -= 2;
	return (0 - c) / 2 * (t * t * t * t - 2) + b;
}

float EasyLib::OutInQuart(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutQuart(t * 2, b, c / 2, d);
	}
	return InQuart(t * 2 - d, b + c / 2, c / 2, d);
}

float EasyLib::InQuint(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t /= d;
	return c * t * t * t * t * t + b;
}

float EasyLib::OutQuint(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d - 1;
	return c * (t * t * t * t * t + 1) + b;
}

float EasyLib::InOutQuint(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return c / 2 * t * t * t * t * t + b;
	}
	t = t - 2;
	return c / 2 * (t * t * t * t * t + 2) + b;
}

float EasyLib::OutInQuint(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutQuint(t * 2, b, c / 2, d);
	}
	return InQuint(t * 2 - d, b + c / 2, c / 2, d);
}

/*
float EasyLib::InSine(float t, float b, float c, float d)
{
return (0 - c) * Math.cos(t / d * 1.57079632679) + c + b;
}

float EasyLib::OutSine(float t, float b, float c, float d)
{
return c * Math.sin(t / d * 1.57079632679) + b;
}

float EasyLib::InOutSine(float t, float b, float c, float d)
{
return (0 - c) / 2 * (Math.cos(3.14159265359 * t / d) - 1) + b;
}

float EasyLib::OutInSine(float t, float b, float c, float d)
{
if (t < d / 2) 
{
return OutSine(t * 2, b, c / 2, d);
}
return InSine(t * 2 - d, b + c / 2, c / 2, d);
}
*/

float EasyLib::InExpo(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	return c * MathLib::Pow(2, 10 * (t / d - 1)) + b;
}

float EasyLib::OutExpo(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	return c * (0 - MathLib::Pow(2, -10 * t / d) + 1) + b;
}

float EasyLib::InOutExpo(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return c / 2 * MathLib::Pow(2, 10 * (t - 1)) + b;
	}
	return c / 2 * (0 - MathLib::Pow(2, -10 * --t) + 2) + b;
}

float EasyLib::OutInExpo(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutExpo(t * 2, b, c / 2, d);
	}
	return InExpo(t * 2 - d, b + c / 2, c / 2, d);
}

float EasyLib::InCirc(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t /= d;
	return (0 - c) * (MathLib::Sqrt(1 - t * t) - 1) + b;
}

float EasyLib::OutCirc(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d - 1;
	return c * MathLib::Sqrt(1 - t * t) + b;
}

float EasyLib::InOutCirc(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		return (0 - c) / 2 * (MathLib::Sqrt(1 - t * t) - 1) + b;
	}
	t -= 2;
	return c / 2 * (MathLib::Sqrt(1 - t * t) + 1) + b;
}

float EasyLib::OutInCirc(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutCirc(t * 2, b, c / 2, d);
	}
	return InCirc(t * 2 - d, b + c / 2, c / 2, d);
}

/*
float EasyLib::InElastic(t, b, c, d, a, p)
{
var __reg5 = undefined;
if (t == 0) 
{
return b;
}
if ((t = t / d) == 1) 
{
return b + c;
}
if (!p) 
{
p = d * 0.3;
}
if (!a || a < Math.abs(c)) 
{
a = c;
__reg5 = p / 4;
}
else 
{
__reg5 = p / 6.28318530718 * Math.asin(c / a);
}
return 0 - a * MathLib::Pow(2, 10 * (t = t - 1)) * Math.sin((t * d - __reg5) * 6.28318530718 / p) + b;
}

float EasyLib::OutElastic(t, b, c, d, a, p)
{
var __reg5 = undefined;
if (t == 0) 
{
return b;
}
if ((t = t / d) == 1) 
{
return b + c;
}
if (!p) 
{
p = d * 0.3;
}
if (!a || a < Math.abs(c)) 
{
a = c;
__reg5 = p / 4;
}
else 
{
__reg5 = p / 6.28318530718 * Math.asin(c / a);
}
return a * MathLib::Pow(2, -10 * t) * Math.sin((t * d - __reg5) * 6.28318530718 / p) + c + b;
}

float EasyLib::InOutElastic(t, b, c, d, a, p)
{
var __reg5 = undefined;
if (t == 0) 
{
return b;
}
if ((t = t / (d / 2)) == 2) 
{
return b + c;
}
if (!p) 
{
p = d * 0.45;
}
if (!a || a < Math.abs(c)) 
{
a = c;
__reg5 = p / 4;
}
else 
{
__reg5 = p / 6.28318530718 * Math.asin(c / a);
}
if (t < 1) 
{
return -0.5 * a * MathLib::Pow(2, 10 * (t = t - 1)) * Math.sin((t * d - __reg5) * 6.28318530718 / p) + b;
}
return a * MathLib::Pow(2, -10 * (t = t - 1)) * Math.sin((t * d - __reg5) * 6.28318530718 / p) * 0.5 + c + b;
}

float EasyLib::OutInElastic(t, b, c, d, a, p)
{
if (t < d / 2) 
{
return OutElastic(t * 2, b, c / 2, d, a, p);
}
return InElastic(t * 2 - d, b + c / 2, c / 2, d, a, p);
}
*/

float EasyLib::InBack(float t, float b, float c, float d, float s)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t /= d;
	return c * t * t * ((s + 1) * t - s) + b;
}

float EasyLib::OutBack(float t, float b, float c, float d, float s)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	t = t / d - 1;
	return c * (t * t * ((s + 1) * t + s) + 1) + b;
}

float EasyLib::InOutBack(float t, float b, float c, float d, float s)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / (d / 2)) < 1) 
	{
		s *= 1.525f;
		return c / 2 * t * t * ((s + 1) * t - s) + b;
	}
	s *= 1.525f;
	t -= 2;
	return c / 2 * (t * t * ((s + 1) * t + s) + 2) + b;
}

float EasyLib::OutInBack(float t, float b, float c, float d, float s)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutBack(t * 2, b, c / 2, d, s);
	}
	return InBack(t * 2 - d, b + c / 2, c / 2, d, s);
}

float EasyLib::InBounce(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	return c - OutBounce(d - t, 0, c, d) + b;
}

float EasyLib::OutBounce(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if ((t = t / d) < 0.363636363636f) 
	{
		return c * 7.5625f * t * t + b;
	}
	if (t < 0.727272727273f) 
	{
		t -= 0.545454545455f;
		return c * (7.5625f * t * t + 0.75f) + b;
	}
	if (t < 0.909090909091f) 
	{
		t -= 0.818181818182f;
		return c * (7.5625f * t * t + 0.9375f) + b;
	}
	t -= 0.954545454545f;
	return c * (7.5625f * t * t + 0.984375f) + b;
}

float EasyLib::InOutBounce(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return InBounce(t * 2, 0, c, d) * 0.5f + b;
	}
	return OutBounce(t * 2 - d, 0, c, d) * 0.5f + c * 0.5f + b;
}

float EasyLib::OutInBounce(float t, float b, float c, float d)
{
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	if (t < d / 2) 
	{
		return OutBounce(t * 2, b, c / 2, d);
	}
	return InBounce(t * 2 - d, b + c / 2, c / 2, d);
}

float EasyLib::Run(EDirType dir, EMethod method, float t, float b, float c, float d, float s)
{
	bool isBack = dir == BACK;
	if(isBack)
	{
		c = -c;
		b -= c;
		// t = d - t;
	}
	if(t <= 0.0f)
	{
		return b;
	}
	if(t >= d)
	{
		return b + c;
	}
	bool isGeneric = false;
	switch(method)
	{
	case IN_QUAD:
		isGeneric = true;
		// no break
	case OUT_QUAD:
		return (isGeneric ^ isBack) ? InQuad(t, b, c, d) : OutQuad(t, b, c, d);

	case IN_OUT_QUAD:
		isGeneric = true;
		// no break
	case OUT_IN_QUAD:
		return (isGeneric ^ isBack) ? InOutQuad(t, b, c, d) : OutInQuad(t, b, c, d);

	case IN_CUBIC:
		isGeneric = true;
		// no break
	case OUT_CUBIC:
		return (isGeneric ^ isBack) ? InCubic(t, b, c, d) : OutCubic(t, b, c, d);

	case IN_OUT_CUBIC:
		isGeneric = true;
		// no break
	case OUT_IN_CUBIC:
		return (isGeneric ^ isBack) ? InOutCubic(t, b, c, d) : OutInCubic(t, b, c, d);

	case IN_QUART:
		isGeneric = true;
		// no break
	case OUT_QUART:
		return (isGeneric ^ isBack) ? InQuart(t, b, c, d) : OutQuart(t, b, c, d);

	case IN_OUT_QUART:
		isGeneric = true;
		// no break
	case OUT_IN_QUART:
		return (isGeneric ^ isBack) ? InOutQuart(t, b, c, d) : OutInQuart(t, b, c, d);

	case IN_QUINT:
		isGeneric = true;
		// no break
	case OUT_QUINT:
		return (isGeneric ^ isBack) ? InQuint(t, b, c, d) : OutQuint(t, b, c, d);

	case IN_OUT_QUINT:
		isGeneric = true;
		// no break
	case OUT_IN_QUINT:
		return (isGeneric ^ isBack) ? InOutQuint(t, b, c, d) : OutInQuint(t, b, c, d);

		/*
		case IN_SINE:
		isGeneric = true;
		// no break
		case OUT_SINE:
		case IN_OUT_SINE:
		isGeneric = true;
		// no break
		case OUT_IN_SINE:
		*/
	case IN_EXPO:
		isGeneric = true;
		// no break
	case OUT_EXPO:
		return (isGeneric ^ isBack) ? InExpo(t, b, c, d) : OutExpo(t, b, c, d);

	case IN_OUT_EXPO:
		isGeneric = true;
		// no break
	case OUT_IN_EXPO:
		return (isGeneric ^ isBack) ? InOutExpo(t, b, c, d) : OutInExpo(t, b, c, d);

	case IN_CIRC:
		isGeneric = true;
		// no break
	case OUT_CIRC:
		return (isGeneric ^ isBack) ? InCirc(t, b, c, d) : OutCirc(t, b, c, d);

	case IN_OUT_CIRC:
		isGeneric = true;
		// no break
	case OUT_IN_CIRC:
		return (isGeneric ^ isBack) ? InOutCirc(t, b, c, d) : OutInCirc(t, b, c, d);

		/*
		case IN_ELASTIC:
		isGeneric = true;
		// no break
		case OUT_ELASTIC:
		case IN_OUT_ELASTIC:
		isGeneric = true;
		// no break
		case OUT_IN_ELASTIC:
		*/
	case IN_BACK:
		isGeneric = true;
		// no break
	case OUT_BACK:
		return (isGeneric ^ isBack) ? InBack(t, b, c, d, s) : OutBack(t, b, c, d, s);

	case IN_OUT_BACK:
		isGeneric = true;
		// no break
	case OUT_IN_BACK:
		return (isGeneric ^ isBack) ? InOutBack(t, b, c, d, s) : OutInBack(t, b, c, d, s);

	case IN_BOUNCE:
		isGeneric = true;
		// no break
	case OUT_BOUNCE:
		return (isGeneric ^ isBack) ? InBounce(t, b, c, d) : OutBounce(t, b, c, d);

	case IN_OUT_BOUNCE:
		isGeneric = true;
		// no break
	case OUT_IN_BOUNCE:
		return (isGeneric ^ isBack) ? InOutBounce(t, b, c, d) : OutInBounce(t, b, c, d);
	}
	return b + c * t / d;
}

EasyLib::EMethod EasyLib::ReverseMethod(EMethod method)
{
	switch(method)
	{
	case LINEAR:
		return LINEAR;

	case IN_QUAD:
		return OUT_QUAD;
	case OUT_QUAD:
		return IN_QUAD;

	case IN_OUT_QUAD:
		return OUT_IN_QUAD;
	case OUT_IN_QUAD:
		return IN_OUT_QUAD;

	case IN_CUBIC:
		return OUT_CUBIC;
	case OUT_CUBIC:
		return IN_CUBIC;

	case IN_OUT_CUBIC:
		return OUT_IN_CUBIC;
	case OUT_IN_CUBIC:
		return IN_OUT_CUBIC;

	case IN_QUART:
		return OUT_QUART;
	case OUT_QUART:
		return IN_QUART;

	case IN_OUT_QUART:
		return OUT_IN_QUART;
	case OUT_IN_QUART:
		return IN_OUT_QUART;

	case IN_QUINT:
		return OUT_QUINT;
	case OUT_QUINT:
		return IN_QUINT;

	case IN_OUT_QUINT:
		return OUT_IN_QUINT;
	case OUT_IN_QUINT:
		return IN_OUT_QUINT;

		/*
		case IN_SINE:
		return OUT_SINE;
		case OUT_SINE:
		return IN_SINE;

		case IN_OUT_SINE:
		return OUT_IN_SINE;
		case OUT_IN_SINE:
		return IN_OUT_SINE;
		*/
	case IN_EXPO:
		return OUT_EXPO;
	case OUT_EXPO:
		return IN_EXPO;

	case IN_OUT_EXPO:
		return OUT_IN_EXPO;
	case OUT_IN_EXPO:
		return IN_OUT_EXPO;

	case IN_CIRC:
		return OUT_CIRC;
	case OUT_CIRC:
		return IN_CIRC;

	case IN_OUT_CIRC:
		return OUT_IN_CIRC;
	case OUT_IN_CIRC:
		return IN_OUT_CIRC;
		/*
		case IN_ELASTIC:
		return OUT_ELASTIC;
		case OUT_ELASTIC:
		return IN_ELASTIC;

		case IN_OUT_ELASTIC:
		return OUT_IN_ELASTIC;
		case OUT_IN_ELASTIC:
		return IN_OUT_ELASTIC;
		*/
	case IN_BACK:
		return OUT_BACK;
	case OUT_BACK:
		return IN_BACK;

	case IN_OUT_BACK:
		return OUT_IN_BACK;
	case OUT_IN_BACK:
		return IN_OUT_BACK;

	case IN_BOUNCE:
		return OUT_BOUNCE;
	case OUT_BOUNCE:
		return IN_BOUNCE;

	case IN_OUT_BOUNCE:
		return OUT_IN_BOUNCE;
	case OUT_IN_BOUNCE:
		return IN_OUT_BOUNCE;
	}
	ASSERT(false);
	return method;
}
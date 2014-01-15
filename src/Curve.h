#ifndef __MATH_CURVE_H__
#define __MATH_CURVE_H__

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

#include <vector>
#include <IwArray.h>

#define CURVE_INLINE inline

#if 1
#define CurveArray CIwArray
#else
template <class T>
class CurveArray: public std::vector<T>
{
	typedef std::vector<T> super;

public:

	void erase(int i)
	{
		super::erase(super::begin() + i);
	}

	void insert_slow(const T& obj, int i)
	{
		super::insert(super::begin()+i, obj);
	}
};
#endif

#define _alloca16( x )	((void *)((((int)alloca( (x)+15 )) + 15) & ~15))

/*
===============================================================================

	Curve base template.

===============================================================================
*/

template< class type >
class Curve
{
public:

	Curve();
	virtual				~Curve();

	virtual int			AddValue(const float time, const type &value);
	virtual void		RemoveIndex(const int index) { values.erase(index); times.erase(index); changed = true; }
	virtual void		Clear() { values.clear(); times.clear(); currentIndex = -1; changed = true; }

	virtual type		GetCurrentValue(const float time) const;
	virtual type		GetCurrentFirstDerivative(const float time) const;
	virtual type		GetCurrentSecondDerivative(const float time) const;

	virtual bool		IsDone(const float time) const;

	int					GetNumValues() const { return values.size(); }
	void				SetValue(const int index, const type &value) { values[index] = value; changed = true; }
	type				GetValue(const int index) const { return values[index]; }
	type *				GetValueAddress(const int index) { return &values[index]; }
	float				GetTime(const int index) const { return times[index]; }

	float				GetLengthForTime(const float time) const;
	float				GetTimeForLength(const float length, const float epsilon = 0.1f) const;
	float				GetLengthBetweenKnots(const int i0, const int i1) const;

	void				MakeUniform(const float totalTime);
	void				SetConstantSpeed(const float totalTime);
	void				ShiftTime(const float deltaTime);
	void				Translate(const type &translation);

protected:

	CurveArray<float>	times;			// knots
	CurveArray<type>		values;			// knot values
	mutable int			currentIndex;	// cached index for fast lookup
	mutable bool		changed;		// set whenever the curve changes

	int					IndexForTime(const float time) const;
	float				TimeForIndex(const int index) const;
	type				ValueForIndex(const int index) const;

	float				GetSpeed(const float time) const;
	float				RombergIntegral(const float t0, const float t1, const int order) const;
};

/*
====================
Curve::Curve
====================
*/
template< class type >
CURVE_INLINE Curve<type>::Curve() {
	currentIndex = -1;
	changed = false;
}

/*
====================
Curve::~Curve
====================
*/
template< class type >
CURVE_INLINE Curve<type>::~Curve() {
}

/*
====================
Curve::AddValue

  add a timed/value pair to the spline
  returns the index to the inserted pair
====================
*/
template< class type >
CURVE_INLINE int Curve<type>::AddValue(const float time, const type &value) {
	int i;

	i = IndexForTime(time);
	times.insert_slow(time, i);
	values.insert_slow(value, i);
	changed = true;
	return i;
}

/*
====================
Curve::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE type Curve<type>::GetCurrentValue(const float time) const {
	int i;

	i = IndexForTime(time);
	if (i >= (int)values.size()) {
		return values[values.size() - 1];
	} else {
		return values[i];
	}
}

/*
====================
Curve::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
CURVE_INLINE type Curve<type>::GetCurrentFirstDerivative(const float time) const {
	return (values[0] - values[0]);
}

/*
====================
Curve::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
CURVE_INLINE type Curve<type>::GetCurrentSecondDerivative(const float time) const {
	return (values[0] - values[0]);
}

/*
====================
Curve::IsDone
====================
*/
template< class type >
CURVE_INLINE bool Curve<type>::IsDone(const float time) const {
	return (time >= times[ times.size() - 1 ]);
}

/*
====================
Curve::GetSpeed
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::GetSpeed(const float time) const {
	int i;
	float speed;
	type value;

	value = GetCurrentFirstDerivative(time);
	for (speed = 0.0f, i = 0; i < value.GetDimension(); i++) {
		speed += value[i] * value[i];
	}
	return sqrt(speed);
}

/*
====================
Curve::RombergIntegral
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::RombergIntegral(const float t0, const float t1, const int order) const {
	int i, j, k, m, n;
	float sum, delta;
	float *temp[2];

	temp[0] = (float *) _alloca16(order * sizeof(float));
	temp[1] = (float *) _alloca16(order * sizeof(float));

	delta = t1 - t0;
	temp[0][0] = 0.5f * delta * (GetSpeed(t0) + GetSpeed(t1));

	for (i = 2, m = 1; i <= order; i++, m *= 2, delta *= 0.5f) {

		// approximate using the trapezoid rule
		sum = 0.0f;
		for (j = 1; j <= m; j++) {
			sum += GetSpeed(t0 + delta * (j - 0.5f));
		}

		// Richardson extrapolation
		temp[1][0] = 0.5f * (temp[0][0] + delta * sum);
		for (k = 1, n = 4; k < i; k++, n *= 4) {
			temp[1][k] = (n * temp[1][k-1] - temp[0][k-1]) / (n - 1);
		}

		for (j = 0; j < i; j++) {
			temp[0][j] = temp[1][j];
		}
	}
	return temp[0][order-1];
}

/*
====================
Curve::GetLengthBetweenKnots
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::GetLengthBetweenKnots(const int i0, const int i1) const {
	float length = 0.0f;
	for (int i = i0; i < i1; i++) {
		length += RombergIntegral(times[i], times[i+1], 5);
	}
	return length;
}

/*
====================
Curve::GetLengthForTime
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::GetLengthForTime(const float time) const {
	float length = 0.0f;
	int index = IndexForTime(time);
	for (int i = 0; i < index; i++) {
		length += RombergIntegral(times[i], times[i+1], 5);
	}
	length += RombergIntegral(times[index], time, 5);
	return length;
}

/*
====================
Curve::GetTimeForLength
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::GetTimeForLength(const float length, const float epsilon) const {
	int i, index;
	float *accumLength, totalLength, len0, len1, t, diff;

	if (length <= 0.0f) {
		return times[0];
	}

	accumLength = (float *) _alloca16(values.size() * sizeof(float));
	totalLength = 0.0f;
	for (index = 0; index < values.size() - 1; index++) {
		totalLength += GetLengthBetweenKnots(index, index + 1);
		accumLength[index] = totalLength;
		if (length < accumLength[index]) {
			break;
		}
	}

	if (index >= values.size() - 1) {
		return times[times.size() - 1];
	}

	if (index == 0) {
		len0 = length;
		len1 = accumLength[0];
	} else {
		len0 = length - accumLength[index-1];
		len1 = accumLength[index] - accumLength[index-1];
	}

	// invert the arc length integral using Newton's method
	t = (times[index+1] - times[index]) * len0 / len1;
	for (i = 0; i < 32; i++) {
		diff = RombergIntegral(times[index], times[index] + t, 5) - len0;
		if (fabs(diff) <= epsilon) {
			return times[index] + t;
		}
		t -= diff / GetSpeed(times[index] + t);
	}
	return times[index] + t;
}

/*
====================
Curve::MakeUniform
====================
*/
template< class type >
CURVE_INLINE void Curve<type>::MakeUniform(const float totalTime) {
	int i, n;

	n = times.size() - 1;
	for (i = 0; i <= n; i++) {
		times[i] = i * totalTime / n;
	}
	changed = true;
}

/*
====================
Curve::SetConstantSpeed
====================
*/
template< class type >
CURVE_INLINE void Curve<type>::SetConstantSpeed(const float totalTime) {
	int i, j;
	float *length, totalLength, scale, t;

	length = (float *) _alloca16(values.size() * sizeof(float));
	totalLength = 0.0f;
	for (i = 0; i < (int)values.size() - 1; i++) {
		length[i] = GetLengthBetweenKnots(i, i + 1);
		totalLength += length[i];
	}
	scale = totalTime / totalLength;
	for (t = 0.0f, i = 0; i < (int)times.size() - 1; i++) {
		times[i] = t;
		t += scale * length[i];
	}
	times[times.size() - 1] = totalTime;
	changed = true;
}

/*
====================
Curve::ShiftTime
====================
*/
template< class type >
CURVE_INLINE void Curve<type>::ShiftTime(const float deltaTime) {
	for (int i = 0; i < (int)times.size(); i++) {
		times[i] += deltaTime;
	}
	changed = true;
}

/*
====================
Curve::Translate
====================
*/
template< class type >
CURVE_INLINE void Curve<type>::Translate(const type &translation) {
	for (int i = 0; i < (int)values.size(); i++) {
		values[i] += translation;
	}
	changed = true;
}

/*
====================
Curve::IndexForTime

  find the index for the first time greater than or equal to the given time
====================
*/
template< class type >
CURVE_INLINE int Curve<type>::IndexForTime(const float time) const {
	int len, mid, offset, res;

	if (currentIndex >= 0 && currentIndex <= (int)times.size()) {
		// use the cached index if it is still valid
		if (currentIndex == 0) {
			if (time <= times[currentIndex]) {
				return currentIndex;
			}
		} else if (currentIndex == (int)times.size()) {
			if (time > times[currentIndex-1]) {
				return currentIndex;
			}
		} else if (time > times[currentIndex-1] && time <= times[currentIndex]) {
			return currentIndex;
		} else if (time > times[currentIndex] && (currentIndex+1 == (int)times.size() || time <= times[currentIndex+1])) {
			// use the next index
			currentIndex++;
			return currentIndex;
		}
	}

	// use binary search to find the index for the given time
	len = (int)times.size();
	mid = len;
	offset = 0;
	res = 0;
	while(mid > 0) {
		mid = len >> 1;
		if (time == times[offset+mid]) {
			return offset+mid;
		} else if (time > times[offset+mid]) {
			offset += mid;
			len -= mid;
			res = 1;
		} else {
			len -= mid;
			res = 0;
		}
	}
	currentIndex = offset+res;
	return currentIndex;
}

/*
====================
Curve::ValueForIndex

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE type Curve<type>::ValueForIndex(const int index) const {
	int n = (int)values.size()-1;

	if (index < 0) {
		return values[0] + index * (values[1] - values[0]);
	} else if (index > n) {
		return values[n] + (index - n) * (values[n] - values[n-1]);
	}
	return values[index];
}

/*
====================
Curve::TimeForIndex

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE float Curve<type>::TimeForIndex(const int index) const {
	int n = (int)times.size()-1;

	if (index < 0) {
		return times[0] + index * (times[1] - times[0]);
	} else if (index > n) {
		return times[n] + (index - n) * (times[n] - times[n-1]);
	}
	return times[index];
}

/*
===============================================================================

	Spline base template.

===============================================================================
*/

template< class type >
class CurveSpline : public Curve<type>
{
public:

	enum				boundary_t { BT_FREE, BT_CLAMPED, BT_CLOSED };

	CurveSpline();

	virtual bool		IsDone(const float time) const;

	virtual void		SetBoundaryType(const boundary_t bt) { boundaryType = bt; Curve<type>::changed = true; }
	virtual boundary_t	GetBoundaryType() const { return boundaryType; }

	virtual void		SetCloseTime(const float t) { closeTime = t; Curve<type>::changed = true; }
	virtual float		GetCloseTime() { return boundaryType == BT_CLOSED ? closeTime : 0.0f; }

protected:

	boundary_t			boundaryType;
	float				closeTime;

	type				ValueForIndex(const int index) const;
	float				TimeForIndex(const int index) const;
	float				ClampedTime(const float t) const;
};

/*
====================
CurveSpline::CurveSpline
====================
*/
template< class type >
CURVE_INLINE CurveSpline<type>::CurveSpline() {
	boundaryType = BT_FREE;
	closeTime = 0.0f;
}

/*
====================
CurveSpline::ValueForIndex

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE type CurveSpline<type>::ValueForIndex(const int index) const {
	const CurveArray<type>& values = this->Curve<type>::values;
	int n = (int)values.size()-1;

	if (index < 0) {
		if (boundaryType == BT_CLOSED) {
			return values[ values.size() + index % values.size() ];
		}
		else {
			return values[0] + index * (values[1] - values[0]);
		}
	}
	else if (index > n) {
		if (boundaryType == BT_CLOSED) {
			return values[ index % values.size() ];
		}
		else {
			return values[n] + (index - n) * (values[n] - values[n-1]);
		}
	}
	return values[index];
}

/*
====================
CurveSpline::TimeForIndex

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE float CurveSpline<type>::TimeForIndex(const int index) const {
	const CurveArray<float>& times = this->Curve<type>::times;
	int n = (int)times.size()-1;

	if (index < 0) {
		if (boundaryType == BT_CLOSED) {
			return (index / times.size()) * (times[n] + closeTime) - (times[n] + closeTime - times[times.size() + index % times.size()]);
		}
		else {
			return times[0] + index * (times[1] - times[0]);
		}
	}
	else if (index > n) {
		if (boundaryType == BT_CLOSED) {
			return (index / times.size()) * (times[n] + closeTime) + times[index % times.size()];
		}
		else {
			return times[n] + (index - n) * (times[n] - times[n-1]);
		}
	}
	return times[index];
}

/*
====================
CurveSpline::ClampedTime

  return the clamped time based on the boundary type
====================
*/
template< class type >
CURVE_INLINE float CurveSpline<type>::ClampedTime(const float t) const {
	if (boundaryType == BT_CLAMPED) {
		const CurveArray<float>& times = this->Curve<type>::times;
		if (t < times[0]) {
			return times[0];
		}
		else if (t >= times[times.size()-1]) {
			return times[times.size()-1];
		}
	}
	return t;
}

/*
====================
CurveSpline::IsDone
====================
*/
template< class type >
CURVE_INLINE bool CurveSpline<type>::IsDone(const float time) const {
	const CurveArray<float>& times = this->Curve<type>::times;
	return (boundaryType != BT_CLOSED && time >= times[ times.size() - 1 ]);
}

/*
===============================================================================

	Uniform Cubic Interpolating Spline template.
	The curve goes through all the knots.

===============================================================================
*/

template< class type >
class CurveCatmullRomSpline : public CurveSpline<type>
{
public:

	CurveCatmullRomSpline();

	virtual type		GetCurrentValue(const float time) const;
	virtual type		GetCurrentFirstDerivative(const float time) const;
	virtual type		GetCurrentSecondDerivative(const float time) const;

protected:

	void				Basis(const int index, const float t, float *bvals) const;
	void				BasisFirstDerivative(const int index, const float t, float *bvals) const;
	void				BasisSecondDerivative(const int index, const float t, float *bvals) const;
};

/*
====================
CurveCatmullRomSpline::CurveCatmullRomSpline
====================
*/
template< class type >
CURVE_INLINE CurveCatmullRomSpline<type>::CurveCatmullRomSpline() {
}

/*
====================
CurveCatmullRomSpline::GetCurrentValue

  get the value for the given time
====================
*/
template< class type >
CURVE_INLINE type CurveCatmullRomSpline<type>::GetCurrentValue(const float time) const {
	int i, j, k;
	float bvals[4], clampedTime;
	type v;

	const CurveArray<float>& times = this->Curve<type>::times;
	const CurveArray<type>& values = this->Curve<type>::values;
	if (times.size() == 1) {
		return values[0];
	}

	clampedTime = CurveSpline<type>::ClampedTime(time);
	i = CurveSpline<type>::IndexForTime(clampedTime);
	Basis(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * CurveSpline<type>::ValueForIndex(k);
	}
	return v;
}

/*
====================
CurveCatmullRomSpline::GetCurrentFirstDerivative

  get the first derivative for the given time
====================
*/
template< class type >
CURVE_INLINE type CurveCatmullRomSpline<type>::GetCurrentFirstDerivative(const float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	const CurveArray<float>& times = this->Curve<type>::times;
	const CurveArray<type>& values = this->Curve<type>::values;
	if (times.size() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = CurveSpline<type>::ClampedTime(time);
	i = CurveSpline<type>::IndexForTime(clampedTime);
	BasisFirstDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * CurveSpline<type>::ValueForIndex(k);
	}
	d = (CurveSpline<type>::TimeForIndex(i) - CurveSpline<type>::TimeForIndex(i-1));
	return v / d;
}

/*
====================
CurveCatmullRomSpline::GetCurrentSecondDerivative

  get the second derivative for the given time
====================
*/
template< class type >
CURVE_INLINE type CurveCatmullRomSpline<type>::GetCurrentSecondDerivative(const float time) const {
	int i, j, k;
	float bvals[4], d, clampedTime;
	type v;

	const CurveArray<float>& times = this->Curve<type>::times;
	const CurveArray<type>& values = this->Curve<type>::values;	if (times.size() == 1) {
		return (values[0] - values[0]);
	}

	clampedTime = CurveSpline<type>::ClampedTime(time);
	i = CurveSpline<type>::IndexForTime(clampedTime);
	BasisSecondDerivative(i-1, clampedTime, bvals);
	v = values[0] - values[0];
	for (j = 0; j < 4; j++) {
		k = i + j - 2;
		v += bvals[j] * CurveSpline<type>::ValueForIndex(k);
	}
	d = (CurveSpline<type>::TimeForIndex(i) - CurveSpline<type>::TimeForIndex(i-1));
	return v / (d * d);
}

/*
====================
CurveCatmullRomSpline::Basis

  spline basis functions
====================
*/
template< class type >
CURVE_INLINE void CurveCatmullRomSpline<type>::Basis(const int index, const float t, float *bvals) const {
	float s = (float) (t - CurveSpline<type>::TimeForIndex(index)) / (CurveSpline<type>::TimeForIndex(index+1) - CurveSpline<type>::TimeForIndex(index));
	bvals[0] = ((-s + 2.0f) * s - 1.0f) * s * 0.5f;				// -0.5f s * s * s + s * s - 0.5f * s
	bvals[1] = (((3.0f * s - 5.0f) * s) * s + 2.0f) * 0.5f;	// 1.5f * s * s * s - 2.5f * s * s + 1.0f
	bvals[2] = ((-3.0f * s + 4.0f) * s + 1.0f) * s * 0.5f;		// -1.5f * s * s * s - 2.0f * s * s + 0.5f s
	bvals[3] = ((s - 1.0f) * s * s) * 0.5f;						// 0.5f * s * s * s - 0.5f * s * s
}

/*
====================
CurveCatmullRomSpline::BasisFirstDerivative

  first derivative of spline basis functions
====================
*/
template< class type >
CURVE_INLINE void CurveCatmullRomSpline<type>::BasisFirstDerivative(const int index, const float t, float *bvals) const {
	float s = (float) (t - CurveSpline<type>::TimeForIndex(index)) / (CurveSpline<type>::TimeForIndex(index+1) - CurveSpline<type>::TimeForIndex(index));
	bvals[0] = (-1.5f * s + 2.0f) * s - 0.5f;						// -1.5f * s * s + 2.0f * s - 0.5f
	bvals[1] = (4.5f * s - 5.0f) * s;								// 4.5f * s * s - 5.0f * s
	bvals[2] = (-4.5f * s + 4.0f) * s + 0.5f;						// -4.5 * s * s + 4.0f * s + 0.5f
	bvals[3] = 1.5f * s * s - s;									// 1.5f * s * s - s
}

/*
====================
CurveCatmullRomSpline::BasisSecondDerivative

  second derivative of spline basis functions
====================
*/
template< class type >
CURVE_INLINE void CurveCatmullRomSpline<type>::BasisSecondDerivative(const int index, const float t, float *bvals) const {
	float s = (float) (t - CurveSpline<type>::TimeForIndex(index)) / (CurveSpline<type>::TimeForIndex(index+1) - CurveSpline<type>::TimeForIndex(index));
	bvals[0] = -3.0f * s + 2.0f;
	bvals[1] = 9.0f * s - 5.0f;
	bvals[2] = -9.0f * s + 4.0f;
	bvals[3] = 3.0f * s - 1.0f;
}

#endif /* __MATH_CURVE_H__ */

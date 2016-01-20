/*
 * Name:
 *	IASFixed.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Wrapper class for ASFixed.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/10/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IASFixed_hpp__
#define __IASFixed_hpp__

/*
 * Includes
 */
 
#include "ASTypes.h"

/*
 * Types and Defines
 */

// Problem: Mac headers define GetItem.  DRL 7/29/96
#ifdef GetItem
#undef GetItem
#endif

#define kASFixedZero		 	((ASFixed) 0x00000000L)
#define kASFixedOne				((ASFixed) 0x00010000L)
#define kASFixedMin			 	((ASFixed) 0x80000000L)
#define kASFixedMax 			((ASFixed) 0x7FFFFFFFL)
#define kASFixedUnknown 		((ASFixed) 0x80000000L)

typedef long double asdouble_t;

#define _ShortToASFixed(a)		((ASFixed)(a) << 16)

/*
 * Global Suite Pointer
 */

class IASFixed;
inline int	operator!=(const IASFixed& a, const IASFixed& b);
inline int	operator>(const IASFixed& a, const IASFixed& b);
inline int	operator>=(const IASFixed& a, const IASFixed& b);
inline int	operator<=(const IASFixed& a, const IASFixed& b);

class IASFixed
{
protected:
	ASFixed f;

	inline asdouble_t FixToX(ASFixed x);
	inline ASFixed XToFix(asdouble_t x);
	inline ASFixed FixMul  (ASFixed a, ASFixed b);
	inline ASFixed FixDiv(ASFixed x, ASFixed y);
	inline ASFixed FixedSumChk(ASFixed a, ASFixed b);
	
public:
	/*
	 * Constructors
	 */
	IASFixed();				
	IASFixed(const ASFixed a);
	IASFixed(const short a);
	IASFixed(const int a);
	IASFixed(const float a);	
	IASFixed(const double a);	
	
	/*
	 * Conversion operator
	 */
	operator ASFixed() const;
	operator short() const;
	operator int() const;
	operator float() const;
	operator double() const;
	
	/*
	 * Comparison Operators
	 */
	friend inline int	operator==(const IASFixed& a, const IASFixed& b);
	friend inline int	operator==(short a, const IASFixed& b);
	friend inline int	operator==(const IASFixed& a, short b);
	friend inline int	operator==(int a, const IASFixed& b);
	friend inline int	operator==(const IASFixed& a, int b);
	friend inline int	operator==(const float &a, const IASFixed& b);
	friend inline int	operator==(const IASFixed& a, const float &b);
	friend inline int	operator==(const double &a, const IASFixed& b);
	friend inline int	operator==(const IASFixed& a, const double &b);
	
	friend inline int	operator!=(const IASFixed& a, const IASFixed& b);
	friend inline int	operator!=(short a, const IASFixed& b);
	friend inline int	operator!=(const IASFixed& a, short b);
	friend inline int	operator!=(int a, const IASFixed& b);
	friend inline int	operator!=(const IASFixed& a, int b);
	friend inline int	operator!=(const float &a, const IASFixed& b);
	friend inline int	operator!=(const IASFixed& a, const float &b);
	friend inline int	operator!=(const double &a, const IASFixed& b);
	friend inline int	operator!=(const IASFixed& a, const double &b);
	
	friend inline int	operator>(const IASFixed& a, const IASFixed& b);
	friend inline int	operator>(short a, const IASFixed& b);
	friend inline int	operator>(const IASFixed& a, short b);
	friend inline int	operator>(int a, const IASFixed& b);
	friend inline int	operator>(const IASFixed& a, int b);
	friend inline int	operator>(const float &a, const IASFixed& b);
	friend inline int	operator>(const IASFixed& a, const float &b);
	friend inline int	operator>(const double &a, const IASFixed& b);
	friend inline int	operator>(const IASFixed& a, const double &b);
	
	friend inline int	operator<(const IASFixed& a, const IASFixed& b);
	friend inline int	operator<(short a, const IASFixed& b);
	friend inline int	operator<(const IASFixed& a, short b);
	friend inline int	operator<(int a, const IASFixed& b);
	friend inline int	operator<(const IASFixed& a, int b);
	friend inline int	operator<(const float &a, const IASFixed& b);
	friend inline int	operator<(const IASFixed& a, const float &b);
	friend inline int	operator<(const double &a, const IASFixed& b);
	friend inline int	operator<(const IASFixed& a, const double &b);

	friend inline int	operator>=(const IASFixed& a, const IASFixed& b);
	friend inline int	operator>=(short a, const IASFixed& b);
	friend inline int	operator>=(const IASFixed& a, short b);
	friend inline int	operator>=(int a, const IASFixed& b);
	friend inline int	operator>=(const IASFixed& a, int b);
	friend inline int	operator>=(const float &a, const IASFixed& b);
	friend inline int	operator>=(const IASFixed& a, const float &b);
	friend inline int	operator>=(const double &a, const IASFixed& b);
	friend inline int	operator>=(const IASFixed& a, const double &b);

	friend inline int	operator<=(const IASFixed& a, const IASFixed& b);
	friend inline int	operator<=(short a, const IASFixed& b);
	friend inline int	operator<=(const IASFixed& a, short b);
	friend inline int	operator<=(int a, const IASFixed& b);
	friend inline int	operator<=(const IASFixed& a, int b);
	friend inline int	operator<=(const float &a, const IASFixed& b);
	friend inline int	operator<=(const IASFixed& a, const float &b);
	friend inline int	operator<=(const double &a, const IASFixed& b);
	friend inline int	operator<=(const IASFixed& a, const double &b);

	/*
	 * Unary Minus
	 */
	friend inline IASFixed operator-(const IASFixed &a);
	
	/*
	 * Binary arithmetic operators
	 */
	friend inline IASFixed operator+(const IASFixed &a, const IASFixed &b);
	friend inline IASFixed operator-(const IASFixed &a, const IASFixed &b);
	friend inline IASFixed operator*(const IASFixed &a, const IASFixed &b);
	friend inline IASFixed operator/(const IASFixed &a, const IASFixed &b);
	
	/*
	 * "Unary" aritmetic opererators
	 */
	void operator+=(const IASFixed& a);
	void operator-=(const IASFixed& a);
	void operator*=(const IASFixed& a);
	void operator/=(const IASFixed& a);
	
	friend inline IASFixed operator>>(const IASFixed &a, int amount);
	friend inline IASFixed operator<<(const IASFixed &a, int amount);
	
	friend inline IASFixed abs(const IASFixed &a);
};

inline asdouble_t FixToX(ASFixed x)
{
	return ((asdouble_t)x) / 65536.0;
}


inline ASFixed XToFix(asdouble_t x)
{
	ASFixed fx;

	if (x - 1.0 >= (asdouble_t)0x00007fffL)
		fx = 0x7fffffffL;
	else if (x + 1.0 <= -(asdouble_t)0x00008000L)
		fx = 0x80000000L;
	else
		fx = (ASFixed)(x * (asdouble_t)0x00010000L + (x > 0.0 ? 0.5 : -0.5));

	return fx;
}


inline ASFixed FixedMul  (ASFixed a, ASFixed b)
{
	asdouble_t da, db;
	ASFixed fx;

	da = FixToX (a);
	db = FixToX (b);

	fx = XToFix (da * db);

	return fx;
}


inline ASFixed FixedDiv(ASFixed x, ASFixed y)
{
	if (!y)
		{
		if (x < 0)
			return 0x80000000L;
		else
			return 0x7fffffffL;
		}

	return XToFix ((asdouble_t)x / (asdouble_t)y);
}

inline ASFixed FixedSumChk(ASFixed a, ASFixed b)
{
	if (a == kASFixedMax || b == kASFixedMax)
		return kASFixedMax;
	else if (a == kASFixedMin || b == kASFixedMin)
		return kASFixedMin;
	else
		{
		ASFixed c;
		ASBoolean aNeg, bNeg;

		aNeg = a < kASFixedZero;
		bNeg = b < kASFixedZero;
		c = a + b;
		if (aNeg == bNeg && aNeg != (c < kASFixedZero))
			c = aNeg ? kASFixedMin : kASFixedMax;
		return c;
		}
}

inline IASFixed::IASFixed() 				
{ 
}
	
inline IASFixed::IASFixed(const ASFixed a)	
{ 
	f = a;
}
	
inline IASFixed::IASFixed(const short a)	
{ 
	f = ASFixed(a) << 16;
}
	
inline IASFixed::IASFixed(const int a)	
{ 
	f = ASFixed(a) << 16;
}
	
inline IASFixed::IASFixed(const float a)	
{ 
	f = long(a * 65536.0);
}

inline IASFixed::IASFixed(const double a)	
{ 
	f = long(a * 65536.0);
}

inline IASFixed::operator ASFixed() const 
{
	return f; 
}

inline IASFixed::operator short() const 
{
	return short(f >> 16); 
}

inline IASFixed::operator int() const 
{
	return int(f >> 16); 
}

inline IASFixed::operator float() const 
{
	return float(f / 65536.0);
}

inline IASFixed::operator double() const 
{
	return double(f / 65536.0);
}
	

inline int operator==(const IASFixed &a, const IASFixed &b) 
{
	return(a.f == b.f);
}
	
inline int operator==(short a, const IASFixed &b) 
{
	return(IASFixed(a).f != b.f);
}
	
inline int operator==(const IASFixed &a, short b) 
{
	return(a.f == IASFixed(b).f);
}
	
inline int operator==(int a, const IASFixed &b) 
{
	return(IASFixed(a).f == b.f);
}
	
inline int operator==(const IASFixed &a, int b) 
{
	return(a.f == IASFixed(b).f);
}
	
inline int operator==(const float &a, const IASFixed &b) 
{
	return(a == float(b));
}
	
inline int operator==(const IASFixed &a, const float &b) 
{
	return(float(a) == b);
}
	
inline int operator==(const double &a, const IASFixed &b) 
{
	return(a == double(b));
}
	
inline int operator==(const IASFixed &a, const double &b) 
{
	return(double(a) == b);
}
	

inline int operator!=(const IASFixed &a, const IASFixed &b) 
{
	return(a.f != b.f);
}
	
inline int operator!=(short a, const IASFixed &b) 
{
	return(IASFixed(a).f != b.f);
}
	
inline int operator!=(const IASFixed &a, short b) 
{
	return(a.f != IASFixed(b).f);
}
	
inline int operator!=(int a, const IASFixed &b) 
{
	return(IASFixed(a).f != b.f);
}
	
inline int operator!=(const IASFixed &a, int b) 
{
	return(a.f != IASFixed(b).f);
}
	
inline int operator!=(const float &a, const IASFixed &b) 
{
	return(a != float(b));
}
	
inline int operator!=(const IASFixed &a, const float &b) 
{
	return(float(a) != b);
}
	
inline int operator!=(const double &a, const IASFixed &b) 
{
	return(a != double(b));
}
	
inline int operator!=(const IASFixed &a, const double &b) 
{
	return(double(a) != b);
}
	

inline int operator>(const IASFixed &a, const IASFixed &b) 
{
	return(a.f > b.f);
}
	
inline int operator>(short a, const IASFixed &b) 
{
	return(IASFixed(a).f > b.f);
}
	
inline int operator>(const IASFixed &a, short b) 
{
	return(a.f > IASFixed(b).f);
}
	
inline int operator>(int a, const IASFixed &b) 
{
	return(IASFixed(a).f > b.f);
}
	
inline int operator>(const IASFixed &a, int b) 
{
	return(a.f > IASFixed(b).f);
}
	
inline int operator>(const float &a, const IASFixed &b) 
{
	return(a > float(b));
}
	
inline int operator>(const IASFixed &a, const float &b) 
{
	return(float(a) > b);
}
	
inline int operator>(const double &a, const IASFixed &b) 
{
	return(a > double(b));
}
	
inline int operator>(const IASFixed &a, const double &b) 
{
	return(double(a) > b);
}
	

inline int operator<(const IASFixed &a, const IASFixed &b) 
{
	return(a.f < b.f);
}
	
inline int operator<(short a, const IASFixed &b) 
{
	return(IASFixed(a).f < b.f);
}
	
inline int operator<(const IASFixed &a, short b) 
{
	return(a.f < IASFixed(b).f);
}
	
inline int operator<(int a, const IASFixed &b) 
{
	return(IASFixed(a).f < b.f);
}
	
inline int operator<(const IASFixed &a, int b) 
{
	return(a.f < IASFixed(b).f);
}
	
inline int operator<(const float &a, const IASFixed &b) 
{
	return(a < float(b));
}
	
inline int operator<(const IASFixed &a, const float &b) 
{
	return(float(a) < b);
}
	
inline int operator<(const double &a, const IASFixed &b) 
{
	return(a < double(b));
}
	
inline int operator<(const IASFixed &a, const double &b) 
{
	return(double(a) < b);
}
	

inline int operator>=(const IASFixed &a, const IASFixed &b) 
{
	return(a.f >= b.f);
}
	
inline int operator>=(short a, const IASFixed &b) 
{
	return(IASFixed(a).f >= b.f);
}
	
inline int operator>=(const IASFixed &a, short b) 
{
	return(a.f >= IASFixed(b).f);
}
	
inline int operator>=(int a, const IASFixed &b) 
{
	return(IASFixed(a).f >= b.f);
}
	
inline int operator>=(const IASFixed &a, int b) 
{
	return(a.f >= IASFixed(b).f);
}
	
inline int operator>=(const float &a, const IASFixed &b) 
{
	return(a >= float(b));
}
	
inline int operator>=(const IASFixed &a, const float &b) 
{
	return(float(a) >= b);
}
	
inline int operator>=(const double &a, const IASFixed &b) 
{
	return(a >= double(b));
}
	
inline int operator>=(const IASFixed &a, const double &b) 
{
	return(double(a) >= b);
}
	

inline int operator<=(const IASFixed &a, const IASFixed &b) 
{
	return(a.f <= b.f);
}
	
inline int operator<=(short a, const IASFixed &b) 
{
	return(IASFixed(a).f <= b.f);
}
	
inline int operator<=(const IASFixed &a, short b) 
{
	return(a.f <= IASFixed(b).f);
}
	
inline int operator<=(int a, const IASFixed &b) 
{
	return(IASFixed(a).f <= b.f);
}
	
inline int operator<=(const IASFixed &a, int b) 
{
	return(a.f <= IASFixed(b).f);
}
	
inline int operator<=(const float &a, const IASFixed &b) 
{
	return(a <= float(b));
}
	
inline int operator<=(const IASFixed &a, const float &b) 
{
	return(float(a) <= b);
}
	
inline int operator<=(const double &a, const IASFixed &b) 
{
	return(a <= double(b));
}
	
inline int operator<=(const IASFixed &a, const double &b) 
{
	return(double(a) <= b);
}
	

inline IASFixed operator-(const IASFixed &a)
{
	return -a.f;
}

inline IASFixed operator+(const IASFixed &a, const IASFixed &b)
{
	return a + b;
}

inline IASFixed operator-(const IASFixed &a, const IASFixed &b)
{
	return a - b;
}

inline IASFixed operator*(const IASFixed &a, const IASFixed &b)
{
	return FixedMul(a, b);
}

inline IASFixed operator/(const IASFixed &a, const IASFixed &b)
{
	return FixedDiv(a, b);
}

inline void IASFixed::operator+=(const IASFixed &a)
{
	*this = *this + a;
}

inline void IASFixed::operator-=(const IASFixed &a)
{
	*this = *this - a;
}

inline void IASFixed::operator*=(const IASFixed &a)
{
	*this = *this * a;
}

inline void IASFixed::operator/=(const IASFixed &a)
{
	*this = *this / a;
}

inline IASFixed operator>>(const IASFixed &a, int amount)
{
	return a.f >> amount;
}

inline IASFixed operator<<(const IASFixed &a, int amount)
{
	return a.f << amount;
}

inline IASFixed abs(const IASFixed &a)
{
	if (a.f >= 0)
		return a.f;
	else
		return -a.f; 
}

#endif


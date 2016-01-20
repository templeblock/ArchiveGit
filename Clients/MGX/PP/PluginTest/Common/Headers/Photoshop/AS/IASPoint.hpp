/*
 * Name:
 *	IASPoint.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IASPoint Wrapper Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/26/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IASPoint_hpp__
#define __IASPoint_hpp__

/*
 * Includes
 */
 
#ifndef __ASTypes__
#include "ASTypes.h"
#endif

/*
 * Wrapper Class
 */

struct IASPoint : public ASPoint
{
public:	
	IASPoint();
	IASPoint(const ASPoint &p);
	IASPoint(int h, int v);
	
	friend inline ASBoolean operator == (const ASPoint &a, const ASPoint &b);
	friend inline ASBoolean operator != (const ASPoint &a, const ASPoint &b);
	
	friend inline ASPoint operator + (const ASPoint &a, const ASPoint& b);
	friend inline ASPoint operator - (const ASPoint &a, const ASPoint& b);

	friend inline ASPoint operator * (const ASPoint &p, int s);
	friend inline ASPoint operator * (int s, const ASPoint& p);

	friend inline ASPoint operator / (const ASPoint &p, int s);

	friend inline ASPoint operator - (const ASPoint &p);

	void operator = (const ASPoint &p);
	void operator += (const ASPoint &p);
	void operator -= (const ASPoint &p);

	void operator *= (int s);
	void operator /= (int s);
};


inline IASPoint::IASPoint()
{
}


inline IASPoint::IASPoint(const ASPoint &p)	
{ 
	h = p.h; 
	v = p.v; 
}


inline IASPoint::IASPoint(int h, int v)
{ 
	this->h = h; this->v = v; 
}


inline ASBoolean operator == (const ASPoint &a, const ASPoint &b)
{
	return a.h == b.h && a.v == b.v;
}
	

inline ASBoolean operator != (const ASPoint &a, const ASPoint &b)
{
	return a.h != b.h || a.v != b.v;
}


inline ASPoint operator + (const ASPoint &a, const ASPoint &b)
{
	return IASPoint(a.h + b.h, a.v + b.v);
}


inline ASPoint operator - (const ASPoint &a, const ASPoint &b)
{
	return IASPoint(a.h - b.h, a.v - b.v);
}


inline ASPoint operator * (const ASPoint &p, int s)
{
	return IASPoint(p.h * s, p.v * s);
}


inline ASPoint operator * (int s, const ASPoint &p)
{
	return IASPoint(p.h * s, p.v * s);
}


inline ASPoint operator / (const ASPoint &p, int s)
{
	return IASPoint(p.h / s, p.v / s);
}


inline ASPoint operator - (const ASPoint &p)
{
	return IASPoint(-p.h, -p.v);
}


inline void IASPoint::operator = (const ASPoint& p) 
{
	h = p.h;
	v = p.v;
}


inline void IASPoint::operator += (const ASPoint& p) 
{ 
	h += p.h;
	v += p.v; 
}


inline void IASPoint::operator -= (const ASPoint& p) 
{ 
	h -= p.h; 
	v -= p.v; 
}


inline void IASPoint::operator *= (int s)
{ 
	h *= s;
	v *= s; 
}


inline void IASPoint::operator /= (int s)
{
	h /= s;
	v /= s; 
}


#endif









/*
 * Name:
 *	IASRect.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	IASRect Wrapper Class
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 1/26/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IASRect_hpp__
#define __IASRect_hpp__

/*
 * Includes
 */
 
#ifndef __ASTypes__
#include "ASTypes.h"
#endif

/*
 * Wrapper Class
 */

struct IASRect : public ASRect
{
public:	
	IASRect();
	IASRect(const ASRect &rect);
	IASRect(const ASPoint &a);
	IASRect(const ASPoint &a, const ASPoint &b);
	IASRect(int left, int top, int right, int bottom);
	
	int Width() const;
	int Height() const;
	
	ASBoolean IsEmpty() const;
	void SetEmpty();
	
	ASBoolean IsNormalized() const;	
	void Normalize();

	int Area() const;
	
	ASPoint Center() const;
	
	ASBoolean Contains(const ASPoint &p) const;
	ASBoolean Contains(const ASRect &r) const;
	
	ASBoolean Overlaps(const ASRect &r) const;
	
	IASRect &operator ++ ();
	IASRect &operator -- ();
	IASRect operator ++ (int);
	IASRect operator -- (int);
	IASRect &operator += (int n);
	IASRect &operator -= (int n);
	
	IASRect &operator += (ASPoint &p);
	IASRect &operator -= (ASPoint &p);
	
	IASRect &operator = (ASPoint &p);
	
	void Inset(int h, int v);
	void Inflate(int h, int v);
	void Offset(int h, int v);
	
	void Union(const ASPoint &a);
	void Union(const ASRect &a);
	void Union(const ASRect &a, const ASRect &b);
	
	void Intersect(const ASRect &a);
	void Intersect(const ASRect &a, const ASRect &b);
	
	void Center(const ASRect &centerRect);
	
	friend inline ASBoolean operator == (const ASRect &a, const ASRect &b);
	friend inline ASBoolean operator != (const ASRect &a, const ASRect &b);

// Doesn't work in CodeWarrior 1.4.  Anyone know why?  DRL 4/22/96	
//	friend inline ASRect operator + (const IASRect &a, const IASPoint &b);
//	friend inline ASRect operator + (const IASPoint &a, const IASRect &b);
//	friend inline ASRect operator - (const IASRect &a, const IASPoint &b);
//	friend inline ASRect operator - (const IASPoint &a, const IASRect &b);
};


inline IASRect::IASRect() 					
{
}


inline IASRect::IASRect(const ASRect &rect) 
{ 
	left = rect.left;
	top = rect.top;
	right = rect.right;
	bottom = rect.bottom; 
}


inline IASRect::IASRect(const ASPoint &a)	
{ 
	left = right = a.h; 
	top = bottom = a.v; 
}


inline IASRect::IASRect(const ASPoint &a, const ASPoint &b)
{
	if (a.h < b.h)
	{
		left = a.h;
		right = b.h;
	}
	else
	{
		left = b.h;
		right = a.h;
	}
	
	if (a.v < b.v)
	{
		top = a.v;
		bottom = b.v;
	}
	else
	{
		top = b.v;
		bottom = a.v;
	}
}


inline IASRect::IASRect(int left, int top, int right, int bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}


inline int IASRect::Width()	const				
{ 
	return right - left; 
}


inline int IASRect::Height() const			
{ 
	return bottom - top; 
}


inline ASBoolean IASRect::IsEmpty()	const		
{ 
	return top == bottom && right == left; 
}


inline void IASRect::SetEmpty()				
{ 
	left = top = right = bottom = 0; 
}


inline ASBoolean IASRect::IsNormalized() const
{
	return left <= right && top <= bottom;
}


inline void IASRect::Normalize()
{
	if (left > right) 
	{
		int saveLeft = left; left = right; right = saveLeft;
	}
	if (top > bottom)
	{
		int saveTop = top; top = bottom; bottom = top;
	}
}


inline int IASRect::Area() const					
{ 
	return Width() * Height(); 
}

inline ASPoint IASRect::Center() const
{
	ASPoint center;
	
	center.h = left + Width() / 2;
	center.v = top + Height() / 2;
	
	return center;
}

inline ASBoolean IASRect::Contains(const ASPoint &p) const
{
	return left <= p.h && right >= p.h && top <= p.v && bottom >= p.v;
}


inline ASBoolean IASRect::Contains(const ASRect &r) const
{
	return left <= r.left && right >= r.left && top <= r.top && bottom >= r.top &&
		   left <= r.right && right >= r.right && top <= r.bottom && bottom >= r.bottom;
}


inline ASBoolean IASRect::Overlaps(const ASRect &r) const
{
	return  right  >= r.left && left <= r.right  && 
		    bottom >= r.top  && top  <= r.bottom;
}


inline IASRect &IASRect::operator ++ ()
{ 
	left--; 
	top--; 
	right++; 
	bottom++; 
	
	return *this; 
}

inline IASRect &IASRect::operator -- ()		
{ 	
	left++; 
	top++; 
	right--; 
	bottom--; 
	
	return *this; 
}


inline IASRect IASRect::operator ++ (int)
{ 
	left--; 
	top--; 
	right++; 
	bottom++; 
	
	return *this; 
}

inline IASRect IASRect::operator -- (int)		
{ 
	left++; 
	top++; 
	right--; 
	bottom--; 
	
	return *this; 
}


inline IASRect &IASRect::operator += (int n)	
{ 
	left -= n; 
	top -= n; 
	right += n; 
	bottom += n; 
	
	return *this; 
}


inline IASRect &IASRect::operator -= (int n)
{ 
	left += n; 
	top += n; 
	right -= n; 
	bottom -= n; 
	
	return *this; 
}

inline IASRect &IASRect::operator += (ASPoint &p)	
{ 
	left += p.h; 
	top += p.v; 
	right += p.h; 
	bottom += p.v; 
	
	return *this; 
}


inline IASRect &IASRect::operator -= (ASPoint &p)
{ 
	left -= p.h; 
	top -= p.v; 
	right -= p.h; 
	bottom -= p.v; 
	
	return *this; 
}

	
inline IASRect &IASRect::operator = (ASPoint &p)
{
	left = right = p.h;
	top = bottom = p.h;
	
	return *this;
}


inline ASBoolean operator == (const ASRect &a, const ASRect &b)
{
	return a.left == b.left &&
		   a.top == b.top &&
		   a.right == b.right &&
		   a.bottom == b.bottom;
}


inline ASBoolean operator != (const ASRect &a, const ASRect &b)
{
	return a.left != b.left ||
		   a.top != b.top ||
		   a.right != b.right ||
		   a.bottom != b.bottom;
}


inline void IASRect::Inset(int h, int v)
{ 
	left += h; 
	top += v; 
	right -= h; 
	bottom -= v; 
}	


inline void IASRect::Inflate(int h, int v)
{ 
	left -= h; 
	top -= v; 
	right += h; 
	bottom += v; 
}	


inline void IASRect::Offset(int h, int v)
{ 
	left += h; 
	top += v; 
	right += h; 
	bottom += v; 
}	


inline void IASRect::Union(const ASPoint &a)
{
	if (left > a.h)
		left = a.h;
	if (top > a.v)
		top = a.v;
	if (right < a.h)
		right = a.h;
	if (bottom < a.v)
		bottom = a.v; 
}


inline void IASRect::Union(const ASRect &a)
{
	if (left > a.left)
		left = a.left;
	if (top > a.top)
		top = a.top;
	if (right < a.right)
		right = a.right;
	if (bottom < a.bottom)
		bottom = a.bottom; 
}


inline void IASRect::Union(const ASRect &a, const ASRect &b)
{
	if (b.left > a.left)
		left = a.left;
	else
		left = b.left;
	if (b.top > a.top)
		top = a.top;
	else
		top = b.top;
	if (b.right < a.right)
		right = a.right;
	else
		right = b.right;
	if (b.bottom < a.bottom)
		bottom = a.bottom;
	else
		bottom = b.bottom; 
}


inline void IASRect::Intersect(const ASRect &a)
{
	if (left < a.left)
		left = a.left;
	if (top < a.top)
		top = a.top;
	if (right > a.right)
		right = a.right;
	if (bottom > a.bottom)
		bottom = a.bottom;
		
	if (!Overlaps(a))
		SetEmpty();	
}


inline void IASRect::Intersect(const ASRect &a, const ASRect &b)
{
	if (b.left < a.left)
		left = a.left;
	else
		left = b.left;
	if (b.top < a.top)
		top = a.top;
	else
		top = b.top;
	if (b.right > a.right)
		top = a.right;
	else
		top = b.right;
	if (b.bottom > a.bottom)
		bottom = a.bottom;
	else
		bottom = b.bottom;			
	if (!Overlaps(a))
		SetEmpty();	
} 


inline void IASRect::Center(const ASRect &centerRect)
{
	Offset(centerRect.left + ((centerRect.right - centerRect.left) - Width()) / 2,
		   centerRect.top + ((centerRect.bottom - centerRect.top) - Height()) / 2);
}


/*
 * Doesn't work in CodeWarrior 1.4.  Anyone know why?  DRL 4/22/96.
inline ASRect operator + (const IASRect &a, const IASPoint &b)
{
	return IASRect(a.left + b.h, a.top + b.v, a.right + b.h, a.bottom + b.v);
}

inline ASRect operator + (const IASPoint &a, const IASRect &b)
{
	IASRect r;
	
	r.left = b.left + a.h;
	r.top = b.top + a.v;
	r.right = b.right + a.h;
	r.bottom = b.bottom + a.v;
	
	return r;
}

inline ASRect operator - (const IASRect &a, const IASPoint &b)
{
	IASRect r;
	
	r.left = a.left - b.h;
	r.top = a.top - b.v;
	r.right = a.right - b.h;
	r.bottom = a.bottom - b.v;
	
	return r;
}

inline ASRect operator - (const IASPoint &a, const IASRect &b)
{
	IASRect r;
	
	r.left = b.left - a.h;
	r.top = b.top - a.v;
	r.right = b.right - a.h;
	r.bottom = b.bottom - a.v;
	
	return r;
}
*/

#endif









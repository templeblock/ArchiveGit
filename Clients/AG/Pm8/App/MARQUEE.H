/*
// $Header: /PM8/App/MARQUEE.H 1     3/03/99 6:08p Gbeddow $
//
// Marquee definitions.
//
// $Log: /PM8/App/MARQUEE.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:28:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:48   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:44   JAY
// Reverted. New series.
// 
//    Rev 1.0   16 Dec 1993 16:28:52   JAY
//  
*/

#ifndef __MARQUEE_H__
#define __MARQUEE_H__

class Marquee
{
private:

	typedef enum TRISTATE { DRAW, UNDRAW, ANIMATE };

	BOOL			drawn;
	HWND			hWnd;

	void drawSelf( TRISTATE ts, HDC in_hDC );

public:

	RECT			r;

	Marquee(HWND hWndOwner) : hWnd(hWndOwner), drawn(FALSE) {}

	~Marquee() { undraw(); }

	void draw(HDC hDC = 0);
	void animate(HDC hDC = 0);
	void undraw(HDC hDC = 0);

	void init();
};

#endif			// #ifndef __MARQUEE_H__

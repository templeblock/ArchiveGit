/*
// $Header: /PM8/App/ROTUTILS.H 1     3/03/99 6:11p Gbeddow $
//
// Rotation utility definitions.
//
// $Log: /PM8/App/ROTUTILS.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.1   17 Sep 1997 09:12:34   Jay
// Bitmap rotation
// 
//    Rev 1.0   14 Aug 1997 15:25:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:46   Fred
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 17:11:54   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:48:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:10   FRED
// Initial revision.
// 
//    Rev 2.1   26 Jun 1995 10:08:22   JAY
// Got rid of doubles. Now uses CFixed.
// 
//    Rev 2.0   07 Feb 1995 15:48:56   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:14   JAY
//  
*/

#ifndef __ROTUTILS_H__
#define __ROTUTILS_H__

typedef struct
{
	LPBYTE data;
	int width;
	int height;
	int bytes_per_row;
	int bytes_per_pixel;					// 1 or 3 usually
	RECT	bound;								// portion within the map
} ROTATE_MAP, far *ROTATE_MAP_PTR;

typedef struct
{
	POINT center;
	ANGLE angle;
//	double dAlpha;								// == -tan(angle/2);
//	double dBeta;								// == sin(angle);
} ROTATE_PARMS, far *ROTATE_PARMS_PTR;

extern int skew_number(int n, double dSkew);
extern void skew_x(ROTATE_MAP_PTR map, double dSkew, int center_y);
extern void skew_y(ROTATE_MAP_PTR map, double dSkew, int center_x, int x_offset);
extern void rotate_it(ROTATE_MAP_PTR map, ROTATE_PARMS_PTR parms);
extern void rotate_it_offset(ROTATE_MAP_PTR map, ROTATE_PARMS_PTR parms, int offset_center_x);

#endif					// #ifndef __ROTUTILS_H__

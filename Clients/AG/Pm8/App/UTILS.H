/*
// $Header: /PM8/App/UTILS.H 1     3/03/99 6:13p Gbeddow $
//
// Utility routine definitions.
//
// $Log: /PM8/App/UTILS.H $
// 
// 1     3/03/99 6:13p Gbeddow
 * 
 * 4     1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 3     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 2     5/26/98 2:39p Jayn
// New table code.
// 
//    Rev 1.0   14 Aug 1997 15:26:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:26   Fred
// Initial revision.
// 
//    Rev 1.6   14 Feb 1997 09:35:22   Jay
// Made scale_number() use ints
// 
//    Rev 1.5   31 Jan 1997 15:51:16   Jay
// Some new toolbar bitmaps.
// 
//    Rev 1.4   24 Jan 1997 12:39:28   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.3   08 Jan 1997 15:22:32   Fred
// More text box changes
// 
//    Rev 1.2   01 Nov 1996 17:11:54   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.1   08 Jul 1996 08:49:10   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.0   14 Mar 1996 13:47:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:18   FRED
// Initial revision.
// 
//    Rev 2.2   25 Apr 1995 16:02:30   FRED
// 
//    Rev 2.1   25 Apr 1995 10:33:46   FRED
// 
//    Rev 2.0   07 Feb 1995 15:49:08   JAY
// Initial revision.
// 
//    Rev 1.10   04 Aug 1994 07:33:48   JAY
// Large model and 32-bit changes
// 
//    Rev 1.9   18 Jul 1994 09:55:44   JAY
// Added access_file() to replace C library access routine.
// 
//    Rev 1.8   15 Jul 1994 11:28:54   JAY
// access_file().
// 
//    Rev 1.7   26 May 1994 13:40:48   JAY
// Added append_slash() and remove_slash().
// 
//    Rev 1.6   19 May 1994 08:47:48   JAY
// Changed get_base_path() to reference LPCSTRs instead of LPSTRs.
// 
//    Rev 1.5   31 Mar 1994 16:02:48   JAY
// Added get_base_name()
// 
//    Rev 1.4   07 Feb 1994 15:58:38   MACDUFF
// Added optional Clip parameter to xor_line and do_XorBox
// 
//    Rev 1.3   30 Jan 1994 09:01:02   JAY
// Added build_file_path for general use to UTILS.
// 
//    Rev 1.2   19 Jan 1994 17:08:04   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:24   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:54   JAY
//  
*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include "redisp.h"

extern BYTE near bit_masks[];			/* [0..7]; bit position in byte */

extern int midpoint(int n1, int x2);
extern int scale_number(int x, int n, int d);
extern PCOORD scale_pcoord(PCOORD x, PCOORD n, PCOORD d);

extern RGBCOLOR rgbcolor_from_color(COLOR color);
extern BYTE rgbcolor_to_gray(RGBCOLOR rgbcolor);

extern int mulrem(int x, int n, int d);

extern ANGLE const PI;					/* PI */
extern ANGLE const PI2;					/* PI*2 */
extern ANGLE const DEG2RADC;
extern ANGLE const RAD2DEGC;
#define DEG2RAD(a)	(((ANGLE)a)*DEG2RADC)
#define RAD2DEG(a)	((SHORT)(a*RAD2DEGC))

extern ANGLE normalize_angle(ANGLE angle);

extern BOOL IntersectBox(PBOX_PTR result, PBOX_PTR a, PBOX_PTR b);
extern VOID UnionBox(PBOX_PTR result, PBOX_PTR a, PBOX_PTR b );

extern COLOR ColorFromRColor( const RColor& rColor );
extern RColor RColorFromColor( COLOR color );

extern COLOR color_from_colorref(COLORREF color);
extern COLORREF colorref_from_color(COLOR color);

extern VOID rotate_points(PPNT_PTR p, SHORT points, PPNT center, ANGLE angle);
extern void RotatePoints(POINT* p, int nPoints, POINT center, double dAngle);

extern VOID compute_rotated_bound(PBOX normal_bound, ANGLE rotation, PBOX_PTR out_bound, PPNT origin, PPNT_PTR verts = NULL);
extern VOID compute_enclosing_bound(PPNT_PTR pp, SHORT count, PBOX_PTR out_bound);

// The "control dot" width. It helps if this is an odd number.
#define CONTROL_DOT_WIDTH	7
//#define SELECT_COLOR		RGB(192, 192, 192)
#define SELECT_COLOR		RGB(255, 255, 255)
extern VOID do_XorBox(HDC hdc, RECT r, int width = 1, RECT *prectClip = NULL);
extern VOID ControlDot(HDC hdc, int x, int y);
extern VOID ControlDot(HDC hdc, int x, int y, ANGLE dAngle, int nRotateX, int nRotateY);
extern VOID xor_line(HDC hdc, int x0, int y0, int x1, int y1, RECT *prectClip=NULL);
extern VOID xor_polyline(HDC hdc, POINT *p, int points, BOOL close);
extern VOID XorBox(RedisplayContextPtr rc, PBOX pbox);

extern int16 bits_required_by(uint32 nn);
inline int16 bits_required_by_long(int32 nn)
	{ return bits_required_by((uint32)(nn < 0 ? -nn : nn)); }
extern long ext_SafeLongMulDiv(long a, long b, long c);

extern VOID float_to_string(CHAR *buffer, double f);

extern VOID build_path_name(LPSTR out, LPCSTR path, LPCSTR name);

extern LPCSTR get_base_name(LPCSTR in_name);

extern VOID append_slash(LPSTR name);
extern VOID remove_slash(LPSTR name);

extern int access_file(PCSTR pName, int mode = 0);

/*
// Check if it's safe to convert to CFixed coordinates.
*/

extern BOOL SafeToConvertPboxToFixed(PBOX* p);
extern BOOL SafeToConvertPpntToFixed(PPNT* p);

extern void ScalePBOXWithRounding(PBOX_PTR pbox, SHORT x_num, SHORT x_denom, SHORT y_num, SHORT y_denom);

#endif					// #ifndef __UTILS_H__

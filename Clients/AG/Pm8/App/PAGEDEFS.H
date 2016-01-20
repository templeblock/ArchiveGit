#ifndef __PAGEDEFS_H__
#define __PAGEDEFS_H__

#define PAGE_RESOLUTION	1800

/*
// Page orientations.
*/

enum ORIENTATION
{
  ORIENTATION_INVALID = -1,
  PORTRAIT = 0,
	LANDSCAPE = 1
};

/*
// Page coordinate system.
*/

/* A page coordinate. */

typedef	long	PCOORD, far *PCOORD_PTR;

/* A page point. */

struct PPNT
{
	PCOORD	x;
	PCOORD	y;
};
typedef PPNT far *PPNT_PTR, far &PPNT_REF;

/* A page box. */

struct PBOX
{
	PCOORD	x0;
	PCOORD	y0;
	PCOORD	x1;
	PCOORD	y1;

   PCOORD
   Width (void)
   {
      return x1 - x0;
   }

   PCOORD
   Height (void)
   {
      return y1 - y0;
   }

   PCOORD
   CenterX (void)
   {
      return x0 + (Width () / 2);
   }
   
   PCOORD
   CenterY (void)
   {
      return y0 + (Height () / 2);
   }

   void
   Empty(void)
   {
      x0 = y0 = x1 = y1 = 0;
   }

   BOOL
   IsEmpty(void)
   {
      if ((Width() != 0) || (Height() != 0))
         return FALSE;
      return TRUE;
   }
};
typedef PBOX far *PBOX_PTR, far &PBOX_REF;

/*
// Conversion from and to inches.
*/

#define PageToInches(/*PCOORD*/p)			(DivFixed(p, PAGE_RESOLUTION))
#define InchesToPage(/*CFixed*/cf)		((PCOORD)MulFixed(cf, PAGE_RESOLUTION))

// Maximum dimension of an object (16000" = 1/4 mile)
#define	MAXIMUM_OBJECT_DIMENSION	(16000L*PAGE_RESOLUTION)

/*
// Color definitions.
//
// A "color" is a 32 bit value, with 8 bits for each of red, green and blue.
*/

typedef UCHAR COLOR_COMPONENT;

typedef ULONG COLOR;
typedef ULONG RGBCOLOR;
typedef COLOR FAR *LPCOLOR;

#define MAKE_RGBCOLOR(r, g, b)		(((COLOR)(r&255)<<16) +\
												((COLOR)(g&255)<<8) +\
 												((COLOR)(b&255)))

#define MAKE_COLOR(c, m, y, k)		(((COLOR)(k&255)<<24) +\
												((COLOR)(c&255)<<16) +\
 												((COLOR)(m&255)<<8) +\
 												((COLOR)(y&255)))

/* RGBCOLOR is ??RRGGBB */

#define RED_COMPONENT_OF(rgbcolor)		((COLOR_COMPONENT)((rgbcolor>>16)&255))
#define GREEN_COMPONENT_OF(rgbcolor)	((COLOR_COMPONENT)((rgbcolor>>8)&255))
#define BLUE_COMPONENT_OF(rgbcolor)		((COLOR_COMPONENT)(rgbcolor&255))

/* COLOR is KKCCMMYY */

#define CYAN_COMPONENT_OF(color)			((COLOR_COMPONENT)((color>>16)&255))
#define MAGENTA_COMPONENT_OF(color)		((COLOR_COMPONENT)((color>>8)&255))
#define YELLOW_COMPONENT_OF(color)		((COLOR_COMPONENT)(color&255))
#define BLACK_COMPONENT_OF(color)		((COLOR_COMPONENT)((color>>24)&255))

#define	COLOR_BLACK				MAKE_COLOR(  0,   0,   0, 255)

#define	COLOR_DARK_BLUE		MAKE_COLOR(255, 255,   0, 127)
#define	COLOR_DARK_GREEN		MAKE_COLOR(255,   0, 255, 127)
#define	COLOR_DARK_CYAN		MAKE_COLOR(255,   0,   0, 127)
#define	COLOR_DARK_RED			MAKE_COLOR(  0, 255, 255, 127)
#define	COLOR_DARK_MAGENTA	MAKE_COLOR(  0, 255,   0, 127)
#define	COLOR_BROWN				MAKE_COLOR(  0,   0, 255, 127)

#define	COLOR_LIGHT_GRAY		MAKE_COLOR(  0,   0,   0, 127)

#define	COLOR_BLUE				MAKE_COLOR(255, 255,   0,   0)
#define	COLOR_GREEN				MAKE_COLOR(255,   0, 255,   0)
#define	COLOR_CYAN				MAKE_COLOR(255,   0,   0,   0)
#define	COLOR_RED				MAKE_COLOR(  0, 255, 255,   0)
#define	COLOR_MAGENTA			MAKE_COLOR(  0, 255,   0,   0)
#define	COLOR_YELLOW			MAKE_COLOR(  0,   0, 255,   0)

#define	COLOR_WHITE				MAKE_COLOR(  0,   0,   0,   0)

#define	RGBCOLOR_BLACK			MAKE_RGBCOLOR(  0,   0,   0)
#define	RGBCOLOR_WHITE			MAKE_RGBCOLOR(255, 255, 255)

#define	COLOR_FROM_RGBCOLOR(rgbcolor)	((rgbcolor)^0x00FFFFFFL)
#define	COLOR_FROM_RGB(r, g, b)			MAKE_COLOR((r)^0xFF,(g)^0xFF,(b)^0xFF,0)
#define	COLOR_FROM_COLORREF(rgb)		color_from_colorref(rgb)
#define	COLORREF_FROM_COLOR(color)		colorref_from_color(color)
#define	RGBCOLOR_FROM_COLOR(color)		rgbcolor_from_color(color)

#define UNDEFINED_COLOR	((COLOR)-1)
#define TRANSPARENT_COLOR ((COLOR)-2)

// Convert an existing COLORREF to a PALETTERGB COLORREF.
// On 256-color systems, this will map the color to the nearest color
// in the current palette (see the standard PALETTERGB() macro).

#define	MAKE_PALETTERGB(rgb)	((rgb)|0x02000000L)

/*
// An RGB triple (not to be confused with the Windows RGBTRIPLE).
*/

typedef COLOR_COMPONENT RGBTriple[3];

/*
// Some useful PBOX macros.
*/

#define IsPtInBox(x, y, b) ((x) >= (b)->x0 && (x) <= (b)->x1 && (y) >= (b)->y0 && (y) <= (b)->y1)
#define IsBoxOnBox(b0, b1) ((b0)->x0 < (b1)->x1 && (b0)->y0 < (b1)->y1 && (b0)->x1 > (b1)->x0 && (b0)->y1 > (b1)->y0)
#define IsBoxInBox(b0, b1) ((b0)->x0 >= (b1)->x0 && (b0)->y0 >= (b1)->y0 && (b0)->x1 <= (b1)->x1 && (b0)->y1 <= (b1)->y1)

/*
// A rotation angle.
// Angles are stored in radians since the math package needs them that way
// and we only need to convert them for humans which isn't very often.
*/

typedef double ANGLE;

// Clip a value to a maximum distance from another value.
inline PCOORD ClipToMaximumObjectDimension(PCOORD c, PCOORD b = 0)
{
	if (labs(c-b) > MAXIMUM_OBJECT_DIMENSION)
	{
		if (c >= b)
		{
			c = b+MAXIMUM_OBJECT_DIMENSION;
		}
		else
		{
			c = b-MAXIMUM_OBJECT_DIMENSION;
		}
	}

	return c;
}

// ****************************************************************************
//
//  Function Name:	PageResolutionToSystemDPI()
//
//  Description:		Converts units expressed in terms of PAGE_RESOLUTION into
//							units in terms of kSystemDPI
//
//	 Note:	This function arises from the integration of historical Print Master
//				code base with historical Print Shop code base.  Print Master dealt
//				with PAGE_RESOLUTION units and Print Shop with kSystemDPI.
//
//  Returns:			Units in terms of kSystemDPI
//
// ****************************************************************************
inline YIntDimension PageResolutionToSystemDPI( PCOORD n )
{
	return (YIntDimension)(n * kSystemDPI) / PAGE_RESOLUTION;
}

// ****************************************************************************
//
//  Function Name:	SystemDPIToPageResolution()
//
//  Description:		Converts units expressed in terms of kSystemDPI into
//							units in terms of PAGE_RESOLUTION
//
//	 Note:	This function arises from the integration of historical Print Master
//				code base with historical Print Shop code base.  Print Master dealt
//				with PAGE_RESOLUTION units and Print Shop with kSystemDPI.
//
//  Returns:			Units in terms of PAGE_RESOLUTION
//
// ****************************************************************************
inline PCOORD SystemDPIToPageResolution( YIntDimension n )
{
	return (PCOORD)(n * PAGE_RESOLUTION) / kSystemDPI;
}

#endif					/* #ifndef __PAGEDEFS_H__ */

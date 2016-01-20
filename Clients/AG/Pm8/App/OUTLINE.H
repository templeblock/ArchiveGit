/*
// OUTLINE.H
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/OUTLINE.H 1     3/03/99 6:08p Gbeddow $
//
// $Log: /PM8/App/OUTLINE.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 6     7/12/98 5:33p Jayn
// Fix for WMFs with Polylines drawn with NULL pen.
// 
// 5     12/09/97 4:31p Jayn
// Bitmap and bitmap rotation (incomplete)
// 
// 4     12/08/97 2:52p Jayn
// Text, arcs, pie charts, improved transform, etc.
// 
// 3     11/25/97 10:57a Jayn
// New and improved OLE/WMF support.
// 
// 2     11/19/97 3:43p Jayn
// New WMF parser.
// 
//    Rev 1.0   14 Aug 1997 15:23:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:54   Fred
// Initial revision.
// 
//    Rev 1.10   23 May 1997 16:13:48   Jay
// Support for nFX winding
// 
//    Rev 1.9   24 Apr 1997 15:08:44   Jay
// Fixed a bug in GetPointBuffer() with large number of points
// 
//    Rev 1.8   31 Mar 1997 17:12:38   Jay
// Start of NFX code
// 
//    Rev 1.7   24 Mar 1997 10:34:46   Jay
// Fixed pallete/non-palette bug with brush caching.
// 
//    Rev 1.6   22 Jan 1997 10:24:36   dennisp
// Optimized draw_polygon by retaining allocated buffer for repeated calls to
// draw_polygon.  Added GetPointBuffer().
// 
//    Rev 1.5   17 Jan 1997 14:42:24   dennisp
// Changed ASSERT to VERIFY macro.
// 
//    Rev 1.4   15 Jan 1997 16:43:44   dennisp
// Added caching of brushes.
// 
//    Rev 1.3   09 Dec 1996 10:19:48   Jay
// More image stuff.
// 
//    Rev 1.2   05 Dec 1996 12:30:38   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   01 Nov 1996 17:11:54   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:48:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:26   FRED
// Initial revision.
// 
//    Rev 2.3   07 Jul 1995 16:10:50   JAY
// Misc changes.
// 
//    Rev 2.2   23 Jun 1995 18:03:28   JAY
// Now uses polygons to draw rectangles.
// 
//    Rev 2.1   29 Mar 1995 08:58:36   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:48:48   JAY
// Initial revision.
// 
//    Rev 1.8   14 Jun 1994 16:51:10   JAY
// Doesn't use Geronimo anymore (code-wise); it's commented out.
// 
//    Rev 1.7   23 May 1994 08:24:44   JAY
// Moved the constructor into outline.cpp
// 
//    Rev 1.6   15 Mar 1994 15:13:12   JAY
// Now picks the smaller scaled pen width (x vs y expansion) to keep it from
// getting really large from a single dimension.
// 
//    Rev 1.5   24 Feb 1994 15:31:28   JAY
// delete_bitmaps() now takes a 'force' parameter to assist with the gdi calls
// code needing to remember its bitmaps.
// 
//    Rev 1.4   22 Feb 1994 15:02:20   JAY
// New rasterizing code for CGMs which uses GDI calls.
// 
//    Rev 1.3   25 Jan 1994 15:18:10   JAY
// More
// 
//    Rev 1.2   05 Jan 1994 08:08:28   JAY
// Restructuring, etc.
// 
//    Rev 1.1   30 Dec 1993 16:11:06   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:10   JAY
//  
*/

#ifndef __OUTLINE_H__
#define __OUTLINE_H__

#include "paint.h"
#include "file.h"

//#include "geronimo.h"
#include "dlist.h"
#include "outpath.h"

#define	OUTLINE_UPDATE_DELAY	1000
#define	OUS_NEAR near

#define FIX_CGMSx

#define MAX_OUTLINE_PLANES 24

class CBitmapObjectImage;

typedef struct
{
	SHORT x;
	SHORT y;
} OUTLINE_POINT;

typedef struct
{
	SHORT red;
	SHORT green;
	SHORT blue;
} OUTLINE_COLOR;

typedef enum
{
	MARKER_DOT,
	MARKER_PLUS,
	MARKER_ASTERISK,
	MARKER_CIRCLE,
	MARKER_CROSS,
	MARKER_RESERVED
} MARKER_TYPE;

typedef struct
{
	BITMAPINFOHEADER	bmiHeader;
	SHORT					bmiColors[256];			/* 256 palette entries */
	BYTE					bmiData[8*8];				/* bitmap data */
} DIBPATTERN, far *LPDIBPATTERN;

class CBrushDoubleLinkedListItem : public tDoubleLinkedListItem
{
protected:
	HBRUSH    m_hBrush;
	COLORREF  m_dwColorRef;
	BOOL		 m_fPalette;

public:
	CBrushDoubleLinkedListItem()     { m_hBrush = 0; }
	~CBrushDoubleLinkedListItem()    { if(m_hBrush) VERIFY(::DeleteObject(m_hBrush));}
	HBRUSH    GetBrush()                     { return m_hBrush; }
	HBRUSH    SetBrush(HBRUSH hNewBrush)     { return (m_hBrush = hNewBrush); }
	COLORREF  GetColor()                     { return m_dwColorRef; }
	COLORREF  SetColor(COLORREF dwNewColor)  { return (m_dwColorRef = dwNewColor); }
	BOOL		 GetPalette(void) const			  { return m_fPalette; }
	BOOL		 SetPalette(BOOL fPalette)		  { return (m_fPalette = fPalette); }
};

struct CBrushCompareStruct
{
	COLORREF m_Color;
	BOOL m_fPalette;
};

struct COutlinePaintParams
{
	// Required parameters.
	RedisplayContext* m_pRC;
	PBOX m_Bound;
	PBOX m_UnrotatedBound;	// Only required if m_dRotation is not 0.

	// Parameters with default values.
	double m_dRotation;
	FLAGS m_Flags;
	COLOR m_Color;
	COLOR m_BColor;

	// Object parameters.
	PMGPageObject* m_pObject;		// Generic. Required for making images.
	BOOL m_fColoringBook;
	GraphicPtr m_pGraphic;

	// Display parameters.
	LPRECT m_pClip;

	// Constructor.
	COutlinePaintParams();
	void FromObject(RectPageObject* pObject);
	void FromGraphicObject(GraphicObject* pObject);
};

class OutlineUpdateState : public UpdateState
{
protected:
/* Brush cache - linked list of brushes - DGP */
  static tDoubleLinkedList   m_BrushLinkedList;

	friend struct GDIDrawingSet;

	SHORT	strip;					/* Which strip are we doing? */
	POINT where;					/* Where we are plotting. */

	BOOL coloring_book;
	CBitmapObjectImage*		object_image;	/* The object image when recording */
	BITMAP m_bm;						/* Info from GetObject(). */
	POINT *m_pPointBuffer;			// Point buffer - DGP
	int m_nPointBufferSize;			// Current size of point buffer - DGP

	// Transformation variables.
	COutlinePathMatrix m_UnrotatedTransform;	// Transform without rotation
	COutlinePathMatrix m_RotationTransform;	// Rotation transform
	COutlinePathMatrix m_Transform;				// Master transformation (concatentation of above)

/* Outline rotation variables. */

	ANGLE dest_angle;
	CFixedPoint dest_center;
	SHORT dest_x_offset;
	SHORT dest_y_offset;

/* Source extent, transform. */

	POINT source_extent;
	POINT source_origin;

/* Background attributes. */

	OUTLINE_COLOR background_color;
	BYTE background_trans_table[MAX_OUTLINE_PLANES+1];

/* Fill attributes. */

	SHORT interior_style;
	SHORT hatch_index;
	OUTLINE_COLOR fill_color;
	BYTE fill_trans_table[MAX_OUTLINE_PLANES+1];

/* Edge attributes. */

	BOOL edge_visibility;
	SHORT edge_type;
	SHORT edge_width;
	OUTLINE_COLOR edge_color;
	BYTE edge_trans_table[MAX_OUTLINE_PLANES+1];

/* Line attributes. */

	SHORT line_type;
	SHORT line_width;
	OUTLINE_COLOR line_color;
	BYTE line_trans_table[MAX_OUTLINE_PLANES+1];

/* Marker attributes. */

	MARKER_TYPE marker_type;
	SHORT marker_size;
	OUTLINE_COLOR marker_color;
	BYTE marker_trans_table[MAX_OUTLINE_PLANES+1];

/* Bitmap attributes. */

	int m_nBltMode;
	int m_nBkMode;
	COLORREF m_clBk;
	COLORREF m_clText;
	int m_nTextAlign;
	LOGPALETTE* m_pCurrentPalette;
	CPoint m_cpCurrentPoint;

/* Text attributes. */

	int m_lfUnscaledHeight;
	LOGFONT m_CurrentFont;

/* The global memory pointer for DIB pattern brush creation. */

	HGLOBAL dib_info_mem;
	LPDIBPATTERN dib_info;

/* The source bitmap and its HDC (for Geronomo -> temp BitBlts). */

	HBITMAP source_bitmap;
	HDC source_hdc;
	HBITMAP source_old_bitmap;

/* The mask bitmap and its HDC (for temp -> rc HDC BitBlts). */

	HBITMAP mask_bitmap;
	HDC mask_hdc;
	HBITMAP mask_old_bitmap;

/* Extent variables. */

	SHORT	mask_plane;				/* 0 for none, 1 for mask plane */

	POINT dest_origin;
	POINT dest_extent;	

	SHORT	bitmap_width;
	SHORT	bitmap_height;

/*
// Variables for color remapping.
*/

	BOOL do_remapping;

	SHORT red_min, red_delta;
	SHORT green_min, green_delta;
 	SHORT blue_min, blue_delta;

/*
// Did we draw something?
*/

	BOOL something_drawn;			/* Whether we have drawn or not. */
	RECT what_drawn;					/* The extent of what was drawn. */

	BOOL all_white;					/* If all colors drawn were white. */
	BOOL all_black;					/* If all colors drawn were black. */

	OUTLINE_COLOR RGBToOutlineColor(COLORREF c);
	void OUS_NEAR set_outline_color(OUTLINE_COLOR far *color, OUTLINE_COLOR new_color, LPBYTE trans_table);
	void OUS_NEAR update_what_drawn(RECT r);
	void OUS_NEAR reset_outline_drawn(BOOL reset);
	void OUS_NEAR draw_chunk_to_dc(HDC hdc, PRECT src_r, PRECT dst_r);
	void OUS_NEAR show_outline_drawn(BOOL reset);
	void OUS_NEAR setup_outline_color(COLOR Color, COLOR BColor);
	IMAGE_CODE OUS_NEAR parse_outline_file(BOOL do_update, BOOL can_interrupt, ST_DEV_POSITION position);
	IMAGE_CODE OUS_NEAR plot_outline(int x, int y,
								int width, int height,
								LONG file_offset,
								BOOL can_interrupt,
								ANGLE angle);
	BOOL OUS_NEAR gdi_box_valid(RECT r, SHORT width);

/* GDI management. */

	BOOL OUS_NEAR create_DCs(void);
	void OUS_NEAR delete_DCs(void);
	BOOL OUS_NEAR create_bitmaps(SHORT width, SHORT height);
	void OUS_NEAR delete_bitmaps(BOOL force = TRUE);
	void OUS_NEAR create_image_bitmaps(SHORT width, SHORT height, BOOL clear);
	void OUS_NEAR delete_image_bitmaps(void);

/* Path processing. */

	int OUS_NEAR compute_scaled_width(int in_width);
	int ScaleX(int n);
	int ScaleY(int n);

	virtual ST_DEV_POSITION file_start(void) { return 0L; }
	virtual BOOL read_next_point(OUTLINE_POINT *point) { return FALSE; }

protected:
	// Common construction code.
	void CommonConstruct(void);

	// Interior styles.
	enum
	{
		STYLE_Hollow = 0,
		STYLE_Solid = 1,
		STYLE_Pattern = 2,
		STYLE_Hatch = 3,
		STYLE_Empty = 4,
	};

	PaintContext pc;

	// Drawing variables.
   HDC m_hdc;
   int m_nXNorm;
   int m_nYNorm;
   BOOL m_fDrawToDC;

#ifdef FIX_CGMS
	StorageFile file;
#else
	ReadOnlyFile file;
#endif
	LONG	file_offset;
	int m_nFillMode;					// Polygon fill mode

	int source_scale(int w);

/* Transform control. */

	BOOL	reverse_x_transform;
	BOOL	reverse_y_transform;
	BOOL	landscape_transform;

/* Methods available to derived objects. */

	void set_source_extent(OUTLINE_POINT p0, OUTLINE_POINT p1);

	void transform_point(OUTLINE_POINT p, CFixedPoint* new_p);

	void reset_primitive_draw(void);

	HBRUSH create_solid_brush(BYTE red, BYTE green, BYTE blue);

	void add_ellipse(OUTLINE_POINT center, SHORT rx, SHORT ry);
	void draw_ellipse(OUTLINE_POINT center, SHORT rx, SHORT ry, double rotation = 0.0);
	void draw_rectangle(OUTLINE_POINT p0, OUTLINE_POINT p1);
	void draw_elliptical_arc(OUTLINE_POINT center, double rx, double ry, double start_angle, double end_angle, SHORT closure);
	void draw_marker(OUTLINE_POINT center);
	void draw_polymarker(SHORT count);
	void draw_polygon(SHORT number_of_points);
	void draw_polyline(SHORT number_of_points, BOOL fCheckVisibility = FALSE);
	void draw_polycurve_triples(SHORT number_of_points, SHORT closure);
	void draw_elliptical_arc_3pt(OUTLINE_POINT ps, OUTLINE_POINT pi, OUTLINE_POINT pe, SHORT closure);
	void DrawBitmap(CRect crDest, CRect crSource, LPBYTE pData, BITMAPINFO* pbi, WORD wUsage, DWORD dwROP);
	void DrawText(OUTLINE_POINT p, WORD wOptions, LPCSTR pText, int nStrLength, LPRECT pRectangle, short* pDX = NULL);
	void MoveTo(OUTLINE_POINT p);
	void LineTo(OUTLINE_POINT p);
	void DoPatBlt(OUTLINE_POINT p0, OUTLINE_POINT p1, DWORD dwRop);

	void DrawPath(COutlinePath& Path);

	void GeneratePolygonPoints(CRect cr, POINT* Points, CRect* pcrBound = NULL);
	LPPOINT GetPointBuffer(int nPoints);  // DGP

	void set_interior_style(SHORT s) { interior_style = s; }
	void set_hatch_index(SHORT i) { hatch_index = i; }
	void set_fill_color(OUTLINE_COLOR color);

	void set_line_width(SHORT w) { line_width = w; }
	SHORT get_line_width(void) { return line_width; }
	void set_line_type(SHORT t) { line_type = t; }
	void set_line_color(OUTLINE_COLOR color);

	void set_edge_width(SHORT w) { edge_width = w; }
	SHORT get_edge_width(void) { return edge_visibility ? edge_width : 0; }
	void set_edge_visibility(SHORT v) { edge_visibility = v; }
	void set_edge_type(SHORT t) { edge_type = t; }
	void set_edge_color(OUTLINE_COLOR color);

	void set_marker_size(SHORT s) { marker_size = s; }
	SHORT get_marker_size(void) { return marker_size; }
	void set_marker_type(SHORT t) { marker_type = (MARKER_TYPE)t; }
	void set_marker_color(OUTLINE_COLOR color);

	void set_background_color(OUTLINE_COLOR color);

	void SetBltMode(int nMode)
		{ m_nBltMode = nMode; }
	void SetBkMode(int nMode)
		{ m_nBkMode = nMode; }
	void SetBkColor(COLORREF Color)
		{ m_clBk = Color; }
	void SetTextColor(COLORREF Color)
		{ m_clText = Color; }
	void SetTextAlign(int nAlign)
		{ m_nTextAlign = nAlign; }

	void SetCurrentFont(const LOGFONT& Font);
	void SetCurrentPalette(LOGPALETTE* pPalette);

	virtual void set_defaults(void);

/* Math utilities. */

	void center_from_3pt(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT *yc);
	double distance(OUTLINE_POINT a, OUTLINE_POINT b);
	double angle_from_vector(SHORT dx, SHORT dy);

	void DrawPolygon(LPPOINT point_buffer, int number_of_points);
	void DrawPolyPolygon(LPPOINT pPoints, const int* pPolyCounts, int nPolygons);

public:
	UpdateStatePtr paint(const COutlinePaintParams& Params, UpdateStatePtr ustate);

	OutlineUpdateState(GraphicPtr graphic);
	OutlineUpdateState(StorageDevice* pSourceDevice);
	virtual 	~OutlineUpdateState();

	virtual	ERRORCODE open_outline(void);
	virtual	BOOL parse_next_command(BOOL *primitive) = 0;
	virtual	void close_outline(void);

	// Special access.
	StorageFile& GetFile(void)
		{ return file; }
};

typedef OutlineUpdateState far *OutlineUpdateStatePtr;

#endif					// #ifndef __OUTLINE_H__


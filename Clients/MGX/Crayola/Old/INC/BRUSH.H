//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _BRUSH_H_
#define _BRUSH_H_

/********************************************************************* 
 **** Brush contants
 *********************************************************************/
#define MAX_BRUSH_SIZE 50

// SourceColor
#define SC_ACTIVE_COLOR		0
#define SC_ALTERNATE_COLOR	1
#define SC_RGB_COLOR		2

// SourceData
#define SD_CURRENT_DEST		0
#define SD_CURRENT_UNDO		1
#define SD_FILE				2
#define SD_NONE				3

// SourceFunction
#define SF_SOLID_COLOR		0
#define SF_COPY_IMAGE		1
#define SF_SHARPEN_IMAGE	2
#define SF_SMOOTH_IMAGE		3
#define SF_LIGHTEN_IMAGE	4
#define SF_DARKEN_IMAGE		5
#define SF_SMEAR_IMAGE		6

// SourceFlipMode
#define SFM_NO_FLIP		0
#define SFM_MIRROR		1
#define SFM_TILE		2

// StrokeMethod
#define SM_CONNECTED	0
#define SM_UNCONNECTED	1
#define SM_SIMPLE_DDA	2

// State
#define STATE_BEGIN		1
#define STATE_FIRSTSTROKE 	2
#define STATE_NEWSTROKE		4
#define STATE_END		8

// ReadOnlyMask bits
#define RO_SHAPE		1
#define RO_SIZE			2
#define RO_OPACITY		4
#define RO_SOURCE_DATA	8
#define RO_FLIPMODE		16
#define RO_PRESSURE		32
#define RO_MERGEMODE	64
#define RO_FEATHER		128

/********************************************************************* 
 **** MGXBRUSH: The brush structure
 *********************************************************************/
typedef struct _mgxbrush
	{
	int			SourceColor;
	COLORINFO	ColorStruct;
	int			SourceData;
	LPFRAME		SourceFrame;
	int			SourceFlipMode;
	BOOL		SourceFlipHorz;
	BOOL		SourceFlipVert;
	int			SourceFunction;
	STRING		szSourceExtName;
	FNAME		szSource;
	int			xSrcOffset;
	int			ySrcOffset;
	int			StrokeMethod;
	int			Shape;
	FNAME		ShapeName;
	int			Style;
	int			Size;
	int			Pressure;
	int			Feather;
	int			FadeSize;
	int			FadeOpacity;
	int			Opacity;
	int			OpacityScale;
	MERGE_MODE 	MergeMode;
	WORD		Timer;
	int			StylePressure;
	int			Weight;
	int			Spacing;
	int			Smoothness;
	BOOL		NoOverlap;
	ULONG		ReadOnlyMask;
	int			nSourcePatterns;
	BOOL		fSourceRandom;
	BOOL		fSourceDirectional;
	BOOL		fSourceStartPatterns;
	BOOL		fSourceEndPatterns;
	BOOL		fSourceUseDropoutMask;
	BYTE		BrushMask[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
	int			iDDA;
	BOOL		fMirrorMode;
	} MGXBRUSH;
typedef MGXBRUSH _far *LPMGXBRUSH;
typedef LPTR HMGXBRUSH;

typedef void (_far *LPBRUSHUPDATEPROC)(HWND hWnd, LPRECT lpUpdateRect, BOOL fUpdateCursor);
typedef void (_far *LPSRCPOSPROC)(int x, int y, int BrushSize, LPINT lpX, LPINT lpY);

#endif // _BRUSH_H_



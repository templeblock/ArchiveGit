//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


typedef enum _masktype
{
	MT_NONE  	= 1,
	MT_BASE   	= 2,
	MT_OBJECT 	= 3,
} MASKTYPE;

typedef void (_far *LPREADDATAPROC)(int y, int left, int right, LPTR lpSrc, int depth);
typedef void (_far *LPDATAPROC)(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth);
typedef void (_far *LPMASKPROC)(int y, int left, int right, LPTR lpMsk);

typedef struct _engine {
	BOOL fEditAlpha;		// work on the alpha channel of the object
	LPRECT lpEditRect;		// the rect to work on.  In base image coords.
	LPCOLORINFO lpColor;	// if not NULL, fill with this color.
	LPFRAME lpSrcFrame;		// if not NULL, get souce data from here.
	BOOL FlipHorz;			// flip horizontaly.
	BOOL FlipVert;			// flip verticaly.
	LPDATAPROC lpDataProc;	// proc to change data.
	LPMASKPROC lpMaskProc;	// proc to change masking.
	int Opacity;			// opacity of changes to original data.
	MERGE_MODE MergeMode;	// Merge mode.
	MASKTYPE MaskType;		// what mask to use for masking.
	int	ScatterPressure;	// Scatter the mask - 255 = no scattering
	BOOL fNoUndo;			// Don't do any undo
}	ENGINE;
typedef ENGINE _far *LPENGINE;


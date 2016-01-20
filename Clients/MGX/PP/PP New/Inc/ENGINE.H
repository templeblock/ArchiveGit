//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081


typedef enum _masktype
{
	MT_NONE  	= 1,
	MT_BASE   	= 2,
	MT_OBJECT 	= 3
} MASKTYPE;

typedef void (FAR *LPREADDATAPROC)(int y, int left, int right, LPTR lpSrc, FRMTYPEINFO TypeInfo);
typedef void (FAR *LPDATAPROC)(int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth, LPVOID lpParam);
typedef void (FAR *LPMASKPROC)(int y, int left, int right, LPTR lpMsk, LPVOID lpParam);

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
	LPVOID	lpParam;		// caller param passed to callbacks
	RECT	rUpdate;		// area needed to be update on image
	BOOL	fThread;		// Thread this function
	BOOL	fEscapable;		// Can use escape from this function?
}	ENGINE;
typedef ENGINE FAR *LPENGINE;

// for engine.c
// Source file: engine.c

// functions to setup ENGINE struct
void SetEngineOld(LPENGINE lpEngine,BOOL fEditAlpha, LPRECT lpEditRect,LPCOLORINFO lpColor,LPFRAME lpSrcFrame, BOOL FlipHorz, BOOL FlipVert,LPDATAPROC lpDataProc,LPMASKPROC lpMaskProc,int Opacity, MERGE_MODE MergeMode);
void SetEngineDef(LPENGINE lpEngine);
void SetEngineDraw(LPENGINE lpEngine,LPDATAPROC lpDataProc, int Opacity, MERGE_MODE MergeMode);
void SetEngineColor(LPENGINE lpEngine,LPCOLORINFO lpColor, int Opacity, MERGE_MODE MergeMode);

// line engines
STATUS_CODE LineEngineSelObj(LPIMAGE lpImage, LPENGINE lpEngine, int dirty);
STATUS_CODE LineEngineObj(LPIMAGE lpImage, LPOBJECT lpObject, LPENGINE lpEngine, int dirty);

// line processing- just process the data
BOOL ReadEngineSelObj(LPIMAGE lpImage, LPRECT lpEditRect, LPREADDATAPROC lpReadDataProc);
void WriteLine(LPTR lpSrcData, LPPIXMAP lpDstPix, LPMASK lpMask,int y, int offx, int offy, int xstart=0, int width=-1);
void LoadMaskDataHuge(int xs,int ys,int dx,int dy, HPTR pm, LPMASK lpMask, COMBINE_ROP rop);
void LoadMaskData(int xs,int ys,int dx,int dy, LPTR pm, LPMASK lpMask, COMBINE_ROP rop);
void mload(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void mload8(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void mload8P(int xs,int ys,int dx,int dy, LPTR pm, LPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void mload24(int xs,int ys,int dx,int dy, LPTR pm, LPRGB pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void mload32(int xs,int ys,int dx,int dy, LPTR pm, LPCMYK pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void MLoad8Huge(int xs,int ys,int dx,int dy, HPTR pm, HPTR pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void MLoad24Huge(int xs,int ys,int dx,int dy, HPTR pm, HPRGB pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void MLoad32Huge(int xs,int ys,int dx,int dy, HPTR pm, HPCMYK pd, FRMTYPEINFO TypeInfo, LPMASK lpMask);
void Shield(LPTR pm, LPTR pd, WORD wCount, FRMTYPEINFO TypeInfo);
void Shield8(LPTR pm, LPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield8P(LPTR pm, LPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield24(LPTR pm, LPRGB pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield32(LPTR pm, LPCMYK pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield8Huge(HPTR pm, HPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield8PHuge(HPTR pm, HPTR pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield24Huge(HPTR pm, HPRGB pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void Shield32Huge(HPTR pm, HPCMYK pd, DWORD dwCount, FRMTYPEINFO TypeInfo);
void MergeMask(LPTR lpDst, LPRECT lpDstRect, int DstWidth, LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect, int iMaxValue);
void MergeMaskS(LPTR lpDst, LPRECT lpDstRect, int DstWidth, LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData8(LPTR lpDst, LPRECT lpDstRect, int DstWidth, LPTR lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData24(LPRGB lpDst, LPRECT lpDstRect, int DstWidth, LPRGB lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData24S(LPRGB lpDst, LPRECT lpDstRect, int DstWidth, LPRGB lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect, LPTR lpMsk);
void MergeData32(LPCMYK lpDst, LPRECT lpDstRect, int DstWidth, LPCMYK lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData32S(LPCMYK lpDst, LPRECT lpDstRect, int DstWidth, LPCMYK lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect, LPTR lpMsk);
void ScatterMask(LPTR lpSrc, LPTR lpDst, LPINT16 lpErrors, int dx, int dy, int iPressure);
void ScatterMaskHuge(HPTR lpSrc, HPTR lpDst, LPINT16 lpErrors, int dx, int dy, int iPressure);
#ifndef C_CODE
extern "C" {
#endif
void ScatterLine(LPTR lpSrc, LPTR lpDst, LPINT16 lpErrors, int dx, int iPressure);
#ifndef C_CODE
}
#endif
void LoadColor(LPFRAME lpFrame, LPTR lpBuf, DWORD dwCount, LPCOLORINFO lpColor);
BOOL LoadSourceData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, int dx, int dy, HPTR hpData, LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip);
BOOL LoadTiledData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, int dx, int dy, HPTR hpData, LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip);
BOOL LoadMirroredData(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType, int x, int y, int dx, int dy, HPTR hpData, LPRECT lpRect, BOOL HorzFlip, BOOL VertFlip);
void copyifset( LPTR lptr1, LPTR lptr2, int count );
void TexturizeData32(LPCMYK lpDst, LPTR lpMsk, int iCount);
#ifndef C_CODE
extern "C" {
#endif
void TexturizeData24(LPRGB lpDst, LPTR lpMsk, int iCount);
void TexturizeData8(LPTR lpDst, LPTR lpMsk, int iCount);
#ifndef C_CODE
}
#endif


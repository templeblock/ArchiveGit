//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _CMDPARMS_H_
#define _CMDPARMS_H_

#define MAX_CMD_LEN	512
#define END_OF_LINE	_T("\r\n")
#define FIXED_PREC		4

typedef enum
	{
	WF_DONE,
	WF_WRITE,
	WF_ERROR
	} WRITE_FUNC;

//
// Common parameters to all functions except CT_LOAD
//
typedef struct _common_parms
	{
	ITEMID			idDirty;
	UPDATE_TYPE 	UpdateType;
	RECT			rUpdateArea;
	LIST			NewImageList;
	LONG			lUsageCount;
	BOOL			fParmsStored;
	STATUS_CODE		StatusCode;
	} COMMON_PARMS, FAR *LPCOMMON_PARMS;

//
// Program control function parameters
//
typedef struct _activatewindow_parms
	{
	COMMON_PARMS	Common;
	FNAME			szFileName;
	int				iOffset;
	} ACTIVATEWINDOW_PARMS, FAR *LPACTIVATEWINDOW_PARMS;

//
// Image load function parameters
//

typedef struct _loadfile_parms
	{
	COMMON_PARMS	Common;
	LPCMDLIST		lpCmdList;
	FNAME			szFileName;
	VOLID 			szVolumeID;
	ITEMID			idFileType;
	CMSINFO			cmsInfo;
	LPVOID			lpOptions;
	} LOADFILE_PARMS, FAR *LPLOADFILE_PARMS;

typedef struct _lowresload_parms
	{
	COMMON_PARMS	Common;
	LPCMDLIST		lpCmdList;
	FNAME			szFileName;
	VOLID 			szVolumeID;
	ITEMID			idFileType;
	int				Resolution;
	CMSINFO			cmsInfo;
	LPVOID			lpOptions;
	} LOWRESLOAD_PARMS, FAR *LPLOWRESLOAD_PARMS;

typedef struct _fastbits_parms
	{
	COMMON_PARMS	Common;
	LPCMDLIST		lpCmdList;
	FNAME			szFileName;
	VOLID 			szVolumeID;
	RECT			Rect;
	CMSINFO			cmsInfo;
	} FASTBITS_PARMS, FAR *LPFASTBITS_PARMS;

typedef struct _new_parms
	{
	COMMON_PARMS	Common;
	LPCMDLIST		lpCmdList;
	FRMDATATYPE		Type;
	LFIXED			Width;
	LFIXED			Height;
	int				Resolution;
	COLORINFO		Color;
	CMSINFO			CmsInfo;
	} NEW_PARMS, FAR *LPNEW_PARMS;

//
// Mode function parameters
//

typedef struct _savefile_parms
	{
	COMMON_PARMS	Common;
	FNAME			szFileName;
	ITEMID			idFileType;
	ITEMID			idDataType;
	CMSINFO			CmsInfo;
	struct _epsoptions EPSOptions;
	struct _tiffoptions TIFFOptions;
	struct _tgaoptions TGAOptions;
	struct _jpegoptions JPEGOptions;
	struct _pp40options PPFFOptions;
	struct _avioptions AVIOptions;
	} SAVEFILE_PARMS, FAR *LPSAVEFILE_PARMS;

typedef struct _savetoalbum_parms
	{
	COMMON_PARMS	Common;
	FNAME			szFileName;
	FNAME			szAlbum;
	ITEMID			idFileType;
	ITEMID			idDataType;
	struct _epsoptions EPSOptions;
	struct _tiffoptions TIFFOptions;
	struct _tgaoptions TGAOptions;
	struct _jpegoptions JPEGOptions;
	struct _pp40options PPFFOptions;
	struct _avioptions AVIOptions;
	} SAVETOALBUM_PARMS, FAR *LPSAVETOALBUM_PARMS;

typedef struct _selectobjects_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	RECT			SelectRect;
	BOOL			fExtendedSel;
	BOOL			fSelectAll;
	} SELECTOBJECTS_PARMS, FAR *LPSELECTOBJECTS_PARMS;

typedef struct _selectobjrel_parms
	{
	COMMON_PARMS	Common;
   int            Pos;        // position in the object list
   int            Shift;
	} SELECTOBJRELATIVE_PARMS, FAR *LPSELECTOBJRELATIVE_PARMS;

typedef struct _objectlayering_parms
	{
	COMMON_PARMS	Common;
   int            Offset;        // relative move to current Pos, +/-
	} OBJECTLAYERING_PARMS, FAR *LPOBJECTLAYERING_PARMS;

typedef struct _moveobjects_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	int				x;
	int				y;
	} MOVEOBJECTS_PARMS, FAR *LPMOVEOBJECTS_PARMS;

typedef enum
	{
	AT_OPACITY,
	AT_MERGEMODE
	} ATTRIBUTE_TYPE;

typedef struct _setattrobjects_parms
	{
	COMMON_PARMS	Common;
	ATTRIBUTE_TYPE	AttributeType;
	int				Opacity;
	MERGE_MODE		MergeMode;
	} SETATTROBJECTS_PARMS, FAR *LPSETATTROBJECTS_PARMS;

typedef struct _layerobjects_parms
	{
	COMMON_PARMS	Common;
	BOOL			fToTop;
	BOOL			fOneLevel;
	} LAYEROBJECTS_PARMS, FAR *LPLAYEROBJECTS_PARMS;

typedef enum
	{
	HA_NONE,
	HA_LEFT,
	HA_CENTER,
   HA_RIGHT,
	HA_EQUAL
	} HORZ_ALIGN;

typedef enum
	{
	VA_NONE,
	VA_TOP,
	VA_MID,
   VA_BOTTOM,
	VA_EQUAL
	} VERT_ALIGN;

typedef enum
	{
	RA_IMAGE,
   RA_MASK,
	RA_OBJECT
	} RECT_ALIGN;

typedef struct _alignbjects_parms
	{
	COMMON_PARMS	Common;
	HORZ_ALIGN		HorzAlign;
	VERT_ALIGN		VertAlign;
	RECT_ALIGN		RectAlign;
	BOOL			bPreview;
	LPRECT			lpRect;
	} ALIGNOBJECTS_PARMS, FAR *LPALIGNOBJECTS_PARMS;

typedef struct _posobjects_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	int				XOffset;
	int				YOffset;
	} POSOBJECTS_PARMS, FAR *LPPOSOBJECTS_PARMS;

//
// Image editing function parameters
//

typedef struct _cmd_parms
	{
	COMMON_PARMS	Common;
	} CMD_PARMS, FAR *LPCMD_PARMS;

typedef struct _rotate_parms
	{
	COMMON_PARMS	Common;
	LFIXED	Angle;
	BOOL	fUseWeightedAvg;
	} ROTATE_PARMS, FAR *LPROTATE_PARMS;

typedef struct _mirror_parms
	{
	COMMON_PARMS	Common;
	BOOL	xmirror;
	BOOL	ymirror;
	} MIRROR_PARMS, FAR *LPMIRROR_PARMS;

typedef struct _size_parms
	{
	COMMON_PARMS	Common;
	LFIXED	Width;
	LFIXED	Height;
	int		Resolution;
	BOOL	SmartSize;
	BOOL	MaintainSize;
	} SIZE_PARMS, FAR *LPSIZE_PARMS;

typedef struct _expand_parms
	{
	COMMON_PARMS	Common;
	LFIXED	Top;
	LFIXED	Left;
	LFIXED	Right; 
	LFIXED	Bottom;
	LFIXED	Width;
	LFIXED  Height;
	COLORINFO Color;
	BOOL	fUseRightBottom;
	} EXPAND_PARMS, FAR *LPEXPAND_PARMS;

typedef enum
	{
	CT_OPTIMIZED,
	CT_STANDARD,
	CT_CUSTOM
	} CONVERT_TYPE, FAR *LPCONVERT_TYPE;

typedef struct _convert_parms
	{
	COMMON_PARMS	Common;
	CMSINFO			cmsInfo;
	DITHER_TYPE		DitherType;
	CONVERT_TYPE	ConvertType;
	int				iLevels;
	EXTNAME			szPalette;
	} CONVERT_PARMS, FAR *LPCONVERT_PARMS;

typedef struct _gradient_parms
	{
	COMMON_PARMS	Common;
	int			VigOpacity;
	MERGE_MODE 	VigMergeMode;
	int			iBaseRes;
  	int 		x1;
  	int			y1;
  	int			x2;
  	int			y2;
	COLORINFO	StartColor;
	COLORINFO	EndColor;
	int			Gradient;
	int			RepeatCount;
	BOOL		SoftTransition;
	int			Midpoint;
	int			VigColorModel;
	} GRADIENT_PARMS, FAR *LPGRADIENT_PARMS;

typedef struct _tintfill_parms
	{
	COMMON_PARMS	Common;
	int			FillOpacity;
	MERGE_MODE 	FillMergeMode;
	COLORINFO	Color;
	} TINTFILL_PARMS, FAR *LPTINTFILL_PARMS;

typedef struct _texturefill_parms
	{
	COMMON_PARMS	Common;
	int			TextureOpacity;
	MERGE_MODE 	TextureMergeMode;
	EXTNAME		TextureName;
	BOOL		fHorzFlip;
	BOOL		fVertFlip;
	} TEXTUREFILL_PARMS, FAR *LPTEXTUREFILL_PARMS;

typedef struct _flood_parms
	{
	COMMON_PARMS	Common;
	int			FloodOpacity;
	MERGE_MODE 	FloodMergeMode;
	COLORINFO	Color;
	int				iBaseRes;
	int				x;
	int				y;
	COLORINFO	InColor;
	int			FloodRange;
	int			FloodFade;
	int			idFloodColorModel;
	} FLOOD_PARMS, FAR *LPFLOOD_PARMS;

typedef struct _contbrit_parms
	{
	COMMON_PARMS	Common;
	int			Contrast;
	int			Brightness[NUM_BANDS];
	} CONTBRIT_PARMS, FAR *LPCONTBRIT_PARMS;

typedef struct _balance_parms
	{
	COMMON_PARMS	Common;
	int	 		MasterContrast;
	int			MasterBrightness[NUM_BANDS];
	BOOL		fMasterReverse;
	int			RedContrast;
	int			RedBrightness[NUM_BANDS];
	BOOL		fRedReverse;
	int			GreenContrast;
	int			GreenBrightness[NUM_BANDS];
	BOOL		fGreenReverse;
	int			BlueContrast;
	int			BlueBrightness[NUM_BANDS];
	BOOL		fBlueReverse;
	int			BlackContrast;
	int			BlackBrightness[NUM_BANDS];
	BOOL		fBlackReverse;
	} BALANCE_PARMS, FAR *LPBALANCE_PARMS;

typedef struct _mapinfo
	{
	int		Points;
	int 	iCurves;		// 0 for no curve interpolation
	int 	Reverse;		
	double 	gamma;		  
	POINT 	Pnt[MAXPOINTS]; // Curve break points in pixels from 0-255
	} MAPINFO, FAR *LPMAPINFO;

typedef struct _modifymap_parms
	{
	COMMON_PARMS	Common;
	MAPINFO			MasterInfo;
	MAPINFO			RedInfo;
	MAPINFO			GreenInfo;
	MAPINFO			BlueInfo;
	MAPINFO			BlackInfo;
	} MODIFYMAP_PARMS, FAR *LPMODIFYMAP_PARMS;

typedef struct _tone_parms
	{
	COMMON_PARMS	Common;
	MAPINFO			MasterInfo;
	MAPINFO			RedInfo;
	MAPINFO			GreenInfo;
	MAPINFO			BlueInfo;
	MAPINFO			BlackInfo;
	} TONE_PARMS, FAR *LPTONE_PARMS;

typedef struct _effect_parms
	{
	COMMON_PARMS	Common;
	EXTNAME			szEffect;
	LPTSTR			lpString;
	int 			bInOut;
	} EFFECT_PARMS, FAR *LPEFFECT_PARMS;

typedef struct _transformer_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	RECT			SelectRect;
	int				Function;
	int				Mode;
	int				TransformOpacity;
	MERGE_MODE 		TransformMergeMode;
	BOOL			fHiQuality;
	BOOL			fPasteInto;
	BOOL			fDelete;		// applies to mask move only
	POINT			DistortPoints[4];
	} TRANSFORMER_PARMS, FAR *LPTRANSFORMER_PARMS;

typedef struct _transformobjects_parms
	{
	COMMON_PARMS	Common;
	BOOL			fHiQuality;
	int				iBaseRes;
	POINT			DistortPoints[4];
	} TRANSFORMOBJECTS_PARMS, FAR *LPTRANSFORMOBJECTS_PARMS;

typedef struct _pastefrom_parms
	{
	COMMON_PARMS	Common;
	int				TransformOpacity;
	MERGE_MODE 		TransformMergeMode;
	BOOL			fHiQuality;
	BOOL			fPasteInto;
	int				iBaseRes;
	POINT			DistortPoints[4];
	EXTNAME			szClipboard;
	FNAME			szClipFileName;
	BOOL			fTemporary;
	CMSINFO			cmsInfo;
	} PASTEFROM_PARMS, FAR *LPPASTEFROM_PARMS;

typedef struct _paste_parms
	{
	COMMON_PARMS	Common;
	int				TransformOpacity;
	MERGE_MODE 		TransformMergeMode;
	BOOL			fHiQuality;
	BOOL			fPasteInto;
	int				iBaseRes;
	POINT			DistortPoints[4];
	} PASTE_PARMS, FAR *LPPASTE_PARMS;


typedef struct _mask_smooth_parms
	{
	COMMON_PARMS	Common;
	int  iBaseRes;
	int  gauss;
	int  thresh;
	} MASK_SMOOTH_PARMS, FAR *LPMASK_SMOOTH_PARMS;

typedef struct _remove_holes_parms
	{
	COMMON_PARMS	Common;
	} REMOVE_HOLES_PARMS, FAR *LPREMOVE_HOLES_PARMS;

typedef struct _featheralpha_parms
	{
	COMMON_PARMS	Common;
	int      inside;
	int	 	 iBaseRes;
	int 	 size;
	int   edge;
	} FEATHERALPHA_PARMS, FAR *LPFEATHERALPHA_PARMS;

typedef struct _text_parms
	{
	COMMON_PARMS	Common;
	LPTSTR			lpTextBuf;
	TCHAR			szFaceName[LF_FACESIZE];
	int				Size;
	BOOL			Italic;
	BOOL			Underline;
	BOOL			Strikeout;
	int				Weight;
	int				Angle;
	int				AntiAlias;
	int				iBaseRes;
	POINT			StartPos;
	POINT			DestPos;
	COLORINFO		Color;
	int				Opacity;
	MERGE_MODE		MergeMode;
   int		      Justification;
	} TEXT_PARMS, FAR *LPTEXT_PARMS;

typedef struct _posterizeinfo
	{
	int		Levels;
	int		Threshold;
	} POSTERIZEINFO, FAR *LPPOSTERIZEINFO;

typedef struct _posterize_parms
	{
	COMMON_PARMS	Common;
	POSTERIZEINFO	MasterInfo;
	POSTERIZEINFO	RedInfo;
	POSTERIZEINFO	GreenInfo;
	POSTERIZEINFO	BlueInfo;
	POSTERIZEINFO	BlackInfo;
	} POSTERIZE_PARMS, FAR *LPPOSTERIZE_PARMS;

typedef struct _hue_parms
	{
	COMMON_PARMS	Common;
	MAPINFO			HueInfo;
	int				Saturation;
	int				Brightness;
	} HUE_PARMS, FAR *LPHUE_PARMS;

typedef struct _applycal_parms
	{
	COMMON_PARMS	Common;
	EXTNAME			szScanMap;
	EXTNAME			szPrintMap;
	} APPLYCAL_PARMS, FAR *LPAPPLYCAL_PARMS;

typedef struct _copytofile_parms
	{
	COMMON_PARMS	Common;
	EXTNAME			szName;
	int				Type;
	} COPYTOFILE_PARMS, FAR *LPCOPYTOFILE_PARMS;

//
// Mask editing function parameters
//

typedef struct _shapemask_parms
	{
	COMMON_PARMS	Common;
	int				Shape;
	int				Mode;
	int				iBaseRes;
	int				x1;
	int				y1;
	int				x2;
	int				y2;
	} SHAPEMASK_PARMS, FAR *LPSHAPEMASK_PARMS;

typedef struct _freemask_parms
	{
	COMMON_PARMS	Common;
	int				Mode;
	int				Antialias;
	int				iBaseRes;
	int				nPoints;
	LPPOINT		lpPoints;
	} FREEMASK_PARMS, FAR *LPFREEMASK_PARMS;

typedef struct _pointedit_parms
	{
	COMMON_PARMS	Common;
	int				Mode;
	int				Antialias;
	int				iBaseRes;
	RECT 			rErase;
	int				iEraseVal;
	int				nPoints;
	LPPOINT		lpPoints;
	} POINTEDIT_PARMS, FAR *LPPOINTEDIT_PARMS;

typedef struct _magicmask_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	int				x;
	int				y;
	COLORINFO	Color;
	int			Mode;
	int			WandRange;
	int			WandFade;
	int			idMagicColorModel;
	} MAGICMASK_PARMS, FAR *LPMAGICMASK_PARMS;

typedef struct _similar_parms
	{
	COMMON_PARMS	Common;
	int			iBaseRes;
	int			x;
	int			y;
	COLORINFO	Color;
	int			Mode;
	int			WandRange;
	int			WandFade;
	int			idMagicColorModel;
	} SIMILAR_PARMS, FAR *LPSIMILAR_PARMS;

typedef struct _chroma_parms
	{
	COMMON_PARMS	Common;
   int         ChromaCombineMode;
   int         ChromaColorMode;
   int         Fade;
   int		   Mask;
   BOOL		   Delete;
   int		   MaskRange[8];
   COLORINFO   MaskColor[8];
	} CHROMA_PARMS, FAR *LPCHROMA_PARMS;

typedef struct _loadmask_parms
	{
	COMMON_PARMS	Common;
	int				Mode;
	BOOL			fHiQuality;
	int				iBaseRes;
	POINT			DistortPoints[4];
	EXTNAME			szMask;
	BOOL			fDeleteMask;
	FNAME			szMaskFileName;
	} LOADMASK_PARMS, FAR *LPLOADMASK_PARMS;

typedef struct _maskfromobject_parms
	{
	COMMON_PARMS	Common;
	int				Mode;
	BOOL			   fHiQuality;
	int				iBaseRes;
	POINT			DistortPoints[4];
	} MASKFROMOBJECT_PARMS, FAR *LPMASKFROMOBJECT_PARMS;

typedef struct _objectfrommask_parms
	{
	COMMON_PARMS	Common;
	} OBJECTFROMMASK_PARMS, FAR *LPOBJECTFROMMASK_PARMS;

typedef struct _brushpt
	{
	int	x;
	int	y;
	BYTE	Pressure;
	} BRUSHPT, FAR *LPBRUSHPT;

typedef enum
	{
	BT_PAINT,
	BT_SPRAY,
	BT_CLONE,
	BT_TEXTURE,
	BT_SMEAR,
	BT_ERASER,
	BT_SHARP,
	BT_SMOOTH,
	BT_LIGHTEN,
	BT_DARKEN,
	BT_PAINTON,
	BT_UNDOERASER
	} BRUSH_TYPE, FAR *LPBRUSH_TYPE;

/********************************************************************* 
 **** STROKEINFO: The stroke information structure
 *********************************************************************/

 typedef HPVOID HPSTROKEINFO;
 typedef struct _strokeinfo1
 	{
	int		dx;
	int		dy;
	} STROKEINFO1, FAR *LPSTROKEINFO1, HUGE *HPSTROKEINFO1;

 typedef struct _strokeinfo2
 	{
	int		dx;
	int		dy;
	int			Pressure;
	} STROKEINFO2, FAR *LPSTROKEINFO2, HUGE *HPSTROKEINFO2;

 typedef struct _strokeinfo3
 	{
	int		dx;
	int		dy;
	int		sx;
	int		sy;
	} STROKEINFO3, FAR *LPSTROKEINFO3, HUGE *HPSTROKEINFO3;

 typedef struct _strokeinfo4
 	{
	int			dx;
	int			dy;
	int			sx;
	int			sy;
	int			Pressure;
	} STROKEINFO4, FAR *LPSTROKEINFO4, HUGE *HPSTROKEINFO4;

typedef struct _retouch_parms
	{
	COMMON_PARMS	Common;
	BRUSH_TYPE		BrushType;
	// Parameters common to all retouching tools
	int				nPoints;
	BOOL			fUsePressure;
	EXTNAME			StyleName;
	int				BrushShape;
	EXTNAME			ShapeName;
	int				Size;
	int				Feather;
	// Parameters not common to all retouch tools
	int				Opacity;
	int				Pressure;
	MERGE_MODE		MergeMode;
	COLORINFO		Color;
	FNAME			szSourceImage;		// Clone
	POINT			ptSource;			// Clone
	int				Mode;				// Paint On Mask
	BOOL			fPaintOnObject;		// Pain on object alpha
	// All have stroke info - last just because their last in macro
	int				iDstRes;
	int				iSrcRes;
	HPSTROKEINFO 	hpStrokeInfo;
	} RETOUCH_PARMS, FAR *LPRETOUCH_PARMS;

typedef struct _strokemask_parms
	{
	COMMON_PARMS	Common;
	ITEMID			idBrush;
	EXTNAME			StyleName;
	int				BrushShape;
	EXTNAME			ShapeName;
	int				iBaseRes;
	int 			Size;
	int				Feather;
	int				Opacity;
	int				Pressure;
	MERGE_MODE		MergeMode;
	COLORINFO		Color;
	} STROKEMASK_PARMS, FAR *LPSTROKEMASK_PARMS;

typedef enum
	{
	FS_BORDER,
	FS_FILL,
	FS_BORDERFILL
	} FILL_STYLE, FAR *LPFILL_STYLE;

typedef enum
	{
	ST_RECTANGLE,
	ST_ELLIPSE
	} SHAPE_TYPE, FAR *LPSHAPE_TYPE;

typedef struct _drawbrush
	{
	EXTNAME			StyleName;
	int				BrushShape;
	EXTNAME			ShapeName;
	int				Size;
	int				Feather;
	int				Opacity;
	MERGE_MODE		MergeMode;
	} DRAWBRUSH, FAR *LPDRAWBRUSH;

typedef struct _drawshape_parms
	{
	COMMON_PARMS	Common;
	SHAPE_TYPE		ShapeType;
	FILL_STYLE 		FillStyle;
	DRAWBRUSH		Brush;
	COLORINFO		BorderColor;
	COLORINFO		FillColor;
	int				iBaseRes;
	int				x1;
	int				y1;
	int				x2;
	int				y2;
	} DRAWSHAPE_PARMS, FAR *LPDRAWSHAPE_PARMS;

typedef struct _drawfree_parms
	{
	COMMON_PARMS	Common;
	FILL_STYLE 		FillStyle;
	BOOL			fClosed;
	DRAWBRUSH		Brush;
	COLORINFO		BorderColor;
	COLORINFO		FillColor;
	int				iBaseRes;
	int				nPoints;
	LPPOINT			lpPoints;
	} DRAWFREE_PARMS, FAR *LPDRAWFREE_PARMS;

typedef struct _paletteedit_parms
	{
	COMMON_PARMS	Common;
	BOOL			fReMap;
	DITHER_TYPE		DitherType;
	LPCOLORMAP		lpColorMap;
	} PALETTEEDIT_PARMS, FAR *LPPALETTEEDIT_PARMS;

typedef struct _psfilter_parms
	{
	COMMON_PARMS	Common;
	EXTNAME			szFilter;
	BOOL			fHandle;
	LPTR			lpParameters;
	} PSFILTER_PARMS, FAR *LPPSFILTER_PARMS;

typedef struct _crop_parms
	{
	COMMON_PARMS	Common;
	int				iBaseRes;
	RECT			CropRect;
	} CROP_PARMS, FAR *LPCROP_PARMS;

typedef struct _savemask_parms
	{
	COMMON_PARMS	Common;
	EXTNAME			szName;
	} SAVEMASK_PARMS, FAR *LPSAVEMASK_PARMS;

typedef struct _discardundo_parms
	{
	COMMON_PARMS	Common;
	BOOL			fDiscardUndo;
	} DISCARDUNDO_PARMS, FAR *LPDISCARDUNDO_PARMS;

LPTSTR ReadLine(MACRO_FILE_HANDLE fh, LPTSTR lpBuf, DWORD dwMaxSize);
BOOL WriteString(MACRO_FILE_HANDLE fh, LPTSTR lpString);
LPVOID ChromaMaskReadParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID CmdGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID LoadFileGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID LowResLoadGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SaveFileGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID Rotate90GetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID Rotate180GetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID Rotate270GetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID RotateAnyGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SizeGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ExpandGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MirrorXGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MirrorYGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MirrorXYGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ConvertGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID GradientGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TintFillGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID DeleteGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID CutGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TextureFillGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID FloodGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ContBritGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID BalanceGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MapGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ShapeMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID FreeMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PointEditGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MagicMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SimilarGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TransformerGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TransformObjectsGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SelectObjectsGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MoveObjectsGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SetOpacityGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID OrderUpGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PasteFromGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PasteGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MaskFromObjectRead(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ObjectFromMaskRead(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID LoadMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID EffectGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID FeatherAlphaGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID MaskSmoothGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID RemoveHolesGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TextGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ActivateWindowGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID NewImageGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ToneGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PosterizeGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID HueGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ApplyCalGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID CopyToFileGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PaintGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SprayGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID CloneGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID TextureGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SmearGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID EraserGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID AltEraserGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SharpGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SmoothGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID LightenGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID DarkenGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PaintOnGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID DrawShapeGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID DrawFreeGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PencilGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PaletteEditGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID AlignObjGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SetMergeModeGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID OrderDownGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID OrderToTopGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID OrderToBottomGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PosObjGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID PSFilterGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID CropGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SaveToAlbumGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID FastBitsReadParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID StrokeMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ReadSelectObjectRelativeParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID ReadObjectLayeringGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID SaveMaskGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);
LPVOID DiscardUndoGetParms(LPCMDLIST lpCmdList, LPTSTR lpCommand, LPVOID lpCmdParms, MACRO_FILE_HANDLE fh);

WRITE_FUNC WriteObjectLayeringParms(LPOBJECTLAYERING_PARMS lpParms,
                  MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC WriteSelectObjectRelativeParms(LPSELECTOBJRELATIVE_PARMS lpParms,
                   MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ChromaMaskWriteParms(LPCHROMA_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC MaskFromObjectWrite(LPMASKFROMOBJECT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ObjectFromMaskWrite(LPOBJECTFROMMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC LoadFileWriteParms(LPLOADFILE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC RotateWriteParms(LPROTATE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SizeWriteParms(LPSIZE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SaveFileWriteParms(LPSAVEFILE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ExpandWriteParms(LPEXPAND_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ConvertWriteParms(LPCONVERT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC GradientWriteParms(LPGRADIENT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TintFillWriteParms(LPTINTFILL_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TextureFillWriteParms(LPTEXTUREFILL_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC FloodWriteParms(LPFLOOD_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ContBritWriteParms(LPCONTBRIT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC BalanceWriteParms(LPBALANCE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ShapeMaskWriteParms(LPSHAPEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC LowResLoadWriteParms(LPLOWRESLOAD_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC FreeMaskWriteParms(LPFREEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PointEditWriteParms(LPPOINTEDIT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString); 
WRITE_FUNC MagicMaskWriteParms(LPMAGICMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SimilarWriteParms(LPSIMILAR_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TransformerWriteParms(LPTRANSFORMER_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TransformObjectsWriteParms(LPTRANSFORMOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SelectObjectsWriteParms(LPSELECTOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC MoveObjectsWriteParms(LPMOVEOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SetOpacityWriteParms(LPSETATTROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC LayerObjectsWriteParms(LPLAYEROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PasteFromWriteParms(LPPASTEFROM_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PasteWriteParms(LPPASTE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC LoadMaskWriteParms(LPLOADMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC EffectWriteParms(LPEFFECT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC FeatherAlphaWriteParms(LPFEATHERALPHA_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC MaskSmoothWriteParms(LPMASK_SMOOTH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TextWriteParms(LPTEXT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ActivateWindowWriteParms(LPACTIVATEWINDOW_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC NewImageWriteParms(LPNEW_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC MapWriteParms(LPMODIFYMAP_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ToneWriteParms(LPTONE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PosterizeWriteParms(LPPOSTERIZE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC HueWriteParms(LPHUE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC ApplyCalWriteParms(LPAPPLYCAL_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC CopyToFileWriteParms(LPCOPYTOFILE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PaintWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SprayWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC CloneWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC TextureWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SmearWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC EraserWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC AltEraserWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC FilterWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PaintOnWriteParms(LPRETOUCH_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC DrawShapeWriteParms(LPDRAWSHAPE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC DrawFreeWriteParms(LPDRAWFREE_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PaletteEditWriteParms(LPPALETTEEDIT_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC AlignObjWriteParms(LPALIGNOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SetMergeModeWriteParms(LPSETATTROBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PosObjWriteParms(LPPOSOBJECTS_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC PSFilterWriteParms(LPPSFILTER_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC CropWriteParms(LPCROP_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SaveToAlbumWriteParms(LPSAVETOALBUM_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC FastBitsWriteParms(LPFASTBITS_PARMS lpParms, MACRO_FILE_HANDLE fh,	LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC StrokeMaskWriteParms(LPSTROKEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC SaveMaskWriteParms(LPSAVEMASK_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);
WRITE_FUNC DiscardUndoWriteParms(LPDISCARDUNDO_PARMS lpParms, MACRO_FILE_HANDLE fh, LPTSTR lpFormat, LPTSTR lpString);

void LoadFileFreeParms(LPLOADFILE_PARMS lpParms);
void FreeMaskFreeParms(LPFREEMASK_PARMS lpParms);
void PointEditFreeParms(LPPOINTEDIT_PARMS lpParms);
void EffectFreeParms(LPEFFECT_PARMS lpParms);
void TextFreeParms(LPTEXT_PARMS lpParms);
void RetouchFreeParms(LPRETOUCH_PARMS lpParms);
void DrawFreeFreeParms(LPDRAWFREE_PARMS lpParms);
void PaletteEditFreeParms(LPPALETTEEDIT_PARMS lpParms);
void PSFilterFreeParms(LPPSFILTER_PARMS lpParms);
void PasteFromFreeParms(LPPASTEFROM_PARMS lpParms);

#endif // _CMDPARMS_H_






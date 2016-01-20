/*
// $Header: /PM8/App/WMF.H 1     3/03/99 6:14p Gbeddow $
//
// Windows Metafile (WMF) file definitions.
//
// $Log: /PM8/App/WMF.H $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 7     5/29/98 2:39p Jayn
// Can now read non-placeable WMFs
// 
// 6     4/02/98 1:53p Jayn
// ScaleViewportExt
// 
// 5     2/06/98 11:35a Jayn
// Broke some functions out to use elsewhere.
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
//    Rev 1.0   14 Aug 1997 15:27:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:44   Fred
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 17:11:56   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.0   14 Mar 1996 13:48:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:22   FRED
// Initial revision.
// 
//    Rev 2.3   15 Sep 1995 14:04:14   JAY
// More 32-bit changes.
// 
//    Rev 2.2   11 May 1995 09:36:14   JAY
// Fixed bug with flipped IntersectClipRect record.
// 
//    Rev 2.1   09 Mar 1995 08:26:54   JAY
//  
// 
//    Rev 1.5   06 Mar 1995 10:41:22   JAY
// Fixed a few bugs with WMF handling:
// (1) Now does a RestoreDC within the enum proc before returning 0.
// (2) Some scaling adjustments to make offset scaled WMFs work correctly
//     within other WMFs.
// (3) Fixed a SelectPalette() problem with a NULL palette.
// Bugs left:
// Multiple nesting does not position the origin correctly.
// 
//    Rev 1.4   09 Jan 1995 16:26:32   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.3   23 May 1994 08:25:38   JAY
// Now uses prep_storage_file() so we can base the device on the document
// database device if necessary.
// 
//    Rev 1.2   18 Mar 1994 11:17:04   JAY
// Fix for "directional" records (ARC, CHORD, PIE).
// 
//    Rev 1.1   15 Mar 1994 15:14:12   JAY
// Support for coloring book.
// 
//    Rev 1.0   11 Mar 1994 14:23:12   JAY
// Initial revision.
*/

#ifndef __WMF_H__
#define __WMF_H__

#include "outline.h"

#define  ALDUS_WMF_KEY        0x9AC6CDD7

/* placeable metafile header */
typedef struct {
	DWORD   key;
#ifdef WIN32
	WORD hmf;
	RECTS bbox;
#else
	HMETAFILE hmf;
	RECT    bbox;
#endif
	WORD    inch;
	DWORD   reserved;
	WORD    checksum;
} ALDUS_WMF_HEADER;

// Log brush as stored in a WMF.
typedef struct
{
	WORD			lbStyle;
	COLORREF		lbColor;
	short int	lbHatch;
} LOGBRUSH16;

// Log pen as stored in a WMF.
typedef struct
{
	WORD		lopnStyle;
	POINTS	lopnWidth;
	COLORREF	lopnColor;
} LOGPEN16;

typedef struct
{
    short   lfHeight;
    short   lfWidth;
    short   lfEscapement;
    short   lfOrientation;
    short   lfWeight;
    BYTE    lfItalic;
    BYTE    lfUnderline;
    BYTE    lfStrikeOut;
    BYTE    lfCharSet;
    BYTE    lfOutPrecision;
    BYTE    lfClipPrecision;
    BYTE    lfQuality;
    BYTE    lfPitchAndFamily;
    char    lfFaceName[LF_FACESIZE];
} LOGFONT16;

class WMFHelper : public GraphicHelper
{
public:	

/*
// The pertinent data.
*/

	struct
	{
		SHORT m_type;
	} record;

/*
// The constructor.
*/

	WMFHelper(GraphicPtr graphic) : GraphicHelper(graphic) {}

/*
// Overridden methods (for storage purposes).
*/

	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_DEV_IO_SIZE size(StorageDevicePtr device);

/*
// The init function.
*/

	ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs);
};

//
// The representation of a GDI object in the WMF parser.
//

class CWMFGdiObject
{
public:
	enum ObjectType
	{
		Brush,
		Pen,
		Font,
		Palette,
		Region
	};
	CWMFGdiObject(ObjectType nType);
	virtual ~CWMFGdiObject() {}

	ObjectType m_nType;
};

// A brush object.
class CWMFBrushObject : public CWMFGdiObject
{
public:
	CWMFBrushObject(const LOGBRUSH16* pLogBrush = NULL);

	LOGBRUSH16 m_LogBrush;
};

// A pen object.
class CWMFPenObject : public CWMFGdiObject
{
public:
	CWMFPenObject(const LOGPEN16* pLogPen = NULL);

	LOGPEN16 m_LogPen;
};

// A font object.
class CWMFFontObject : public CWMFGdiObject
{
public:
	CWMFFontObject(const LOGFONT* pLogFont = NULL);

	LOGFONT m_LogFont;
};

// A palette object.
class CWMFPaletteObject : public CWMFGdiObject
{
public:
	CWMFPaletteObject();
	virtual ~CWMFPaletteObject();

	LOGPALETTE* m_pPalette;
};

// A region object.
class CWMFRegionObject : public CWMFGdiObject
{
public:
	CWMFRegionObject();
};

//
// The new state used for saving and restoring the state of the DC.
//

struct CWMFDCState
{
	CPoint m_cpWindowOrg;
	CPoint m_cpWindowExt;
	CPoint m_cpViewportScaleNum;
	CPoint m_cpViewportScaleDenom;

	int m_nCurrentBrush;
	int m_nCurrentPen;
	int m_nCurrentFont;
	int m_nCurrentPalette;
};

//
// The WMF update state (parser).
//

class WMFUpdateState : public OutlineUpdateState
{
protected:
/*
// Push and pop the DC state.
// These are used internally by SaveDC and RestoreDC.
*/

	void PushState(void);
	void PopState(void);

//
// Virtual functions.
//

	// Open the outline. This prepares to parse.
	virtual	ERRORCODE open_outline(void);
	// Parse the next command.
	virtual BOOL parse_next_command(BOOL *primitive);
	// The start of the file.
	virtual ST_DEV_POSITION file_start(void);
	// Read the next point.
	virtual BOOL read_next_point(OUTLINE_POINT *p);
	// Set default values.
	virtual void set_defaults(void);

//
// New variables.
//
	CPtrArray m_cpaGdiObjects;
	int m_nRecord;

	CWMFDCState m_DCState;
	Array m_DCStateArray;
	BOOL m_fHasAldusHeader;

	void NewSourceVars(void);

	// GDI object support.
	void NewObject(CWMFGdiObject* pObject);
	void SelectObject(int nIndex);
	void DeleteObject(int nIndex);

	void DoDibBlt(CRect crSource, CRect crdest, DWORD dwROP, WORD wUsage = DIB_RGB_COLORS);

	void CommonConstruct(void);

public:

/*
// Constructor.
*/

	WMFUpdateState(GraphicPtr pGraphic);
	WMFUpdateState(StorageDevice* pDevice);

/*
// Destructor.
*/

	virtual ~WMFUpdateState();
};

/*
// Fill out an ALDUS header based on the METAFILEPICT passed.
*/

extern void FillOutAldusHeader(ALDUS_WMF_HEADER& Header, const METAFILEPICT* pPict);

/*
// Create a WMF graphic from clipboard data.
*/

extern ERRORCODE CreateWMFFromMetaFilePict(LPCSTR pName,
														 PMGDatabase* pDatabase,
														 const METAFILEPICT* pPict,
														 GraphicObject*& pObject);

// Create a WMF file from clipboard data.

extern ERRORCODE CreateWMFFromMetaFilePict(const METAFILEPICT* pPict, LPCSTR pFileName);

#endif					// #ifndef __WMF_H__

/*
// WMF.CPP
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/WMF.CPP 1     3/03/99 6:14p Gbeddow $
//
// $Log: /PM8/App/WMF.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 13    9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 12    7/12/98 5:34p Jayn
// Fix for WMFs with Polylines with NULL pen.
// 
// 11    6/24/98 8:26a Jayn
// 
// 10    6/05/98 11:02a Jayn
// Fixed a bug in RestoreDC.
// 
// 9     5/29/98 2:39p Jayn
// Can now read non-placeable WMFs
// 
// 8     4/14/98 4:42p Jayn
// Support for TextOut.
// 
// 7     4/02/98 1:53p Jayn
// ScaleViewportExt
// 
// 6     2/06/98 11:35a Jayn
// Broke some functions out to use elsewhere.
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
//    Rev 1.0   14 Aug 1997 15:27:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:08   Fred
// Initial revision.
// 
//    Rev 1.7   07 Aug 1997 13:23:08   Jay
// Now uses draw bounds.
// 
//    Rev 1.6   09 Dec 1996 10:19:42   Jay
// More image stuff.
// 
//    Rev 1.5   05 Nov 1996 14:06:58   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.4   04 Nov 1996 16:06:06   Jay
// New object image routines.
// 
//    Rev 1.3   01 Nov 1996 17:11:50   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   15 Oct 1996 08:54:36   Jay
// is_printing -> IsScreen()
// 
//    Rev 1.1   25 Aug 1996 12:59:38   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:44:18   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:46:42   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:06:56   FRED
// Initial revision.
// 
//    Rev 2.11   09 Nov 1995 13:48:06   JAY
// Now handles open errors correctly.
// 
//    Rev 2.10   27 Sep 1995 15:25:46   JAY
// More 32-bit fixes.
// 
//    Rev 2.9   18 Sep 1995 10:15:20   JAY
// 32-bit fixes.
// 
//    Rev 2.8   15 Sep 1995 14:04:22   JAY
// More 32-bit changes.
// 
//    Rev 2.7   13 Sep 1995 11:33:42   JAY
// New 32-bit stuff
// 
//    Rev 2.6   07 Jul 1995 16:10:48   JAY
// Misc changes.
// 
//    Rev 2.5   05 Jun 1995 11:30:48   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.4   11 May 1995 09:36:24   JAY
// Fixed bug with flipped IntersectClipRect record.
// 
//    Rev 2.3   19 Apr 1995 12:31:30   JAY
//  
// 
//    Rev 2.2   09 Mar 1995 08:27:50   JAY
//  
// 
//    Rev 1.15   06 Mar 1995 10:43:06   JAY
// Fixed a few bugs with WMF handling:
// (1) Now does a RestoreDC within the enum proc before returning 0.
// (2) Some scaling adjustments to make offset scaled WMFs work correctly
//     within other WMFs.
// (3) Fixed a SelectPalette() problem with a NULL palette.
// Bugs left:
// Multiple nesting does not position the origin correctly.
// 
//    Rev 1.14   03 Mar 1995 08:42:18   JAY
// Doesn't use SaveDC index in RestoreDC if writing to a metafile.
// 
//    Rev 1.13   21 Feb 1995 14:44:08   JAY
// Changes when drawing to metafile; no more MakeProcInstance; etc.
// 
//    Rev 2.0   07 Feb 1995 15:48:18   JAY
// Initial revision.
// 
//    Rev 1.12   30 Jan 1995 09:18:16   JAY
// Improvements to metafile support.
// Uses attribute DC in RC when necessary.
// 
//    Rev 1.11   09 Jan 1995 16:26:56   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.10   30 Nov 1994 16:33:28   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.9   04 Aug 1994 07:32:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   23 May 1994 08:25:58   JAY
// Now uses prep_storage_file() so we can base the device on the document
// database device if necessary.
// 
//    Rev 1.7   16 May 1994 08:42:10   JAY
// Broke the RC gamma curve into two separate curves, one for bitmaps and one
// for outlines.
// 
//    Rev 1.6   19 Apr 1994 11:07:40   JAY
// Code in create_wmf() now ignores the actual amount of data read in case of
// weird (e.g. AI) WMFs which report a size which is too large.
// 
//    Rev 1.5   07 Apr 1994 10:47:02   JAY
// Checks m_use_ddbs and won't use DDBs if it's FALSE.
// 
//    Rev 1.4   18 Mar 1994 14:09:04   JAY
// Fixed problem with printout "scaling factor".
// 
//    Rev 1.3   18 Mar 1994 11:17:22   JAY
// Fix for "directional" records (ARC, CHORD, PIE).
// 
//    Rev 1.2   15 Mar 1994 15:14:22   JAY
// Support for coloring book.
// 
//    Rev 1.1   11 Mar 1994 17:17:30   JAY
// Text mask is now always black.
// 
//    Rev 1.0   11 Mar 1994 14:23:20   JAY
// Initial revision.
// 
*/

#include "stdafx.h"
#include "file.h"
#include "pmgdb.h"
#include "grafrec.h"
#include "grafobj.h"
#include "wmf.h"
#include "utils.h"
#include "objimg.h"
#include "memdev.h"

#include "bmp.h"

#define WMF_UPDATE_INTERVAL	750			// 3/4 second

#ifndef META_DRAWTEXT
#define META_DRAWTEXT	0x062f			// May not be used in Win32!
#endif

#define WIDTHBYTES(bits)      (((bits) + 31) / 32 * 4)

/////////////////////////////////////////////////////////////////////////////
// WMFHelper

/*
// Read the WMF helper info.
*/

ERRORCODE WMFHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the WMF helper info.
*/

ERRORCODE WMFHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the WMF helper info.
*/

ST_DEV_IO_SIZE WMFHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Initialize a WMF graphic.
*/

ERRORCODE WMFHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error = ERRORCODE_None;

/* We need a file to read. */

	StorageDevicePtr pSource;
	ReadOnlyFile file;

	if (gcs == NULL || (pSource = gcs->pSourceDevice) == NULL)
	{
	/* Go directly to disk. */
		file.set_name(graphic->m_csFileName);
		pSource = &file;
	}

/* Read the header. */

	ALDUS_WMF_HEADER header;

	if ((error = pSource->read(&header, sizeof(header))) != ERRORCODE_None)
	{
		return error;
	}

	// Fill out the header.
	if (header.key == ALDUS_WMF_KEY)
	{
		graphic->record.x_size = header.bbox.right - header.bbox.left;
		graphic->record.y_size = header.bbox.bottom - header.bbox.top;
		graphic->record.x_resolution = \
			graphic->record.y_resolution = header.inch;
	}
	else
	{
		graphic->record.x_size = 300*2;
		graphic->record.y_size = 300*2;
		graphic->record.x_resolution = \
			graphic->record.y_resolution = 300;
	}

	graphic->record.storage = GRAPHIC_STORAGE_FILE;

	record.m_type = 0;

	return error;
}

/////////////////////////////////////////////////////////////////////////////
// CWMFGdiObject

CWMFGdiObject::CWMFGdiObject(CWMFGdiObject::ObjectType nType)
{
	m_nType = nType;
}

/////////////////////////////////////////////////////////////////////////////
// CWMFBrushObject

CWMFBrushObject::CWMFBrushObject(const LOGBRUSH16* pLogBrush /*=NULL*/)
	: CWMFGdiObject(CWMFGdiObject::Brush)
{
	if (pLogBrush != NULL)
	{
		m_LogBrush = *pLogBrush;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWMFPenObject

CWMFPenObject::CWMFPenObject(const LOGPEN16* pLogPen /*=NULL*/)
	: CWMFGdiObject(CWMFGdiObject::Pen)
{
	if (pLogPen != NULL)
	{
		m_LogPen = *pLogPen;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWMFFontObject

CWMFFontObject::CWMFFontObject(const LOGFONT* pLogFont /*=NULL*/)
	: CWMFGdiObject(CWMFGdiObject::Font)
{
	if (pLogFont != NULL)
	{
		m_LogFont = *pLogFont;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWMFPaletteObject

CWMFPaletteObject::CWMFPaletteObject()
	: CWMFGdiObject(CWMFGdiObject::Palette)
{
	m_pPalette = NULL;
}

CWMFPaletteObject::~CWMFPaletteObject()
{
	delete [] (LPBYTE)m_pPalette;
}

/////////////////////////////////////////////////////////////////////////////
// CWMFRegionObject

CWMFRegionObject::CWMFRegionObject()
	: CWMFGdiObject(CWMFGdiObject::Region)
{
}

/////////////////////////////////////////////////////////////////////////////
// WMFUpdateState

/*
// Update state constructor.
*/

WMFUpdateState::WMFUpdateState(GraphicPtr pGraphic)
	: OutlineUpdateState(pGraphic), m_DCStateArray(sizeof(CWMFDCState))
{
	CommonConstruct();
}

WMFUpdateState::WMFUpdateState(StorageDevice* pDevice)
	: OutlineUpdateState(pDevice), m_DCStateArray(sizeof(CWMFDCState))
{
	CommonConstruct();
}

void WMFUpdateState::CommonConstruct(void)
{
	m_fHasAldusHeader = TRUE;
}

/*
// Update state destructor.
*/

WMFUpdateState::~WMFUpdateState()
{
	// Delete all the GDI objects.
	for (int i = 0; i < m_cpaGdiObjects.GetSize(); i++)
	{
		CWMFGdiObject* pObject = (CWMFGdiObject*)m_cpaGdiObjects.GetAt(i);
		delete pObject;
	}
}

// Open the outline. This prepares to parse.
ERRORCODE WMFUpdateState::open_outline(void)
{
	m_nRecord = 0;
	ERRORCODE error;
	if ((error = OutlineUpdateState::open_outline()) == ERRORCODE_None)
	{
		DWORD dwKey;
		if ((error = file.read(&dwKey, sizeof(dwKey))) == ERRORCODE_None)
		{
			m_fHasAldusHeader = (dwKey == ALDUS_WMF_KEY);
			error = file.seek(0, ST_DEV_SEEK_SET);
		}
	}
	return error;
}

// Parse the next command.
BOOL WMFUpdateState::parse_next_command(BOOL *primitive)
{
	BOOL fPrimitive = FALSE;
	// Remember the start of the record.
	ST_DEV_POSITION lRecordOffset;
	file.tell(&lRecordOffset);

	// Read the next record.
	METARECORD Record;
	if ((error = file.read(&Record, sizeof(Record)-sizeof(Record.rdParm))) != ERRORCODE_None)
	{
		return FALSE;
	}

//	TRACE("Record %d function: %04x, size: %08lx\n", m_nRecord, Record.rdFunction, Record.rdSize);
	// Process the record.
	switch (Record.rdFunction)
	{
		case 0:
		{
			// End of records. All done.
			return FALSE;
		}
		case META_ESCAPE:
		{
//			short n;
//			file.read(&n, sizeof(n));
//			TRACE("Escape: %x\n", n);
			break;
		}
		case META_SETROP2:
		case META_SETRELABS:
		case META_SETMAPMODE:
		{
//			TRACE("IGNORED Record %d function: %04x, size: %08lx\n", m_nRecord, Record.rdFunction, Record.rdSize);
			break;
		}
		case META_SETTEXTALIGN:
		{
			short nAlign;
			file.read(&nAlign, sizeof(nAlign));
			SetTextAlign(nAlign);
			break;
		}
		case META_SETBKCOLOR:
		{
			COLORREF Color;
			file.read(&Color, sizeof(Color));
			SetBkColor(Color);
			break;
		}
		case META_SETBKMODE:
		{
			short nMode;
			file.read(&nMode, sizeof(nMode));
			SetBkMode(nMode);
			break;
		}
		case META_SETTEXTCOLOR:
		{
			COLORREF Color;
			file.read(&Color, sizeof(Color));
			SetTextColor(Color);
			break;
		}
		case META_SETWINDOWORG:
		{
			// Read the parameter.
			short Parms[2];
			ASSERT(Record.rdSize == 5);
			file.read(Parms, sizeof(Parms));
			m_DCState.m_cpWindowOrg.y = Parms[0];
			m_DCState.m_cpWindowOrg.x = Parms[1];
			NewSourceVars();
			break;
		}
		case META_SETWINDOWEXT:
		{
			// Read the parameter.
			short Parms[2];
			ASSERT(Record.rdSize == 5);
			file.read(Parms, sizeof(Parms));
			m_DCState.m_cpWindowExt.y = Parms[0];
			m_DCState.m_cpWindowExt.x = Parms[1];
			NewSourceVars();
			break;
		}
		case META_CREATEBRUSHINDIRECT:
		{
			CWMFBrushObject* pNewBrush = new CWMFBrushObject;
			file.read(&pNewBrush->m_LogBrush, sizeof(LOGBRUSH16));
			NewObject(pNewBrush);
			break;
		}
		case META_DIBCREATEPATTERNBRUSH:
		{
			CWMFBrushObject* pNewBrush = new CWMFBrushObject;
			pNewBrush->m_LogBrush.lbStyle = BS_SOLID;
			pNewBrush->m_LogBrush.lbColor = RGB(128, 0, 0);
			NewObject(pNewBrush);
			break;
		}
		case META_CREATEPENINDIRECT:
		{
			CWMFPenObject* pNewPen = new CWMFPenObject;
			file.read(&pNewPen->m_LogPen, sizeof(LOGPEN16));
			NewObject(pNewPen);
//			TRACE("New pen - s: %d; w: %d, %d; c: %08lx\n",
//					pNewPen->m_LogPen.lopnStyle,
//					pNewPen->m_LogPen.lopnWidth.x,
//					pNewPen->m_LogPen.lopnWidth.y,
//					pNewPen->m_LogPen.lopnColor);
			break;
		}
		case META_CREATEFONTINDIRECT:
		{
			CWMFFontObject* pNewFont = new CWMFFontObject;
			// Read the data.
			LOGFONT16 lf;
			file.read(&lf, sizeof(LOGFONT16));

			// Translate it over.
			pNewFont->m_LogFont.lfHeight = lf.lfHeight; 
			pNewFont->m_LogFont.lfWidth = lf.lfWidth; 
			pNewFont->m_LogFont.lfEscapement = lf.lfEscapement; 
			pNewFont->m_LogFont.lfOrientation = lf.lfOrientation; 
			pNewFont->m_LogFont.lfWeight = lf.lfWeight;
			// Warning: hard-coded size ahead.
			memcpy(&pNewFont->m_LogFont.lfItalic, &lf.lfItalic, 8 + LF_FACESIZE);

			NewObject(pNewFont);
			break;
		}
		case META_CREATEPALETTE:
		{
			// This is mostly here to make sure the object array stays in sync.
			CWMFPaletteObject* pNewPalette = new CWMFPaletteObject;
			struct
			{
				WORD palVersion;
				WORD palNumEntries;
			} Header;
			file.read(&Header, sizeof(Header));

			// Now we have the header. See how many entries we want.
			if (Header.palVersion == 0x0300)
			{
				int nPaletteSize = sizeof(PALETTEENTRY)*Header.palNumEntries;
				pNewPalette->m_pPalette = (LOGPALETTE*)new BYTE[sizeof(LOGPALETTE) + nPaletteSize - sizeof(PALETTEENTRY)];
				pNewPalette->m_pPalette->palVersion = Header.palVersion;
				pNewPalette->m_pPalette->palNumEntries = Header.palNumEntries;
				file.read(pNewPalette->m_pPalette->palPalEntry, nPaletteSize);
			}

			NewObject(pNewPalette);
			break;
		}
		case META_CREATEREGION:
		{
			// This is mostly here to make sure the object array stays in sync.
			// I don't think there's hope of determining the parms format for
			// this record.
			CWMFRegionObject* pNewRegion = new CWMFRegionObject;
			NewObject(pNewRegion);
			break;
		}
		case META_SELECTPALETTE:
		case META_SELECTOBJECT:
		{
			short nIndex;
			file.read(&nIndex, sizeof(nIndex));
			SelectObject(nIndex);
//			TRACE("SelectObject: %d\n", nIndex);
			break;
		}
		case META_DELETEOBJECT:
		{
			short nIndex;
			file.read(&nIndex, sizeof(nIndex));
			DeleteObject(nIndex);
//			TRACE("DeleteObject: %d\n", nIndex);
			break;
		}
		case META_SETPOLYFILLMODE:
		{
			short nFillMode;
			file.read(&nFillMode, sizeof(nFillMode));
			m_nFillMode = nFillMode;
			break;
		}
		case META_SETSTRETCHBLTMODE:
		{
			short nMode;
			file.read(&nMode, sizeof(nMode));
			SetBltMode(nMode);
			break;
		}
		case META_POLYPOLYGON:
		{
			// Read the number of polygons.
			short nPolygons;
			file.read(&nPolygons, sizeof(nPolygons));
			// Proceed to read counts and points (and draw).
			int* pCounts = NULL;
			POINT* pPoints = NULL;
			TRY
			{
				// Allocate the polygon counts.
				pCounts = new int[nPolygons];
				// Read the polygon counts.
				int nPoints = 0;
				for (int nPolygon = 0; nPolygon < nPolygons; nPolygon++)
				{
					short p;
					file.read(&p, sizeof(p));
					pCounts[nPolygon] = p;
					nPoints += p;
				}
				// Allocate the polygon points.
				pPoints = new POINT[nPoints];
				// Read the polygon points.
				for (int nPoint = 0; nPoint < nPoints; nPoint++)
				{
					short p[2];
					file.read(p, sizeof(p));
					pPoints[nPoint].x = p[0];
					pPoints[nPoint].y = p[1];
				}

				// Draw the polypolygon.
				DrawPolyPolygon(pPoints, pCounts, nPolygons);
			}
			END_TRY
			delete [] pPoints;
			delete [] pCounts;
			fPrimitive = TRUE;
			break;
		}
		case META_POLYGON:
		{
			WORD wCount;
			file.read(&wCount, sizeof(wCount));
			draw_polygon(wCount);
			fPrimitive = TRUE;
			break;
		}
		case META_POLYLINE:
		{
			WORD wCount;
			file.read(&wCount, sizeof(wCount));
			draw_polyline(wCount, TRUE);
			fPrimitive = TRUE;
			break;
		}
		case META_ELLIPSE:
		{
			short Parms[4];
			file.read(Parms, sizeof(Parms));

			// Compute parameters to pass.
			// Parms[3] = left
			// Parms[2] = top
			// Parms[1] = right
			// Parms[0] = bottom

			OUTLINE_POINT center;
			center.x = (short)((int)Parms[3] + (int)Parms[1])/2;
			center.y = (short)((int)Parms[2] + (int)Parms[0])/2;

			short rx = Parms[3] - center.x;
			if (rx < 0) rx = -rx;

			short ry = Parms[2] - center.y;
			if (ry < 0) ry = -ry;

			draw_ellipse(center, rx, ry);
			fPrimitive = TRUE;
			break;
		}
		case META_ROUNDRECT:
		{
			// Cheat for now. Just draw it as a rectangle.
			// Skip the corner radii.
			file.seek(2*sizeof(short), ST_DEV_SEEK_CUR);

			// Fall through to...
		}
		case META_RECTANGLE:
		{
			short Parms[4];
			file.read(Parms, sizeof(Parms));

			// Parms[3] = left
			// Parms[2] = top
			// Parms[1] = right
			// Parms[0] = bottom

			OUTLINE_POINT p0, p1;
			p0.x = Parms[3];
			p0.y = Parms[2];
			p1.x = Parms[1];
			p1.y = Parms[0];

			draw_rectangle(p0, p1);
			fPrimitive = TRUE;
			break;
		}
		case META_DIBSTRETCHBLT:
		{
			// Read the numeric parameters.
			short Parms[10];
			file.read(Parms, sizeof(Parms));

			// Parms[0] = low-order word of raster op
			// Parms[1] = high-order word of raster op
			// Parms[2] = source y extent
			// Parms[3] = source x extent
			// Parms[4] = source y coordinate
			// Parms[5] = source x coordinate
			// Parms[6] = destination y extent
			// Parms[7] = destination x extent
			// Parms[8] = destination y coordinate
			// Parms[9] = destination x coordinate
			CRect crSource(CPoint(Parms[5], Parms[4]), CSize(Parms[3], Parms[2]));
			CRect crDest(CPoint(Parms[9], Parms[8]), CSize(Parms[7], Parms[6]));
			DWORD dwROP = MAKELONG(Parms[0], Parms[1]);

			// Process the rest of the dib blt.
			DoDibBlt(crSource, crDest, dwROP);

			fPrimitive = TRUE;
			break;
		}
		case META_DIBBITBLT:
		{
			// Read the numeric parameters.
			short Parms[7];
			file.read(Parms, sizeof(Parms));

			// Parms[0] = high-order word of raster op
			// Parms[1] = source y coordinate
			// Parms[2] = source x coordinate
			// Parms[3] = destination y extent
			// Parms[4] = destination x extent
			// Parms[5] = destination y coordinate
			// Parms[6] = destination x coordinate
			CRect crSource(CPoint(Parms[5], Parms[4]), CSize(Parms[7], Parms[6]));
			CRect crDest(CPoint(Parms[9], Parms[8]), CSize(Parms[7], Parms[6]));
			DWORD dwROP = MAKELONG(0, Parms[1]);

			// Process the rest of the dib blt.
			DoDibBlt(crSource, crDest, dwROP);

			fPrimitive = TRUE;
			break;
		}
		case META_STRETCHDIB:
		{
			// Read the numeric parameters.
			short Parms[11];
			file.read(Parms, sizeof(Parms));

			// Parms[0]  = low-order word of raster op
			// Parms[1]  = high-order word of raster op
			// Parms[2]  = usage flag
			// Parms[3]  = source y extent
			// Parms[4]  = source x extent
			// Parms[5]  = source y coordinate
			// Parms[6]  = source x coordinate
			// Parms[7]  = destination y extent
			// Parms[8]  = destination x extent
			// Parms[9]  = destination y coordinate
			// Parms[10] = destination x coordinate
			CRect crSource(CPoint(Parms[6], Parms[5]), CSize(Parms[4], Parms[3]));
			CRect crDest(CPoint(Parms[10], Parms[9]), CSize(Parms[8], Parms[7]));
			DWORD dwROP = MAKELONG(Parms[0], Parms[1]);
			WORD wUsage = (WORD)Parms[2];

			// Process the rest of the dib blt.
			DoDibBlt(crSource, crDest, dwROP, wUsage);

			fPrimitive = TRUE;
			break;
		}
		case META_EXTTEXTOUT:
		{
			// Handle ExtTextOut call.

			// Read the numeric parameters.
			short Parms[4];
			file.read(Parms, sizeof(Parms));

			// Parms[0] = y
			// Parms[1] = x
			// Parms[2] = string length
			// Parms[3] = option flags

			RECTS rClip;
			if (Parms[3] != 0)
			{
				file.read(&rClip, sizeof(rClip));
			}

			// String data follows

			int nStrLength = Parms[2];
			int nStrSize = (nStrLength + 1) & ~1;
			LPBYTE pString = NULL;
			TRY
			{
				// Read the text.
				pString = new BYTE[nStrSize];
				file.huge_read(pString, nStrSize);

				// See if there are any widths.
				ST_DEV_POSITION Here;
				file.tell(&Here);
				ST_DEV_POSITION lRecordEnd = lRecordOffset + Record.rdSize*sizeof(WORD);
//				TRACE("Count:%d; Here: %ld; lRecordEnd: %ld\n",
//						nStrLength, Here, lRecordEnd);
				int nDXSize = nStrLength*sizeof(short);
				short* pDX = NULL;
				if (lRecordEnd >= Here + nDXSize)
				{
					pDX = new short[nStrLength];
					file.read(pDX, nDXSize);
				}

				OUTLINE_POINT p;
				p.x = (short)Parms[1];
				p.y = (short)Parms[0];

				RECT r;
				r.left = rClip.left;
				r.top = rClip.top;
				r.right = rClip.right;
				r.bottom = rClip.bottom;
				// Draw the text.
				DrawText(p, Parms[3], (LPCSTR)pString, nStrLength, &r, pDX);

				// Free the dx array.
				delete [] pDX;
				// Free the text.
				delete pString;
			}
			END_TRY
			fPrimitive = TRUE;
			break;
		}
		case META_TEXTOUT:
		{
			// Handle TextOut call.

			// Read the string size.
			short nStrLength;
			file.read(&nStrLength, sizeof(nStrLength));

			int nStrSize = (nStrLength + 1) & ~1;
			LPBYTE pString = NULL;
			TRY
			{
				// Read the text.
				pString = new BYTE[nStrSize];
				file.huge_read(pString, nStrSize);
				// Read the x and y.
				short Parms[2];
				file.read(Parms, sizeof(Parms));

				OUTLINE_POINT p;
				p.x = (short)Parms[1];
				p.y = (short)Parms[0];

				// Draw the text.
				DrawText(p, 0, (LPCSTR)pString, nStrLength, NULL);

				// Free the text.
				delete pString;
			}
			END_TRY
			fPrimitive = TRUE;
			break;
		}
		case META_MOVETO:
		case META_LINETO:
		{
			short Parms[2];
			file.read(&Parms, sizeof(Parms));

//			TRACE("%s: %d, %d\n", Record.rdFunction == META_MOVETO ? "MoveTo" : "LineTo", Parms[1], Parms[0]);

			OUTLINE_POINT p;
			p.x = Parms[1];
			p.y = Parms[0];

			if (Record.rdFunction == META_MOVETO)
			{
				// Move to.
				MoveTo(p);
			}
			else
			{
				// Must be Line to.
				LineTo(p);
				fPrimitive = TRUE;
			}
			break;
		}
		case META_PATBLT:
		{
			WORD Parms[6];
			// Parms[0] - op low word
			// Parms[1] - op high word
			// Parms[2] - height
			// Parms[3] - width
			// Parms[4] - top
			// Parms[5] - left
			file.read(Parms, sizeof(Parms));

			OUTLINE_POINT p0, p1;
			p0.x = Parms[5];
			p0.y = Parms[4];
			p1.x = p0.x + Parms[3];
			p1.y = p0.y + Parms[2];

			DoPatBlt(p0, p1, MAKELONG(Parms[0], Parms[1]));

			fPrimitive = TRUE;
			break;
		}
		case META_PIE:
		case META_ARC:
		{
			short Parms[8];
			// Parms[0] = y4		end pt y
			// Parms[1] = x4		end pt x
			// Parms[2] = y3		start pt y
			// Parms[3] = x3		start pt x
			// Parms[4] = y2		Bound bottom
			// Parms[5] = x2		Bound right
			// Parms[6] = y1		Bound top
			// Parms[7] = x1		Bound left
			file.read(Parms, sizeof(Parms));

			OUTLINE_POINT Center;
			Center.x = (short)midpoint(Parms[7], Parms[5]);
			Center.y = (short)midpoint(Parms[6], Parms[4]);

			double rx = Parms[5] - Center.x;
			if (rx < 0) rx = -rx;
			double ry = Parms[4] - Center.y;
			if (ry < 0) ry = -ry;

			double dStart = angle_from_vector(Parms[2] - Center.y, Parms[3] - Center.x);
			double dEnd = angle_from_vector(Parms[0] - Center.y, Parms[1] - Center.x);

			draw_elliptical_arc(Center, rx, ry, dStart-PI/2, dEnd-PI/2, (Record.rdFunction == META_PIE) ? 1 : -1);
			break;
		}
		case META_SAVEDC:
		{
			PushState();
			break;
		}
		case META_RESTOREDC:
		{
			PopState();
			break;
		}
		case META_SCALEVIEWPORTEXT:
		{
			// Read the parameter.
			short Parms[4];
			// Parms[0] = ydenom
			// Parms[1] = ynum
			// Parms[2] = xdenom
			// Parms[3] = xnum
			file.read(Parms, sizeof(Parms));

			m_DCState.m_cpViewportScaleNum.x = Parms[3];
			m_DCState.m_cpViewportScaleDenom.x = Parms[2];
			m_DCState.m_cpViewportScaleNum.y = Parms[1];
			m_DCState.m_cpViewportScaleDenom.y = Parms[0];

			NewSourceVars();
			break;
		}
		default:
		{
			TRACE("UNKNOWN Record %d function: %04x, size: %08lx\n", m_nRecord, Record.rdFunction, Record.rdSize);
			break;
		}
	}

	m_nRecord++;

	// Seek past the record
	file.seek(lRecordOffset + Record.rdSize*sizeof(WORD), ST_DEV_SEEK_SET);
	*primitive = fPrimitive;
	return TRUE;
}

ST_DEV_POSITION WMFUpdateState::file_start(void)
{
	int nSize = sizeof(METAHEADER);
	if (m_fHasAldusHeader)
	{
		nSize += sizeof(ALDUS_WMF_HEADER);
	}
	return nSize;
}

/*
// Read the next point.
// This is used by polygon and polyline code.
*/

BOOL WMFUpdateState::read_next_point(OUTLINE_POINT *p)
{
	short Point[2];
	file.read(Point, sizeof(Point));
	p->x = Point[0];
	p->y = Point[1];
	return TRUE;
}

void WMFUpdateState::set_defaults(void)
{
	OutlineUpdateState::set_defaults();

	m_DCState.m_cpWindowOrg.x = 0;
	m_DCState.m_cpWindowOrg.y = 0;
	m_DCState.m_cpWindowExt.x = 1;
	m_DCState.m_cpWindowExt.y = 1;

	// No viewport scaling yet.
	m_DCState.m_cpViewportScaleNum.x = 1;
	m_DCState.m_cpViewportScaleNum.y = 1;
	m_DCState.m_cpViewportScaleDenom.x = 1;
	m_DCState.m_cpViewportScaleDenom.y = 1;

	// No current objects.
	m_DCState.m_nCurrentBrush = -1;
	m_DCState.m_nCurrentPen = -1;
	m_DCState.m_nCurrentFont = -1;
	m_DCState.m_nCurrentPalette = -1;
}

void WMFUpdateState::NewSourceVars(void)
{
	OUTLINE_POINT Org;
	OUTLINE_POINT Ext;
	Org.x = (short)m_DCState.m_cpWindowOrg.x;
	Org.y = (short)m_DCState.m_cpWindowOrg.y;
	Ext.x = (short)m_DCState.m_cpWindowExt.x;
	Ext.y = (short)m_DCState.m_cpWindowExt.y;

	// Handle viewport scaling.
	if (m_DCState.m_cpViewportScaleNum.x != 0)
		Ext.x = scale_number(Ext.x, m_DCState.m_cpViewportScaleDenom.x, m_DCState.m_cpViewportScaleNum.x);
	if (m_DCState.m_cpViewportScaleNum.y != 0)
		Ext.y = scale_number(Ext.y, m_DCState.m_cpViewportScaleDenom.y, m_DCState.m_cpViewportScaleNum.y);

	if (Ext.x < 0)
	{
		Org.x += Ext.x;
		Ext.x = -Ext.x;
		reverse_x_transform = !reverse_x_transform;
	}
	if (Ext.y < 0)
	{
		Org.y += Ext.y;
		Ext.y = -Ext.y;
		reverse_y_transform = !reverse_y_transform;
	}

	// set_source_extent wants upper left, lower right. Switch from org, ext.
	Ext.x += Org.x;
	Ext.y += Org.y;
	set_source_extent(Org, Ext);
}

//
// Handle a new GDI object.
//

void WMFUpdateState::NewObject(CWMFGdiObject* pObject)
{
	int nSize = m_cpaGdiObjects.GetSize();
	// Search for an empty slot.
	for (int i = 0; i < nSize; i++)
	{
		if (m_cpaGdiObjects.GetAt(i) == NULL)
		{
			m_cpaGdiObjects.SetAt(i, pObject);
			return;
		}
	}

	// No empty slots. Add the object.
	m_cpaGdiObjects.Add(pObject);
}

//
// Select a GDI object.
//

void WMFUpdateState::SelectObject(int nIndex)
{
	ASSERT(nIndex < m_cpaGdiObjects.GetSize());
	if (nIndex != -1 && nIndex < m_cpaGdiObjects.GetSize())
	{
		CWMFGdiObject* pObject = (CWMFGdiObject*)m_cpaGdiObjects.GetAt(nIndex);
		ASSERT(pObject != NULL);
		if (pObject != NULL)
		{
			// We have an object.
			switch (pObject->m_nType)
			{
				case CWMFGdiObject::Brush:
				{
					m_DCState.m_nCurrentBrush = nIndex;
					CWMFBrushObject* pBrush = (CWMFBrushObject*)pObject;

					// Handle the interior style.
					switch (pBrush->m_LogBrush.lbStyle)
					{
						case BS_NULL:
						{
							set_interior_style(STYLE_Empty);
							break;
						}
						case BS_HATCHED:
						{
							set_interior_style(STYLE_Hatch);
//							set_hatch_index(?);
							break;
						}
						case BS_PATTERN:
						{
							set_interior_style(STYLE_Pattern);
							break;
						}
						default:
						case BS_SOLID:
						{
							set_interior_style(STYLE_Solid);
							break;
						}
					}

					// Handle the interior color.
					set_fill_color(RGBToOutlineColor(pBrush->m_LogBrush.lbColor));
					break;
				}
				case CWMFGdiObject::Pen:
				{
					m_DCState.m_nCurrentPen = nIndex;
					CWMFPenObject* pPen = (CWMFPenObject*)pObject;

					// Handle the edge visibility.
					set_edge_visibility(pPen->m_LogPen.lopnStyle != PS_NULL);

					// Handle the line/edge color.
					OUTLINE_COLOR Color = RGBToOutlineColor(pPen->m_LogPen.lopnColor);
					set_line_color(Color);
					set_edge_color(Color);

//					TRACE("Select Pen - Style: %u; Width: %d, %d; Color: %08lx\n",
//							pPen->m_LogPen.lopnStyle,
//							pPen->m_LogPen.lopnWidth.x,
//							pPen->m_LogPen.lopnWidth.y,
//							pPen->m_LogPen.lopnColor);

					// Handle the line/edge width.
					int nWidth = (pPen->m_LogPen.lopnWidth.y > pPen->m_LogPen.lopnWidth.x)
										? pPen->m_LogPen.lopnWidth.y : pPen->m_LogPen.lopnWidth.x;
					set_edge_width((short)nWidth);
					set_line_width((short)nWidth);
					break;
				}
				case CWMFGdiObject::Font:
				{
					m_DCState.m_nCurrentFont = nIndex;
					SetCurrentFont(((CWMFFontObject*)pObject)->m_LogFont);

					break;
				}
				case CWMFGdiObject::Palette:
				{
					m_DCState.m_nCurrentPalette = nIndex;
					SetCurrentPalette(((CWMFPaletteObject*)pObject)->m_pPalette);

					break;
				}
				default:
				{
					// Invalid GDI object type.
					ASSERT(FALSE);
					break;
				}
			}
		}
	}
}

//
// Delete a GDI object.
//

void WMFUpdateState::DeleteObject(int nIndex)
{
	ASSERT(nIndex < m_cpaGdiObjects.GetSize());
	if (nIndex < m_cpaGdiObjects.GetSize())
	{
		CWMFGdiObject* pObject = (CWMFGdiObject*)m_cpaGdiObjects.GetAt(nIndex);
		ASSERT(pObject != NULL);
		if (pObject != NULL)
		{
			ASSERT(nIndex != m_DCState.m_nCurrentBrush);
			ASSERT(nIndex != m_DCState.m_nCurrentPen);
			ASSERT(nIndex != m_DCState.m_nCurrentFont);
			ASSERT(nIndex != m_DCState.m_nCurrentPalette);
			delete pObject;
			m_cpaGdiObjects.SetAt(nIndex, NULL);
		}
	}
}

// Push the DC state.
// This is used internally by SaveDC.

void WMFUpdateState::PushState(void)
{
	m_DCStateArray.add_element(&m_DCState);
}

// Pop the DC state.
// This is used internally by RestoreDC.

void WMFUpdateState::PopState(void)
{
/* Make sure we're sane. */

	int nSize;
	if ((nSize = m_DCStateArray.count()) > 0)
	{
		// Do the pop.
		m_DCState = *(CWMFDCState*)m_DCStateArray.get_element(nSize-1);
		m_DCStateArray.delete_element(nSize-1);

		// Reselect the current objects to set the internal states.
		SelectObject(m_DCState.m_nCurrentBrush);
		SelectObject(m_DCState.m_nCurrentPen);
		SelectObject(m_DCState.m_nCurrentFont);
		SelectObject(m_DCState.m_nCurrentPalette);
	}
}

//
// Process a dib blt.
// The numeric arguments have been processed by the caller, and
// the DIB header and bits are still remaining to be read from "file".
//

void WMFUpdateState::DoDibBlt(CRect crSource, CRect crDest, DWORD dwROP, WORD wUsage /*=DIB_RGB_COLORS*/)
{
	// Read the bitmap info header. This tells us how many colors we have.
	BITMAPINFOHEADER bmi;
	file.read(&bmi, sizeof(bmi));
//	TRACE("DoBitBlt\n");

	// Compute the size of the color table.
	// If wUsage is DIB_PAL_COLORS, then the color table is composed of 16-bit
	// indices into the currently realized palette.
	WORD wPaletteSize = DIBNumColors(&bmi) * (wUsage == DIB_PAL_COLORS ? sizeof(WORD) : sizeof(RGBQUAD));

	BITMAPINFO* pbi = NULL;
	LPBYTE pData = NULL;

	TRY
	{
		if (wPaletteSize == 0)
		{
			// We do not have a palette. Just use the info header as is.
			pbi = (BITMAPINFO*)&bmi;
		}
		else
		{
			pbi = (BITMAPINFO*)(new BYTE[sizeof(bmi) + wPaletteSize]);

			// Copy over the header.
			pbi->bmiHeader = bmi;

			// Read the color table.
			file.read(pbi->bmiColors, wPaletteSize);
		}

		ASSERT(pbi != NULL);
		// Read the bitmap data.
		DWORD dwSize = pbi->bmiHeader.biSizeImage;
		if (dwSize == 0)
		{
			// Compute biSizeImage.
			dwSize = WIDTHBYTES (pbi->bmiHeader.biWidth * pbi->bmiHeader.biBitCount) * pbi->bmiHeader.biHeight;
		}
		pData = new BYTE[dwSize];
		file.huge_read(pData, dwSize);

//		TRACE("DIB -  w:%d, h:%d, b:%d (s: %d); ROP: %08lx\n",
//				pbi->bmiHeader.biWidth,
//				pbi->bmiHeader.biHeight,
//				pbi->bmiHeader.biBitCount,
//				dwSize,
//				dwROP);

		// Now, draw it! (Arguments similar to StretchDIBits).
		DrawBitmap(crDest, crSource, pData, pbi, wUsage, dwROP);
	}
	END_TRY

	// Free any memory we allocated.
	delete [] pData;
	if (wPaletteSize != 0)
	{
		// Free the bitmap info we allocated.
		delete [] (BYTE*)pbi;
	}
}

/*
// Fill out an ALDUS header based on the METAFILEPICT passed.
*/

void FillOutAldusHeader(ALDUS_WMF_HEADER& Header, const METAFILEPICT* pPict)
{
	Header.key = ALDUS_WMF_KEY;
	Header.hmf = NULL;				// Not used.
	Header.bbox.left = 0;
	Header.bbox.top = 0;
	Header.bbox.right = (short int)pPict->xExt;
	Header.bbox.bottom = (short int)pPict->yExt;
	if (Header.bbox.right < 0)
	{
		Header.bbox.right = -Header.bbox.right;
	}
	if (Header.bbox.bottom < 0)
	{
		Header.bbox.bottom = -Header.bbox.bottom;
	}
	int nMapMode = pPict->mm;

	if (nMapMode < MM_TEXT || nMapMode > MM_ANISOTROPIC)
	{
		nMapMode = MM_TEXT;
	}

	static int BASED_CODE nMMRes[] =
	{
		96,					// MM_TEXT
		254,					// MM_LOMETRIC
		2540,					// MM_HIMETRIC
		100,					// MM_LOENGLISH
		1000,					// MM_HIENGLISH
		1440,					// MM_TWIPS
		2540,					// MM_ISOTROPIC == MM_HIMETRIC for units
		2540					// MM_ANISOTROPIC == MM_HIMETRIC for units
	};

	Header.inch = nMMRes[nMapMode-1];
	Header.reserved = 0;
	Header.checksum = 0;				// We ignore this.
}

/*
// Create a WMF graphic from a clipboard metafile.
*/

ERRORCODE CreateWMFFromMetaFilePict(LPCSTR pName,
												PMGDatabase* pDatabase,
												const METAFILEPICT* pPict,
												GraphicObject*& pObject)
{
	ERRORCODE error;

	pObject = NULL;
	
//	od("WMF - mm: %d, xExt: %d, yExt: %d, hMF: %u\r\n",
//		pPict->mm,
//		pPict->xExt,
//		pPict->yExt,
//		pPict->hMF);

/*
// Build the device image.
// This will consist of two pieces:
// (1) Our constructed placeable metafile header, and
// (2) The actual WMF data.
*/

	MemoryDevice DataDevice;

	// (1) The header.

	ALDUS_WMF_HEADER Header;

	FillOutAldusHeader(Header, pPict);

	DataDevice.AddPiece(&Header, sizeof(Header));

	// (2) The WMF data.

	error = ERRORCODE_Memory;

	HMETAFILE hMF;
#ifdef WIN32
#ifdef _MAC
   // TODO:MAC -- GetMetaFileBitsEx not supported
   hMF = NULL;
   if (0)
   {
      HGLOBAL hMem = NULL;
      if (0)
      {
         LPVOID pMem = NULL;
         if (0)
         {
#else //_MAC
	UINT uSizeData;
	if ((hMF = pPict->hMF) != NULL
		 && (uSizeData = ::GetMetaFileBitsEx(hMF, 0, NULL)) != 0)
	{
		HGLOBAL hMem;
		if ((hMem = GlobalAlloc(GMEM_MOVEABLE, uSizeData)) != NULL)
		{
			LPVOID pMem;
			if ((pMem = ::GlobalLock(hMem)) != NULL)
			{
				::GetMetaFileBitsEx(hMF, uSizeData, pMem);
#endif // else _MAC
#else // _WIN32
	// GetMetaFileBits() will destroy the metafile under Win16.
	// So we copy it.
	if ((hMF = ::CopyMetaFile(pPict->hMF, NULL)) != NULL)
	{
		HGLOBAL hMem;
		if ((hMem = ::GetMetaFileBits(hMF)) == NULL)
		{
			::DeleteMetaFile(hMF);
		}
		else
		{
			LPVOID pMem;
			if ((pMem = ::GlobalLock(hMem)) != NULL)
			{
#endif
				DataDevice.AddPiece(pMem, ::GlobalSize(hMem));

			/*
			// Build the graphic creation structure.
			// We want to embed source so it will use our little memory device.
			*/

				GRAPHIC_CREATE_STRUCT gcs;

				gcs.m_csFileName = pName;
				gcs.embed_source = TRUE;
				gcs.pSourceDevice = &DataDevice;

			/* Create the graphic. */

				if ((pObject = pDatabase->create_graphic_object(&gcs)) == NULL)
				{
					error = pDatabase->last_creation_error();
				}
				else
				{
				/* Success! */
					error = ERRORCODE_None;
				}
				::GlobalUnlock(hMem);
			}
			::GlobalFree(hMem);
		}
	}

	return error;
}

//
// Create a WMF file from the passed MetaFilePict.
//

ERRORCODE CreateWMFFromMetaFilePict(const METAFILEPICT* pPict, LPCSTR pFileName)
{
	ERRORCODE error = ERRORCODE_Memory;
	MemoryDevice DataDevice;

	// Save the header.
	ALDUS_WMF_HEADER Header;
	FillOutAldusHeader(Header, pPict);
	DataDevice.AddPiece(&Header, sizeof(Header));

	// Save the WMF data.
	HMETAFILE hMF;
	UINT uSizeData;
	if ((hMF = pPict->hMF) != NULL
		 && (uSizeData = ::GetMetaFileBitsEx(hMF, 0, NULL)) != 0)
	{
		HGLOBAL hMem;
		if ((hMem = ::GlobalAlloc(GMEM_MOVEABLE, uSizeData)) != NULL)
		{
			LPVOID pMem;
			if ((pMem = ::GlobalLock(hMem)) != NULL)
			{
				::GetMetaFileBitsEx(hMF, uSizeData, pMem);
				DataDevice.AddPiece(pMem, uSizeData);

				StorageFile Dest(pFileName);
				if ((error = copy_file(&DataDevice, &Dest)) != ERRORCODE_None)
				{
               Dest.flush();
               Dest.zap();
            }

				::GlobalUnlock(hMem);
			}
			::GlobalFree(hMem);
		}
	}
	return error;
}

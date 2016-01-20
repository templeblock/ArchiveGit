//
// TFENGINE.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
//		Interface to TrueType Font Engine
//
// $Header: /PM8/App/TFENGINE.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TFENGINE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:14   Fred
// Initial revision.
// 
//    Rev 1.8   15 May 1997 16:54:50   Fred
// New text effects
// 
//    Rev 1.7   17 Apr 1997 15:35:22   Fred
// Rotated fill patterns
// 
//    Rev 1.6   03 Apr 1997 09:32:42   Fred
// More text cleanup
// 
//    Rev 1.5   26 Mar 1997 15:18:50   Fred
// Removed old geronimo stuff
// 
//    Rev 1.4   07 Feb 1997 09:01:42   Fred
// Added ability to offset curve in MakePath()
// 
//    Rev 1.3   24 Jan 1997 15:00:38   Fred
// Removed some Geronimo conventions including changing G_FRACTION to a long
// 
//    Rev 1.2   13 Jan 1997 15:26:20   Jay
// 16-bit now compiles.
// 
//    Rev 1.1   06 Jan 1997 10:16:36   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:42:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:00   FRED
// Initial revision.
// 
//    Rev 2.6   04 Oct 1995 10:56:32   FRED
// 32 bit TrueType engine
// 
//    Rev 2.5   20 Sep 1995 10:01:14   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 2.4   28 Apr 1995 13:49:12   FRED
//  
// 
//    Rev 2.3   27 Apr 1995 13:20:26   FRED
// 
//    Rev 2.2   25 Apr 1995 10:34:40   FRED
// 
//    Rev 2.1   15 Mar 1995 10:40:28   JAY
// Support for new text editing
// 
//    Rev 2.0   07 Feb 1995 15:49:04   JAY
// Initial revision.
// 
//    Rev 1.7   20 Jun 1994 13:19:44   JAY
// Made ClearLastFace public.
// 
//    Rev 1.6   01 Jun 1994 16:43:34   JAY
// Added parameter to TypefaceCallback.
// Created tfcallb.h to get rid of duplicate declarations.
// Got rid of "0x8000" and "0x8002" special checks since they are totally
// incompatible with the current typeface numbering scheme.
// 
//    Rev 1.5   11 May 1994 16:33:04   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.4   16 Mar 1994 16:01:42   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.3   04 Mar 1994 11:53:28   MACDUFF
// New DoGetFontData method allows reading of TT files either via
// file I/O or by the Windows GetFontData API.
// 
//    Rev 1.2   01 Feb 1994 10:19:58   JAY
// Increased the CONTEXT size.
// 
//    Rev 1.1   30 Jan 1994 09:49:48   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:16   JAY
//  
// 
//    Rev 1.1   25 Apr 1993 20:33:24   MACDUFF
// Support for min_pixel_height().
// 
//    Rev 1.0   04 Mar 1993 15:16:40   MACDUFF
// Initial revision.
//

#ifndef __TFENGINE_H__
#define __TFENGINE_H__

#include "geronimo.h"
#include "ttdll.h"

class COutlinePath;

// The Font Engine works with typeface numbers. A callback function is provided
// to return a LOGFONT or file for accessing the font data.
typedef int (*GetFontCallback)(int, LPLOGFONT, StorageFilePtr);
const int GFC_RESULT_failure = 0;
const int GFC_RESULT_file = 1;
const int GFC_RESULT_logfont = 2;


// CFontFaceInformation class
//
// This class contains the information returned by the TrueType
// get_face_information call.

const int TT_TYPEFACE_FULL_NAME_LENGTH	= 81;
const int TT_TYPEFACE_FAMILY_NAME_LENGTH = 40;
const int TT_TYPEFACE_STYLE_NAME_LENGTH = TT_TYPEFACE_FULL_NAME_LENGTH-TT_TYPEFACE_FAMILY_NAME_LENGTH-1;

typedef char TT_TYPEFACE_FULL_NAME[TT_TYPEFACE_FULL_NAME_LENGTH];
typedef char TT_TYPEFACE_FAMILY_NAME[TT_TYPEFACE_FAMILY_NAME_LENGTH];
typedef char TT_TYPEFACE_STYLE_NAME[TT_TYPEFACE_STYLE_NAME_LENGTH];

class CFontFaceInformation
{
public:
	void* GetDataPtr()
		{ return &style; }

  	virtual char* get_full_name(void);
	virtual char* get_family_name(void);
	virtual char* get_style_name(void);

	uint16 get_min_pixel_height(void);

	G_STYLE style;
	G_WEIGHT stroke_weight;
	G_SPACING spacing_type;
	unsigned short typeface_number;
	G_CLASS typeface_class;
	unsigned short family_number;
	unsigned short default_symbol_set;
	unsigned short hp_face;

	unsigned short windows_fsSelection;
	unsigned short windows_usWeightClass;

	TT_TYPEFACE_FULL_NAME full_name;
	TT_TYPEFACE_FAMILY_NAME family_name;
	TT_TYPEFACE_STYLE_NAME style_name;

	unsigned char is_windows_font;
	unsigned char vendor_id[1+4];
	uint16 is_text_font;
	uint16 lowestRecPPEM;
};


// CTrueTypeCharacterOutlineInformation class
//
//		Information about a TrueType character outline
//		is collected into this structure.

class CFontCharacterOutlineInformation
{
	friend class CFontEngine;
public:
	CFontCharacterOutlineInformation();
	virtual ~CFontCharacterOutlineInformation();
public:
	void FreeMemory(void);
	virtual BOOL MakePath(COutlinePath* pPath, long lXOffset = 0, long lYOffset = 0);
	virtual BOOL FromPath(COutlinePath* pPath, long lXOffset = 0, long lYOffset = 0);
protected:
	BOOL m_fOutlinesExist;
	WORD m_nNumberOfContours;
	short* m_pStart;
	short* m_pEnd;
	long* m_pX;
	long* m_pY;
	unsigned char* m_pOnCurve;
	BOOL m_fAllocatedMemory;
public:
	short m_fUseNewMetrics;
	unsigned long m_dwCellHeight;
	unsigned long m_dwCellWidth;
	long m_lCellAscent;
	long m_lCharX0;
	long m_lCharX1;
	long m_lCharY0;
	long m_lCharY1;
protected:
	long TrueTypeToFixed(long TrueType)
		{ return TrueType << (16-6); }
	long FixedToTrueType(long Fixed)
		{ return (Fixed+(1<<(16-7))) >> (16-6); }
};


//
// Definitions for the table cache.  The cache holds up to three tables --
// CMAP, HMTX, and LOCA -- that are used frequently by the font scaler.
//

class CFontTableCacheItem
{
	uint32 file_position;
	VOIDPTR table_buffer;

	friend class CFontEngine;
};

const int number_of_tables_to_cache = 3;

// CFontEngine class
//
// This is the font engine for TrueType fonts.

#ifdef WIN32
extern "C" void* __stdcall GetFontFragment(long dwClientID, long dwPosition, long dwBytesToRead);
extern "C" void __stdcall ReleaseFontFragment(void* pData);
#else
extern "C" void* pascal _loadds GetFontFragment(long dwClientID, long dwPosition, long dwBytesToRead);
extern "C" void pascal _loadds ReleaseFontFragment(void* pData);
#endif

class CFontEngine
{
private:
	TrueTypeDLL	dll;

	// Handles for use with GetFontData()
	HDC m_hdcScreen;
	HFONT m_hfontCurrent;
	HFONT m_hfontPrevious;

	// Underline position in units per Em.  -1 means not yet determined.

	int m_nEmUnderlinePosition;

	int GetEmUnderlinePosition() const
		{  return m_nEmUnderlinePosition; }
	BOOL UnderlinePositionDefined() const
		{ return (GetEmUnderlinePosition() != -1); }
	void InvalidateUnderlinePosition()
		{ m_nEmUnderlinePosition = -1; }
	void SetEmUnderlinePosition(int nUnderlinePosition)
		{ m_nEmUnderlinePosition = nUnderlinePosition; }

	// Underline thickness in units per Em.  -1 means not yet determined.

	int m_nEmUnderlineThickness;

	int GetEmUnderlineThickness() const
		{  return m_nEmUnderlineThickness; }
	BOOL UnderlineThicknessDefined() const
		{ return (GetEmUnderlineThickness() != -1); }
	void InvalidateUnderlineThickness()
		{ m_nEmUnderlineThickness = -1; }
	void SetEmUnderlineThickness(int nUnderlineThickness)
		{ m_nEmUnderlineThickness = nUnderlineThickness; }

public:
/*
// Constructor.
*/

	CFontEngine(GetFontCallback pCallback);

	virtual ~CFontEngine();

	BOOL Initialize(LPCSTR pszDllPath);

	virtual G_RESULT open_context(void);
	virtual G_RESULT close_context(void);
	virtual G_RESULT make_bitmap(G_BITMAP_PARAMETERS* bitmap_parameters);
	virtual G_RESULT face_information(unsigned short typeface_number, CFontFaceInformation* face_information);
	virtual G_RESULT font_metrics(G_FONT_DESCRIPTION* font_description);
	virtual G_RESULT character_metrics(unsigned short character, G_FONT_DESCRIPTION* font_description);
	virtual G_RESULT character_outline(unsigned short character, G_FONT_DESCRIPTION* font_description, CFontCharacterOutlineInformation* pInformation);

	void ClearLastFace(void);

	pfs_GlyphInputType FSIn;
	pfs_GlyphInfoType FSOut;
	pTransMatrix aTransformMatrix;

	uint16 DoGetFontFragment(uint32 Position, uint16 BytesToRead, void far *Address);

	fs_GlyphInputType FSInput;
	fs_GlyphInfoType FSOutput;
	transMatrix TransformationMatrix;

protected:
	void FontPrologue(uint16 TypefaceID);
	void SetFace(int typeface);
	void SetupForCharacter(uint16 Character, void* pOutline, uint16 BitmapMetrics);
	void InvalidateTransformation(void);
	uint16 TransformationInvalid(void);
	void RememberTransformation(void);
	void IdentityMatrix(pTransMatrix);
	void TTName(tTTNameIndex TTNameID, VOIDPTR TTNamePtr, LPSTR result, int16 maxlen);
	void MapToMacintosh(void);
	DWORD DoGetFontData(DWORD dwOffset, LPVOID lpvBuffer, DWORD dwBytes);

	// make_bitmap support
	void MakeBitmap_ExtractValues(void);
	void MakeBitmap_ComputeRowCount(void);
	void MakeBitmap_ScanBand(void);
	G_RESULT SizeBitmap(void);

	GetFontCallback pGetFontCallback;
	ReadOnlyFile FontFile;
	int CurrentTypeface;
	int CurrentPlatformID;
	int CurrentSpecificID;
	G_RESULT Result;
	transMatrix OldTransMatrix;
	struct _tNewTrans OldNewTrans;
	G_FONT_DESCRIPTION* FontDescription;
	G_BITMAP_PARAMETERS BitmapParameters;
	int16 MakeBitmap_LastRow;
	uint16 CharacterHeight;
	uint16 CharacterWidth;
	uint16 CharacterByteWidth;
	uint16 CharacterSize;
	CFontTableCacheItem TableCache[number_of_tables_to_cache];
	BOOL TableCacheValid;

};

/*
// Global initialize and deinitialize routines.
*/

extern BOOL InitializeFontEngine(GetFontCallback pCallback, LPCSTR pszDllPath);
extern void DeinitializeFontEngine(void);

/* 
// Global TrueType engine managed by the preceding routines.
*/

extern CFontEngine* pFontEngine;

#endif					// #ifndef __TFENGINE_H__

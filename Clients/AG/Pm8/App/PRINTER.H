/*
// Printer information.
//
// $Header: /PM8/App/PRINTER.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/PRINTER.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 2     7/16/98 1:45p Psasse
// Added Advanced Printing Functionality "Print As Bitmap"
// 
//    Rev 1.0   14 Aug 1997 15:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:28   Fred
// Initial revision.
// 
//    Rev 1.10   01 Jul 1997 17:11:14   johno
// Added GetPrinterCaps
// 
//    Rev 1.9   05 Jun 1997 16:47:44   Jay
// New PostScript type check and handling
// 
//    Rev 1.8   08 Apr 1997 16:11:20   Jay
// Print Instructions control
// 
//    Rev 1.7   23 Sep 1996 12:55:14   Jay
// Reversed printout order for DownDown case.
// 
//    Rev 1.6   23 Sep 1996 10:42:16   Jay
// Reversed printout fixes.
// 
//    Rev 1.5   22 Aug 1996 16:58:46   Jay
// Fixed the definition for DBLPRN_DownUp.
// 
//    Rev 1.4   07 Aug 1996 08:51:38   Jay
//  
// 
//    Rev 1.3   06 Aug 1996 14:37:32   Jay
//  
// 
//    Rev 1.2   06 Aug 1996 11:17:42   Jay
//  
// 
//    Rev 1.1   11 Apr 1996 12:29:32   FRED
// Copied from PMW2
// 
//    Rev 1.9   03 Apr 1996 09:06:16   FRED
// Fixes for DeskJet envelope printing
// 
//    Rev 1.8   24 Jan 1996 12:56:16   JAY
// New DeviceCapabilities handling for WIN32. Added m_csPrinterName.
// 
//    Rev 1.7   20 Nov 1995 11:18:08   JAY
// Nre reverse landscape check
// 
//    Rev 1.6   17 Nov 1995 18:05:44   FRED
// Updated to versions from L:\PMW2.16
// 
//    Rev 1.5   09 Oct 1995 15:15:54   JAY
// New direct (fast) bitmap printing
// 
//    Rev 1.4   06 Oct 1995 11:59:44   JAY
// New print code, etc.
// 
//    Rev 1.2   28 Aug 1995 20:32:38   FRED
// Print alignment
// 
//    Rev 1.1   08 Aug 1995 11:01:38   JAY
// Envelope stuff.
// 
//    Rev 1.0   28 Jul 1995 12:29:04   JAY
// Initial revision.
*/

#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "cfixed.h"

/*
// Envelope positions.
*/

enum
{
	ENVELOPE_Tall		= 0,
	ENVELOPE_Wide		= 1,
	ENVELOPE_OMASK		= (ENVELOPE_Tall | ENVELOPE_Wide),

	ENVELOPE_Left		= 0,
	ENVELOPE_Center	= 2,
	ENVELOPE_Right		= 4,
	ENVELOPE_HMASK		= (ENVELOPE_Left | ENVELOPE_Center | ENVELOPE_Right),

	ENVELOPE_VNormal	= 0,
	ENVELOPE_VReverse	= 8,
	ENVELOPE_VMASK		= (ENVELOPE_VNormal | ENVELOPE_VReverse),

	ENVELOPE_FlapUp	= 0,
	ENVELOPE_FlapDown	= 16,
	ENVELOPE_FMASK		= (ENVELOPE_FlapUp | ENVELOPE_FlapDown),

	ENVELOPE_RNormal	= 0,
	ENVELOPE_RRotate	= 32,
	ENVELOPE_RMASK		= (ENVELOPE_RNormal | ENVELOPE_RRotate),

	ENVELOPE_NoValue	= 64
};

/*
// Double-sided printing bit fields.
*/

enum
{
	DBLPRN_Valid		= 1,

	// Phase 1 - direction
	DBLPRN_FrontForward	= 0,
	DBLPRN_FrontBackward	= 2,
	DBLPRN_FRONTDIRMASK	= (DBLPRN_FrontForward | DBLPRN_FrontBackward),

	// Phase 1 - key sheet
	DBLPRN_FrontKeyFirst	= 0,
	DBLPRN_FrontKeyLast	= 4,
	DBLPRN_FRONTKEYMASK	= (DBLPRN_FrontKeyFirst | DBLPRN_FrontKeyLast),

	// User - stack face up or down
	DBLPRN_StackUp		= 0,
	DBLPRN_StackDown	= 8,
	DBLPRN_STACKMASK	= (DBLPRN_StackUp | DBLPRN_StackDown),

	// Phase 2 - direction
	DBLPRN_BackForward	= 0,
	DBLPRN_BackBackward	= 16,
	DBLPRN_BACKDIRMASK	= (DBLPRN_BackForward | DBLPRN_BackBackward),

	// Phase 2 - key sheet
	DBLPRN_BackKeyFirst	= 0,
	DBLPRN_BackKeyLast	= 32,
	DBLPRN_BACKKEYMASK	= (DBLPRN_BackKeyFirst | DBLPRN_BackKeyLast),

	// Overrides
	DBLPRN_PrintOnBlank	= 64,

	// Reversed state (LaserJet 4L, etc.)
	DBLPRN_NotReversed	= 0,
	DBLPRN_Reversed		= 128,
	DBLPRN_REVERSEDMASK	= (DBLPRN_NotReversed | DBLPRN_Reversed),
};

// The printer cases we know of.

enum
{
	// Normal
	DBLPRN_UpDown = DBLPRN_FrontForward | DBLPRN_FrontKeyFirst | DBLPRN_StackDown | DBLPRN_BackBackward | DBLPRN_BackKeyLast,
	DBLPRN_DownUp = DBLPRN_FrontForward | DBLPRN_FrontKeyLast | DBLPRN_StackUp | DBLPRN_BackBackward | DBLPRN_BackKeyFirst,
	DBLPRN_UpUp = DBLPRN_FrontForward | DBLPRN_FrontKeyLast | DBLPRN_StackDown | DBLPRN_BackForward | DBLPRN_BackKeyLast,
	DBLPRN_DownDown = DBLPRN_FrontBackward | DBLPRN_FrontKeyFirst | DBLPRN_StackUp | DBLPRN_BackBackward | DBLPRN_BackKeyFirst,
	// Reversed
	DBLPRN_UpDownReversed = DBLPRN_FrontForward | DBLPRN_FrontKeyFirst | DBLPRN_StackDown | DBLPRN_BackBackward | DBLPRN_BackKeyLast | DBLPRN_Reversed,
	DBLPRN_DownUpReversed = DBLPRN_FrontForward | DBLPRN_FrontKeyLast | DBLPRN_StackUp | DBLPRN_BackBackward | DBLPRN_BackKeyFirst | DBLPRN_Reversed,
	DBLPRN_UpUpReversed = DBLPRN_FrontForward | DBLPRN_FrontKeyLast | DBLPRN_StackDown | DBLPRN_BackForward | DBLPRN_BackKeyLast | DBLPRN_Reversed,
	DBLPRN_DownDownReversed = DBLPRN_FrontBackward | DBLPRN_FrontKeyFirst | DBLPRN_StackUp | DBLPRN_BackBackward | DBLPRN_BackKeyFirst | DBLPRN_Reversed
};

#define DEFAULT_OUTLINE_GAMMA			1000
#define DEFAULT_BITMAP_GAMMA			400
#define DEFAULT_FAST_BITMAP_GAMMA	1000
#define DEFAULT_ENVELOPE_POSITION	ENVELOPE_NoValue
#define DEFAULT_PRINT_INSTRUCTIONS	TRUE

class CPrinterInfo
{
public:
	CPrinterInfo();

	/*
	// Set values to default.
	*/

	void Defaults(void);

	/*
	// Set the name of the INI file used by the printer info.
	*/

	void SetIniName(LPCSTR pName);

	/*
	// Update our information from the device names.
	// If this is not available, use the WIN.INI setting.
	*/

	void UpdateFromDevNames(HGLOBAL hDevNames, HGLOBAL hDevMode, HDC hDC, int nOrientation, BOOL fUseWinIni = TRUE);

	/*
	// Get the names for the current printer.
	*/

	void GetNames(HGLOBAL hDevNames, BOOL fUseWinIni = TRUE);

	/*
	// Get the string to use for an INI entry.
	*/

	BOOL GetSectionName(CString& csName);

	/*
	// Get the driver name (without the ".DRV").
	*/

	const CString& GetDriverName(void) const
		{ return m_csDriverName; }

	/*
	// Read and write settings.
	*/

	void ReadSettings(void);
	void WriteSettings(void);

	/*
	// Access to individual settings.
	*/

	BOOL WriteProfileString(LPCSTR pEntry, LPCSTR pString);
	
	UINT GetProfileInt(LPCSTR pEntry, UINT wDefault);
	BOOL WriteProfileInt(LPCSTR pEntry, UINT wValue);
	BOOL WriteProfileInt(LPCSTR pEntry, UINT wValue, UINT wDefault);
	
	CFixed GetProfileFixed(LPCSTR pEntry, CFixed cfDefault);
	BOOL WriteProfileFixed(LPCSTR pEntry, CFixed cfValue);
	BOOL WriteProfileFixed(LPCSTR pEntry, CFixed cfValue, CFixed cfDefault);
	
	BOOL RemoveEntry(LPCSTR pEntry);

	/*
	// Printer variable access.
	//
	// OutlineGamma	- gamma value for outline graphics
	// BitmapGamma		- gamma value for bitmap graphics
	// PostScriptType	- PostScript type
	//
	// Margin/Offset Overrides
	//
	//		LeftMargin
	//		TopMargin
	//		RightMargin
	//		LeftMargin
	//		LeftOffset
	//		TopOffset
	*/

	int GetOutlineGamma(void) const;
	void SetOutlineGamma(int nGamma);
	int GetBitmapGamma(void) const;
	void SetBitmapGamma(int nGamma);
	int GetPostScriptType(void) const;
	void SetPostScriptType(int nType);
	CFixed GetLeftMargin(void) const;
	void SetLeftMargin(CFixed cfLeftMargin);
	CFixed GetTopMargin(void) const;
	void SetTopMargin(CFixed cfTopMargin);
	CFixed GetRightMargin(void) const;
	void SetRightMargin(CFixed cfRightMargin);
	CFixed GetBottomMargin(void) const;
	void SetBottomMargin(CFixed cfBottomMargin);
	CFixed GetLeftOffset(void) const;
	void SetLeftOffset(CFixed cfLeftOffset);
	CFixed GetTopOffset(void) const;
	void SetTopOffset(CFixed cfTopOffset);
	BOOL GetPrintAsBitmap(void) const;
	void SetPrintAsBitmap(BOOL fSet);

	/*
	// Get and set the envelope position.
	*/

	WORD GetEnvelopePosition(void) const
		{ return m_Record.m_wEnvelopePosition; }
	void SetEnvelopePosition(WORD wEnvelopePosition)
		{ m_Record.m_wEnvelopePosition = wEnvelopePosition; }

	WORD GetDefaultEnvelopePosition(void) const
		{ return m_Record.m_wDefaultEnvelopePosition; }
	void SetDefaultEnvelopePosition(WORD wPosition)
		{ m_Record.m_wDefaultEnvelopePosition = wPosition; }

	// Double-sided configuration.
	WORD GetDoubleSidedType(void) const
		{ return m_Record.m_wDoubleSidedType; }
	void SetDoubleSidedType(WORD wDoubleSidedType)
		{ m_Record.m_wDoubleSidedType = wDoubleSidedType; }

	// Do we print double-sided instructions?
	BOOL GetPrintInstructions(void) const
		{ return m_Record.m_fPrintInstructions; }
	void SetPrintInstructions(BOOL fPrintInstructions)
		{ m_Record.m_fPrintInstructions = fPrintInstructions; }

	/*
	// More access.
	*/

	int Copies(void) const
		{ return m_Record.m_nCopies; }
	int Orientation(void) const						// 90 or 270 for landscape orientation
		{ return m_Record.m_nOrientation; }
	int GetSafeOrientation(void);						// Resolve unknown case to 90 or 270
	int Tractor(void) const
		{ return m_Record.m_nTractor; }
		
	const CFixedRect& Margins(void)
		{ return m_Record.m_Margins; }
	void GetOrientedMargins(CFixedRect* pRect);
	void Margins(const CFixedRect& frMargins)
		{ m_Record.m_Margins = frMargins; }
	const CFixedPoint& OriginOffset(void)
		{ return m_Record.m_OriginOffset; }
	void Offset(const CFixedPoint& fpOffset)
		{ m_Record.m_OriginOffset = fpOffset; }

	const CFixedRect&	PhysicalMargins(void)
		{ return m_PhysicalMargins; }
	const CFixedPoint& PaperSize(void)
		{ return m_PaperSize; }
	const CFixedPoint& PrintableSize(void)
		{ return m_PrintableSize; }
	void SetPrintableSize(const CFixedPoint& pt)
		{ m_PrintableSize = pt; }
		
	PAPER_TYPE PaperType(void)
		{ return m_nPaperType; }

	short DevModePaperSize(void) const
		{ return m_nDevModePaperSize; }
	BOOL IsEnvelopePaperSize(void) const;
	BOOL ReverseLandscape(void) const
		{ return m_Record.m_fReverseLandscape; }
	void ReverseLandscape(BOOL fReverseLandscape)
		{ m_Record.m_fReverseLandscape = fReverseLandscape; }

	BOOL FastBitmaps(void) const
		{ return m_Record.m_fFastBitmaps; }
	void FastBitmaps(BOOL fFastBitmaps)
		{ m_Record.m_fFastBitmaps = fFastBitmaps; UpdateBitmapGamma(); }

	void OrientRect(CFixedRect* pRect, BOOL fUnOrient = FALSE, BOOL fJustDoIt = FALSE);

	BOOL GetDefaultReverseLandscape(void);

	BOOL IsDeskJet(void);

   int   GetPrinterCaps ()
   {
      return m_Record.m_nRasterCaps;
   }

protected:

	void GetDeviceInfo(HGLOBAL hDevMode, HDC hDC);
	BOOL GetDefaultFastBitmaps(void);
	void UpdateBitmapGamma(void);
	int DeterminePostScriptType(HDC hDC);

	struct
	{
		WORD m_wEnvelopePosition;
		WORD m_wDefaultEnvelopePosition;
		int m_nCopies;					// -1 = unknown
		int m_nOrientation;			// Degrees to make landscape; -1 = unknown
		int m_nTractor;				// FALSE = no; TRUE = yes; -1 = unknown
		CFixedRect m_Margins;
		CFixedPoint m_OriginOffset;
		BOOL m_fReverseLandscape;
		BOOL m_fFastBitmaps;
		int m_nBitmapGamma;
		int m_nFastBitmapGamma;
		int m_nNumColors;
		int m_nRasterCaps;
		int m_nBitsPixel;
		int m_nPlanes;
		WORD m_wDoubleSidedType;
		BOOL m_fPrintInstructions;
		BOOL m_fPrintAsBitmap;
	} m_Record;

	CString m_csIniName;

	CString m_csDriverName;
	CString m_csDeviceName;
	CString m_csOutputName;
	// This is the same as csDeviceName under 16 bit.
	// For 32 bit, it is the name the user gave the printer.
	CString m_csPrinterName;

	CFixedRect m_PhysicalMargins;
	CFixedPoint m_PaperSize;
	CFixedPoint m_PrintableSize;

	PAPER_TYPE m_nPaperType;	/* What type of paper are we printing to? */
	int m_nOrientationType;

	short m_nDevModePaperSize;	// From DEVMODE structure: DMPAPER_LETTER, ...
	int m_nDefaultPostScriptType;
};

#endif

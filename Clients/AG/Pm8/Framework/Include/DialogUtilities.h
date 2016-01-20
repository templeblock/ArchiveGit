//****************************************************************************
//
// File Name:  DialogUtilities.h
//
// Project:    Renaissance application framework
//
// Author:     Mike Heydlauf
//
// Description: Contains utility functions prototypes and global defines
//				for the Headline UI
//
// Portability: Windows Specific
//
// Developed by:   Turning Point Software
//				   One Gateway Center, Suite 800
//				   Newton, Ma 02158
//			       (617)332-0202
//
// Client:		   Broderbund
//
// Copyright(C)1995, Turning Point Software, All Rights Reserved.
//
// $Header:: /PM8/Framework $
//
//****************************************************************************

#ifndef _DIALOGUTILITIES_H_
#define _DIALOGUTILITIES_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#ifdef	_WINDOWS

typedef CArray<CString,CString&>	StringArray;
#define kFontArrayGrowSize 10
#define kFontNameLen       32 //based on LOGFONT.lfFaceName length

FrameworkLinkage void MoveChildWindows( CWnd* pParent, int nDeltaX, int nDeltaY, BOOL fRedraw );
extern "C" int CALLBACK MyFontEnumProc(
    const ENUMLOGFONT FAR *lpelf,	// pointer to logical-font data 
    const NEWTEXTMETRIC FAR *lpntm,	// pointer to physical-font data 
    unsigned long FontType,	// type of font 
    LPARAM lParam 	// address of application-defined data  
   );

FrameworkLinkage void GetFontNames( CDC* pDC, StringArray* parFonts );
FrameworkLinkage void SortFontNames( StringArray* parFonts );
FrameworkLinkage void FormatStringForEditCtrl( RMBCString& rString, int& nWordStartPos );
FrameworkLinkage void FormatStringForEditCtrl( RMBCString& rString );
FrameworkLinkage void StripOutNewlineChars( CString& cString );
FrameworkLinkage void ParseAndPutInListBox( RMBCString& rListToParse, CListBox& cListBox );
FrameworkLinkage void ParseAndPutInComboBox( RMBCString& rListToParse, CComboBox& cComboBox );
FrameworkLinkage BOOLEAN ExactStringIsInList( const RMBCString& rListToParse, const RMBCString& rString );
FrameworkLinkage RMBCString GetDefaultFont();
FrameworkLinkage BOOLEAN    SubclassControl( CWnd* pParent, UINT nControlID, CWnd& ctlChild ) ;
FrameworkLinkage void ChangeToDefaultProjectDirectory();
FrameworkLinkage HPALETTE CreateDIBPalette (LPBITMAPINFO lpbmi, LPINT lpiNumColors);
FrameworkLinkage HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
                           HPALETTE FAR* lphPalette);

FrameworkLinkage HBITMAP MagicLoad( LPCTSTR nID );

// ****************************************************************************
//
//  Class:				RIndeterminateCheckBox
//
//  Description:		Supports an indeterminate bitmap for tri-state check boxes
//							with the pushbutton (bitmap) style.
//
// ****************************************************************************
//
class FrameworkLinkage RIndeterminateCheckBox : public CButton
{
private:
	int		m_nState;
	enum		{kUnChecked, kChecked, kIndeterminate};

	CBitmap	m_UpBitmap;
	CBitmap	m_DownBitmap;
	CBitmap	m_IndeterminateBitmap;

	WNDPROC	m_OriginalWndProc;

	static LRESULT WINAPI IndeterminateWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

public:
	RIndeterminateCheckBox();
	virtual ~RIndeterminateCheckBox();
	void SetIndeterminateBitmap( int nUp, int nIndeterimate, int nDown = 0 );
};

#endif	//	_WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_DIALOGUTILITIES_H_

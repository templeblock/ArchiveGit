//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/OwnerDrawMenu.h 1     3/03/99 6:08p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/OwnerDrawMenu.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 1     2/25/99 11:15a Dennis
// 
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef   _POWNERDRAWMENU_INC
#define   _POWNERDRAWMENU_INC

#define   OEMRESOURCE // Allows LoadOEMBitmap

#ifdef  WIN32
  #define CHECKMENUITEMRETURN DWORD
#else
  #define CHECKMENUITEMRETURN BOOL
#endif

//============================================================================
// This is a "special" menu class, it may not support all CMenu functionality,
// or it may require special implementation.
class CPMenuSpecial  : public  CMenu
{
public:
	// Construction
	CPMenuSpecial ();
	~CPMenuSpecial ();

	// Operations
	void				SetPalette (HPALETTE hPalette)
		{	m_hPalette = hPalette; }
	BOOL			   SetFont (CFont &pFont);
	BOOL				SetDefaultCheckMark (UINT ID);

	// Overrides
	virtual void	DrawItem (LPDRAWITEMSTRUCT pDraw);
	virtual void	MeasureItem (LPMEASUREITEMSTRUCT lpMIS);

	/*
	BOOL  
	SizeBitmap (CDC *pDC, CBitmap *pBitmapNew, CBitmap *pBitmapOld);

	BOOL  
	SizeBitmap (CDC *pDC, 
				 CBitmap *pBitmapNew, 
				 CBitmap *pBitmapOld,
				 UINT NewWidth,
				 UINT NewHeight);

	BOOL
	SetDefaultCheckMark (CDC *pDC, CBitmap *check, UINT Width, UINT Height);                        
	*/

	// Public Data Members
	COLORREF  m_lColorText;    
	COLORREF  m_lColorBk;
	COLORREF  m_lColorHiText;    
	COLORREF  m_lColorHiBk;
	COLORREF  m_lColorGrayed;

private:    
	CBitmap   m_bmDefaultCheck;

	HPALETTE  m_hPalette;

	UINT      m_nDefaultTextHeight;
	UINT      m_nDefaultCheckWidth;
	UINT      m_nDefaultCheckHeight;
	UINT      m_nWindowsCheckWidth;

	CFont     m_Font;
};

class CPOwnerDrawMenu
{
public:
	// Construction
	CPOwnerDrawMenu ();
	~CPOwnerDrawMenu ();
 
	// Attribute Operations
	void		SetColorText (COLORREF ct)
		{ m_Menu.m_lColorText = ct; }

	void		SetColorBk (COLORREF bk)
		{ m_Menu.m_lColorBk = bk; }

	void		SetColorHiText (COLORREF htx)
		{ m_Menu.m_lColorHiText = htx; }

	void		SetColorHiBk (COLORREF hbk)
		{ m_Menu.m_lColorHiBk = hbk; }

	void		SetColorGrayed (COLORREF gry)
		{ m_Menu.m_lColorGrayed = gry; }

	void		SetPalette (HPALETTE hPalette)
		{ m_Menu.SetPalette (hPalette); }

	BOOL		SetFont (CFont  &pFont)
		{ return m_Menu.SetFont (pFont);}

	BOOL		SetFont (HFONT hFont);
	
	BOOL		SetMenuItemBitmaps (UINT nPos, UINT nFlags, const CBitmap *uncheck, const CBitmap *check);

	BOOL		SetDefaultCheckMark (UINT ID)
		{ return m_Menu.SetDefaultCheckMark (ID); }

	// Operations
	BOOL		CreatePopupMenu (void)
		{ return m_Menu.CreatePopupMenu (); }

	UINT		EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
		{ return m_Menu.EnableMenuItem(nIDEnableItem, nEnable); }
	
	BOOL		AppendMenu (UINT flag, UINT id = 0, LPCSTR Strg = NULL);

	BOOL		TrackPopupMenu(UINT nFlags, int x, int y, CWnd* pWnd, LPCRECT lpRect = 0)
		{ return m_Menu.TrackPopupMenu (nFlags, x, y, pWnd, lpRect); } 
  
	LRESULT	IsDisAKey (UINT key);

	BOOL		DestroyMenu ();

	CHECKMENUITEMRETURN	CheckMenuItem (UINT id, UINT chk);
private:
	 void KillAllItems (void);

	 CPtrArray     m_Items;
	 CWordArray    m_AccKeys;
	 CPMenuSpecial  m_Menu;
};    

class CPOwnerDrawMenuItem
{
  public:  
    CPOwnerDrawMenuItem();
    
    BOOL          m_bIsSeparator;
    
    CBitmap       *m_pBitmapUnCheck; 
    UINT          m_nUnCheckHeight;
    
    CBitmap       *m_pBitmapCheck; 
    UINT          m_nCheckHeight;
    
    CString       *m_pText;
    UINT          m_nFlag;
  private:    
};

#endif	// end of _POWNERDRAWMENU_INC

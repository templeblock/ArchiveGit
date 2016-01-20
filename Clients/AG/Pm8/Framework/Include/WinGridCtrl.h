//****************************************************************************
//
// File Name:  WinGridCtrl.h
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Definition class for a multi-column listbox.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	              500 Redwood Blvd.
//               Novato, CA 94948
//	              (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/WinGridCtrl.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WINGRIDCTRL_H_
#define _WINGRIDCTRL_H_

#include <afxtempl.h>

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////////////////////////////////////////////////////////////////////////
// RGridCtrlBase window definition

class FrameworkLinkage RGridCtrlBase : public CWnd
{
public:

	enum EStyleType
	{
		kDefault    = 0x0000,
		k3dStyle    = 0x0001,
		kPushLike   = 0x0002,
		kAddSpacing = 0x0004,
		kIntegral   = 0x0008
	} ;
						RGridCtrlBase( UINT uiStyles = kDefault );
	virtual			~RGridCtrlBase() ;

// Operations
public:
						//
						// Initialization
						//
	virtual int		InitStorage( int nItems ) ;

						//
						// General operations
						//
	virtual int		GetCount() const ;
	virtual int		GetTopIndex() const;
	virtual int		SetTopIndex( int nIndex );
	virtual int		GetItemRect( int nIndex, LPRECT lpRect ) const;
	virtual int		GetCellHeight() const;
	virtual int		GetCellWidth() const;
	virtual int		GetNumRows() const { return m_nNumRows; }
	virtual int		GetNumCols() const { return m_nNumCols; }
	virtual int		GetNumVisibleCols() const { return m_cxVisibleCols; }
	virtual int		GetNumVisibleRows() const { return m_cxVisibleRows; }
	virtual int		GetTopCellRow() const { return m_nTopCellRow; }
	virtual int		GetTopCellCol() const { return m_nTopCellCol; }

	virtual UINT	ItemFromPoint( CPoint& pt, BOOL& fOutside ) const;

	virtual int		GetCurSel() const;
	virtual int		SetCurSel( int nSelect );

	virtual void	SetCellDimensions( int cxWidth, int cxHeight ) ;
	virtual void	SetNumCols( int nNumCols ) ;
	virtual void	SizeCellsToClient( int nNumRows, int nNumCols ) ;

	virtual COLORREF GetGridBackgroundColor() const;

	UINT				GetGridStyle() const ;
	void				SetGridStyle( UINT uiStyles ) ;
	void				SetRedraw(BOOL bRedraw = TRUE);
	BOOL				GetRedraw() {return m_bSetRedraw;}

protected:
						//
						// Overridables
						//
	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) = 0 ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) = 0 ;
	virtual void	CharToItem( UINT nKey, UINT nIndex ) ; 
	virtual void	VKeyToItem( UINT nKey, UINT nIndex ) ; 

// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGridCtrlBase)
	protected:
	virtual void	PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(RGridCtrlBase)
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) ;
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg LRESULT OnLbGetSel( UINT wParam, LONG lParam );
	afx_msg LRESULT OnLbGetCount( UINT wParam, LONG lParam );
	afx_msg LRESULT OnLbSetCurSel( UINT wParam, LONG lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	int				CellToIndex( int nRow, int nCol ) const ;
	void				DrawPushLikeCell( LPDRAWITEMSTRUCT lpDrawItemStruct ) const ;
	void				GetCellRect( int nRow, int nCol, LPRECT lpRect ) const ;
	UINT				GetItemState( int nIndex ) const;
	void				SendDrawItem( CDC& dc, UINT uiItemID, UINT uiItemAction ) const ;
	void				UpdateMetrics( ) ;
	void				UpdateScrollBars( ) ;

	void				OnItemAdded( int nIndex ) ;


private:

	WORD	m_cxCellHeight ;
	WORD	m_cxCellWidth ;
	WORD	m_cxVisibleRows ;
	WORD	m_cxVisibleCols ;

	int		m_nCurCell ;
	int		m_nTopCellRow ;
	int		m_nTopCellCol ;

	WORD		m_nNumCols ;
	WORD		m_nNumRows ;

	BOOL		m_fResizeCells ;

	UINT		m_cxItemCount ;
	UINT		m_uiFlags ;

	BOOL		m_bSetRedraw;
};

inline void	RGridCtrlBase::CharToItem( UINT, UINT ) 
{
	NULL ;
}

inline void	RGridCtrlBase::VKeyToItem( UINT, UINT ) 
{
	NULL ;
}

/////////////////////////////////////////////////////////////////////////////
// RGridCtrl window definition

template <class T> 
class RGridCtrl : public RGridCtrlBase
{
public:
						RGridCtrl( UINT uiStyles = kDefault );
	virtual			~RGridCtrl() ;

// Operations
public:

						//
						// Item Operations
						//
	int				AddItem (const T& item ) ;
	int				DeleteItem( UINT nIndex ) ;
	int				FindItem( int nStartAfter, const T& item ) ;
	int				InsertItem( int nIndex, const T& item ) ;
	int				SelectItem( int nStartAfter, T& item );

	void				ResetContent() ;

						//
						// General operations
						//
	BOOL				GetItemData( int nIndex, T& item ) const;
	int				SetItemData( int nIndex, T& item );

						//
						// Overridables
						//
	virtual int		GetCount() const ;
	virtual int		InitStorage( int nItems ) ;

	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) = 0 ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) = 0 ;

protected:

	int				FindSortedItem( int nStartAfter, const T& item, BOOL& fFound ) ;

private:

	CArray<T, T&>	m_itemArray ;

} ;

template <class T> 
class RItemDrawnGridCtrl : public RGridCtrl<T>
{
public:
						RItemDrawnGridCtrl();
	virtual			~RItemDrawnGridCtrl() ;

	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) ;
} ;

#include "WinGridCtrl.inl"

/////////////////////////////////////////////////////////////////////////////

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _WINGRIDCTRL_H_

// CustomizeQSLDlg.h : header file
//
#ifndef __QSLITEMLIST_H__
#define __QSLITEMLIST_H__

#include "WinGridCtrl.h"

//#define USECACHE
#define UM_TOP_INDEX_CHANGED  WM_USER + 100 

class RImage;

struct QSLItemData
{
	CString	m_strFile ;
	CString	m_strDesc ;

#ifndef USECACHE
	RImage* m_pImage;
#endif

						QSLItemData() ;
	virtual				~QSLItemData() ;

	QSLItemData&		operator=( const QSLItemData& rhs );

	BOOL				operator>(const QSLItemData& rhs) const
						{
							return (m_strDesc.Compare( rhs.m_strDesc ) > 0);
						}

	BOOL				operator==(const QSLItemData& rhs) const
						{
							return (m_strDesc.Compare( rhs.m_strDesc ) == 0);
						}

	RImage*				GetImage();
	void				SetPathName( CString& strDocPathname );

protected:

	RImage*				Load( CString& strDocPathname, BOOL fDescOnly = FALSE );
	RImage*				Load2(	LPSTORAGE pIStorage, BOOL fDescOnly );
};

typedef RGridCtrl<QSLItemData> CQSLItemDataGrid;
class CQSLItemList : public CQSLItemDataGrid 
{
// Construction/Destruction
public:
					CQSLItemList();
	virtual			~CQSLItemList();

// Operations
public:

	void			EmptyCache() ;
	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;
	virtual void	MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct ) ;
	virtual int		SetTopIndex( int nIndex );

	// Generated message map functions
protected:

#ifdef USECACHE
	struct CBCachedItem
	{
		CBCachedItem() ;
		~CBCachedItem() ;

		int			m_nIndex;
		clock_t		m_nLastUsed;
		RImage*		m_pImage;
	} ;

	RImage*			GetCachedBitmap( int nIndex ) ;
#endif // USECACHE

	//{{AFX_MSG(CBMultiColListBox)
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

#ifdef USECACHE
	CArray<CBCachedItem, CBCachedItem&>	m_arrItemCache ;
#endif

};

#endif // __QSLITEMLIST_H__

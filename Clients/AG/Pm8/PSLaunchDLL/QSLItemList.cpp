// CustomizeQSLDlg.cpp : implementation file
//

#include "stdafx.h"
ASSERTNAME

#include "afxconv.h"
#include "QSLItemList.h"

// Framework support
#include "BitmapImage.h"
#include "ImageLibrary.h"
#include "MemoryMappedBuffer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int kMaxCacheSize = 8 ;
const int kBufLength = 256;

/////////////////////////////////////////////////////////////////////////////
// QSLItemData
						
QSLItemData::QSLItemData()
{
#ifndef USECACHE
	m_pImage = NULL;
#endif
}

QSLItemData::~QSLItemData()
{
	// Note: the owner of the list is responsible
	// for deleting the image.  This is because
	// when the item is copied, only the pointer
	// to the image is copied not the entire 
	// image.
}

QSLItemData& QSLItemData::operator=( const QSLItemData& rhs )
{
	m_strFile = rhs.m_strFile ;
	m_strDesc = rhs.m_strDesc ;

#ifndef USECACHE
	m_pImage = rhs.m_pImage ;
#endif

	return *this ;
}

void QSLItemData::SetPathName( CString& strDocPathname )
{
	m_strFile = strDocPathname;
	Load( m_strFile, TRUE );
}

RImage*	QSLItemData::GetImage()
{
#ifndef USECACHE
	if (m_pImage)
		return m_pImage;
#endif

	return Load( m_strFile );
}

RImage* QSLItemData::Load(
	CString& strDocPathname,		// @parm string containing document pathname
	BOOL     fDescOnly )
{
    USES_CONVERSION;

	LPSTORAGE	pIStorage = NULL;

	// get path name in Unicode
	const WCHAR *pDocPathname = T2COLE((LPCTSTR)strDocPathname.GetBuffer (10));
	strDocPathname.ReleaseBuffer();

	// open storage
	StgOpenStorage( pDocPathname, NULL, STGM_DIRECT | STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pIStorage );

	if (!pIStorage)
		return NULL ;

	RImage* pImage = Load2( pIStorage, fDescOnly );
	
	if (dynamic_cast<RBitmapImage *>( pImage ))
		((RBitmapImage *) pImage)->LockImage();

	// release storage
	pIStorage->Release();

	return( pImage );
}

RImage* QSLItemData::Load2(
	LPSTORAGE	pIStorage,			// @parm storage
	BOOL        fDescOnly )
{
	RImage* pImage = NULL;
	char	strDescription[kBufLength];

	TRY
	{
		COleStreamFile  str;

		if (!fDescOnly)
		{
			// open stream
			CString  szStream = "PREVIEW";

			if (str.OpenStream (pIStorage, szStream, CFile::modeRead | CFile::shareExclusive))
			{
				// Read in the image data
				RTempFileBuffer	buffer;
				buffer.Resize( str.GetLength() );
				str.Read( buffer.GetBuffer(), str.GetLength() );
				str.Close();

				// Create the image
				pImage = RImageLibrary().ImportImage( buffer );
			}
		}

		if (fDescOnly)
		{
			// open stream
			CString szStream = "DESCRIPTION";

			if (str.OpenStream (pIStorage, szStream, CFile::modeRead | CFile::shareExclusive))
			{
				str.Read (&strDescription, min( kBufLength, str.GetLength() ));
				str.Close();
			}

			// return bitmap handle and description string
			m_strDesc = strDescription;
		}

	} // End try

	CATCH(CArchiveException, archExcept)
	{
		//CBExceptionReporter::ArchiveException(archExcept);
	}
	END_CATCH


	return pImage;
}

/////////////////////////////////////////////////////////////////////////////
// CQSLItemList 

#ifdef USECACHE
CQSLItemList::CBCachedItem::CBCachedItem() 
{ 
	m_nIndex = -1 ;
	m_pImage = NULL ;
}

CQSLItemList::CBCachedItem::~CBCachedItem() 
{ 
	// Note: the owner of the list is responsible
	// for deleting the image.  This is because
	// when the item is copied, only the pointer
	// to the image is copied not the entire 
	// image.
}
#endif

CQSLItemList::CQSLItemList()
	: CQSLItemDataGrid( kAddSpacing | kIntegral )
{
}

CQSLItemList::~CQSLItemList()
{
//#ifndef USECACHE
	EmptyCache();
//#endif
}


BEGIN_MESSAGE_MAP(CQSLItemList, RGridCtrlBase)
	//{{AFX_MSG_MAP(CQSLItemList)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CQSLItemList::EmptyCache()
{
#ifdef USECACHE
	m_arrItemCache.RemoveAll() ;
#else
	QSLItemData itemData;

	for (int i = 0; i < GetCount(); i++)
	{
		GetItemData( i, itemData );

		if (itemData.m_pImage)
		{
			itemData.m_pImage->UnlockImage();
			delete itemData.m_pImage;
			itemData.m_pImage = NULL;
		}
	}
#endif
}

#ifdef USECACHE
RImage* CQSLItemList::GetCachedBitmap( int nIndex )
{
	CBCachedItem* pItem = NULL ;

	for (int i = 0; i < m_arrItemCache.GetSize(); i++)
	{
		if (m_arrItemCache[i].m_nIndex == nIndex)
		{
			pItem = &m_arrItemCache[i] ;
			break ;
		}
	}

	if (!pItem)
	{
		if (m_arrItemCache.GetSize() >= kMaxCacheSize)
		{
			// Find the oldest item
			pItem = &(m_arrItemCache[0]) ;

			for (int i = 1; i < m_arrItemCache.GetSize(); i++)
			{
				CBCachedItem& item = m_arrItemCache[i] ;

				if (pItem->m_nLastUsed > item.m_nLastUsed)
					pItem = &item ;
			}

			ASSERT( pItem ) ;
			if (pItem->m_pImage)
			{
				delete pItem->m_pImage;
				pItem->m_pImage = NULL;
			}
		}
		else
		{
			// Add a new item
			CBCachedItem item ;
			m_arrItemCache.Add( item ) ;
			pItem = &(m_arrItemCache[m_arrItemCache.GetSize() - 1]) ;
		}

		QSLItemData itemData;
		GetItemData( nIndex, itemData );
		pItem->m_pImage = itemData.GetImage();
		pItem->m_nIndex = nIndex ;
	}

	ASSERT( pItem ) ;
	pItem->m_nLastUsed = clock() ;

	return pItem->m_pImage;
}
#endif // USECACHE

//@mFunc draws the listbox items in their proper state
void CQSLItemList::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	QSLItemData itemData;
	GetItemData( lpDrawItemStruct->itemID, itemData );
	
#ifdef USECACHE
	RImage* pImage = GetCachedBitmap( lpDrawItemStruct->itemID ) ;
#else
	RImage* pImage = itemData.GetImage();

	if (!itemData.m_pImage)
	{
		itemData.m_pImage = pImage;
		SetItemData( lpDrawItemStruct->itemID, itemData );
	}

//	HBITMAP hBitmap = itemData.m_hBitmap ;
//	HPALETTE hPalette = itemData.m_hPalette ;
	
//	if (!itemData.m_pImage)
//	{
//		hBitmap = itemData.m_hBitmap = 
//			LoadBitmapPreview( lpDrawItemStruct->itemID, &itemData.m_hPalette ) ;
//		hPalette = itemData.m_hPalette;
//
//		// Save the new data into the control
//		SetItemData( lpDrawItemStruct->itemID, itemData );
//	}
//
//	// If we have a palette with the bitmap, realize it into the DC prior to painting.
//	if (hPalette != NULL)
//	{
//		CPalette palBitmap;
//		palBitmap.Attach( hPalette );
//		CPalette *pOldPal = pDC->SelectPalette( &palBitmap, TRUE );
//		pDC->RealizePalette();
//		pDC->SelectPalette( pOldPal, TRUE );
//		palBitmap.Detach();
//	}
#endif

	BITMAP  bm= { 0, 0, 0, 0, 0, 0, NULL };
	HBITMAP hBitmap = NULL;
	
	if (pImage)
	{
		hBitmap = (HBITMAP) pImage->GetSystemHandle();
		::GetObject( hBitmap, sizeof( bm ), &bm );
	}

	// Determine colors to use for drawing text and selection
	//
	COLORREF crFillColor = GetSysColor( COLOR_WINDOW ) ;
	COLORREF crTextColor = GetSysColor( COLOR_WINDOWTEXT ) ;

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		crFillColor = GetSysColor( COLOR_HIGHLIGHT ) ;
		crTextColor = GetSysColor( COLOR_HIGHLIGHTTEXT ) ;
	}

	// Handle drawing according to action
	//

	// Setup DC
	COLORREF oldTextColor = pDC->SetTextColor( crTextColor );
	COLORREF oldBkColor   = pDC->SetBkColor( crFillColor );
	CFont* pOldFont = pDC->SelectObject( GetParent()->GetFont() );

	// Determine location to draw bitmap.  This information 
	// is needed for all drawing modes, so might as well
	// just determine it once, and in one place.
	CSize	  szExtent = pDC->GetTextExtent( itemData.m_strDesc );

	RIntRect  cellRect( lpDrawItemStruct->rcItem );
	cellRect.Inset( RIntSize( 4, 4 ) );
	cellRect.m_Bottom -= szExtent.cy + 2; // + 2 is for spacing between graphic & text

	RIntRect  imgRect( 0, 0, bm.bmWidth - 1, bm.bmHeight - 1 );
	imgRect.ShrinkToFit( cellRect );
	imgRect.CenterRectInRect( cellRect );

	CSize  szImage( imgRect.Width(), imgRect.Height() );
//	CPoint centerPt( cellRect.CenterPoint() );
//	CPoint ptTopLeft( centerPt.x - szImage.cx / 2, centerPt.y - szImage.cy / 2 - 2 );

	switch (lpDrawItemStruct->itemAction)
	{
	case ODA_DRAWENTIRE:
		{
		CDC memDC;
		memDC.CreateCompatibleDC( pDC );

		// Get the bitmap
		CBitmap* pBmp    = CBitmap::FromHandle( hBitmap );
		CBitmap* pOldBmp = memDC.SelectObject( pBmp );

		pDC->StretchBlt( imgRect.m_Left, imgRect.m_Top, imgRect.Width(), imgRect.Height(), 
			&memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY );

		memDC.SelectObject( pOldBmp );

		if (!(lpDrawItemStruct->itemState & ODS_SELECTED))
		{
			break ;
		}

		// Fall through
		}

	case ODA_SELECT:
		{
		// Draw/Clear the highlight rect
//		CRect bmpRect( ptTopLeft.x, ptTopLeft.y, ptTopLeft.x + bm.bmWidth, ptTopLeft.y + bm.bmHeight);
		
		CRect bmpRect( imgRect );
		bmpRect.InflateRect( 2, 2 );

		CPen pen( PS_SOLID, 2, crFillColor );
		CPen* pOldPen = pDC->SelectObject( &pen );

		pDC->MoveTo( bmpRect.left, bmpRect.top );
		pDC->LineTo( bmpRect.right, bmpRect.top );
		pDC->LineTo( bmpRect.right, bmpRect.bottom );
		pDC->LineTo( bmpRect.left, bmpRect.bottom );
		pDC->LineTo( bmpRect.left, bmpRect.top );
		pDC->SelectObject( pOldPen );
		}

	} // switch

	// Draw the text
	CPoint ptText( cellRect.m_Left + (cellRect.Width() - szExtent.cx) / 2, cellRect.m_Bottom + 4 );
	pDC->TextOut( ptText.x, ptText.y, itemData.m_strDesc );

//	pDC->DrawText( itemData.m_strDesc, &textRect, DT_CALCRECT | DT_SINGLELINE  );
//	textRect.OffsetRect( -textRect.Width() / 2, 2 );
//	pDC->DrawText( itemData.m_strDesc, &textRect, DT_CENTER | DT_VCENTER );

	// restore DC
	pDC->SelectObject( pOldFont );
	pDC->SetTextColor( oldTextColor );
	pDC->SetBkColor( oldBkColor );
}

//@mFunc must have this function because base classes is a pure virtual.  However
//sizing takes plase in the client by a call to SizeCells to Client
void CQSLItemList::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
}

//@mFunc virtual override.  Posts message to parent telling parent that the top
//index has changed.  This is necessary to grey out the next and prev buttons

int CQSLItemList::SetTopIndex( int nIndex )
{
	//If there is a parent we need to let it know if the top index changed
	//so it can enable or disable buttons if needed
	CWnd* pWnd = GetParent();
	if( pWnd )
	{
		pWnd->PostMessage( WM_COMMAND, UM_TOP_INDEX_CHANGED );
	}
	//call base classes SetTopIndex to actually do the work
	return RGridCtrlBase::SetTopIndex( nIndex );
}

//*****************************************************************************
//
// Function Name: CQSLItemList::OnEraseBkgnd(CDC* pDC) 
//
// Description:   Message handler for WM_ERASEBKGND
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
BOOL CQSLItemList::OnEraseBkgnd(CDC* pDC) 
{
	COLORREF crColor = GetSysColor( COLOR_WINDOW ) ;
 
	CRect rect ;
	GetClientRect( rect ) ;
	pDC->FillSolidRect( &rect, crColor ) ;

	return TRUE ;
}

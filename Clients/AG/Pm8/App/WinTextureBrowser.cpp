//****************************************************************************
//
// File Name:   WinTextureBrowser.cpp
//
// Project:     Framework user interface
//
// Author:      Lance Wilson
//
// Description: Implementation of the RWinTextureBrowser dialog object
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/WinTextureBrowser.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:14p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************
#include "stdafx.h"
ASSERTNAME

#include "WinTextureBrowser.h"
#include "CollectionManager.h"
#include "DcDrawingSurface.h"
#include "ComponentView.h"
#include "ImageInterface.h"
#include "ImageColor.h"
#include "Buffer.h"

const int kTextureGridRows = 4;
const int kTextureGridCols = 6;

const YRealDimension _arScalars[3] = { 1.0, 0.5, 0.3 };

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class RReadOnlyBuffer : public RBuffer
{
public:
							RReadOnlyBuffer( uBYTE*	pBuffer, uLONG ulBufferSize );
	virtual					~RReadOnlyBuffer(); 
};

RReadOnlyBuffer::RReadOnlyBuffer( uBYTE* pBuffer, uLONG ulBufferSize )
{
	m_pBuffer     = pBuffer;
	m_uBufferSize = ulBufferSize;
}

RReadOnlyBuffer::~RReadOnlyBuffer()
{
	// Prevent the buffer from being deleted,
	// as it is only borrowed.
	m_pBuffer = NULL;
}

//*****************************************************************************
//
// Function Name: RBackdropGridCntrl::RBackdropGridCntrl() 
//
// Description:   Default constructor
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
RTextureGridCtrl::RTextureGridCtrl( ) :
	RGridCtrlBase( kAddSpacing | k3dStyle | kIntegral ),
	m_nTint( kMaxTint )
{
}

//*****************************************************************************
//
// Function Name: RTextureGridCtrl::~RTextureGridCtrl() 
//
// Description:   Destructor
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
RTextureGridCtrl::~RTextureGridCtrl()
{
	EmptyCache();
}

BEGIN_MESSAGE_MAP(RTextureGridCtrl, RGridCtrlBase)
	//{{AFX_MSG_MAP(RTextureGridCtrl)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void RTextureGridCtrl::EmptyCache()
{
	for (int i = m_arrComponentCache.Count() - 1; i >= 0; i--)
		delete m_arrComponentCache[i] ;

	m_arrComponentCache.Empty() ;
}

RImageColor* RTextureGridCtrl::GetCachedItem( int nIndex )
{
	nIndex /= 3;

	if (nIndex >= m_arrComponentCache.Count() || !m_arrComponentCache[nIndex])
	{
		while (m_arrComponentCache.Count() <= nIndex)
			m_arrComponentCache.InsertAtEnd( NULL );

		// Get the storage the item is in
		SearchResult& item = (*m_pResultArray)[nIndex] ;
		RStorage& storage = *( item.collectionStorage->GetDataParent( ) );
		TpsAssertValid( &storage );

		// Save the current storage position, so we can go back when we are done
		YStreamLength pos	= storage.GetPosition( );

		// Seek to the location in the storage of the item
		storage.SeekAbsolute( item.dataOffset );

		// Read in the graphic header
		GRAF_Header	grHeader;
		storage.Read( sizeof(grHeader), (uBYTE*) &grHeader );

		// Seek past the thumbnail and the DS_CLIPIMAGE structure
		// REVIEW STA - When we have a declaration for DS_CLIPIMAGE, use sizeof it instead
		storage.SeekRelative( grHeader.thumbnailSize + 24 );

		// We are now positioned at the beginning of image data. Load from it.
		RStream& rStream = *(storage.GetStorageStream());
		RReadOnlyBuffer rBuffer( rStream.GetBuffer( grHeader.grDataSize ), grHeader.grDataSize );
		m_arrComponentCache[nIndex] = new RImageColor();
		m_arrComponentCache[nIndex]->Initialize( item.id, rBuffer );

		// Get back to where we started in the storage
		storage.SeekAbsolute( pos );
	}

	return m_arrComponentCache[nIndex];
}

void RTextureGridCtrl::SetTint( YTint tint )
{
	if (tint != m_nTint)
	{
		m_nTint = tint;
		Invalidate( FALSE );
	}
}

YTint RTextureGridCtrl::SelectColor( const RColor& crColor )
{
	if (RColor::kBitmap == crColor.GetFillMethod())
	{
		const RImageColor* pColor = crColor.GetImageColor();

		uLONG nPos ;
		FindSearchResultArrayID( m_pResultArray, pColor->m_ulID, nPos ) ;

		if ((int) nPos < m_pResultArray->Count())
		{
			YTint tint = pColor->GetTint();

			int nCurSel = int( nPos * 3 ) + pColor->m_uwIndex;
			SetTint( tint );
			SetCurSel( nCurSel );

			return tint;
		}
	}

	return kMaxTint;
}

void RTextureGridCtrl::SetData( RSearchResultArray* pResultArray )
{
	SetRedraw( FALSE ) ;
	SetTopIndex( 0 ) ;
	SetCurSel( LB_ERR ) ;

	EmptyCache();
	m_pResultArray = pResultArray;
	InitStorage( m_pResultArray->Count() * 3 ) ;

	SetRedraw( TRUE ) ;
	Invalidate( TRUE ) ;
}

void RTextureGridCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// Load the component if necessary
	int nIndex = lpDrawItemStruct->itemID; // / 3;
	int nSubIndex = nIndex % 3; //lpDrawItemStruct->itemID % 3;


	// Draw the button style portion
	//
	RECT& rect = lpDrawItemStruct->rcItem;
	RIntRect rcDestRect( rect.left, rect.top, rect.right, rect.bottom ) ;

	RDcDrawingSurface ds;
	ds.Initialize( lpDrawItemStruct->hDC, lpDrawItemStruct->hDC );

	// Draw the button attributes
	RSolidColor crHilite( GetSysColor( COLOR_BTNHILIGHT ) );
	RSolidColor crShadow( GetSysColor( COLOR_BTNSHADOW ) );
	RSolidColor crWhite( RGB( 255, 255, 255 ) );
	RSolidColor crBlack( RGB( 0, 0, 0 ) );

	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		// Reverse the colors so the button is drawn in the down state.
		crHilite = ( GetSysColor( COLOR_BTNSHADOW ) );
		crShadow = ( GetSysColor( COLOR_BTNHILIGHT ) );
		crBlack  = RGB( 255, 255, 255 );
		crWhite  = RGB( 0, 0, 0 );
	}
	
	ds.Draw3dRect( rcDestRect, crWhite, crBlack );
	rcDestRect.Inset( RIntSize( 1, 1 ) );
	ds.Draw3dRect( rcDestRect, crHilite, crShadow );
	rcDestRect.Inset( RIntSize( 1, 1 ) );

	if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
	{
		RImageColor* pColor = GetCachedItem( nIndex );

		if (pColor)
		{
			// Determine transformation: Note, multiplying the base
			// transform by 0.5 is just to make the tile more apparent
			// in the small selection cells.
			RRealSize dpi = ds.GetDPI( );
			R2dTransform transform;
			transform.PostScale( _arScalars[nSubIndex % 3] * 0.5, _arScalars[nSubIndex % 3] * 0.5 );
			
			pColor->SetBaseTransform( transform );
			pColor->SetTint( m_nTint );


			// Draw the texture
			//
			RRealRect rLogicalRect( rcDestRect );
			::ScreenUnitsToLogicalUnits( rLogicalRect );

			RColor crFillColor( *pColor );
			crFillColor.SetBoundingRect( RIntRect( (RIntSize) rLogicalRect.WidthHeight() ) );

			transform.MakeIdentity();
			transform.PostScale( dpi.m_dx / kSystemDPI, dpi.m_dy / kSystemDPI );
			crFillColor *= transform;

			ds.SetFillColor( crFillColor );
			ds.FillRectangle( rcDestRect );
		}
	}
//	else if (lpDrawItemStruct->itemAction == ODA_FOCUS) 
//		ds.DrawFocusRect( rcDestRect ) ;

	ds.DetachDCs() ;
}

void RTextureGridCtrl::MeasureItem( LPMEASUREITEMSTRUCT )
{
}

RColor RTextureGridCtrl::SelectedColor() const
{
	int nIndex = GetCurSel() / 3;
	int nSubIndex = GetCurSel() % 3;

	if (nIndex >= 0)
	{
		RImageColor* pImageColor = m_arrComponentCache[nIndex];

		if (pImageColor)
		{
			// Determine transformation
			R2dTransform transform;
			transform.PostScale( _arScalars[nSubIndex % 3], _arScalars[nSubIndex % 3] );
			pImageColor->SetBaseTransform( transform );
			pImageColor->m_uwIndex = nSubIndex;;

			return RColor( *pImageColor );
		}
		else
		{
			// TODO: Implement some kind of error message
		}
	}

	return RColor();
}

//*****************************************************************************
//
// Function Name: RTextureGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
//
// Description:   Message handler for WM_VSCROLL
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RTextureGridCtrl::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
//	if (nSBCode != SB_THUMBTRACK)
		RGridCtrlBase::OnVScroll( nSBCode, nPos, pScrollBar );
}


/////////////////////////////////////////////////////////////////////////////
// RWinTextureBrowser dialog

RColor RWinTextureBrowser::mruColors[5];

//*****************************************************************************
//
// Function Name:  RWinTextureBrowser
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RWinTextureBrowser::RWinTextureBrowser( CWnd* pParent ): 
	CDialog( IDD, pParent )
{
	//{{AFX_DATA_INIT(RWinTextureBrowser)
	//}}AFX_DATA_INIT

	m_pCollectionSearcher = NULL;
}

RWinTextureBrowser::~RWinTextureBrowser()
{
	if (m_pCollectionSearcher)
	{
		delete m_pCollectionSearcher ;
	}

	RCollectionManager::TheCollectionManager().CloseCollectionsOfType( 
		kTexturesCollectionType );
}

//*****************************************************************************
//
// Function Name: DoDataExchange
//
// Description:   Method for transferning the contents of the child
//				      controls to/from the data member variables.  As
//						well as any subclassing.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
void RWinTextureBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RWinTextureBrowser)
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_CURRENT, m_scCurrent ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_MRU1, m_scMRUList[0] ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_MRU2, m_scMRUList[1] ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_MRU3, m_scMRUList[2] ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_MRU4, m_scMRUList[3] ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES_MRU5, m_scMRUList[4] ) ;
	DDX_Control( pDX, CONTROL_COLOR_NONE, m_scTransparent ) ;
	DDX_Control( pDX, CONTROL_LIST_TEXTURES,  m_gcTextureList ) ;
	DDX_Control(pDX, CONTROL_LIST_TEXTURES_TINT, m_cbTintList) ;
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( RWinTextureBrowser, CDialog )
	//{{AFX_MSG_MAP(RWinTextureBrowser)
	ON_CBN_SELCHANGE(CONTROL_LIST_TEXTURES_TINT, OnSelchangeTextureTint)
	ON_LBN_SELCHANGE(CONTROL_LIST_TEXTURES, OnColorSelChange)
	ON_MESSAGE( UM_COLOR_CHANGED, OnColorChange )
	ON_WM_DESTROY( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//****************************************************************************
//
// Function Name: OnInitDialog
//
// Description:   Initializes dialog, gradient button controls and creates and 
//                positions the angle control.
//
// Returns:       TRUE
//
// Exceptions:	   None
//
//****************************************************************************
BOOL RWinTextureBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();

	try 
	{
		RCollectionArray& rArray = RCollectionManager::TheCollectionManager().
			OpenCollectionsOfType( kTexturesCollectionType ) ;

		// If we didn't find any collection files, we should exit.  Note:
		// an error message will have already been displayed.
		if (rArray.Count() == 0)
			throw kResource;

		// Create the collection searcher object.
		m_pCollectionSearcher  = new RCollectionSearcher< 
			RTextureSearchCollection >( rArray ) ;

		// search for all textures
		RSearchResultArray* pResultArray = m_pCollectionSearcher->SearchCollections();
		m_gcTextureList.SizeCellsToClient( kTextureGridRows, kTextureGridCols );
		m_gcTextureList.SetData( pResultArray );
		
		YTint tint = m_gcTextureList.SelectColor( m_crColor );
		m_cbTintList.SetCurSel( 10 - tint / 10 ) ;

		RColor crTransparent;
		m_scCurrent.SetColor( m_crColor );
		m_scTransparent.SetColor( crTransparent );

		for (int i = 0; i < NumElements( mruColors ); i++)
		{
			m_scMRUList[i].SetColor( mruColors[i] );
		}
	}
	catch( ... )
	{
		EndDialog( IDCANCEL ) ;
		return TRUE ;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//****************************************************************************
//
// Function Name: OnSelchangeTextureTint
//
// Description:   Selection change handler for the tint
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RWinTextureBrowser::OnSelchangeTextureTint() 
{
	int nSel = m_cbTintList.GetCurSel() ;

	if (LB_ERR != nSel)
	{
		CString strText ;
		m_cbTintList.GetLBText( nSel, strText ) ;
		m_gcTextureList.SetTint( atoi( strText ) ) ;
		m_gcTextureList.UpdateWindow();

		if (LB_ERR != m_gcTextureList.GetCurSel())
		{
			m_crColor = m_gcTextureList.SelectedColor();
			m_scCurrent.SetColor( m_crColor );
		}
	}
}

RColor RWinTextureBrowser::SelectedColor() const
{
	return m_crColor;
}

void RWinTextureBrowser::SetColor( RColor crColor )
{
	m_crColor = crColor;
}

void RWinTextureBrowser::AddSelectedToMRU()
{
	if (m_crColor.GetFillMethod() != RColor::kTransparent)
	{
		// First see if it is in the list
		for (int i = 0; i < NumElements( mruColors ); i++)
			if (m_crColor == mruColors[i])
				break;

		for (i = i - 1; i >= 0; i--)
			if (i < NumElements( mruColors ) - 1)
				mruColors[i + 1] = mruColors[i];

		mruColors[0] = m_crColor;
	}
}

//*****************************************************************************
//
// Function Name: RWinTextureBrowser::OnColorSelChange() 
//
// Description:   Handler for selection change in grid list
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RWinTextureBrowser::OnColorSelChange() 
{
	int nSelected = m_gcTextureList.GetCurSel();

	if (nSelected != LB_ERR)
	{
		m_crColor = m_gcTextureList.SelectedColor();
		m_scCurrent.SetColor( m_crColor );
	}
}

LRESULT RWinTextureBrowser::OnColorChange( WPARAM nID, LPARAM )
{
	switch (nID)
	{
	case CONTROL_LIST_TEXTURES_CURRENT:
		m_crColor = m_scCurrent.SelectedColor();
		break;

	case CONTROL_COLOR_NONE:
		m_crColor = m_scTransparent.SelectedColor();
		break;

	default:
		m_crColor = m_scMRUList[nID - CONTROL_LIST_TEXTURES_MRU1].SelectedColor();
	}

	YTint tint = m_gcTextureList.SelectColor( m_crColor );
	m_cbTintList.SetCurSel( 10 - tint / 10 ) ;
	m_scCurrent.SetColor( m_crColor );

	return 0L;
}

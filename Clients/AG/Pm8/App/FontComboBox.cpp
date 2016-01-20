// ****************************************************************************
//
//  File Name:			FontComboBox.cpp
//
//  Project:			Renaissance application
//
//  Author:				D. Selman
//
//  Description:		Definition of the CPFontComboBox class
//
//  Portability:		Windows
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/FontComboBox.cpp                                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:05p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"stdafx.h"
ASSERTNAME

#include "FontComboBox.h"
//#include	"RenaissanceView.h"
//#include "PanelView.h"
#include	"FontLists.h"
#include	"DialogUtilities.h"
#include	"ApplicationGlobals.h"
//#include "TextToolbar.h"
//#include "RenaissanceResource.h"

// Default bitmap width is base on a 128 dlus (currently the
// setting of the font combo box in the headline dialog).
const int kDefaultFontBitmapWidth = 128 * LOWORD( GetDialogBaseUnits() ) / 4;

//these are static so that both the thread and the combo box can access them.
//care must be taken so they are not being altered by both threads at the same time
static int nItemHeight = 0;
//flag to signal thread that it needs to exit as soon as possible.
static BOOL bKillThread = FALSE;
// the DIB that holds all the fonts
static CBitmap*	m_gFontDIB = NULL;
// the number of fonts currently in the DIB
static int			m_gFonts = 0;

// the linked control, if bitmap is used for more then one combobox
CComboBox* CPFontComboBox::m_pLinkedControl = NULL;


UINT FontInitProc(LPVOID lpParam);
//return 0 thread completed successfully.
//return 1 thread returned prematurely.


// ****************************************************************************
//
//  Function Name:	CPFontComboBox::CPFontComboBox()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

CPFontComboBox::CPFontComboBox( BOOLEAN fDeleteBitmap ) :
	m_fDeleteBitmap( fDeleteBitmap ),
	m_pBmpInitThread( NULL )
{
}

CPFontComboBox::~CPFontComboBox()
{
	if (m_pBmpInitThread != NULL)
	{
		bKillThread = TRUE;
		//wait for the thread to exit before deleting array contents.
		WaitForSingleObject( m_pBmpInitThread, INFINITE );
	}

	if (m_fDeleteBitmap && m_gFontDIB)
		delete m_gFontDIB;
}
 
// ****************************************************************************
//
//  Function Name:	RSizeComboBox::Initialise()
//
//  Description:		Initialise function
//
//  Returns:			FALSE on error
//
//  Exceptions:		None
//
// ****************************************************************************

BOOL CPFontComboBox::Initialise()
{
	try
	{
		RMBCString rFontList = (RApplication::GetApplicationGlobals())->GetFontList();
		::ParseAndPutInComboBox( rFontList, *this );

		nItemHeight = GetItemHeight(0);
		//setup memory dc and logfont so we do not have to create and initialize it for
		//each bitblt.
		m_memDC.CreateCompatibleDC(NULL);
		memset(&m_logFont, 0, sizeof(LOGFONT));
		m_logFont.lfWeight		= 400;
		m_logFont.lfCharSet	= DEFAULT_CHARSET;	// this means any charset and uses name to match
		m_logFont.lfHeight = nItemHeight;

		//start the thread to init the bitmaps
		if (m_fDeleteBitmap || !m_gFontDIB)
			m_pBmpInitThread = AfxBeginThread(FontInitProc, NULL );
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

void CPFontComboBox::UpdateFontList()
{
	(RApplication::GetApplicationGlobals())->BuildFontList();
	RMBCString rFontList = (RApplication::GetApplicationGlobals())->GetFontList();
	::ParseAndPutInComboBox( rFontList, *this );

	if (m_pLinkedControl && m_pLinkedControl != this)
		m_pLinkedControl->GetParent()->PostMessage( WM_FONTCHANGE );

	bKillThread = TRUE;
	//wait for the thread to exit before deleting array contents.
	WaitForSingleObject( m_pBmpInitThread, INFINITE );
	//start the thread to init the bitmaps
	bKillThread = FALSE;
	m_pBmpInitThread = AfxBeginThread(FontInitProc, NULL );
}


void CPFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	int itemID = lpDrawItemStruct->itemID;
	//No item selected if we don't return here we'll get an error when trying to get the text
	if( itemID == -1 )
		return;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CString strText;
	CRect rcItem = lpDrawItemStruct->rcItem;

	GetLBText(itemID, strText);
	
		//draw the name of the font in the edit window using the window font
	if(lpDrawItemStruct->itemState & ODS_COMBOBOXEDIT)
	{
		CFont* pOldFont = pDC->SelectObject(GetFont());
		pDC->TextOut(rcItem.left, rcItem.top, strText);
		pDC->SelectObject( pOldFont );
		return;
	}

	CBitmap* pBmp = NULL;
	BOOL bNewBitmap = FALSE;

	// the starting height depends on where the bitmap is coming from
	int	nStartY = 0;

	//make sure that the bitmap is initialized for the item if not create one.
	//we don't add this to the bitmap array because we don't want to take a chance
	//on colliding with the thread that is initializing the DIB.  We could use a 
	//mutex but this seems easier.
	if( itemID >= m_gFonts )
	{
//		TRACE("have created one\n");
		pBmp = new CBitmap;
		bNewBitmap  =	TRUE;
		strcpy(m_logFont.lfFaceName, strText);
		CreateBmp( pBmp, &m_memDC, &m_logFont);

		nStartY = 0;
	}
	//else get the correct one
	if(!pBmp)
	{
		TRACE("have correct one\n");
		pBmp = m_gFontDIB;
		nStartY = nItemHeight * itemID;
	}

	CBitmap* pOldBmp = m_memDC.SelectObject( pBmp );

	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		//set the back and foreground colors to the highlight colors
		COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		COLORREF oldBkColor = pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
		pDC->BitBlt(rcItem.left, rcItem.top, kDefaultFontBitmapWidth, nItemHeight, &m_memDC, 0, nStartY, SRCCOPY);
		pDC->SetTextColor(oldTextColor);
		pDC->SetBkColor(oldBkColor);
	}
	else
	{
		pDC->BitBlt(rcItem.left, rcItem.top, kDefaultFontBitmapWidth, nItemHeight, &m_memDC, 0, nStartY, SRCCOPY);
	}
 
	m_memDC.SelectObject(pOldBmp);
	//if we created a temp bmp we need to delete it
	if(bNewBitmap)
		delete pBmp;
}

void CPFontComboBox::CreateBmp( CBitmap* pBmp, CDC* pDC, LOGFONT* pLF )
{
	//make sure width is even
	int width = kDefaultFontBitmapWidth; //RTextToolbar::kFontDroppedWidth;
	if(width % 2)
		width += 1;
	//create the bitmap as a monochrome bitmap
	pBmp->CreateBitmap( width, nItemHeight, 1,
							1, NULL );

	CBitmap* pOldBmp = pDC->SelectObject(pBmp);
	//we need to init the whole bitmap rect because text out doesn't fill beyond the 
	//end of the text
	CRect rect(0,0,width, nItemHeight );
	pDC->FillSolidRect(rect, RGB(255,255,255));

	//Set the fore and background color
	COLORREF oldColor = pDC->SetTextColor(RGB(0,0,0));
	COLORREF oldBkColor = pDC->SetBkColor(RGB(255,255,255));

	//create, select, and draw the font
	CFont* pFont = new CFont;
	pFont->CreateFontIndirect(pLF);
	CFont* pOldFont = pDC->SelectObject(pFont);
	pDC->TextOut(0, 0, pLF->lfFaceName);

	//clean up
	pDC->SelectObject( pOldFont );
	pDC->SetTextColor(oldColor);
	pDC->SetBkColor(oldBkColor);
	pDC->SelectObject(pOldBmp);

	delete pFont;
}	

// ****************************************************************************
//
//  Function Name:	RSizeComboBox::CreateFontDIB()
//
//  Description:		Create the DIB and clear it
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL CPFontComboBox::CreateFontDIB( CDC* pDC )
{
	// delete old bitmap if exists
	if (m_gFontDIB)
		delete(m_gFontDIB);

	// get number of fonts
	uLONG	uNumberFonts = (RApplication::GetApplicationGlobals())->GetNumberOfFonts();

	// 05/04/98 - Made the width a little longer so that
	// it will work inside the headline dialog too.
	//make sure width is even
	int nWidth = kDefaultFontBitmapWidth;  //RTextToolbar::kFontDroppedWidth;
	if(nWidth % 2)
		nWidth += 1;
	
	int nHeight = nItemHeight * (int)uNumberFonts;

	m_gFontDIB = new CBitmap();		
	BOOL bCreate = m_gFontDIB->CreateBitmap(nWidth, nHeight, 1, 1, NULL);
	ASSERT( bCreate );
	
	if (bCreate)
	{
		CBitmap* pOldBmp = pDC->SelectObject(m_gFontDIB);
	
		//we need to init the whole bitmap rect because text out doesn't fill beyond the 
		//end of the text
		CRect rect(0, 0, nWidth, nHeight);
		pDC->FillSolidRect(rect, RGB(255,255,255));

		//clean up
		pDC->SelectObject(pOldBmp);
	}
	return bCreate;
}	

// ****************************************************************************
//
//  Function Name:	RSizeComboBox::AddFontToDIB()
//
//  Description:		Adds the next font to the DIB
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void CPFontComboBox::AddFontToDIB( int nFontToAdd, CDC* pDC, LOGFONT* pLF )
{
	// get number of fonts
	uLONG	uNumberFonts = (RApplication::GetApplicationGlobals())->GetNumberOfFonts();

	// cant add more fonts than reserved space for
	if ((nFontToAdd + 1) > (int)uNumberFonts)
		return;

	//make sure width is even
	int nWidth = kDefaultFontBitmapWidth; //RTextToolbar::kFontDroppedWidth;
	if(nWidth % 2)
		nWidth += 1;
	
	// starting height for this font 
	int nStartY = nItemHeight * nFontToAdd;

	CBitmap* pOldBmp = pDC->SelectObject(m_gFontDIB);

	//Set the fore and background color
	COLORREF oldColor = pDC->SetTextColor(RGB(0,0,0));
	COLORREF oldBkColor = pDC->SetBkColor(RGB(255,255,255));

	//create, select, and draw the font
	CFont* pFont = new CFont;
	pFont->CreateFontIndirect(pLF);
	CFont* pOldFont = pDC->SelectObject(pFont);
	pDC->TextOut(0, nStartY, pLF->lfFaceName);

	//clean up
	pDC->SelectObject( pOldFont );
	pDC->SetTextColor(oldColor);
	pDC->SetBkColor(oldBkColor);
	pDC->SelectObject(pOldBmp);

	delete pFont;
}	

UINT FontInitProc(LPVOID /*lpParam*/)
{
	// clear number of fonts
	m_gFonts = 0;

	//get font list and number of fonts
	RMBCString rFontList = (RApplication::GetApplicationGlobals())->GetFontList();

	//make a copy for use by strtok
	char* pList = new char[ rFontList.GetStringLength() + 1 ];
	strcpy( pList, rFontList );

	CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	//set these variables so they are the same for all fonts
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfWeight		= 400;
	lf.lfCharSet	= DEFAULT_CHARSET;	// this means any charset and uses name to match
	lf.lfHeight = nItemHeight;
	
	// Create DIB to hold all of the fonts
	BOOL bCreate = CPFontComboBox::CreateFontDIB( &memDC );

	if (bCreate)
	{
		char* pFontName = NULL;

		//get the first font name
		pFontName = strtok( pList, kListFieldSeperator );
		//loop through all fonts
		while( pFontName )
		{
			//are we being requested to exit early
			if( bKillThread)
			{	
				delete pList;//clean up
				return 1;//premature return
			}

			ASSERT(pFontName);
			if(!pFontName)
				break;

			strcpy(lf.lfFaceName, pFontName);
		
			//create the font bitmap and put it in the DIB
			CPFontComboBox::AddFontToDIB ( m_gFonts, &memDC, &lf );

			// increment font count
			m_gFonts++;

			//get the next font name
			pFontName = strtok( NULL, kListFieldSeperator );
		}
	}
	delete pList;

	return 0;
}



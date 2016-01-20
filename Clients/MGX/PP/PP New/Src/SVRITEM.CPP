/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                svritem.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include <limits.h>
#include "svrdoc.h"
#include "svrview.h"
#include "svritem.h"
#include "dataitem.h"

__declspec( dllimport ) LPKCMSEVALPROC lpKCMSEvalProc;

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

#define CX_MARGIN   8
#define CY_MARGIN   4

#define HIMETRIC_PER_INCH 2540

/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */
/* ******************************* Local Data ******************************* */
/* ******************************* Private Data ***************************** */
/* ******************************* Public Data ****************************** */
/* *************************** Private Functions **************************** */
/* *************************** Private Methods ****************************** */

void CServerItem::get_bounds (CRect & rBounds)
{
   CString szItemName = GetItemName();

   ImgGetImageRect(m_lpImage, &rBounds);
   
   if ( szItemName != "." ) 
   {
      CRect rTemp;
      CRect rTemp2;

		sscanf( szItemName.GetBuffer(szItemName.GetLength()), "(%d,%d)-(%d,%d)",	     
			&rTemp.left, &rTemp.top, &rTemp.right, &rTemp.bottom );

      // make sure that the masked bounds we thinks we have are within the 
      // bounds of the image.  This can change due an action such as cropping.
      // If the areas intersect then take that intersection and make it the
      // NEW linked area.
      if (rTemp2.IntersectRect(&rBounds,&rTemp))
      {
         CString ItemName;
         rBounds = rTemp2;   
         CreateItemName ((LPRECT)&rBounds,ItemName);
         SetItemName((LPCSTR)ItemName.GetBuffer(ItemName.GetLength()));  
      }
   }
}

void CServerItem::init_dataformats()
{
	// This is here to provide DIB presentations for originally OLE1.0 PP objects
	// Generally, we render all cached presentation in METAFILE format for
	// our OLE2.0 Implementation ( thru the MFC support ).  However, this 
	// was not always the case and there is no way to get Servers like Word 6 
	// and Excel 5 to requery for our supported cache types.
    FORMATETC FormatEtc;
    SETDefFormatEtc(FormatEtc,CF_DIB,TYMED_HGLOBAL);
    GetDataSource()->DelayRenderData(CF_DIB,&FormatEtc);

#ifndef _DESIGNERFIX
	// add formats that can be SET/GET on the item.  ie) This is used when 
	// Designer >= 4.0 uses PP50 as the editor of choice.  The idea is for
	// that app to be able to launch us and at will be able to set new or retrieve
	// existing iamge information.
	SETDefFormatEtc(FormatEtc,CF_DIB,TYMED_HGLOBAL);
	GetDataSource()->DelaySetData(CF_DIB,&FormatEtc);
	SETDefFormatEtc(FormatEtc,CF_BITMAP,TYMED_GDI);
	GetDataSource()->DelaySetData(CF_BITMAP,&FormatEtc);
#endif //_DESIGNERFIX
}

/* ***************************** Public Functions ************************** */

void CreateItemName (LPRECT lpMaskRect,CString & ItemName)
{
    // Create an object string to describe the masked area    
    if (!lpMaskRect)                   
        // If there is no mask/object rectangle...
		ItemName = ".";
    else  
    {
        wsprintf( ItemName.GetBuffer(MAX_STR_LEN), "(%d,%d)-(%d,%d)",
           lpMaskRect->left, lpMaskRect->top, lpMaskRect->right, 
           lpMaskRect->bottom );    
		ItemName.ReleaseBuffer();
    }        
}

HANDLE CreateMetaFromImage( LPIMAGE lpImage, RECT rect )
{
	HANDLE hMem = NULL;
	POINT  point;	

	rect.right--;
	rect.bottom--;
	
	HDC hDC;
	LPBLT lpBlt;
	CWindowDC WindowDC(::AfxGetMainWnd()); 

	// Create a memory metafile and return its handle.
	if ( !(hDC = CreateMetaFile( NULL )) )
	{
	    Message( IDS_EMEMALLOC );
	    return( NULL );
	}
	
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetWindowOrgEx (hDC,0,0,&point);
	SetWindowExtEx (hDC,RectWidth(&rect), RectHeight(&rect), NULL);
	if (lpBlt = InitSuperBlt( WindowDC.GetSafeHdc(), 0, NULL, YES, YES, YES, NO))
	{
		if( lpImage->m_cmsXform )
			{
			lpImage->m_cmsXform = CmsCopyGammaXform( lpImage->m_cmsXform );
			copy( lpBltScreen->CmsRGammaLut, lpBlt->CmsRGammaLut,
				sizeof( lpBlt->CmsRGammaLut ) );
			copy( lpBltScreen->CmsGGammaLut, lpBlt->CmsGGammaLut,
				sizeof( lpBlt->CmsGGammaLut ) );
			copy( lpBltScreen->CmsBGammaLut, lpBlt->CmsBGammaLut,
				sizeof( lpBlt->CmsBGammaLut ) );
			}

		if (DrawImage(hDC,lpBlt,&rect,&rect,lpImage,NULL,TRUE,0,TRUE))
		{
			HMETAFILE hMF;
			LPMETAFILEPICT lpMF;
	
			if ( !(hMF = CloseMetaFile( hDC )) )
			{
			    Message( IDS_EMEMALLOC );
			    return( NULL );
		    }
	
			// Allocate the METAFILEPICT structure.
			if ( !(hMem = GlobalAlloc( GMEM_SHARE | GMEM_MOVEABLE, 
				sizeof(METAFILEPICT) )) )
		    {
			    Message( IDS_EMEMALLOC );
			    DeleteMetaFile( hMF );
			    return( NULL );
		    }
	
			int xRes = WindowDC.GetDeviceCaps(LOGPIXELSX);
			int yRes = WindowDC.GetDeviceCaps(LOGPIXELSY);
			int xExt = MulDiv(RectWidth(&rect), HIMETRIC_PER_INCH, xRes);
			int yExt = MulDiv(RectHeight(&rect), HIMETRIC_PER_INCH, yRes);
			lpMF = (LPMETAFILEPICT)GlobalLock(hMem);
			lpMF->hMF = hMF;
			lpMF->mm = MM_ANISOTROPIC; // Required by OLE libraries.
			lpMF->xExt = xExt;
			lpMF->yExt = yExt;

			GlobalUnlock( hMem );
		}
		EndSuperBlt( lpBlt );
	}
	return( hMem );
}

int DrawImage(
    HDC     hDC,
	LPBLT   lpBlt,
    LPRECT  lpSourceRect,
    LPRECT  lpDestRect,
	LPIMAGE lpImageIn,
    LPRECT  lpRepairRect, /* NULL */
	BOOL    fUseStretchDIB, /* FALSE */
	int     LinesPerBlt, /* 0 */
	BOOL    bMetaDC /*FALSE*/)
{
	FRMTYPEINFO TypeInfo;
	int y, yline, ystart, ylast, dxCount, xstart, depth, ddx, ddy, sdx;
	int SrcWidth, DstWidth, PaintWidth;
	LFIXED yrate, xrate, yoffset, xoffset;
	RECT rPaint, rSource, rDest;
	long lSizeNeeded, lOffset;
	LPTR lpIn,lpOut;
	LPTR lpImageData;
	BLTSESSION BltSession;
	LPTR lpBuffer = NULL;
	LPFRAME lpBaseFrame = NULL;
	LPBLT lpB = NULL;


	ASSERT(lpImageIn);
	if (!lpImageIn)
		return FALSE;

	clr( ( LPTR )&BltSession, sizeof( BLTSESSION ) );

	lpImageData = NULL;
	lpBaseFrame = ImgGetBaseEditFrame(lpImageIn);

	if (!lpBaseFrame)
		return (FALSE);

	if (!lpBlt)
		lpB = lpBltScreen;
	else
		lpB = lpBlt;

	depth = FrameDepth( lpBaseFrame );
	if (depth == 0) depth = 1;
	FrameGetTypeInfo(lpBaseFrame, &TypeInfo);

	rDest = *lpDestRect;
	rSource = *lpSourceRect;
	if ( !lpRepairRect )
	    rPaint = rDest;
	else
	{
	    rPaint = *lpRepairRect;
	    BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
    }
	DstWidth = RectWidth( &rDest );
	SrcWidth = RectWidth( &rSource );
	PaintWidth = RectWidth(&rPaint);

	lSizeNeeded = (long)DstWidth * depth;
    if ( !(lpBuffer = Alloc(lSizeNeeded)) )
	{
		Message(IDS_EMEMALLOC);
        return(FALSE);
	}

	if (!AllocLines((LPPTR)&lpImageData, 1, FrameXSize(lpBaseFrame), depth))
	{
		Message(IDS_EMEMALLOC);
		FreeUp( lpBuffer );
		return(FALSE);
	}

	// compute x and y display rates
	yrate = FGET(RectHeight( &rSource),RectHeight(&rDest));
	xrate = FGET(SrcWidth, DstWidth);
	
	// get offsets from display edge to paint rect to adjust xoffset/yoffset
	ddx = rPaint.left - rDest.left;
	ddy = rPaint.top - rDest.top;
	dxCount = ddx + PaintWidth;
	yoffset = (yrate*ddy)+FDIV2(yrate);
	xoffset = (xrate*ddx)+FDIV2(xrate);
	
	// Find x-position in file coordinates where painting begins and ends
	sdx = WHOLE(xoffset);
	lOffset = ((long)sdx * (long)depth);
	xstart = rSource.left;
	
	ystart = rSource.top;
	ylast = -1;

	if (lpBlt) // never set new palette for lpBltScreen
	{
		SetNewPalette(lpB, lpB->ColorPaletteType, lpB->hColorPal, 
				  	lpB->lpColorPaletteLUT, lpB->ColorPalette, 
				  	lpB->nColorEntries, FALSE /* fApplyGamma */);
	}

	StartSuperBlt( &BltSession, hDC, NULL, lpB, &rPaint, TypeInfo,
	    LinesPerBlt, 0, 0, YES, lpImageIn->m_cmsXform, lpKCMSEvalProc, fUseStretchDIB,FALSE,bMetaDC);
	for ( y=rPaint.top; y<=rPaint.bottom; y++ )
    {
    	yline = ystart + WHOLE(yoffset);
	    yoffset += yrate;
	    if ( yline != ylast )
        {
    	    ylast = yline;

			ImgGetLine(lpImageIn, NULL, xstart, yline, 
				FrameXSize(lpBaseFrame) - xstart, lpImageData );
			lpIn = lpImageData;
	        if ( lpIn )
            {
	            FrameSample( lpBaseFrame, lpIn, 0, lpBuffer, (WORD)ddx, dxCount,
    	                xrate );
				lpOut = lpBuffer;
            }
			else
			{
				Message(IDS_EMEMALLOC);
				if (lpImageData)
					FreeUp(lpImageData);
				return FALSE;
			}
        }
	    SuperBlt( &BltSession, lpOut, bMetaDC ); 
    }
	SuperBlt( &BltSession, NULL, bMetaDC ); 

    if (lpBuffer)
   	    FreeUp(lpBuffer);

	if (lpImageData)
		FreeUp(lpImageData);

	return( TRUE );
}


/* ***************************** Public Methods ***************************** */

//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
// CServerItem Methods

CServerItem::CServerItem (
CServerDoc* pDoc, 
LPIMAGE lpImage,
LPRECT lpMaskRect) : COleServerItem (pDoc,TRUE /* bAutoDelete */)
{                  
    ASSERT(pDoc!=NULL);
	ASSERT(lpImage);

    m_lpImage = lpImage;  
	m_FileName.Empty();
    
    // Create an object string to describe the masked area    
	CString ItemName;
	CreateItemName (lpMaskRect,ItemName);
    SetItemName((LPCSTR)ItemName.GetBuffer(ItemName.GetLength()));  

	init_dataformats();
}

CServerItem::CServerItem (
CServerDoc* pDoc, 
LPSTR lpFileName,
LPRECT lpMaskRect) : COleServerItem (pDoc,TRUE /* bAutoDelete */)
{                  
    ASSERT(pDoc!=NULL);
	ASSERT(lpFileName);

    m_lpImage = NULL;  
	m_FileName = lpFileName;

    // Create an object string to describe the masked area    
	CString ItemName;
	CreateItemName (lpMaskRect,ItemName);
    SetItemName((LPCSTR)ItemName.GetBuffer(ItemName.GetLength()));  

	init_dataformats();
}

CServerItem::~CServerItem ()
{
	// this class doesn't own the lpImage it is passed in therefore don't 
	// delete m_lpImage
}                                         


BOOL CServerItem::OnDraw(
CDC* pDC, 
CSize& rSize)
{          
	BOOL bResult = FALSE;
	LPBLT lpBlt;
	CRect rBounds;
	CRect rItemPos;
	CSize rWindowExt;
	CWindowDC WindowDC(::AfxGetMainWnd()); 

	if (!pDC || !WindowDC.GetSafeHdc() || !pDC->GetSafeHdc())
		return TRUE;

	ProgressBegin(1,0);

	get_bounds (rBounds);
	rSize.cx = rBounds.Width();
	rSize.cy = rBounds.Height();

	if (GetDocument()->IsInPlaceActive())
	{
		GetDocument()->GetItemPosition(&rItemPos);
		rItemPos.right = rItemPos.Width();
		rItemPos.bottom = rItemPos.Height();
		rItemPos.left = 0;
		rItemPos.top = 0;
		rWindowExt.cx = rItemPos.Width();
		rWindowExt.cy = rItemPos.Height();
	}
	else
	{
		rItemPos.SetRect(0,0,rBounds.Width(),rBounds.Height());
		rWindowExt.cx = rBounds.Width();
		rWindowExt.cy = rBounds.Height();
	}


	// prepare to draw and remember the extent in himetric units
	// NOTE Do not use SetViewportOrg or SetViewportExt iff object 
	// can be resized
	pDC->SetWindowOrg (0,0);
	pDC->SetWindowExt (rWindowExt);

	// Second to Last Parm on initSuperBlt is TRUE to use RGB values in ColorPal LUT instead of indices.
	// Last Parm on initSuperBlt is FALSE to prevent the palette entries being created 
	// with Reserved flag.
	if (lpBlt = InitSuperBlt( WindowDC.GetSafeHdc(), 0, NULL, YES, YES, YES, NO))
	{
		if (m_lpImage)
			bResult = DrawImage(pDC->GetSafeHdc(),lpBlt,&rBounds,&rItemPos,
				m_lpImage,NULL,TRUE,0,TRUE);
		else 
		if (!m_FileName.IsEmpty())
		{
			LPIMAGE lpImage;
    		CReadImage read((LPSTR)&m_FileName,IDN_PP);

			if (lpImage = read.ReadImage())
			{
				bResult = DrawImage(pDC->GetSafeHdc(),lpBlt,&rBounds,
					&rItemPos,m_lpImage,NULL,TRUE,0,TRUE);
				DestroyImage (lpImage);
			}
		}
	 	EndSuperBlt( lpBlt );
	}
	pDC->LPtoHIMETRIC(&rSize);   // convert pixels to HIMETRIC            

	ProgressEnd();

    return bResult;
}   

BOOL CServerItem::OnGetExtent( 
DVASPECT dwDrawAspect, 
CSize& rSize)
{
    if (dwDrawAspect != DVASPECT_CONTENT)
        return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

    rSize = CSize(0, 0);
    
    // determine extent based on screen dc
    CClientDC dc(NULL);
    dc.SetMapMode(MM_ANISOTROPIC);
    CPoint ptStart(0, 0);

	CRect rBounds;
	get_bounds (rBounds);
    rSize.cx = rBounds.Width();
    rSize.cy = rBounds.Height();
    dc.LPtoHIMETRIC(&rSize); // convert pixels to HIMETRIC

    return TRUE;
}

#ifndef _DESIGNERFIX
#if _MFC_VER == 0x0250
BOOL CServerItem::OnSetData( LPFORMATETC pFormatEtc, LPSTGMEDIUM pStgMedium ) 
#else
BOOL CServerItem::OnSetData( LPFORMATETC pFormatEtc, LPSTGMEDIUM pStgMedium,
    BOOL bRelease)
#endif
{
	ASSERT(GetDocument()->m_lpImage);  //Assert for now ; actually could work w/o but not implemented
	BOOL bResult = FALSE;
	LPFRAME lpFrame = NULL;
               
	if (pFormatEtc->cfFormat == CF_DIB)
	{
		LPBITMAPINFOHEADER lpDIB = 
			(LPBITMAPINFOHEADER)GlobalLock(pStgMedium->hGlobal);
		lpFrame = DIBToFrame( (LPTR)lpDIB, YES );
		GlobalUnlock(pStgMedium->hGlobal);
	}
	else if (pFormatEtc->cfFormat == CF_BITMAP)
	{
		CWindowDC WindowDC(::AfxGetMainWnd()); 
		HBITMAP hBitmap = (HBITMAP)pStgMedium->hGlobal;
		HPALETTE hPal = lpBltScreen->hPal; 
		lpFrame = BitmapToFrame( WindowDC.GetSafeHdc(), hBitmap, hPal );
	}

	if (lpFrame)
	{
		LPIMAGE lpImage = CreateImage (GetDocument()->m_lpImage->CurFile ,
			lpFrame,NULL,NULL,
			GetDocument()->m_lpImage->FileType,
			GetDocument()->m_lpImage->DataType,
			GetDocument()->m_lpImage->DocumentType,
			GetDocument()->m_lpImage->ImageName);
		if (lpImage)
		{
			// Free Old Image and replace with the new image
			DestroyImage(GetDocument()->m_lpImage); 
			GetDocument()->m_lpImage = m_lpImage = lpImage;
			GetDocument()->GetActiveView()->OnInitialUpdate();
			bResult = TRUE;
		}
	}

	return (bResult);
}
#endif //_DESIGNERFIX

void CServerItem::OnOpen()  
// called when opening Full size
{
    COleServerItem::OnOpen();
}

BOOL CServerItem::OnRenderGlobalData(
// This is here to provide DIB presentations for Originally OLE1.0 PP objects
// Generally, we render all cached presentation in METAFILE format for
// our OLE2.0 Implementation ( thru the MFC support ).  However, this 
// was not always the case and there is no way to get Servers like Word 6 
// and Excel 5 to requery for our supported cache types.
LPFORMATETC lpFormatEtc, 
HGLOBAL* phGlobal)
{
    ASSERT(lpFormatEtc != NULL);

    if (lpFormatEtc->cfFormat != CF_DIB)
        return COleServerItem::OnRenderGlobalData(lpFormatEtc, phGlobal);

    BOOL bResult = TRUE;
    HGLOBAL hGlobal = NULL;

    ASSERT(*phGlobal == NULL);         

	ProgressBegin(1,0);

	if (m_lpImage)
	{
		CRect   rBounds;
	    LPTR    lpDIB;
	    LPFRAME lpFrame;
    	LPRECT  lpRect = NULL;

	    ImgGetImageRect(m_lpImage, &rBounds);
        if ( !ImgMultipleObjects( m_lpImage ) )
        { // If no objects, use a rectangular area of the base
            lpFrame = ImgGetBaseEditFrame(m_lpImage);
            lpRect = &rBounds;
        }
        else
        { // If objects, Combine and copy the object data into a new frame
            if ( !(lpFrame = ImgCopyFrame( m_lpImage, NULL, &rBounds,YES, NO)))
                // Error OLE ERROR_MEMORY
                return (FALSE);
            lpRect = NULL;
        }

        lpDIB = FrameToDIB( lpFrame, lpRect );
#ifdef WIN32
        hGlobal = GlobalHandle( lpDIB );
#else
        {
	    DWORD dwValue = GlobalHandle( HIWORD(lpDIB) );
    	hGlobal = (HGLOBAL)LOWORD( dwValue );
        }
#endif
        GlobalUnlock( hGlobal );
        if (!lpRect && lpFrame) 
            FrameClose( lpFrame );
	}
	else
	{
		// FILE based render Not Implemented yet!
		ASSERT(m_lpImage);
	}

	if (hGlobal == NULL)
        bResult = FALSE;
    else
        *phGlobal = hGlobal;
    
	ProgressEnd();

    return (bResult);
}

void CServerItem::OnShow()  
// called when opening Insitu
{
    COleServerItem::OnShow();
}

void CServerItem::OnUpdate(
COleServerItem* pSender,
LPARAM lHint, 
CObject* pHint, 
DVASPECT nDrawAspect)
{
	if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CUpdateArea)))
	{
		// CAUSING TOO MUCH REDRAW ON CONTAINER SIDE
		/* NotifyChanged(nDrawAspect); // for now...  */
		return;	// handled therefore return
	}

	COleServerItem::OnUpdate(pSender,lHint,pHint,nDrawAspect);
}


//******************************* EXPORTED Functions *****************************/





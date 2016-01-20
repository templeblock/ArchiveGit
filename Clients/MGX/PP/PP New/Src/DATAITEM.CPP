/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

********************************************************************************
********************************************************************************

                                dataitem.cpp

********************************************************************************
********************************************************************************

*/

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include <limits.h>
#include "svritem.h"
#include "dataitem.h"
#include "svrdoc.h"

/* ********************************* History ******************************** */
/* ******************************** Constants ******************************* */

#if 0
    #define IFDEBUG(s) {s;}
    #define IFCOPY(s) s
#else
    #define IFDEBUG(s)
    #define IFCOPY(s)
#endif

/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */

IMPLEMENT_DYNAMIC(CDataItem, COleServerItem)

/* ******************************* Local Data ******************************* */

PPOLE_DATA NEAR _oleData;
PPOLE_DATA::PPOLE_DATA()
{
	// Clipboard formats
	static char BASED_CODE szNative[] = "Native";
	cfNative = ::RegisterClipboardFormat(szNative);
	ASSERT(cfNative != NULL);
	static char BASED_CODE szOwnerLink[] = "OwnerLink";
	cfOwnerLink = ::RegisterClipboardFormat(szOwnerLink);
	ASSERT(cfOwnerLink != NULL);
	static char BASED_CODE szObjectLink[] = "ObjectLink";
	cfObjectLink = ::RegisterClipboardFormat(szObjectLink);
	ASSERT(cfObjectLink != NULL);

	// OLE 2.0 Clipboard formats
	static char BASED_CODE szPPFileName[] = "PPFileName";
	cfPPFileName = ::RegisterClipboardFormat(szPPFileName);
	ASSERT(cfPPFileName != NULL);
	static char BASED_CODE szEmbeddedObject[] = "Embedded Object";
	cfEmbeddedObject = ::RegisterClipboardFormat(szEmbeddedObject);
	ASSERT(cfEmbeddedObject != NULL);
	static char BASED_CODE szEmbedSource[] = "Embed Source";
	cfEmbedSource = ::RegisterClipboardFormat(szEmbedSource);
	ASSERT(cfEmbedSource != NULL);
	static char BASED_CODE szLinkSource[] = "Link Source";
	cfLinkSource = ::RegisterClipboardFormat(szLinkSource);
	ASSERT(cfLinkSource != NULL);
	static char BASED_CODE szObjectDescriptor[] = "Object Descriptor";
	cfObjectDescriptor = ::RegisterClipboardFormat(szObjectDescriptor);
	ASSERT(cfObjectDescriptor != NULL);
	static char BASED_CODE szLinkSourceDescriptor[] = "Link Source Descriptor";
	cfLinkSourceDescriptor = ::RegisterClipboardFormat(szLinkSourceDescriptor);
	ASSERT(cfLinkSourceDescriptor != NULL);
}

/* ******************************* Public Data ****************************** */

/* *************************** Private Functions **************************** */

LOCAL HGLOBAL render_format( LPIMAGE, RECT, UINT);

LOCAL HGLOBAL render_format( LPIMAGE lpImage, RECT rect, UINT wFormat /* 0 */)
{
    HGLOBAL hMem = NULL;
    LPTR lpDIB;
    LPFRAME lpFrame;
    LPRECT lpRect;

    if ((wFormat == Control.cfImage) ||
		(wFormat == Control.cfImage5Ptr) ||
    	(wFormat == Control.cfImagePP4) ||
        (wFormat == CF_PALETTE))
    {
        // Return a data handle as requested.
        if (wFormat == Control.cfImage5Ptr)
		{
			LPDWORD lpImagePtr;

			lpImagePtr = (LPDWORD)Alloc(sizeof(DWORD));
			if (!lpImagePtr)
			{
				Message(IDS_EMEMALLOC);
				return(NULL);
			}
			*lpImagePtr = (DWORD)lpImage;
			hMem = GlobalPtrHandle(lpImagePtr);
			GlobalUnlockPtr(lpImagePtr);
		}
		else
        // Return a data handle as requested.
        if (wFormat == Control.cfImage)
        {                                        
			FNAME szFileName;

			// create a base name for the clipboard filename
			FormFileName (szFileName, IDN_CLIPBOARD,"WINCLIP.PP5");

            // Write out the Clipboard Image to file     (for Internal PP Paste)
            if ( !ImgWriteClipOut( lpImage, szFileName, lpImage->DataType, IDN_PP, NO) )
            {
                return (NULL);
            }
    		// Remember the locations of the files we have cut or copied
    		// and use them when we render; only set these strings here
        	lstrcpy( Names.ClipImageFile, szFileName );
            hMem = ConstructMGXImage( szFileName);
        }
        else
    	if (wFormat == Control.cfImagePP4)
		{
            // Write out the Clipboard Image to file  
            if ( !ImgWritePP4ClipOut( lpImage, NULL, NULL, NULL, 
				lpImage->DataType ) )
            {
                Message(IDS_EMEMALLOC);
                return (NULL);
            }
            return (ConstructPP4MGXImage( (LPTR)Names.ClipImageFile,
				(LPTR)Names.ClipMaskFile ));
		}
        else
        if ( wFormat == CF_PALETTE)
        {
            hMem = (lpBltScreen->hPal?ConstructPalette(lpBltScreen->hPal):NULL);
        }
    }
	else
    if (wFormat == CF_METAFILEPICT)
	{
		hMem = CreateMetaFromImage( lpImage, rect );
	}
    else if (( wFormat == CF_DIB ) || ( wFormat == CF_BITMAP ))
    {
        if ( !ImgMultipleObjects( lpImage ) )
        { // If no objects, use a rectangular area of the base
            lpFrame = ImgGetBaseEditFrame(lpImage);
            lpRect = &rect;
        }
        else
        { // If objects, Combine and copy the object data into a new frame
            if ( !(lpFrame = ImgCopyFrame( lpImage, NULL/*lpBaseObject*/, &rect,
                YES/*bCrop*/, NO/*fSelected*/ )) )
                // Error OLE ERROR_MEMORY
                return (hMem);
            lpRect = NULL;
        }

        if ( wFormat == CF_DIB )
        {
            lpDIB = FrameToDIB( lpFrame, lpRect );
            hMem = GlobalPtrHandle( lpDIB );
            GlobalUnlockPtr( lpDIB );
        }
        else
        if ( wFormat == CF_BITMAP )
            hMem = FrameToBitmap( lpFrame, lpRect );

        if (!lpRect) 
            FrameClose( lpFrame );
    }
    else
    {
        ASSERT(FALSE);
        hMem = NULL;
    }

    return( hMem );
}

/* ***************************** Public Methods ***************************** */

/////////////////////////////////////////////////////////////////////////////
// CDataSource Methods
CDataSource::CDataSource(
CDataItem *pDataItem,
LPIMAGE lpImage)
{
	ASSERT(pDataItem); 
    ASSERT(lpImage);

	TRY
	{
		m_pDataItem = pDataItem; 
	    m_lpImage = lpImage;
	}
    CATCH_ALL(e)
	{
		ASSERT(FALSE);
		THROW_LAST();
	}
	END_CATCH_ALL
}

CDataSource::CDataSource(
CDataItem *pDataItem,
LPSTR lpFileName,
BOOL bExtName)
{
	ASSERT(pDataItem); 
    ASSERT(lpFileName);

	TRY
	{
		m_FileName = lpFileName;
		m_bExtName = bExtName;
		m_pDataItem = pDataItem; 
		m_lpImage = NULL;
	}
    CATCH_ALL(e)
	{
		ASSERT(FALSE);
		THROW_LAST();
	}
	END_CATCH_ALL
}

CDataSource::~CDataSource()
{
    if (m_lpImage)
        DestroyImage(m_lpImage);    

	if (m_pDataItem)
	{
		delete m_pDataItem;
		m_pDataItem = NULL;
	}
}

LPIMAGE CDataSource::ExtractImage()
{
    LPIMAGE lpRetImage = m_lpImage;
    m_lpImage = NULL;
    return lpRetImage;
}

BOOL CDataSource::OnRenderData( 
LPFORMATETC lpFormatEtc, 
LPSTGMEDIUM lpStgMedium )
{
	if	(!m_lpImage && (lpFormatEtc->cfFormat != _oleData.cfPPFileName)) // MAKE ONE!
	{
	    FRMTYPEINFO TypeInfo;
        FNAME szFileName;
		FormFileName (szFileName, IDN_CLIPBOARD,"WINCLIP.PP4");

		ITEMID      FileType;
	    ITEMID      DataType;   
	    LPOBJECT    lpObject;
		FNAME 		szDataFile;
		FNAME 		szMaskFile;

		szMaskFile[0] ='\0';
		if (m_bExtName)
		{
			if (LookupExtFileN(m_FileName.GetBuffer(m_FileName.GetLength()), 
				szDataFile, IDN_CLIPBOARD, NO))
			{
				if (GetPPFileType(szDataFile) != IDN_PP)
					LookupExtFileN(m_FileName.GetBuffer(m_FileName.GetLength()), 
						szMaskFile, IDN_MASK, NO);
			}
			else
				return (FALSE);
		}
		else
			lstrcpy(szDataFile, m_FileName.GetBuffer(m_FileName.GetLength()));

	    if (lpObject = ImgCreateClipInObject(NULL, szDataFile, szMaskFile))
		{
		    FileType = GetPPFileType(szDataFile);
		    FrameGetTypeInfo(ObjGetEditFrame(lpObject),&TypeInfo);
		    DataType  = GetSaveDataType(TypeInfo);
		    m_lpImage = CreateImage(szDataFile,NULL,lpObject,NULL,FileType,
				DataType,IMG_DOCUMENT,NULL);
		}
		else
		{
			return FALSE;
		}
	}

    if (lpFormatEtc->cfFormat != _oleData.cfEmbedSource &&
		lpFormatEtc->cfFormat != CF_BITMAP &&
        lpFormatEtc->cfFormat != CF_METAFILEPICT &&
        lpFormatEtc->cfFormat != CF_PALETTE)
        return COleDataSource::OnRenderData (lpFormatEtc,lpStgMedium);

	// implementation does not support extended layout
	if (lpFormatEtc->lindex != -1)
		return FALSE;

	ASSERT(m_lpImage);
	if (!m_lpImage)
		return (FALSE);

    if (lpFormatEtc->cfFormat == _oleData.cfEmbedSource)
	{
		ASSERT(m_pDataItem);
		if (!m_pDataItem)
			return (FALSE);

	   	ProgressBegin(1, PROGRESS_ID(IDS_UNDORENDER));
		TRY
		{
			m_pDataItem->OurGetEmbedSourceData(lpStgMedium);
		}
		CATCH_ALL(e)
		{
			ProgressEnd();
			return FALSE;
		}
		END_CATCH_ALL

		ProgressEnd();
		return (TRUE);
	}
	else
	{
	    HGLOBAL hGlobal;
		CRect rBounds;

		ProgressBegin(1,0);
	   	ImgGetImageRect(m_lpImage, &rBounds);
		hGlobal = render_format ( m_lpImage, rBounds, lpFormatEtc->cfFormat);
		ProgressEnd();

	    if (hGlobal)
	    {
	        lpStgMedium->tymed =  (lpFormatEtc->cfFormat == CF_METAFILEPICT) ?
				TYMED_MFPICT : TYMED_GDI;    
	        lpStgMedium->hGlobal = hGlobal;
			return TRUE;
	    }
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}


BOOL CDataSource::OnRenderGlobalData(
LPFORMATETC lpFormatEtc, 
HGLOBAL* phGlobal)
{
    ASSERT(lpFormatEtc != NULL);

    if (lpFormatEtc->cfFormat != CF_DIB &&
    	lpFormatEtc->cfFormat != _oleData.cfPPFileName &&
        lpFormatEtc->cfFormat != Control.cfImage5Ptr &&
        lpFormatEtc->cfFormat != Control.cfImage &&
        lpFormatEtc->cfFormat != Control.cfImagePP4)
        return COleDataSource::OnRenderGlobalData(lpFormatEtc, phGlobal);

    BOOL bResult = TRUE;
    HGLOBAL hGlobal = NULL;

    ASSERT(*phGlobal == NULL);         

	ProgressBegin(1,0);

	if (m_lpImage)
	{
		CRect rBounds;
	    ImgGetImageRect(m_lpImage, &rBounds);
		hGlobal = render_format ( m_lpImage, rBounds, lpFormatEtc->cfFormat);
	}
	else if (lpFormatEtc->cfFormat == _oleData.cfPPFileName)
    {
		PPFILENAME *ppFileName;
        LPSTR str;

		ppFileName = (PPFILENAME *)Alloc(sizeof(PPFILENAME));
        str = m_FileName.GetBuffer(256);
        lstrcpy(ppFileName->szFileName, str);
		ppFileName->bExtName = m_bExtName;
       
        hGlobal = GlobalPtrHandle(ppFileName);
        GlobalUnlock(hGlobal);
    }

	if (hGlobal == NULL)
	{
        bResult = FALSE;
	}
    else
        *phGlobal = hGlobal;
    
	ProgressEnd();

    return (bResult);
}

//////////////////////////////////////////////////////////////////////////
//  CDataItem Methods

CDataItem::CDataItem (
CServerDoc * pDoc, 
LPIMAGE lpImage,
LPRECT lpMaskRect, /*NULL*/
BOOL bDoingDragDrop /*FALSE*/) : COleServerItem (pDoc, TRUE /* bAutoDelete */)
{
	ASSERT(pDoc);
	ASSERT(lpImage);

    IncludeEmbedded(TRUE);

	// this clears any clipboard formats promised by the COleServerItem
	// namely CF_METAFILEPICT.  This is done to allow the CDataSource (we
	// are about to create it) to DELAY the actual render operation
	m_dataSource.Empty();
	m_bDoingDragDrop = bDoingDragDrop;
    m_pDataSource = new CDataSource (this,lpImage);

    // Create an object string to describe the masked area    
	CString ItemName;
	CreateItemName (lpMaskRect,ItemName);
    SetItemName((LPCSTR)ItemName.GetBuffer(ItemName.GetLength()));  
}

CDataItem::CDataItem (
CServerDoc * pDoc, 
LPSTR lpFileName,
BOOL bExtName,
LPRECT lpMaskRect, /*NULL*/
BOOL bDoingDragDrop /*FALSE*/) : COleServerItem(pDoc, TRUE /* bAutoDelete */)
{
    IncludeEmbedded(TRUE);

	// this clears any clipboard formats promised by the COleServerItem
	// namely CF_METAFILEPICT.  This is done to allow the CDataSource (we
	// are about to create it) to DELAY the actual render operation
	m_dataSource.Empty();

    // Create an object string to describe the masked area    
	CString ItemName;
	CreateItemName (lpMaskRect,ItemName);
    SetItemName((LPCSTR)ItemName.GetBuffer(ItemName.GetLength()));  

	m_bDoingDragDrop = bDoingDragDrop;
    m_pDataSource = new CDataSource (this,lpFileName,bExtName);
}

CDataItem::~CDataItem () 
{
	// prevent CDataSource attempting to delete when already deleted
	if (m_pDataSource)
		m_pDataSource->m_pDataItem = NULL;
}

COleDataSource* CDataItem::OnGetClipboardData(
BOOL bIncludeLink,
LPPOINT lpOffset,
LPSIZE lpSize)
{
    ASSERT_VALID(this);
    ASSERT(m_pDataSource);
    ASSERT(m_pDataSource->m_lpImage || !m_pDataSource->m_FileName.IsEmpty());

	m_pDataSource->Empty();
    TRY
    {
	    FORMATETC FormatEtc;
		STGMEDIUM stgMedium;
        if (m_bIncludeEmbedded)
        {
            if (Control.Retail)
            {
           		// CF_EMBEDDEDOBJECT 
               	SETDefFormatEtc(FormatEtc,_oleData.cfEmbedSource,TYMED_ISTORAGE);
           	    m_pDataSource->DelayRenderData(_oleData.cfEmbedSource,&FormatEtc);
    
           		// add CF_OBJECTDESCRIPTOR
           		GetObjectDescriptorData(lpOffset, lpSize, &stgMedium);
           		m_pDataSource->CacheData(_oleData.cfObjectDescriptor, &stgMedium);
            }
        }            
	    // CF_METAFILEPICT Format
	    SETDefFormatEtc(FormatEtc,CF_METAFILEPICT,TYMED_MFPICT);
	    m_pDataSource->DelayRenderData(CF_METAFILEPICT,&FormatEtc);

	    // DIB Format
	    SETDefFormatEtc(FormatEtc,CF_DIB,TYMED_HGLOBAL);
	    m_pDataSource->DelayRenderData(CF_DIB,&FormatEtc);

    	// Bitmap Format
	    SETDefFormatEtc(FormatEtc,CF_BITMAP,TYMED_GDI);
	    m_pDataSource->DelayRenderData(CF_BITMAP,&FormatEtc);

	    // Palette
	    if (lpBltScreen->hPal != NULL)
	    {
	        SETDefFormatEtc(FormatEtc,CF_PALETTE,TYMED_GDI);
	        m_pDataSource->DelayRenderData(CF_PALETTE,&FormatEtc);
	    }
	
		if (m_bDoingDragDrop)
		{
		    // MGX Image Format PP5 POINTER!
		    SETDefFormatEtc(FormatEtc,Control.cfImage5Ptr,TYMED_HGLOBAL);
		    m_pDataSource->DelayRenderData(Control.cfImage5Ptr, &FormatEtc);
		}

        if (!m_pDataSource->m_lpImage && !m_pDataSource->m_FileName.IsEmpty())
        {
		    // add CF_PPFILENAME
    		SETDefFormatEtc(FormatEtc,_oleData.cfPPFileName,TYMED_HGLOBAL);
        	m_pDataSource->DelayRenderData(_oleData.cfPPFileName,&FormatEtc);
        }

	    // MGX Image Format PP5
	    SETDefFormatEtc(FormatEtc,Control.cfImage,TYMED_HGLOBAL);
	    m_pDataSource->DelayRenderData(Control.cfImage, &FormatEtc);

	    // MGX Image Format PP4
	    SETDefFormatEtc(FormatEtc,Control.cfImagePP4,TYMED_HGLOBAL);
	    m_pDataSource->DelayRenderData(Control.cfImagePP4, &FormatEtc);
        if (Control.Retail)
        {
    		// add CF_LINKSOURCE if supporting links to pseudo objects
    		if (bIncludeLink && GetLinkSourceData(&stgMedium))
    		{
    			m_pDataSource->CacheData(_oleData.cfLinkSource, &stgMedium);
    
    			// add CF_LINKSOURCEDESCRIPTOR
    			GetObjectDescriptorData(lpOffset, lpSize, &stgMedium);
    			m_pDataSource->CacheData(_oleData.cfLinkSourceDescriptor, &stgMedium);
    		}
        }
    }
    CATCH_ALL(e)
    {
        THROW_LAST();
    }
    END_CATCH_ALL

    ASSERT_VALID(m_pDataSource);
    return m_pDataSource;
}

void CDataItem::OurGetEmbedSourceData(LPSTGMEDIUM lpStgMedium)
// This is called for Delayed Render of Embedded Data from CDataSource.
// The main difference between this and COleServerITems::GetEmbedSourceData
// is that in this routine we do not need create and lock down a storage.  
// Instead we us the one passed in !
{
	ASSERT_VALID(this);
	ASSERT(AfxIsValidAddress(lpStgMedium, sizeof(STGMEDIUM)));
	ASSERT(lpStgMedium->pstg != NULL);

	// setup for save copy as
	LPSTORAGE lpStorage = lpStgMedium->pstg;
	CServerDoc* pDoc = GetDocument();
	pDoc->m_bSameAsLoad = FALSE;
	pDoc->m_bRemember = FALSE;

	TRY
	{
		OnSaveEmbedding(lpStorage);
	}
	CATCH_ALL(e)
	{
		pDoc->m_bSameAsLoad = TRUE;
		pDoc->m_bRemember = TRUE;
		THROW_LAST();
	}
	END_CATCH_ALL

	pDoc->m_bSameAsLoad = TRUE;
	pDoc->m_bRemember = TRUE;

	// add it to the data source
	lpStgMedium->tymed = TYMED_ISTORAGE;
	lpStgMedium->pstg  = lpStorage;
	lpStgMedium->pUnkForRelease = NULL;
}

void CDataItem::Serialize(
CArchive& ar)
{ 
	ProgressBegin(1,0);

    if (ar.IsStoring())
    {                     
		ASSERT(m_pDataSource->m_lpImage);
		if (m_pDataSource->m_lpImage)
		{
	        ASSERT(m_pDataSource->m_lpImage);    
	        Store(ar,IDN_PP,m_pDataSource->m_lpImage,FALSE,FALSE);
		}
    }
    else
	{
        m_pDataSource->m_lpImage = Load(ar,IDN_PP);
		ASSERT(m_pDataSource->m_lpImage);
	}

	ProgressEnd();
}

/******************************* EXPORTED Functions *****************************/

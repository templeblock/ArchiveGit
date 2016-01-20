/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  DataItem.h

*****************************************************************************
*/

#ifndef _DATAITEM_H
#define _DATAITEM_H

#include "cmdlist.h"
#include "object.h"

/* ******************************* Macros ***************************** */

//Useful macro.
#define SETDefFormatEtc(fe, cf, med)\
    {\
    (fe).cfFormat=cf;\
    (fe).dwAspect=DVASPECT_CONTENT;\
    (fe).ptd=NULL;\
    (fe).tymed=med;\
    (fe).lindex=-1;\
    };

/* ********************************* Constants ****************************** */

/* *************************** Class Declarations ************************** */

class CServerDoc;
class CDataSource;
class CDataItem;

/* ******************************* Typedefs ******************************* */

/* ******************************* Classes ******************************** */
struct PPFILENAME
{
	FNAME szFileName;
	BOOL bExtName;
};

struct PPOLE_DATA
{
	// OLE 1.0 clipboard formats
	UINT    cfNative, cfOwnerLink, cfObjectLink, cfPPFileName;

	// OLE 2.0 clipboard formats
	UINT    cfEmbeddedObject, cfEmbedSource, cfLinkSource;
	UINT    cfObjectDescriptor, cfLinkSourceDescriptor;

	PPOLE_DATA();
};

class CDataSource : public COleDataSource
{
public:
	// Constructors
    CDataSource(CDataItem *pDataItem, LPIMAGE lpImage);
    CDataSource(CDataItem *pDataItem,LPTSTR lpFileName,BOOL bExtName = FALSE);
	// Destructors
    ~CDataSource();

	// implementation
	BOOL ExchangeCacheStorage(FORMATETC FormatEtc,LPSTGMEDIUM lpstgMedium);
	LPIMAGE ExtractImage (); // Recommend you don't use ADVANCED!
	const IMAGE FAR *GetImage() { return ((const IMAGE FAR *)m_lpImage); };
	const CDataItem *GetDataItem() {return (const CDataItem *)m_pDataItem;};
	BOOL IsOwner (const CServerDoc *pDoc);

	// overrides
    virtual BOOL OnRenderData( LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium ); 
    virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);

	// friends
	friend CDataItem;

private:
	CDataItem   	*m_pDataItem; // reference to creator

	// One Way
    LPIMAGE   m_lpImage;

	// Tuther Way
    CString   m_FileName;
    BOOL      m_bExtName;
};

//-----------------------------------------
// Use this class for Data Transfers 
//-----------------------------------------
class CDataItem : public COleServerItem                  
{
public:
	// Constructors
    DECLARE_DYNAMIC(CDataItem)
    CDataItem (CServerDoc * pDoc, LPIMAGE lpImage, LPRECT lpMaskRect = NULL, 
		BOOL bDoingDragDrop = FALSE); 
    CDataItem (CServerDoc * pDoc, LPTSTR lpFileName, BOOL bExtName, LPRECT lpMaskRect = NULL,
		BOOL bDoingDragDrop = FALSE);

	// Destructors
    ~CDataItem (); 

	// implementation
    CServerDoc* GetDocument() const // return type-safe container
        { return (CServerDoc*)COleServerItem::GetDocument(); };
	void IncludeEmbedded(BOOL bIncludeEmbedded) {m_bIncludeEmbedded = bIncludeEmbedded;};
	void OurGetEmbedSourceData(LPSTGMEDIUM lpStgMedium);

	// overrides
    virtual BOOL OnDraw(CDC* pDC, CSize& rSize) { return TRUE; };
    virtual COleDataSource* OnGetClipboardData(BOOL bIncludeLink,
        LPPOINT lpOffset,LPSIZE lpSize);
    virtual void Serialize(CArchive& ar);   // called for clipboard save

	// friend
    friend class CDataSource;

private:

	CDataSource *m_pDataSource;
	BOOL m_bDoingDragDrop;
	BOOL m_bIncludeEmbedded;
};

/* *************************** Inline Methods ****************************** */

inline BOOL CDataSource::IsOwner (const CServerDoc *pDoc)
{
	return (m_pDataItem && (m_pDataItem->GetDocument() == pDoc)); 
};

inline BOOL CDataSource::ExchangeCacheStorage(FORMATETC FormatEtc,
	LPSTGMEDIUM lpstgMedium)
// exchanges
{
// AFX_DATACACHE_ENTRY!!
// declared locally in MFC2.5 in OLEDOBJ2.CPP.  This is the only 
// I could get at the COleDataSource's Format Cache to change the 
// the status of single an entry.
struct AFX_DATACACHE_ENTRY
{
	FORMATETC m_formatEtc;
	STGMEDIUM m_stgMedium;
	DATADIR m_dataDir;
};
	BOOL bResult= FALSE;
	AFX_DATACACHE_ENTRY* pCacheEntry;
	if (pCacheEntry = Lookup(&FormatEtc, DATADIR_GET))
	{
		ASSERT(lpstgMedium!=NULL);
		::ReleaseStgMedium(&pCacheEntry->m_stgMedium);
		pCacheEntry->m_stgMedium = *lpstgMedium;
		bResult = TRUE;
	}
	return bResult;
}

/* *************************** Global Data ********************************* */

extern PPOLE_DATA NEAR _oleData;

/* *************************** Global Function ***************************** */


/* ************************************************************************* */

#endif // _DATAITEM_H




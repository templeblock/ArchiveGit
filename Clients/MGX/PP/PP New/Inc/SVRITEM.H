/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  svritem.h

*****************************************************************************
*/

#ifndef _SVRITEM_H
#define _SVRITEM_H

/* ******************************* Public Data ***************************** */
/* ********************************* Constants ****************************** */
/* *************************** Class Declarations ************************** */

class CServerDoc;
class CServerItem;
 
/* ******************************* Structs ******************************** */
/* ******************************* Classes ******************************** */

class CServerItem : public COleServerItem                  
{
public:        
	// Constructors
    CServerItem (CServerDoc * pDoc, LPIMAGE lpImage, LPRECT lpMaskRect = NULL); 
    CServerItem (CServerDoc * pDoc, LPTSTR lpFileName, LPRECT lpMaskRect = NULL); 

	// Destructors
    ~CServerItem (); 
    
	// implementation
    CServerDoc* GetDocument() const // return type-safe container
        { return (CServerDoc*)COleServerItem::GetDocument(); };

	// overrides
    virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
    virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
    virtual void OnOpen();          
    virtual BOOL OnRenderGlobalData(LPFORMATETC lpFormatEtc, HGLOBAL* phGlobal);
#ifndef _DESIGNERFIX
#if _MFC_VER == 0x0250
	virtual BOOL OnSetData( LPFORMATETC pFormatEtc, LPSTGMEDIUM pStgMedium ); 
#else
	virtual BOOL OnSetData(LPFORMATETC lpFormatEtc, LPSTGMEDIUM lpStgMedium,
		BOOL bRelease);
#endif
#endif //_DESIGNERFIX
    virtual void OnShow();          
	virtual void OnUpdate(COleServerItem* pSender,LPARAM lHint, 
		CObject* pHint, DVASPECT nDrawAspect);

protected:

	virtual void get_bounds (CRect &rBounds);
	void 	init_dataformats();
    LPIMAGE m_lpImage;  // Image belonging to the document we're attached to.
	CString m_FileName;

private:

};


/* *************************** Inline Methods ****************************** */
/* *************************** Global Data ********************************* */
/* *************************** Global Function ***************************** */

extern void CreateItemName (LPRECT lpMaskRect,CString & ItemName);
extern HANDLE CreateMetaFromImage( LPIMAGE lpImage, RECT rect );
extern int DrawImage(HDC hDC,LPBLT lpBlt,LPRECT lpSourceRect,
	LPRECT lpDestRect,LPIMAGE lpImageIn, LPRECT  lpRepairRect = NULL,
	BOOL fUseStretchDIB = FALSE, int LinesPerBlt = 0, BOOL bMetaDC = FALSE);

/* ************************************************************************* */

#endif // _SVRITEM_H



/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                  svrdoc.h

*****************************************************************************
*/

#ifndef _SVRDOC_H
#define _SVRDOC_H

#include "id.h"

/* ******************************* Public Data ***************************** */

/* ********************************* Constants ****************************** */

#define CF_PP_PRIVATE    _T("MGX_IMAGE")

/* *************************** Class Declarations ************************** */

class CDataItem;
class CServerItem;
class CServerDoc;
class CServerView;

/* ******************************* Typedefs ******************************* */

/* ******************************* Classes ******************************** */

class CServerDoc : public COleServerDoc
{
public:

    LPIMAGE m_lpImage;

	// For Inplace processing
	CSize   m_ZoomNum, 	 // used for Zoom factor while inplace
			m_ZoomDenom; // used for Zoom factor while inplace
	CSize   m_sizeDoc;	 // used for Doc Size while inplace
	CSummaryInfo m_SummaryInfo;

public:
	CServerDoc(LPIMAGE m_lpImage); // Proxy creation for Delayed render of Dataobject

protected: // create from serialization only
    CServerDoc();  
    ~CServerDoc();
    DECLARE_DYNCREATE(CServerDoc)
                   
public:

// Implementation 
    virtual ITEMID IsA();
    void CloseDocumentFile();
    CServerItem* GetEmbeddedItem();
    void FileUpdate();
    void FileSaveCopyAs();
	CServerView* GetActiveView();
	void UpdateImageSize();

	void OnClosePPDocumentEx(BOOL fRegenrating);

	void RevokeAndRegister(const TCHAR* pszPathName );

	void AttachImage(CImage *pImage);
	void SetImage(CImage *pImage);
	BOOL OnSavedDocument();
	void SetCmdList(LPCMDLIST lpCmdList);
	LPCMDLIST GetCmdList() { return m_lpCmdList; }

    // Function to Get the Root Compound Storage associated with each document
    LPSTORAGE GetRootCompoundStorage(BOOL *pbTempStg=NULL);
    // Function to Set the Root Compound Storage associated with each document
    void SetRootCompoundStorage(LPSTORAGE lpRootStg,BOOL bTempStg);
	BOOL SwapStorage(LPTSTR lpFileName);
	BOOL InitCompoundFile();
    // Function to Get the Root Compound Storage associated with each Image
    BOOL DoSummaryInfo();
    BOOL ReadSummaryInfo();
    BOOL WriteSummaryInfo();


	CImage *GetImage() { return m_lpImage; }

// Inplace methods
	void InPlaceInitialUpdate(CWnd *pParentWnd);
	void UpdateInPlaceDisplay(CWnd *pParentWnd);

// static methods to allow calls when there is no document
	static void ProcessStatus(CServerView *pActiveView,LPCTSTR lpHintStr);
	static void SetToolbar(CServerView *pActiveView, BOOL bShow = TRUE,
		int idRibbon = -1, DLGPROC lpRibbonProc = NULL);
	static void SetToolboxes(CServerView *pActiveView, BOOL bShow = TRUE);

// overrides
    virtual void DeleteContents ();                            
    virtual COleServerItem* OnGetEmbeddedItem( );
	virtual COleServerItem* OnGetLinkedItem( LPCTSTR lpszItemName );
    virtual void SetTitle(const TCHAR* pszTitle);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);

// implementation helpers overides
	virtual void LoadFromStorage();
	virtual void SaveToStorage(CObject* pObject = NULL);

// Insitu Overrides
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual void OnDocWindowActivate(BOOL bActivate);
	virtual void OnFrameWindowActivate(BOOL bActivate);
	virtual void OnSetItemRects(LPCRECT lpPosRect, LPCRECT lpClipRect);
	 
// Document i/o overrides
    virtual void OnCloseDocument();
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument( const TCHAR* pszPathName );
    virtual BOOL OnSaveDocument(const TCHAR* pszPathName);
    virtual BOOL SaveModified();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o


// Friends
    friend class CServerView;       
    friend class CServerItem;       
    friend class CDataItem;       
    
// Message Maps
	//-------------------------------------
	// Command UI update (in doccmds.cpp)
	//-------------------------------------
	afx_msg void OnUpdateCloseImage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileUpdate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveCopyAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);

	//-------------------------------------
	// Command targets (in doccmds.cpp)
	//-------------------------------------
	afx_msg void OnCloseImage();
	afx_msg void OnFileUpdate();
	afx_msg void OnFileSaveCopyAs();
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnPrint();

    DECLARE_MESSAGE_MAP()    

private:
	void set_bubble_hintstate (BOOL bActivate);
    LPIMAGE do_fastbitsload( ITEMID idFileType, LPTSTR lpFileName, int iDoZoom, 
        BOOL bCheckDuplicates, LPFRAME lpFastbitsPreview,LPRECT lpFastbitsGridRect);                               
    BOOL do_save(LPCTSTR lpFileName, BOOL bReplace = TRUE, BOOL bCommand = TRUE);  
	static void do_status(LPCTSTR lpString);

private:
    ITEMID m_DataType;
    ITEMID m_FileType;


	BOOL   m_bIPFrameActive;  // TRUE when InPlace Owner is active
	BOOL   m_bIPDeactivating; // TRUE when InPlace Doc is being deactivated

	BOOL   m_fReadFromOLE10;
	BOOL   m_fConvert;
	CLSID  m_TreatAsClsid;

	LPSTORAGE m_lpRootImageStorage;
    BOOL m_bTempStg; // TRUE if the Storage is Temporary
	LPCMDLIST m_lpCmdList;  // link back to command list
};

/* *************************** Inline Methods ****************************** */

inline CServerItem* CServerDoc::GetEmbeddedItem()
{
	return (CServerItem*)COleServerDoc::GetEmbeddedItem();
};

inline ITEMID CServerDoc::IsA() 
{
    return(IDN_PP);
};

inline void CServerDoc::RevokeAndRegister( const TCHAR* pszPathName )
{
	Revoke();
	#ifdef WIN32
	RegisterIfServerAttached(pszPathName, FALSE);
	#else
	RegisterIfServerAttached(pszPathName);
	#endif
   	AfxOleSetUserCtrl(TRUE);			
};

/* *************************** Global Data ********************************* */

/* *************************** Global Function ***************************** */

extern LPIMAGE Load(CArchive & ar, ITEMID FileType, BOOL bOle10 = FALSE);
extern void Store(CArchive & ar, ITEMID FileType, LPIMAGE lpImage,
	BOOL m_fReadFromOLE10 = FALSE, BOOL m_fConvert = FALSE );

/* ************************** Inline Functions ************************** */

/* ************************************************************************* */

#endif // _SVRDOC_H




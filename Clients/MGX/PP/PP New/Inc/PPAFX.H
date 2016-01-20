// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
//    This material is confidential and a trade secret.
//    Permission to use this work for any purpose must be obtained
//    in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081  
//-------------------------------------------------------------------------
//                            ppafx.h
//-------------------------------------------------------------------------

#ifndef __ppafx
#define __ppafx

#include "browser.h" 
#include "svrdoc.h"
#include "mdiframe.h"
#include "mail.h"

class CPictPubApp;
class CPPMultiDocTemplate;
class CPPOleTemplateServer;

#include "initthrd.h"

typedef struct _openinfo
{
	ITEMID			idCommand;
	LPCMD_PARMS		lpParms;
	int				PhotoCDResOverride;
} OPENINFO;

/* ************************* Constants ************************************ */
/* ************************* Types **************************************** */ 
class CPPOleTemplateServer : public COleTemplateServer
{
public:
	CPPOleTemplateServer()  {m_bResourcesLoaded = FALSE;};

protected:
    virtual CCmdTarget* OnCreateObject();

private:
	BOOL m_bResourcesLoaded;
};

class CPictPubApp : public CWinApp
{
public:

	// ----------------------------------------------------
	// Constructors and Destructors
	// ----------------------------------------------------
    CPictPubApp ();
    ~CPictPubApp ();

	// ----------------------------------------------------
	// StartUp Routines
	// ----------------------------------------------------
    AstralInit ();                                                      
	BOOL ActivatePreviousInstance();
	BOOL PreviousInstance();

	// ----------------------------------------------------
	// Helper Routines
	// ----------------------------------------------------
    CPPToolList *GetCustomBoxes();
	HWND Get_hActiveWindow(BOOL bUseAfxGetMainWnd=TRUE);
	HWND Get_hClientAstral();
	HWND Get_hWndAstral(BOOL bUseAfxGetMain=TRUE);
	HINSTANCE Get_hInstAstral(BOOL bUseAfxGetMain=TRUE);
	BOOL IsActiveDocInPlaceActive();
	CServerView *GetActiveView();
 	void   SetActiveView(CServerView *pViewWnd);
    void DocumentChanged (LPIMAGE lpImage = NULL);       
    CServerDoc *GetDocument (LPIMAGE lpImage); 
    CServerDoc *GetDocument (LPTSTR lpFileName);
    ATOM MyRegisterClass( CONST WNDCLASS *lpwc, UINT IconSmID = 0);
	DWORD GetMainThreadId() { return m_dwMainThreadId; }
	void OnSysColorChange();
	void SetInPlaceActiveState(BOOL bActivate) { m_bInPlaceActive = bActivate; }
	HINSTANCE GetResourceHandle();
 
	// ----------------------------------------------------
	// Profile routines
	// ----------------------------------------------------
	HKEY GetLocalRegistryKey();
	HKEY GetLocalSectionKey(LPCTSTR lpszSection);
	UINT GetLocalProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nDefault);
	int GetLocalProfileInt( LPTSTR lpKeyName, int nDefault );
	CString GetLocalProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault);
	UINT GetLocalProfileString( LPTSTR lpKeyName, LPTSTR lpDefault, LPTSTR lpReturnedString, int nSize);
	BOOL WriteLocalProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);
	BOOL WriteLocalProfileInt( LPTSTR lpKeyName, int nDefault );
	BOOL WriteLocalProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);
	BOOL WriteLocalProfileString( LPTSTR lpKeyName, LPTSTR lpString);
	BOOL WriteLocalProfileFileName( LPTSTR lpKeyName, LPTSTR lpFileName);

	// ----------------------------------------------------
	// Doc Template routines
	// ----------------------------------------------------
	POSITION GetFirstDocTemplate ();
	CDocTemplate * GetNextDocTemplate (POSITION &pos);

	// ----------------------------------------------------
	// Menu Setup and management routines
	// ----------------------------------------------------
	void SetupAppMainMenu(CMenu *pMenu);
	void SetupAppSharedMenu(CMenu *pMenu);
	void AddFileMainMenuItems(CMenu *pFileMenu);
	void AddFileSharedMenuItems(CMenu *pFileMenu);
	void AddImageMenuItems(CMenu *pImageMenu);
	void EnableObjectMenu(CMenu *pMenu);
	CMenu *GetSubMenu(CMenu *pMenu, int nPos);

	// ----------------------------------------------------
    // InHouse file i/o routines
	// ----------------------------------------------------
    BOOL AreWeDoingIO() {return(bPPAppFileIo);};
    void CloseDocumentFile(LPIMAGE lpImage);    
    CDocument* NewDocumentFile(LPIMAGE lpImage);
    CDocument* OpenDocumentFile( LPCTSTR lpszFileName,LPIMAGE lpImage, BOOL bMakeVisible = TRUE);
    CDocument* OpenDocumentFile( LPCTSTR lpszFileName, OPENINFO *pOpenInfo, BOOL bMakeVisible = TRUE);
    CDocument* OpenDocumentFileImmediate (LPCTSTR lpszFileName,OPEN_MODE OpenMode, LPCMSINFO pCmsInfo = NULL);    

	// ----------------------------------------------------
	// Update the Registration DBase information
	// ----------------------------------------------------
	void UpdateRegInfo();

	// ----------------------------------------------------
	// Clipboard File List Functions
	// ----------------------------------------------------
	LPTSTR GetClipboardFileName(LPTSTR lpFileName);
	BOOL  SetClipboardFileName(LPTSTR lpFileName, DWORD ClipboardID);
	void  AddClipboardFileReference(LPTSTR lpFileName);

	// ----------------------------------------------------
	// Overrides
	// ----------------------------------------------------
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file
	virtual BOOL OnIdle(long lCount);
	virtual BOOL SaveAllModified(); // save before exit
    virtual BOOL ExitInstance ();
    virtual BOOL InitInstance ();
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
   
	// ----------------------------------------------------
	// Message Map and Handlers
	// ----------------------------------------------------
    DECLARE_MESSAGE_MAP ();
    afx_msg void OnHelp();
    afx_msg void OnSwitchActiveColor();
    afx_msg void OnRevertColorSwatch();
    afx_msg void OnStatusInfo();
    afx_msg void OnViewSlime();
    afx_msg void OnShield();
    afx_msg void OnPalette();
    afx_msg void OnUpdatePalette(CCmdUI* pCmdUI);
    afx_msg void OnStatusMask();
    afx_msg void OnEditAlpha();
	afx_msg void OnPasteFromFile();
	afx_msg void OnUpdatePasteFromFile(CCmdUI* pCmdUI);
    afx_msg void OnToolShow();
    afx_msg void OnUpdateToolShow(CCmdUI* pCmdUI);
	afx_msg void OnAbout();
	afx_msg void OnUpdateAlwaysEnabled(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNeedImage(CCmdUI* pCmdUI);
    afx_msg void OnPasteAsNew();
	afx_msg void OnUpdatePasteAsNew(CCmdUI* pCmdUI);
    afx_msg void OnMacroPlay();
    afx_msg void OnMacroPlayBatch();
    afx_msg void OnMacroRecord();
	afx_msg void OnUpdateMacroPlay(CCmdUI* pCmdUI);
    afx_msg void OnMacroStop();
	afx_msg void OnUpdateMacroStop(CCmdUI* pCmdUI);
    afx_msg void OnMacroEdit();
    afx_msg void OnExit();
	afx_msg void OnUpdateExit(CCmdUI* pCmdUI);
    afx_msg void OnNew();
    afx_msg void OnOpen();
    afx_msg void OnImport();
    afx_msg void OnRevert();
	afx_msg void OnUpdateRevert(CCmdUI* pCmdUI);
    afx_msg void OnMonitorMap();
    afx_msg void OnUpdateMonitorMap();
    afx_msg void OnPrintSetup();
    afx_msg void OnCalibrateScan();
    afx_msg void OnCalibratePrint();
    afx_msg void OnScratchPad();
    afx_msg void OnScanSetup();
	afx_msg void OnUpdateScratchPad(CCmdUI* pCmdUI);
    afx_msg void OnStatusShow();
	afx_msg void OnUpdateStatusShow(CCmdUI* pCmdUI);
    afx_msg void OnTaskManShow();
	afx_msg void OnUpdateTaskManShow(CCmdUI* pCmdUI);
	afx_msg void OnObjManager();
	afx_msg void OnUpdateObjManager(CCmdUI* pCmdUI);
    afx_msg void OnArrangeIcons();
    afx_msg void OnCascade();
    afx_msg void OnTile();
    afx_msg void OnBHintsShow();
	afx_msg void OnUpdateBHintsShow(CCmdUI* pCmdUI);
    afx_msg void OnSHintsShow();
	afx_msg void OnUpdateSHintsShow(CCmdUI* pCmdUI);
    afx_msg void OnWHintsShow();
	afx_msg void OnUpdateWHintsShow(CCmdUI* pCmdUI);
    afx_msg void OnShowInfo();
	afx_msg void OnUpdateShowInfo(CCmdUI* pCmdUI);
    afx_msg void OnShowZoomBox();
	afx_msg void OnUpdateShowZoomBox(CCmdUI* pCmdUI);
    afx_msg void OnCloseAll();
    afx_msg void OnSavePositions();
    afx_msg void OnSizeUp();
    afx_msg void OnSizeDown();
    afx_msg void OnShapeUp();
    afx_msg void OnShapeDown();
    afx_msg void OnPref();
    afx_msg void OnHelpContextMode();
    afx_msg void OnHelpContext();
    afx_msg void OnHelpIndex();
    afx_msg void OnHelpUsing();
    afx_msg void OnHelpReadMe();
    afx_msg void OnHelpOnHelp();
	afx_msg void OnNewWindow();
	afx_msg void OnUpdateNewWindow(CCmdUI* pCmdUI);
	afx_msg void OnAcquire();
	afx_msg void OnUpdateAcquire(CCmdUI* pCmdUI);
	afx_msg void OnSendMail();
	afx_msg void OnUpdateSendMail(CCmdUI* pCmdUI);
	
	// ----------------------------------------------------
    // Friends
	// ----------------------------------------------------
    friend class CServerDoc;

private:

	void on_brush_size(int idCmd);
	void on_brush_shape(int idCmd);

protected:

    CPPOleTemplateServer m_PPserver; 

private:    

    BOOL     bPPAppFileIo; 
	CServerView *m_pActiveView;
	CPtrList *m_pClipboardFileList;

    // Mail enableing 
	CMapiMail *m_pMail;
    BOOL   m_bMailInitDone; 

	DWORD	m_dwMainThreadId;

    CInitializationThread *m_pInitThread;
    BOOL m_bCustoolsLoaded;
	BOOL m_bInPlaceActive;
};

class CPPMultiDocTemplate : public CMultiDocTemplate
{
public:

	// ----------------------------------------------------
	// Constructors and Destructors
	// ----------------------------------------------------
    CPPMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
        CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

	// ----------------------------------------------------
	// Registration stuff
	// ----------------------------------------------------
	UINT m_nIDOleEmbedding; 
	UINT m_nIDOleInPlaceServer;
	void OurSetServerInfo(UINT nIDOleEmbedding, UINT nIDOleInPlaceServer = 0,
		CRuntimeClass* pOleFrameClass = NULL, CRuntimeClass* pOleViewClass = NULL);

    virtual void CloseAllDocuments(BOOL bEndSession);
	CDocument* OpenDocumentFile(LPIMAGE lpImage,
        const TCHAR* pszPathName, BOOL bMakeVisible = TRUE);
	CDocument* OpenDocumentFile(const TCHAR* pszPathName, OPENINFO *pOpenInfo, BOOL bMakeVisible = TRUE);
    
private:

};

// ******************* Global Data ***************************************

extern CPictPubApp PictPubApp;
extern LPTSTR lpszAppClass;
extern LPTSTR lpszMDIChildClass;
extern LPTSTR lpszImageClass;

// ******************* Inlines *******************************************
// ***********************************************************************

#endif // __ppafx.h


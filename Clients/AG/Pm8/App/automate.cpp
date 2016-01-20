// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "automate.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "browser.h"
#include "clip.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// from wmf.cpp (wmf.h needs too much other stuff).
extern ERRORCODE CreateWMFFromMetaFilePict(const METAFILEPICT* pPict, LPCSTR pFileName);

/////////////////////////////////////////////////////////////////////////////
// CPrintMasterDispatch

IMPLEMENT_DYNCREATE(CPrintMasterDispatch, CCmdTarget)

CPrintMasterDispatch::CPrintMasterDispatch()
{
	EnableAutomation();

	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();
}

CPrintMasterDispatch::~CPrintMasterDispatch()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	AfxOleUnlockApp();
}

void CPrintMasterDispatch::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// delete the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CPrintMasterDispatch, CCmdTarget)
	//{{AFX_MSG_MAP(CPrintMasterDispatch)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPrintMasterDispatch, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CPrintMasterDispatch)
	DISP_FUNCTION(CPrintMasterDispatch, "RunArtGallery", RunArtGallery, VT_BSTR, VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IPrintMasterDispatch to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {83DC6B37-719A-11D1-9828-00A0246D4780}
static const IID IID_IPrintMasterDispatch =
{ 0x83dc6b37, 0x719a, 0x11d1, { 0x98, 0x28, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80 } };

BEGIN_INTERFACE_MAP(CPrintMasterDispatch, CCmdTarget)
	INTERFACE_PART(CPrintMasterDispatch, IID_IPrintMasterDispatch, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {83DC6B35-719A-11D1-9828-00A0246D4780}
//IMPLEMENT_OLECREATE2(CPrintMasterDispatch, "PrintMaster.Application", 0x83dc6b35, 0x719a, 0x11d1, 0x98, 0x28, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80)
IMPLEMENT_OLECREATE(CPrintMasterDispatch, "PrintMaster.Application", 0x83dc6b35, 0x719a, 0x11d1, 0x98, 0x28, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80)

/////////////////////////////////////////////////////////////////////////////
// CPrintMasterDispatch message handlers

BSTR CPrintMasterDispatch::RunArtGallery(long hParentWindow) 
{
	HWND hWndParent = (HWND)hParentWindow;
	CString strResult;

	CArtBrowserDialog Browser(CArtBrowserDialog::TYPE_ModalPicture, NULL);
	Browser.m_pDoc = NULL;
   Browser.m_pszLastPicture = NULL;
   Browser.m_pLastPicture = NULL;
	HGLOBAL hMetaFilePict = NULL;

	::EnableWindow(hWndParent, FALSE);

	CWnd* pMainWnd = AfxGetMainWnd();
	pMainWnd->SetForegroundWindow();
	pMainWnd->BringWindowToTop();

	if (Browser.DoModal() == IDOK)
	{
		// Create a temporary document to hold our graphic.
		CPmwDoc* pDoc = GET_PMWAPP()->NewHiddenDocument();

		if (pDoc != NULL)
		{
			pDoc->size_to_paper();
			// Create a graphic to draw with.
			GRAPHIC_CREATE_STRUCT* pGcs = pDoc->get_gcs();
			pDoc->SetImportPictureName(Browser.m_csChosenFileName, Browser.m_csChosenFriendlyName);
			pGcs->proof = TRUE;

			if (pDoc->create_chosen_picture() == ERRORCODE_None)
			{
				ASSERT(pDoc->object_list()->first_object() != NULL);

				TRY
				{
					// We now have our object. Setup the metafile.
					hMetaFilePict = CreateMetafilePictFromObjects(pDoc->object_list());
				}
				END_TRY
			}

			// Destroy the document. This frees the graphic as well.
			pDoc->OnCloseDocument();

			// Write the metafile to a WMF file so it can be imported.
			if (hMetaFilePict != NULL)
			{
				METAFILEPICT* pPict = (METAFILEPICT*)::GlobalLock(hMetaFilePict);
				if (pPict != NULL)
				{
					// Create the WMF file.
					if (Util::MakeDirectory(GetGlobalPathManager()->ExpandPath("[[U]]\\TMP")))
					{
						strResult = GetGlobalPathManager()->ExpandPath("[[U]]\\TMP\\AUTOMATE.WMF");
						if (CreateWMFFromMetaFilePict(pPict, strResult) != ERRORCODE_None)
						{
							strResult.Empty();
						}

						// Delete the contained metafile.
						if (pPict->hMF != NULL)
						{
							::DeleteMetaFile(pPict->hMF);
							pPict->hMF = NULL;
						}
					}

					// Unlock what we locked.
					::GlobalUnlock(hMetaFilePict);
				}

				// Free the memory.
				::GlobalFree(hMetaFilePict);
			}
		}
	}

	::EnableWindow(hWndParent, TRUE);
	::SetForegroundWindow(hWndParent);
	::BringWindowToTop(hWndParent);

	return strResult.AllocSysString();
}

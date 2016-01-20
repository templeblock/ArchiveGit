// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "PAHStudio.h"
#include "DlgProxy.h"
#include "PAHStudioDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPAHStudioDlgAutoProxy

IMPLEMENT_DYNCREATE(CPAHStudioDlgAutoProxy, CCmdTarget)

CPAHStudioDlgAutoProxy::CPAHStudioDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CPAHStudioDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CPAHStudioDlg)))
		{
			m_pDialog = reinterpret_cast<CPAHStudioDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CPAHStudioDlgAutoProxy::~CPAHStudioDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CPAHStudioDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CPAHStudioDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CPAHStudioDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_IPAHStudio to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {AFBB908F-6EDF-43DF-B640-9C127350088F}
static const IID IID_IPAHStudio =
{ 0xAFBB908F, 0x6EDF, 0x43DF, { 0xB6, 0x40, 0x9C, 0x12, 0x73, 0x50, 0x8, 0x8F } };

BEGIN_INTERFACE_MAP(CPAHStudioDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CPAHStudioDlgAutoProxy, IID_IPAHStudio, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {98036F45-00EC-4EC0-A5F2-1FBAC24C15D9}
IMPLEMENT_OLECREATE2(CPAHStudioDlgAutoProxy, "PAHStudio.Application", 0x98036f45, 0xec, 0x4ec0, 0xa5, 0xf2, 0x1f, 0xba, 0xc2, 0x4c, 0x15, 0xd9)


// CPAHStudioDlgAutoProxy message handlers

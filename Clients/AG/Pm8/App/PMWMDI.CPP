// pmwmdi.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "mainfrm.h"
#include "pmwmdi.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL RemoveUnneededMenus(CMenu* pMenu);

/////////////////////////////////////////////////////////////////////////////
// CPmwMDIChild

IMPLEMENT_DYNCREATE(CPmwMDIChild, CMDIChildWnd)

CPmwMDIChild::CPmwMDIChild()
{
}

CPmwMDIChild::~CPmwMDIChild()
{
}

BEGIN_MESSAGE_MAP(CPmwMDIChild, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPmwMDIChild)
	ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPmwMDIChild::ActivateFrame(int nCmdShow)
{
	BOOL fCascade = FALSE;

	if (nCmdShow == -1)
	{
		if (GET_PMWAPP()->GetOpenDocumentCount() > 1)
		{
         // We now have more than one document.
         // Show it non-maximized, and cascade the windows.
			fCascade = TRUE;
			nCmdShow = SW_NORMAL;
		}
		else
		{
			nCmdShow = SW_SHOWMAXIMIZED;
		}
	}

	CMDIChildWnd::ActivateFrame(nCmdShow);

	if (fCascade)
	{
		Util::SendWmCommand(AfxGetMainWnd(), ID_WINDOW_CASCADE, NULL, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPmwMDIChild message handlers

void CPmwMDIChild::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

#if 1
	od("OnMDIActivate: %d (act:%lx [%x], deact:%lx, this:%lx), Act:%lx [%x]\r\n",
					bActivate,
					pActivateWnd,
					(WORD)pActivateWnd->GetSafeHwnd(),
					pDeactivateWnd,
					this,
					GetActiveView(),
					(WORD)GetActiveView()->GetSafeHwnd());
#endif
      // Tell the main window we have a view (or not)
      CWnd  *pWnd = AfxGetMainWnd ();
		
      if ((bActivate == FALSE) && (pActivateWnd == NULL))
         pWnd->PostMessage (WM_VIEWSCHANGED, FALSE, 0);
      else
         pWnd->PostMessage (WM_VIEWSCHANGED, TRUE, 0);
}

BOOL CPmwMDIChild::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
		CWnd* pParentWnd, CCreateContext* pContext)
{
	if (!CMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	CMenu menu;

	menu.Attach(m_hMenuShared);
	RemoveUnneededMenus(&menu);
	menu.Detach();
	return TRUE;
}

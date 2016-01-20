// PMToolbar.h : main header file for the PMTOOLBAR DLL
//

#ifndef PMTOOLBAR_H
#define PMTOOLBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "resource.h"		// main symbols
#include "pmwtool.h"


CFileToolBar         m_wndFileBar;
CEdit1ToolBar        m_wndEdit1Bar;
CEdit2ToolBar        m_wndEdit2Bar;
CFormatToolBar			m_wndFormatBar;

extern CPalette* pOurPal;

extern "C" BOOL WINAPI LoadToolbar(CWnd* pParent, CPalette* pPalette);


extern "C" CToolBar* WINAPI GetToolbar(UINT nID);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PMTOOLBAR_H__3702A816_8EE9_11D2_8008_00A0C9C56CDE__INCLUDED_)

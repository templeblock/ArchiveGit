/*
// $Header: /PM8/App/BARMENU.CPP 1     3/03/99 6:03p Gbeddow $
//
// barmenu.cpp : implementation file
//
// $Log: /PM8/App/BARMENU.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:18:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:10   Fred
// Initial revision.
// 
//    Rev 1.1   23 Jul 1996 08:09:12   Fred
// Removed border -- need to test in 16 bit
// 
//    Rev 1.0   14 Mar 1996 13:24:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:26:28   FRED
// Initial revision.
// 
//    Rev 2.4   24 Jan 1996 11:40:44   JAY
// More string customizations
// 
//    Rev 2.3   26 Dec 1995 10:28:24   FRED
// Project structure changes for mac port
// 
//    Rev 2.2   20 Mar 1995 16:32:12   JAY
// Undid last change. Added PVCS header.
*/

#include "stdafx.h"
#include "system.h"
#include "pmw.h"
#include "menutype.h"
#include "barmenu.h"
#include "pmwcfg.h"

#ifdef _WIN32
#include <..\src\afximpl.h>
#else
#include <auxdata.h>
#endif
#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarMenu

CBarMenu::CBarMenu()
{
}

CBarMenu::~CBarMenu()
{
}

/*
// The definition for the bar menu entry.
*/

#pragma pack(1)

typedef struct BARMENU_ENTRY far *BARMENU_ENTRY_PTR;

struct far BARMENU_ENTRY
{
	WORD		type;
	WORD		id;
	DWORD		style;
	char		text[1];			// Variable length text follows

/*
// Return the next entry after this one.
*/

	BARMENU_ENTRY_PTR next(VOID);

/*
// Return the size in bytes of this entry.
*/

	uint16	size(VOID);

/*
// Return the size of the text string.
*/

	uint16	text_size(VOID)
		{ return strlen(text) + 1; }

};

PRIVATE CHAR font_name[] = "MS Sans Serif";

typedef struct
{
	DWORD	lStyle;
	BYTE	bNumberOfItems;
	WORD	x;
	WORD	y;
	WORD	cx;
	WORD	cy;
	char	szMenuName[1];				/* Always "" for us */
	char	szClassName[1];			/* Always "" for us */
	char	szCaption[1];				/* Always "" for us */
/* 
// If we want to set the font, use the following,
// and add DS_SETFONT to lStyle.
*/
	WORD	wPointSize;
	char	szFaceName[sizeof(font_name)];
} DIALOG_HEADER, far *DIALOG_HEADER_PTR;

typedef struct
{
	WORD	x;
	WORD	y;
	WORD	cx;
	WORD	cy;
	WORD	wID;
	DWORD	lStyle;
	BYTE	bClass;
	char	text[1];
} CONTROL_DATA, far *CONTROL_DATA_PTR;

#pragma pack()

/**************************/
/* Bar Menu Entry Methods */
/**************************/

/*
// Return the size of this entry.
*/

uint16 BARMENU_ENTRY::size()
{
/*
// The [1] on the text field conveniently counts the null-terminator of the
// 'text' string.
*/
	return sizeof(*this) + strlen(text);
}

/*
// Return the next bar entry given the current bar entry.
*/

BARMENU_ENTRY_PTR BARMENU_ENTRY::next()
{
/* Catch as catch can. */

	if (type == BM_END)
	{
		return NULL;
	}

/*
// Calculate the next one.
*/

	return (BARMENU_ENTRY_PTR) (((LPBYTE)this) + size());
}

/********************/
/* Bar Menu Methods */
/********************/

/*
// Try to create a bar menu from a menu definition.
*/

BOOL CBarMenu::create_from_menu(CWnd* pParentWnd, UINT nIDMenu, UINT nStyle, UINT nID)
{
#if 1
/* We never use this, so just comment it out. */
	return FALSE;
#else
/*
// Most of this is copied from CDialogBar::Create.
// But we use CreateDialogIndirect, and we create the template ourselves.
*/

	ASSERT(pParentWnd != NULL);

	HINSTANCE hInst = AfxGetResourceHandle();

/* Find the resource. */

	HRSRC hRsrc;

	if ((hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(nIDMenu), MAKEINTRESOURCE(RT_BARMENU))) == NULL)
	{
		return FALSE;
	}

/* Load the resource. */

	HGLOBAL hGlbl;

	if ((hGlbl = ::LoadResource(hInst, hRsrc)) == NULL)
	{
		return FALSE;
	}

/* Lock the resource. */

	BARMENU_ENTRY_PTR pEntries;

	if ((pEntries = (BARMENU_ENTRY_PTR)::LockResource(hGlbl)) == NULL)
	{
		::FreeResource(hGlbl);
		return FALSE;
	}

/*
// Translate the resource into a dialog template.
*/

/* Step #1 - count the entries in the menu. */

	BARMENU_ENTRY_PTR pEntry;
	SHORT count;
	uint16 text_size = 0;

	for (pEntry = pEntries, count = 0; pEntry->type != BM_END; pEntry = pEntry->next(), count++)
	{
	/* Also compute the size of all the text. */
		text_size += pEntry->text_size();
	}

/* Step #2 - allocate the memory we will need for the dialog template. */

	LPBYTE pTemp = NULL;

	uint16 total_size = sizeof(DIALOG_HEADER)
 						+ text_size
 						+ count*(sizeof(CONTROL_DATA)-1 + 1);

	TRY
		pTemp = new BYTE[total_size];
	END_TRY

	if (pTemp == NULL)
	{
		UnlockResource(hGlbl);
		FreeResource(hGlbl);
		return FALSE;
	}

/* Step 3 - Construct the dialog template header. */

	DIALOG_HEADER_PTR pHead = (DIALOG_HEADER_PTR)pTemp;

	pHead->lStyle = WS_CHILD | /*WS_BORDER |*/ DS_SETFONT;
	pHead->bNumberOfItems = (BYTE)count;
	pHead->x = 0;
	pHead->y = 0;
	pHead->cx = 68;
	pHead->cy = 200;
	pHead->szMenuName[0] = '\0';
	pHead->szClassName[0] = '\0';
	pHead->szCaption[0] = '\0';
	pHead->wPointSize = 8;
	strcpy(pHead->szFaceName, font_name);

/* Step 4 - Add entries for each control. */

	SHORT current_y = 5;
	LPBYTE pControlMem = pTemp + sizeof(*pHead);

	for (pEntry = pEntries; pEntry->type != BM_END; pEntry = pEntry->next())
	{
		CONTROL_DATA_PTR pControl = (CONTROL_DATA_PTR)pControlMem;

	/* Insert this control. */

		pControl->x = 5;
		pControl->y = current_y;
		pControl->wID = pEntry->id;
		pControl->lStyle = WS_CHILD | WS_VISIBLE | (DWORD)pEntry->style;
		pControl->bClass = 0x80 | (BYTE)pEntry->type;

		switch (pEntry->type)
		{
			case BM_BUTTON:
			{
				pControl->cx = 55;
				pControl->cy = 14;

				current_y += 20;
				break;
			}
			case BM_EDIT:
			{
				pControl->cx = 55;
				pControl->cy = 14;

				current_y += 20;
				break;
			}
			case BM_STATIC:
			{
				pControl->cx = 55;
				pControl->cy = 8;			/* Full font height. */

				current_y += 15;
				break;
			}
			case BM_LISTBOX:
			{
				pControl->cx = 55;
				pControl->cy = 14;

				current_y += 20;
				break;
			}
			case BM_SCROLLBAR:
			{
				pControl->cx = 55;
				pControl->cy = 14;

				current_y += 20;
				break;
			}
			case BM_COMBOBOX:
			{
				pControl->cx = 55;
				pControl->cy = 40;

				current_y += 20;
				break;
			}
			default:
			{
			/* Shouldn't happen! */
				break;
			}
		}

		strcpy(pControl->text, pEntry->text);

	/* Store the "mis-documented" cbCreateInfo size field. */

		SHORT len = strlen(pControl->text);
		LPBYTE pByte = (LPBYTE)pControl->text + len + 1;
		
		*pByte = 0;

	/* Advance to next control. */

		pControlMem = pByte + 1;
	}

/* Step 5 - Get rid of the resource. We no longer need it. */

	UnlockResource(hGlbl);
	FreeResource(hGlbl);

/* Step 6 - Do the dialog box create. */

	_AfxHookWindowCreate(this);
	HWND hWnd = ::CreateDialogIndirect(hInst, pTemp,
		pParentWnd->GetSafeHwnd(), NULL);
	if (!_AfxUnhookWindowCreate())
		PostNcDestroy();        // cleanup if Create fails too soon

/* Free the template we created. */

	delete [] pTemp;

/*
// Step 7 - Finish up.
// The rest of this code is from MFC.
*/

	if (hWnd == NULL)
		return FALSE;
	ASSERT(hWnd == m_hWnd);

	// Dialog template MUST specify that the Dialog
	// is an invisible child window

	_AfxSetDlgCtrlID(m_hWnd, nID);

	CRect rect;
	GetWindowRect(&rect);
#ifdef _WIN32
	m_sizeDefault = rect.Size();    // set fixed size
#else
	m_sizeFixedLayout = rect.Size();    // set fixed size
#endif

	// OR in the control bar sub-style
	::SetWindowLong(m_hWnd, GWL_STYLE, GetStyle() | nStyle);

	// initialize VBX controls etc
	if (!ExecuteDlgInit(MAKEINTRESOURCE(nID)))
		return FALSE;

	// force the size to zero - resizing bar will occur later
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);

	return TRUE;
#endif
}
	
/*
// Create a bar menu.
*/

BOOL CBarMenu::Create(CWnd* pParentWnd, UINT nIDMenu, UINT nStyle, UINT nID)
{
	m_nIDMenu = nIDMenu;
	if (CDialogBar::Create(pParentWnd, nIDMenu, nStyle, nID))
	{
		// MLSTODO: Just take off WS_BORDER in app studio if it works for 16-bit
		SetWindowLong(GetSafeHwnd(), GWL_STYLE, GetStyle() & ~WS_BORDER);
		GetConfiguration()->ReplaceDialogText(this);
		return TRUE;
	}

/* Try the menu. */

	if (create_from_menu(pParentWnd, nIDMenu, nStyle, nID))
	{
		GetConfiguration()->ReplaceDialogText(this);
		return TRUE;
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CBarMenu, CDialogBar)
	//{{AFX_MSG_MAP(CBarMenu)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarMenu message handlers

LRESULT CBarMenu::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd;

	if ((hWnd = ::ChildWindowFromPoint(m_hWnd, CPoint(lParam))) == NULL)
	{
		return 0;
	}
	else if (hWnd != m_hWnd)
	{
		UINT nID = _AfxGetDlgCtrlID(hWnd);
		if (nID != 0 && nID != (UINT)-1)
			return HID_BASE_COMMAND+nID;
	}

/* Default action. */

	return HID_BASE_RESOURCE+m_nIDMenu;				// What we were created with.
}

void CBarMenu::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CDialogBar::OnUpdateCmdUI(pTarget, FALSE);
//	CDialogBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);
}

BOOL CBarMenu::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CDialogBar::PreCreateWindow(cs))
	{
		return FALSE;
	}
#ifdef WIN32
	m_dwStyle &= ~(CBRS_BORDER_ANY);
	m_dwStyle |= CBRS_BORDER_TOP;
#endif
	return TRUE;
}

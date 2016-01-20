/*
// $Workfile: CDRAWCON.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDRAWCON.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:25:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:12   FRED
// Initial revision.
// 
//    Rev 2.4   20 Jun 1995 15:51:54   JAY
// Now handles (by passing through) some private MFC message.
// 
//    Rev 2.3   06 Jun 1995 16:54:34   JAY
// Project previews in the album.
// 
//    Rev 2.2   06 Jun 1995 10:12:22   JAY
// Moved more stuff over from CATALOG.
// 
//    Rev 1.6   19 Jan 1995 14:21:10   JAY
// New free pictures!
// 
//    Rev 1.5   23 Dec 1994 11:05:56   JAY
// Moved Breathe() method to here.
// 
//    Rev 1.4   23 Dec 1994 10:23:50   JAY
// CDrawException.
// 
//    Rev 1.3   01 Dec 1994 15:01:10   JAY
// 
//    Rev 1.2   15 Nov 1994 08:35:40   JAY
// Support for multiple palettes.
// 
//    Rev 1.1   03 Nov 1994 15:53:06   JAY
//  
// 
//    Rev 1.8   02 Nov 1994 15:09:14   JAY
// Support for palettes; cleanup in CCGMItemData::OnDelete
// 
//    Rev 1.7   27 Oct 1994 14:50:40   FRED
// Removed state stuff, Push, and Pop.
// Added new stuff for painting.
// 
// 
//    Rev 1.6   24 Oct 1994 09:16:44   FRED
// Removed ASSERT(m_pDC == NULL) in state destructor.
// 
// Added Init(CWnd*) methods.
// 
// 
//    Rev 1.5   11 Oct 1994 10:29:22   JAY
// Fixed a GP.
// 
//    Rev 1.4   11 Oct 1994 08:38:42   JAY
// Reimplemented CDrawContext with states
// 
//    Rev 1.3   10 Oct 1994 17:43:14   JAY
//  
// 
//    Rev 1.2   14 Sep 1994 10:07:20   JAY
// Toggle support in draw context.
// 
//    Rev 1.1   31 Aug 1994 16:01:30   FRED
// Added GetDC() and ReleaseDC() methods.
// 
// 
//    Rev 1.0   30 Aug 1994 14:59:24   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "cdrawcon.h"
#include "cupdlist.h"

#include <afxpriv.h>

extern CPalette* pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CSharedPalette

CPalette* CSharedPalette::GetPalette(void)
{
#if 0
	if (m_pPalette == NULL)
	{
		TRY
		{
			m_pPalette = new CPalette;
		}
		END_TRY

		if (m_pPalette != NULL)
		{
		// Fill out the palette entries, etc.
		}
	}
	return m_pPalette;
#endif
	return pOurPal;
}

/////////////////////////////////////////////////////////////////////////////
// CDrawContext

// The palette.
CSharedPalette near CDrawContext::m_GlobalPalette;
CWordArray near CDrawContext::m_PushedPalettes;

CDrawContext::CDrawContext()
{
	UseDC(NULL);
	m_pOldPalette = NULL;
}

CDrawContext::~CDrawContext()
{
	ASSERT(m_nDCCount == 0);
	ASSERT(m_nDestinationDCLevel == 0);
}

void CDrawContext::UseDC(CDC* pDC)
{
	if (pDC != NULL)
	{
		Init(pDC);				// Set m_info, etc.
   }

	m_pDC = pDC;
	m_pDestinationDC = NULL;
	m_pSourceDC = NULL;
	m_pClipRect = NULL;
	m_pWnd = NULL;
	m_nDCCount = 0;
	m_nDestinationDCLevel = 0;
}

CDC* CDrawContext::GetDC(void)
{
	if (m_pDC != NULL)
	{
		if (m_nDCCount != 0)
		{
			m_nDCCount++;
		}
	}

	else
	{
		ASSERT(m_pWnd != NULL);
		ASSERT(m_nDCCount == 0);
		
		m_pDC = m_pWnd->GetDC();
		m_nDCCount = 1;
	}

	return m_pDC;
}

void CDrawContext::ReleaseDC(void)
{
	if (m_nDCCount != 0)
	{
		ASSERT(m_pDC != NULL);
		
		if (--m_nDCCount == 0)
		{
			m_pWnd->ReleaseDC(m_pDC);
			m_pDC = NULL;
		}
	}
}

CDC* CDrawContext::MakeCompatibleDC(void)
{
	BOOL fSuccess = FALSE;
	CDC* pCompatibleDC = NULL;
	
	ASSERT(m_pDC != NULL);
	
	if (m_pDC != NULL)
	{
		TRY
		{
			pCompatibleDC = new CDC;
			ASSERT(pCompatibleDC != NULL);
			
			if (pCompatibleDC != NULL)
			{
				fSuccess = pCompatibleDC->CreateCompatibleDC(m_pDC);
			}
		}
		END_TRY
	}
	
	if (!fSuccess)
	{
		delete pCompatibleDC;
		pCompatibleDC = NULL;
	}
	
	return pCompatibleDC;
}

CDC* CDrawContext::GetDestinationDC(BOOL fIncrement /*=FALSE*/)
{
	ASSERT(m_pDC != NULL);
	
	if (m_pDC != NULL)
	{
		if (fIncrement)
		{
			m_nDestinationDCLevel++;
		}
		
		if (m_nDestinationDCLevel > 0)
		{
			if (m_pDestinationDC == NULL)
			{
				m_pDestinationDC = MakeCompatibleDC();
			}
			
			return m_pDestinationDC;
		}
	}
		
	return m_pDC;
}

void CDrawContext::ReleaseDestinationDC(BOOL fDecrement /*=FALSE*/)
{
	if (fDecrement)
	{
		ASSERT(m_nDestinationDCLevel > 0);
		m_nDestinationDCLevel--;
	}
}

CDC* CDrawContext::GetSourceDC(void)
{
	if (m_pSourceDC == NULL)
	{
		m_pSourceDC = MakeCompatibleDC();
	}
			
	return m_pSourceDC;
}

void CDrawContext::BeginPaint(CDrawState* pState)
{
	DrawState(pState);
	
	GetDC();
	ASSERT(m_pDC != NULL);
	
	ASSERT(m_nDestinationDCLevel == 0);
	m_nDestinationDCLevel = 0;
	
	ASSERT(m_pDestinationDC == NULL);
	ASSERT(m_pSourceDC == NULL);
}

void CDrawContext::EndPaint(void)
{
	DrawState(NULL);
	
	ASSERT(m_pDC != NULL);
	ReleaseDC();
	
	ASSERT(m_nDestinationDCLevel == 0);
	
	delete m_pDestinationDC;
	m_pDestinationDC = NULL;
	
	delete m_pSourceDC;
	m_pSourceDC = NULL;
}

void CDrawContext::RegisterToggle(TOGGLE_FUNC pFunc, LPVOID pData)
{
	CToggleElement Element;

	Element.m_pFunction = pFunc;
	Element.m_pData = pData;

	m_ToggleArray.Add(Element);
}

void CDrawContext::UnregisterToggle(TOGGLE_FUNC pFunc, LPVOID pData /*=NULL*/)
{
	CToggleElement Element;

	Element.m_pFunction = pFunc;
	Element.m_pData = pData;

	m_ToggleArray.Remove(Element);
}

/* Invoke all toggles in the array. */

void CDrawContext::Toggle(BOOL fTurnOn, const CRect* pcrArea)
{
	if (m_nDestinationDCLevel == 0)
	{
		int nSize = m_ToggleArray.GetSize();
	
		if (nSize != 0)
		{
			CDC* pDC = GetDC();
	
			int nContext;
			if ((nContext = pDC->SaveDC()) != 0)
			{
				for (int i = 0; i < nSize; i++)
				{
					const CToggleElement* pElement = m_ToggleArray.GetAt(i);
					ASSERT(pElement != NULL);
	
				/* Invoke the toggle with the initial passed data. */
	
					pElement->m_pFunction(this, fTurnOn, pcrArea, pElement->m_pData);
				}
				pDC->RestoreDC(nContext);
			}
			ReleaseDC();
		}
	}
}

void CDrawContext::Invalidate(CInterfaceObject* pObject, CUpdateList* pMaskUpdate /*=NULL*/, CUpdateList* pImageUpdate /*=NULL*/)
{
	ASSERT(m_pWnd != NULL);

	CRect crBounds(0, 0, 0, 0);

	if (pMaskUpdate != NULL)
	{
		pMaskUpdate->Bounds(crBounds);
	}

	if (pImageUpdate != NULL)
	{
		pImageUpdate->Bounds(crBounds);
	}

	if (!crBounds.IsRectEmpty())
	{
		m_pWnd->InvalidateRect(&crBounds);
	}
}

void CDrawContext::Window(CWnd* pWnd)
{
	m_pWnd = pWnd;
}

CWnd* CDrawContext::Window(void)
{
	return m_pWnd;
}

void CDrawContext::DC(CDC* pDC)
{
	m_pDC = pDC;
}

CDC* CDrawContext::DC(void)
{
	return m_pDC;
}

void CDrawContext::DrawState(CDrawState* pDrawState)
{
	m_pDrawState = pDrawState;
}

CDrawState* CDrawContext::DrawState(void)
{
	return m_pDrawState;
}

void CDrawContext::ClipRect(LPRECT pcrClip)
{
	m_pClipRect = pcrClip;
}

LPRECT CDrawContext::ClipRect(void)
{
	return m_pClipRect;
}

void CDrawContext::BeginDraw(CDC* pDC, LPRECT pcrClip /*=NULL*/)
{
	DC(pDC);
	ClipRect(pcrClip);

// Currently, we assume that BeginDraw/EndDraw pairs are only called one at
// a time. If this needs to change, then... change it!

	ASSERT(m_pOldPalette == NULL);

	CPalette* pPalette = Palette();

	if (pPalette != NULL)
	{
		m_pOldPalette = pDC->SelectPalette(pPalette, FALSE);
		pDC->RealizePalette();
	}
}

void CDrawContext::EndDraw(void)
{
/* Restore the old palette if we have one to restore. */

	if (m_pOldPalette != NULL)
	{
		ASSERT(m_pDC != NULL);
		if (m_pDC != NULL)
		{
			m_pDC->SelectPalette(m_pOldPalette, FALSE);
		}
		m_pOldPalette = NULL;
	}

	DC(NULL);
	ClipRect(NULL);
}

void CDrawContext::Init(CDC* pDC)
{
	m_info.Init(pDC);
}

void CDrawContext::Init(CWnd* pWnd)
{
	ASSERT(pWnd != NULL);
	
	if (pWnd != NULL)
	{	
		CDC* pDC = NULL;

		TRY
		{
			pDC = pWnd->GetDC();
			ASSERT(pDC != NULL);
			
			if (pDC != NULL)
			{
				Init(pDC);
				
				pWnd->ReleaseDC(pDC);
				pDC = NULL;
				
				m_pWnd = pWnd;
			}
		}
		CATCH_ALL(e)
		{
			if (pDC != NULL)
			{
				pWnd->ReleaseDC(pDC);
				pDC = NULL;
			}
		}
		END_CATCH_ALL
	}
}

CRasterInfo& CDrawContext::Info(void)
{
	return m_info;
}

/////////////////////////////////////////////////////////////////////////////
// Palette support in CDrawContext.

CPalette* CDrawContext::Palette(void)
{
	int nCount = m_PushedPalettes.GetSize();

//	TRACE("Return palette (%d)\n", nCount);

	if (nCount == 0)
	{
		return m_GlobalPalette.GetPalette();
	}
	else
	{
		return CPalette::FromHandle((HPALETTE)m_PushedPalettes.GetAt(nCount-1));
	}
}

void CDrawContext::PushPalette(HPALETTE hPalette)
{
	m_PushedPalettes.Add((WORD)hPalette);
}

void CDrawContext::PopPalette(void)
{
	int nCount = m_PushedPalettes.GetSize();
	ASSERT(nCount != 0);

	if (nCount != 0)
	{
	/* Remove last pallette pushed. */
		m_PushedPalettes.RemoveAt(nCount-1);
	}
}

/*
// Breathe during drawing.
*/

BOOL CDrawContext::Breathe(void)
{
	MSG msg;
	HWND hwnd = Window()->GetSafeHwnd();

	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{       
	/* See if it's a casual mouse move or other skippable event. */

		if (msg.message != WM_TIMER
			 && msg.message != WM_NCMOUSEMOVE
			 && (msg.message != 0x118)		// undocumented WM_SYSTIMER
			 && (msg.message != WM_MOUSEMOVE
				  || (msg.wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
			 && (msg.message != WM_PAINT
				  || hwnd == msg.hwnd)	// This check done by semaphore in Draw().
			 && (msg.message != WM_KEYDOWN
				  || !(msg.wParam == VK_SHIFT
						 || msg.wParam == VK_CONTROL))
			 && (msg.message != WM_KEYUP
				  || msg.wParam == VK_MENU)
			 && msg.message != WM_SETMESSAGESTRING)
		{
			TRACE("Interrupt for %x (%d) [%u vs. %u]\n", msg.message, msg.wParam, msg.hwnd, hwnd);
			return TRUE;
		}

	/* Pump the message through. */

//		TRACE("Pump msg %x (%d) [%u vs. %u]\n", msg.message, msg.wParam, msg.hwnd, hwnd);
		AfxGetApp()->PumpMessage();
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
// CToggleArray

CToggleArray::CToggleArray()
{
}

CToggleArray::~CToggleArray()
{
	RemoveAll();
}

/*
// Return the number of elements in the array.
*/

int CToggleArray::GetSize(void) const
{
	return m_cpaElements.GetSize();
}

/*
// Get the element at a particular index.
*/

const CToggleElement* CToggleArray::GetAt(int nIndex) const
{
	return (const CToggleElement*)m_cpaElements.GetAt(nIndex);
}

/*
// Add an element to the array.
*/

void CToggleArray::Add(const CToggleElement& Element)
{
	CToggleElement* pNewElement = new CToggleElement(Element);
	m_cpaElements.Add(pNewElement);
}

/*
// Get the index of an element.
*/

int CToggleArray::Find(const CToggleElement& Element) const
{
	int nSize = GetSize();

	for (int i = 0; i < nSize; i++)
	{
		const CToggleElement* pElement = GetAt(i);

		if (pElement->m_pFunction == Element.m_pFunction
				&& (Element.m_pData == NULL || pElement->m_pData == Element.m_pData))
		{
			return i;
		}
	}
	ASSERT(FALSE);
	return -1;
}

/*
// Remove an element by index.
*/

void CToggleArray::Remove(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < GetSize());

	delete (CToggleElement*)GetAt(nIndex);
	m_cpaElements.RemoveAt(nIndex);
}

/*
// Remove an element by value.
*/

void CToggleArray::Remove(const CToggleElement& Element)
{
	Remove(Find(Element));
}

/*
// Remove all elements.
*/

void CToggleArray::RemoveAll(void)
{
	while (GetSize() != 0)
	{
		Remove(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawException

IMPLEMENT_DYNAMIC(CDrawException, CException)

/*
// Constructor.
*/

CDrawException::CDrawException(int nCause)
{
	m_nCause = nCause;
}

/*
// Throw a draw exception.
*/

void ThrowDrawException(int nCause /*=CDrawException::user_interrupt*/)
{
	THROW(new CDrawException(nCause));
}

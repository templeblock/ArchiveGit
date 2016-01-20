/*
// $Workfile: WIDTHSEL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WIDTHSEL.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:52   FRED
// Initial revision.
// 
//    Rev 1.5   28 Aug 1995 15:26:02   FRED
// None/NoNone cases for line width selection.
// 
//    Rev 1.4   19 Aug 1995 17:49:40   FRED
// Maximum line sizes
// 
//    Rev 1.3   13 May 1995 11:10:08   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.2   11 May 1995 08:51:06   FRED
// Choose line width dialog
// 
//    Rev 1.1   05 May 1995 17:10:04   FRED
//  
// 
//    Rev 1.0   03 May 1995 10:42:46   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "utils.h"

#include "widthsel.h"
#include "linewid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWidthElement

CWidthElement::CWidthElement(LPCSTR pszName, int nWidth)
{
	m_csName = pszName;
	m_nWidth = nWidth;
}

CWidthElement::~CWidthElement(void)
{
}
	
/////////////////////////////////////////////////////////////////////////////
// CWidthSelectComboBoxItem

CWidthSelectComboBoxItem::CWidthSelectComboBoxItem(CWidthElement* pWidth)
{
	Width(pWidth);
}
	
CWidthSelectComboBoxItem::~CWidthSelectComboBoxItem(void)
{
	delete m_pWidth;
	m_pWidth = NULL;
}

void CWidthSelectComboBoxItem::PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CBrush cbFill;
	CPen cpOutline;
		
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pComboBox->FontMetrics(tmFont))
		{
			CWidthElement* pWidth = Width();
		
			if (pWidth != NULL)
			{
				if (cbFill.CreateSolidBrush(RGB(0,0,0)))
				{
					if (cpOutline.CreatePen(PS_SOLID, 0, RGB(0,0,0)))
					{
						pDC->SelectObject(&cbFill);
						pDC->SelectObject(&cpOutline);
					
						CRect crWidthBounds(crBounds);
						
						int nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
						int nHeight = MulDiv(pWidth->Width(), nYResolution, 72);
						
						if (nHeight <= 0)
						{
							nHeight = 1;
						}
						
						if (nHeight > tmFont.tmHeight)
						{
							nHeight = tmFont.tmHeight;
						}
						
						if (nHeight > crBounds.Height())
						{
							nHeight = crBounds.Height();
						}
						
						crWidthBounds.top += (crBounds.Height()-nHeight)/2;
						crWidthBounds.bottom = crWidthBounds.top+nHeight;
			
						crWidthBounds.left = crWidthBounds.left+1;
						crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;
						
						pDC->Rectangle(crWidthBounds);
					}
				}
			}
							
			pDC->SetTextColor(clForeground);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextAlign(TA_TOP);
						
			CString csName;
			int nTextLeft;
			
			if (pWidth != NULL)
			{
				csName = pWidth->Name();
				nTextLeft = crBounds.left+(9*tmFont.tmAveCharWidth)/2;
			}
			else
			{
				csName = ((CWidthSelectComboBox*)pComboBox)->NoneLabel();
				nTextLeft = crBounds.left+2;
			}
					
			pDC->ExtTextOut(
				nTextLeft,
				crBounds.top,
				ETO_CLIPPED,
				crBounds,
				csName,
				csName.GetLength(),
				NULL);
		}
	}
	if (nContext != 0)
	{
		pDC->RestoreDC(nContext);
		nContext = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWidthSelectComboBox

CWidthSelectComboBox::CWidthSelectComboBox()
{
	m_pCustomWidth = NULL;
	m_nCustomWidthIndex = CB_ERR;
	m_fAllowNone = TRUE;
}

CWidthSelectComboBox::~CWidthSelectComboBox()
{
}

BEGIN_MESSAGE_MAP(CWidthSelectComboBox, COwnerDrawComboBox)
	//{{AFX_MSG_MAP(CWidthSelectComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CWidthSelectComboBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CWidthSelectComboBox::Widths(LPCSTR pszNoneLabel /*=NULL*/)
{
	static int Widths[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	
	SetRedraw(FALSE);
	ResetContent();
	
	m_csNoneLabel.Empty();
	
	if (pszNoneLabel != NULL)
	{
		m_csNoneLabel = pszNoneLabel;
		
		CWidthSelectComboBoxItem* pItem = new CWidthSelectComboBoxItem(NULL);
		
		if (pItem != NULL)
		{
			AddString((LPCSTR)pItem);
		}
	}
	else
	{
		m_fAllowNone = FALSE;
	}
   
   for (int nIndex = 0; nIndex < sizeof(Widths)/sizeof(Widths[0]); nIndex++)
   {
   	CString csName;
   	WidthName(csName, Widths[nIndex], FALSE);
   	
   	CWidthElement* pWidth = new CWidthElement(csName, Widths[nIndex]);
   	
   	if (pWidth != NULL)
   	{
			CWidthSelectComboBoxItem* pItem = new CWidthSelectComboBoxItem(pWidth);
		
			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}
		
	SetRedraw(TRUE);
	Invalidate();
}

void CWidthSelectComboBox::Width(int nCurrentWidth)
{
	int nCount = GetCount();
	
	if (nCount != CB_ERR)
	{
		// Search for the width in the width list.
		
		BOOL fFound = FALSE;
		int nIndex;
		
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CWidthSelectComboBoxItem* pItem = (CWidthSelectComboBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				CWidthElement* pWidth = pItem->Width();
				
				if (nCurrentWidth < 0)
				{
					if (pWidth == NULL)
					{
						fFound = TRUE;
						break;
					}
				}
				else
				{
					if (pWidth != NULL)
					{
						if (pWidth->Width() == nCurrentWidth)
						{
							fFound = TRUE;
							break;
						}
					}
				}
			}
		}
		
		if (!fFound)
		{
			// Did not find width, create a custom width.
			
			if (m_nCustomWidthIndex != CB_ERR)
			{
				DeleteString(m_nCustomWidthIndex);
				m_nCustomWidthIndex = CB_ERR;
				m_pCustomWidth = NULL;
			}
			
			CString csName;
			WidthName(csName, nCurrentWidth, TRUE);
			
			ASSERT(m_pCustomWidth == NULL);
			
			m_pCustomWidth = new CWidthElement(csName, nCurrentWidth);
			
			if (m_pCustomWidth != NULL)
			{
				CWidthSelectComboBoxItem* pItem = new CWidthSelectComboBoxItem(m_pCustomWidth);
		
				if (pItem != NULL)
				{
					m_nCustomWidthIndex = AddString((LPCSTR)pItem);
				}
			}
			
			if (m_nCustomWidthIndex != CB_ERR)
			{
				nIndex = m_nCustomWidthIndex;
			}
			else
			{
				nIndex = -1;
				
				delete m_pCustomWidth;
				m_pCustomWidth = NULL;
			}
		}
		
		// Set the index of the width that was found, -1 clears any selection.
		
		SetCurSel(nIndex);
	}
}

int CWidthSelectComboBox::Width(void)
{
	int nResult = -1;
	
	int nIndex = GetCurSel();
	
	if (nIndex != CB_ERR)
	{
		CWidthSelectComboBoxItem* pItem = (CWidthSelectComboBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			CWidthElement* pWidth = pItem->Width();

			if (pWidth != NULL)
			{
				nResult = pWidth->Width();
			}
		}
	}
	
	return nResult;
}

void CWidthSelectComboBox::WidthName(CString& csName, int nWidth, BOOL fCustom /*=FALSE*/)
{
	if (fCustom)
	{
		csName = "Custom: ";
	}
	else
	{
		csName.Empty();
	}
	
	if (nWidth == 0)
	{
		csName += "Hairline";
	}
	else
	{
		char szWidth[64];
		
		wsprintf(szWidth, "%d pt", nWidth);
		csName += szWidth;
	}
}

BOOL CWidthSelectComboBox::ChooseCustomWidth(int nMaximumWidth /*=DEFAULT_MAX_WIDTH*/)
{
	CChooseLineWidthDialog Dialog(Width(), nMaximumWidth, m_fAllowNone);
	
	if (Dialog.DoModal() == IDOK)
	{
		Width(Dialog.m_nLineWidth);
		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWidthSelectComboBox message handlers

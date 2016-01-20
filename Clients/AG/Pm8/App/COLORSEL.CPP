/*
// $Workfile: COLORSEL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLORSEL.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:44   Fred
// Initial revision.
// 
//    Rev 1.4   10 Jun 1997 15:29:46   Jay
// New color dialog; saves custom colors.
// 
//    Rev 1.3   15 May 1997 16:39:04   Fred
// Handles undefined color
// 
//    Rev 1.2   12 Nov 1996 10:41:36   johno
// Moved strings to resource DLL
// 
//    Rev 1.1   28 Jul 1996 10:15:30   Jay
// Will not create custom color for non-existent color (index becomes -1 instead).
// 
//    Rev 1.0   14 Mar 1996 13:35:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:36   FRED
// Initial revision.
// 
//    Rev 1.4   23 Jun 1995 16:53:48   JAY
// Won't use palettes (for the dib brush) on non-palette devices.
// 
//    Rev 1.3   13 May 1995 11:12:04   FRED
// More warp preview stuff
// 
//    Rev 1.2   03 May 1995 10:42:10   FRED
//  
// 
//    Rev 1.1   02 May 1995 15:37:58   FRED
// 
//    Rev 1.0   01 May 1995 13:39:40   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "utils.h"
#include "dibbrush.h"
#include "pmwcolor.h"

#include "colorsel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBoxItem

CColorSelectComboBoxItem::CColorSelectComboBoxItem(ColorElement* pColor)
{
	Color(pColor);
}
	
void CColorSelectComboBoxItem::PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CPen cpOutline;
		
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pComboBox->FontMetrics(tmFont))
		{
			ColorElement* pColor = Color();
		
			if (pColor != NULL)
			{
				CDibPatternBrush* pBrush = ((CColorSelectComboBox*)pComboBox)->Brush();

				if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
				{
					CPalette* pPalette = pBrush->Palette();
							
					if (pPalette != NULL)
					{
						pDC->SelectPalette(pPalette, FALSE);
						pDC->RealizePalette();
					}
					pBrush->UsePalette(TRUE);
				}
				else
				{
					pBrush->UsePalette(FALSE);
				}
						
				if (pBrush->Color(colorref_from_color(pColor->get_color())))
				{
					if (cpOutline.CreatePen(PS_NULL, 0, RGB(0,0,0)))
					{
						pDC->SelectObject(pBrush->Brush());
						pDC->SelectObject(&cpOutline);
					
						CRect crColorBounds(crBounds);
			
						crColorBounds.left = crColorBounds.left+1;
						crColorBounds.right = crColorBounds.left+4*tmFont.tmAveCharWidth;
						
						pDC->Rectangle(crColorBounds);
					}
				}
			}
							
			pDC->SetTextColor(clForeground);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextAlign(TA_TOP);
						
			CString csName;
			int nTextLeft;
			
			if (pColor != NULL)
			{
				csName = pColor->get_name();
				csName = csName.Mid(csName.Find('-')+1);
				nTextLeft = crBounds.left+(9*tmFont.tmAveCharWidth)/2;
			}
			else
			{
				csName = ((CColorSelectComboBox*)pComboBox)->NoneLabel();
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
// CColorSelectComboBox

CColorSelectComboBox::CColorSelectComboBox()
{
	m_pCustomColor = NULL;
	m_nCustomColorIndex = CB_ERR;
}

CColorSelectComboBox::~CColorSelectComboBox()
{
	delete m_pCustomColor;
	m_pCustomColor = NULL;
}

BEGIN_MESSAGE_MAP(CColorSelectComboBox, COwnerDrawComboBox)
	//{{AFX_MSG_MAP(CColorSelectComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CColorSelectComboBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CColorSelectComboBox::Colors(ColorList* pList /*=NULL*/, LPCSTR pszNoneLabel /*=NULL*/)
{
	if (pList == NULL)
	{
		pList = &system_color_list;
	}
	
	SetRedraw(FALSE);
	ResetContent();
	
	m_csNoneLabel.Empty();
	
	if (pszNoneLabel != NULL)
	{
		m_csNoneLabel = pszNoneLabel;
		
		CColorSelectComboBoxItem* pItem = new CColorSelectComboBoxItem(NULL);
		
		if (pItem != NULL)
		{
			AddString((LPCSTR)pItem);
		}
	}

	ColorElement* pColor = pList->first_color();
	
	while (pColor != NULL)
	{
		CColorSelectComboBoxItem* pItem = new CColorSelectComboBoxItem(pColor);
		
		if (pItem != NULL)
		{
			AddString((LPCSTR)pItem);
		}
		
		pColor = pColor->next_color();
	}
		
	SetRedraw(TRUE);
	Invalidate();
}

void CColorSelectComboBox::Color(COLOR CurrentColor, BOOL fColorExists /*=TRUE*/)
{
	if (CurrentColor == UNDEFINED_COLOR)
	{
		SetCurSel(-1);
		return;
	}

	if (CurrentColor == TRANSPARENT_COLOR)
	{
		fColorExists = FALSE;
	}

	int nCount = GetCount();
	
	if (nCount != CB_ERR)
	{
		// Search for the color in the color list.
		
		BOOL fFound = FALSE;
		int nIndex;
		
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CColorSelectComboBoxItem* pItem = (CColorSelectComboBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				ColorElement* pColor = pItem->Color();
				
				if (!fColorExists)
				{
					if (pColor == NULL)
					{
						fFound = TRUE;
						break;
					}
				}
				else
				{
					if (pColor != NULL)
					{
						if ((pColor->get_color() == CurrentColor)
						 || (colorref_from_color(pColor->get_color()) == colorref_from_color(CurrentColor)))
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
			if (fColorExists)
			{
				// Did not find color, create a custom color.
				
				if (m_nCustomColorIndex != CB_ERR)
				{
					DeleteString(m_nCustomColorIndex);
					m_nCustomColorIndex = CB_ERR;
				}
				
				delete m_pCustomColor;
            m_pCustomColor = new ColorElement((char *)GET_PMWAPP()->GetResourceStringPointer(IDS_CUSTOM), CurrentColor);
				
				if (m_pCustomColor != NULL)
				{
					CColorSelectComboBoxItem* pItem = new CColorSelectComboBoxItem(m_pCustomColor);
			
					if (pItem != NULL)
					{
						m_nCustomColorIndex = AddString((LPCSTR)pItem);
					}
				}
				
				if (m_nCustomColorIndex != CB_ERR)
				{
					nIndex = m_nCustomColorIndex;
				}
				else
				{
					nIndex = -1;
				}
			}
			else
			{
				// Did not find non-existent color. Do not select anything.
				nIndex = -1;
			}
		}
		
		// Set the index of the color that was found, -1 clears any selection.
		
		SetCurSel(nIndex);
	}
}

BOOL CColorSelectComboBox::Color(COLOR* pCurrentColor)
{
	ASSERT(pCurrentColor != NULL);
	
	BOOL fResult = FALSE;
	
	int nIndex = GetCurSel();
	
	if (nIndex != CB_ERR)
	{
		CColorSelectComboBoxItem* pItem = (CColorSelectComboBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			ColorElement* pColor = pItem->Color();

			if (pColor != NULL)
			{
				*pCurrentColor = pColor->get_color();
				fResult = TRUE;
			}
		}
	}
	
	return fResult;
}

BOOL CColorSelectComboBox::ChooseCustomColor(void)
{
	COLOR CurrentColor;
	
	if (!Color(&CurrentColor))
	{
		CurrentColor = COLOR_BLACK;
	}
	
	CPmwColorDialog Dialog(colorref_from_color(CurrentColor));
	
	if (Dialog.DoModal() == IDOK)
	{
		Color(color_from_colorref(Dialog.GetColor()));
		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CColorSelectComboBox message handlers

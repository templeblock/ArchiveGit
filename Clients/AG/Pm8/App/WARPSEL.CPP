/*
// $Workfile: WARPSEL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPSEL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:36   FRED
// Initial revision.
// 
//    Rev 1.1   15 May 1995 07:43:56   FRED
// Properly matches warp filed names.
// 
// 
//    Rev 1.0   13 May 1995 11:57:36   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "utils.h"

#include "warpsel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarpSelectListBoxItem

CWarpSelectListBoxItem::CWarpSelectListBoxItem(CWarpField* pWarpField)
{
	WarpField(pWarpField);
}
	
void CWarpSelectListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	INHERITED::MeasureItem(pList, pMeasureItemStruct);
	pMeasureItemStruct->itemHeight *= 2;
}

void CWarpSelectListBoxItem::PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CPen cpPen;
	CBrush cbBrush;
	
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pListBox->FontMetrics(tmFont))
		{
			CWarpField* pWarpField = WarpField();
			
			// Draw warp field preview. If there is no warp field, use a rectangle.
			
			CRect crWarpBounds(crBounds);
					
			crWarpBounds.left = crWarpBounds.left+1;
			crWarpBounds.right = crWarpBounds.left+8*tmFont.tmAveCharWidth;
			crWarpBounds.top = crWarpBounds.top+2;
			crWarpBounds.bottom = crWarpBounds.bottom-3;
								
			if (pWarpField == NULL)
			{
				if (cpPen.CreateStockObject(BLACK_PEN))
				{
					if (cbBrush.CreateSolidBrush(RGB(128,128,128)))
					{
						pDC->SelectObject(&cpPen);
						pDC->SelectObject(&cbBrush);
						pDC->Rectangle(crWarpBounds);
					}
				}
			}
			else
			{
				pWarpField->Transform(crWarpBounds);
				
				COutlinePath Path;
				pWarpField->TransformedBounds()->Path(&Path);
				Path.Draw2(
					pDC,
					RGB(128,128,128),
					TRUE,
					0,
					RGB(0,0,0),
					TRUE,
					WINDING,
					FALSE);
			}
								
			pDC->SetTextColor(clForeground);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextAlign(TA_TOP);
							
			CString csName;
				
			if (pWarpField != NULL)
			{
				csName = pWarpField->Name();
			}
			else
			{
				csName = ((CWarpSelectListBox*)pListBox)->NoneLabel();
			}
						
			pDC->ExtTextOut(
				crBounds.left+9*tmFont.tmAveCharWidth,
				crBounds.top+(crBounds.Height()-tmFont.tmHeight)/2,
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
// CWarpSelectListBox

CWarpSelectListBox::CWarpSelectListBox()
{
}

CWarpSelectListBox::~CWarpSelectListBox()
{
}

BEGIN_MESSAGE_MAP(CWarpSelectListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CWarpSelectListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CWarpSelectListBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CWarpSelectListBox::WarpFields(CWarpFile* pWarpFile, LPCSTR pszNoneLabel /*=NULL*/)
{
	SetRedraw(FALSE);
	ResetContent();
	
	m_csNoneLabel.Empty();
	
	if (pszNoneLabel != NULL)
	{
		m_csNoneLabel = pszNoneLabel;
		
		CWarpSelectListBoxItem* pItem = new CWarpSelectListBoxItem(NULL);
		
		if (pItem != NULL)
		{
			AddString((LPCSTR)pItem);
		}
	}

	for (int i = 0; i < pWarpFile->WarpFieldCount(); i++)
	{
		CWarpField* pWarpField = pWarpFile->WarpField(i);
				
		if (pWarpField != NULL)
		{
			CWarpSelectListBoxItem* pItem = new CWarpSelectListBoxItem(pWarpField);
			
			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}
		
	SetRedraw(TRUE);
	Invalidate();
}

void CWarpSelectListBox::WarpField(CWarpField* pCurrentWarpField)
{
	int nCount = GetCount();
	
	if (nCount != LB_ERR)
	{
		// Search for the warp field in the warp field list.
		
		BOOL fFound = FALSE;
		int nIndex;
		
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CWarpSelectListBoxItem* pItem = (CWarpSelectListBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				CWarpField* pWarpField = pItem->WarpField();
				
				if ((pWarpField == NULL) || (pCurrentWarpField == NULL))
				{
					if (pWarpField == pCurrentWarpField)
					{
						fFound = TRUE;
						break;
					}
				}
				else
				{
					if (pWarpField->Name() == pCurrentWarpField->Name())
					{
						fFound = TRUE;
						break;
					}
				}
			}
		}
		
		if (!fFound)
		{
			nIndex = -1;
		}
		
		SetCurSel(nIndex);
	}
}

CWarpField* CWarpSelectListBox::WarpField(void)
{
	CWarpField* pResult = NULL;
	
	int nIndex = GetCurSel();
	
	if (nIndex != LB_ERR)
	{
		CWarpSelectListBoxItem* pItem = (CWarpSelectListBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			pResult = pItem->WarpField();
		}
	}
	
	return pResult;
}

/////////////////////////////////////////////////////////////////////////////
// CWarpSelectListBox message handlers

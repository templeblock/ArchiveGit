/*
// $Workfile: SHAPESEL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SHAPESEL.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:52   FRED
// Initial revision.
// 
//    Rev 1.0   23 May 1995 09:17:46   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "utils.h"

#include "shapesel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShapeSelectListBoxItem

CShapeSelectListBoxItem::CShapeSelectListBoxItem(CShapeEntry* pShapeEntry)
{
	Shape(pShapeEntry);
}
	
void CShapeSelectListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	INHERITED::MeasureItem(pList, pMeasureItemStruct);
	pMeasureItemStruct->itemHeight *= 2;
}

void CShapeSelectListBoxItem::PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CPen cpPen;
	CBrush cbBrush;
	
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pListBox->FontMetrics(tmFont))
		{
			CShapeEntry* pShapeEntry = Shape();
			
			CRect crShapeBounds(crBounds);
						
			if (pShapeEntry != NULL)
			{
				// Compute the rectangle that describes the area for the shape preview.
				
				crShapeBounds.top = crShapeBounds.top+2;
				crShapeBounds.bottom = crShapeBounds.bottom-3;
				
				crShapeBounds.left = crShapeBounds.left+1;
				
				int nXResolution = pDC->GetDeviceCaps(LOGPIXELSX);
				int nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
				
				crShapeBounds.right = crShapeBounds.left+MulDiv(crShapeBounds.bottom-crShapeBounds.top, nXResolution*2, nYResolution);
				
				CFixed fxShapeBoundsWidth = MakeFixed(crShapeBounds.Width());
				CFixed fxShapeBoundsHeight = MakeFixed(crShapeBounds.Height());
				
				// Get the bounding box for the shape path.
				
				switch (pShapeEntry->m_nType)
				{
					case CShapeEntry::DrawingObject:
					{
						CFixed fxPathWidth = pShapeEntry->m_Bounds.Width();
						CFixed fxPathHeight = pShapeEntry->m_Bounds.Height();
						
						if ((fxPathWidth != 0) && (fxPathHeight != 0))
						{
							// Compute the path dimensions of the path to draw in the preview.
							// The aspect of the path is preserved.
							
							if (DivFixed(fxPathHeight, fxPathWidth) > DivFixed(fxShapeBoundsHeight, fxShapeBoundsWidth))
							{
								fxPathWidth = MulDivFixed(fxPathWidth, fxShapeBoundsHeight, fxPathHeight);
								fxPathHeight = fxShapeBoundsHeight;
							}
							else
							{
								fxPathHeight = MulDivFixed(fxPathHeight, fxShapeBoundsWidth, fxPathWidth);
								fxPathWidth = fxShapeBoundsWidth;
							}
							
							// Compute the matrix for transforming the path to the desired size.
							// By convention, the path has been normalized to the unit square.
							
							COutlinePathMatrix Matrix;
							Matrix.Identity();
							
							Matrix.ScaleBy(fxPathWidth, fxPathHeight);
							
							// Adjust the matrix to center the shape within the preview box.
							
							Matrix.TranslateBy(MakeFixed(crShapeBounds.left)+(fxShapeBoundsWidth-fxPathWidth)/2,
													 MakeFixed(crShapeBounds.top)+(fxShapeBoundsHeight-fxPathHeight)/2);
			
							// Transform the path and draw it.
							
							COutlinePath Path;
							Path.Copy(&(pShapeEntry->m_Path));
						
							Path.Transform(&Matrix);
							
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
						
						break;
					}
					
					case CShapeEntry::RectangleObject:
					{
						if (cpPen.CreateStockObject(BLACK_PEN))
						{
							if (cbBrush.CreateSolidBrush(RGB(128,128,128)))
							{
								crShapeBounds.InflateRect(-4, -2);
								pDC->SelectObject(&cpPen);
								pDC->SelectObject(&cbBrush);
								pDC->Rectangle(crShapeBounds);
							}
						}
						
						break;
					}
					
					case CShapeEntry::EllipseObject:
					{
						if (cpPen.CreateStockObject(BLACK_PEN))
						{
							if (cbBrush.CreateSolidBrush(RGB(128,128,128)))
							{
								crShapeBounds.InflateRect(-4, -2);
								pDC->SelectObject(&cpPen);
								pDC->SelectObject(&cbBrush);
								pDC->Ellipse(crShapeBounds);
							}
						}
						
						break;
					}
					
					case CShapeEntry::LineObject:
					{
						if (cpPen.CreatePen(PS_SOLID, 3, RGB(0,0,0)))
						{
							crShapeBounds.InflateRect(-4, -2);
							pDC->SelectObject(&cpPen);
							pDC->MoveTo(crShapeBounds.left, crShapeBounds.top);
							pDC->LineTo(crShapeBounds.right, crShapeBounds.bottom);
						}
						
						break;
					}
					
					default:
					{
						ASSERT(FALSE);
						break;
					}
				}
							
				crShapeBounds.right += tmFont.tmAveCharWidth;
			}
								
			pDC->SetTextColor(clForeground);
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextAlign(TA_TOP);
							
			pDC->ExtTextOut(
				crShapeBounds.right,
				crBounds.top+(crBounds.Height()-tmFont.tmHeight)/2,
				ETO_CLIPPED,
				crBounds,
				pShapeEntry->m_csName,
				pShapeEntry->m_csName.GetLength(),
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
// CShapeSelectListBox

CShapeSelectListBox::CShapeSelectListBox()
{
}

CShapeSelectListBox::~CShapeSelectListBox()
{
}

BEGIN_MESSAGE_MAP(CShapeSelectListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CShapeSelectListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CShapeSelectListBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CShapeSelectListBox::Shapes(CShapeFile* pShapeFile)
{
	SetRedraw(FALSE);
	ResetContent();
	
	for (int i = 0; i < pShapeFile->ShapeCount(); i++)
	{
		CShapeEntry* pShapeEntry = pShapeFile->Shape(i);
				
		if (pShapeEntry != NULL)
		{
			CShapeSelectListBoxItem* pItem = new CShapeSelectListBoxItem(pShapeEntry);
			
			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}
		
	SetRedraw(TRUE);
	Invalidate();
}

void CShapeSelectListBox::Shape(CShapeEntry* pCurrentShapeEntry)
{
	int nCount = GetCount();
	
	if (nCount != LB_ERR)
	{
		// Search for the shape in the shape list.
		
		BOOL fFound = FALSE;
		int nIndex;
		
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CShapeSelectListBoxItem* pItem = (CShapeSelectListBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				if (pItem->Shape()->m_csName == pCurrentShapeEntry->m_csName)
				{
					fFound = TRUE;
					break;
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

CShapeEntry* CShapeSelectListBox::Shape(void)
{
	CShapeEntry* pResult = NULL;
	
	int nIndex = GetCurSel();
	
	if (nIndex != LB_ERR)
	{
		CShapeSelectListBoxItem* pItem = (CShapeSelectListBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			pResult = pItem->Shape();
		}
	}
	
	return pResult;
}

/////////////////////////////////////////////////////////////////////////////
// CShapeSelectListBox message handlers

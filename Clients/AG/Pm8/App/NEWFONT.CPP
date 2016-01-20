/*
// $Workfile: NEWFONT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/NEWFONT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
 * 
 * 9     1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 8     12/23/98 1:50p Psasse
// Code clean up
// 
// 7     11/30/98 2:34p Mwilson
// fixed font format dialog.  No longer show stetching in preview
// 
// 6     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 5     4/29/98 4:27p Fredf
// New PMW fonts. Required changes to PMW.FFL.
// 
// 4     4/24/98 6:34p Fredf
// Font dialog uses new color combo boxes.
// 
// 3     4/17/98 11:40a Fredf
// Fixes to code to make "Show Actual Size" persistent.
// 
// 2     4/16/98 7:32p Fredf
// "Preview Actual Size" control in font dialog (NEWFONT.CPP)
// 
//    Rev 1.0   14 Aug 1997 15:22:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:20   Fred
// Initial revision.
// 
//    Rev 1.7   12 Aug 1997 15:44:10   Fred
// Stretched text no longer changes size in preview
// 
//    Rev 1.6   16 Jul 1997 10:14:56   Jay
// Don't memset fcs; it has a constructor
// 
//    Rev 1.5   15 Jul 1997 16:12:42   dennisp
// Apply button is now hidden for font dialog
// 
//    Rev 1.4   02 Jun 1997 18:19:58   Fred
// Bug fixes
// 
//    Rev 1.3   27 May 1997 11:07:40   Fred
// Changed outline selections
// 
//    Rev 1.2   23 May 1997 17:06:42   Fred
// New effects
// 
//    Rev 1.1   15 May 1997 17:39:26   Fred
// Fixed 16-bit bug and zapped some warnings
// 
//    Rev 1.0   15 May 1997 16:55:04   Fred
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "mainfrm.h"
#include "pmwview.h"
#include "pmgfont.h"
#include "frameobj.h"
#include "textrec.h"
#include "ctxp.h"
#include "linewid.h"
#include "util.h"
#include "db386.h"
#include "newfont.h"

#include <afxpriv.h>

#define	WM_MOVE_PREVIEW_TO_TOP	(WM_USER+2048)


extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(VOIDPTR data);
extern CPalette * near pOurPal;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CPmwView* GetCurrentView(void)
{
	CPmwView* pView = NULL;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if ((pMainFrame != NULL) && (!pMainFrame->InPreviewMode()))
	{
		CFrameWnd* pActiveFrame = pMainFrame->GetActiveFrame();
		if (pActiveFrame != NULL)
		{
			pView = (CPmwView*)(pActiveFrame->GetActiveView());
		}
	}

	return pView;
}

/////////////////////////////////////////////////////////////////////////////
// COutlineElement

COutlineElement::COutlineElement(LPCSTR pszName, const OutlineDescription* pDescription)
{
	m_csName = pszName;
	if (pDescription == NULL)
	{
		m_fDescriptionExists = FALSE;
	}
	else
	{
		m_fDescriptionExists = TRUE;
		m_Description = *pDescription;
	}
}

COutlineElement::~COutlineElement(void)
{
}

	
/////////////////////////////////////////////////////////////////////////////
// COutlineSelectComboBoxItem

COutlineSelectComboBoxItem::COutlineSelectComboBoxItem(COutlineElement* pOutline)
{
	Outline(pOutline);
}
	
COutlineSelectComboBoxItem::~COutlineSelectComboBoxItem(void)
{
	delete m_pOutline;
	m_pOutline = NULL;
}

void COutlineSelectComboBoxItem::PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CBrush cbFill;
	CPen cpOutline;
		
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pComboBox->FontMetrics(tmFont))
		{
			COutlineElement* pOutline = Outline();
		
			if (pOutline != NULL)
			{
				const OutlineDescription* pDescription = pOutline->Description();
				if (pDescription != NULL)
				{
					int nStyle = pDescription->m_nStyle;
					int nWidthType = pDescription->m_nWidthType;

					if ((nStyle == OutlineFormat::Inflate) || (nStyle == OutlineFormat::Light))
					{
						int nWidth;
						if (nWidthType == OutlineFormat::Relative)
						{
							nWidth = (int)MulFixed(pDescription->m_lWidth, 400);	// oooh - magic number
						}
						else
						{
							nWidth = FixedInteger(pDescription->m_lWidth);
						}

						if (cbFill.CreateSolidBrush(RGB(0,0,0)))
						{
							if (cpOutline.CreatePen(PS_SOLID, 0, RGB(0,0,0)))
							{
								pDC->SelectObject(&cbFill);
								pDC->SelectObject(&cpOutline);
							
								CRect crWidthBounds(crBounds);
								
								int nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
								int nHeight = MulDiv(nWidth, nYResolution, 72);
								
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
					else
					{
						switch (nStyle)
						{
							case OutlineFormat::Contour1:
							{
								int nHeight = 7;

								CRect crWidthBounds(crBounds);
								
								crWidthBounds.top += (crBounds.Height()-nHeight)/2;
								crWidthBounds.bottom = crWidthBounds.top+nHeight;
					
								crWidthBounds.left = crWidthBounds.left+1;
								crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

								crWidthBounds.bottom = crWidthBounds.top+2;

								Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));

								crWidthBounds.top = crWidthBounds.bottom;
								crWidthBounds.bottom = crWidthBounds.top+5;

								Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(192,192,192));
								
								break;
							}
							case OutlineFormat::Contour2:
							{
								int nHeight = 8;

								CRect crWidthBounds(crBounds);
								
								crWidthBounds.top += (crBounds.Height()-nHeight)/2;
								crWidthBounds.bottom = crWidthBounds.top+nHeight;
					
								crWidthBounds.left = crWidthBounds.left+1;
								crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

								crWidthBounds.bottom = crWidthBounds.top+2;

								Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));

								crWidthBounds.top = crWidthBounds.bottom;
								crWidthBounds.bottom = crWidthBounds.top+4;

								Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(192,192,192));

								crWidthBounds.top = crWidthBounds.bottom;
								crWidthBounds.bottom = crWidthBounds.top+2;

								Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));
								
								break;
							}
							case OutlineFormat::Calligraphy:
							{
								CRect crWidthBounds(crBounds);
								
								crWidthBounds.left = crWidthBounds.left+1;
								crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

								crWidthBounds.InflateRect(0, -1);

								int nHeight = crWidthBounds.Height();
								int nWidth = crWidthBounds.Width();

								if (nWidth > nHeight)
								{
									crWidthBounds.left += (nWidth-nHeight)/2;
									crWidthBounds.right = crWidthBounds.left+nHeight;
								}
								else
								{
									crWidthBounds.top += (nHeight-nWidth)/2;
									crWidthBounds.bottom = crWidthBounds.top+nWidth;
								}

								pDC->SetMapMode(MM_TEXT);

								CPen Pen;
								if (Pen.CreatePen(PS_SOLID, 2, RGB(0,0,0)))
								{
									pDC->SelectObject(&Pen);

									pDC->MoveTo(crWidthBounds.right, crWidthBounds.top);
									pDC->LineTo(crWidthBounds.left, crWidthBounds.bottom);

									pDC->SelectStockObject(NULL_PEN);
								}

								break;
							}
						}
					}
				}
								
				pDC->SetTextColor(clForeground);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextAlign(TA_TOP);
							
				CString csName;
				TRY
				{
					csName = pOutline->Name();
				}
				END_TRY

				int nTextLeft = crBounds.left;
				if ((pDescription != NULL) && (pDescription->m_nStyle != OutlineFormat::None))
				{
					nTextLeft += (9*tmFont.tmAveCharWidth)/2;
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
	}
	if (nContext != 0)
	{
		pDC->RestoreDC(nContext);
		nContext = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// COutlineSelectComboBox

static const OutlineDescription OutlineDescriptions[] =
{
	{ IDS_NO_OUTLINE,		OutlineFormat::None,				OutlineFormat::Absolute,	MakeFixed(0) },
	{ IDS_HAIRLINE,		OutlineFormat::Light,			OutlineFormat::Absolute,	MakeFixed(0) },
	{ IDS_THIN,				OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_THIN_FRACTION) },
	{ IDS_MEDIUM,			OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_MEDIUM_FRACTION) },
	{ IDS_THICK,			OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_THICK_FRACTION) },
	{ IDS_CONTOUR1,		OutlineFormat::Contour1,		OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_CONTOUR2,		OutlineFormat::Contour2,		OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_CALLIGRAPHY,	OutlineFormat::Calligraphy,	OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(1) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(2) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(4) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(8) }
};

#define NUMBER_OF_OUTLINE_DESCRIPTIONS	(sizeof(OutlineDescriptions)/sizeof(OutlineDescriptions[0]))

COutlineSelectComboBox::COutlineSelectComboBox()
{
	m_pCustomOutline = NULL;
	m_nCustomOutlineIndex = CB_ERR;
}

COutlineSelectComboBox::~COutlineSelectComboBox()
{
}

BEGIN_MESSAGE_MAP(COutlineSelectComboBox, COwnerDrawComboBox)
	//{{AFX_MSG_MAP(COutlineSelectComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* COutlineSelectComboBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void COutlineSelectComboBox::Outlines(const OutlineDescription* pDescriptions, int nCount)
{
	SetRedraw(FALSE);
	ResetContent();
	
   for (int nIndex = 0; nIndex < nCount; nIndex++)
   {
		const OutlineDescription* pDescription = &(pDescriptions[nIndex]);

	  	CString csName;
  		OutlineName(csName, pDescription, FALSE);
   	
   	COutlineElement* pOutline = NULL;
		TRY
		{
			pOutline = new COutlineElement(csName, pDescription);
		}
		END_TRY
   	
   	if (pOutline != NULL)
   	{
			COutlineSelectComboBoxItem* pItem = NULL;
			TRY
			{
				pItem = new COutlineSelectComboBoxItem(pOutline);
			}
			END_TRY
		
			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}

	SetRedraw(TRUE);
	Invalidate();
}

void COutlineSelectComboBox::Outline(const OutlineDescription* pDescription)
{
	if (pDescription->m_nStyle == OutlineFormat::Undefined)
	{
		SetCurSel(-1);
		return;
	}

	int nCount = GetCount();
	
	if (nCount != CB_ERR)
	{
		// Search for the outline in the outline list.
		
		BOOL fFound = FALSE;
		int nIndex;

		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			COutlineSelectComboBoxItem* pItem = (COutlineSelectComboBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				COutlineElement* pOutline = pItem->Outline();
				if (pOutline != NULL)
				{
					const OutlineDescription* pOutlineDescription = pOutline->Description();
					if (pOutlineDescription != NULL)
					{
						if ((pOutlineDescription->m_nStyle == pDescription->m_nStyle)
						 && (pOutlineDescription->m_nWidthType == pDescription->m_nWidthType)
						 && (pOutlineDescription->m_lWidth == pDescription->m_lWidth))
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
			// Did not find outline, create a custom outline.
			
			if (m_nCustomOutlineIndex != CB_ERR)
			{
				DeleteString(m_nCustomOutlineIndex);
				m_nCustomOutlineIndex = CB_ERR;
				m_pCustomOutline = NULL;
			}
			
			CString csName;
			OutlineName(csName, pDescription, TRUE);
			
			ASSERT(m_pCustomOutline == NULL);
			
			TRY
			{
				m_pCustomOutline = new COutlineElement(csName, pDescription);
			}
			END_TRY
			
			if (m_pCustomOutline != NULL)
			{
				COutlineSelectComboBoxItem* pItem = NULL;
				
				TRY
				{
					pItem = new COutlineSelectComboBoxItem(m_pCustomOutline);
				}
				END_TRY
		
				if (pItem != NULL)
				{
					m_nCustomOutlineIndex = AddString((LPCSTR)pItem);
				}
			}
			
			if (m_nCustomOutlineIndex != CB_ERR)
			{
				nIndex = m_nCustomOutlineIndex;
			}
			else
			{
				nIndex = -1;
				
				delete m_pCustomOutline;
				m_pCustomOutline = NULL;
			}
		}
		
		// Set the index of the width that was found, -1 clears any selection.
		
		SetCurSel(nIndex);
	}
}

COutlineElement* COutlineSelectComboBox::Outline(void)
{
	COutlineElement* pOutline = NULL;
	
	int nIndex = GetCurSel();
	
	if (nIndex != CB_ERR)
	{
		COutlineSelectComboBoxItem* pItem = (COutlineSelectComboBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			pOutline = pItem->Outline();
		}
	}
	
	return pOutline;
}

void COutlineSelectComboBox::OutlineName(CString& csName, const OutlineDescription* pDescription, BOOL fCustom /*=FALSE*/)
{
	ASSERT(pDescription != NULL);

	csName.Empty();

	if (fCustom)
	{
		TRY
		{
			csName.LoadString(IDS_CUSTOM_OUTLINE);
		}
		END_TRY
	}

	CString csFormat;
	CString csText;
	TRY
	{
		csFormat.LoadString(pDescription->m_uName);
		csText.Format(csFormat, FixedInteger(pDescription->m_lWidth));
		csName += csText;
	}
	END_TRY
}

BOOL COutlineSelectComboBox::ChooseCustomOutline(int nMaximumWidth /*=DEFAULT_MAX_WIDTH*/)
{
	static OutlineDescription NoOutlineDescription = { IDS_NONE, OutlineFormat::None, OutlineFormat::Absolute, MakeFixed(0) };
	static OutlineDescription HairlineOutlineDescription = { IDS_HAIRLINE, OutlineFormat::Light, OutlineFormat::Absolute, MakeFixed(0) };
	static OutlineDescription FixedWidthOutlineDescription = { IDS_POINT, OutlineFormat::Inflate, OutlineFormat::Absolute, MakeFixed(0) };

	int nWidth = 0;

	COutlineElement* pOutline = Outline();
	if (pOutline != NULL)
	{
		const OutlineDescription* pDescription = pOutline->Description();
		if (pDescription != NULL)
		{
			if ((pDescription->m_nStyle == OutlineFormat::Inflate) && (pDescription->m_nWidthType == OutlineFormat::Absolute))
			{
				nWidth = FixedInteger(pDescription->m_lWidth);
			}
		}
	}

	CChooseLineWidthDialog Dialog(nWidth, nMaximumWidth, TRUE);
	
	if (Dialog.DoModal() == IDOK)
	{
		if (Dialog.m_nLineWidth < 0)
		{
			Outline(&NoOutlineDescription);
		}
		else if (Dialog.m_nLineWidth == 0)
		{
			Outline(&HairlineOutlineDescription);
		}
		else
		{
			FixedWidthOutlineDescription.m_lWidth = MakeFixed(Dialog.m_nLineWidth);
			Outline(&FixedWidthOutlineDescription);
		}

		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// COutlineSelectComboBox message handlers


/////////////////////////////////////////////////////////////////////////////
// CShadowElement

CShadowElement::CShadowElement(LPCSTR pszName, const ShadowDescription* pDescription)
{
	ASSERT(pDescription != NULL);

	m_csName = pszName;
	m_Description = *pDescription;
}

CShadowElement::~CShadowElement(void)
{
}

	
/////////////////////////////////////////////////////////////////////////////
// CShadowSelectComboBoxItem

CShadowSelectComboBoxItem::CShadowSelectComboBoxItem(CShadowElement* pShadow)
{
	Shadow(pShadow);
}
	
CShadowSelectComboBoxItem::~CShadowSelectComboBoxItem(void)
{
	delete m_pShadow;
	m_pShadow = NULL;
}

void CShadowSelectComboBoxItem::PaintItem(COwnerDrawComboBox* pComboBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CBrush cbFill;
	CPen cpShadow;
		
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		TEXTMETRIC tmFont;
		
		if (pComboBox->FontMetrics(tmFont))
		{
			CShadowElement* pShadow = Shadow();
		
			if (pShadow != NULL)
			{
				const ShadowDescription* pDescription = pShadow->Description();
				if (pDescription != NULL)
				{
					int nStyle = pDescription->m_nStyle;

					if ((nStyle == ShadowFormat::Drop) || (nStyle == ShadowFormat::Extruded))
					{
						CRect crImage(crBounds);
								
						crImage.left = crImage.left+1;
						crImage.right = crImage.left+4*tmFont.tmAveCharWidth;

						crImage.InflateRect(0, -1);

						int nHeight = crImage.Height();
						int nWidth = crImage.Width();

						if (nWidth > nHeight)
						{
							crImage.left += (nWidth-nHeight)/2;
							crImage.right = crImage.left+nHeight;
						}
						else
						{
							crImage.top += (nHeight-nWidth)/2;
							crImage.bottom = crImage.top+nWidth;
						}

						int nCount = (crImage.Width()+3)/7;
						int nSize = crImage.Width()-nCount;
						int nX = crImage.left+((pDescription->m_lXOffset >= 0) ? nCount : 0);
						int nY = crImage.top+((pDescription->m_lYOffset <= 0) ? nCount : 0);
						int nXInc = pDescription->m_lXOffset >= 0 ? -1 : 1;
						int nYInc = pDescription->m_lYOffset <= 0 ? -1 : 1;
						CRect crRect(nX, nY, nX+nSize, nY+nSize);

						for (int nStep = 0; nStep <= nCount; nStep++)
						{
							if ((nStyle == ShadowFormat::Extruded)
							 || (nStep == 0)
							 || (nStep == nCount))
							{
								Util::DrawBorderWithColor(*pDC, crRect, (nStep == nCount) ? RGB(0,0,0) : RGB(128,128,128), 1);
							}
							crRect.OffsetRect(nXInc, nYInc);
						}
					}
				}
								
				pDC->SetTextColor(clForeground);
				pDC->SetBkMode(TRANSPARENT);
				pDC->SetTextAlign(TA_TOP);
							
				CString csName;
				TRY
				{
					csName = pShadow->Name();
				}
				END_TRY

				int nTextLeft = crBounds.left;
				if ((pDescription != NULL) && (pDescription->m_nStyle != ShadowFormat::None))
				{
					nTextLeft += (9*tmFont.tmAveCharWidth)/2;
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
	}
	if (nContext != 0)
	{
		pDC->RestoreDC(nContext);
		nContext = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CShadowSelectComboBox

static const ShadowDescription ShadowDescriptions[] =
{
	{ IDS_NO_SHADOW,				ShadowFormat::None,		ShadowFormat::Absolute,	 MakeFixed(0),								 MakeFixed(0) },
	{ IDS_DROP_DOWN_RIGHT,		ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_DOWN_LEFT,		ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_RIGHT,			ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_LEFT,			ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_DOWN_RIGHT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_DOWN_LEFT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_UP_RIGHT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_UP_LEFT,		ShadowFormat::Extruded,	ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) }
};

#define NUMBER_OF_SHADOW_DESCRIPTIONS	(sizeof(ShadowDescriptions)/sizeof(ShadowDescriptions[0]))

CShadowSelectComboBox::CShadowSelectComboBox()
{
	m_nCustomShadowIndex = CB_ERR;
	m_pCustomShadow = NULL;
}

CShadowSelectComboBox::~CShadowSelectComboBox()
{
}

BEGIN_MESSAGE_MAP(CShadowSelectComboBox, COwnerDrawComboBox)
	//{{AFX_MSG_MAP(CShadowSelectComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CShadowSelectComboBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CShadowSelectComboBox::Shadows(const ShadowDescription* pDescriptions, int nCount)
{
	SetRedraw(FALSE);
	ResetContent();
	
   for (int nIndex = 0; nIndex < nCount; nIndex++)
   {
		const ShadowDescription* pDescription = &(pDescriptions[nIndex]);

	  	CString csName;
  		ShadowName(csName, pDescription, FALSE);
   	
   	CShadowElement* pShadow = NULL;
		TRY
		{
			pShadow = new CShadowElement(csName, pDescription);
		}
		END_TRY
   	
   	if (pShadow != NULL)
   	{
			CShadowSelectComboBoxItem* pItem = NULL;
			TRY
			{
				pItem = new CShadowSelectComboBoxItem(pShadow);
			}
			END_TRY
		
			if (pItem != NULL)
			{
				AddString((LPCSTR)pItem);
			}
		}
	}

	SetRedraw(TRUE);
	Invalidate();
}

void CShadowSelectComboBox::Shadow(const ShadowDescription* pDescription)
{
	if (pDescription->m_nStyle == ShadowFormat::Undefined)
	{
		SetCurSel(-1);
		return;
	}

	int nCount = GetCount();
	
	if (nCount != CB_ERR)
	{
		// Search for the Shadow in the Shadow list.
		
		BOOL fFound = FALSE;
		int nIndex;
		
		for (nIndex = 0; nIndex < nCount; nIndex++)
		{
			CShadowSelectComboBoxItem* pItem = (CShadowSelectComboBoxItem*)GetItemData(nIndex);
			
			if (pItem != NULL)
			{
				CShadowElement* pShadow = pItem->Shadow();
				if (pShadow != NULL)
				{
					const ShadowDescription* pShadowDescription = pShadow->Description();
					if (pShadowDescription != NULL)
					{
						if ((pShadowDescription->m_nStyle == pDescription->m_nStyle)
						 && (pShadowDescription->m_nOffsetType == pDescription->m_nOffsetType)
						 && (pShadowDescription->m_lXOffset == pDescription->m_lXOffset)
						 && (pShadowDescription->m_lYOffset == pDescription->m_lYOffset))
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
			// Did not find Shadow, create a custom Shadow.
			
			if (m_nCustomShadowIndex != CB_ERR)
			{
				DeleteString(m_nCustomShadowIndex);
				m_nCustomShadowIndex = CB_ERR;
				m_pCustomShadow = NULL;
			}
			
			CString csName;
			ShadowName(csName, pDescription, TRUE);
			
			ASSERT(m_pCustomShadow == NULL);
			
			TRY
			{
				m_pCustomShadow = new CShadowElement(csName, pDescription);
			}
			END_TRY
			
			if (m_pCustomShadow != NULL)
			{
				CShadowSelectComboBoxItem* pItem = NULL;
				
				TRY
				{
					pItem = new CShadowSelectComboBoxItem(m_pCustomShadow);
				}
				END_TRY
		
				if (pItem != NULL)
				{
					m_nCustomShadowIndex = AddString((LPCSTR)pItem);
				}
			}
			
			if (m_nCustomShadowIndex != CB_ERR)
			{
				nIndex = m_nCustomShadowIndex;
			}
			else
			{
				nIndex = -1;
				
				delete m_pCustomShadow;
				m_pCustomShadow = NULL;
			}
		}
		
		// Set the index of the width that was found, -1 clears any selection.
		
		SetCurSel(nIndex);
	}
}

CShadowElement* CShadowSelectComboBox::Shadow(void)
{
	CShadowElement* pShadow = NULL;
	
	int nIndex = GetCurSel();
	
	if (nIndex != CB_ERR)
	{
		CShadowSelectComboBoxItem* pItem = (CShadowSelectComboBoxItem*)GetItemData(nIndex);
			
		if (pItem != NULL)
		{
			pShadow = pItem->Shadow();
		}
	}
	
	return pShadow;
}

void CShadowSelectComboBox::ShadowName(CString& csName, const ShadowDescription* pDescription, BOOL fCustom /*=FALSE*/)
{
	ASSERT(pDescription != NULL);

	csName.Empty();

	if (fCustom)
	{
		CString csType;

		TRY
		{
			csName.LoadString(IDS_CUSTOM_SHADOW);
			switch (pDescription->m_nOffsetType)
			{
				case ShadowFormat::Drop:
				{
					csType.LoadString(IDS_DROP_SHADOW);
					break;
				}
				case ShadowFormat::Extruded:
				{
					csType.LoadString(IDS_EXTRUDED_SHADOW);
					break;
				}
				default:
				{
					csType.LoadString(IDS_OTHER_SHADOW);
					break;
				}
			}
			csName += csType;
		}
		END_TRY
	}
	else
	{
		TRY
		{
			csName.LoadString(pDescription->m_uName);
		}
		END_TRY
	}
}

/////////////////////////////////////////////////////////////////////////////
// CShadowSelectComboBox message handlers


/////////////////////////////////////////////////////////////////////////////
// CFontDialogData

CFontDialogData::CFontDialogData()
{
	Data.m_nFace = -1;
	Data.m_fBold = FALSE;
	Data.m_fItalic = FALSE;
	Data.m_lSize = UNDEFINED_CFIXED;
	Data.m_lBaseSize = UNDEFINED_CFIXED;
	Data.m_lExpansion = UNDEFINED_CFIXED;
	Data.m_lSpacing = UNDEFINED_CFIXED;
	Data.m_Fill.SetType(FillFormatV1::FillNone);
	Data.m_Outline.SetType(OutlineFormat::OutlineNone);
	Data.m_Shadow.SetType(ShadowFormat::ShadowNone);
	Data.m_fWhiteOut = FALSE;
	Data.m_nUnderline = UNDERLINE_STYLE_None;
}

CFontDialogData::~CFontDialogData()
{
}

void CFontDialogData::FromStyle(CPmwDoc* pDoc, CTextStyle* pStyle)
{
	ASSERT(pDoc != NULL);
	ASSERT(pStyle != NULL);
	if ((pDoc != NULL) && (pStyle != NULL))
	{
		PMGFontServer* pFontServer = (PMGFontServer*)pDoc->get_font_server();
		DB_RECORD_NUMBER lFont = pStyle->Font();
		if (lFont > 0)
		{
			Data.m_nFace = pFontServer->font_record_to_face(lFont);
		}
		else
		{
			Data.m_nFace = -1;
		}
		Data.m_fBold = pStyle->Bold();
		Data.m_fItalic = pStyle->Italic();
		Data.m_lSize = pStyle->Size();
		Data.m_lBaseSize = pStyle->BaseSize();
		Data.m_lExpansion = pStyle->Expansion();
		Data.m_lSpacing = pStyle->Spacing();
		Data.m_Fill = pStyle->Fill();
		Data.m_Outline = pStyle->Outline();
		Data.m_Shadow = pStyle->Shadow();
		Data.m_fWhiteOut = pStyle->WhiteOut();
		Data.m_nUnderline = pStyle->Underline();
	}
}

void CFontDialogData::ToStyle(CPmwDoc* pDoc, CTextStyle* pStyle)
{
	ASSERT(pStyle != NULL);
	if (pStyle != NULL)
	{
		PMGFontServer* pFontServer = (PMGFontServer*)pDoc->get_font_server();
		int nFace = Data.m_nFace;
		if (nFace == -1)
		{
			pStyle->Font(0);
		}
		else
		{
			DB_RECORD_NUMBER dwRecord = pFontServer->font_face_to_record(nFace);
			if (dwRecord > 0)
			{
				pStyle->Font(dwRecord);												// The style gets its own reference on the font.
				pDoc->get_database()->free_font_record(dwRecord, TRUE);	// Free out temporary reference.
			}
		}
		pStyle->Bold(Data.m_fBold);
		pStyle->Italic(Data.m_fItalic);
		pStyle->Size(Data.m_lSize);
		pStyle->BaseSize(Data.m_lBaseSize);
		pStyle->Expansion(Data.m_lExpansion);
		pStyle->Spacing(Data.m_lSpacing);
		pStyle->Fill(Data.m_Fill);
		pStyle->Outline(Data.m_Outline);
		pStyle->Shadow(Data.m_Shadow);
		pStyle->WhiteOut(Data.m_fWhiteOut);
		pStyle->Underline(Data.m_nUnderline);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFontTypefacePropertyPage dialog

CFontTypefacePropertyPage::CFontTypefacePropertyPage(CFontDialogData* pFontData)
	: CPmwPropertyPage(CFontTypefacePropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CFontTypefacePropertyPage)
	//}}AFX_DATA_INIT

	m_pFontData = pFontData;
}

CFontTypefacePropertyPage::~CFontTypefacePropertyPage()
{
}

void CFontTypefacePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CFontTypefacePropertyPage)
	DDX_Control( pDX, IDC_COLOR, m_FillColor );
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		// Update the selected size.
		UpdateSize();
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CFontTypefacePropertyPage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CFontTypefacePropertyPage)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_LBN_SELCHANGE(IDC_FACE_LIST, OnSelchangeFaceList)
	ON_CBN_SELCHANGE(IDC_FONT_SIZE, OnSelchangeFontSize)
	ON_BN_CLICKED(IDC_UNDERLINE, OnUnderline)
	ON_BN_CLICKED(IDC_WHITE_BORDER, OnWhiteBorder)
	ON_CBN_SELCHANGE(IDC_FONT_STYLE, OnSelchangeFontStyle)
	ON_CBN_KILLFOCUS(IDC_FONT_SIZE, OnKillfocusFontSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFontTypefacePropertyPage::SetFillColor(COLOR Color)
{
	RColor rColor = RColorFromColor( Color );
	m_FillColor.SetColor(rColor);
}

void CFontTypefacePropertyPage::UpdateEffectsFillColor(void)
{
	CFontPropertySheet* pSheet = (CFontPropertySheet*)GetParent();
	if (pSheet != NULL)
	{
		CFontEffectsPropertyPage* pEffects = pSheet->GetEffectsPage();
		if (pEffects != NULL)
		{
			pEffects->SetFillColor(m_pFontData->Data.m_Fill.m_ForegroundColor);
		}
	}
}

BOOL CFontTypefacePropertyPage::UpdateSize(void)
{
	CFixed lCurrentSize = m_pFontData->Data.m_lBaseSize;

	CString csSize;

	int nIndex = m_Size.GetCurSel();
	if (nIndex != CB_ERR)
	{
		m_Size.GetLBText(nIndex, csSize);
	}
	else
	{
		m_Size.GetWindowText(csSize);
	}

	if (!Util::ValidDouble(csSize))
	{
		if (lCurrentSize == UNDEFINED_CFIXED)
		{
			csSize.Empty();
		}
		else
		{
			Util::FormatDouble(MakeDouble(lCurrentSize), -1, csSize);
		}
	}
	else
	{
		double dSize = atof(csSize);
		if (dSize < 4.0)
		{
			dSize = 4.0;
		}
		if (dSize > 1000.0)
		{
			dSize = 1000.0;
		}
		Util::FormatDouble(dSize, -1, csSize);
		m_pFontData->Data.m_lBaseSize = MakeFixed(atof(csSize));
	}

	nIndex = m_Size.FindStringExact(-1, csSize);
	if (nIndex != CB_ERR)
	{
		m_Size.SetCurSel(nIndex);
	}
	else
	{
		m_Size.SetWindowText(csSize);
	}

	return lCurrentSize != m_pFontData->Data.m_lBaseSize;
}

/////////////////////////////////////////////////////////////////////////////
// CFontTypefacePropertyPage message handlers

BOOL CFontTypefacePropertyPage::OnInitDialog()
{
	CPmwPropertyPage::OnInitDialog();

	m_Typeface.SubclassDlgItem(IDC_FACE_LIST, this);

	// Add all the chooseable faces to the list.
	FaceEntry* pEntry;
	int nEntry;

	m_Typeface.SetRedraw(FALSE);

	BOOL fIsRegistered = GET_PMWAPP()->user_is_registered();

	for (pEntry = typeface_server.face_list.first_face(), nEntry = 0;
		  pEntry != NULL;
		  pEntry = pEntry->next_face(), nEntry++)
	{
		if (pEntry->is_chooseable(fIsRegistered))
		{
			int nIndex = m_Typeface.AddString(pEntry->get_name());
			if (nIndex != LB_ERR)
			{
				m_Typeface.SetItemData(nIndex, (DWORD)nEntry);
				if (nEntry == m_pFontData->Data.m_nFace)
				{
					m_Typeface.SetCurSel(nIndex);
				}
			}
		}
	}

	m_Typeface.SetRedraw(TRUE);

	m_Style.SubclassDlgItem(IDC_FONT_STYLE, this);

	// Initialize style list box.
	static UINT StyleNames[] =
	{
		IDS_REGULAR,
		IDS_BOLD,
		IDS_ITALIC,
		IDS_BOLDITALIC
	};

	for (int nName = 0; nName < sizeof(StyleNames)/sizeof(StyleNames[0]); nName++)
	{
		CString csName;
		TRY
		{
			csName.LoadString(StyleNames[nName]);
			m_Style.AddString(csName);
		}
		END_TRY;
	}

	// Figure out the style to use.
	int nStyle = -1;
	if ((m_pFontData->Data.m_fBold != -1) && (m_pFontData->Data.m_fItalic != -1))
	{
		nStyle = 0;
		if (m_pFontData->Data.m_fBold != 0)
		{
			nStyle += 1;
		}
		if (m_pFontData->Data.m_fItalic != 0)
		{
			nStyle += 2;
		}
	}

	m_Style.SetCurSel(nStyle);

	m_Size.SubclassDlgItem(IDC_FONT_SIZE, this);
	UpdateSize();

//	m_FillColor.SubclassDlgItem(IDC_COLOR, this);
	m_FillColor.SetColor( RColorFromColor( m_pFontData->Data.m_Fill.m_ForegroundColor ) );

	if (m_pFontData->Data.m_nUnderline == UNDERLINE_STYLE_Undefined)
	{
		CheckDlgButton(IDC_UNDERLINE, 2);
	}
	else
	{
		CheckDlgButton(IDC_UNDERLINE, (m_pFontData->Data.m_nUnderline == UNDERLINE_STYLE_None) ? 0 : 1);
	}
	
	if (m_pFontData->Data.m_fWhiteOut == -1)
	{
		CheckDlgButton(IDC_WHITE_BORDER, 2);
	}
	else
	{
		CheckDlgButton(IDC_WHITE_BORDER, (m_pFontData->Data.m_fWhiteOut == 0) ? 0 : 1);
	}
	
	return TRUE;
}

void CFontTypefacePropertyPage::OnColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_FillColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_FillColor.SetColor( rColor ) ;

		COLOR Color = ColorFromRColor( rColor );
		m_pFontData->Data.m_Fill.m_ForegroundColor = Color;
		RedrawPreview();

		UpdateEffectsFillColor();
	}
}

void CFontTypefacePropertyPage::OnSelchangeFaceList() 
{
	int nIndex = m_Typeface.GetCurSel();
	if (nIndex != LB_ERR)
	{
		int nFace = (int)m_Typeface.GetItemData(nIndex);
		if (nFace != (int)LB_ERR)
		{
			m_pFontData->Data.m_nFace = nFace;
			RedrawPreview();
		}
	}
}

void CFontTypefacePropertyPage::OnSelchangeFontStyle() 
{
	int nStyle = m_Style.GetCurSel();
	if (nStyle != LB_ERR)
	{
		BOOL fBold = (nStyle & 1) != 0;
		BOOL fItalic = (nStyle & 2) != 0;
		m_pFontData->Data.m_fBold = fBold;
		m_pFontData->Data.m_fItalic = fItalic;
		RedrawPreview();
	}
}

void CFontTypefacePropertyPage::OnSelchangeFontSize() 
{
	if (UpdateSize())
	{
		RedrawPreview();
	}
#if 0
	int nIndex = m_Size.GetCurSel();
	if (nIndex != -1)
	{
		CString csSize;
		m_Size.GetLBText(nIndex, csSize);
		m_pFontData->Data.m_lBaseSize = MakeFixed(atof(csSize));
		RedrawPreview();
	}
#endif
}

void CFontTypefacePropertyPage::OnKillfocusFontSize() 
{
	CFixed lSize = m_pFontData->Data.m_lBaseSize;
	UpdateData();
	if (lSize != m_pFontData->Data.m_lBaseSize)
	{
		RedrawPreview();
	}
}

void CFontTypefacePropertyPage::OnUnderline() 
{
	UINT uCheck = IsDlgButtonChecked(IDC_UNDERLINE);
	if (uCheck == 2)
	{
		CheckDlgButton(IDC_UNDERLINE, 0);
		uCheck = 0;
	}
	m_pFontData->Data.m_nUnderline = (uCheck == 1) ? UNDERLINE_STYLE_Single : UNDERLINE_STYLE_None;
	RedrawPreview();
}

void CFontTypefacePropertyPage::OnWhiteBorder() 
{
	UINT uCheck = IsDlgButtonChecked(IDC_WHITE_BORDER);
	if (uCheck == 2)
	{
		CheckDlgButton(IDC_WHITE_BORDER, 0);
		uCheck = 0;
	}
	m_pFontData->Data.m_fWhiteOut = (uCheck == 1);
}


/////////////////////////////////////////////////////////////////////////////
// CFontEffectsPropertyPage property page

CFontEffectsPropertyPage::CFontEffectsPropertyPage(CFontDialogData* pFontData)
	: CPmwPropertyPage(CFontEffectsPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CFontEffectsPropertyPage)
	//}}AFX_DATA_INIT

	m_pFontData = pFontData;
}

CFontEffectsPropertyPage::~CFontEffectsPropertyPage()
{
}

void CFontEffectsPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontEffectsPropertyPage)
	DDX_Control( pDX, IDC_COLOR, m_FillColor );
	DDX_Control( pDX, IDC_OUTLINE_COLOR, m_OutlineColor );
	DDX_Control( pDX, IDC_SHADOW_COLOR, m_ShadowColor );
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CFontEffectsPropertyPage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CFontEffectsPropertyPage)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_OUTLINE_COLOR, OnOutlineColor)
	ON_BN_CLICKED(IDC_SHADOW_COLOR, OnShadowColor)
	ON_CBN_SELCHANGE(IDC_FONT_OUTLINE, OnSelchangeFontOutline)
	ON_CBN_SELCHANGE(IDC_FONT_PATTERN, OnSelchangeFontPattern)
	ON_CBN_SELCHANGE(IDC_FONT_SHADOW, OnSelchangeFontShadow)
	ON_BN_CLICKED(IDC_OUTLINE_MORE, OnOutlineMore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFontEffectsPropertyPage::SetFillColor(COLOR Color)
{
	RColor rColor = RColorFromColor( Color );
	m_FillColor.SetColor(rColor);
}

void CFontEffectsPropertyPage::UpdateTypefaceFillColor(void)
{
	CFontPropertySheet* pSheet = (CFontPropertySheet*)GetParent();
	if (pSheet != NULL)
	{
		CFontTypefacePropertyPage* pTypeface = pSheet->GetTypefacePage();
		if (pTypeface != NULL)
		{
			pTypeface->SetFillColor(m_pFontData->Data.m_Fill.m_ForegroundColor);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFontEffectsPropertyPage message handlers

BOOL CFontEffectsPropertyPage::OnInitDialog()
{
	static OutlineDescription InitialOutlineDescription;
	static ShadowDescription InitialShadowDescription;

	CPmwPropertyPage::OnInitDialog();

	m_Fill.SubclassDlgItem(IDC_FONT_PATTERN, this);
	if (m_pFontData->Data.m_Fill.m_nPattern == FillFormatV1::Undefined)
	{
		m_Fill.SetCurSel(-1);
	}
	else
	{
		m_Fill.SetCurSel(m_pFontData->Data.m_Fill.m_nPattern+1);
	}

//	m_FillColor.SubclassDlgItem(IDC_COLOR, this);
	m_FillColor.SetColor(RColorFromColor( m_pFontData->Data.m_Fill.m_ForegroundColor ));
	
	m_Outline.SubclassDlgItem(IDC_FONT_OUTLINE, this);
	m_Outline.SizeItems();
   m_Outline.Outlines(&(OutlineDescriptions[0]), NUMBER_OF_OUTLINE_DESCRIPTIONS);
	InitialOutlineDescription.m_uName = 0;
	InitialOutlineDescription.m_nStyle = m_pFontData->Data.m_Outline.m_nStyle;
	InitialOutlineDescription.m_nWidthType = m_pFontData->Data.m_Outline.m_nWidthType;
	InitialOutlineDescription.m_lWidth = m_pFontData->Data.m_Outline.m_lWidth;
	m_Outline.Outline(&InitialOutlineDescription);

//	m_OutlineColor.SubclassDlgItem(IDC_OUTLINE_COLOR, this);
	m_OutlineColor.SetColor(RColorFromColor( m_pFontData->Data.m_Outline.m_ForegroundColor ));

	m_Shadow.SubclassDlgItem(IDC_FONT_SHADOW, this);
	m_Shadow.SizeItems();
   m_Shadow.Shadows(&(ShadowDescriptions[0]), NUMBER_OF_SHADOW_DESCRIPTIONS);
	InitialShadowDescription.m_uName = 0;
	InitialShadowDescription.m_nStyle = m_pFontData->Data.m_Shadow.m_nStyle;
	InitialShadowDescription.m_nOffsetType = m_pFontData->Data.m_Shadow.m_nOffsetType;
	InitialShadowDescription.m_lXOffset = m_pFontData->Data.m_Shadow.m_lXOffset;
	InitialShadowDescription.m_lYOffset = m_pFontData->Data.m_Shadow.m_lYOffset;
	m_Shadow.Shadow(&InitialShadowDescription);

//	m_ShadowColor.SubclassDlgItem(IDC_SHADOW_COLOR, this);
	m_ShadowColor.SetColor(RColorFromColor( m_pFontData->Data.m_Shadow.m_ForegroundColor ));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFontEffectsPropertyPage::OnSelchangeFontPattern() 
{
	int nPattern = m_Fill.GetCurSel();
	if (nPattern == CB_ERR)
	{
		nPattern = FillFormatV1::Undefined;
	}
	else if (nPattern == 0)
	{
		nPattern = FillFormatV1::None;
	}
	else
	{
		nPattern--;
	}
	m_pFontData->Data.m_Fill.m_nPattern = nPattern;

	if (m_pFontData->Data.m_Fill.m_nPattern != FillFormatV1::None)
	{
		COLOR Color = m_pFontData->Data.m_Fill.m_ForegroundColor;
		if (Color == TRANSPARENT_COLOR)
		{
			m_pFontData->Data.m_Fill.m_ForegroundColor = COLOR_BLACK;
			m_FillColor.SetColor( RColorFromColor( m_pFontData->Data.m_Fill.m_ForegroundColor ) );
			UpdateTypefaceFillColor();
		}
	}

	RedrawPreview();
}

void CFontEffectsPropertyPage::OnSelchangeFontOutline() 
{
	COutlineElement* pOutline = m_Outline.Outline();
	if (pOutline != NULL)
	{
		const OutlineDescription* pDescription = pOutline->Description();
		if (pDescription != NULL)
		{
			m_pFontData->Data.m_Outline.m_nStyle = pDescription->m_nStyle;
			m_pFontData->Data.m_Outline.m_nWidthType = pDescription->m_nWidthType;
			m_pFontData->Data.m_Outline.m_lWidth = pDescription->m_lWidth;

			if (m_pFontData->Data.m_Outline.m_nStyle != OutlineFormat::None)
			{
				COLOR Color = m_pFontData->Data.m_Outline.m_ForegroundColor;
				if (Color == TRANSPARENT_COLOR)
				{
					Color = m_pFontData->Data.m_Fill.m_ForegroundColor;
					Color = (Color != COLOR_BLACK) ? COLOR_BLACK : COLOR_LIGHT_GRAY;
					m_pFontData->Data.m_Outline.m_ForegroundColor = Color;
					m_OutlineColor.SetColor(RColorFromColor( m_pFontData->Data.m_Outline.m_ForegroundColor ));
				}
			}

			RedrawPreview();
		}
	}
}

void CFontEffectsPropertyPage::OnOutlineMore() 
{
	if (m_Outline.ChooseCustomOutline(72))
	{
		OnSelchangeFontOutline();
	}
}

void CFontEffectsPropertyPage::OnSelchangeFontShadow() 
{
	CShadowElement* pShadow = m_Shadow.Shadow();
	if (pShadow != NULL)
	{
		const ShadowDescription* pDescription = pShadow->Description();
		if (pDescription != NULL)
		{
			m_pFontData->Data.m_Shadow.m_nStyle = pDescription->m_nStyle;
			m_pFontData->Data.m_Shadow.m_nOffsetType = pDescription->m_nOffsetType;
			m_pFontData->Data.m_Shadow.m_lXOffset = pDescription->m_lXOffset;
			m_pFontData->Data.m_Shadow.m_lYOffset = pDescription->m_lYOffset;

			if (m_pFontData->Data.m_Shadow.m_nStyle != ShadowFormat::None)
			{
				COLOR Color = m_pFontData->Data.m_Shadow.m_ForegroundColor;
				if (Color == TRANSPARENT_COLOR)
				{
					Color = m_pFontData->Data.m_Fill.m_ForegroundColor;
					Color = (Color != COLOR_BLACK) ? COLOR_BLACK : COLOR_LIGHT_GRAY;
					m_pFontData->Data.m_Shadow.m_ForegroundColor = Color;
					m_ShadowColor.SetColor(RColorFromColor( m_pFontData->Data.m_Shadow.m_ForegroundColor ));
				}
			}

			RedrawPreview();
		}
	}
}

void CFontEffectsPropertyPage::OnColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_FillColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_FillColor.SetColor( rColor ) ;

		COLOR Color = ColorFromRColor( rColor );
		m_pFontData->Data.m_Fill.m_ForegroundColor = Color;
		RedrawPreview();

		UpdateTypefaceFillColor();
	}
}

void CFontEffectsPropertyPage::OnOutlineColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_OutlineColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_OutlineColor.SetColor( rColor );
		COLOR Color = ColorFromRColor( rColor );

		if (Color != TRANSPARENT_COLOR)
		{
			if (m_pFontData->Data.m_Outline.m_nStyle == OutlineFormat::None)
			{
				static OutlineDescription MediumOutlineDescription = { IDS_MEDIUM, OutlineFormat::Inflate, OutlineFormat::Relative, MakeFixed(TEXT_OUTLINE_MEDIUM_FRACTION) };
				m_pFontData->Data.m_Outline.SetType(OutlineFormat::OutlineMedium);
				m_Outline.Outline(&MediumOutlineDescription);
			}
		}

		m_pFontData->Data.m_Outline.m_ForegroundColor = Color;
		RedrawPreview();
	}
}

void CFontEffectsPropertyPage::OnShadowColor() 
{
	RWinColorDlg dlg(this, kShowTransparent) ;
	dlg.SetColor( m_ShadowColor.GetColor() ) ;

	if (IDOK == dlg.DoModal())
	{
		RColor rColor = dlg.SelectedColor() ;
		m_ShadowColor.SetColor( rColor );
		COLOR Color = ColorFromRColor( rColor );

		if (Color != TRANSPARENT_COLOR)
		{
			if (m_pFontData->Data.m_Shadow.m_nStyle == OutlineFormat::None)
			{
				static ShadowDescription DropDownRightShadowDescription = { IDS_DROP_DOWN_RIGHT, ShadowFormat::Drop, ShadowFormat::Relative, MakeFixed(TEXT_SHADOW_FRACTION), -MakeFixed(TEXT_SHADOW_FRACTION) };
				m_pFontData->Data.m_Shadow.SetType(ShadowFormat::ShadowDropDownRight, MakeFixed(TEXT_SHADOW_FRACTION));
				m_Shadow.Shadow(&DropDownRightShadowDescription);
			}
		}

		m_pFontData->Data.m_Shadow.m_ForegroundColor = Color;
		RedrawPreview();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFontPreviewButton

CFontPreviewButton::CFontPreviewButton()
{
}

CFontPreviewButton::~CFontPreviewButton()
{
}

BEGIN_MESSAGE_MAP(CFontPreviewButton, CButton)
	//{{AFX_MSG_MAP(CFontPreviewButton)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontPreviewButton message handlers

BOOL CFontPreviewButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CFontPropertySheet

IMPLEMENT_DYNAMIC(CFontPropertySheet, CPmwPropertySheet)

CFontPropertySheet::CFontPropertySheet(CPmwDoc* pDoc, CTextStyle* pStyle, BOOL fShowActualSize, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	  m_TypefacePage(&m_NewFontData),
	  m_EffectsPage(&m_NewFontData)
{
	ASSERT(pDoc != NULL);
	ASSERT(pStyle != NULL);

	m_pDoc = pDoc;
	m_pStyle = pStyle;

	m_fShowActualSize = fShowActualSize;

	AddPages();
}

CFontPropertySheet::~CFontPropertySheet()
{
}

BEGIN_MESSAGE_MAP(CFontPropertySheet, CPmwPropertySheet)
	//{{AFX_MSG_MAP(CFontPropertySheet)
	ON_WM_DRAWITEM()
   ON_COMMAND(IDC_SHOW_ACTUAL_SIZE, OnShowActualSize)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TCN_SELCHANGE, AFX_IDC_TAB_CONTROL, OnTabChange)
	ON_MESSAGE(WM_MOVE_PREVIEW_TO_TOP, OnMovePreviewToTop)
	ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

void CFontPropertySheet::AddPages(void)
{
	AddPage(&m_TypefacePage);
	AddPage(&m_EffectsPage);
}

BOOL CFontPropertySheet::GetStyleChanges(CPmwDoc* pDoc, CTextStyle* pStyle)
{
	ASSERT(pStyle != NULL);

	BOOL fResult = FALSE;

	if (m_NewFontData != m_OriginalFontData)
	{
		m_NewFontData.ToStyle(pDoc, pStyle);
		fResult = TRUE;
	}

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CFontPropertySheet message handlers

BOOL CFontPropertySheet::OnInitDialog()
{
	// Get the current font attributes.
	m_OriginalFontData.FromStyle(m_pDoc, m_pStyle);
	m_NewFontData = m_OriginalFontData;

	CPmwPropertySheet::OnInitDialog();

	// Hide the apply button.
   CWnd* pApplyButton = GetDlgItem(ID_APPLY_NOW);
   if (pApplyButton != NULL)
	{
      pApplyButton->ShowWindow(SW_HIDE);
	}

   CWnd* pHelpButton = GetDlgItem(IDHELP);
   if (pHelpButton != NULL)
	{
      pHelpButton->ShowWindow(SW_HIDE);
	}

	// Get preview control (owner draw button) from the first page and make it our child.
   CWnd* pPreview = m_TypefacePage.GetDlgItem(IDC_FONT_PREVIEW);
	if (pPreview != NULL)
	{
		pPreview->GetWindowRect(m_crPreview);
		ScreenToClient(m_crPreview);
		pPreview->SetParent(this);
		pPreview->SetWindowPos(&wndTop, m_crPreview.left, m_crPreview.top, 0, 0, SWP_NOSIZE);
		m_Preview.SubclassDlgItem(IDC_FONT_PREVIEW, this);
	}

	// Convert m_crPreview to control coordinates
	m_crPreview.SetRect(0, 0, m_crPreview.Width(), m_crPreview.Height());
	
	// Get "show actual size" control (check box) from the first page and make it our child.
   CWnd* pShowActualSize = m_TypefacePage.GetDlgItem(IDC_SHOW_ACTUAL_SIZE);
	if (pShowActualSize != NULL)
	{
		CRect crRect;
		pShowActualSize->GetWindowRect(crRect);
		ScreenToClient(crRect);
		pShowActualSize->SetParent(this);
		pShowActualSize->SetWindowPos(&wndTop, crRect.left, crRect.top, 0, 0, SWP_NOSIZE);
		CheckDlgButton(IDC_SHOW_ACTUAL_SIZE, GetShowActualSize());
	}
	
	return TRUE;
}

void CFontPropertySheet::OnShowActualSize(void)
{
	m_fShowActualSize = IsDlgButtonChecked(IDC_SHOW_ACTUAL_SIZE);
	m_Preview.Invalidate();
}

void CFontPropertySheet::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	switch (nIDCtl)
	{
		case IDC_FONT_PREVIEW:
		{
			DrawPreview(lpDrawItemStruct);
			break;
		}
		default:
		{
			CPmwPropertySheet::OnDrawItem(nIDCtl, lpDrawItemStruct);
			break;
		}
	}
}

void CFontPropertySheet::DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Get the DC to use.
	HDC hDC = lpDrawItemStruct->hDC;

	CRect crFrame(m_crPreview);
	crFrame.InflateRect(1000, -2);

	// Build the redisplay context.
	RedisplayContext rc;

	// Copy over any scaling factors from the view
	CPmwView* pView = GetCurrentView();
	if (pView != NULL)
	{
		RedisplayContext* pViewRC = pView->get_rc();
		if (pViewRC != NULL)
		{
			rc.SetScaling(pViewRC->GetScaleNumerator(), pViewRC->GetScaleDenominator());
		}
	}

	rc.destination_hdc = hDC;
	rc.hwnd = lpDrawItemStruct->hwndItem;

	rc.scaled_source_x0 =
 		rc.scaled_source_y0 = 0;

	rc.destination_rect = crFrame;

	rc.destination_x0 = 0;
	rc.destination_y0 = 0;

	rc.x_resolution = GetDeviceCaps(hDC, LOGPIXELSX);
	rc.y_resolution = GetDeviceCaps(hDC, LOGPIXELSY);

	rc.outline_gamma_curve =
		rc.bitmap_gamma_curve = screen_gamma_curve;

	rc.set_check_interrupt(standard_check_interrupt, (void*)&rc);
	rc.terminate_interrupts = TRUE;

	rc.clip_rect = crFrame;

	CRect crClip(m_crPreview);
	crClip.InflateRect(-2, -2);
	CRgn ClipRegion;
	ClipRegion.CreateRectRgn(crClip.left, crClip.top, crClip.right, crClip.bottom);
	rc.clip_region = (HRGN)(ClipRegion.m_hObject);

	// Initialize the redisplay context so we can use it.

	rc.set_info(hDC);
	rc.screen_to_pbox(crFrame, &rc.source_pbox);

	// Realize our palette.
	HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
	HPALETTE hOldPal = SelectPalette(hDC, hOurPal, FALSE);
	RealizePalette(hDC);

	// Draw the frame background and border.
	HPEN hOldPen;
	HBRUSH hOldBrush;

	CRect crBorder(m_crPreview);

	// Account for screwy Window drawing behavior!
	crBorder.right--;
	crBorder.bottom--;

	hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
	hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
	Rectangle(hDC, crBorder.left, crBorder.top, crBorder.right, crBorder.bottom);
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	// Account for screwy Window drawing behavior!
	crBorder.left++;
	crBorder.top++;

	hOldPen = (HPEN)SelectObject(hDC, GetStockObject(NULL_PEN));
	hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	Rectangle(hDC, crBorder.left, crBorder.top, crBorder.right, crBorder.bottom);
	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);

	// Build the font parameters to use for the preview. Undefined
	// values are forced to some default value so that the preview
	// is always shown.

	CFontDialogData PreviewData;
	PreviewData = m_NewFontData;

	BOOL fStretched = m_OriginalFontData.Data.m_lBaseSize != m_OriginalFontData.Data.m_lSize;

	if (PreviewData.Data.m_nFace == -1)
	{
		ASSERT(typeface_server.replacement_face != -1);
		PreviewData.Data.m_nFace = typeface_server.replacement_face;
	}

	if (PreviewData.Data.m_fBold == -1)
	{
		PreviewData.Data.m_fBold = 0;
	}

	if (PreviewData.Data.m_fItalic == -1)
	{
		PreviewData.Data.m_fItalic = 0;
	}

	if (GetShowActualSize())
	{
		if (PreviewData.Data.m_lSize == UNDEFINED_CFIXED)
		{
			PreviewData.Data.m_lSize = MakeFixed(36);
		}

		if (PreviewData.Data.m_lBaseSize == UNDEFINED_CFIXED)
		{
			PreviewData.Data.m_lBaseSize = MakeFixed(36);
		}

		if (m_OriginalFontData.Data.m_lBaseSize != UNDEFINED_CFIXED)
		{
			if (fStretched)
			{
				PreviewData.Data.m_lBaseSize = m_OriginalFontData.Data.m_lBaseSize;
				PreviewData.Data.m_lSize = m_OriginalFontData.Data.m_lSize;
			}
			else
			{
				if ((PreviewData.Data.m_lBaseSize != m_OriginalFontData.Data.m_lBaseSize)
				 && (m_OriginalFontData.Data.m_lBaseSize != 0))
				{
					PreviewData.Data.m_lSize = SafeMulDivLong(PreviewData.Data.m_lSize, PreviewData.Data.m_lBaseSize, m_OriginalFontData.Data.m_lBaseSize);
				}
			}
		}
	}
	else
	{
		double dPointSize = (((double)(crClip.Height()))/((double)(rc.GetScaledYResolution())))*72.0*0.6;
		PreviewData.Data.m_lSize = MakeFixed(dPointSize);
	}

	PreviewData.Data.m_lBaseSize = PreviewData.Data.m_lSize;

	if (PreviewData.Data.m_lExpansion == UNDEFINED_CFIXED)
	{
		PreviewData.Data.m_lExpansion = MakeFixed(1);
	}

	if (PreviewData.Data.m_lSpacing == UNDEFINED_CFIXED)
	{
		PreviewData.Data.m_lSpacing = MakeFixed(1);
	}

	// Figure out the style to use.
	int nStyle = 0;
	if (PreviewData.Data.m_fBold == 1)
	{
		nStyle |= FONT_STYLE_Bold;
	}
	if (PreviewData.Data.m_fItalic == 1)
	{
		nStyle |= FONT_STYLE_Italic;
	}

	// Get the entry for this face.
	FaceEntry* pEntry;
	int nFaceToUse = PreviewData.Data.m_nFace;
	int nFontNumber = -1;

	if ((pEntry = typeface_server.face_list.get_face(nFaceToUse)) != NULL)
	{
		VARIATION_TYPE nVariationType = pEntry->get_variation_type(nStyle);

		// Check if it's a PMG sourced font. If so, the face will be in the
		// special preview font file.
		if ((nVariationType == VARIATION_TYPE_VIRTUAL)
		 || (nVariationType == VARIATION_TYPE_REGISTRATION))
		{
			// We want to use the preview font for "virtual" (PMW.FFL) fonts.
			// Check if the font has an entry in the preview font.
			nFontNumber = pEntry->get_preview_font_number(nStyle);
			if (nFontNumber != -1)
			{
				// Set the font number back to -1 in case we fail. It will
				// be reloaded if we can sucessfully use the preview font.
				nFontNumber = -1;

				// We want to use the font preview.
				PCSTR pszPreviewFaceName = typeface_server.preview_face_name();
				if (pszPreviewFaceName != NULL)
				{
					int nFace;
					if ((nFace = typeface_server.find_face(pszPreviewFaceName, FALSE)) != -1)
					{
						nFaceToUse = nFace;
						nFontNumber = pEntry->get_preview_font_number(nStyle);
					}
				}
			}
		}
	}

	PreviewData.Data.m_nFace = nFaceToUse;

	if (PreviewData.Data.m_Fill.m_nPattern == FillFormatV1::Undefined)
	{
		PreviewData.Data.m_Fill.m_nPattern = FillFormatV1::Black;
	}

	if (PreviewData.Data.m_Fill.m_ForegroundColor == UNDEFINED_COLOR)
	{
		PreviewData.Data.m_Fill.m_ForegroundColor = COLOR_BLACK;
	}

	if (PreviewData.Data.m_Outline.m_nStyle == OutlineFormat::Undefined)
	{
		PreviewData.Data.m_Outline.SetType(OutlineFormat::OutlineNone);
	}

	if (PreviewData.Data.m_Outline.m_ForegroundColor == UNDEFINED_COLOR)
	{
		PreviewData.Data.m_Outline.m_ForegroundColor = COLOR_BLACK;
	}

	if (PreviewData.Data.m_Shadow.m_nStyle == ShadowFormat::Undefined)
	{
		PreviewData.Data.m_Shadow.SetType(ShadowFormat::ShadowNone);
	}

	if (PreviewData.Data.m_Shadow.m_ForegroundColor == UNDEFINED_COLOR)
	{
		PreviewData.Data.m_Shadow.m_ForegroundColor = COLOR_BLACK;
	}

	if (PreviewData.Data.m_nUnderline == -1)
	{
		PreviewData.Data.m_nUnderline = UNDERLINE_STYLE_None;
	}

	// Don't allow invisible fonts.
	if ((PreviewData.Data.m_Fill.m_ForegroundColor == COLOR_WHITE)
	 && (PreviewData.Data.m_Outline.m_ForegroundColor == COLOR_WHITE)
	 && (PreviewData.Data.m_Shadow.m_ForegroundColor == COLOR_WHITE))
	{
		PreviewData.Data.m_Fill.m_ForegroundColor = COLOR_BLACK;
		PreviewData.Data.m_Outline.m_ForegroundColor = COLOR_BLACK;
		PreviewData.Data.m_Shadow.m_ForegroundColor = COLOR_BLACK;
	}

	PreviewData.Data.m_fWhiteOut = (SHORT)FALSE;

	// Get a pointer to the database.
	PMGDatabasePtr pDatabase = m_pDoc->get_database();

	// We need a text style to show for this font. */
	CTextStyle Style(pDatabase);

	// Get a database record for this face.
	// This will be a 'temporary' reference which we must free.

	PMGFontServer* pFontServer = (PMGFontServer*)m_pDoc->get_font_server();

	// Set the face in the style. The style gets its own reference.
	pFontServer->SetDefault(&Style);			// For whatever we don't change.

	// Set the style to the current font attributes. Note that the face
	// may not be the actual selected typeface, but may be the "preview"
	// font instead.
	PreviewData.ToStyle(m_pDoc, &Style);

	Style.LeadingType(LEADING_points);
	Style.Leading(MakeFixed(MulDiv((int)(rc.screen_y_to_page(crFrame.Height())), 72, PAGE_RESOLUTION)));

	// Set the typeface to use for the preview.
	DB_RECORD_NUMBER dwRecord = pFontServer->font_face_to_record(PreviewData.Data.m_nFace);
	Style.Font(dwRecord);

	// Free our temporary reference.
	pDatabase->free_font_record(dwRecord, TRUE);

	Style.UpdateFontMetrics();

	// Create the frame object now.
	FRAMEOBJ_CREATE_STRUCT fcs;

	fcs.alignment = ALIGN_center;
	fcs.vert_alignment = ALIGN_top;
	fcs.bound = rc.source_pbox;

	CFrameObject* pObject;

	if ((pObject = pDatabase->create_frame_object(&fcs)) != NULL)
	{
		CTextRecord* pTextRecord = pObject->LockTextRecord();
		if (pTextRecord != NULL)
		{
			if(m_fShowActualSize && fStretched)
				pTextRecord->SetStretch(MakeFixed(1), MakeFixed(1), PreviewData.Data.m_lExpansion);
			
			pTextRecord->ChangeParagraphStyle(0, 0, &Style);
			pTextRecord->release();
		}

		// Defeat the dastardly "draw gray border" behavior.
		pObject->remove_select_flags(SELECT_FLAG_boundary);

		// This object is not on the page!
		pObject->OnPage(FALSE);

		// Prepare to add some text.
		CTxp Txp(pDatabase);

		CHARACTER Text[4];

		if (nFontNumber == -1)
		{
			Text[0] = 'T';
			Text[1] = 'y';
			Text[2] = 'p';
			Text[3] = 'e';
		}
		else
		{
			// We substituted the preview font.
			int nChBase = nFontNumber*4 + '!';
			Text[0] = nChBase++;
			Text[1] = nChBase++;
			Text[2] = nChBase++;
			Text[3] = nChBase++;
		}

		// Stick in some text.
		Txp.Init(pObject, 0);
		Txp.SetHorizontalAlignment(ALIGN_center);
		Txp.Style(Style);
		Txp.InsertText(Text, 4);

		// We have the object.

		// And draw the object!
		UpdateStatePtr ustate;
		if ((ustate = pObject->update(&rc, &fcs.bound, crFrame, NULL, REFRESH_ALL)) != NULL)
		{
			// See what happened.
			switch (ustate->type)
			{
				case UPDATE_TYPE_Interrupted:
				{
					// This should not happen.
					delete ustate;

					// FALL THROUGH TO...
				}
				case UPDATE_TYPE_Terminated:
				{
					// Try again later.
					::InvalidateRect(lpDrawItemStruct->hwndItem, crFrame, FALSE);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		pObject->destroy();			// Remove from the document.
		delete pObject;
	}

	// Select the old palette back.
	if (hOldPal != NULL)
	{
		SelectPalette(hDC, hOldPal, FALSE);
	}
}

BOOL CFontPropertySheet::OnTabChange(UINT id, NMHDR* pNotifyStruct, LRESULT* pResult)
{
	PostMessage(WM_MOVE_PREVIEW_TO_TOP, 0, 0);

	return FALSE;
}

LRESULT CFontPropertySheet::OnMovePreviewToTop(WPARAM wParam, LPARAM lParam)
{
	m_Preview.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

   CWnd* pShowActualSize = GetDlgItem(IDC_SHOW_ACTUAL_SIZE);
	if (pShowActualSize != NULL)
	{
		pShowActualSize->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}

	return 0L;
}

void CFontPropertySheet::OnOK()
{
	// Copy selections to style.
	CPmwPropertySheet::OnOK();
}

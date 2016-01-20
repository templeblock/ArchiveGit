/*
// $Workfile: WARPFX.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFX.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.2   07 Nov 1996 14:24:44   johno
// Moved strings to resource DLL
// 
//    Rev 1.1   26 Jun 1996 14:23:16   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:35:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:34   FRED
// Initial revision.
// 
//    Rev 1.14   04 Sep 1995 09:03:42   FRED
// Got rid of GetParent() error.
// 
//    Rev 1.13   03 Sep 1995 17:58:00   FRED
// Warp text dialog works with new tabbed dialog code
// 
//    Rev 1.12   03 Sep 1995 15:54:46   FRED
//  
// 
//    Rev 1.11   02 Sep 1995 15:06:08   FRED
// More changes to make help work
// 
//    Rev 1.10   21 Aug 1995 09:34:46   FRED
// Fixed bug
// 
//    Rev 1.9   19 Aug 1995 17:49:28   FRED
// Maximum line sizes
// 
//    Rev 1.8   17 May 1995 14:26:40   FRED
// 
//    Rev 1.7   13 May 1995 11:10:04   FRED
// More warp preview stuffwarpshap.h
// 
//    Rev 1.6   12 May 1995 14:55:24   FRED
// More warp effects preview stuff
// 
//    Rev 1.5   12 May 1995 10:34:50   FRED
// Warp text preview
// 
//    Rev 1.4   11 May 1995 08:51:36   FRED
// Choose line width dialog
// 
//    Rev 1.3   10 May 1995 12:46:42   FRED
// Shadows for warp text objects.
// 
// 
//    Rev 1.2   03 May 1995 10:41:48   FRED
//  colorsel.cpp
// 
//    Rev 1.1   02 May 1995 15:37:56   FRED
// 
//    Rev 1.0   27 Apr 1995 13:30:10   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "warpobj.h"
#include "warpfx.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWarpTextEffectsPage property page

CWarpTextEffectsPage::CWarpTextEffectsPage(CPmwView* pView)
	: CPmwPropertyPage(CWarpTextEffectsPage::IDD)
{
	//{{AFX_DATA_INIT(CWarpTextEffectsPage)
	//}}AFX_DATA_INIT
	
	m_pObject = NULL;
	m_hPreview = NULL;
}

CWarpTextEffectsPage::~CWarpTextEffectsPage()
{
}

void CWarpTextEffectsPage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWarpTextEffectsPage)
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CWarpTextEffectsPage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CWarpTextEffectsPage)
	ON_BN_CLICKED(IDC_TEXT_FILL_COLOR_CUSTOM, OnTextFillColorCustom)
	ON_BN_CLICKED(IDC_TEXT_LINE_WIDTH_CUSTOM, OnTextLineWidthCustom)
	ON_BN_CLICKED(IDC_TEXT_LINE_COLOR_CUSTOM, OnTextLineColorCustom)
	ON_BN_CLICKED(IDC_TEXT_SHADOW_COLOR_CUSTOM, OnTextShadowColorCustom)
	ON_BN_CLICKED(IDC_BACKGROUND_FILL_COLOR_CUSTOM, OnBackgroundFillColorCustom)
	ON_BN_CLICKED(IDC_BACKGROUND_LINE_WIDTH_CUSTOM, OnBackgroundLineWidthCustom)
	ON_BN_CLICKED(IDC_BACKGROUND_LINE_COLOR_CUSTOM, OnBackgroundLineColorCustom)
	ON_BN_CLICKED(IDC_BACKGROUND_SHADOW_COLOR_CUSTOM, OnBackgroundShadowColorCustom)
	ON_CBN_SELCHANGE(IDC_BACKGROUND_FILL_COLOR, OnSelchangeBackgroundFillColor)
	ON_CBN_SELCHANGE(IDC_BACKGROUND_LINE_WIDTH, OnSelchangeBackgroundLineWidth)
	ON_CBN_SELCHANGE(IDC_BACKGROUND_LINE_COLOR, OnSelchangeBackgroundLineColor)
	ON_CBN_SELCHANGE(IDC_BACKGROUND_SHADOW_COLOR, OnSelchangeBackgroundShadowColor)
	ON_CBN_SELCHANGE(IDC_TEXT_FILL_COLOR, OnSelchangeTextFillColor)
	ON_CBN_SELCHANGE(IDC_TEXT_LINE_WIDTH, OnSelchangeTextLineWidth)
	ON_CBN_SELCHANGE(IDC_TEXT_LINE_COLOR, OnSelchangeTextLineColor)
	ON_CBN_SELCHANGE(IDC_TEXT_SHADOW_COLOR, OnSelchangeTextShadowColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWarpTextEffectsPage::Object(WarpTextObject* pObject)
{
	m_pObject = pObject;
	
	if (m_pObject != NULL)
	{
		m_nTextLineWidth = m_nInitialTextLineWidth;
		m_nBackgroundLineWidth = m_nInitialBackgroundLineWidth;
		
		m_TextFillColor.Color(m_pObject->Record().m_clTextFillColor, m_pObject->Record().m_fTextFillColorExists);
		m_TextLineWidth.Width(m_nTextLineWidth);
		m_TextLineColor.Color(m_pObject->Record().m_clTextLineColor, m_pObject->Record().m_fTextLineColorExists);
		m_TextShadowColor.Color(m_pObject->Record().m_clTextShadowColor, m_pObject->Record().m_fTextShadowColorExists);
		m_BackgroundFillColor.Color(m_pObject->Record().m_clBackgroundFillColor, m_pObject->Record().m_fBackgroundFillColorExists);
		m_BackgroundLineWidth.Width(m_nBackgroundLineWidth);
		m_BackgroundLineColor.Color(m_pObject->Record().m_clBackgroundLineColor, m_pObject->Record().m_fBackgroundLineColorExists);
		m_BackgroundShadowColor.Color(m_pObject->Record().m_clBackgroundShadowColor, m_pObject->Record().m_fBackgroundShadowColorExists);
	}

	if (GetSafeHwnd() != NULL)
	{	
		// get the preview window
		
		CWnd* pParent = GetParent();
		
		if (pParent != NULL)
		{
			m_hPreview = pParent->GetDlgItem(IDC_PREVIEW)->GetSafeHwnd();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWarpTextEffectsPage message handlers

BOOL CWarpTextEffectsPage::OnInitDialog()
{
	// initialize the various effects combo boxes
	
	m_TextFillColor.SubclassDlgItem(IDC_TEXT_FILL_COLOR, this);
	m_TextFillColor.SizeItems();
	// JRO m_TextFillColor.Colors(NULL, "No Color");
	m_TextFillColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_COLOR));

	m_TextLineWidth.SubclassDlgItem(IDC_TEXT_LINE_WIDTH, this);
	m_TextLineWidth.SizeItems();
	// JRO m_TextLineWidth.Widths("No Border");
   m_TextLineWidth.Widths(GET_PMWAPP()->GetResourceStringPointer (IDS_NO_BORDER));
	
	m_TextLineColor.SubclassDlgItem(IDC_TEXT_LINE_COLOR, this);
	m_TextLineColor.SizeItems();
	// JRO m_TextLineColor.Colors(NULL, "No Color");
   m_TextLineColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_COLOR));
	
	m_TextShadowColor.SubclassDlgItem(IDC_TEXT_SHADOW_COLOR, this);
	m_TextShadowColor.SizeItems();
	// JRO m_TextShadowColor.Colors(NULL, "No Shadow");
   m_TextShadowColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_SHADOW));
	
	m_BackgroundFillColor.SubclassDlgItem(IDC_BACKGROUND_FILL_COLOR, this);
	m_BackgroundFillColor.SizeItems();
	// JRO m_BackgroundFillColor.Colors(NULL, "No Background");
   m_BackgroundFillColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_BACKGROUND));
	
	m_BackgroundLineWidth.SubclassDlgItem(IDC_BACKGROUND_LINE_WIDTH, this);
	m_BackgroundLineWidth.SizeItems();
	// JRO m_BackgroundLineWidth.Widths("No Border");
   m_BackgroundLineWidth.Widths(GET_PMWAPP()->GetResourceStringPointer (IDS_NO_BORDER));
	
	m_BackgroundLineColor.SubclassDlgItem(IDC_BACKGROUND_LINE_COLOR, this);
	m_BackgroundLineColor.SizeItems();
	// JRO m_BackgroundLineColor.Colors(NULL, "No Color");
   m_BackgroundLineColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_COLOR));
	
	m_BackgroundShadowColor.SubclassDlgItem(IDC_BACKGROUND_SHADOW_COLOR, this);
	m_BackgroundShadowColor.SizeItems();
	// JRO m_BackgroundShadowColor.Colors(NULL, "No Shadow");
   m_BackgroundShadowColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_SHADOW)); 

	// select the current combo box items based on the current object
	
	if (m_pObject != NULL)
	{	
		m_TextFillColor.Color(m_pObject->Record().m_clTextFillColor, m_pObject->Record().m_fTextFillColorExists);
		m_TextLineWidth.Width(m_nTextLineWidth);
		m_TextLineColor.Color(m_pObject->Record().m_clTextLineColor, m_pObject->Record().m_fTextLineColorExists);
		m_TextShadowColor.Color(m_pObject->Record().m_clTextShadowColor, m_pObject->Record().m_fTextShadowColorExists);
		m_BackgroundFillColor.Color(m_pObject->Record().m_clBackgroundFillColor, m_pObject->Record().m_fBackgroundFillColorExists);
		m_BackgroundLineWidth.Width(m_nBackgroundLineWidth);
		m_BackgroundLineColor.Color(m_pObject->Record().m_clBackgroundLineColor, m_pObject->Record().m_fBackgroundLineColorExists);
		m_BackgroundShadowColor.Color(m_pObject->Record().m_clBackgroundShadowColor, m_pObject->Record().m_fBackgroundShadowColorExists);
	}
	
	CPmwPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWarpTextEffectsPage::OnTextFillColorCustom()
{
	if (m_TextFillColor.ChooseCustomColor())
	{
		OnSelchangeTextFillColor();
	}
}

void CWarpTextEffectsPage::OnTextLineWidthCustom()
{
	if (m_TextLineWidth.ChooseCustomWidth(24))
	{
		OnSelchangeTextLineWidth();
	}
}

void CWarpTextEffectsPage::OnTextLineColorCustom()
{
	if (m_TextLineColor.ChooseCustomColor())
	{
		OnSelchangeTextLineColor();
	}
}

void CWarpTextEffectsPage::OnTextShadowColorCustom()
{
	if (m_TextShadowColor.ChooseCustomColor())
	{
		OnSelchangeTextShadowColor();
	}
}

void CWarpTextEffectsPage::OnBackgroundFillColorCustom()
{
	if (m_BackgroundFillColor.ChooseCustomColor())
	{
		OnSelchangeBackgroundFillColor();
	}
}

void CWarpTextEffectsPage::OnBackgroundLineWidthCustom()
{
	if (m_BackgroundLineWidth.ChooseCustomWidth(36))
	{
		OnSelchangeBackgroundLineWidth();
	}
}

void CWarpTextEffectsPage::OnBackgroundLineColorCustom()
{
	if (m_BackgroundLineColor.ChooseCustomColor())
	{
		OnSelchangeBackgroundLineColor();
	}
}

void CWarpTextEffectsPage::OnBackgroundShadowColorCustom()
{
	if (m_BackgroundShadowColor.ChooseCustomColor())
	{
		OnSelchangeBackgroundShadowColor();
	}
}

void CWarpTextEffectsPage::OnSelchangeTextFillColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;
		
		m_pObject->Record().m_fTextFillColorExists = m_TextFillColor.Color(&Color);
		m_pObject->Record().m_clTextFillColor = Color;
		
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeTextLineWidth()
{
	if (m_pObject != NULL)
	{
		m_nTextLineWidth = m_TextLineWidth.Width();
		
		int nWidth = m_nTextLineWidth;
		if (nWidth > 4)
		{
			nWidth = 4;
		}
		
		m_pObject->Record().m_nTextLineWidth = nWidth;
			
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeTextLineColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;
			
		m_pObject->Record().m_fTextLineColorExists = m_TextLineColor.Color(&Color);
		m_pObject->Record().m_clTextLineColor = Color;
			
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeTextShadowColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;
			
		m_pObject->Record().m_fTextShadowColorExists = m_TextShadowColor.Color(&Color);
		m_pObject->Record().m_clTextShadowColor = Color;
		
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeBackgroundFillColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;
		
		m_pObject->Record().m_fBackgroundFillColorExists = m_BackgroundFillColor.Color(&Color);
		m_pObject->Record().m_clBackgroundFillColor = Color;
		
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeBackgroundLineWidth()
{
	if (m_pObject != NULL)
	{
		m_nBackgroundLineWidth = m_BackgroundLineWidth.Width();
		
		int nWidth = m_nBackgroundLineWidth;
		if (nWidth > 4)
		{
			nWidth = 4;
		}
		
		m_pObject->Record().m_nBackgroundLineWidth = nWidth;
		
		long lWidth = (long)(nWidth*1800L)/72L;
		
		PBOX Margins;
		Margins.x0 =
			Margins.y0 =
			Margins.x1 =
			Margins.y1 = lWidth;

		m_pObject->Margins(Margins);
		m_pObject->calc();
			
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeBackgroundLineColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;
			
		m_pObject->Record().m_fBackgroundLineColorExists = m_BackgroundLineColor.Color(&Color);
		m_pObject->Record().m_clBackgroundLineColor = Color;
			
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::OnSelchangeBackgroundShadowColor()
{
	if (m_pObject != NULL)
	{
		COLOR Color;

		m_pObject->Record().m_fBackgroundShadowColorExists = m_BackgroundShadowColor.Color(&Color);
		m_pObject->Record().m_clBackgroundShadowColor = Color;
		
		UpdatePreview();
	}
}

void CWarpTextEffectsPage::UpdatePreview(void)
{
	if (m_hPreview != NULL)
	{
		::InvalidateRect(m_hPreview, NULL, TRUE);
	}
}

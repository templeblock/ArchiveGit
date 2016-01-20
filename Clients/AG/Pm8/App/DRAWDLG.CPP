/*
// $Workfile: DRAWDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DRAWDLG.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:14   Fred
// Initial revision.
// 
//    Rev 1.2   23 May 1997 17:05:16   Fred
// New effects
// 
//    Rev 1.1   06 Nov 1996 11:12:54   johno
// Moved strings to resource DLL
// 
//    Rev 1.0   14 Mar 1996 13:36:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:14   FRED
// Initial revision.
// 
//    Rev 1.4   19 Aug 1995 17:49:40   FRED
// Maximum line sizes
// 
//    Rev 1.3   18 Aug 1995 11:46:34   FRED
// Line attributes
// 
//    Rev 1.2   17 May 1995 16:57:02   FRED
// Draw object shadows
// 
//    Rev 1.1   16 May 1995 12:00:18   JAY
// 
//    Rev 1.0   10 May 1995 08:01:36   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "drawdlg.h"
#include "drawobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawingAttributesDialog dialog

CDrawingAttributesDialog::CDrawingAttributesDialog(DrawingObject* pObject, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CDrawingAttributesDialog::IDD, pParent)
{
	m_pObject = pObject;
	//{{AFX_DATA_INIT(CDrawingAttributesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CDrawingAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawingAttributesDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		COLOR Color;
#if 0		
		m_pObject->Record().m_fFillColorExists = m_FillColor.Color(&Color);
		m_pObject->Record().m_clFillColor = Color;
		
		m_pObject->Record().m_nLineWidth = m_LineWidth.Width();
		
		m_pObject->Record().m_fLineColorExists = m_LineColor.Color(&Color);
		m_pObject->Record().m_clLineColor = Color;
		
		m_pObject->Record().m_fShadowColorExists = m_ShadowColor.Color(&Color);
		m_pObject->Record().m_clShadowColor = Color;
#endif
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CDrawingAttributesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CDrawingAttributesDialog)
	ON_BN_CLICKED(IDC_FILL_COLOR_CUSTOM, OnFillColorCustom)
	ON_BN_CLICKED(IDC_LINE_COLOR_CUSTOM, OnLineColorCustom)
	ON_BN_CLICKED(IDC_LINE_WIDTH_CUSTOM, OnLineWidthCustom)
	ON_BN_CLICKED(IDC_SHADOW_COLOR_CUSTOM, OnShadowColorCustom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawingAttributesDialog message handlers

BOOL CDrawingAttributesDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_FillColor.SubclassDlgItem(IDC_FILL_COLOR, this);
	m_FillColor.SizeItems();
//	m_FillColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_FILL_COLOR));
//	m_FillColor.Color(m_pObject->Record().m_clFillColor, m_pObject->Record().m_fFillColorExists);
	
	m_LineWidth.SubclassDlgItem(IDC_LINE_WIDTH, this);
	m_LineWidth.SizeItems();
//	m_LineWidth.Widths(GET_PMWAPP()->GetResourceStringPointer (IDS_NO_BORDER));
//	m_LineWidth.Width(m_pObject->Record().m_nLineWidth);
	
	m_LineColor.SubclassDlgItem(IDC_LINE_COLOR, this);
	m_LineColor.SizeItems();
//	m_LineColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_BORDER_COLOR));
//	m_LineColor.Color(m_pObject->Record().m_clLineColor, m_pObject->Record().m_fLineColorExists);

	m_ShadowColor.SubclassDlgItem(IDC_SHADOW_COLOR, this);
	m_ShadowColor.SizeItems();
//	m_ShadowColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_SHADOW));
//	m_ShadowColor.Color(m_pObject->Record().m_clShadowColor, m_pObject->Record().m_fShadowColorExists);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDrawingAttributesDialog::OnFillColorCustom()
{
	m_FillColor.ChooseCustomColor();
}

void CDrawingAttributesDialog::OnLineColorCustom()
{
	m_LineColor.ChooseCustomColor();
}

void CDrawingAttributesDialog::OnLineWidthCustom()
{
	m_LineWidth.ChooseCustomWidth(72);
}

void CDrawingAttributesDialog::OnShadowColorCustom()
{
	m_ShadowColor.ChooseCustomColor();
}

/////////////////////////////////////////////////////////////////////////////
// CLineAttributesDialog dialog

CLineAttributesDialog::CLineAttributesDialog(DrawingObject* pObject, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CLineAttributesDialog::IDD, pParent)
{
	m_pObject = pObject;
	//{{AFX_DATA_INIT(CLineAttributesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLineAttributesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineAttributesDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		COLOR Color;
		
//		m_pObject->Record().m_nLineWidth = m_LineWidth.Width();
		
//		m_pObject->Record().m_fLineColorExists = m_LineColor.Color(&Color);
//		m_pObject->Record().m_clLineColor = Color;
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CLineAttributesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CLineAttributesDialog)
	ON_BN_CLICKED(IDC_LINE_COLOR_CUSTOM, OnLineColorCustom)
	ON_BN_CLICKED(IDC_LINE_WIDTH_CUSTOM, OnLineWidthCustom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLineAttributesDialog message handlers

BOOL CLineAttributesDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_LineWidth.SubclassDlgItem(IDC_LINE_WIDTH, this);
	m_LineWidth.SizeItems();
	m_LineWidth.Widths(NULL);
//	m_LineWidth.Width(m_pObject->Record().m_nLineWidth);
	
	m_LineColor.SubclassDlgItem(IDC_LINE_COLOR, this);
	m_LineColor.SizeItems();
//	m_LineColor.Colors(NULL, GET_PMWAPP()->GetResourceStringPointer (IDS_NO_LINE_COLOR));
//	m_LineColor.Color(m_pObject->Record().m_clLineColor, m_pObject->Record().m_fLineColorExists);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLineAttributesDialog::OnLineColorCustom()
{
	m_LineColor.ChooseCustomColor();
}

void CLineAttributesDialog::OnLineWidthCustom()
{
	m_LineWidth.ChooseCustomWidth(72);
}

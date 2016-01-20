// GradDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "GradDlg.h"
#include "polypoly.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GRAD_ITEM_HEIGHT(r)	\
	((r) / 4)

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////////////////////////
// CGradientStyleListBoxItem

CGradientStyleListBoxItem::CGradientStyleListBoxItem(int nType)
{
	m_pFillObject = (CGradientFill*)DeriveFillType(nType);	
}

CGradientStyleListBoxItem::~CGradientStyleListBoxItem()
{
	if (m_pFillObject != NULL)
	{
		delete m_pFillObject;
	}
}
	
void CGradientStyleListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	INHERITED::MeasureItem(pList, pMeasureItemStruct);
	CRect crClient;
	pList->GetClientRect(crClient);

	pMeasureItemStruct->itemHeight = crClient.Height();
	pMeasureItemStruct->itemWidth = crClient.Width()/4;

	pList->SetColumnWidth(pMeasureItemStruct->itemWidth);
}

void CGradientStyleListBoxItem::PaintItem(COwnerDrawListBox* pListBox, CDC* pDC, LPDRAWITEMSTRUCT pDrawItemStruct, CRect& crBounds, COLORREF clForeground, COLORREF clBackground)
{
	CPen cpPen;
	CBrush cbBrush;
	
	int nContext = pDC->SaveDC();
	
	if (nContext != 0)
	{
		CRect crGradBounds(crBounds);
					
		// Compute the rectangle that describes the area for the style preview.
		crGradBounds.InflateRect(-2, -2);			

		// Create & fill a CPolyPolygon
		CPolyPolygon poly;
		poly.MoveTo(MakeFixed(crGradBounds.left), MakeFixed(crGradBounds.top));
		poly.LineTo(MakeFixed(crGradBounds.right), MakeFixed(crGradBounds.top));
		poly.LineTo(MakeFixed(crGradBounds.right), MakeFixed(crGradBounds.bottom));
		poly.LineTo(MakeFixed(crGradBounds.left), MakeFixed(crGradBounds.bottom));
		poly.LineTo(MakeFixed(crGradBounds.left), MakeFixed(crGradBounds.top));
		poly.Close();

		CPalette* pOldPal = pDC->SelectPalette(pOurPal, FALSE);
		pDC->RealizePalette();

		// Draw it
		BOOL fPalette = (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
		poly.Draw(pDC, m_pFillObject, 1, RGB(0, 0, 0), TRUE, ALTERNATE, fPalette);

		if (pOldPal != NULL)
		{
			pDC->SelectPalette(pOldPal, FALSE);
		}	
	}
	
	if (nContext != 0)
	{
		pDC->RestoreDC(nContext);
		nContext = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGradientStyleListBox

CGradientStyleListBox::CGradientStyleListBox()
{
}

CGradientStyleListBox::~CGradientStyleListBox()
{
}

BEGIN_MESSAGE_MAP(CGradientStyleListBox, COwnerDrawListBox)
	//{{AFX_MSG_MAP(CGradientStyleListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WNDPROC* CGradientStyleListBox::GetSuperWndProcAddr()
{
	static WNDPROC NEAR pfnSuper;
	return &pfnSuper;
}

void CGradientStyleListBox::BuildList(COLOR BaseColor, COLOR BlendColor)
{
	SetRedraw(FALSE);
	ResetContent();

	for (int i = FIRST_GRADIENT_TYPE; i < FillFormatV2::FillNumTypes; i++)
	{				
		CGradientStyleListBoxItem* pItem = new CGradientStyleListBoxItem(i);
		
		if (pItem != NULL)
		{
			AddString((LPCSTR)pItem);
			(pItem->GetFillObject())->SetFillColor(BaseColor);
			(pItem->GetFillObject())->SetBlendColor(BlendColor);
		}
	}

	SizeItems();
		
	SetRedraw(TRUE);
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CGradientStyleListBox message handlers

/////////////////////////////////////////////////////////////////////////////
// CGradientDlg dialog

COLOR CGradientDlg::m_BaseColor = COLOR_BLACK;
COLOR CGradientDlg::m_BlendColor = COLOR_WHITE;
SHORT CGradientDlg::m_nFillType = FIRST_GRADIENT_TYPE;

CGradientDlg::CGradientDlg(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CGradientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGradientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGradientDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGradientDlg)
	DDX_Control(pDX, IDC_GRADIENT_STYLE, m_listStyles);
	DDX_Control(pDX, IDC_BLEND_COLOR, m_comboBlendColor);
	DDX_Control(pDX, IDC_BASE_COLOR, m_comboBaseColor);
	//}}AFX_DATA_MAP
}

COLOR CGradientDlg::GetBaseColor()
{
	return (m_BaseColor);
}

COLOR CGradientDlg::GetBlendColor()
{
	return (m_BlendColor);
}

SHORT CGradientDlg::GetFillType()
{
	return (m_nFillType);
}

void CGradientDlg::SetBaseColor(COLOR BaseColor)
{
	m_BaseColor = BaseColor;
}

void CGradientDlg::SetBlendColor(COLOR BlendColor)
{
	m_BlendColor = BlendColor;
}

BEGIN_MESSAGE_MAP(CGradientDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CGradientDlg)
   ON_CBN_SELCHANGE(IDC_BASE_COLOR, OnSelchangeBaseColor)
   ON_CBN_SELCHANGE(IDC_BLEND_COLOR, OnSelchangeBlendColor)
	ON_LBN_DBLCLK(IDC_GRADIENT_STYLE, OnDblclkGradientStyle)
	ON_BN_CLICKED(ID_SWAP_COLOR, OnSwapColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGradientDlg message handlers

BOOL CGradientDlg::OnInitDialog() 
{
	CPmwDialog::OnInitDialog();
	
	// Build the style list
	m_listStyles.BuildList(m_BaseColor, m_BlendColor);
	m_listStyles.SizeItems();
	m_listStyles.SetCurSel(m_nFillType - FIRST_GRADIENT_TYPE);

	// Set up color boxes
	m_comboBaseColor.SetColor(m_BaseColor);
	m_comboBlendColor.SetColor(m_BlendColor);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGradientDlg::OnSelchangeBaseColor()
{
	// Modify all the items and redraw
	ASSERT(FillFormatV2::FillNumTypes - FIRST_GRADIENT_TYPE <= m_listStyles.GetCount());
	for (int i = FIRST_GRADIENT_TYPE; i < FillFormatV2::FillNumTypes; i++)
	{				
		CGradientStyleListBoxItem* pItem = (CGradientStyleListBoxItem*)m_listStyles.GetItemDataPtr(i-FIRST_GRADIENT_TYPE);
		(pItem->GetFillObject())->SetFillColor(m_comboBaseColor.GetColor());
	}
	m_listStyles.Invalidate();
}

void CGradientDlg::OnSelchangeBlendColor()
{
	// Modify all the items and redraw
	ASSERT(FillFormatV2::FillNumTypes - FIRST_GRADIENT_TYPE <= m_listStyles.GetCount());
	for (int i = FIRST_GRADIENT_TYPE; i < FillFormatV2::FillNumTypes; i++)
	{				
		CGradientStyleListBoxItem* pItem = (CGradientStyleListBoxItem*)m_listStyles.GetItemDataPtr(i-FIRST_GRADIENT_TYPE);
		(pItem->GetFillObject())->SetBlendColor(m_comboBlendColor.GetColor());
	}
	m_listStyles.Invalidate();
}

void CGradientDlg::OnSwapColor() 
{
	// Swap base & blend colors and redraw
	COLOR BaseColor = m_comboBaseColor.GetColor();
	m_comboBaseColor.SetColor(m_comboBlendColor.GetColor());
	m_comboBlendColor.SetColor(BaseColor);
	m_comboBaseColor.Invalidate();
	m_comboBlendColor.Invalidate();

	// Modify all the items and redraw
	ASSERT(FillFormatV2::FillNumTypes - FIRST_GRADIENT_TYPE <= m_listStyles.GetCount());
	for (int i = FIRST_GRADIENT_TYPE; i < FillFormatV2::FillNumTypes; i++)
	{				
		CGradientStyleListBoxItem* pItem = (CGradientStyleListBoxItem*)m_listStyles.GetItemDataPtr(i-FIRST_GRADIENT_TYPE);
		(pItem->GetFillObject())->SetFillColor(m_comboBaseColor.GetColor());
		(pItem->GetFillObject())->SetBlendColor(m_comboBlendColor.GetColor());
	}
	m_listStyles.Invalidate();
}

void CGradientDlg::OnDblclkGradientStyle()
{
	OnOK();	
}

void CGradientDlg::OnOK() 
{
	// Set colors
	m_BaseColor = m_comboBaseColor.GetColor();
	m_BlendColor = m_comboBlendColor.GetColor();
	if (!CFill::IsValidColor(m_BlendColor))
	{
		m_BlendColor = m_BaseColor;
	}
	if (!CFill::IsValidColor(m_BaseColor))
	{
		m_BaseColor = m_BlendColor;
	}
		
	// Set fill type
	m_nFillType = FIRST_GRADIENT_TYPE + m_listStyles.GetCurSel();

	CPmwDialog::OnOK();
}

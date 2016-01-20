#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelText2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelText2::CToolsPanelText2(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;

	m_LineWidth = POINTUNITS(0);
	m_CurrentFillType = FT_Solid;
	m_CurrentFillColor = RGB(0,0,0);
}

//////////////////////////////////////////////////////////////////////
CToolsPanelText2::~CToolsPanelText2()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText2::UpdateControls()
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsText() || (pSym->IsAddAPhoto() && m_pCtp->GetDocWindow()->SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)(CAGSymText*)pSym;
		UpdateTextControls(pText);
	}
	else if (pSym->IsCalendar())
	{
		CAGSymCalendar* pCalSym = (CAGSymCalendar*)pSym;
		UpdateCalendarTextControls(pCalSym);
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText2::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanelCommon::InitToolCtrls(true);
	CToolsCtlPanelCommon::InitLineCombos(true);
	CToolsCtlPanelCommon::InitToolTips();

	bHandled = true;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText2::UpdateTextControls(CAGText* pText)
{
	scTypeSpecList tsList;
	pText->GetSelTSList(tsList);
	int nNumItems = tsList.NumItems();
	if (!nNumItems)
		return;

	CAGSpec* pAGSpec = (CAGSpec*)(tsList[0].ptr());

	FillType Fill = pAGSpec->m_FillType;
	COLORREF FillColor = pAGSpec->m_FillColor;
	COLORREF FillColor2 = pAGSpec->m_FillColor2;
	int LineWidth = pAGSpec->m_LineWidth;
	COLORREF LineColor = pAGSpec->m_LineColor;
	bool bSameFillType = true;
	bool bSameFillColor = true;
	bool bSameFillColor2 = true;
	bool bSameLineWidth = true;
	bool bSameLineColor = true;
	
	for (int i = 1; i < nNumItems; i++)
	{
		pAGSpec = (CAGSpec*)(tsList[i].ptr());
		if (Fill != pAGSpec->m_FillType)
			bSameFillType = false;

		if (FillColor != pAGSpec->m_FillColor)
			bSameFillColor = false;

		if (FillColor2 != pAGSpec->m_FillColor2)
			bSameFillColor2 = false;

		if (LineWidth != pAGSpec->m_LineWidth)
			bSameLineWidth = false;

		if (LineColor != pAGSpec->m_LineColor)
			bSameLineColor = false;
	}

	// Update the FillType control
	if (!bSameFillType)
		Fill = (FillType)-1;
	SetFillType(Fill);

	// Update the FillColor control
	if (!bSameFillColor)
		FillColor = CLR_NONE;
	SetFillColor(FillColor);

	// Update the FillColor2 control
	if (!bSameFillColor2)
		FillColor2 = CLR_NONE;
	SetFillColor2(FillColor2);

	// Update the LineWidth control
	if (!bSameLineWidth)
		LineWidth = -999;
	SetLineWidth(LineWidth);

	// Update the LineColor control
	if (!bSameLineColor)
		LineColor = CLR_NONE;
	SetLineColor(LineColor);

	ShowHideColors();
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText2::UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym)
{
	CAGSpec* pAGSpec = pCalendarSym->GetActiveSpec();
	if (!pAGSpec)
		return;
	
	// Update the FillType control
	SetFillType(pAGSpec->m_FillType);

	// Update the FillColor control
	SetFillColor(pAGSpec->m_FillColor);

	// Update the FillColor2 control
	SetFillColor2(pAGSpec->m_FillColor2);

	// Update the LineWidth control
	SetLineWidth(pAGSpec->m_LineWidth);

	// Update the LineColor control
	SetLineColor(pAGSpec->m_LineColor);

	ShowHideColors();
}


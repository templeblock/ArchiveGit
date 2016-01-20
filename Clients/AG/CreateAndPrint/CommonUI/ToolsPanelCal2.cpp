#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelCal2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelCal2::CToolsPanelCal2(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelCal2::~CToolsPanelCal2()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelCal2::UpdateControls()
{
	if (m_nResID != IDD_BOTTOOLSPANEL_CALENDAR)
		return;

	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym)
		return;

	if (!pSym->IsCalendar())
		return;

	CAGSymCalendar* pCalSym = (CAGSymCalendar*)pSym;

	// Update the Fill controls
	SetFillColor(pCalSym->GetFillColor());
	SetFillColor2(pCalSym->GetFillColor2());
	SetFillColor3(pCalSym->GetFillColor3());

	// Update the Line control
	SetLineWidth(pCalSym->GetLineWidth());
	SetLineColor(pCalSym->GetLineColor());
	//CToolsCtlPanelCommon::UpdateLineCtrls();

}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal2::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bCalendarPanel = (m_nResID == IDD_BOTTOOLSPANEL_CALENDAR);
	if (bCalendarPanel)
	{
		CToolsCtlPanelCommon::InitLineCombos(false);

		m_ColorComboFill.InitDialog(GetDlgItem(IDC_FILL_COLOR), true, true);
		m_ColorComboFill2.InitDialog(GetDlgItem(IDC_FILL_COLOR2), true, true);
		m_ColorComboFill3.InitDialog(GetDlgItem(IDC_FILL_COLOR3), true, true);
	}

	CToolsCtlPanel::InitToolTips();
	AddToolTip(IDC_FILL_TYPE);
	AddToolTip(IDC_COLOR_1, IDS_CAL_COLOR_1);
	AddToolTip(IDC_COLOR_2, IDS_CAL_COLOR_2);
	AddToolTip(IDC_COLOR_3, IDS_CAL_COLOR_3);
	AddToolTip(IDC_LINE_SIZE, IDS_LINE_SIZE);
	AddToolTip(IDC_COLOR_LINE, IDS_LINE_COLOR);

	return S_OK;
}

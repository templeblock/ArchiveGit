#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelGraphics2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelGraphics2::CToolsPanelGraphics2(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelGraphics2::~CToolsPanelGraphics2()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelGraphics2::UpdateControls()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsGraphic())
	{
		CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

		// Update the Fill controls
		SetFillType(pGraphicSym->GetFillType());
		SetFillColor(pGraphicSym->GetFillColor());
		SetFillColor2(pGraphicSym->GetFillColor2());

		// Update the Line control
		SetLineWidth(pGraphicSym->GetLineWidth());
		SetLineColor(pGraphicSym->GetLineColor());

		ShowHideColors();
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelGraphics2::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanelCommon::InitToolCtrls(true);
	CToolsCtlPanelCommon::InitLineCombos(false);
	CToolsCtlPanelCommon::InitToolTips();

	return S_OK;
}
#include "stdafx.h"
#include "Ctp.h"
#include "ToolsCtlPanelGraphics.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsCtlPanelGraphics::CToolsCtlPanelGraphics(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
	m_iCurrentShape = -1;
}

//////////////////////////////////////////////////////////////////////
CToolsCtlPanelGraphics::~CToolsCtlPanelGraphics()
{
}


//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelGraphics::UpdateControls()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsGraphic())
	{
		CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

		if (pSym->IsRectangle())
			SetShapeName("Rectangle");
		else
		if (pSym->IsEllipse())
			SetShapeName("Circle/Ellipse");
		else
		if (pSym->IsLine())
			SetShapeName("Line");
		else
		if (pSym->IsShape())
		{
			SetShapeName(((CAGSymDrawing*)pSym)->GetShapeName());
		}

		// Update the Fill controls
		SetFillType(pGraphicSym->GetFillType());
		SetFillColor(pGraphicSym->GetFillColor());
		SetFillColor2(pGraphicSym->GetFillColor2());

		// Update the Line control
		SetLineWidth(pGraphicSym->GetLineWidth());
		SetLineColor(pGraphicSym->GetLineColor());

		//UpdateFillCtrls();
		//UpdateLineCtrls();
		ShowHideColors();
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelGraphics::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanelCommon::InitToolCtrls(true);
	CToolsCtlPanelCommon::InitLineCombos(false);
	CToolsCtlPanelCommon::InitToolTips();

	m_ComboShapeName.InitDialog(GetDlgItem(IDC_SHAPE_NAME));
	int iShapeCount = CShape::GetShapeCount();
	for (int i = 0; i < iShapeCount; i++)
		m_ComboShapeName.AddString(CShape::GetShapeName(i));
	m_ComboShapeName.SetCurSel(-1);

	AddToolTip(IDC_SHAPE_NAME);
	bHandled = true;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelGraphics::OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	char strShapeName[256];
	strShapeName[0] = '\0';
	m_ComboShapeName.GetWindowText(strShapeName, sizeof(strShapeName));
	m_iCurrentShape = CShape::GetShapeIndex(strShapeName);
	if (m_iCurrentShape >= 0)
		m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SHAPENAMECHANGE);

	if (m_ComboShapeName.GetAfterCloseUp())
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelGraphics::SetShapeName(LPCSTR pShapeName)
{
	m_iCurrentShape = m_ComboShapeName.FindString(-1, pShapeName);
	if (m_iCurrentShape >= 0)
		m_ComboShapeName.SetCurSel(m_iCurrentShape);
	else
		m_ComboShapeName.SetWindowText(pShapeName);
}


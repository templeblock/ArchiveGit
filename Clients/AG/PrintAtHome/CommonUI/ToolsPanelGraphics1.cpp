#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelGraphics1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelGraphics1::CToolsPanelGraphics1(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
	m_iCurrentShape = -1;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelGraphics1::~CToolsPanelGraphics1()
{
}


//////////////////////////////////////////////////////////////////////
void CToolsPanelGraphics1::UpdateControls()
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
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelGraphics1::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanel::InitToolTips();

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
LRESULT CToolsPanelGraphics1::OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
void CToolsPanelGraphics1::SetShapeName(LPCSTR pShapeName)
{
	m_iCurrentShape = m_ComboShapeName.FindString(-1, pShapeName);
	if (m_iCurrentShape >= 0)
		m_ComboShapeName.SetCurSel(m_iCurrentShape);
	else
		m_ComboShapeName.SetWindowText(pShapeName);
}


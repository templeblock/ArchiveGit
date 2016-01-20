#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelImage1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelImage1::CToolsPanelImage1(CCtp* pMainWnd) : CToolsCtlPanel(pMainWnd)
{
	m_nResID = IDD;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelImage1::~CToolsPanelImage1()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelImage1::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanel::InitToolTips();
	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelImage1::UpdateControls()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsImage())
	{
		CAGSymImage* pImageSym = (CAGSymImage*)pSym;

		{
			//COLORREF Color = pImageSym->GetLastHitColor();
			//if (Color == CLR_NONE)
			//	return;

			//m_CurrentFillColor = Color;
		}
		//SetFillColor(pImageSym->GetLastHitColor());
	}
}

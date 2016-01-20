#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelImage2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelImage2::CToolsPanelImage2(CCtp* pMainWnd) : CToolsCtlPanel(pMainWnd)
{
	m_nResID = IDD;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelImage2::~CToolsPanelImage2()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelImage2::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CToolsCtlPanel::InitToolTips();
	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelImage2::UpdateControls()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsImage())
	{
		CAGSymImage* pImageSym = (CAGSymImage*)pSym;

		COLORREF Color = pImageSym->GetLastHitColor();
		if (Color == CLR_NONE)
			return;

		//Get the text panel and set the color
		CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
		if (pCtlPanel)
		{
			CToolsPanelText2* pBotTxtToolsPanel = (CToolsPanelText2*)pCtlPanel->GetBotToolPanel();
			if (pBotTxtToolsPanel)
				pBotTxtToolsPanel->SetFillColor(Color);
		}
	}
}

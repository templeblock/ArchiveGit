#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelContainer.h"
#include "ToolsCtlPanel.h"
#include "ToolsPanelText1.h"
#include "ToolsPanelText2.h"
#include "ToolsPanelCal1.h"
#include "ToolsPanelCal2.h"
#include "ToolsPanelGraphics1.h"
#include "ToolsPanelGraphics2.h"
#include "ToolsPanelImage1.h"
#include "ToolsPanelImage2.h"

//////////////////////////////////////////////////////////////////////
CToolsPanelContainer::CToolsPanelContainer(CCtp* pMainWnd)
:m_pCtp(pMainWnd),
 pTopToolsPanel(NULL),
 pBotToolsPanel(NULL)
{
}

//////////////////////////////////////////////////////////////////////
CToolsPanelContainer::~CToolsPanelContainer()
{
	Free();
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::Free()
{
	if (m_pCtp)
		m_pCtp = NULL;

	if (pTopToolsPanel)
	{
		pTopToolsPanel->DestroyWindow();
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
	}

	if (pBotToolsPanel)
	{
		pBotToolsPanel->DestroyWindow();
		delete pBotToolsPanel;
		pBotToolsPanel = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::Create()
{
	ATLASSERT(!pTopToolsPanel && !pBotToolsPanel);

	pTopToolsPanel = new CToolsCtlPanel(m_pCtp);
	if (!pTopToolsPanel)
		return;

	pBotToolsPanel = new CToolsCtlPanel(m_pCtp);
	if (!pBotToolsPanel)
	{
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
		return;
	}

	pTopToolsPanel->SetBkgdImage1();
	pBotToolsPanel->SetBkgdImage2();

	pTopToolsPanel->Create(m_pCtp->m_hWnd);
	pBotToolsPanel->Create(m_pCtp->m_hWnd);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::ShowWindow(int nCmdShow)
{
	if (!pTopToolsPanel || !pBotToolsPanel)
		return;

	pTopToolsPanel->ShowWindow(nCmdShow);
	pBotToolsPanel->ShowWindow(nCmdShow);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::ActivateNewDoc()
{
	if (!pTopToolsPanel || !pBotToolsPanel)
		return;

	pTopToolsPanel->ActivateNewDoc();
	pBotToolsPanel->ActivateNewDoc();
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::UpdateControls()
{
	if (!pTopToolsPanel || !pBotToolsPanel)
		return;

	pTopToolsPanel->UpdateControls();
	pBotToolsPanel->UpdateControls();
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelContainer::SetWindowsPos(const RECT& rect)
{
	if (!pTopToolsPanel || !pBotToolsPanel)
		return;

	// position top toolbar
	pTopToolsPanel->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
	// position bottom toolbar
	pBotToolsPanel->SetWindowPos(NULL, rect.left, rect.bottom, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
}

//////////////////////////////////////////////////////////////////////
CTextPanelContainer::CTextPanelContainer(CCtp* pMainWnd)
:CToolsPanelContainer(pMainWnd)
{
}

//////////////////////////////////////////////////////////////////////
void CTextPanelContainer::Create()
{
	ATLASSERT(!pTopToolsPanel && !pBotToolsPanel);

	pTopToolsPanel = new CToolsPanelText1(m_pCtp);
	if (!pTopToolsPanel)
		return;

	pBotToolsPanel = new CToolsPanelText2(m_pCtp);
	if (!pBotToolsPanel)
	{
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
		return;
	}

	pTopToolsPanel->SetBkgdImage1();
	pBotToolsPanel->SetBkgdImage2();

	pTopToolsPanel->Create(m_pCtp->m_hWnd);
	pBotToolsPanel->Create(m_pCtp->m_hWnd);
}

//////////////////////////////////////////////////////////////////////
void CTextPanelContainer::SetFonts()
{
	if (!pTopToolsPanel)
		return;

	((CToolsPanelText1*)pTopToolsPanel)->SetFonts();
}

//////////////////////////////////////////////////////////////////////
CGraphicsPanelContainer::CGraphicsPanelContainer(CCtp* pMainWnd)
:CToolsPanelContainer(pMainWnd)
{
}

//////////////////////////////////////////////////////////////////////
void CGraphicsPanelContainer::Create()
{
	ATLASSERT(!pTopToolsPanel && !pBotToolsPanel);

	pTopToolsPanel = new CToolsPanelGraphics1(m_pCtp);
	if (!pTopToolsPanel)
		return;

	pBotToolsPanel = new CToolsPanelGraphics2(m_pCtp);
	if (!pBotToolsPanel)
	{
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
		return;
	}

	pTopToolsPanel->SetBkgdImage1();
	pBotToolsPanel->SetBkgdImage2();

	pTopToolsPanel->Create(m_pCtp->m_hWnd);
	pBotToolsPanel->Create(m_pCtp->m_hWnd);
}

//////////////////////////////////////////////////////////////////////
CCalPanelContainer::CCalPanelContainer(CCtp* pMainWnd)
:CToolsPanelContainer(pMainWnd)
{
}

//////////////////////////////////////////////////////////////////////
void CCalPanelContainer::Create()
{
	ATLASSERT(!pTopToolsPanel && !pBotToolsPanel);

	pTopToolsPanel = new CToolsPanelCal1(m_pCtp);
	if (!pTopToolsPanel)
		return;

	pBotToolsPanel = new CToolsPanelCal2(m_pCtp);
	if (!pBotToolsPanel)
	{
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
		return;
	}

	pTopToolsPanel->SetBkgdImage1();
	pBotToolsPanel->SetBkgdImage2();

	pTopToolsPanel->Create(m_pCtp->m_hWnd);
	pBotToolsPanel->Create(m_pCtp->m_hWnd);
}

//////////////////////////////////////////////////////////////////////
CImagePanelContainer::CImagePanelContainer(CCtp* pMainWnd)
:CToolsPanelContainer(pMainWnd)
{
}

//////////////////////////////////////////////////////////////////////
void CImagePanelContainer::Create()
{
	ATLASSERT(!pTopToolsPanel && !pBotToolsPanel);

	pTopToolsPanel = new CToolsPanelImage1(m_pCtp);
	if (!pTopToolsPanel)
		return;
	
	if (!m_pCtp->IsAmericanGreetings())
		pBotToolsPanel = new CToolsPanelImage1(m_pCtp);
	else
		pBotToolsPanel = new CToolsPanelImage2(m_pCtp);

	if (!pBotToolsPanel)
	{
		delete pTopToolsPanel;
		pTopToolsPanel = NULL;
		return;
	}

	pTopToolsPanel->SetBkgdImage1();
	pBotToolsPanel->SetBkgdImage2();

	pTopToolsPanel->Create(m_pCtp->m_hWnd);
	pBotToolsPanel->Create(m_pCtp->m_hWnd);
}

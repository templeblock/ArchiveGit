#pragma once

#include "resource.h"
#include "AGDoc.h"

class CCtp;
class CToolsCtlPanel;

class CToolsPanelContainer
{
public:
	CToolsPanelContainer(CCtp* pMainWnd);
	virtual ~CToolsPanelContainer();

	virtual void Free();
	virtual void Create();
	virtual void ShowWindow(int nCmdShow);
	virtual void SetWindowsPos(const RECT& rect);

	virtual void ActivateNewDoc();
	virtual void UpdateControls();

	virtual CToolsCtlPanel* GetTopToolPanel() { return pTopToolsPanel; }
	virtual CToolsCtlPanel* GetBotToolPanel() { return pBotToolsPanel; }

public:
	CCtp* m_pCtp;
	CToolsCtlPanel* pTopToolsPanel;
	CToolsCtlPanel* pBotToolsPanel;
};

class CTextPanelContainer : public CToolsPanelContainer
{
public:
	CTextPanelContainer(CCtp* pMainWnd);
	~CTextPanelContainer(){};

	void Create();
	void SetFonts();
};

class CGraphicsPanelContainer : public CToolsPanelContainer
{
public:
	CGraphicsPanelContainer(CCtp* pMainWnd);
	~CGraphicsPanelContainer(){};

	void Create();
};

class CCalPanelContainer : public CToolsPanelContainer
{
public:
	CCalPanelContainer(CCtp* pMainWnd);
	~CCalPanelContainer(){};

	void Create();
};

class CImagePanelContainer : public CToolsPanelContainer
{
public:
	CImagePanelContainer(CCtp* pMainWnd);
	~CImagePanelContainer(){};

	void Create();
};

#pragma once
#include "atlscrl.h"
#include "atlprint.h"

class CPrintPreviewWindowEx : public CPrintPreviewWindow, public CScrollImpl<CPrintPreviewWindowEx>
{
public:
	BEGIN_MSG_MAP(CPrintPreviewWindowEx)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CScrollImpl<CPrintPreviewWindowEx>)
	END_MSG_MAP()

	CPrintPreviewWindowEx()
	{
		Init(false/*bEnvelope*/);
	}

	void Init(bool bEnvelope)
	{
		m_nZoom = 0;
		m_nOldZoom = 0;
		m_BackgroundColor = (bEnvelope ? ::GetSysColor(COLOR_BTNSHADOW) : RGB(255,255,255)); //COLOR_BTNFACE
	}

	void OnDestroy();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void DoPaint(CDCHandle dc);

	int GetZoom()
	{
		return m_nZoom;
	}

	void SetZoom(int nZoom)
	{
		m_nZoom = nZoom;
	}

private:
	int m_nZoom;
	int m_nOldZoom;
	COLORREF m_BackgroundColor;
};

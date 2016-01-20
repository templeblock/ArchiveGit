// PrintPreviewWindowEx.h

#ifndef PRINT_PREVIEW_WINDOW_EX_H
#define PRINT_PREVIEW_WINDOW_EX_H
#include "atlscrl.h"
#include "atlprint.h"

class CPrintPreviewWindowEx: 
							 public CPrintPreviewWindow,
							 public CScrollImpl<CPrintPreviewWindowEx>
{
public:
	BEGIN_MSG_MAP(CPrintPreviewWindowEx)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CScrollImpl<CPrintPreviewWindowEx>)
	END_MSG_MAP()

	CPrintPreviewWindowEx() : m_nZoom(0),
							  m_nOldZoom(0)
	{
	}

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
	// Attributes
	int m_nZoom, m_nOldZoom;

};

#endif

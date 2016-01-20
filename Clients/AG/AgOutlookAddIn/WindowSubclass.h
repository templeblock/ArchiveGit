#pragma once

typedef void (CALLBACK * SUBCLASSCALLBACK)(LPARAM lParam1, LPARAM lParam2);

class CWindowSubclass : public CWindowImpl<CWindowSubclass>
{
public:
	//////////////////////////////////////////////////////////////////////
	CWindowSubclass::CWindowSubclass()
	{
		m_pCallBack = NULL;
		m_hWnd = NULL;
		m_lParam1 = NULL;
		m_lParam2 = NULL;
	}

	//////////////////////////////////////////////////////////////////////
	CWindowSubclass::~CWindowSubclass()
	{
	}

	//////////////////////////////////////////////////////////////////////
	void CWindowSubclass::OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	//////////////////////////////////////////////////////////////////////
	void CWindowSubclass::InitDialog(HWND hWnd, SUBCLASSCALLBACK pCallBack, LPARAM lParam1, LPARAM lParam2)
	{
		if (!hWnd)
			return;

		SubclassWindow(hWnd);
		m_pCallBack = pCallBack;
		m_hWnd = hWnd;
		m_lParam1 = lParam1;
		m_lParam2 = lParam2;
	}

	//////////////////////////////////////////////////////////////////////
	LRESULT CWindowSubclass::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = false;
		if (m_pCallBack)
		{
			m_pCallBack(m_lParam1, m_lParam2);
			m_pCallBack = NULL;
		}

		return S_OK;
	}

private:
	BEGIN_MSG_MAP(CWindowSubclass)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	SUBCLASSCALLBACK m_pCallBack;
	HWND m_hWnd;
	LPARAM m_lParam1;
	LPARAM m_lParam2;
};

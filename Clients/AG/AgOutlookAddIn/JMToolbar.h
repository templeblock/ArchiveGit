#pragma once
#include "OEAddin.h"
#include "atlcrack.h"

const UINT ID_BTN1_CMD			    = WM_APP + 801;
const UINT ID_BTN2_CMD			    = WM_APP + 802;
const UINT ID_BTN3_CMD			    = WM_APP + 803;
const UINT ID_BTN4_CMD			    = WM_APP + 804;
const UINT ID_BTN5_CMD			    = WM_APP + 805;
const UINT ID_BTN6_CMD			    = WM_APP + 806;
const UINT ID_BTN7_CMD			    = WM_APP + 807;
const UINT ID_BTN8_CMD			    = WM_APP + 808;
const UINT ID_BTN9_CMD			    = WM_APP + 809;
const UINT ID_BTN10_CMD			    = WM_APP + 810;
const UINT ID_BTN11_CMD			    = WM_APP + 811;

class CJMToolbar : public CWindowImpl<CJMToolbar>// CToolBarCtrl>
{
public:
	//DECLARE_WND_SUPERCLASS("CJMToolbar", TOOLBARCLASSNAME)

    CJMToolbar(void);
    ~CJMToolbar(void);

public:
	BEGIN_MSG_MAP(CJMToolbar)
	END_MSG_MAP()

protected:
	HWND m_hTopParent;
	HWND m_hRebarWnd;
	HWND m_hToolbar;
	UINT m_uOwnerClass;
	UINT m_uTimerID;
	BOOL m_bInitialized;
	BOOL m_bToolbarCreated;
	HIMAGELIST m_himlBtns;

public:

	const HWND GetRebarHandle() const
	{
		return m_hRebarWnd;
	}
	const UINT GetOwnerClass() const
	{
		return m_uOwnerClass;
	}
	HWND CreateJMToolbar (HWND hwndTopParent, HWND hwndRebar, UINT OwnerClass);
	BOOL AddButtons(HWND hwndToolbar);
	BOOL Initialize(HWND hwndOwner, const HINSTANCE hInst);
	BOOL Cleanup(void);
	BOOL MoveToLastBand();
	BOOL GetItemRect(int nIndex, LPRECT lpRect) const;
	BOOL RemoveBand();
	BOOL ShowBand(BOOL bShow);
	void GetBandIndex(INT &iBandIndex);
	
};

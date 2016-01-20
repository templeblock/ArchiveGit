#pragma once

// CHyperLink

class CHyperLink : public CButton
{
public:
	CHyperLink();
	virtual ~CHyperLink();

protected:
	DECLARE_DYNAMIC(CHyperLink)
	DECLARE_MESSAGE_MAP()

public:
	//Get the link text
	TCHAR* GetLinkText();
	//Set the link text
	void SetLinkText(TCHAR* lpText);
	//Get the link url
	TCHAR* GetLinkUrl();
	//Set the link url
	void SetLinkUrl(TCHAR* lpUrl);
	void LockInPosition(bool bLockInPosition = false) { m_bLockInPosition = bLockInPosition; }
	void IsLink(bool bIsLink = true) { m_bIsLink = bIsLink; }
	void UnderlineAlways(bool bUnderlineAlways = false) { m_bUnderlineAlways = bUnderlineAlways; }
	void SetColors(COLORREF crLinkUp, COLORREF crLinkHover, COLORREF crLinkDown, COLORREF crBackGround = ::GetSysColor(COLOR_BTNFACE));
	UINT GetShowIconsOpt() { return m_uShowIcon; }
	void SetIcons(HICON hIconUp, HICON hIconHover, HICON hIconDown, UINT uShowIcons);
	bool SetFont(TCHAR* lpFaceName = _T("Arial"), int nSize = 14, int nWeight = FW_NORMAL, bool bItalic = false);
	void Disable(bool bDisable = false) { m_bDisabled = bDisable; }

	enum {
		SI_ICONUP_ON    = 0x0001, //Show icon when mouse Up (normal)
		SI_ICONUP_LEFT  = 0x0002, //Show icon when mouse up on the left
		SI_ICONUP_RIGHT = 0x0004, //Show icon when mouse up on the right
		//
		SI_ICONHOVER_ON    = 0x0010, //Show icon when mouse hover
		SI_ICONHOVER_LEFT  = 0x0020, //Show icon when mouse hover on the left
		SI_ICONHOVER_RIGHT = 0x0040, //Show icon when mouse hover on the right
		//
		SI_ICONDOWN_ON    = 0x0100, //Show icon when mouse down
		SI_ICONDOWN_LEFT  = 0x0200, //Show icon when mouse down on the left
		SI_ICONDOWN_RIGHT = 0x0400 //Show icon when mouse down on the right
	};
private:
	void Initialize();
	int OpenUrl();

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	TCHAR* m_lpLinkText;
	TCHAR* m_lpLinkUrl;
	bool m_bUnderlineAlways;
	bool m_bIsLink;
	bool m_bLockInPosition;
	CFont m_hFont;
	COLORREF m_crLinkUp;
	COLORREF m_crLinkHover;
	COLORREF m_crLinkDown;
	//BackGround color
	COLORREF m_crBackGround;
	HCURSOR m_hReg;
	HCURSOR m_hHand;
	HICON m_hIconUp;
	HICON m_hIconHover;
	HICON m_hIconDown;
	UINT  m_uShowIcon;
	CToolTipCtrl m_ToolTip;
	//Status and Mouse control variables
	bool m_bDisabled;
	bool m_bMouseIn;
	bool m_bMouseDown;
	bool m_bChangePosAndSize;
};

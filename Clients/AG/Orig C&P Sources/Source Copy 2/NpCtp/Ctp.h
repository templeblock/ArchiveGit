// Ctp.h : Declaration of the CCtp

#ifndef __CTP_H_
#define __CTP_H_

#include "CtlPanel.h"
#include "AGDoc.h"
#include "AGSym.h"
#include "AGDC.h"
#include "Font.h"

#include "npapi.h"

class CCtp : public CWindowImpl<CCtp>
{
public:
	CCtp()
	{
		m_pCtlPanel = NULL;
		m_pClientDC = NULL;
		m_pAGDoc = NULL;
		m_pText = NULL;
		SetRect (&m_PageRect, 0, 0, 0, 0);
		SetRect (&m_ShadowRect, 0, 0, 0, 0);
		m_pDownloadData = NULL;
		m_dwDownloadSize = 0;
		m_hBitmap = NULL;
		m_szFontURL[0] = 0;
		m_pNPPInstance = NULL;
		m_bHasFocus = false;
	}


BEGIN_MSG_MAP(CCtp)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_CHAR, OnChar)
	MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
END_MSG_MAP()


public:


	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL &bHandled)
	{
		bHandled = TRUE;
		return (TRUE);
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);

	LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL &bHandled)
	{
		ATL_DRAWINFO di;
		PAINTSTRUCT ps;
		BeginPaint(&ps);
		di.hdcDraw = ps.hdc;
		RECT ClientRect;
		GetClientRect(&ClientRect);
		di.prcBounds = (const RECTL *) &ClientRect;
		SaveDC(di.hdcDraw);
		OnDraw(di);
		RestoreDC(di.hdcDraw, -1);
		EndPaint(&ps);

		bHandled = TRUE;
		return (TRUE);
	}

	void CreateBackPage();
	void DrawEditRect(CAGDC *pDC);
	void FileData(BYTE *pBytes, DWORD dwLen);
	void FileEnd();
	void FileStart();
	void FontData(const char *pszFontFile, BYTE *pBytes, DWORD dwLen);
	void FontEnd(const char *pszFontFile);
	void FontStart(const char *pszFontFile);
	BOOL GetAmbientDisplayName(BSTR &)
		{ return (FALSE); }
	void GetAmbientUserMode(BOOL &bUserMode)
		{ bUserMode = TRUE; }

	CFontList &GetFontList()
		{ return (m_FontList); }
	CAGSymImage *GetImage(int nID);
	CAGText *GetText()
		{ return (m_pText); }
	bool HasFocus()
		{ return (m_bHasFocus); }
	void NewPage();
	void SetFontURL(const char *pszFontURL)
		{ lstrcpy(m_szFontURL, pszFontURL); }
	void SetNPPInstance(NPP pInstance)
		{ m_pNPPInstance = pInstance; }
	void StartDownloadFont(const char *pszFontName)
	{
		char szFontURL[_MAX_PATH];
		lstrcpy(szFontURL, m_szFontURL);
		lstrcat(szFontURL, pszFontName);

		NPN_GetURL(m_pNPPInstance, szFontURL, NULL);
	}
	void StartEdit(CAGSymText *pText, POINT Pt, bool bClick);
	void StopEdit();

protected:
	CCtlPanel			*m_pCtlPanel;
	CAGClientDC 		*m_pClientDC;
	CAGDoc				*m_pAGDoc;
	CAGSymText			*m_pText;
	RECT				m_PageRect;
	RECT				m_ShadowRect;
	CAGMatrix			m_ViewMatrix;
	BOOL				m_bWindowOnly;
	BYTE				*m_pDownloadData;
	DWORD				m_dwDownloadSize;
	HBITMAP 			m_hBitmap;
	char				m_szFontURL[_MAX_PATH];
	NPP 				m_pNPPInstance;
	CFontList			m_FontList;
	FONTDOWNLOADARRAY	m_FontDownloadArray;
	bool				m_bHasFocus;
};


#endif //__CTP_H_

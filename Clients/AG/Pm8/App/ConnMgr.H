#if !defined(AFX_CONNMGR_H__66E17A82_6367_11D2_8BEF_DE07821ECC4D__INCLUDED_)
#define AFX_CONNMGR_H__66E17A82_6367_11D2_8BEF_DE07821ECC4D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CConnMgr wrapper class

class CConnMgr : public CWnd
{
protected:
	DECLARE_DYNCREATE(CConnMgr)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x94fb2c46, 0x512e, 0x11d2, { 0x8b, 0xef, 0x0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:
	CString GetCaption();
	void SetCaption(LPCTSTR);

// Operations
public:
	long Connect();
	BOOL IsConnected();
	long EditPrefs();
	BOOL HangUp();
	BOOL EnableIsConnectedCheck(BOOL bEnable);
	BOOL EnableHangUpPrompt(BOOL bEnable);
	void SetURL(LPCTSTR szURL);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNMGR_H__66E17A82_6367_11D2_8BEF_DE07821ECC4D__INCLUDED_)

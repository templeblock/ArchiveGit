#pragma once
#ifndef __BANDEDITCTRL_H_
#define __BANDEDITCTRL_H_


#include <atlctrls.h>
#include <atlmisc.h>
#include <commctrl.h>
#include "RegKeys.h"

class CJMToolband;

class RegIterator
{
public:
    RegIterator() : m_nIndex(0)
    {
		if (m_hKey.Create(REGKEY_APPROOT, REGKEY_SEARCH_HISTORY) == ERROR_SUCCESS)
		{
			m_nIndex = 0;
			operator++ (1);
		}
    }
    ~RegIterator() {;}
    
    void operator++ (int)
    {
        unsigned long size = 200;
        m_bValid = ((::RegEnumValue(m_hKey, m_nIndex++, m_szBuf, &size, 0, 0, 0, 0)) != ERROR_NO_MORE_ITEMS);
    }

    operator bool() {return m_bValid;}
    operator TCHAR* () { return m_szBuf;}
    operator int() { return m_nIndex - 1;}

protected:
    CRegKey m_hKey;
    TCHAR m_szBuf[200];
    int m_nIndex;
    bool m_bValid;
};


#include <string>
class CBandComboBoxCtrl : public CWindowImpl<CBandComboBoxCtrl, CComboBox>
{
public:	
    CBandComboBoxCtrl();
	virtual ~CBandComboBoxCtrl();
    void SetWebBrowser(IWebBrowser2* pWebBrowser) {m_spWebBrowser = pWebBrowser;}

// Operations
public:
	STDMETHOD(TranslateAcceleratorIO)(LPMSG pMsg);

   
                             
    DECLARE_WND_SUPERCLASS(_T("JazzyBand_ComboBox"), TEXT("COMBOBOX"))

	BEGIN_MSG_MAP(CBandComboBoxCtrl)
        REFLECTED_COMMAND_CODE_HANDLER(CBN_EDITUPDATE, OnTVSelUpdate)
        REFLECTED_COMMAND_CODE_HANDLER(CBN_SELENDOK, OnTVSelChanged)
        REFLECTED_COMMAND_CODE_HANDLER(CBN_SETFOCUS, OnTVSetFocus)
        REFLECTED_COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnTVKillFocus)
    END_MSG_MAP()

    LRESULT OnTVSelUpdate(WORD, WORD pnmh, HWND,BOOL& );
    LRESULT OnTVSelChanged(WORD, WORD pnmh, HWND,BOOL& );
    LRESULT OnTVSetFocus(WORD, WORD pnmh, HWND,BOOL& );
    LRESULT OnTVKillFocus(WORD, WORD pnmh, HWND,BOOL& );  
    LRESULT Process(LPCTSTR szQuery);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LPDATAOBJECT m_pIDataObject;

    bool InsertHistory();
    void SetDropDownSize(UINT LinesToDisplay);
	void SetToolBand(CJMToolband *pToolband);
	bool ProcessSearch();

    typedef std::basic_string<TCHAR> zstring;
public:
	CJMToolband* m_pBand;


protected:
	ULONG m_cRef;
    bool m_bAuto;
    CComPtr<IWebBrowser2> m_spWebBrowser;
};

#endif //__BANDEDITCTRL_H_
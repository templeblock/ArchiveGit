#pragma once

#include <mapix.h>
//j #include <edkmdb.h>
#pragma comment(lib, "mapi32")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMapiAdmin  
{
public:
	bool IsLoggedOn();
	
	CMapiAdmin();
	virtual ~CMapiAdmin();
	void Logoff();
	bool Logon(HWND hWndParent, LPTSTR lpszProfileName, LPTSTR lpszPassword, FLAGS flFlags);

	bool SendMessage();
	bool GetStorePath(CString strStoreID, CString& strStorePath);
	bool RemoveService(BYTE* pbyEntryID, DWORD dwSize);
	bool RemoveService(LPTSTR lpszDisplayName);
	
	bool GetProfileName(LPTSTR& lpszProfileName);
	
	bool CreateMessage(DWORD dwRecipCount, LPCTSTR* ppRecipents, LPCTSTR pMsgSubject, LPCTSTR pMsgText, BOOL bImportant);

	bool CreateNewProfile(LPTSTR lpszNewName);
	bool CreateMsgService(LPTSTR lpszService, LPTSTR lpszDisplayName);

protected:
	void TestTag(LPSPropValue lpProp);
	int QueryRows(LPMAPITABLE pTable, SRestriction* pRestriction, LPSRowSet& lpRows /* out */, int nMin=0, int nMax=9999);
	CString ByteArrayToString(BYTE* pData, int nSize);
	CString ByteArrayToString(LPSPropValue lpProperty);

	bool m_bIsInitialized;
	LPMAPIFOLDER		m_pOutBoxFolder;
	LPMDB				m_pMessageStore;
	LPMAPITABLE			m_pStatusTable;
	LPMAPITABLE			m_pMsgStoresTable;
	LPMAPITABLE			m_pMsgServiceTable;
	LPPROFADMIN         m_pProfAdmin;
	IMsgServiceAdmin* m_pServiceAdmin;
	IMAPISession*		m_pSession;
	LPMESSAGE			m_pMessage;
	LPADRBOOK			m_pAddressBook;

	SRestriction		m_sRestrict;    
	SPropValue			m_sProps;

private:
	HRESULT hr;
};

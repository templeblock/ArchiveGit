// ConnMgrW.h: interface for the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNMGRW_H__96F68323_641E_11D2_8BEF_ACC4E039B776__INCLUDED_)
#define AFX_CONNMGRW_H__96F68323_641E_11D2_8BEF_ACC4E039B776__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iconnmgr.h"			// IConnectionManager interface
#include "connmgr.h"

// Connection Manager Wrapper
// The purpose of this class is to serve as a wrapper for the Connection Manager
// control so that all methods can safely be called if control creation fails.

// Note: This class does not use virtual functions
class CConnectionManager : public CConnMgr  
{
public:
	CConnectionManager();
	virtual ~CConnectionManager();

   // Create the control
   BOOL  Create(CWnd* pParentWnd, UINT nID);
   BOOL  IsCreated(void);

// Operations
public:
	long Connect();
	BOOL IsConnected();
	long EditPrefs();
	BOOL HangUp();
	BOOL EnableIsConnectedCheck(BOOL bEnable);
	BOOL EnableHangUpPrompt(BOOL bEnable);
	void SetURL(LPCTSTR szURL);
private:
   BOOL  m_bCreated;

protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XConnectionManager.
	BEGIN_INTERFACE_PART(ConnectionManager, IConnectionManager)
		INIT_INTERFACE_PART(CConnectionManager, ConnectionManager)
		STDMETHOD_(long, Connect) (THIS);
		STDMETHOD_(BOOL, IsConnected) (THIS);
		STDMETHOD_(long, EditPrefs) (THIS);
		STDMETHOD_(BOOL, HangUp) (THIS);
		STDMETHOD_(BOOL, EnableIsConnectedCheck)(THIS_ BOOL bEnable);
		STDMETHOD_(BOOL, EnableHangUpPrompt) (THIS_ BOOL bEnable);
		STDMETHOD_(void, SetURL) (THIS_ LPCTSTR szURL);
	END_INTERFACE_PART(ConnectionManager)
};

inline BOOL CConnectionManager::IsCreated()
{
   return m_bCreated;
}

#endif // !defined(AFX_CONNMGRW_H__96F68323_641E_11D2_8BEF_ACC4E039B776__INCLUDED_)

// ConnMgrW.cpp: implementation of the CConnectionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "ConnMgrW.h"
#include "RegCtrl.h"    // For registering OCX controls

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConnectionManager::CConnectionManager()
{
   m_bCreated = FALSE;
}

CConnectionManager::~CConnectionManager()
{

}

BOOL CConnectionManager::Create(CWnd* pParentWnd, UINT nID)
{
   if(m_bCreated)
      return TRUE;

   CRect crControl(0, 0, 1, 1);
   m_bCreated = CConnMgr::Create(
      NULL,
      "Connection Manager",
      WS_CHILD,
      crControl,
      pParentWnd,
      nID);
   // If Connection Manager creation failed, it may not be registered
   // If not registered, try to automatically register it
   if(!m_bCreated)
   {
      CRegisterControl              regCtrl;
      CRegisterControl::ErrorCode   errorCode;

      errorCode = regCtrl.Register("ConnMgr.ocx");
      if(errorCode == CRegisterControl::ecNone)
      {
         m_bCreated = CConnMgr::Create(
            NULL,
            "Connection Manager",
            WS_CHILD,
            crControl,
            pParentWnd,
            nID);
      }
   }
   return m_bCreated;
}

long CConnectionManager::Connect()
{
   if(!IsCreated())
      return CInternetConnectStatus::statusInitFailed;

   return CConnMgr::Connect();
}

BOOL CConnectionManager::IsConnected()
{
   if(!IsCreated())
      return FALSE;

   return CConnMgr::IsConnected();
}

long CConnectionManager::EditPrefs()
{
   if(!IsCreated())
      return FALSE;
   return CConnMgr::EditPrefs();
}

BOOL CConnectionManager::HangUp()
{
   if(!IsCreated())
      return FALSE;
   return CConnMgr::HangUp();
}

BOOL CConnectionManager::EnableIsConnectedCheck(BOOL bEnable)
{
   if(!IsCreated())
      return FALSE;
   return CConnMgr::EnableIsConnectedCheck(bEnable);
}

BOOL CConnectionManager::EnableHangUpPrompt(BOOL bEnable)
{
   if(!IsCreated())
      return FALSE;
   return CConnMgr::EnableHangUpPrompt(bEnable);
}

void CConnectionManager::SetURL(LPCTSTR szURL)
{
   if(!IsCreated())
      return;

   CConnMgr::SetURL(szURL);
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CConnectionManager

BEGIN_INTERFACE_MAP(CConnectionManager, CConnMgr)
	INTERFACE_PART(CConnectionManager, IID_IConnectionManager, ConnectionManager)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CConnectionManager.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CConnectionManager::XConnectionManager::AddRef()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CConnectionManager::XConnectionManager::Release()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->ExternalRelease();
}

STDMETHODIMP CConnectionManager::XConnectionManager::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IConnectionManager interface for CConnectionManager.

STDMETHODIMP_(long) CConnectionManager::XConnectionManager::Connect()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->Connect();
}

STDMETHODIMP_(BOOL) CConnectionManager::XConnectionManager::IsConnected()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->IsConnected();
}

STDMETHODIMP_(long) CConnectionManager::XConnectionManager::EditPrefs()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->EditPrefs();
}

STDMETHODIMP_(BOOL) CConnectionManager::XConnectionManager::HangUp()
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->HangUp();
}

STDMETHODIMP_(BOOL) CConnectionManager::XConnectionManager::EnableIsConnectedCheck(BOOL bEnable)
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->EnableIsConnectedCheck(bEnable);
}

STDMETHODIMP_(BOOL) CConnectionManager::XConnectionManager::EnableHangUpPrompt (BOOL bEnable)
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	return pThis->EnableHangUpPrompt(bEnable);
}

STDMETHODIMP_(void) CConnectionManager::XConnectionManager::SetURL(LPCTSTR szURL)
{
	METHOD_PROLOGUE(CConnectionManager, ConnectionManager)
	pThis->SetURL(szURL);
}

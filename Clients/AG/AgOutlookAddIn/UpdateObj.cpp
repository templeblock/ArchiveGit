// UpdateObj.cpp : Implementation of CUpdateObj

#include "stdafx.h"
#include "UpdateObj.h"

////////////////////////////////////////////////////////////////////////////////
CUpdateObj::CUpdateObj()
{
}

////////////////////////////////////////////////////////////////////////////////
CUpdateObj::~CUpdateObj()
{
}

///////////////////////////////////////////////////////////////////////////////
void CUpdateObj::AutoUpdateDone(DWORD dwResult)
{
	Fire_AutoUpdateComplete(dwResult);
}

////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CUpdateObj::DoAutoUpdate(OLE_HANDLE hwndOwner, VARIANT_BOOL Silent)
{
	HWND hwnd = (HWND) hwndOwner;
	if (::IsWindow(hwnd))
		m_AutoUpdate.SetWndOwner(hwnd);

	bool bSilent = (Silent != VARIANT_FALSE);
	m_AutoUpdate.Start(bSilent);
	return S_OK;
}

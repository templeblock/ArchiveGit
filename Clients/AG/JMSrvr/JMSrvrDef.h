#pragma once
#include "stdafx.h"
#include "resource.h"
#include "JMSrvr.h"
#include "SingleInstance.h"

class CJMSrvrModule : public CAtlExeModuleT< CJMSrvrModule >, 
					  public CAtlBaseModule//,
					 // public CComModule
{
public :
	DECLARE_LIBID(LIBID_JMSrvrLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_JMSRVR, "{07DA3D00-195B-43B3-AF4B-F1E3BE5D1381}")

	CSingleInstance m_SingleInstance;
	CRITICAL_SECTION m_csWindowCreate;

	int WinMain(int nShowCmd) throw();
	HRESULT PreMessageLoop(int nShowCmd) throw( );

	HRESULT PostMessageLoop() throw( );


};
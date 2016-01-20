// Async.h : Declaration of the CAsync
#pragma once

#include "Download.h"
#include "AutoUpdate.h"
#include "Extensions.h"


class CAsync
{
public:
friend CAutoUpdate;

	/////////////////////////////////////////////////////////////////////////////
	CAsync();
	virtual ~CAsync();
	/////////////////////////////////////////////////////////////////////////////

	virtual void AutoUpdateDone(DWORD dwResult) = 0;

	
protected:
	CDownload m_Download;
	CAutoUpdate m_AutoUpdate;
	CExtensions m_Extend;


	
};

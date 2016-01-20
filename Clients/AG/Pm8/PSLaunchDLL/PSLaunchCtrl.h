// PSLaunchCtrl.h : Declaration of the CPSLaunchCtrl

#ifndef __PSLAUNCHCTRL_H_
#define __PSLAUNCHCTRL_H_

#include "LaunchInterfaces.h"

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchCtrl
class CPSLaunchCtrl : public CCmdTarget
{
	DECLARE_DYNCREATE(CPSLaunchCtrl)

public:
	CPSLaunchCtrl();

// Implmentation
protected:
	~CPSLaunchCtrl();

	DECLARE_OLECREATE_EX(CPSLaunchCtrl)    // Class factory and guid

// Interface Maps
protected:    
	
	DECLARE_INTERFACE_MAP()
    BEGIN_INTERFACE_PART(LaunchCtrl, IPSLaunchCtrl)
		STDMETHOD(DoFileOpen)(HWND hwndParent, EAppType eAppType, EResult* pResult);
		STDMETHOD(DoFileNew)(HWND hwndParent, EAppType eAppType, EResult* pResult);
		STDMETHOD(GetData)(IPSLaunchData** ppObj);
	END_INTERFACE_PART(LaunchCtrl)

protected:

//	BOOL					AlreadyRunning( HSZ hServName );
	HWND					FindAppWindow( const CString& strPropName );

private:

//	IPSLaunchData*		m_pData;
//	ULONG				m_nRefCnt;

};

#endif //__PSLAUNCHCTRL_H_

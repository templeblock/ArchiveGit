// PSLaunchData.h : Declaration of the CPSLaunchData

#ifndef __PSLAUNCHDATA_H_
#define __PSLAUNCHDATA_H_

//#include "LaunchInterfaces.h"
#include "CommonTypes.h"

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchData
class CPSLaunchData : public CCmdTarget
{
	DECLARE_DYNCREATE(CPSLaunchData)

public:
	CPSLaunchData()
	{
	}

	CPSLaunchData( const class CProjectInfo& cProjInfo )
		: m_cProjInfo( cProjInfo )
	{
	}

	IUnknown* GetIUnknown() { return (IUnknown *) &m_xLaunchData; }

	// Implementation
protected:

	~CPSLaunchData();

	DECLARE_OLECREATE_EX(CPSLaunchData)    // Class factory and guid

// Interface Maps
protected:    
	
	DECLARE_INTERFACE_MAP()
    BEGIN_INTERFACE_PART(LaunchData, IPSLaunchData)
		STDMETHOD_( short, GetProjectMethod)( void );
		STDMETHOD_(short, GetProjectPaperSize)(void);
		STDMETHOD_(short, GetProjectFormat)(void);
		STDMETHOD_(short, GetProjectType)(void);
		STDMETHOD_(void, GetQSLData)( QSLData* m_pData);
		STDMETHOD_(void, GetDocDescription)(SBDocDescription* m_pData);
		STDMETHOD(Initialize)(HANDLE hFile);
	END_INTERFACE_PART(LaunchData)

private:

	CProjectInfo	m_cProjInfo;
};

#endif //__PSLAUNCHDATA_H_

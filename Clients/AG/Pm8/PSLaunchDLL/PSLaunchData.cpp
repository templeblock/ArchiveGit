// PSLaunchData.cpp : Implementation of CPSLaunchData
#include "stdafx.h"
#include "LaunchResource.h"

#include "PSLaunchDLL.h"
#include "PSLaunchData.h"

#include <afxtempl.h>

static CMap<ULONG, ULONG, short, short> cPaperProjectMap;

IMPLEMENT_DYNCREATE(CPSLaunchData, CCmdTarget)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPSLaunchData, "PSLaunch.PSLaunchData.1",
	0x8DC39CD1, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0)

/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_IPSLaunchData =
		{ 0x8DC39CD0, 0x4D90, 0x11d2, { 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0 } };

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchData::CPSLaunchDataFactory::UpdateRegistry -
// Adds or removes system registry entries for CPSLaunchCtrl

BOOL CPSLaunchData::CPSLaunchDataFactory::UpdateRegistry(BOOL bRegister)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchData

CPSLaunchData::~CPSLaunchData()
{
}

BEGIN_INTERFACE_MAP(CPSLaunchData, CCmdTarget)
    INTERFACE_PART(CPSLaunchData, IID_IPSLaunchData, LaunchData)
END_INTERFACE_MAP()

ULONG CPSLaunchData::XLaunchData::AddRef( )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);
	return pThis->ExternalAddRef();
}

ULONG CPSLaunchData::XLaunchData::Release( )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);
	return pThis->ExternalRelease();
}

HRESULT CPSLaunchData::XLaunchData::QueryInterface( REFIID iid, void** ppvObj )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CPSLaunchData::XLaunchData::Initialize( HANDLE hFile )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);

	LPVOID pData = ::MapViewOfFile(
		(HANDLE) hFile,		// file-mapping object to map into address space
		FILE_MAP_READ,		// access mode
		0,					// high-order 32 bits of file offset
		0,					// low-order 32 bits of file offset
		0 );				// number of bytes to map);

	if (!pData)
		return E_FAIL;

	memcpy( &(pThis->m_cProjInfo), pData, sizeof( pThis->m_cProjInfo ) );
	::UnmapViewOfFile( pData );

	return S_OK;
}

STDMETHODIMP_(short) CPSLaunchData::XLaunchData::GetProjectMethod( void )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);

	return pThis->m_cProjInfo.eProjectMethod;
}

STDMETHODIMP_(short) CPSLaunchData::XLaunchData::GetProjectType( void )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);

	if (cPaperProjectMap.IsEmpty())
	{
		// Load the paper project map
		//
		HMODULE hModule = AfxGetResourceHandle();
		HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE( ID_PROJECT_PAPER_MAP ), "SYSTEMDATA" );

		if (hRes)
		{
			HGLOBAL hData = ::LoadResource( hModule, hRes );

			if (hData)
			{
				int nCount = SizeofResource( hModule, hRes ) / (sizeof( short ) * 3);
				short *m_pData = (short *) hData;

				for (int i = 0; i < nCount; i++)
				{
					cPaperProjectMap.SetAt( MAKELONG( m_pData[1], m_pData[0] ), m_pData[2] );
					m_pData += 3;
				}

				::FreeResource( hData );
			}
		}
	}

	short nProjType = pThis->m_cProjInfo.nProjectType;
	short nLookupValue;

	// NOTE: projects are 1 based in the resource file, and 0 based in 
	// the application.  So add 1 to the project type when doing lookups,
	// and subtract 1 from the resulting lookup value.
	if (cPaperProjectMap.Lookup( MAKELONG( pThis->m_cProjInfo.nPaperType, nProjType + 1 ), nLookupValue )) 
		nProjType = nLookupValue - 1;
	else if (cPaperProjectMap.Lookup( MAKELONG( kAllPaperTypes, nProjType + 1 ), nLookupValue )) 
		nProjType = nLookupValue - 1;

	if (pThis->m_cProjInfo.nProjectType >= kPressWriterProjectStart)
	{
		nProjType -= kPressWriterProjectStart;
	}

	return nProjType;
}

STDMETHODIMP_(short) CPSLaunchData::XLaunchData::GetProjectFormat( void )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);

	return pThis->m_cProjInfo.nProjectFormat;
}

STDMETHODIMP_(short) CPSLaunchData::XLaunchData::GetProjectPaperSize( void )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);

	return pThis->m_cProjInfo.nPaperType; 
}

STDMETHODIMP_(void) CPSLaunchData::XLaunchData::GetQSLData( QSLData* m_pData )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);
	*m_pData = pThis->m_cProjInfo.sQSLData; 
}

STDMETHODIMP_(void) CPSLaunchData::XLaunchData::GetDocDescription( SBDocDescription* m_pData )
{
	METHOD_PROLOGUE(CPSLaunchData, LaunchData);
	*m_pData = pThis->m_cProjInfo.sDocDesc; 
}

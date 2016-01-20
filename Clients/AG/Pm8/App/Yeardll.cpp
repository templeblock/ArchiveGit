#include "stdafx.h"

#include "yeardll.h"

CYearDll::CYearDll()
{
	m_hLibrary = NULL;
	m_fInitialized = FALSE;
	m_pStartUp = NULL;
	m_pShutDown = NULL;
	m_pSetDateAndName = NULL;
	m_pGetData = NULL;
	m_pGetCategories = NULL;
	m_pGetErrorCode = NULL;
	m_pGetErrorString = NULL;
}

CYearDll::~CYearDll()
{
	FreeDll();
}

int CYearDll::Initialize(LPCSTR pszDllPath, LPCSTR pszStandardPath, LPCSTR pszAdditionalPath, CYearDllInterface*& pInterface)
{
	// Release any existing attachment to the DLL.
	FreeDll();

	// Get the full name of the DLL.
	CString csDllName = pszDllPath;
	csDllName += "\\YEAR32.DLL";

	// Attempt to load the DLL.
	m_hLibrary = LoadLibrary(csDllName);
	if (m_hLibrary == NULL)
	{
		return ERR_DLLNOTFOUND;
	}

	// Get the entry points. They must all exist.
	m_pStartUp = (FN_StartUp)GetProcAddress(m_hLibrary, "StartUp");
	m_pShutDown = (FN_ShutDown)GetProcAddress(m_hLibrary, "ShutDown");
	m_pSetDateAndName = (FN_SetDateAndName)GetProcAddress(m_hLibrary, "SetDateAndName");
	m_pGetData = (FN_GetData)GetProcAddress(m_hLibrary, "GetData");
	m_pGetCategories = (FN_GetCategories)GetProcAddress(m_hLibrary, "GetCategories");
	m_pGetErrorCode = (FN_GetErrorCode)GetProcAddress(m_hLibrary, "GetErrorCode");
	m_pGetErrorString = (FN_GetErrorString)GetProcAddress(m_hLibrary, "GetErrorString");

	if ((m_pStartUp == NULL)
	 || (m_pShutDown == NULL)
	 || (m_pSetDateAndName == NULL)
	 || (m_pGetData == NULL)
	 || (m_pGetCategories == NULL)
	 || (m_pGetErrorCode == NULL)
	 || (m_pGetErrorString == NULL))
	{
		FreeDll();
		return ERR_ENTRYPOINTNOTFOUND;
	}

	// Call the StartUp method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		(*m_pStartUp)(pszStandardPath, pszAdditionalPath);
	}
	CATCH_ALL(e)
	{
		nResult = ERR_INITIALIZING;
	}
	END_CATCH_ALL
	if (nResult != ERR_STATUSOK)
	{
		FreeDll();
		return nResult;
	}

	// We started up, so remember to shut down.
	m_fInitialized = TRUE;

	// Initialize our interface object and return its address.
	pInterface = this;

	return ERR_STATUSOK;
}

void CYearDll::FreeDll(void)
{
	if (m_hLibrary != NULL)
	{
		// The DLL and "started up", then we need to "shut it down".
		if (m_fInitialized)
		{
			if (m_pShutDown != NULL)
			{
				TRY
				{
					(*m_pShutDown)();
				}
				END_TRY
			}
			m_fInitialized = FALSE;
		}

		// Release the entry points.
		m_pStartUp = NULL;
		m_pShutDown = NULL;
		m_pSetDateAndName = NULL;
		m_pGetData = NULL;
		m_pGetCategories = NULL;
		m_pGetErrorCode = NULL;
		m_pGetErrorString = NULL;

		// Free the library.
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}
}

int CYearDll::Release(void)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// We are released, so we're no longer valid. I guess we could
	// free the DLL at this point, but that's really the job of the
	// DLL object.
	m_fInitialized = FALSE;

	return ERR_STATUSOK;
}

int CYearDll::SetDateAndName(SYSTEMTIME Birthdate, LPCSTR pszName)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// Call the SetDateAndName method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		(*m_pSetDateAndName)(Birthdate, pszName);
	}
	CATCH_ALL(e)
	{
		nResult = ERR_UNKNOWNERROR;
	}
	END_CATCH_ALL

	// Return the result, whatever it is.
	return nResult;
}

int CYearDll::GetData(DWORD dwCategory, CString& csData)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// Call the GetData method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		nResult = (*m_pGetData)(dwCategory, NULL);
		if (nResult >= 0)
		{
			LPSTR pszBuffer = csData.GetBuffer(nResult);
			if (pszBuffer == NULL)
			{
				nResult = ERR_DATANOTFOUND;
			}
			else
			{
				nResult = (*m_pGetData)(dwCategory, pszBuffer);
			}
			csData.ReleaseBuffer();
		}
	}
	CATCH_ALL(e)
	{
		nResult = ERR_UNKNOWNERROR;
	}
	END_CATCH_ALL

	// Return the result, whatever it is.
	return nResult;
}

int CYearDll::GetCategories(CategoryData* pCategories)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// Call the GetCategories method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		nResult = (*m_pGetCategories)(pCategories);
	}
	CATCH_ALL(e)
	{
		nResult = ERR_UNKNOWNERROR;
	}
	END_CATCH_ALL

	// Return the result, whatever it is.
	return nResult;
}

int CYearDll::GetErrorCode(void)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// Call the GetErrorCode method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		nResult = (*m_pGetErrorCode)();
	}
	CATCH_ALL(e)
	{
		nResult = ERR_UNKNOWNERROR;
	}
	END_CATCH_ALL

	// Return the result, whatever it is.
	return nResult;
}

int CYearDll::GetErrorString(CString& csError)
{
	if (!m_fInitialized)
	{
		return ERR_INVALIDINTERFACE;
	}

	// Call the GetErrorString method of the DLL.
	int nResult = ERR_STATUSOK;
	TRY
	{
		nResult = (*m_pGetErrorString)(NULL);
		if (nResult >= 0)
		{
			LPSTR pszBuffer = csError.GetBuffer(nResult);
			if (pszBuffer == NULL)
			{
				nResult = ERR_DATANOTFOUND;
			}
			else
			{
				nResult = (*m_pGetErrorString)(pszBuffer);
			}
			csError.ReleaseBuffer();
		}
	}
	CATCH_ALL(e)
	{
		nResult = ERR_UNKNOWNERROR;
	}
	END_CATCH_ALL

	// Return the result, whatever it is.
	return nResult;
}

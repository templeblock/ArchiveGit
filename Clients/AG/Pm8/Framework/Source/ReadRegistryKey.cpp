// ****************************************************************************
//
//  File Name:			CollectionManager.cpp
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Definition of ReadRegistryKey global function
//
//  Portability:		Win32
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#include "FrameworkIncludes.h"	// standard Framework includes

ASSERTNAME

#include "ReadRegistryKey.h"		// declaration of ReadRegistryKey()


// ****************************************************************************
//
//  Function Name:	BOOL ReadRegistryKey( HKEY hTopKey, const CString& szRoot, 
//							const CString& szSubKey, CString& szResult )
//
//  Description:		Helper routine. Reads a key from the registry
//
//  Returns:			szResult
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
BOOL ReadRegistryKey( HKEY hTopKey, const CString& szRoot, const CString& szSubKey, CString& szResult )
{
	HKEY hKey = 0;
	BOOL bResult = FALSE;

	if (::RegOpenKeyEx( hTopKey, szRoot, (DWORD)0, KEY_READ, &hKey ) == ERROR_SUCCESS)
	{
		DWORD dwType = 0;
		DWORD dwSize = 0;
		if ( ::RegQueryValueEx( hKey, szSubKey, 0, &dwType, NULL, &dwSize ) == ERROR_SUCCESS)
		{
			char *pData = (char*) malloc( dwSize );
			if (pData != NULL)
			{
				if ( ::RegQueryValueEx( hKey, szSubKey, 0, &dwType, (LPBYTE) pData, &dwSize ) == ERROR_SUCCESS)
				{
					if (dwType == REG_SZ)
					{
						szResult = pData;
						bResult = TRUE;
					}
					else
					{
						szResult.Format("%x", pData );
					}
				}
				
				free( pData );
			}
		}
		::RegCloseKey(hKey);
	}
	return bResult;
}


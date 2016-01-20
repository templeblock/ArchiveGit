////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File name:		ConfigurationBase.cpp
//	Project:			Renaissance framework
//	Author:			G. Brown
//	Description:	Definition of base configuration classes
//	Portability:	Platform-independent
//	Developer:		Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//	Client:			Broderbund Software, Inc.
//
//	Copyright (c) 1997 Turning Point Software.  All Rights Reserved.
//

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ConfigurationBase.h"

#ifdef	_WINDOWS
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::RTopLevelConfiguration()
//	Description:	Constructor
//
RTopLevelConfiguration::RTopLevelConfiguration(HKEY hTopLevelKey)
:	m_hTopLevelKey(hTopLevelKey),
	m_topLevelPath()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::GetKey()
//	Description:	Returns m_hTopLevelKey
//
HKEY RTopLevelConfiguration::GetKey() const
{
	return m_hTopLevelKey;
}
#endif	// _WINDOWS

#ifdef	MAC
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::RTopLevelConfiguration()
//	Description:	Constructor
//
RTopLevelConfiguration::RTopLevelConfiguration()
{
}
#endif	// MAC

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
BOOLEAN RTopLevelConfiguration::GetStringValue(const RMBCString& rItem, RMBCString& rValue) const
{
#ifdef	_WINDOWS
	rValue.Empty();	
	uLONG uDataLength;
	if (GetDataLength(rItem, uDataLength))
	{
		// The value exists
		if (uDataLength > 0)
		{
			uBYTE* pData = NULL;
			try
			{
				DWORD uDataType;
				pData = new uBYTE[uDataLength];
				if (::RegQueryValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, &uDataType, pData, &uDataLength) != ERROR_SUCCESS) throw ::kRegistryError;
				TpsAssert(uDataType == REG_SZ, "Expected and actual registry data types differ.");
				rValue = RMBCString(reinterpret_cast<LPCTSTR>(pData));
				delete [] pData;
			}
			catch(YException)
			{
				delete [] pData;
				throw;
			}
		}	
		return TRUE;
	}
	else
	{
		// The value does not exist
		return FALSE;
	}	
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	rValue;
	return FALSE;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
void RTopLevelConfiguration::SetStringValue(const RMBCString& rItem, const RMBCString& rValue)
{	
#ifdef	_WINDOWS
	DWORD uType = REG_SZ;
	DWORD uDataSize = rValue.GetDataLength();
	if (::RegSetValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, uType, reinterpret_cast<const BYTE*>(static_cast<LPCTSTR>(rValue)), uDataSize) != ERROR_SUCCESS) throw ::kRegistryError;
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	rValue;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
BOOLEAN RTopLevelConfiguration::GetLongValue(const RMBCString& rItem, uLONG& uValue) const
{
#ifdef	_WINDOWS
	uLONG uDataLength;
	if (GetDataLength(rItem, uDataLength))
	{
		// The value exists
		DWORD uDataType;			
		if (::RegQueryValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, &uDataType, reinterpret_cast<BYTE*>(&uValue), &uDataLength) != ERROR_SUCCESS) throw ::kRegistryError;
		TpsAssert(uDataType == REG_DWORD, "Expected and actual registry data types differ.");
		return TRUE;
	}
	else
	{
		// The value does not exist
		return FALSE;
	}	
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	uValue;
	return FALSE;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
void RTopLevelConfiguration::SetLongValue(const RMBCString& rItem, uLONG uValue)
{	
#ifdef	_WINDOWS
	DWORD uType = REG_DWORD;	
	DWORD uDataSize = sizeof(uLONG);	
	if (::RegSetValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, uType, reinterpret_cast<const BYTE*>(&uValue), uDataSize) != ERROR_SUCCESS) throw ::kRegistryError;
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	uValue;	
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
BOOLEAN RTopLevelConfiguration::GetBinaryValue(const RMBCString& rItem, void* pData, uLONG& uDataLength) const
{
#ifdef	_WINDOWS
	if (GetDataLength(rItem, uDataLength))
	{
		DWORD uDataType;		
		if (::RegQueryValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, &uDataType, reinterpret_cast<BYTE*>(pData), &uDataLength) != ERROR_SUCCESS) throw ::kRegistryError;
		TpsAssert(uDataType == REG_BINARY, "Expected and actual registry data types differ.");
		return TRUE;
	}
	else
	{
		// The value does not exist
		return FALSE;
	}
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	pData;
	uDataLength;
	return FALSE;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
void RTopLevelConfiguration::SetBinaryValue(const RMBCString& rItem, const void* pData, uLONG uDataLength)
{
#ifdef	_WINDOWS
	DWORD uType = REG_BINARY;		
	if (::RegSetValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, uType, reinterpret_cast<const BYTE*>(pData), uDataLength) != ERROR_SUCCESS) throw ::kRegistryError;
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
	pData;
	uDataLength;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
void RTopLevelConfiguration::DeleteValue(const RMBCString& rItem)
{
#ifdef	_WINDOWS
	LONG sResult = ::RegDeleteValue(GetKey(), static_cast<LPCTSTR>(rItem));
	if (sResult == ERROR_SUCCESS || sResult == REGDB_E_KEYMISSING || sResult == ERROR_FILE_NOT_FOUND) 
		return;
	else
		throw ::kRegistryError;
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
#endif	// MAC
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::()
//	Description:	
//
BOOLEAN RTopLevelConfiguration::GetDataLength(const RMBCString& rItem, uLONG& uDataLength) const
{
#ifdef	_WINDOWS
	uDataLength = 0;
	DWORD uDataType;
	return static_cast<BOOLEAN>(::RegQueryValueEx(GetKey(), static_cast<LPCTSTR>(rItem), 0, &uDataType, NULL, &uDataLength) == ERROR_SUCCESS);
#endif	// _WINDOWS
#ifdef	MAC
	rItem;
#endif	// MAC
}

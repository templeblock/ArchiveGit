////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File name:		ConfigurationBase.h
//	Project:			Renaissance framework
//	Author:			G. Brown
//	Description:	Declaration of base configuration classes
//	Portability:	Platform-independent
//	Developer:		Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//	Client:			Broderbund Software, Inc.
//
//	Copyright (c) 1997 Turning Point Software.  All Rights Reserved.
//

#ifndef	_CONFIGURATIONBASE_H_
#define	_CONFIGURATIONBASE_H_

#ifdef	_WINDOWS
#include <winreg.h>
#endif	// _WINDOWS

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				CTopLevelConfiguration
//	Description:	Base class to encapsulate registry manipulation
//
class FrameworkLinkage RTopLevelConfiguration
{
// Initialization
protected:
#ifdef	_WINDOWS
								RTopLevelConfiguration(HKEY hTopLevelKey);
#endif	// _WINDOWS
#ifdef	MAC
								RTopLevelConfiguration();
#endif	// MAC
								
// Operations
public:
	BOOLEAN					GetStringValue(const RMBCString& rItem, RMBCString& rValue) const;
	BOOLEAN					GetLongValue(const RMBCString& rItem, uLONG& uValue) const;
	BOOLEAN					GetBinaryValue(const RMBCString& rItem, void* pData, uLONG& uDataLength) const;
	BOOLEAN					GetDataLength(const RMBCString& rItem, uLONG& uDataLength) const;
	
	void						SetStringValue(const RMBCString& rItem, const RMBCString& rValue);
	void						SetLongValue(const RMBCString& rItem, uLONG uValue);
	void						SetBinaryValue(const RMBCString& rItem, const void* pData, uLONG uDataLength);
	
	void						DeleteValue(const RMBCString& rItem);

#ifdef	_WINDOWS
// Accessors
public:
	virtual const RMBCString&	GetPath() const;
protected:
	virtual HKEY			GetKey() const;
#endif	// _WINDOWS

// Data members
#ifdef	_WINDOWS
private:
	RMBCString				m_topLevelPath;
	const HKEY				m_hTopLevelKey;
#endif	// _WINDOWS
};


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				RTopLevelConfiguration::GetPath()
//	Description:	Returns m_hKey
//
inline const RMBCString& RTopLevelConfiguration::GetPath() const 
{
	return m_topLevelPath;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				CUserConfiguration
//	Description:	Class to encapsulate HKEY_CURRENT_USER registry entries
//
class FrameworkLinkage RUserConfiguration : public RTopLevelConfiguration
{
#ifdef	_WINDOWS
protected:
	RUserConfiguration() : RTopLevelConfiguration(HKEY_CURRENT_USER) { }
#endif	// _WINDOWS
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				CSystemConfiguration
//	Description:	Class to encapsulate HKEY_LOCAL_MACHINE registry entries
//
class FrameworkLinkage RSystemConfiguration : public RTopLevelConfiguration
{
#ifdef	_WINDOWS
protected:
	RSystemConfiguration() : RTopLevelConfiguration(HKEY_LOCAL_MACHINE) { }
#endif	// _WINDOWS
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				CClassConfiguration
//	Description:	Class to encapsulate HKEY_CLASSES_ROOT registry entries
//
class FrameworkLinkage RClassConfiguration : public RTopLevelConfiguration
{
#ifdef	_WINDOWS
protected:
	RClassConfiguration() : RTopLevelConfiguration(HKEY_CLASSES_ROOT) { }
#endif	// _WINDOWS
};

#ifdef	_WINDOWS
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				template <class T> TConfiguration
//	Description:	Template class to facilitate arrangement of registry encapsulation
//						classes
//
template <class T>
class TConfiguration : public T
{
// Initializations
protected:
								TConfiguration(const RMBCString& rPath);
	virtual					~TConfiguration();

// Data persistency
public:
//	void						LoadData();
//	void						StoreData();

protected:
//	void						AssociateMember(const RMBCString& rItem, uLONG& uLongMember);
//	void						AssociateMember(const RMBCString& rItem, RMBCString& rStringMember);
//	void						AssociateMember(const RMBCString& rItem, void* pDataMember, uLONG& uDataLengthMember);

// Accessors
public:
	virtual const RMBCString&	GetPath() const;

// Operations
protected:
	virtual HKEY			GetKey() const;

// Data members
private:
	RMBCString				m_path;
	HKEY						m_hKey;

//	struct SBinaryData
//	{
//		void*		m_pDataMember;
//		uLONG*	m_pDataLengthMember;
//	};
//
//	map<RMBCString, uLONG*>			m_mTagToLong;
//	map<RMBCString, RMBCString*>	m_mTagToString;
//	map<RMBCString, SBinaryData>	m_mTagToBinaryData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				TConfiguration::TConfiguration()
//	Description:	Constructor
//
template <class T>
inline TConfiguration<T>::TConfiguration(const RMBCString& rPath)
	: m_path( T::GetPath() + rPath )
{
	// Create/open the registry key
	DWORD uDisposition;
	if (::RegCreateKeyEx(T::GetKey(),
		static_cast<LPCTSTR>(rPath),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&m_hKey,
		&uDisposition) != ERROR_SUCCESS) throw kRegistryError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				TConfiguration::~TConfiguration()
//	Description:	Destructor
//
template <class T>
inline TConfiguration<T>::~TConfiguration()
{
	// Close the registry key
	::RegCloseKey(m_hKey);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				TConfiguration::GetKey()
//	Description:	Returns m_hKey
//
template <class T>
inline HKEY TConfiguration<T>::GetKey() const 
{
	return m_hKey;
}
#endif	// _WINDOWS

#ifdef	MAC
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				template <class T> TConfiguration
//	Description:	Template class to facilitate arrangement of registry encapsulation
//						classes
//
template <class T>
class TConfiguration : public T
{
};
#endif	// MAC

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Name:				TConfiguration::GetPath()
//	Description:	Returns m_hKey
//
template <class T>
inline const RMBCString& TConfiguration<T>::GetPath() const 
{
	return m_path;
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _CONFIGURATIONBASE_H_

// ****************************************************************************
//
//  File Name:			WinPreferences.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Definition of the RPreferences class
//
//  Portability:		Windows-specific
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef MAC

#include "Preferences.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RPreferences::RPreferences()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RPreferences::RPreferences(ETopLevelPreference, 
									YPreferenceTag& rPreferenceHeading)
{
}

// ****************************************************************************
//
//  Function Name:	RPreferences::~RPreferences()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RPreferences::~RPreferences()
{
}

// ****************************************************************************
//
//  Function Name:	RPreferences::AddValue()
//
//  Description:		Adds a value into the registry under the currently open key
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::AddValue(const YPreferenceTag&, //rValueName, 
										 const uLONG&) //uLongValue)											 
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::AddValue()
//
//  Description:		Adds a value into the registry under the currently open key
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::AddValue(const YPreferenceTag&, //rValueName, 
										 const WORD&) //uWordValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::AddValue()
//
//  Description:		Adds a value into the registry under the currently open key
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::AddValue(const YPreferenceTag&, //rValueName, 
										 const uBYTE&) //uByteValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::AddValue()
//
//  Description:		Adds a value into the registry under the currently open key
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::AddValue(const YPreferenceTag&, //rValueName, 
										 const RMBCString&) //rStringValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::AddValue()
//
//  Description:		Adds a value into the registry under the currently open key
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::AddValue(const YPreferenceTag&, //rValueName, 
										 void*, //pBinaryValue,
										 const uLONG&) //uBinaryValueLength)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::SetValue()
//
//  Description:		Sets a value in the registry under the currently open key,
//							if the named value exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::SetValue(const YPreferenceTag&, //rValueName,
										 const uLONG&) //uLongValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::SetValue()
//
//  Description:		Sets a value in the registry under the currently open key,
//							if the named value exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::SetValue(const YPreferenceTag&, //rValueName, 
										 const uWORD&) //uWordValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::SetValue()
//
//  Description:		Sets a value in the registry under the currently open key,
//							if the named value exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::SetValue(const YPreferenceTag&, //rValueName, 
										 const uBYTE&) //uByteValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::SetValue()
//
//  Description:		Sets a value in the registry under the currently open key,
//							if the named value exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::SetValue(const YPreferenceTag&, //rValueName, 
										 const RMBCString&) //rStringValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::SetValue()
//
//  Description:		Sets a value in the registry under the currently open key,
//							if the named value exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::SetValue(const YPreferenceTag&, //rValueName, 
										 void*, //pBinaryValue,
										 const uLONG&) //uBinaryValueLength)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::GetValue()
//
//  Description:		Gets the requested value ffrom the registry, if it exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::GetValue(const YPreferenceTag&, //rValueName,
										 uLONG&) //uLongValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::GetValue()
//
//  Description:		Gets the requested value ffrom the registry, if it exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::GetValue(const YPreferenceTag&, //rValueName,
										 uWORD&) //uWordValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::GetValue()
//
//  Description:		Gets the requested value ffrom the registry, if it exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::GetValue(const YPreferenceTag&, //rValueName,
										 uBYTE&) //uByteValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::GetValue()
//
//  Description:		Gets the requested value ffrom the registry, if it exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::GetValue(const YPreferenceTag&, //rValueName,
										 RMBCString&) //rStringValue)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::GetValue()
//
//  Description:		Gets the requested value ffrom the registry, if it exists
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::GetValue(const YPreferenceTag&, //rValueName,
										 void*, //pBinaryValue,
										 const uLONG&) //uBinaryValueLength)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPreferences::DeleteValue()
//
//  Description:		Deletes the requested value from the registry
//
//  Returns:			TRUE or FALSE
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPreferences::DeleteValue(const YPreferenceTag&) //rValueName)
{
	return FALSE;
}

#endif //MAC
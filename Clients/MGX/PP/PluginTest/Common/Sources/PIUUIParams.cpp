//-------------------------------------------------------------------------------
//
//	File:
//		PIUUIParams.cpp
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the source and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//		Most expect on Windows, for hDllInstance to
//		be a global handle reference to your plug-in DLL.
//
//	Version history:
//		Version 1.0.0	7/31/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUUIParams.h"

//-------------------------------------------------------------------------------
//	User interface parameters.
//-------------------------------------------------------------------------------
PIUUIParams_t::
	PIUUIParams_t
		(
		SPPluginRef plugInRef = NULL
		) :
		plugInRef_(plugInRef),
		hDLLInstance_(NULL)
	{
	// Intentionally left blank.
	}

PIUUIParams_t::PIUUIParams_t() :
	plugInRef_(NULL)
	{
	// Intentionally left blank.
	}
	
PIUUIParams_t::
	PIUUIParams_t
	(
	const PIUUIParams_t & source
	) :
	plugInRef_(source.plugInRef_),
	hDLLInstance_(source.hDLLInstance_)
	{
	// Intentionally left blank.
	}

PIUUIParams_t & PIUUIParams_t::operator=
	(
	const PIUUIParams_t & source
	)
	{
	plugInRef_ = source.plugInRef_;
	hDLLInstance_ = source.hDLLInstance_;
	return *this;
	}

bool PIUUIParams_t::ParamsValid()
	{
	return (plugInRef_ != NULL);
	}

void PIUUIParams_t::SetPlugInRef
	(
	SPPluginRef plugInRef
	)
	{
	plugInRef_ = plugInRef;
	}
	
SPPluginRef PIUUIParams_t::GetPlugInRef(void) const
	{
	return plugInRef_;
	}


void PIUUIParams_t::SetDLLInstance
	(
	void* hDLLInstance
	)
	{
	hDLLInstance_ = hDLLInstance;
	}
	
void* PIUUIParams_t::GetDLLInstance(void) const
	{
	return hDLLInstance_;
	}

//-------------------------------------------------------------------------------

// end PIUUIParams.cpp

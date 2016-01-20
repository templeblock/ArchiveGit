//-------------------------------------------------------------------------------
//
//	File:
//		PIUUIParams.h
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the headers for the macros, and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//		Most expect A4-globals to be set-up already, for
//		gStuff to be a valid pointer to your global structure's
//		parameter block, and, on Windows, for hDllInstance to
//		be a global handle reference to your plug-in DLL.
//
//	Version history:
//		Version 1.0.0	7/31/1997	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#ifndef __PIUUIParams_h__
#define __PIUUIParams_h__

#include "SPPlugs.h"

//-------------------------------------------------------------------------------
//	User interface parameters.
//-------------------------------------------------------------------------------
class PIUUIParams_t
	{
	public:
		// Constructor:
		PIUUIParams_t
			(
			SPPluginRef plugInRef
			);
			
		PIUUIParams_t();
		
		// Copy constructor:
		PIUUIParams_t
			(
			const PIUUIParams_t & source
			);
		
		// Assignment operator:
		PIUUIParams_t & operator= 
			(
			const PIUUIParams_t & source
			);
			
		// Destructor:
		~PIUUIParams_t() {};	// Specified as nothing.
		
		// Check for validity of resource before using:
		bool ParamsValid();
		
		// Change the parameter block:
		void SetPlugInRef
			(
			SPPluginRef plugInRef
			);
			
		// Return the plug-in ref:
		SPPluginRef GetPlugInRef(void) const;
		
		// Install the DLL instance:
		void SetDLLInstance
			(
			void*	hDLLInstance
			);
		
		// Return the DLL instance:
		void* GetDLLInstance(void) const;

	private:
		// None.
	
	protected:
		SPPluginRef plugInRef_;
		void*		hDLLInstance_;
	};

//-------------------------------------------------------------------------------

#endif // __PIUUIParams_h__

//-------------------------------------------------------------------------------
//
//	File:
//		PIUActionParams.h
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

#ifndef __PIUActionParams_h__
#define __PIUActionParams_h__

#include "PIActions.h"

//-------------------------------------------------------------------------------
//	Definitions.
//-------------------------------------------------------------------------------

// This is defined in <AEObjects.h> but was accidentally not included in
// PIActions.h:
#ifndef typeObjectSpecifier
#define typeObjectSpecifier		0x6f626a20	// 'obj '
#endif

//-------------------------------------------------------------------------------
//	Action parameters references.
//-------------------------------------------------------------------------------
class PIUActionParams_t
	{
	public:
		// Constructor:
		PIUActionParams_t
			(
			PIActionParameters * actionsParams
			);
			
		PIUActionParams_t();
		
		// Copy constructor:
		PIUActionParams_t
			(
			const PIUActionParams_t & source
			);
		
		// Assignment operator:
		PIUActionParams_t & operator= 
			(
			const PIUActionParams_t & source
			);
			
		// Destructor:
		~PIUActionParams_t ();
		
		// Check for validity of resource before using:
		bool ParamsValid();
		
		// Change the parameter block:
		void SetParamBlock
			(
			PIActionParameters * actionParams
			);
		
		// Clear out the parameter descriptor:
		SPErr ClearParamDescriptor();
		
		// Fill out param block with return descriptor:
		void SetReturnInfo
			(
			PIActionDescriptor descriptor,
			PIDialogRecordOptions recordInfo
			);
		
		void SetReturnInfo
			(
			PIActionDescriptor descriptor
			); // Sets recordInfo = dialogOptional.
			
		// Return the playback descriptor and info:
		SPErr GetPlayInfo
			(
			PIActionDescriptor * descriptor,
			PIDialogPlayOptions * playInfo
			);
		
		// Simpler methods for making and freeing a descriptor:
		PIActionDescriptor Make(void);
		void Free(PIActionDescriptor* descriptor);
		
		// Read a string from a descriptor and return it as a block
		// that you must free when you're done:
		char* GetString
			(
			PIActionDescriptor descriptor,
			DescriptorKeyID key
			);
					
	private:
		// None.
	
	protected:
		PIActionParameters * actionParams_;
	};

//-------------------------------------------------------------------------------
//	Errors.
//-------------------------------------------------------------------------------
#define kPSErrorUnknownStringID	'!sid'

//-------------------------------------------------------------------------------

#endif // __PIUActionParams_h__

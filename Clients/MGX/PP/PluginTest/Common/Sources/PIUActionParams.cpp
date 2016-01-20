//-------------------------------------------------------------------------------
//
//	File:
//		PIUActionParams.cpp
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

#include "PIUActionParams.h"
#include "PIUSuites.h" // Include here to prevent circular errors.

//-------------------------------------------------------------------------------
//	Action suite references.
//-------------------------------------------------------------------------------
PIUActionParams_t::
	PIUActionParams_t
		(
		PIActionParameters * actionsParams = NULL
		) :
		actionParams_(actionsParams)
	{
	// Probably will break PIDescriptorParams out into
	// parts, or make accessor functions.
	}

PIUActionParams_t::PIUActionParams_t() :
	actionParams_(NULL)
	{
	// Intentionally left blank.
	}
	
PIUActionParams_t::
	PIUActionParams_t
	(
	const PIUActionParams_t & source
	) :
	actionParams_(source.actionParams_)
	{
	// Intentionally left blank.
	}

PIUActionParams_t & PIUActionParams_t::operator=
	(
	const PIUActionParams_t & source
	)
	{
	actionParams_ = source.actionParams_;
	return *this;
	}
	
PIUActionParams_t::~PIUActionParams_t()
	{
	// If there are any things that could be left dangling
	// in the descriptor parameter block (besides a return
	// descriptor) then clean them up here.
	}

bool PIUActionParams_t::ParamsValid()
	{
	return (actionParams_ != NULL);
	}

void PIUActionParams_t::SetParamBlock
	(
	PIActionParameters * actionParams
	)
	{
	// If we're blasting over something, make sure
	// if its valid we destroy the descriptor:
	if (actionParams_ != NULL)
		ClearParamDescriptor();
		
	actionParams_ = actionParams;
	}
	
SPErr PIUActionParams_t::ClearParamDescriptor()
	{
	SPErr error = noErr;
	
	if (ParamsValid())
		{
		if (sPSActionDescriptor.IsValid() &&
			actionParams_->descriptor != NULL)
			{
			sPSActionDescriptor->Free
				(
				actionParams_->descriptor
				);
			actionParams_->descriptor = NULL;
			}
		else
			error = kSPSuiteNotFoundError;
		}
	
	return error;

	}

void PIUActionParams_t::SetReturnInfo
	(
	PIActionDescriptor descriptor = NULL,
	PIDialogRecordOptions recordInfo = plugInDialogOptional
	)
	{
	if (ParamsValid())
		{
		ClearParamDescriptor();
		actionParams_->descriptor = descriptor;
		actionParams_->recordInfo = recordInfo;
		}
	}

void PIUActionParams_t::SetReturnInfo
	(
	PIActionDescriptor descriptor = NULL
	)
	{
	SetReturnInfo(descriptor, plugInDialogOptional);
	}
	
SPErr PIUActionParams_t::GetPlayInfo
	(
	PIActionDescriptor* descriptor,
	PIDialogPlayOptions* playInfo
	)
	{
	SPErr error = noErr;
	if (ParamsValid())
		{
		if (descriptor != NULL)
			*descriptor = actionParams_->descriptor;
		
		if (playInfo != NULL)
			*playInfo = actionParams_->playInfo;
		}
	else
		{
		error = kSPBadParameterError;
		}
	
	return error;
	}
	
//-------------------------------------------------------------------------------
//
//	Make/Free
//
//	This provides simpler Make and Free methods for descriptors.
//
//-------------------------------------------------------------------------------
PIActionDescriptor PIUActionParams_t::Make(void)
	{
	PIActionDescriptor descriptor = NULL;
	
	if (sPSActionDescriptor.IsValid())
		{
		SPErr error = sPSActionDescriptor->Make(&descriptor);
		}
		
	return descriptor;
	}
	
void PIUActionParams_t::Free(PIActionDescriptor* descriptor)
	{
	if (sPSActionDescriptor.IsValid() && descriptor != NULL && *descriptor != NULL)
		{
		SPErr error = sPSActionDescriptor->Free(*descriptor);
		*descriptor = NULL;
		}
	}

//-------------------------------------------------------------------------------
//
//	GetString
//
//	GetString is attached the sPSActionDescriptor and allocates a block on the
//	heap to receive a string from a descriptor.  Use sSPBasic->FreeBlock() to
//	release it when you're done.
//
//-------------------------------------------------------------------------------
char* PIUActionParams_t::GetString
	(
	PIActionDescriptor descriptor,
	DescriptorKeyID key
	)
	{
	char* returnString = NULL;
			
	if 
		(
		sSPBasic.IsValid() && 
		sPSActionDescriptor.IsValid() && 
		descriptor != 0
		)
		{
		uint32 stringLength = 0;
		
		SPErr error = sPSActionDescriptor->GetStringLength
			(
			descriptor,
			key,
			&stringLength
			);
			
		if (error == kSPNoError)
			{
			// Found the key.  Continue.
			// Current implimentation is if you don't have a string
			// of at least stringLength size, asking for anything
			// smaller will result in no string, so we'll allocate
			// a buffer to receive the entire string:
			error = sSPBasic->AllocateBlock(stringLength+1, (void**)&returnString);
			
			if (error == kSPNoError && returnString != NULL)
				{
				error = sPSActionDescriptor->GetString
					(
					descriptor,
					key,
					returnString,
					stringLength+1
					);
					
				// Just in case, null terminate:
				returnString[stringLength] = '\0';
				}
		
			} // no key

		} // sSPBasic
			
	return returnString;
	}
						

//-------------------------------------------------------------------------------
// end PIUActionParams.cpp

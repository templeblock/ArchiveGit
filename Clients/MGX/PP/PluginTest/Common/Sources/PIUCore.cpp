//-------------------------------------------------------------------------------
//
//	File:
//		PIUCore.cpp
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains utility routines for common
//		functions related to the scripting system.
//
//	Use:
//		You can use this code as documentation for how
//		to script specific functions in Photoshop or you
//		may call the higher-level routines directly.
//		If you need specifics about what a function's
//		parameters do, refer to the Photoshop user guide
//		for that actual feature.
//
//	Version history:
//		1.0.0	4/3/1998	Ace		Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUCore.h"
#include "PIUFile.h" // For ResolveFileSpec, ClearFileSpec.

//-------------------------------------------------------------------------------
//	Globals.
//-------------------------------------------------------------------------------

// sPIUBasic is a global pointer to the Actions Abstraction Layer utilities.  It's set up
// so you can use it just like the suite pointers:
PIUBasic sPIUBasic;

//-------------------------------------------------------------------------------
//	PIUResult UUID for registry entries of last descriptor and event.
//-------------------------------------------------------------------------------
static const char* const kResultDescriptorID = "fb39c080-cc49-11d1-bc44-0060b0a13dc4";

//-------------------------------------------------------------------------------
//
//	PIUResult::Get and SetDescriptor.
//
//	We store a copy of the descriptor in the registry so it will be around
//	between invocations of the plug-in (descriptors are tracked
//	resources and are automatically deallocated when the plug-in
//	execute routine is finished.)
//
//-------------------------------------------------------------------------------
PIActionDescriptor PIUResult::GetDescriptor(void) const
	{
	PIActionDescriptor descriptor = NULL;
	
	if (sPSDescriptorRegistry.IsValid() && sPSActionDescriptor.IsValid())
		{
		PIActionDescriptor registryDescriptor = NULL;
		
		SPErr error = sPSDescriptorRegistry->Get
			(
			kResultDescriptorID,
			&registryDescriptor
			);
			
		if (registryDescriptor != NULL && error == kSPNoError)
			{
			DescriptorClassID classID = NULL;
			
			error = sPSActionDescriptor->GetObject
				(
				registryDescriptor,
				keySource,
				&classID,
				&descriptor
				);
			}
		}
		
	// Descriptors are opaque ids so we can return it from the stack:
	return descriptor;
	}		
		
void PIUResult::SetDescriptor
	(
	const PIActionDescriptor descriptor
	)
	{
	// We store the descriptor in the registry so it will be around
	// between invocations of the plug-in.  Descriptors are tracked
	// resources and are automatically deallocated when the plug-in
	// execute routine is finished.
	if 
		(
		sPSDescriptorRegistry.IsValid() && 
		sPSActionDescriptor.IsValid() &&
		sSPBasic.IsValid()
		)
		{
		char* lastID = GetID();
		
		SPErr error = sPSDescriptorRegistry->Erase(kResultDescriptorID);
		
		// If there was a key there, it was erased.  Otherwise we got
		// an error.  (keyNotFound.)
		
		PIActionDescriptor registryDescriptor = sPSActionDescriptor.Make();
				
		if (error == kSPNoError && registryDescriptor != NULL)
			{
			if (lastID != NULL)
				{
				error = sPSActionDescriptor->PutString
					(
					registryDescriptor,
					keyUsing,
					lastID
					);
				
				sSPBasic->FreeBlock(lastID);
				lastID = NULL;
				}
			
			if (descriptor != NULL)
				{
				error = sPSActionDescriptor->PutObject
					(
					registryDescriptor,
					keySource,
					classNull,
					descriptor
					);
				}
				
			// Now store our descriptor:
			error = sPSDescriptorRegistry->Register
				(
				kResultDescriptorID,
				registryDescriptor,
				false			// Not persistent.
				);

			sPSActionDescriptor.Free(&registryDescriptor);
			
			} // error && registryDescriptor
		
			
		// That just made a copy of the descriptor so make sure we
		// Free it in our calling routine!
		}
	else
		{
		// Can't store descriptor because we can't get to the
		// registry suite.
		}
	}
	
//-------------------------------------------------------------------------------
//
//	PIUResult::Get and SetID.
//
//	We store a copy of the descriptor and event in the registry so it will be around
//	between invocations of the plug-in (descriptors are tracked
//	resources and are automatically deallocated when the plug-in
//	execute routine is finished.  Since we're storing the descriptor
//	here, it makes sense to store the event here, too.)
//
//-------------------------------------------------------------------------------
char* PIUResult::GetID(void) const
	{
	char* returnEvent = NULL;
	
	if (sPSDescriptorRegistry.IsValid() && sPSActionDescriptor.IsValid())
		{
		PIActionDescriptor registryDescriptor = NULL;
		
		SPErr error = sPSDescriptorRegistry->Get
			(
			kResultDescriptorID,
			&registryDescriptor
			);
			
		if (registryDescriptor != NULL && error == kSPNoError)
			{
			returnEvent = sPSActionDescriptor.GetString(registryDescriptor, keyUsing);

			sPSActionDescriptor.Free(&registryDescriptor);
			}

		}
		
		// If error there was no key in the registry.
		
	// This is allocated for you, so release it when you're done with FreeBlock:
	return returnEvent;
	}		
		
void PIUResult::SetID
	(
	char* id
	)
	{
	// We store the descriptor in the registry so it will be around
	// between invocations of the plug-in.  Descriptors are tracked
	// resources and are automatically deallocated when the plug-in
	// execute routine is finished.
	if (sPSDescriptorRegistry.IsValid() && sPSActionDescriptor.IsValid())
		{
		PIActionDescriptor lastDescriptor = GetDescriptor();
		
		SPErr error = sPSDescriptorRegistry->Erase(kResultDescriptorID);
		
		// If there was a key there, it was erased.  Otherwise we got
		// an error.  (keyNotFound.)
		
		PIActionDescriptor registryDescriptor = sPSActionDescriptor.Make();
		
		if (error == kSPNoError && registryDescriptor != NULL)
			{
			if (id != NULL)
				{
				error = sPSActionDescriptor->PutString
					(
					registryDescriptor,
					keyUsing,
					id
					);
				}
			
			if (lastDescriptor != NULL)
				{
				error = sPSActionDescriptor->PutObject
					(
					registryDescriptor,
					keySource,
					classNull,
					lastDescriptor
					);
				
				sPSActionDescriptor.Free(&lastDescriptor);
				}
				
			// Now store our descriptor:
			error = sPSDescriptorRegistry->Register
				(
				kResultDescriptorID,
				registryDescriptor,
				false			// Not persistent.
				);

			sPSActionDescriptor.Free(&registryDescriptor);
			
			} // error && registryDescriptor
		
			
		// That just made a copy of the descriptor so make sure we
		// Free it in our calling routine!
		}
	else
		{
		// Can't store descriptor because we can't get to the
		// registry suite.
		}
	}

// Can also set the event via 4-character code:
void PIUResult::SetID
	(
	const DescriptorTypeID id
	) 
	{
	char convertString[kConvertStrLen] = "";
	
	PIUIDToChar(id, convertString);
	
	// Call the string version of SetID:
	SetID(convertString);
	}
	
//-------------------------------------------------------------------------------
//
//	PIUResult::DisplayMessage
//
//	If there is a message in the result descriptor, will display it in an
//	ADM message box.
//
//	Returns true if it displayed a message.
//
//-------------------------------------------------------------------------------
bool PIUResult::DisplayMessage(void)
	{
	bool displayedMessage = false;
	
	// If we don't have the suites we need, don't bother:
	if (sPSActionDescriptor.IsValid() && 
		sPSDescriptorRegistry.IsValid() &&
		sADMBasic.IsValid() &&
		sSPBasic.IsValid())
		{
		// We're getting a copy of a descriptor, so its our
		// job to free it once we're done:
		PIActionDescriptor descriptor = GetDescriptor();
		
		if (descriptor != NULL)
			{
			Boolean hasKey = false;
			
			SPErr error = sPSActionDescriptor->HasKey
				(
				descriptor,
				keyMessage,		// Errors are reported in keyMessage.
				&hasKey
				);
				
			if (error == kSPNoError && hasKey)
				{
				// Lets call the utility routine to allocate a
				// buffer and copy the string from the descriptor
				// into it.  It's our responsiblity to free the
				// string block when we're done with it:
				char* displayString = sPSActionDescriptor.GetString
					(
					descriptor,
					keyMessage
					);
					
				if (displayString != NULL)
					{
					sADMBasic->MessageAlert(displayString);
					
					displayedMessage = true;
					 
					sSPBasic->FreeBlock(displayString);
					displayString = NULL;
					}
				}
			
			sPSActionDescriptor.Free(&descriptor);
			
			} // descriptor
		
		} // IsValid
	
		return displayedMessage;
	
	} // end DisplayIfMessage
		
//-------------------------------------------------------------------------------
//
//	PIUResult::DisplayDescriptor
//
//	Goes through each property in the result descriptor and displays it and its
//	type, if simple.
//
//-------------------------------------------------------------------------------
void PIUResult::DisplayDescriptor(void)
	{

	// You probably don't want the overhead of this routine in your
	// non-debug code, so I'll define this and its strings only for
	// a debug build:
	#ifdef _DEBUG

	// If we don't have the suites we need, don't bother:
	if
		(
		sPSActionDescriptor.IsValid() &&
		sPSDescriptorRegistry.IsValid() &&
		sADMBasic.IsValid() &&
		sSPBasic.IsValid()
		)
		{
		// We're getting a copy of this descriptor so its
		// our responsiblity to free it when we're done with it:
		PIActionDescriptor descriptor = GetDescriptor();
		
		if (descriptor != NULL)
			{
			const char* kDisplay = "Key ^0 of ^1: \'^2\', type \'^3\' = ^4";
			const unsigned short kDisplay_size = PIUstrlen(kDisplay);
			
			uint32 total = 0;
			
			SPErr error = sPSActionDescriptor->GetCount
				(
				descriptor,
				&total
				);
			
			// This routine will copy a string.  Its our responsibility
			// to free the result:
			char* displayWithTotal = PIUCopyStringToBlock
				(
				kDisplay,
				kDisplay_size
				);
				
			if (displayWithTotal != NULL)
				{
				// Need a small string for conversion:
				char convertString[kConvertStrLen] = "";
				
				// Convert total to a displayable string:
				sADMBasic->ValueToString
					(
					(float)total,
					convertString,
					kConvertStrLen,
					kADMNoUnits,
					kPIUNoPrecision,
					false				// Always append units?
					);					
				
				error = PIUReplaceCharsInBlock
					(
					"^1",
					convertString,
					kIgnoreCase,
					&displayWithTotal
					);
					
				const unsigned short kDisplayWithTotal_size = PIUstrlen(displayWithTotal);
				
				for (uint32 loop = 0; loop < total; loop++)
					{
					char* displayAll = PIUCopyStringToBlock
						(
						displayWithTotal,
						kDisplayWithTotal_size
						);
						
					if (displayAll != NULL)
						{
						sADMBasic->ValueToString
							(
							(float)loop + 1,	// Looks silly 0...N.
							convertString,
							kConvertStrLen,
							kADMNoUnits,
							kPIUNoPrecision,
							false
							);
									
						error = PIUReplaceCharsInBlock
							(
							"^0",
							convertString,
							kIgnoreCase,
							&displayAll
							);

						DescriptorKeyID key = keyNull;
						
						error = sPSActionDescriptor->GetKey
							(
							descriptor,
							loop,
							&key
							);	
				
						PIUIDToChar(key, convertString);
						
						error = PIUReplaceCharsInBlock
							(
							"^2",
							convertString,
							kIgnoreCase,
							&displayAll
							);
							
						DescriptorTypeID type = typeNull;
						
						error = sPSActionDescriptor->GetType
							(
							descriptor,
							key,
							&type
							);
									
						PIUIDToChar(type, convertString);
						
						error = PIUReplaceCharsInBlock
							(
							"^3",
							convertString,
							kIgnoreCase,
							&displayAll
							);
							
						switch(type)
							{
							case typeInteger:
								{
								int32 value = 0;
								
								error = sPSActionDescriptor->GetInteger
									(
									descriptor,
									key,
									&value
									);
									
								sADMBasic->ValueToString
									(
									(float)value,
									convertString,
									kConvertStrLen,
									kADMNoUnits,
									kPIUNoPrecision,
									false			// Append units?
									);
								break;
								}
							case typeUnitFloat:
								{
								double value = 0.0;
								
								DescriptorUnitID unit = unitNone;
								
								error = sPSActionDescriptor->GetUnitFloat
									(
									descriptor,
									key,
									&unit,
									&value
									);
								
								ADMUnits admUnits = kADMNoUnits;
								
								switch(unit)
									{
									case unitAngle: admUnits = kADMDegreeUnits;	break;
									case unitDensity: admUnits = kADMInchUnits; break;
									case unitDistance: admUnits = kADMPointUnits; break;
									case unitNone: admUnits = kADMNoUnits; break;
									case unitPercent: admUnits = kADMPercentUnits; break;
									case unitPixels: admUnits = kADMPointUnits; break;
									}
								
								sADMBasic->ValueToString
									(
									(float)value,
									convertString,
									kConvertStrLen,
									admUnits,
									kPIUQuadPrecision,
									true		// Append units?
									);
								break;
								}
							case typeFloat:
								{
								double value = 0.0;
								
								error = sPSActionDescriptor->GetFloat
									(
									descriptor,
									key,
									&value
									);
									
								sADMBasic->ValueToString
									(
									(float)value,
									convertString,
									kConvertStrLen,
									kADMNoUnits,
									kPIUNoPrecision,
									false			// Append units?
									);
								break;
								}
							case typeBoolean:
								{
								Boolean value = FALSE;
								
								error = sPSActionDescriptor->GetBoolean
									(
									descriptor,
									key,
									&value
									);
									
								convertString[0] = '0';
								
								if (value == TRUE)
									convertString[0] = '1';
									
								convertString[1] = '\0';
								
								break;
								}
							case typeText:
								{
								// Only going to display the first kConvertStrLen
								// (33) chars of the string.  We'll have to truncate
								// ourselves:
								char* temp = sPSActionDescriptor.GetString
									(
									descriptor,
									key
									);
									
								if (temp != NULL)
									{
									unsigned short length = PIUstrlen(temp);
									if (length >= kConvertStrLen)
										length = kConvertStrLen-1;
										
									PIUCopy
										(
										convertString,
										temp,
										length
										);
									
									// Unless PIUCopy copies length+1 (which
									// would include the null) we need to
									// do it ourselves.  Since the string may
									// be over our desired size, the last
									// character may not be a null, so we
									// do it ourselves:
									convertString[length] = '\0'; // Null terminate.
										
									sSPBasic->FreeBlock(temp);
									temp = NULL;
									}
								break;
								}
							case typeClass:
								{
								DescriptorClassID classID = classNull;
								
								error = sPSActionDescriptor->GetClass
									(
									descriptor,
									key,
									&classID
									);
									
								PIUIDToChar(classID, convertString);
								
								break;
								}
							case typeEnumerated:
								{
								DescriptorEnumTypeID typeID = typeNull;
								DescriptorEnumID enumID = enumNull;
								
								error = sPSActionDescriptor->GetEnumerated
									(
									descriptor,
									key,
									&typeID,
									&enumID
									);
									
								// Make sure this display string is less than 33 chars:
								const char* display = "enum type \'^0\', value \'^1\'";
								const unsigned short display_size = PIUstrlen(display);
								
								// Copy string plus the trailing null:
								char* enumDisplay = PIUCopyStringToBlock
									(
									display,
									display_size
									);
								
								char tempConvert[kConvertStrLen] = "";
								PIUIDToChar(typeID, tempConvert);
								
								error = PIUReplaceCharsInBlock
									(
									"^0",
									tempConvert,
									kIgnoreCase,
									&enumDisplay	// Must be block that can be replaced.
									);
									
								PIUIDToChar(enumID, tempConvert);
								
								error = PIUReplaceCharsInBlock
									(
									"^1",
									tempConvert,
									kIgnoreCase,
									&enumDisplay	// Must be block that can be replaced.
									);
									
								unsigned short enumDisplay_size = PIUstrlen(enumDisplay);
								
								if (enumDisplay_size >= kConvertStrLen)
									enumDisplay_size = kConvertStrLen-1;
									
								// Now copy to convertString:
								PIUCopy(convertString, enumDisplay, enumDisplay_size);
								
								// And clean up our mess:
								error = sSPBasic->FreeBlock(enumDisplay);
								enumDisplay = NULL;
								
								break;
								}
							default:
								{
								convertString[0] = '?';
								convertString[1] = '\0';
								break;
								}
								
							} // end type switch
						
						error = PIUReplaceCharsInBlock
							(
							"^4",
							convertString,
							kIgnoreCase,
							&displayAll
							);
							
						sADMBasic->MessageAlert(displayAll);
						sSPBasic->FreeBlock(displayAll);
						displayAll = NULL;
						
						} // displayAll
						
					} // loop
					
				sSPBasic->FreeBlock(displayWithTotal);
				displayWithTotal = NULL;
				
				} // displayWithTotal
		
			sPSActionDescriptor.Free(&descriptor);
			
			}
		else
			{
			sADMBasic->MessageAlert("Descriptor is empty.");
			}
		
		} // IsValid
	
	#endif // DEBUG ONLY
	
	} // end DisplayDescriptor			

//-------------------------------------------------------------------------------
//	Document utilities.
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
//	class PIUElement
//
//-------------------------------------------------------------------------------

PIUElement::PIUElement
	(
	unsigned short initIndex
	) :
	identity_(initIndex)
	{
	// Intentionally left blank.
	}

// At this point we're going to try to grab the default basic descriptor
// for this element and populate at least the Identity information:
void PIUElement::IElement(PIUElement* parent)
	{
	parent_ = parent;
	reference_.IReference(this); // Reference's parent is this element.
	
	SPErr error = CreateIdentity(&identity_);
	}

//-----------------------------------------------------------------------
//	Object properties.
//-----------------------------------------------------------------------
		
// Return the ID of the element, if available (most are unique by
// name):
unsigned long PIUElement::GetID (void) const
	{
	return identity_.GetID();
	}
	
// Return a string with the name of the currently selected
// element. This is a copy, so the caller is responsible for
// deleting it:
char* PIUElement::GetName (void) const
	{
	return identity_.GetName();
	}

// Return the index of the item amongst its siblings, if
// available:
unsigned short PIUElement::GetIndex (void) const
	{
	return identity_.GetIndex();
	}
	
// Returns the propagation level of the current identity:
bool PIUElement::GetShallow (void) const
	{
	return identity_.GetShallow();
	}
	
// Changes the propagation level of the current identity:
void PIUElement::MarkShallow (void)
	{
	identity_.SetShallow(true);
	}
	
void PIUElement::ClearShallow (void)
	{
	identity_.SetShallow(false);
	}
	
// Returns the class of this element:
DescriptorClassID PIUElement::GetClass (void) const
	{
	return GetClassKey();
	}
	
//-----------------------------------------------------------------------
//	Browsing element tree.
//-----------------------------------------------------------------------

// Return the number of elements of this type:
unsigned short PIUElement::GetCount (void)
	{
	unsigned short count = 0;
	
	if (sPSActionDescriptor.IsValid())
		{
		PIActionDescriptor descriptor = GetAllProperties();
		DescriptorKeyID countKey = GetCountKey();
			
		if (descriptor != NULL && countKey != keyNull)
			{
			// We have a descriptor.  Go see if our useful
			// key is in there:
			Boolean hasKey = false;
			SPErr error = sPSActionDescriptor->HasKey
				(
				descriptor,
				countKey,
				&hasKey
				);
				
			if (hasKey)
				{
				// Key is in there.  Get its value:
				int32 value = 0;
				
				error = sPSActionDescriptor->GetInteger
					(
					descriptor,
					countKey,
					&value
					);
				
				// Assign the long to our count:
				count = (unsigned short)value;
				}
			
			// Free the descriptor:
			sPSActionDescriptor.Free(&descriptor);
			
			}
		}
	
	return count;
	}

// Trigger a generic select event using the current identity:
SPErr PIUElement::Select (PIActionReference reference)
	{
	SPErr error = kSPNoError;
	
	if 
		(
		sPSActionReference.IsValid() &&
		sPSActionDescriptor.IsValid() && 
		sPSActionControl.IsValid()
		)
		{
		// If reference is not provided, we'll make this one:
		if (reference == 0)
			{
			reference = BuildReference();
			}

		if (reference != 0 && error == kSPNoError)
			{
			PIActionDescriptor descriptor = reference_.CreateAndSetTarget(reference);
			
			if (descriptor != 0)
				{
				error = PlayAndSetResult(eventSelect, descriptor);
				sPSActionDescriptor.Free(&descriptor);
				} // descriptor
			} // reference
			
			sPSActionReference.Free(&reference);

		} // actionReference, actionDescriptor, actionControl
		
	return error;
	}
	
// Trigger a generic select event using the current identity:
SPErr PIUElement::SelectShallow (void)
	{
	return Select(BuildReferenceShallow());
	}
	
// Move to an element of the tree by ID really means just
// finding that reference and changing our identity to it:
SPErr PIUElement::SelectByID (const unsigned long id)
	{
	return SelectByIndex(FindIndexForID(id));
	}

// Move to an element of the tree by name:
SPErr PIUElement::SelectByName (const char* const name)
	{
	return SelectByIndex(FindIndexForName(name));
	}
	
// Move to the Nth element of the tree:
SPErr PIUElement::SelectByIndex (const unsigned short index)
	{
	SPErr error = kPIUElementNotValid;

	if (index > 0)
		{
		// Create a new identity and put this info into it:
		PIUIdentity identity;
		identity.SetIndex(index);
		identity.SetNext(identity_.GetNext()); // Retain pointer.
		
		// Now to to resolve and update this new identity:
		error = UpdateIdentity(&identity);
		
		// If we had no error, then select this new identity:
		if (error == kSPNoError)
			error = Select();
		}
				
	return error;
	}		

// Move to an element of the tree by ID without resolving parents:
SPErr PIUElement::SelectByIDShallow (const unsigned long id)
	{
	return SelectByIndexShallow(FindIndexForIDShallow(id));
	}

// Move to an element of the tree by name without resolving parents:
SPErr PIUElement::SelectByNameShallow (const char* const name)
	{
	return SelectByIndexShallow(FindIndexForNameShallow(name));
	}

// Move to the Nth element of the tree without resolving parents:
SPErr PIUElement::SelectByIndexShallow (const unsigned short index)
	{
	SPErr error = kPIUElementNotValid;

	if (index > 0)
		{
		// Create a new identity and put this info into it:
		PIUIdentity identity;
		identity.SetIndex(index);
		identity.SetNext(identity_.GetNext()); // Retain pointer.
		
		// Now to to resolve and update this new identity:
		error = UpdateIdentity(&identity, false); // Don't propagate
		
		// If we had no error, then select this new identity:
		if (error == kSPNoError)
			error = SelectShallow();
		}
				
	return error;
	}		

// Push current selection onto stack:
void PIUElement::Push (void)
	{
	identity_.Push();
	}
	
// Pop current selection from stack:
SPErr PIUElement::Pop (void)
	{
	return identity_.Pop();
	}
	
// Pop current selection from stack and select it:
SPErr PIUElement::PopAndSelect (void)
	{
	SPErr error = Pop();
	if (error == kSPNoError)
		{
		error = Select();
		}
		
	return error;
	}
		
//-----------------------------------------------------------------------
//	Play utilities.
//-----------------------------------------------------------------------

// Plays an action and sets the result class to its result descriptor:
SPErr PIUElement::PlayAndSetResult
	(
	/* IN */	const DescriptorEventID event,
	/* IN */	const PIActionDescriptor descriptor,
	/* IN */	const PIDialogPlayOptions options
	)
	{
	SPErr error = kSPNoError;
	
	if (sPSActionDescriptor.IsValid() && sPSActionControl.IsValid())
		{
		PIActionDescriptor result = 0;
		
		error = sPSActionControl->Play
			(
			&result,
			event,
			descriptor,
			options
			);
			
		Result()->SetDescriptor(result);
		Result()->SetID(eventSelect);
		
		if (result != 0)
			sPSActionDescriptor.Free(&result);
		}
				
	return error;
	}


//-----------------------------------------------------------------------
//	Search facilities.
//-----------------------------------------------------------------------

// Find the index by ID:
unsigned short PIUElement::FindIndexForID (const unsigned long id)
	{
	unsigned short index = 0;
	
	PIUIdentity identity;
	identity.SetID(id);
	
	SPErr error = ResolveIdentity(&identity);
	
	if (error == kSPNoError)
		index = identity.GetIndex();
	
	return index;
	}
		
// Find the index by name:
unsigned short PIUElement::FindIndexForName (const char* const name)
	{
	unsigned short index = 0;
	
	PIUIdentity identity;
	identity.SetName(name);
	
	SPErr error = ResolveIdentity(&identity);
	
	if (error == kSPNoError)
		index = identity.GetIndex();
		
	return index;
	}

// Find the index by ID without resolving parents:
unsigned short PIUElement::FindIndexForIDShallow (const unsigned long id)
	{
	unsigned short index = 0;
	
	PIUIdentity identity;
	identity.SetID(id);
	
	SPErr error = ResolveIdentity(&identity, false);
	
	if (error == kSPNoError)
		index = identity.GetIndex();
	
	return index;
	}
		
// Find the index by name without resolving parents:
unsigned short PIUElement::FindIndexForNameShallow (const char* const name)
	{
	unsigned short index = 0;
	
	PIUIdentity identity;
	identity.SetName(name);
	
	SPErr error = ResolveIdentity(&identity, false);
	
	if (error == kSPNoError)
		index = identity.GetIndex();
		
	return index;
	}

//-----------------------------------------------------------------------
//	Browsing utilities.
//-----------------------------------------------------------------------

// Create an identity from the initial descriptor information that
// we get at construction time. If this fails there's nothing we can
// do but wait until the user supplies us with more info:
SPErr PIUElement::CreateIdentity
	(
	/* IN/OUT */	PIUIdentity* identity
	)
	{
	SPErr error = kSPNoError;

	if (identity != NULL)
		{	
		identity->Clean();
		
		PIActionDescriptor descriptor = GetAllProperties();
		
		if (descriptor != NULL)
			{
			PIUIdentity newIdentity;
			newIdentity.SetID(ParseIDKey(descriptor));
			newIdentity.SetIndex(ParseIndexKey(descriptor));
			
			// ParseNameKey returns a copy of the name string
			// from the descriptor.  SetName() also makes a copy,
			// so we declare a variable so we can free it when
			// we're done:
			char* name = ParseNameKey(descriptor);
			newIdentity.SetName(name);
			if (name != NULL && sSPBasic.IsValid())
				{
				sSPBasic->FreeBlock(name);
				name = NULL;
				}
							
			if 
				(
				newIdentity.GetID() != kPIUInvalidID ||
				newIdentity.GetNamePtr() != NULL ||
				newIdentity.GetIndex() != 0
				)
				{
				// This operator only replaces different items
				// that are non-null.
				*identity += newIdentity;
				}
			else
				{
				// Means we actually did search and couldn't
				// find any valid keys to id with:
				error = kPIUElementNotValid;
				}
			}
		else
			{
			// Means we actually did search and came up with
			// squat:
			error = kPIUElementNotFound;
			}
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	}	

// Resolve and replace the internal identity with the new one. Returns
// an error if it cannot resolve the identity:
SPErr PIUElement::UpdateIdentity
	(
	/* IN */	PIUIdentity* identity,
	/* IN */	const bool propagate
	)
	{
	// First resolve the identity we've been handed:
	SPErr error = kSPNoError;
	
	if (identity != NULL)
		{
		error = ResolveIdentity(identity, propagate);
	
		if (error == kSPNoError)
			{
			// Now replace.
			identity_ = *identity;
			}
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	}

// Resolve an identity from whatever items are available. Returns an
// error if it cannot resolve the identity:
SPErr PIUElement::ResolveIdentity
	(
	/* IN/OUT */	PIUIdentity* identity,
	/* IN */		const bool propagate
	)
	{	
	SPErr error = kSPNoError;

	if (identity != NULL && sPSActionReference.IsValid())
		{
		// We make this particular reference by hand because we want to
		// try to verify this identity before we use our normal routines:
		PIActionReference reference = sPSActionReference.Make();
		
		const bool lastShallow = identity->GetShallow();
		
		// Try all different forms to create a reference:
		AddReference(reference, &error, *identity);

		if (reference != 0 && error == kSPNoError)
			{
			if (propagate || !identity->GetShallow()) Propagate(reference, &error);
		
			PIActionDescriptor descriptor = GetAllKeys(reference);
		
			if (descriptor == 0 && !identity->GetShallow())
				{
				// Couldn't find it.  Try a shallow reference:
				identity->SetShallow(true);
				
				sPSActionReference->Free(reference);
				reference = sPSActionReference.Make(); // New.
				
				AddReference(reference, &error, *identity);
				
				descriptor = GetAllKeys(reference);
				}
				
			if (descriptor != 0)
				{
				PIUIdentity newIdentity;
				newIdentity.SetID(ParseIDKey(descriptor));
				newIdentity.SetIndex(ParseIndexKey(descriptor));
		
				// We get a copy of the string from the descriptor,
				// so we assign it to a variable so we can free it
				// after SetName() makes its copy:
				char* name = ParseNameKey(descriptor);
				newIdentity.SetName(name);
				if (name != NULL && sSPBasic.IsValid())
					{
					sSPBasic->FreeBlock(name);
					name = NULL;
					}
				
				if 
					(
					newIdentity.GetID() != kPIUInvalidID ||
					newIdentity.GetNamePtr() != NULL ||
					newIdentity.GetIndex() != 0
					)
					{
					// This operator only replaces different items
					// that are non-null.
					*identity += newIdentity;
					}
				else
					{
					// Means we actually did search and couldn't
					// find any valid keys to id with:
					error = kPIUElementNotValid;
					identity->SetShallow(lastShallow);
					}
				}
			else
				{
				// Means we actually did search and came up with
				// squat:
				error = kPIUElementNotFound;
				identity->SetShallow(lastShallow);	// Remove shallow.
				}
			}
		else
			{
			error = kSPBadParameterError;
			}
			
		} // identity != NULL
		
	else
		{
		error = kSPBadParameterError;
		}
				
	return error;
	}	

		
//-----------------------------------------------------------------------
//	Object keys.
//-----------------------------------------------------------------------

// This routine is responsible for returning all the keys that the
// host returns for the different pertinent calls.  It maps the standard
// keys and classes to what is specific for this element.  You should
// always provide it with your derived class.
const DescriptorKeyID* PIUElement::ElementKeyMap (void) const
	{
	// This constant object isn't going to move while we use it, which
	// is only a couple of times:
	
	static const DescriptorKeyID map[] = 
		{
		// Original key, translate-to key:
		
		// Class of this element.
		typeClass,		classApplication,
		
		// Which key to start with for reference build.
		keyCurrent,		keyCurrent,
		
		// Order these in order of how you want to search
		// for the name:
		
		// Key to use for id:
		keyID,			keyID,
		
		// Key to use for name:
		keyName,		keyName,
		
		// Key to use for index:
		keyItemIndex,	keyItemIndex,
		
		// Key to use for sibling count:
		keyCount,		keyCount,
		
		NULL	// Must be null terminated.
		};
		
	return map;
	}

DescriptorKeyID PIUElement::DoElementKeyMap 
	(
	const DescriptorKeyID inKey
	) const
	{
	return reference_.DoElementKeyMap(inKey);
	}

//-----------------------------------------------------------------------
//	Object property utilities.
//-----------------------------------------------------------------------

// Hopefully you should never have to diddle with how you're going to
// get your properties back, and the Action Abstration Layer will
// manage the issues in the background.  The issues are substantial,
// since many items cannot be referenced except by index or name, and
// sometimes those things are unique and sometimes they're not.

// Returns a descriptor pointing to all available
// properties for the element.  It is the callers responsibility to
// free the descriptor via Free(descriptor):
PIActionDescriptor PIUElement::GetAllProperties (PIActionReference reference)
	{
	if (reference == 0)
		return GetAllKeys();
	else
		return GetAllKeys(reference);
	}
	
PIActionDescriptor PIUElement::BuildSinglePropertyDescriptor
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* OUT */	bool*						deleteWhenDone
	)
	{
	SPErr error = kSPNoError;
	
	PIActionDescriptor local_descriptor = descriptor;

	if (descriptor == 0 && sPSActionReference.IsValid())
		{
		// Try to build a special reference for this:
		PIActionReference reference = sPSActionReference.Make();
		
		if (reference != 0)
			{
			error = reference_.AddProperty(reference, inKey);

			if (error == kSPNoError)
				{
				PropagateReference(reference, &error);
				}
			}
		
		if (reference != 0)
			{
			if (error == kSPNoError)
				{
				// Reference is deleted for us:
				local_descriptor = GetAllKeys(reference);
				}
			else
				{
				sPSActionReference.Free(&reference);
				}
			}
		}
		
	*deleteWhenDone = (local_descriptor != descriptor);
	
	return local_descriptor;
	}
	
SPErr PIUElement::GetSingleProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* OUT */	DescriptorTypeID*			outType,
	/* OUT */	long*						outValue,
	/* OUT */	double*						outFloat
	)
	{
	SPErr error = kSPNoError;
	
	if (outType == NULL || outValue == NULL || outFloat == NULL)
		{
		error = kSPBadParameterError;
		return error;
		}
	
	bool deleteWhenDone = false;
	
	PIActionDescriptor local_descriptor = BuildSinglePropertyDescriptor
		(
		descriptor,
		inKey,
		&deleteWhenDone
		);
		
	if (local_descriptor != 0 && sPSActionDescriptor.IsValid())
		{
		Boolean hasKey = FALSE;
		
		error = sPSActionDescriptor->HasKey
			(
			local_descriptor,
			inKey,
			&hasKey
			);
		
		if (error == kSPNoError && hasKey == TRUE)
			{
			DescriptorTypeID local_typeID = typeNull;
			
			error = sPSActionDescriptor->GetType
				(
				local_descriptor,
				inKey,
				outType
				);
			
			if (error == kSPNoError)
				{
				switch(*outType)
					{
					case typeInteger:
						{
						error = sPSActionDescriptor->GetInteger
							(
							local_descriptor,
							inKey,
							outValue
							);
						
						break;
						}
						
					case typeFloat:
						{
						error = sPSActionDescriptor->GetFloat
							(
							local_descriptor,
							inKey,
							outFloat
							);
						
						break;
						}
					
					case typeUnitFloat:
						{
						DescriptorUnitID units = unitNone;
						
						error = sPSActionDescriptor->GetUnitFloat
							(
							local_descriptor,
							inKey,
							&units,
							outFloat
							);
							
						*outValue = (long)units;
						
						break;
						}
					
					case typeText:
						{
						char* text = sPSActionDescriptor.GetString
							(
							local_descriptor, 
							inKey
							);
						
						*outValue = (long)text;
						
						break;
						}
						
					case typeBoolean:
						{
						Boolean value = FALSE;
						
						error = sPSActionDescriptor->GetBoolean
							(
							local_descriptor,
							inKey,
							&value
							);
			
						*outValue = 0;
						
						if (value == TRUE)
							*outValue = 1;
							
						break;
						}
						
					case typeEnumerated:
						{
						error = sPSActionDescriptor->GetEnumerated
							(
							local_descriptor,
							inKey,
							(DescriptorEnumTypeID*)outType,
							(DescriptorEnumID*)outValue
							);
						
						break;
						}
						
					case typeObject:
						{
						error = sPSActionDescriptor->GetObject
							(
							local_descriptor,
							inKey,
							(DescriptorClassID*)outType,
							(PIActionDescriptor*)outValue
							);
						
						break;
						}

					case typeGlobalObject:
						{
						error = sPSActionDescriptor->GetGlobalObject
							(
							local_descriptor,
							inKey,
							(DescriptorClassID*)outType,
							(PIActionDescriptor*)outValue
							);
						
						break;
						}
						
					case typeObjectReference:
						{
						error = sPSActionDescriptor->GetReference
							(
							local_descriptor,
							inKey,
							(PIActionReference*)outValue
							);
						
						break;
						}
						
					case typeValueList:
						{
						error = sPSActionDescriptor->GetList
							(
							local_descriptor,
							inKey,
							(PIActionList*)outValue
							);
							
						break;
						}
					
					default:
						{
						error = kSPBadParameterError;
						break;
						}
							
					} // switch
					
				} // get type error
				
			} // haskey
		else
			{
			error = kSPBadParameterError;
			}
				
		if (deleteWhenDone)
			{
			// Then we can free the local descriptor, otherwise its the calling
			// party's responsibility:
			sPSActionDescriptor.Free(&local_descriptor);
			}
		
		} // local_descriptor
		
	return error;
	} // GetSingleProperty

bool PIUElement::GetSingleBooleanProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey
	)
	{
	bool result = false;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeBoolean &&
		value != 0
		)
		{
		result = true;
		}
	
	return result;
	}
	
long PIUElement::GetSingleIntegerProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey
	)
	{
	long result = 0;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeInteger &&
		value != 0
		)
		{
		result = value;
		}
	
	return result;
	}
		
double PIUElement::GetSingleFloatProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey
	)
	{
	long result = 0;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeFloat &&
		value != 0
		)
		{
		result = value;
		}
	
	return result;
	}

double PIUElement::GetSingleUnitFloatProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* OUT */	DescriptorUnitID*			outUnit
	)
	{
	double result = 0;
	
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeUnitFloat &&
		value != 0
		)
		{
		result = floatValue;
		
		if (outUnit != NULL)
			*outUnit = (DescriptorUnitID)value;
		}
	
	return result;
	}

DescriptorEnumID PIUElement::GetSingleEnumeratedProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const DescriptorEnumTypeID	inEnumTypeID
	)
	{
	DescriptorEnumID result = enumNull;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		(DescriptorEnumTypeID)outType == inEnumTypeID
		)
		{
		result = (DescriptorEnumID)value;
		}
	
	return result;
	}

PIActionDescriptor PIUElement::GetSingleObjectProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const DescriptorClassID		inClassID
	)
	{
	PIActionDescriptor result = 0;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		(DescriptorClassID)outType == inClassID
		)
		{
		result = (PIActionDescriptor)value;
		}
	
	return result;
	}

PIActionList PIUElement::GetSingleListProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey
	)
	{
	PIActionList result = 0;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeValueList
		)
		{
		result = (PIActionList)value;
		}
	
	return result;
	}

char* PIUElement::GetSingleStringProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey
	)
	{
	char* result = NULL;
		
	DescriptorTypeID outType = typeNull;
	long value = 0;
	double floatValue = 0.0;
	
	SPErr error = GetSingleProperty
		(
		descriptor,	// Build a descriptor automatically.
		inKey,
		&outType,
		&value,
		&floatValue
		);
		
	if 
		(
		error == kSPNoError &&
		outType == typeText
		)
		{
		result = (char*)value;
		}
	
	return result;
	}

SPErr PIUElement::SetSingleProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const DescriptorTypeID		inType,
	/* IN */	const DescriptorClassID		inClass,	// Class or enum.
	/* IN */	const long					inValue,
	/* IN */	const double				inFloat
	)
	{
	SPErr error = kSPNoError;
	
	PIActionDescriptor local_descriptor = descriptor;
	
	if (inType == typeNull || (inClass == classNull && inValue == 0 && inFloat == 0))
		{
		error = kSPBadParameterError;
		return error;
		}
	
	if (local_descriptor == 0 && sPSActionReference.IsValid())
		{
		// Try to build a special reference for this:
		PIActionReference reference = sPSActionReference.Make();
		
		if (reference != 0)
			{
			error = reference_.AddProperty(reference, inKey);

			if (error == kSPNoError)
				{
				PropagateReference(reference, &error);
				}
			}
		
		// Do second check for reference before deleting it:
		if (reference != 0)
			{
			if (error == kSPNoError)
				{
				// We need to add the reference to our descriptor:
				local_descriptor = reference_.CreateAndSetTarget(reference);
				}
			
			sPSActionReference.Free(&reference);
			}
		}
	
	if (local_descriptor != 0 && sPSActionDescriptor.IsValid() && sPSActionControl.IsValid())
		{
		// Syntax is "set key to value", so we first put a reference
		// to the actual key into a descriptor as "keyTarget", then
		// we set the new value as "keyTo":
		
		switch(inType)
			{
			case typeInteger:
				{
				error = sPSActionDescriptor->PutInteger
					(
					local_descriptor,
					keyTo,
					inValue
					);

				break;
				}
				
			case typeFloat:
				{
				error = sPSActionDescriptor->PutFloat
					(
					local_descriptor,
					keyTo,
					inFloat
					);
				
				break;
				}
			
			case typeUnitFloat:
				{
				DescriptorUnitID units = unitNone;
				
				error = sPSActionDescriptor->PutUnitFloat
					(
					local_descriptor,
					keyTo,
					(DescriptorUnitID)inValue,	// "Value" variable is our units.
					inFloat
					);
					
				break;
				}
			
			case typeText:
				{
				error = sPSActionDescriptor->PutString
					(
					local_descriptor,
					keyTo,
					(char*)inValue		// "Value" variable is our string pointer.
					);
					
				break;
				}
				
			case typeBoolean:
				{
				error = sPSActionDescriptor->PutBoolean
					(
					local_descriptor,
					keyTo,
					(inValue != 0)
					);
	
				break;
				}
				
			case typeEnumerated:
				{
				error = sPSActionDescriptor->PutEnumerated
					(
					local_descriptor,
					keyTo,
					(DescriptorEnumTypeID)inClass,	// "Class" variable is our enum type.
					(DescriptorEnumID)inValue		// "Value" variable is our enum.
					);
				
				break;
				}
				
			case typeObject:
				{
				error = sPSActionDescriptor->PutObject
					(
					local_descriptor,
					keyTo,
					inClass,
					(PIActionDescriptor)inValue	// "Value" variable is our descriptor.
					);
				
				break;
				}
				
			case typeGlobalObject:
				{
				error = sPSActionDescriptor->PutGlobalObject
					(
					local_descriptor,
					keyTo,
					inClass,
					(PIActionDescriptor)inValue	// "Value" variable is our descriptor.
					);
				
				break;
				}

			case typeObjectReference:
				{
				error = sPSActionDescriptor->PutReference
					(
					local_descriptor,
					keyTo,
					(PIActionReference)inValue		// "Value" variable is our reference.
					);
					
				break;
				}

			default:
				{
				error = kSPBadParameterError;
				break;
				}
				
			} // switch
		
		if (error == kSPNoError)
			{
			error = PlayAndSetResult(eventSet, local_descriptor);
			}
			
		if (descriptor == 0)
			{
			// Then we can free the local descriptor, otherwise its the calling
			// party's responsibility:
			sPSActionDescriptor.Free(&local_descriptor);
			}
		
		} // local_descriptor
	
	return error;
	} // end SetSingleProperty
	
SPErr PIUElement::SetSingleBooleanProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const bool					inBool
	)
	{
	long value = 0;
	
	if (inBool)
		value = 1;
		
	SPErr error = SetSingleProperty
		(
		descriptor,	// Build a descriptor automatically if 0.
		inKey,
		typeBoolean,
		classNull,
		value,
		0.0
		);
		
	return error;
	}
	
SPErr PIUElement::SetSingleIntegerProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const long					inValue
	)
	{
	SPErr error = SetSingleProperty
		(
		descriptor,	// Build a descriptor automatically if 0.
		inKey,
		typeInteger,
		classNull,
		inValue,
		0.0
		);
		
	return error;
	}
		
SPErr PIUElement::SetSingleEnumeratedProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const DescriptorEnumTypeID	inEnumType,
	/* IN */	const DescriptorEnumID		inEnum
	)
	{
	SPErr error = SetSingleProperty
		(
		descriptor,	// Build a descriptor automatically if 0.
		inKey,
		typeEnumerated,
		(DescriptorClassID)inEnumType,
		(long)inEnum,
		0.0
		);
		
	return error;
	}

SPErr PIUElement::SetSingleObjectProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const DescriptorClassID		inClass,
	/* IN */	const PIActionDescriptor	classDescriptor
	)
	{
	SPErr error = SetSingleProperty
		(
		descriptor,	// Build a descriptor automatically if 0.
		inKey,
		typeObject,
		inClass,
		(long)classDescriptor,
		0.0
		);
		
	return error;
	}

SPErr PIUElement::SetSingleStringProperty
	(
	/* IN */	const PIActionDescriptor	descriptor,
	/* IN */	const DescriptorKeyID		inKey,
	/* IN */	const char* const			inString
	)
	{
	SPErr error = SetSingleProperty
		(
		descriptor,	// Build a descriptor automatically if 0.
		inKey,
		typeText,
		classNull,
		(long)inString,
		0.0
		);
		
	return error;
	}
	
// Retrieve all the keys for this element by current element, if it
// has a concept of 'current'.  May not always work.  Don't use this,
// use GetAllProperties().
PIActionDescriptor PIUElement::GetAllKeys (void)
	{
	return GetAllKeys(BuildReference());
	}

// Retrieve all the keys given a reference:
PIActionDescriptor PIUElement::GetAllKeys (PIActionReference reference)
	{
	PIActionDescriptor descriptor = 0;
	SPErr error = kSPNoError;
		
	if (reference != 0)
		{
			
		if (sPSActionControl.IsValid())
			{
			// Oooh, we have the action control suite.  Use that.
			error = sPSActionControl->Get
				(
				&descriptor,		
				reference
				);
			}
		else if (sPSBasicActionControl.IsValid())
			{
			// Basic controls will have it, too.
			error = sPSBasicActionControl->Get
				(
				&descriptor,
				reference
				);
			}		
		}
		
		if (descriptor != 0)
			{
			Result()->SetDescriptor(descriptor);
			Result()->SetID(GetClass());
			
			if (error != kSPNoError)
				{
				// There was an error.  Destroy the descriptor.
				sPSActionDescriptor.Free(&descriptor);
				}
			}
			
		// Now destroy this reference, since its our responsibility:
		if (sPSActionReference.IsValid())
			{
			sPSActionReference.Free(&reference);
			}
				
	return descriptor;	
	}

// Return the parent object, if any:
PIUElement* PIUElement::GetParent(void)
	{
	return parent_;
	}

// Rip apart a descriptor and read the useful keys from it:
unsigned long PIUElement::ParseIDKey(const PIActionDescriptor descriptor)
	{
	unsigned long id = kPIUInvalidID;
	
	DescriptorKeyID key = GetIDKey(); // Returns NULL if none.
	
	if (sPSActionDescriptor.IsValid() && descriptor != NULL && key != keyNull)
		{
		int32 value = 0;
		SPErr error = sPSActionDescriptor->GetInteger
			(
			descriptor,
			key,
			&value
			);
			
		if (error == kSPNoError && value != kPIUInvalidID)
			{
			id = (unsigned long)value;
			}
		}
	return id;
	}
	
char* PIUElement::ParseNameKey(const PIActionDescriptor descriptor)
	{
	char* name = NULL;
	
	DescriptorKeyID key = GetNameKey(); // Returns NULL if none.
	
	if (sPSActionDescriptor.IsValid() && descriptor != NULL && key != keyNull)
		{
		name = sPSActionDescriptor.GetString(descriptor, key);
		}
		
	return name;
	}
	
unsigned short PIUElement::ParseIndexKey(const PIActionDescriptor descriptor)
	{
	unsigned short index = 0;
	
	DescriptorKeyID key = GetIndexKey(); // Returns NULL if none.
	
	if (sPSActionDescriptor.IsValid() && descriptor != NULL && key != keyNull)
		{
		int32 value = 0;
		
		SPErr error = sPSActionDescriptor->GetInteger
			(
			descriptor,
			key,
			&value
			);
			
		if (error == kSPNoError && value != 0)
			index = (unsigned short)value;
		}

	return index;
	}
	
//-----------------------------------------------------------------------
//	PIUIdentity.
//-----------------------------------------------------------------------
PIUElement::PIUIdentity::~PIUIdentity()
	{
	Clean();
	// next_ is cleaned when parent auto-destructs.
	}
				
// Init:
void PIUElement::PIUIdentity::IIdentity(void)
	{
	SetID(kPIUInvalidID);
	SetNamePtr(NULL);
	SetIndex(initIndex_); // May want to try the first position if all else fails.
	SetShallow(true);
	SetNext(NULL);
	}
	
// Make a new one on the heap:
PIUElement::PIUIdentity* PIUElement::PIUIdentity::Make(void)
	{
	return new PIUIdentity();
	}

// Clean out, freeing any allocated memory:
void PIUElement::PIUIdentity::Clean(void)
	{
	// Release name pointer:
	if (GetNamePtr() != NULL && sSPBasic.IsValid())
		{
		// Release anything already there:
		sSPBasic->FreeBlock((void*)GetNamePtr());
		}
		
	IIdentity(); // Resets everything, including setting name to NULL.
	}

// Clean all out, freeing all allocated memory, walking stack list:
void PIUElement::PIUIdentity::CleanStack(void)
	{
	Clean();
	
	if (GetNext() != NULL)
		{
		GetNext()->CleanStack();
		delete GetNext();
		}
	}

// Copy constructor:
PIUElement::PIUIdentity::PIUIdentity(const PIUIdentity& identity) :
	initIndex_(identity.initIndex_)
	{
	Clean();
	SetID(identity.GetID());
	SetName(identity.GetNamePtr());	// Use this so we don't make 2 copies.
	SetIndex(identity.GetIndex());
	SetShallow(identity.GetShallow());
	SetNext(identity.GetNext());
	}
	
// Assignment operator:
PIUElement::PIUIdentity& PIUElement::PIUIdentity::operator=(const PIUIdentity& identity)
	{
	Clean();
	SetID(identity.GetID());
	SetName(identity.GetNamePtr());	// Use this so we don't make 2 copies.
	SetIndex(identity.GetIndex());
	SetShallow(identity.GetShallow());
	SetNext(identity.GetNext());
	return *this;
	}
	
// Use addition operator to replace only different non-null items:
PIUElement::PIUIdentity& PIUElement::PIUIdentity::operator+=
	(
	const PIUIdentity& identity
	)
	{
	if (identity.GetID() != kPIUInvalidID)
		{
		SetID(identity.GetID());
		}
		
	if (identity.GetNamePtr() != NULL)
		{
		SetName(identity.GetNamePtr());	// Use this so we don't make 2 copies.
		}

	if (identity.GetIndex() != 0)
		{
		SetIndex(identity.GetIndex());
		}
		
	if (identity.GetShallow() != GetShallow())
		{
		SetShallow(identity.GetShallow());
		}

	// NOTE: We are not updating next_.
	
	return *this;
	}
	
// Push current identity onto the stack.  The top of the stack is
// always the current identity:
void PIUElement::PIUIdentity::Push(void)
	{
	// Make a new identity:
	PIUIdentity* identity = Make();
	
	// Copy the current one to this heap-based one:
	*identity = *this;
	
	// Place a pointer to this heap-based one in the last_ variable:
	SetNext(identity);
	}

// Pop identity into current one from the stack:
SPErr PIUElement::PIUIdentity::Pop(void)
	{
	SPErr error = kPIUElementNotValid;
	
	// Make a copy of the pointer to the stack identity:
	PIUIdentity* old = GetNext();
	
	if (old != NULL)
		{
		// Copy the identity to our current one:
		*this = *old;
		
		// Delete the old copy:
		delete old;
		
		error = kSPNoError;
		}
	
	return error;
	}	

// Get routines for identity:
unsigned long PIUElement::PIUIdentity::GetID(void) const
	{
	return id_;
	}

// We make a copy and pass it back.  It is the callers
// responsibility to delete it:	
char* PIUElement::PIUIdentity::GetName(void) const
	{
	char* name = NULL;

	if (GetNamePtr() != NULL)
		{
		name = PIUCopyStringToBlock(GetNamePtr(), PIUstrlen(GetNamePtr()));
		}
		
	return name;
	}

// Returns the actual pointer:
const char* const PIUElement::PIUIdentity::GetNamePtr(void) const
	{
	return name_;
	}

unsigned short PIUElement::PIUIdentity::GetIndex(void) const
	{
	return index_;
	}
	
bool PIUElement::PIUIdentity::GetShallow(void) const
	{
	return shallow_;
	}

PIUElement::PIUIdentity* PIUElement::PIUIdentity::GetNext(void) const
	{
	return next_;
	}
	
// Set routines for identity:
void PIUElement::PIUIdentity::SetID(const unsigned long id)
	{
	id_ = id;
	}
	
// We make a copy so caller must delete incoming:
void PIUElement::PIUIdentity::SetName(const char* const name)
	{
	char* copy = NULL;

	if (name != NULL)
		{
		const unsigned short length = PIUstrlen(name);
		
		if (length > 0)
			copy = PIUCopyStringToBlock(name, length);
		}

	// Now release what's there:
	if (GetNamePtr() != NULL && sSPBasic.IsValid())
		{
		// Release anything already there:
		sSPBasic->FreeBlock((void*)GetNamePtr());
		}
		
	SetNamePtr(copy);
	
	// We don't free copy because our pointer now points to its
	// memory location.
	}

// Just copy the pointer, and do _nothing else_:
void PIUElement::PIUIdentity::SetNamePtr(const char* const name)
	{
	name_ = (char*)name;	
	}
	
void PIUElement::PIUIdentity::SetIndex(const unsigned short index)
	{
	index_ = index;
	}

void PIUElement::PIUIdentity::SetShallow(const bool shallow)
	{
	shallow_ = shallow;
	}

void PIUElement::PIUIdentity::SetNext(PIUIdentity* const next)
	{
	next_ = next;
	}

//-----------------------------------------------------------------------
//	Reference utilities.
//-----------------------------------------------------------------------

// Hopefully you should never have to build a reference to something
// because the Action Abstraction Layer has all the accessor routines
// you'll ever need to target and do the things you want.  These are
// used as low-level utilities to get around.  Things are still finicky
// as of Photoshop 5, where some partial references will resolve
// correctly and some won't.  We take the approach of building the
// entire reference every time.

// Initialize a reference class:
void PIUElement::PIUReference::IReference(PIUElement* parent)
	{
	parent_ = parent;
	}

// Add a reference to the current element (if it has a concept of
// 'current'.  May not always work.):
SPErr PIUElement::PIUReference::Add 
	(
	/* IN/OUT */	PIActionReference reference
	)
	{
	return Add(reference, typeOrdinal, enumTarget);
	}

// Add a reference to the element by id:
SPErr PIUElement::PIUReference::Add 
	(
	/* IN/OUT */	PIActionReference reference, 
	/* IN */		const unsigned long id
	)
	{
	SPErr error = kSPNoError;

	if (reference != NULL && id != kPIUInvalidID && sPSActionReference.IsValid())
		{
		error = sPSActionReference->PutIdentifier
			(
			reference,
			GetClassKey(),		// Our element's class.
			id
			);
		}
	else
		{
		error = kPIUElementNotValid;
		}
	
	return error;
	}

// Add a reference to the element by name:
SPErr PIUElement::PIUReference::Add 
	(
	/* IN/OUT */	PIActionReference reference, 
	/* IN */		const char* const name
	)
	{
	SPErr error = kSPNoError;

	if (reference != NULL && name != NULL && sPSActionReference.IsValid())
		{
		error = sPSActionReference->PutName
			(
			reference,
			GetClassKey(),		// Our element's class.
			(char*)name
			);
		}
	else
		{
		error = kPIUElementNotValid;
		}
	
	return error;
	}

// Build a reference to the element by index:
SPErr PIUElement::PIUReference::Add 
	(
	/* IN/OUT */	PIActionReference reference, 
	/* IN */		const unsigned short index
	)
	{
	SPErr error = kSPNoError;
	
	if (reference != NULL && index != 0 && sPSActionReference.IsValid())
		{
		uint32 value = (uint32)index;
		
		error = sPSActionReference->PutIndex
			(
			reference,
			GetClassKey(),		// Our element's class.
			value
			);
		}
	else
		{
		error = kPIUElementNotValid;
		}
	
	return error;
	}

// Add an enumeration to a reference
SPErr PIUElement::PIUReference::Add 
	(
	/* IN/OUT */	PIActionReference reference,
	/* IN */		const DescriptorEnumTypeID eTypeID,
	/* IN */		const DescriptorEnumID eEnumID
	)
	{
	SPErr error = kSPNoError;

	if (reference != NULL && eTypeID != 0 && sPSActionReference.IsValid())
		{
		error = sPSActionReference->PutEnumerated
			(
			reference,
			GetClassKey(),		// Our element's class.
			eTypeID,
			eEnumID
			);
		}
	else
		{
		error = kPIUElementNotValid;
		}
	
	return error;
	}
	
// Add a single property to a reference:
SPErr PIUElement::PIUReference::AddProperty
	(
	/* IN/OUT */	PIActionReference reference,
	/* IN */		const DescriptorKeyID inKey,
	/* IN */		const DescriptorClassID inClass
	)
	{
	SPErr error = kSPNoError;
	
	if (reference != NULL && sPSActionReference.IsValid())
		{
		error = sPSActionReference->PutProperty
			(
			reference,
			inClass,
			inKey
			);
		}
	else
		{
		error = kSPBadParameterError;
		}
	
	return error;
	}

DescriptorKeyID PIUElement::PIUReference::DoElementKeyMap 
	(
	const DescriptorKeyID inKey
	) const
	{
	DescriptorKeyID outKey = keyNull;
		
	// If we can't get to the parent's ElementKeyMap() routine, we might as well
	// not bother doing this:
	if (parent_ != NULL)
		{
		const DescriptorKeyID* map = parent_->ElementKeyMap();
		
		unsigned short iterator = 0;
		
		while (outKey == keyNull && iterator < kElementKeyMapMax)
			{
			if (map[iterator] == inKey)
				outKey = map[iterator+1];
			
			// Only need every other key:
			iterator += 2;
			}
		}
			
	return outKey;
	}

PIActionDescriptor PIUElement::PIUReference::CreateAndSetTarget
	(
	PIActionReference reference
	)
	{
	PIActionDescriptor descriptor = 0;
	
	if (reference != 0 && sPSActionDescriptor.IsValid())
		{
		descriptor = sPSActionDescriptor.Make();

		if (descriptor != 0)
			{
			SPErr error = sPSActionDescriptor->PutReference
				(
				descriptor,
				keyTarget,
				reference
				);
			}
		}
	
	return descriptor;
	}
	
//-------------------------------------------------------------------------------

// Add a reference to this specific element in a growing reference
// and pass it on.  This will step down to find the reference
// form that actually works:
void PIUElement::AddReference
	(
	/* IN/OUT */	PIActionReference reference,
	/* IN/OUT */	SPErr* error,
	/* IN */		const PIUIdentity& identity
	)
	{
	SPErr local_error = kSPNoError;
	
	if (reference != NULL && sPSActionReference.IsValid())
		{
	
		// The map is expected to return with the keys in the
		// order that the caller wants us to search.  If there
		// is a search method that is unimplemented, then
		// that key should be removed from the list:
		const DescriptorKeyID* map = ElementKeyMap();
		
		// Keep going until we have no error:
		local_error = kPIUElementNotValid;
		
		unsigned char loop = 0;
		
		// We don't check for keyNull as being the target key in the map because
		// even though there may not be a key we can get this data out of, that
		// doesn't mean the user may not have specified it by name, so just try
		// it and let it error out normally if the identity info is invalid:
		while (local_error != kSPNoError && loop < kElementKeyMapMax)
			{
			switch(map[loop])
				{
				case keyID:
					{
					local_error = reference_.Add(reference, identity.GetID());
					break;
					}
				case keyName:
					{
					// We'll use GetNamePtr because we don't need a new copy of the
					// name string:
					local_error = reference_.Add(reference, identity.GetNamePtr());
					break;
					}
				case keyItemIndex:
					{
					local_error = reference_.Add(reference, identity.GetIndex());
					break;
					}
				case keyCurrent:
					{
					local_error = reference_.Add(reference);
					break;
					}
				case typeClass:
				case keyCount:
					{
					// These two keys appear in the map, but we don't need them
					// for this routine.
					break;
					}
				case NULL:
					{
					// We're at the end of the array.  Bump the counter
					// to the end of the valid entries so we can exit:
					loop = kElementKeyMapMax;
					break;
					}
				default:
					{
					local_error = kPIUElementNotValid;
					break;
					}
				}
						
			// We may not have had an error, but just for
			// safety, bump this so we can do the check at the
			// top and prevent endless loops. And we're doing
			// every other key:
			loop += 2;

			} // while
		
		}
	else
		{
		local_error = kSPBadParameterError; // reference && sSPActionReference
		}
	
	// Want to report successes (kSPNoError), or the last error:
	if (local_error == kSPNoError || (local_error != kSPNoError && *error != kSPNoError))
		*error = local_error;
	// This will propagate up to show that something actually succeeded.
	}
	
// Build a reference all the way up the chain:
PIActionReference PIUElement::BuildReference(void)
	{
	SPErr error = kSPNoError;
	
	PIActionReference reference = 0;
	
	if (sPSActionReference.IsValid())
		{
		reference = sPSActionReference.Make();
	
		// We started here, but if we can't even make a valid
		// reference, don't bother propagating:
		if (reference != 0)
			{
			PropagateReference(reference, &error);
			}
		}
							
	if (error != kSPNoError)
		{
		// Nothing succeeded in the propagate chain.  Delete this
		// reference (sets it to 0, also):
		sPSActionReference.Free(&reference);
		}
		
	return reference;
	}

// Build a reference only at this level, without propagating:
PIActionReference PIUElement::BuildReferenceShallow(void)
	{
	SPErr error = kSPNoError;
	
	PIActionReference reference = 0;
	
	if (sPSActionReference.IsValid())
		{
		reference = sPSActionReference.Make();
	
		// We started here, but if we can't even make a valid
		// reference, don't bother propagating:
		if (reference != 0)
			{
			AddReference(reference, &error, GetIdentity());
			}
		}
							
	if (error != kSPNoError)
		{
		// Nothing succeeded in the propagate chain.  Delete this
		// reference (sets it to 0, also):
		sPSActionReference.Free(&reference);
		}
		
	return reference;
	}

// References are constructed "channel 4 of layer 3 of document 2
// of application 1", always from most specific to least.  The easiest
// way to remember this is "put 'of' between everything".  We're
// going to use this to our advantage to do a logical traversal up
// our parent chain and ask each element in the chain to add itself
// to the reference.
void PIUElement::PropagateReference
	(
	/* IN/OUT */	PIActionReference reference,
	/* IN/OUT */	SPErr* error
	)
	{
	// Will return an error if it can't add to the reference.  It is our callers
	// responsibility to Free the bad reference:
	AddReference (reference, error, identity_); // Add ourselves to this reference.
	
	// May have been an error, but we'll still see if we can construct a
	// reference to something along the chain:
	Propagate (reference, error);
	}
		
void PIUElement::Propagate
	(
	/* IN/OUT */	PIActionReference reference,
	/* IN/OUT */	SPErr* error
	)
	{
	// No need to propagate this reference if its not created, or
	// if we have no parent, or if it represents a shallow (single-level)
	// identity:
	if (GetParent() != NULL && reference != NULL && !GetShallow())
		{
		GetParent()->PropagateReference(reference, error);
		}
	}	

// Return different key values, or keyNull if none available:
DescriptorKeyID PIUElement::PIUReference::GetIDKey(void) const
	{
	return DoElementKeyMap(keyID);
	}
	
DescriptorKeyID PIUElement::PIUReference::GetNameKey(void) const
	{
	return DoElementKeyMap(keyName);
	}
	
DescriptorKeyID PIUElement::PIUReference::GetIndexKey(void) const
	{
	return DoElementKeyMap(keyItemIndex);
	}

DescriptorKeyID PIUElement::PIUReference::GetCountKey(void) const
	{
	return DoElementKeyMap(keyCount);
	}
	
DescriptorClassID PIUElement::PIUReference::GetClassKey(void) const
	{
	return (DescriptorClassID)DoElementKeyMap(typeClass);
	}

PIUElement::PIUIdentity& PIUElement::GetIdentity (void)
	{
	return identity_;
	}

//-------------------------------------------------------------------------------
//	Filter events.
//-------------------------------------------------------------------------------

void PIUFilter::GaussianBlur
	(
	/* IN/OUT */	double* radius,
	/* IN/OUT */	SPErr* error
	)
	{
	if (*error == kSPNoError)
		{
		if (sPSActionDescriptor.IsValid() && sPSActionControl.IsValid())
			{
			const double kRadiusMin = 0.1;
			const double kRadiusMax = 250.0;
			
			// Not even going to bother giving you an error.  We'll
			// just pin the value for you.
			PIUPinDouble(radius, kRadiusMin, kRadiusMax);
			
			// Create the descriptor:
			PIActionDescriptor descriptor = sPSActionDescriptor.Make();
			
			// Put the value in it:
			sPSActionDescriptor->PutFloat
				(
				descriptor,
				keyRadius,
				*radius
				);
				
			// We always assume you want no dialog:
			const PIDialogPlayOptions options = plugInDialogSilent;

			// Create a variable to receive the result:
			PIActionDescriptor result = NULL;
			
			*error = sPSActionControl->Play
				(
				&result,
				eventGaussianBlur,
				descriptor,
				options
				);
			
			// Copy descriptor and event to result tracker:
			Result()->SetDescriptor(result);
			Result()->SetID(eventGaussianBlur);
			
			if (*error == kSPNoError && result != NULL)
				{
				// All happened fine.  Lets rip apart our result
				// descriptor and see if there is a new radius
				// we can report:
				SPErr error = sPSActionDescriptor->GetFloat
					(
					result,
					keyRadius,
					radius
					);
					
				// We got the result descriptor, its our job
				// to free it up:
				sPSActionDescriptor.Free(&result);
				}
			}
		else
			{
			*error = kSPSuiteNotFoundError;
			}
		}
	else
		{
		// Error code was already set to something, so
		// don't do anything and return.
		}
		
	} // end GaussianBlur	

//-------------------------------------------------------------------------------
//
//	class PIUHistory
//
//	This is the suite that provides all the history utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUHistory::ElementKeyMap (void) const
	{
	// First is class, then order indicates search order, and
	// listing indicates actual key to use for generic
	// property:
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classHistoryState,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyName,		keyName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		
		keyID,			keyNull,	// Indicates no key available.
		
		// Must be null terminated:
		NULL,			NULL
		};
						
	return map;
	}
	
// Indicates whether this is an automatic or named history entry:
bool PIUHistory::IsAuto (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyAuto
		);
		
	return result;
	}

// Indicates whether the current entry is the brush source:
bool PIUHistory::IsBrushSource (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyHistoryBrushSource
		);
		
	return result;
	}

// Indicates whether the referenced state is the current state:
bool PIUHistory::IsCurrentState (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyCurrentHistoryState
		);
		
	return result;
	}

		
//-------------------------------------------------------------------------------
//
//	class PIUAction
//
//	This is the suite that provides all the action utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUAction::ElementKeyMap (void) const
	{
	// First is class, then order indicates search order, and
	// listing indicates actual key to use for generic
	// property:
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classAction,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyName,		keyName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		
		keyID,			keyNull,	// Indicates no key available.
		
		// Must be null terminated:
		NULL,			NULL
		};
						
	return map;
	}

// Actions and ActionSets return the count-1, so we override
// and fix it:
unsigned short PIUAction::GetCount (void)
	{
	unsigned short count = PIUElement::GetCount();
	
	// Count is pretty much always off by one, except when
	// there are none, which it reports zero.  To be safe
	// we always report more than one, which will simply
	// result in a not-found error, which we can deal with:
	++count;
		
	return count;
	}
	
unsigned short PIUAction::GetCountSansSet (void)
	{
	unsigned short count = 0;
	
	if 
		(
		sPSActionReference.IsValid() &&
		sPSActionDescriptor.IsValid()
		)
		{
		SPErr error = kSPNoError;
		
		// We make this particular reference by hand because we want to
		// try to verify this identity before we use our normal routines:
		PIActionReference reference = BuildReferenceShallow();

		// Don't propagate as we want this to be the only level
		// when we search:
		error = reference_.AddProperty(reference, GetCountKey());

		if (reference != 0)
			{
			PIActionDescriptor descriptor = GetAllProperties(reference);
		
			if (descriptor != 0)
				{			
				long value = GetSingleIntegerProperty
					(
					descriptor,	// Use the one we just created.
					GetCountKey()
					);
					
				if (value != 0)
					{
					count = (unsigned short)value;
					}
					
				sPSActionDescriptor.Free(&descriptor);
				}
			
			sPSActionReference.Free(&reference);
			}
		}
		
	return count;	
	}		

SPErr PIUAction::SelectByNameSansSet (const char* const name)
	{
	return SelectByNameShallow (name);
	}
	
SPErr PIUAction::SelectByIndexSansSet (const unsigned short index)
	{
	return SelectByIndexShallow (index);
	}

// This routine frees the reference passed to it:
SPErr PIUAction::Play (PIActionReference reference)
	{
	SPErr error = kSPNoError;
	
	if (reference == 0)
		{
		reference = BuildReference();
		}
		
	if (sPSActionControl.IsValid())
		{
		PIActionDescriptor descriptor = reference_.CreateAndSetTarget(reference);
		
		if (descriptor != 0)
			{
			error = PlayAndSetResult(eventPlay, descriptor);
			sPSActionDescriptor.Free(&descriptor);
			} // descriptor
		} // reference
		
		sPSActionReference.Free(&reference);
	
	return error;
	}

SPErr PIUAction::PlayShallow (void)
	{
	return Play (BuildReferenceShallow());
	}

//-------------------------------------------------------------------------------
//
//	class PIUActionSet
//
//	This is the suite that provides all the action set utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUActionSet::ElementKeyMap (void) const
	{
	// First is class, then order indicates search order, and
	// listing indicates actual key to use for generic
	// property:
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classActionSet,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyName,		keyName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference.
		// Not going to supply keyCurrent because there is no
		// such thing as a single "current action set":
		// keyCurrent,		keyCurrent,
		
		keyID,			keyNull,	// Indicates no key available.
		
		// Must be null terminated:
		NULL,			NULL
		};
						
	return map;
	}

// Actions and ActionSets return the count-1, so we override
// and fix it:
unsigned short PIUActionSet::GetCount (void)
	{
	unsigned short count = PIUElement::GetCount();
	
	// We see error cuz we're always one less than we need to be
	// unless we're 0 in which case we might be really 0.  Since
	// we can deal with non-existant indexes, lets just always
	// report more than less:
	++count;
		
	return count;
	}

	
//-------------------------------------------------------------------------------
//
//	class PIUPath
//
//	This is the suite that provides all the path utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUPath::ElementKeyMap (void) const
	{
	// First is class, then order indicates search order, and
	// listing indicates actual key to use for generic
	// property:
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classPath,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyName,		keyPathName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		
		keyID,			keyNull,	// Indicates no key available.
		
		// Must be null terminated:
		NULL,			NULL
		};
						
	return map;
	}
	
// Is the current path the target?
bool PIUPath::IsTarget (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyTargetPath
		);
		
	return result;
	}
	
//-----------------------------------------------------------------------
//	Utilities for Path Kind.
//-----------------------------------------------------------------------
const PIUPathKindToEnum_t pathKindAndEnum_map[] =
	{
	kPIUNormalPath,			enumNormalPath,
	kPIUWorkPath,			enumWorkPath,
	kPIUClippingPath,		enumClippingPath,
	kPIUInvalidPathKind,	enumNull	// Must end with this.
	};

const unsigned short kPIUPathKindToEnum_max = 10;	// Safety for loop.

DescriptorEnumID PIUPath::MapPathKindToEnum
	(
	const PIUPathKind_t kind
	)
	{
	DescriptorEnumID result = pathKindAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUPathKindToEnum_max &&
		!foundResult &&
		pathKindAndEnum_map[iter].kind != kPIUInvalidPathKind
		)
		{
		if (pathKindAndEnum_map[iter].kind == kind)
			{
			foundResult = true;
			result = pathKindAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUPathKind_t PIUPath::MapEnumToPathKind
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUPathKind_t result = kPIUInvalidPathKind;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUPathKindToEnum_max &&
		!foundResult &&
		pathKindAndEnum_map[iter].kind != kPIUInvalidPathKind
		)
		{
		if (pathKindAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = pathKindAndEnum_map[iter].kind;
			}
		
		++iter;
		}
		
	return result;
	}

PIUPathKind_t PIUPath::GetKind (void)
	{
	PIUPathKind_t result = kPIUInvalidPathKind;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyKind,
		typePathKind
		);

	if (value != enumNull)
		{
		result = MapEnumToPathKind(value);
		}
	
	return result;
	}

// Clipping paths have one extra piece of information, their
// flatness.  So if this is a clipping path, flatness will
// be a valid value:
double PIUPath::GetFlatness (void)
	{
	double result = 0.0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyFlatness
		);
		
	if (value != 0)
		{
		result = PIUFixed16ToDouble(value);
		}
		
	return result;
	}

SPErr PIUPath::SetFlatness (const double flatness)
	{
	long value = PIUDoubleToFixed16(flatness);
	
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyFlatness,
		value
		);		
	
	return error;
	}
		
//-------------------------------------------------------------------------------
//
//	class PIUChannel
//
//	This is the suite that provides all the channel utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUChannel::ElementKeyMap (void) const
	{
	// First is class, then order indicates search order, and
	// listing indicates actual key to use for generic
	// property:
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classChannel,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyName,		keyChannelName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		
		keyID,			keyNull,	// Indicates no key available.
		
		// Must be null terminated:
		NULL,			NULL
		};
						
	return map;
	}

// Indicates whether this channel is visible in the palette:
bool PIUChannel::IsVisible (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0, 	// Create descriptor from class.
		keyVisible
		);
		
	return result;
	}
	
// If the current channel is an alpha channel, then these
// routines will return alpha channel info:
bool PIUChannel::IsAlpha (void)
	{
	bool result = false;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyAlphaChannelOptions,
		classAlphaChannelOptions
		);
		
	if (descriptor != 0)
		{
		// Descriptor present means this is an alpha channel.
		result = true;
		
		// Now delete the descriptor we just got:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		}
	
	return result;
	}

unsigned short PIUChannel::GetAlphaOpacity (void)
	{
	unsigned short result = 0;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyAlphaChannelOptions,
		classAlphaChannelOptions
		);
		
	if (descriptor != 0)
		{
		long value = GetSingleIntegerProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyOpacity
			);
		
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		
		if (value != 0)
			{
			result = (unsigned short)value;
			}
		}
	
	return result;
	}

//-----------------------------------------------------------------------
//	Utilities for Image Mode.
//-----------------------------------------------------------------------
const PIUAlphaColorIndicatorToEnum_t indicatorAndEnum_map[] =
	{
	kPIUAlphaColorIsMaskedArea,		enumMaskedAreas,
	kPIUAlphaColorIsSelectedArea,	enumSelectedAreas,
	kPIUAlphaColorIsSpot,			enumSpotColor,
	kPIUInvalidAlphaColorIndicator,	enumNull	// Must end with this.
	};

const unsigned short kPIUAlphaColorIndicatorToEnum_max = 10;	// Safety for loop.

DescriptorEnumID PIUChannel::MapAlphaColorIndicatorToEnum
	(
	const PIUAlphaColorIndicator_t indicator
	)
	{
	DescriptorEnumID result = indicatorAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUAlphaColorIndicatorToEnum_max &&
		!foundResult &&
		indicatorAndEnum_map[iter].indicator != kPIUInvalidAlphaColorIndicator
		)
		{
		if (indicatorAndEnum_map[iter].indicator == indicator)
			{
			foundResult = true;
			result = indicatorAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUAlphaColorIndicator_t PIUChannel::MapEnumToAlphaColorIndicator
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUAlphaColorIndicator_t result = kPIUInvalidAlphaColorIndicator;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUAlphaColorIndicatorToEnum_max &&
		!foundResult &&
		indicatorAndEnum_map[iter].indicator != kPIUInvalidAlphaColorIndicator
		)
		{
		if (indicatorAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = indicatorAndEnum_map[iter].indicator;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUAlphaColorIndicator_t PIUChannel::GetAlphaColorIndicator (void)
	{
	PIUAlphaColorIndicator_t result = kPIUInvalidAlphaColorIndicator;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyAlphaChannelOptions,
		classAlphaChannelOptions
		);
		
	if (descriptor != 0)
		{
		DescriptorEnumID value = GetSingleEnumeratedProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyColorIndicates,
			typeMaskIndicator
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		
		if (value != enumNull)
			{
			result = MapEnumToAlphaColorIndicator(value);
			}
		}
	
	return result;
	}
	
//-------------------------------------------------------------------------------
//
//	class PIULayer
//
//	This is the suite that provides all the channel utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIULayer::ElementKeyMap (void) const
	{
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classLayer,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyID,			keyLayerID,
		keyName,		keyName,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		
		// Must be null terminated:
		NULL,			NULL
		};
		
	return map;
	}

// Is this a background layer? (Which could mean either that this
// is a flat document, or that just this layer is a background
// layer.  You can tell this by whether there are other layers
// or not using GetCount()):
bool PIULayer::IsBackground (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyBackground
		);
	
	return result;
	}

// Indicate whether layer is set to be visible or not in the palette:
bool PIULayer::IsVisible (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyVisible
		);
		
	return result;
	}


//-----------------------------------------------------------------------
//	Utilities for Blend Mode.
//-----------------------------------------------------------------------
const PIUBlendModeToEnum_t blendModeAndEnum_map[] =
	{
	kPIUBlendNormal,		enumNormal,
	kPIUBlendDissolve,		enumDissolve,
	kPIUBlendBehind,		enumBehind,
	kPIUBlendClear,			enumClear,
	kPIUBlendMultiply,		enumMultiply,
	kPIUBlendScreen,		enumScreen,
	kPIUBlendOverlay,		enumOverlay,
	kPIUBlendSoftLight,		enumSoftLight,
	kPIUBlendHardLight,		enumHardLight,
	kPIUBlendDarken,		enumDarken,
	kPIUBlendLighten,		enumLighten,
	kPIUBlendDifference,	enumDifference,
	kPIUBlendHue,			enumHue,
	kPIUBlendSaturation,	enumSaturation,
	kPIUBlendColor,			enumColor,
	kPIUBlendLuminosity,	enumLuminosity,
	kPIUBlendExclusion,		enumExclusion,
	kPIUBlendColorDodge,	enumColorDodge,
	kPIUBlendColorBurn,		enumColorBurn,
	kPIUInvalidBlendMode,	enumNull
	};

const unsigned short kPIUBlendModeToEnum_max = 30;	// Safety for loop.

DescriptorEnumID PIULayer::MapBlendModeToEnum
	(
	const PIUBlendMode_t mode
	)
	{
	DescriptorEnumID result = blendModeAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUBlendModeToEnum_max &&
		!foundResult &&
		blendModeAndEnum_map[iter].mode != kPIUInvalidBlendMode
		)
		{
		if (blendModeAndEnum_map[iter].mode == mode)
			{
			foundResult = true;
			result = blendModeAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUBlendMode_t PIULayer::MapEnumToBlendMode
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUBlendMode_t result = kPIUInvalidBlendMode;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUBlendModeToEnum_max &&
		!foundResult &&
		blendModeAndEnum_map[iter].mode != kPIUInvalidBlendMode
		)
		{
		if (blendModeAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = blendModeAndEnum_map[iter].mode;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUBlendMode_t PIULayer::GetBlendMode (void)
	{
	PIUBlendMode_t result = kPIUInvalidBlendMode;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyMode,
		typeColorSpace
		);

	if (value != enumNull)
		{
		result = MapEnumToBlendMode(value);
		}
	
	return result;
	}
	
SPErr PIULayer::SetBlendMode (const PIUBlendMode_t mode)
	{
	SPErr error = kSPBadParameterError;
	
	if (mode != kPIUInvalidBlendMode)
		{
		DescriptorEnumID inEnum = MapBlendModeToEnum(mode);
		
		error = SetSingleEnumeratedProperty
			(
			0,	// Create descriptor from class.
			keyMode,
			typeBlendMode,
			inEnum
			);
		}
			
	return error;
	}

// NOTE: There is an error in Photoshop 5.0 where you cannot
// get the linked info of the first layer.  This is a known
// bug.  Simply walk the rest of the layers and find the
// other layers that it is linked to, if that one is
// of particular interest.

// Indicates how many other layers are linked to this one:
unsigned short PIULayer::GetLinkCount (void)
	{
	unsigned short result = 0;
	
	PIActionList list = GetSingleListProperty
		(
		0,	// Create descriptor from class.
		keyLinkedLayerIDs
		);
		
	if (list != 0 && sPSActionList.IsValid())
		{
		uint32 count = 0;
		
		SPErr error = sPSActionList->GetCount(list, &count);
		
		if (error == kSPNoError)
			{
			result = (unsigned short)count;
			}
			
		// We retrieved it so it is ours to free:
		error = sPSActionList->Free(list);
		list = 0;
		}
		
	return result;
	}
		
// Returns the id of layer N from the link list.  To change
// to that layer, just use SelectByID(N).  Index is expected
// to be 1-based:
long PIULayer::GetLinkIDByIndex (const unsigned short index)
	{
	long result = 0;
	
	PIActionList list = GetSingleListProperty
		(
		0,	// Create descriptor from class.
		keyLinkedLayerIDs
		);
		
	if (list != 0 && sPSActionList.IsValid())
		{
		uint32 count = 0;
		
		SPErr error = sPSActionList->GetCount(list, &count);
		
		if (error == kSPNoError && count > 0)
			{
			// Now check our index value:
			if (index < 1 || index > count)
				error = kSPBadParameterError;
			else
				{
				// Lists are 0-based, so subtract one from our 1-based index and
				// grab that item from the list:
				error = sPSActionList->GetInteger(list, index-1, &result);
				}
			}
			
		// We retrieved it so it is ours to free:
		error = sPSActionList->Free(list);
		list = 0;
		}
		
	return result;
	}

// The opacity of this layer (if not flat) from 0...255:
unsigned short PIULayer::GetOpacity (void)
	{
	unsigned short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyOpacity
		);
		
	if (value != 0)
		{
		result = (unsigned short)value;
		}
		
	return result;
	}
	
SPErr PIULayer::SetOpacity (const unsigned short opacity)
	{
	long value = (long)opacity;
	
	// Even though this is an unsigned, lets ceiling and
	// floor it to the values we're expecting:
	if (value < 0)
		value = 0;
	else if (value > 255)
		value = 255;
	
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyOpacity,
		value
		);
		
	return error;
	}

// If this layer can be transparent, the value of its preserve transparency:
bool PIULayer::GetPreserveTransparency (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyPreserveTransparency
		);
		
	return result;
	}

SPErr PIULayer::SetPreserveTransparency (const bool preserve)
	{
	SPErr error = SetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyPreserveTransparency,
		preserve
		);
		
	return error;
	}

// If there is a user mask, is it enabled?
bool PIULayer::IsUserMaskEnabled (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyUserMaskEnabled
		);
		
	return result;
	}

// If there is a user mask, is it linked to the layer image?
bool PIULayer::IsUserMaskLinked (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyUserMaskLinked
		);
		
	return result;
	}

// Are layer effects set to visible?
bool PIULayer::IsLayerEffectsVisible (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyLayerFXVisible
		);
		
	return result;
	}

// Global angle of layer effects:
short PIULayer::GetLayerEffectsGlobalAngle (void)
	{
	short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyGlobalAngle
		);
		
	if (value != 0)
		{
		result = (short)value;
		}
		
	return result;
	}

//-------------------------------------------------------------------------------
//
//	class PIUDocument
//
//	This is the suite that provides all the document utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUDocument::ElementKeyMap (void) const
	{
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classDocument,
		
		// keyCount and actual key to use for count:
		keyCount,		keyCount,
		
		// Order to search elements, with their actual keys to use:
		keyID,			keyDocumentID,
		keyName,		keyTitle,
		keyItemIndex,	keyItemIndex,

		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,

		// Must be null terminated:
		NULL,			NULL
		};
		
	return map;
	}

//-----------------------------------------------------------------------
//	Utilities for Image Mode.
//-----------------------------------------------------------------------
const PIUImageModeToEnum_t imageModeAndEnum_map[] =
	{
	kPIUModeBitmap,			enumBitmap,
	kPIUModeGrayscale,		enumGrayScale,
	kPIUModeIndexed,		enumIndexedColor,
	kPIUModeRGB,			enumRGBColor,
	kPIUModeCMYK,			enumCMYKColor,
	kPIUModeHSL,			enumHSLColor,
	kPIUModeHSB,			enumHSBColor,
	kPIUModeMultichannel,	enumMultichannel,
	kPIUModeLab,			enumLabColor,
	kPIUModeGrayscale16,	enumGray16,
	kPIUModeRGB48,			enumRGB48,
	kPIUModeLab48,			enumLab48,
	kPIUModeCMYK64,			enumCMYK64,
	kPIUInvalidImageMode,	enumNull	// Must end with this.
	};

const unsigned short kPIUImageModeToEnum_max = 20;	// Safety for loop.

DescriptorEnumID PIUDocument::MapImageModeToEnum
	(
	const PIUImageMode_t mode
	)
	{
	DescriptorEnumID result = imageModeAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUImageModeToEnum_max &&
		!foundResult &&
		imageModeAndEnum_map[iter].mode != kPIUInvalidImageMode
		)
		{
		if (imageModeAndEnum_map[iter].mode == mode)
			{
			foundResult = true;
			result = imageModeAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUImageMode_t PIUDocument::MapEnumToImageMode
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUImageMode_t result = kPIUInvalidImageMode;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUImageModeToEnum_max &&
		!foundResult &&
		imageModeAndEnum_map[iter].mode != kPIUInvalidImageMode
		)
		{
		if (imageModeAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = imageModeAndEnum_map[iter].mode;
			}
		
		++iter;
		}
		
	return result;
	}
	
// Mode of the document:
PIUImageMode_t PIUDocument::GetImageMode (void)
	{
	PIUImageMode_t result = kPIUInvalidImageMode;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyMode,
		typeColorSpace
		);

	if (value != enumNull)
		{
		result = MapEnumToImageMode(value);
		}
	
	return result;
	}
	

// Return the horizontal and vertical pixel size:
PIUDPoint_t PIUDocument::GetSize (void)
	{
	PIUDPoint_t result = { 0.0, 0.0 };

	DescriptorUnitID unit = unitNone;
	
	result.h = GetSingleUnitFloatProperty
		(
		0,	// Create descriptor from class.
		keyHorizontal,
		&unit	// Unit will be unitDistance (pixels at doc resolution)
		);
		
	result.v = GetSingleUnitFloatProperty
		(
		0,	// Create descriptor from class.
		keyVertical,
		&unit	// Unit will be unitDistance (pixels at doc resolution)
		);

	return result;
	}
	

// Resolution of the document in pixels per inch:
double PIUDocument::GetResolution (void)
	{
	DescriptorUnitID unit = unitNone;
	
	double result = GetSingleUnitFloatProperty
		(
		0,	// Create descriptor from class.
		keyResolution,
		&unit	// Unit will be unitDensity (absolute pixels)
		);
		
	return result;
	}

// Return or set the horizontal and/or vertical nudge values:
PIUDPoint_t PIUDocument::GetNudge (void)
	{
	PIUDPoint_t result = { 0.0, 0.0 };

	DescriptorUnitID unit = unitNone;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyBigNudgeH
		);
		
	if (value != 0)
		{
		result.h = PIUFixed16ToDouble(value);
		}
		
	value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyBigNudgeV
		);
		
	if (value != 0)
		{
		result.v = PIUFixed16ToDouble(value);
		}

	return result;
	}
		
SPErr PIUDocument::SetNudge (const PIUDPoint_t inNudge)
	{
	long value = PIUDoubleToFixed16(inNudge.h);
	
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyBigNudgeH,
		value
		);
		
	if (error == kSPNoError)
		{
		value = PIUDoubleToFixed16(inNudge.v);
		
		error = SetSingleIntegerProperty
			(
			0,	// Create descriptor from class.
			keyBigNudgeV,
			value
			);
		}
	
	return error;
	}

// Return or set the horizontal and/or vertical origin:
PIUDPoint_t PIUDocument::GetOrigin (void)
	{
	PIUDPoint_t result = { 0.0, 0.0 };

	DescriptorUnitID unit = unitNone;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyRulerOriginH
		);
		
	if (value != 0)
		{
		result.h = PIUFixed16ToDouble(value);
		}
		
	value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyRulerOriginV
		);
		
	if (value != 0)
		{
		result.v = PIUFixed16ToDouble(value);
		}

	return result;
	}
		
SPErr PIUDocument::SetOrigin (const PIUDPoint_t inOrigin)
	{
	long value = PIUDoubleToFixed16(inOrigin.h);
	
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyRulerOriginH,
		value
		);
		
	if (error == kSPNoError)
		{
		value = PIUDoubleToFixed16(inOrigin.v);
		
		error = SetSingleIntegerProperty
			(
			0,	// Create descriptor from class.
			keyRulerOriginV,
			value
			);
		}
	
	return error;
	}

// Bit depth of the document:
unsigned short PIUDocument::GetDepth (void)
	{
	unsigned short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyDepth
		);
		
	if (value > 0)
		{
		result = (unsigned short)value;
		}
		
	return result;
	}	
		
// Is this document marked with a copyright?
bool PIUDocument::GetCopyright (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyCopyright
		);
		
	return result;
	}

// Is this document marked with a digital watermark?
bool PIUDocument::GetWatermark (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyWatermark
		);
		
	return result;
	}
	
SPErr PIUDocument::SetWatermark (const bool inWatermark)
	{
	SPErr error = SetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyWatermark,
		inWatermark
		);
		
	return error;
	}

//-----------------------------------------------------------------------
//	Utilities for PreviewCMYK.
//-----------------------------------------------------------------------
const PIUPreviewCMYKToEnum_t previewCMYKAndEnum_map[] =
	{
	kPIUPreviewOff,			enumPreviewOff,
	kPIUPreviewCMYK,		enumPreviewCMYK,
	kPIUPreviewCyan,		enumPreviewCyan,
	kPIUPreviewMagenta,		enumPreviewMagenta,
	kPIUPreviewYellow,		enumPreviewYellow,
	kPIUPreviewBlack,		enumPreviewBlack,
	kPIUPreviewCMY,			enumPreviewCMY,
	kPIUInvalidPreviewCMYK,	enumNull	// Must end with this.
	};

const unsigned short kPIUPreviewCMYKToEnum_max = 10;	// Safety for loop.

DescriptorEnumID PIUDocument::MapPreviewCMYKToEnum
	(
	const PIUPreviewCMYK_t preview
	)
	{
	DescriptorEnumID result = previewCMYKAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUPreviewCMYKToEnum_max &&
		!foundResult &&
		previewCMYKAndEnum_map[iter].preview != kPIUInvalidPreviewCMYK
		)
		{
		if (previewCMYKAndEnum_map[iter].preview == preview)
			{
			foundResult = true;
			result = previewCMYKAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUPreviewCMYK_t PIUDocument::MapEnumToPreviewCMYK
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUPreviewCMYK_t result = kPIUInvalidPreviewCMYK;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUPreviewCMYKToEnum_max &&
		!foundResult &&
		previewCMYKAndEnum_map[iter].preview != kPIUInvalidPreviewCMYK
		)
		{
		if (previewCMYKAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = previewCMYKAndEnum_map[iter].preview;
			}
		
		++iter;
		}
		
	return result;
	}

// Are we previewing in a CMYK mode?  Which mode, if so?
PIUPreviewCMYK_t PIUDocument::GetPreviewCMYK (void)
	{
	PIUPreviewCMYK_t result = kPIUInvalidPreviewCMYK;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyPreviewCMYK,
		typePreviewCMYK
		);

	if (value != enumNull)
		{
		result = MapEnumToPreviewCMYK(value);
		}
	
	return result;
	}
	
SPErr PIUDocument::SetPreviewCMYK (const PIUPreviewCMYK_t preview)
	{
	SPErr error = kSPBadParameterError;
	
	if (preview != kPIUInvalidEyedropperSample)
		{
		DescriptorEnumID inEnum = MapPreviewCMYKToEnum(preview);
		
		error = SetSingleEnumeratedProperty
			(
			0,	// Create descriptor from class.
			keyPreviewCMYK,
			typePreviewCMYK,
			inEnum
			);
		}
			
	return error;
	}

// Is this document marked to be saved?
bool PIUDocument::IsDirty (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyIsDirty
		);
		
	return result;
	}
	
SPErr PIUDocument::SetDirty (const bool dirty)
	{
	SPErr error = SetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyIsDirty,
		dirty
		);
		
	return error;
	}

// Returns a file specification reference to the document, if
// one exists (untitled documents have no alias records until
// they are saved):
SPErr PIUDocument::GetFileReference
	(
	/* OUT */	SPPlatformFileSpecification*	fileSpec
	)
	{
	SPErr error = kSPNoError;

	if (fileSpec != NULL)
		{
		PIUClearFileSpec(fileSpec);
		
		bool deleteWhenDone = true;
		
		PIActionDescriptor descriptor = BuildSinglePropertyDescriptor
			(
			0,	// Create descriptor from class.
			keyFileReference,
			&deleteWhenDone
			);
		
		if (descriptor != NULL)
			{
			error = PIURestoreFileSpec
				(
				descriptor,
				keyFileReference,
				fileSpec
				);
			}
		}
	else
		{
		error = kSPBadParameterError;
		}	
	
	return error;
	}

// keyFileInfo is a long text list of different keys stuffed in classFileInfo.
// You can figure it out yourself.	

//-------------------------------------------------------------------------------
//
//	class PIUApplication
//
//	This is the suite that provides all the application utilities.
//
//-------------------------------------------------------------------------------

const DescriptorKeyID* PIUApplication::ElementKeyMap (void) const
	{
	static const DescriptorKeyID map[] =
		{
		// Class of this element:
		typeClass,		classApplication,

		// keyCount and actual key to use for count:
		keyCount,		keyCount,

		// Order to search elements, with their actual keys to use.
		// Since keyCurrent has no actual key, insert it simply
		// to determine when to use current target as reference:
		keyCurrent,		keyCurrent,
		keyID,			keyNull,
		keyName,		keyHostName,
		keyItemIndex,	keyNull,

		// Must be null terminated:
		NULL,			NULL
		};
		
	return map;
	}
	
//-----------------------------------------------------------------------
//	Application Preferences.
//-----------------------------------------------------------------------			

//-----------------------------------------------------------------------
//	Utilities for Ruler Units.
//-----------------------------------------------------------------------
const PIURulerUnitsToEnum_t rulerUnitsAndEnum_map[] =
	{
	kRulerPixels,	enumRulerPixels,
	kRulerInches,	enumRulerInches,
	kRulerCm,		enumRulerCm,
	kRulerPicas,	enumRulerPicas,
	kRulerPoints,	enumRulerPoints,
	kRulerPercent,	enumRulerPercent,
	kRulerPixels,	enumNull	// Must terminate with enumNull.
	};

const unsigned short kPIURulerUnitsToEnum_max = 20; // Safety maximum.

RulerUnits PIUApplication::MapEnumToRulerUnits (const DescriptorEnumID aEnum)
	{			
	RulerUnits result = rulerUnitsAndEnum_map[0].rulerUnits;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIURulerUnitsToEnum_max &&
		!foundResult &&
		rulerUnitsAndEnum_map[iter].aEnum != enumNull
		)
		{
		if (rulerUnitsAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = rulerUnitsAndEnum_map[iter].rulerUnits;
			}
		
		++iter;
		}
		
	return result;
	}

DescriptorEnumID PIUApplication::MapRulerUnitsToEnum (const RulerUnits rulerUnits)
	{			
	DescriptorEnumID result = 0;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIURulerUnitsToEnum_max &&
		!foundResult &&
		rulerUnitsAndEnum_map[iter].aEnum != enumNull
		)
		{
		if (rulerUnitsAndEnum_map[iter].rulerUnits == rulerUnits)
			{
			foundResult = true;
			result = rulerUnitsAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}

RulerUnits PIUApplication::GetRulerUnits (void)
	{
	RulerUnits result = kRulerPixels;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyRulerUnits,
		typeRulerUnits
		);

	if (value != enumNull)
		{
		result = MapEnumToRulerUnits(value);
		}
	
	return result;
	}
			

double PIUApplication::GetPointsPerInch (void)
	{
	const double kTraditionalPointsPerInch = 72.27;
	const double kPostScriptPointsPerInch = 72.0;
	
	double result = 0;
	
	bool value = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyExactPoints
		);

	if (value)
		{
		result = kTraditionalPointsPerInch;
		}
	else
		{
		result = kPostScriptPointsPerInch;
		}
	
	return result;
	}
		
unsigned short PIUApplication::GetCacheLevels (void)
	{
	unsigned short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyNumberOfCacheLevels
		);
		
	result = (unsigned short)value;
	
	return result;
	}

bool PIUApplication::UseCacheForHistogram (void)
	{
	bool result = GetSingleBooleanProperty
		(
		0,	// Create descriptor from class.
		keyUseCacheForHistograms
		);

	return result;
	}	
			
//-----------------------------------------------------------------------
//	Utilities for Interpolation Method.
//-----------------------------------------------------------------------
const PIUInterpolationMethodToEnum_t interpolationAndEnum_map[] =
	{
	kPIUInterpolateNearestNeighbor,			enumNearestNeighbor,
	kPIUInterpolateBilinear, 				enumBilinear,
	kPIUInterpolateBicubic, 				enumBicubic,
	kPIUInvalidInterpolationMethod, 		enumNull 		// Must end with this.
	};

const unsigned short kPIUInterpolationMethodToEnum_max = 10;	// Safety for loop.

DescriptorEnumID PIUApplication::MapInterpolationToEnum
	(
	const PIUInterpolationMethod_t method
	)
	{
	DescriptorEnumID result = interpolationAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUInterpolationMethodToEnum_max &&
		!foundResult &&
		interpolationAndEnum_map[iter].method != kPIUInvalidInterpolationMethod
		)
		{
		if (interpolationAndEnum_map[iter].method == method)
			{
			foundResult = true;
			result = interpolationAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUInterpolationMethod_t PIUApplication::MapEnumToInterpolation
	(
	const DescriptorEnumID aEnum
	)
	{
	PIUInterpolationMethod_t result = kPIUInvalidInterpolationMethod;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUInterpolationMethodToEnum_max &&
		!foundResult &&
		interpolationAndEnum_map[iter].method != kPIUInvalidInterpolationMethod
		)
		{
		if (interpolationAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = interpolationAndEnum_map[iter].method;
			}
		
		++iter;
		}
		
	return result;
	}

PIUInterpolationMethod_t PIUApplication::GetInterpolationMethod (void)
	{
	PIUInterpolationMethod_t result = kPIUInvalidInterpolationMethod;

	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyInterpolationMethod,
		typeInterpolation
		);
		
	if (value != enumNull)
		{
		result = MapEnumToInterpolation(value);
		}
	
	return result;
	}

double PIUApplication::GetMajorGridUnits (void)
	{
	double result = 0.0;

	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyGridMajor
		);
		
	if (value != 0)
		{
		result = PIUFixed16ToDouble(value);
		}
	
	return result;
	}
	
SPErr PIUApplication::SetMajorGridUnits
	(
	/* IN */	const double inUnits
	)
	{
	long value = PIUDoubleToFixed16(inUnits);
	
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyGridMajor,
		value
		);
		
	return error;
	}
		
unsigned short PIUApplication::GetMinorGridUnits (void)
	{
	// Minor units are an integer number, not shifted
	// like major units.
	unsigned short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyGridMinor
		);
	
	result = (unsigned short)value;
	
	return result;
	}

SPErr PIUApplication::SetMinorGridUnits
	(
	/* IN */	const unsigned short inUnits
	)
	{
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyGridMinor,
		(long)inUnits
		);
	
	return error;
	}
		
//-----------------------------------------------------------------------
//	Utilities for Eyedropper sample.
//-----------------------------------------------------------------------
const PIUEyedropperSampleToEnum_t sampleAndEnum_map[] =
	{
	kPIUSamplePoint,				enumSamplePoint,
	kPIUSample3x3,					enumSample3x3,
	kPIUSample5x5,					enumSample5x5,
	kPIUInvalidEyedropperSample,	enumNull	// Must end with this.
	};
	
const unsigned short kPIUEyedropperSample_max = 10;	// Safety max.

DescriptorEnumID PIUApplication::MapEyedropperSampleToEnum
	(
	PIUEyedropperSample_t sample
	)
	{
	DescriptorEnumID result = sampleAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUEyedropperSample_max &&
		!foundResult &&
		sampleAndEnum_map[iter].sample != kPIUInvalidEyedropperSample
		)
		{
		if (sampleAndEnum_map[iter].sample == sample)
			{
			foundResult = true;
			result = sampleAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUEyedropperSample_t PIUApplication::MapEnumToEyedropperSample
	(
	DescriptorEnumID aEnum
	)
	{
	PIUEyedropperSample_t result = kPIUInvalidEyedropperSample;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUEyedropperSample_max &&
		!foundResult &&
		sampleAndEnum_map[iter].sample != kPIUInvalidEyedropperSample
		)
		{
		if (sampleAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = sampleAndEnum_map[iter].sample;
			}
		
		++iter;
		}
		
	return result;
	}

PIUEyedropperSample_t PIUApplication::GetEyedropperSample (void)
	{
	PIUEyedropperSample_t result = kPIUInvalidEyedropperSample;
	
	DescriptorEnumID value = GetSingleEnumeratedProperty
		(
		0,	// Create descriptor from class.
		keyEyeDropperSample,
		typeEyeDropperSample
		);
		
	if (value != enumNull)
		{
		result = MapEnumToEyedropperSample((DescriptorEnumID)value);
		}
	
	return result;
	}
	
SPErr PIUApplication::SetEyedropperSample
	(
	/* IN */	const PIUEyedropperSample_t	inSample
	)
	{
	SPErr error = kSPBadParameterError;
	
	if (inSample != kPIUInvalidEyedropperSample)
		{
		DescriptorEnumID inEnum = MapEyedropperSampleToEnum(inSample);
		
		error = SetSingleEnumeratedProperty
			(
			0,	// Create descriptor from class.
			keyEyeDropperSample,
			typeEyeDropperSample,
			inEnum
			);
		}
			
	return error;
	}

//-----------------------------------------------------------------------
//	Color Picker preferences.
//-----------------------------------------------------------------------
const PIUColorPickerKindToEnum_t colorPickerKindAndEnum_map [] =
	{
	kPIUSystemPicker,				enumSystemPicker,
	kPIUPhotoshopPicker,			enumPhotoshopPicker,
	kPIUPlugInPicker,				enumPluginPicker,
	kPIUInvalidColorPickerKind,		enumNull	// Must end with this.
	};

const unsigned short kPIUColorPickerKind_max = 10;	// Safety max.

DescriptorEnumID PIUApplication::MapColorPickerKindToEnum
	(
	PIUColorPickerKind_t kind
	)
	{
	DescriptorEnumID result = colorPickerKindAndEnum_map[0].aEnum;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUColorPickerKind_max &&
		!foundResult &&
		colorPickerKindAndEnum_map[iter].kind != kPIUInvalidColorPickerKind
		)
		{
		if (colorPickerKindAndEnum_map[iter].kind == kind)
			{
			foundResult = true;
			result = colorPickerKindAndEnum_map[iter].aEnum;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUColorPickerKind_t PIUApplication::MapEnumToColorPickerKind
	(
	DescriptorEnumID aEnum
	)
	{
	PIUColorPickerKind_t result = kPIUInvalidColorPickerKind;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUColorPickerKind_max &&
		!foundResult &&
		colorPickerKindAndEnum_map[iter].kind != kPIUInvalidColorPickerKind
		)
		{
		if (colorPickerKindAndEnum_map[iter].aEnum == aEnum)
			{
			foundResult = true;
			result = colorPickerKindAndEnum_map[iter].kind;
			}
		
		++iter;
		}
		
	return result;
	}

PIUColorPickerKind_t PIUApplication::GetColorPickerKind (void)
	{
	PIUColorPickerKind_t result = kPIUInvalidColorPickerKind;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyColorPickerPrefs,
		classColorPickerPrefs
		);
		
	if (descriptor != 0)
		{
		DescriptorEnumID value = GetSingleEnumeratedProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyPickerKind,
			typePickerKind
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		
		if (value != enumNull)
			{
			result = MapEnumToColorPickerKind(value);
			}
		}
	
	return result;
	}

SPErr PIUApplication::SetColorPickerKind
	(
	/* IN */	const PIUColorPickerKind_t inKind
	)
	{
	SPErr error = kSPBadParameterError;
	
	if (inKind != kPIUInvalidColorPickerKind)
		{
		// First we'll get the descriptor that's already got
		// the properties:
		PIActionDescriptor descriptor = GetSingleObjectProperty
			(
			0,	// Create descriptor from class.
			keyColorPickerPrefs,
			classColorPickerPrefs
			);
			
		if (descriptor != 0)
			{
			// This should replace the key with our new value in
			// the same descriptor.  We won't use SetSingleIntegerProperty
			// because that also tries to do an eventSet, which we're
			// not ready for yet:
			DescriptorEnumID inEnum = MapColorPickerKindToEnum(inKind);
			
			error = sPSActionDescriptor->PutEnumerated
				(
				descriptor,	// Use descriptor we just retrieved.
				keyPickerKind,
				typePickerKind,
				inEnum
				);
				
			if (error == kSPNoError)
				{
				// Now that we've replaced it, store this object
				// back in a descriptor:
				error = SetSingleObjectProperty
					(
					0,	// Create descriptor from class.
					keyColorPickerPrefs,
					classColorPickerPrefs,
					descriptor
					);
				}
								
			// Destroy the descriptor, since it is ours:
			if (sPSActionDescriptor.IsValid())
				sPSActionDescriptor.Free(&descriptor);
			}
	
		} // inKind
			
	return error;
	}

// It is the caller's responsibility to free this memory with
// FreeBlock if anything non-null is returned:
char* PIUApplication::GetColorPickerID (void)
	{
	char* result = NULL;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyColorPickerPrefs,
		classColorPickerPrefs
		);
		
	if (descriptor != 0 && sPSActionDescriptor.IsValid())
		{
		result = sPSActionDescriptor.GetString
			(
			descriptor, 
			keyPickerID
			);
			
		// Destroy the descriptor.  Its our responsibility:
		sPSActionDescriptor.Free(&descriptor);
		}
	
	return result;
	}
	
SPErr PIUApplication::SetColorPickerID
	(
	/* IN */	const char* const inID
	)
	{
	SPErr error = kSPBadParameterError;
	
	if (inID != NULL)
		{
		// First we'll get the descriptor that's already got
		// the properties:
		PIActionDescriptor descriptor = GetSingleObjectProperty
			(
			0,	// Create descriptor from class.
			keyColorPickerPrefs,
			classColorPickerPrefs
			);
			
		if (descriptor != 0)
			{
			// This should replace the key with our new value in
			// the same descriptor.  We won't use SetSingleIntegerProperty
			// because that also tries to do an eventSet, which we're
			// not ready for yet.
			// Also, since we're setting an ID, that must mean that we're
			// asking for a plug-in color picker, so set kind to that:
			DescriptorEnumID inEnum = MapColorPickerKindToEnum(kPIUPlugInPicker);
			
			error = sPSActionDescriptor->PutEnumerated
				(
				descriptor,	// Use descriptor we just retrieved.
				keyPickerKind,
				typePickerKind,
				inEnum
				);
				
			if (error == kSPNoError)
				{
				error = sPSActionDescriptor->PutString
					(
					descriptor,
					keyPickerID,
					(char*)inID
					);
				
				if (error == kSPNoError)
					{
					// Now that we've replaced it, store this object
					// back in a descriptor:
					error = SetSingleObjectProperty
						(
						0,	// Create descriptor from class.
						keyColorPickerPrefs,
						classColorPickerPrefs,
						descriptor
						);
					}
				
				} // PutEnum
								
			// Destroy the descriptor, since it is ours:
			if (sPSActionDescriptor.IsValid())
				sPSActionDescriptor.Free(&descriptor);
			
			} // descriptor
	
		} // inKind
			
	return error;
	}
	
//-----------------------------------------------------------------------
//	History preferences.
//-----------------------------------------------------------------------			
// These preferences really make more sense as part of the "History"
// class, but since they're application preferences, they appear as
// part of the application object.  This should probably change for
// the future...

unsigned short PIUApplication::GetMaximumHistoryEntries (void)
	{
	unsigned short result = 0;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		long value = GetSingleIntegerProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyMaximumStates
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		
		if (value != 0)
			{
			result = (unsigned short)value;
			}
		}
	
	return result;
	}

SPErr PIUApplication::SetMaximumHistoryEntries
	(
	/* IN */	const unsigned short inNumEntries
	)
	{
	SPErr error = kSPNoError;
	
	// First we'll get the descriptor that's already got
	// the properties:
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		// This should replace the key with our new value in
		// the same descriptor.  We won't use SetSingleIntegerProperty
		// because that also tries to do an eventSet, which we're
		// not ready for yet:
		error = sPSActionDescriptor->PutInteger
			(
			descriptor,	// Use descriptor we just retrieved.
			keyMaximumStates,
			(long)inNumEntries
			);
			
		if (error == kSPNoError)
			{
			// Now that we've replaced it, store this object
			// back in a descriptor:
			error = SetSingleObjectProperty
				(
				0,	// Create descriptor from class.
				keyHistoryPrefs,
				classHistoryPrefs,
				descriptor
				);
			}
							
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		}
	
	return error;
	}
	
bool PIUApplication::StartHistoryWithSnapshot (void)
	{
	bool result = false;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		result = GetSingleBooleanProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keySnapshotInitial
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);		
		}
	
	return result;
	}

SPErr PIUApplication::SetStartHistoryWithSnapshot
	(
	/* IN */	const bool inStartWithSnap
	)
	{
	SPErr error = kSPNoError;
	
	// First we'll get the descriptor that's already got
	// the properties:
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		// This should replace the key with our new value in
		// the same descriptor.  We won't use SetSingleIntegerProperty
		// because that also tries to do an eventSet, which we're
		// not ready for yet:
		Boolean value = FALSE;
		
		if (inStartWithSnap)
			value = TRUE;
			
		error = sPSActionDescriptor->PutBoolean
			(
			descriptor,	// Use descriptor we just retrieved.
			keySnapshotInitial,
			value
			);
			
		if (error == kSPNoError)
			{
			// Now that we've replaced it, store this object
			// back in a descriptor:
			error = SetSingleObjectProperty
				(
				0,	// Create descriptor from class.
				keyHistoryPrefs,
				classHistoryPrefs,
				descriptor
				);
			}
							
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		}
	
	return error;
	}

bool PIUApplication::AllowNonLinearHistory (void)
	{
	bool result = false;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		result = GetSingleBooleanProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyNonLinear
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);		
		}
	
	return result;
	}

SPErr PIUApplication::SetAllowNonLinearHistory
	(
	/* IN */	const bool inAllowNonLinear
	)
	{
	SPErr error = kSPNoError;
	
	// First we'll get the descriptor that's already got
	// the properties:
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHistoryPrefs,
		classHistoryPrefs
		);
		
	if (descriptor != 0)
		{
		// This should replace the key with our new value in
		// the same descriptor.  We won't use SetSingleIntegerProperty
		// because that also tries to do an eventSet, which we're
		// not ready for yet:
		Boolean value = FALSE;
		
		if (inAllowNonLinear)
			value = TRUE;
			
		error = sPSActionDescriptor->PutBoolean
			(
			descriptor,	// Use descriptor we just retrieved.
			keyNonLinear,
			value
			);
			
		if (error == kSPNoError)
			{
			// Now that we've replaced it, store this object
			// back in a descriptor:
			error = SetSingleObjectProperty
				(
				0,	// Create descriptor from class.
				keyHistoryPrefs,
				classHistoryPrefs,
				descriptor
				);
			}
							
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);
		}
	
	return error;
	}

//-----------------------------------------------------------------------
//	Application properties.
//-----------------------------------------------------------------------			

// It is the caller's responsibility to free any non-null pointers
// returned from here with FreeBlock():
char* PIUApplication::GetSerialNumber (void)
	{
	char* result = GetSingleStringProperty
		(
		0,	// Create descriptor from class.
		keySerialString
		);
		
	return result;
	}
	
unsigned short PIUApplication::GetWatchSuspension (void)
	{
	unsigned short result = 0;
	
	long value = GetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyWatchSuspension
		);
		
	if (value != 0)
		{
		result = (unsigned short)value;
		}
		
	return result;
	}
	
SPErr PIUApplication::SetWatchSuspension
	(
	/* IN */	const unsigned short inValue
	)
	{
	SPErr error = SetSingleIntegerProperty
		(
		0,	// Create descriptor from class.
		keyWatchSuspension,
		(long)inValue
		);
	
	return error;
	}

long PIUApplication::GetMajorVersion (void)
	{
	long result = 0;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHostVersion,
		classVersion
		);
		
	if (descriptor != 0)
		{
		result = GetSingleIntegerProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyVersionMajor
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);		
		}
	
	return result;
	}

long PIUApplication::GetMinorVersion (void)
	{
	long result = 0;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHostVersion,
		classVersion
		);
		
	if (descriptor != 0)
		{
		result = GetSingleIntegerProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyVersionMinor
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);		
		}
	
	return result;
	}
	
long PIUApplication::GetFixVersion (void)
	{
	long result = 0;
	
	PIActionDescriptor descriptor = GetSingleObjectProperty
		(
		0,	// Create descriptor from class.
		keyHostVersion,
		classVersion
		);
		
	if (descriptor != 0)
		{
		result = GetSingleIntegerProperty
			(
			descriptor,	// Use descriptor we just retrieved.
			keyVersionFix
			);
			
		// Destroy the descriptor, since it is ours:
		if (sPSActionDescriptor.IsValid())
			sPSActionDescriptor.Free(&descriptor);		
		}
	
	return result;
	}

char* PIUApplication::GetVersionAsString (void)
	{
	char* result = NULL;
	
	if (sADMBasic.IsValid() && sSPBasic.IsValid())
		{
		const char* output = "^0.^1.^2";
		
		result = PIUCopyStringToBlock
			(
			output,
			PIUstrlen(output)
			);
			
		if (result != NULL)
			{
			// Need a small string for conversion:
			char convertString[kConvertStrLen] = "";
			
			sADMBasic->ValueToString
				(
				(float)GetMajorVersion(),
				convertString,
				kConvertStrLen,
				kADMNoUnits,
				kPIUNoPrecision,
				false				// Always append units?
				);					
			
			SPErr error = PIUReplaceCharsInBlock
				(
				"^0",
				convertString,
				kIgnoreCase,
				&result
				);
				
			if (error == kSPNoError)
				{
				sADMBasic->ValueToString
					(
					(float)GetMinorVersion(),
					convertString,
					kConvertStrLen,
					kADMNoUnits,
					kPIUNoPrecision,
					false				// Always append units?
					);					
				
				error = PIUReplaceCharsInBlock
					(
					"^1",
					convertString,
					kIgnoreCase,
					&result
					);
				
				if (error == kSPNoError)
					{
					sADMBasic->ValueToString
						(
						(float)GetFixVersion(),
						convertString,
						kConvertStrLen,
						kADMNoUnits,
						kPIUNoPrecision,
						false				// Always append units?
						);					
					
					SPErr error = PIUReplaceCharsInBlock
						(
						"^2",
						convertString,
						kIgnoreCase,
						&result
						);
						
					} // fix
				} // minor
			} // major
		} // admbasic && spbasic
		
	return result;
	
	} // end GetVersionAsString
	
//-----------------------------------------------------------------------
//	Interface Colors.
//-----------------------------------------------------------------------
const PIUInterfaceColorToKey_t interfaceColorAndKey_map [] =
	{
	kPIUInterfaceWhite,					keyInterfaceWhite,
	kPIUInterfaceButtonUpFill,			keyInterfaceButtonUpFill,
	kPIUInterfaceBevelShadow,			keyInterfaceBevelShadow,
	kPIUInterfaceIconFillActive,		keyInterfaceIconFillActive,
	kPIUInterfaceIconFillDimmed,		keyInterfaceIconFillDimmed,
	kPIUInterfacePaletteFill,			keyInterfacePaletteFill,
	kPIUInterfaceIconFrameDimmed,		keyInterfaceIconFrameDimmed,
	kPIUInterfaceIconFrameActive,		keyInterfaceIconFrameActive,
	kPIUInterfaceBevelHighlight,		keyInterfaceBevelHighlight,
	kPIUInterfaceButtonDownFill,		keyInterfaceButtonDownFill,
	kPIUInterfaceIconFillSelected,		keyInterfaceIconFillSelected,
	kPIUInterfaceBorder,				keyInterfaceBorder,
	kPIUInterfaceButtonDarkShadow,		keyInterfaceButtonDarkShadow,
	kPIUInterfaceIconFrameSelected,		keyInterfaceIconFrameSelected,
	kPIUInterfaceRed,					keyInterfaceRed,
	kPIUInterfaceBlack,					keyInterfaceBlack,
	kPIUInterfaceToolTipBackground,		keyInterfaceToolTipBackground,
	kPIUInterfaceToolTipText,			keyInterfaceToolTipText,
	kPIUInvalidInterfaceColor,			keyNull	// Must end with this.
	};

const unsigned short kPIUInterfaceColor_max = 30;	// Safety max.

DescriptorKeyID PIUApplication::MapInterfaceColorToKey
	(
	PIUInterfaceColor_t color
	)
	{
	DescriptorEnumID result = interfaceColorAndKey_map[0].aKey;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUInterfaceColor_max &&
		!foundResult &&
		interfaceColorAndKey_map[iter].color != kPIUInvalidInterfaceColor
		)
		{
		if (interfaceColorAndKey_map[iter].color == color)
			{
			foundResult = true;
			result = interfaceColorAndKey_map[iter].aKey;
			}
		
		++iter;
		}
		
	return result;
	}
	
PIUInterfaceColor_t PIUApplication::MapKeyToInterfaceColor
	(
	DescriptorKeyID aKey
	)
	{
	PIUInterfaceColor_t result = kPIUInvalidInterfaceColor;
	
	unsigned short iter = 0;
	bool foundResult = false;
	
	while 
		(
		iter < kPIUInterfaceColor_max &&
		!foundResult &&
		interfaceColorAndKey_map[iter].color != kPIUInvalidInterfaceColor
		)
		{
		if (interfaceColorAndKey_map[iter].aKey == aKey)
			{
			foundResult = true;
			result = interfaceColorAndKey_map[iter].color;
			}
		
		++iter;
		}
		
	return result;
	}

RGB16tuple* PIUApplication::GetInterfaceColorsInDepth16 (void)
	{
	RGB16tuple* result = NULL;
	
	PIActionDescriptor descriptor = GetAllProperties();
	
	if (descriptor != 0 && sPSActionDescriptor.IsValid() && sSPBasic.IsValid())
		{
		// First lets just count how many colors we have:
		unsigned short iter = 0;
		
		while 
			(
			iter < kPIUInterfaceColor_max &&
			interfaceColorAndKey_map[iter].color != kPIUInvalidInterfaceColor
			)
			{
			++iter;
			}
			
		const unsigned short total = iter + 1;	// Add one for empty first slot.
		
		const unsigned short size = total * sizeof(RGB16tuple);
		
		SPErr error = sSPBasic->AllocateBlock
			(
			size,
			(void**)&result
			);
			
		if (result != NULL && error == kSPNoError)
			{
			// We could allocate memory, so do this.
			DescriptorKeyID thisKey = keyNull;
			PIActionDescriptor color = NULL;
			long value = 0;
			RGB16tuple thisRGB = {0, 0, 0, 0};
			
			// Leave first slot empty so there is a one-to-one correspondence between
			// this structure and PIUInterfaceColor_t:
			result[0] = thisRGB;
			
			for (iter = 0; iter < total; iter++)
				{
				thisKey = interfaceColorAndKey_map[iter].aKey;
				color = GetSingleObjectProperty
					(
					descriptor,	// Use descriptor with all properties in it.
					thisKey,
					classInterfaceColor
					);
				
				if (color != 0)
					{
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorRed32
						);
						
					thisRGB.r = (unsigned16)value;
					
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorGreen32
						);
						
					thisRGB.g = (unsigned16)value;
					
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorBlue32
						);
						
					thisRGB.b = (unsigned16)value;
					
					thisRGB.alpha = 0;
					
					sPSActionDescriptor.Free(&color);

					result[1+iter] = thisRGB;	// First slot left empty.
					
					} // color
				} // for
			} // result
		} // descriptor, SPBasic

	return result;
	} // end GetInterfaceColorsInDepth16

RGBtuple* PIUApplication::GetInterfaceColorsInDepth8 (void)
	{
	RGBtuple* result = NULL;
	
	PIActionDescriptor descriptor = GetAllProperties();
	
	if (descriptor != 0 && sPSActionDescriptor.IsValid() && sSPBasic.IsValid())
		{
		// First lets just count how many colors we have:
		unsigned short iter = 0;
		
		while 
			(
			iter < kPIUInterfaceColor_max &&
			interfaceColorAndKey_map[iter].color != kPIUInvalidInterfaceColor
			)
			{
			++iter;
			}
			
		const unsigned short total = iter + 1;	// Add one for empty first slot.
		
		const unsigned short size = total * sizeof(RGB16tuple);
		
		SPErr error = sSPBasic->AllocateBlock
			(
			size,
			(void**)&result
			);
			
		if (result != NULL && error == kSPNoError)
			{
			// We could allocate memory, so do this.
			DescriptorKeyID thisKey = keyNull;
			PIActionDescriptor color = NULL;
			long value = 0;
			RGBtuple thisRGB = {0, 0, 0, 0};
			
			// Leave first slot empty so there is a one-to-one correspondence between
			// this structure and PIUInterfaceColor_t:
			result[0] = thisRGB;
			
			for (iter = 0; iter < total; iter++)
				{
				thisKey = interfaceColorAndKey_map[iter].aKey;
				color = GetSingleObjectProperty
					(
					descriptor,	// Use descriptor with all properties in it.
					thisKey,
					classInterfaceColor
					);
				
				if (color != 0)
					{
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorRed2
						);
						
					thisRGB.r = (unsigned8)value;
					
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorGreen2
						);
						
					thisRGB.g = (unsigned8)value;
					
					value = GetSingleIntegerProperty
						(
						color,
						keyInterfaceColorBlue2
						);
						
					thisRGB.b = (unsigned8)value;
					
					thisRGB.alpha = 0;
					
					sPSActionDescriptor.Free(&color);

					result[1+iter] = thisRGB;	// First slot left empty.
					
					} // color
				} // for
			} // result
		} // descriptor, SPBasic

	return result;
	} // end GetInterfaceColorsInDepth8
		

			
//-------------------------------------------------------------------------------
//
//	class PIUBasic
//
//	This is the main suite that provides all the Plug-in utility suites.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//
//	IPIUBasic.
//
//	Lazy initialization routine.  This routine is called each time the
//	PIUSuites initialization is run, since suites aren't instantiated until
//	very late in the cycle (so the init code is called a few times).  This
//	code usually is safe by the second iteration, so we'll put a count on
//	it and watch that:
//
//-------------------------------------------------------------------------------
void PIUBasic::IPIUBasic (void)
	{
	static unsigned short count = 2;	// Max number of inits.

	if (count > 0)
		{
		--count;
		
		// Lazy init individual elements:
		Application()->IElement();
		
		// Document may be logically part of application, but
		// don't pass &application_ because references with it as
		// part of a longer path don't work:
		Document()->IElement();
		
		Layer()->IElement(&document_);
		Channel()->IElement();
		
		// Path is logically part of a document, but a reference
		// to "path foo of document bar" fails. It must be the
		// current document:
		Path()->IElement();
		
		// History is logically part of a document, but a 
		// reference to "history foo of document bar" fails. It
		// must be the current document:
		History()->IElement();
		
		ActionSet()->IElement();
		Action()->IElement(&actionSet_);
		
		// Filter() does not need initializing.
		}
	}			
//-------------------------------------------------------------------------------

// end PIUCore.cpp

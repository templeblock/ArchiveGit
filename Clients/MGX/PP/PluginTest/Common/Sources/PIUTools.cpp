//-------------------------------------------------------------------------------
//
//	File:
//		PIUTools.cpp
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

#include "PIUTools.h"
#include "PIUSuites.h" // Have to include it here to prevent cyclical includes.

//-------------------------------------------------------------------------------
//
//	PIUMakeLower
//	
//	Changes an uppercase character to lowercase.
//
//	Inputs:
//		const char inChar			Character to convert.
//
//	Outputs:
//		char						If between A and Z, the lowercase
//									equivalent.  Otherwise, the original
//									character.
//
//-------------------------------------------------------------------------------

char PIUMakeLower(const char inChar)
{
	char	outChar = inChar;
	
	if (inChar >= 'A' && inChar <= 'Z')
		outChar = (inChar - 'A') + 'a';

	return outChar;
	
} // end PIUMakeLower
	
//-------------------------------------------------------------------------------
//
//	PIUstrlen
//	
//	Returns the length of a null-terminated string.  There are many functions
//	like this one present that are duplicates of the ANSI string library so that
//	most basic functionality is present without including the library.
//
//	Inputs:
//		const char *inString			Pointer to string to check length.
//
//	Outputs:
//		returns short					Length of string up to NULL byte,
//										or 32767 (MAX_INT).
//
//-------------------------------------------------------------------------------

unsigned short PIUstrlen (const char *inString)
	{
		unsigned short	outLength = 0;
	
		if (inString != NULL)
			{	
			while (inString[outLength] != 0 && outLength < USHRT_MAX /* Defined in <limits.h> */)
				{
				outLength++;
				}
			}
					
		return outLength;
		
	} // end PIUstrlen

//-------------------------------------------------------------------------------
//
//	_PIUMatch
//	
//	Finds an occurrence of a target string in a source string.  Note that
//	the target has to match exactly with the source string.  If the target
//	starts in the middle of the source string, HostMatch will return the
//	character position.  Otherwise it will return the constant noMatch
//	or gotMatch.
//
//	Inputs:
//		const char *inSearch		Source C-string to search.
//
//		const char *inTarget		Target C-string to search for.
//
//		const Boolean ignoreCase	TRUE if you want a case-insensitive
//									search; FALSE if you want exact.
//
//		const unsigned short inLength		Maximum number of chars to check.
//									Should not be over 256.
//
//	Outputs:
//		returns kNoMatch			If could not find exact match.
//
//		returns kGotMatch			If an exact match was found.
//
//		returns unsigned short				Exact match was found, from char
//									outMatch to end of string.
//
//-------------------------------------------------------------------------------

short _PIUMatch
	(
	const char* inSearch,
	const char* inTarget,
	const caseSensitive_t ignoreCase,
	const unsigned short inLength
	)
	{
	const unsigned short	targetSize = PIUstrlen(inTarget);
	
	unsigned short			loop = 0;
	unsigned short			start = 0;

	
	short	outMatch = kNoMatch; // -1 = no match; 0 = got match

	while
		(
		inSearch[loop] != '\0' && 
		inTarget[start] != '\0' &&
		loop <= inLength &&
		start <= targetSize
		)
		{
		if (inSearch[loop] == inTarget[start] || 
		   ((ignoreCase == kIgnoreCase) &&
		   (PIUMakeLower(inSearch[loop]) == PIUMakeLower(inTarget[start]))))
			{ // we found a match, this is the character it started at:
			if (outMatch < 0)
				outMatch = loop;
				
			++start;
			
			}
		else
			{ // reset:
			outMatch = kNoMatch;
			start = 0;
			}

			++loop;

		} // while

	return outMatch;
	
	} // end PIUMatch	

//-------------------------------------------------------------------------------
//
//	_PIUStringMatch
//	
//	Finds an occurrence of a target string in a source string.  Note that
//	the target has to match exactly with the source string.  If the target
//	starts in the middle of the source string, HostMatch will return the
//	character position.  Otherwise it will return the constant noMatch
//	or gotMatch.
//
//	This routine is the same as HostMatch, except the search is done
//	on a pascal string.  The pascal string must have a char free so
//	it can be NULL terminated, otherwise this routine will always
//	return noMatch.
//
//	Inputs:
//		const char *inSearch		Source pascal string to search.
//
//		const char *inTarget		Target C-string to search for.
//
//		const Boolean ignoreCase	TRUE if you want a case-insensitive
//									search; FALSE if you want exact.
//
//	Outputs:
//		returns kNoMatch			If could not find exact match.
//
//		returns kGotMatch			If an exact match was found.
//
//		returns short				Exact match was found, from char
//									outMatch to end of string.
//
//-------------------------------------------------------------------------------

short _PIUStringMatch
	(
	Str255 inSearch, 
	const char* inTarget,
	const caseSensitive_t ignoreCase
	)
	{
	if (inSearch[0] < 256) 
		inSearch[ inSearch[0]+1 ] = '\0';

	return _PIUMatch
		(
		(char *)&inSearch[1], 
		inTarget, 
		ignoreCase, 
		inSearch[0]
		);

	} // end HostStringMatch

//-------------------------------------------------------------------------------
//
//	PIUIDToChar
//	
//	Converts an uint32 ResType to its corresponding 4 characters.
//
//	Inputs:
//		const ResType inType		uint32 ResType to convert.
//
//	Outputs:
//		char *outChars				4 characters representing the ResType.
//									Make sure your pointer points to
//									an array of at least 4 characters or
//									this will stomp a little memory (4 bytes).
//
//-------------------------------------------------------------------------------

void PIUIDToChar (const unsigned long inType, /* OUT */ char *outChars)
{
	int8 loopType, loopChar;
	
	// Figure out which byte we're dealing with and move it
	// to the highest character, then mask out everything but
	// the lowest byte and assign that as the character:
	for (loopType = 0, loopChar = 3; loopType < 4; loopType++, loopChar--)
		outChars[loopChar] = (char) ( (inType >> (loopType<<3)) & 0xFF );
	
	outChars[loopType] = '\0';

} // end PIUIDToChar

//-------------------------------------------------------------------------------
//
//	PIUCharToID
//	
//	Convert 4 characters to a corresponding uint32 ResType.
//
//	Inputs:
//		const char *inChars			Pointer to 4 characters to convert.
//
//	Outputs:
//		returns ResType				Unsigned int32 (long) corresponding
//									to the 4 characters.
//
//-------------------------------------------------------------------------------

unsigned long PIUCharToID (const char *inChars)
{
	unsigned short loop;
	ResType outType = 0;
	
	for (loop = 0; loop < 4; loop++)
	{
		outType <<= 8; // bits per char
		outType |= inChars[loop];
	}

	return outType;

} // end PIUCharToID

//-------------------------------------------------------------------------------
//
//	power
//	
//	Raises a given base to a certain power.
//
//-------------------------------------------------------------------------------
double PIUExponent
	(
	/* IN */	const unsigned long base, 
	/* IN */	const unsigned short raise
	)
	{
	/* OUT */	double power = 1;
	
	for (register long loop = 0; loop < raise; loop++)
		power *= base;

	return power;
	}

//-------------------------------------------------------------------------------
//
//	PIUDoubleToCString
//	
//	Converts a double to a CString.
//
//-------------------------------------------------------------------------------
SPErr PIUDoubleToCString
	(
	/* IN */	const double value,
	/* IN */	const unsigned char precision,
	/* OUT */	char outString[kConvertStrLen]
	)
	{
	SPErr error = kSPNoError;
	
	if (outString != NULL)
		{
	
		double 			inValue = value;
		unsigned char	inPrecision = precision;
		bool			negative = (value < 0);
		
		if (negative)
			inValue = -value; // get rid of negative number
		
		// determine if we need to round:
		double work = inValue * PIUExponent(10, inPrecision);
		
		unsigned long nonSignedNumber = (unsigned long)work; // whole portion;
		work -= nonSignedNumber; // subtract whole portion
		work *= 10; // move over one more place
		
		// Round:
		if (work >= 5)
			inValue += (5.0 / (double)PIUExponent(10, (short)(inPrecision+1)));
		
		if (inPrecision > 1)
			{ // now check to see if we can lose a decimal place

			work = inValue * PIUExponent(10, (short)(inPrecision-1));

			nonSignedNumber = (unsigned long)work; // whole portion
			work -= nonSignedNumber; // subtract whole portion
			work *= 10; // number of interest

			// our number of interest is zero, so lose a point of precision:
			if (work < 1)
				inPrecision--;
				
			} // inPrecision
		
		work = 0; // done with this
		
		if (negative)
			nonSignedNumber = (unsigned long)-inValue; // whole number portion
		else
			nonSignedNumber = (unsigned long)inValue;

		// Convert basic number:
		
		PIULongToCString((long)nonSignedNumber, outString);
		unsigned short length = PIUstrlen(outString);
		
		outString[length++] = '.'; // Add decimal point.
		
		inValue -= (unsigned long)inValue; // get rid of whole-number portion

		do
			{ // add each character for the decimal values:

			inValue *= 10;
			nonSignedNumber = (unsigned long)inValue; // whole number
			outString[length++] = '0' + (char)(nonSignedNumber);
			inValue -= nonSignedNumber;

			} while (inValue > 0 && --inPrecision > 0 && length < kConvertStrLen);
		
		PIUNullTerminate(&outString[length]);
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	
	}



//-------------------------------------------------------------------------------
//
//	PIULongToCString
//	
//	Converts a signed long to a CString.
//
//-------------------------------------------------------------------------------
SPErr PIULongToCString
	(
	/* IN */	const long value,
	/* OUT */	char outString[kConvertStrLen]
	)
	{
	
	SPErr error = kSPNoError;
	
	if (outString != NULL)
		{
	 	PIUNullTerminate(outString);
	 	
		#ifdef __PIMac__
		
			Str255 pascalString = "";
			
			NumToString(value, pascalString);
			
			if (pascalString[0] >= kConvertStrLen)
				pascalString[0] = kConvertStrLen-1;
			
			PIUCopy(outString, &pascalString[1], (unsigned short)pascalString[0]);
			PIUNullTerminate(&outString[pascalString[0]]);
		
		#elif defined(__PIWin__)
		
			ltoa(value, outString, 10 /* base 10 */);
		
		#endif
		
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	}	
	

//-------------------------------------------------------------------------------
//
//	PIUCountHits.
//
//	Returns number of occurrences of target in source.
//	
//-------------------------------------------------------------------------------
unsigned short PIUCountHits 
	(
	/* IN */	const char* const source,
	/* IN */	const char* const target,
	/* IN */	const caseSensitive_t ignoreCase
	)
	{
	/* OUT */	unsigned short numberOfHits = 0;
	
	if (source != NULL && target != NULL)
		{
		const unsigned short source_length = PIUstrlen(source);
		const unsigned short target_length = PIUstrlen(target);
		unsigned short currentPosition = 0;
		short foundPosition = 0;
		
		if (source_length > 0 && target_length > 0)
			{			
			while (foundPosition != kNoMatch && currentPosition < source_length)
				{
				foundPosition = _PIUMatch
					(
					&source[currentPosition],
					target,
					ignoreCase,
					source_length
					);			
				
				if (foundPosition != kNoMatch)
					{
					++numberOfHits;
					// Okay, we found one.  Lets skip past it and keep looking:
					currentPosition += foundPosition + target_length;
					}	
				}
			}
		}
		
	return numberOfHits;
	
	} // PIUCountHits
	
//-------------------------------------------------------------------------------
//
//	PIUCopy
//	
//	Copies bytes from a source to a target.
//
//-------------------------------------------------------------------------------
SPErr PIUCopy
	(
	/* OUT */	void* target,
	/* IN */	const void* const source,
	/* IN */	const unsigned short length
	)
	{
	SPErr error = kSPNoError;
	
	if (target != NULL && source != NULL)
		{
		for (register unsigned short loop = 0; loop < length; loop++)
			*((char*)target+loop) = *((char*)source+loop);
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	}
	
//-------------------------------------------------------------------------------
//
//	PIUAppendBlock
//	
//	Replaces one occurrence and returns the next occurrence.
//
//-------------------------------------------------------------------------------

SPErr PIUAppendBlock
	(
	/* IN/OUT */	char* block,
	/* IN */		const char* const append,
	/* IN */		const unsigned short append_size
	)
	{
	
	SPErr error = kSPNoError;
	
	if (block != NULL && append != NULL && append_size > 0)
		{
		const unsigned short block_size = PIUstrlen(block);
		
		PIUCopy(&(block[block_size]), append, append_size);
		PIUNullTerminate(&block[block_size+append_size]);
		}
	else
		{
		error = kSPBadParameterError;
		}
	
	return error;
	
	}

//-------------------------------------------------------------------------------
//
//	PIUReplaceCharsInBlock
//	
//	Replaces all occurrences of target with replace in a block of characters.
//
//-------------------------------------------------------------------------------

SPErr PIUReplaceCharsInBlock
	(
	/* IN */		const char* const target,
	/* IN */		const char* const replace,
	/* IN */		const caseSensitive_t ignoreCase,
	/* IN/OUT */	char** block
	)
	{
	SPErr error = kSPNoError;
	
	if (sSPBasic.IsValid() && 
		block != NULL
		)
		{ // Have to have basic suite and normal incoming params.
		
		//-----------------------------------------------------------------------
		// (1) First walk through block and determine how many occurrences
		// 	   of source are there.
		//-----------------------------------------------------------------------
		const unsigned short block_size = PIUstrlen(*block);
		const unsigned short target_size = PIUstrlen(target);			
		const unsigned short replace_size = PIUstrlen(replace);
		const unsigned short numberOfHits = PIUCountHits
			(
			*block, 
			target, 
			ignoreCase
			);
			
		if (numberOfHits > 0)
			{
			//---------------------------------------------------------------
			// (2) Now determine and allocate new block with new size.
			//---------------------------------------------------------------
		
			const unsigned short newBlock_size = 
				block_size
				- (target_size * numberOfHits)
				+ (replace_size * numberOfHits)
				+ 1; // For trailing null.
				
			char* newBlock = NULL;				
			error = sSPBasic->AllocateBlock(newBlock_size, (void**)&newBlock);
			
			if (error == kSPNoError && newBlock != NULL)
				{
				//-----------------------------------------------------------
				// (3) Walk through source again, this time copying it and
				//	   replacements to new block.
				//-----------------------------------------------------------
				
				PIUNullTerminate(newBlock); // Gotta do this for append to work.
				
				unsigned short currentPosition = 0;
				short foundPosition = 0;
				short lastPosition = 0;
		
				while (foundPosition != kNoMatch && currentPosition < block_size)
					{
					foundPosition = _PIUMatch
						(
						&((*block)[currentPosition]),
						target,
						ignoreCase,
						block_size - currentPosition
						);

					if (foundPosition != kNoMatch)
						{
						if (foundPosition > 0)
							{ // There's chars inbetween.  Append those.
							PIUAppendBlock
								(
								newBlock,
								&((*block)[currentPosition]),
								foundPosition
								);
							}
						
						// Now copy the replacement string:
						if (replace_size > 0)
							{ // If the size is 0 we're deleting, but we've got
							  // some chars, so move them over:
							PIUAppendBlock
								(
								newBlock,
								replace,
								replace_size
								);
							}
						
						// Move to next place to look:
						currentPosition += foundPosition + target_size;
						
						}
					}
					
					//-------------------------------------------------------
					// (4) Copy the remainder over and null terminate.
					//-------------------------------------------------------
					PIUAppendBlock
						(
						newBlock,
						&((*block)[currentPosition]),
						block_size - currentPosition
						);
						
					PIUNullTerminate(&newBlock[newBlock_size]);
						
					//-------------------------------------------------------
					// (5) Last, free the old block and return the pointer
					//	   to the new one.
					//-------------------------------------------------------
					sSPBasic->FreeBlock(*block);
					*block = newBlock;
				
				} // else Probably out of memory if we couldn't create new block.
			
			} // else we had no matches.						 
		}
	else
		{
		error = kSPBadParameterError;
		}
						
	return error;
	
	} // end PIUReplaceCharsInBlock
	
	
//-------------------------------------------------------------------------------
//
//	PIUCopyStringToBlock
//	
//	Takes a string and allocates a block of memory, returning a copy of the
//	string in that block.
//
//-------------------------------------------------------------------------------
char* PIUCopyStringToBlock
	(
	const char* const string,
	const unsigned short size
	)
	{
	char* allocatedBlock = NULL;
			
	if (sSPBasic.IsValid() && size > 0 && string != NULL)
		{
		if (sSPBasic->AllocateBlock(size+1, (void **)&allocatedBlock) == kSPNoError)
			{
			if (allocatedBlock != NULL)
				{
				for (unsigned short loop = 0; loop < size; loop++)
					allocatedBlock[loop] = string[loop];
				
				allocatedBlock[size] = '\0';
				}
			}
		}
	
	return allocatedBlock;
	
	}

//-------------------------------------------------------------------------------
//
//	PIUGetString
//	
//	Gets a resource and returns it as a block-allocated string.
//
//-------------------------------------------------------------------------------
char* PIUGetString(long stringID)
	{
		char* returnBlock = NULL;
		
		#ifdef __PIMac__

		Handle h = (Handle)GetString(stringID);
		HNoPurge(h);
		HLock(h);
	
		Ptr string = *h;
			
		// Convert to C string:
		unsigned char size = (unsigned char)string[0];
		
		returnBlock = PIUCopyStringToBlock
			(
			&string[1],	// It's a pascal string. Store as C-string.
			size
			);
			
		HUnlock(h);
		HPurge(h);			
			
		#elif defined(__PIWin__)
		
		if (sSPBasic.IsValid() && sPSUIHooks.IsValid() && sADMBasic.IsValid())
			{ // Need both basic suite and PlugInRef.
		
			char string[kMaxStr255Len] = "";

			/* This would be the normal Windows way to do it:
			size_t size = ::LoadString
				(
				hInstance, 
				stringID, 
				string, 
				kMaxSize
				);
			*/

			sADMBasic->GetIndexString
				(
				sPSUIHooks.GetPlugInRef(),
				stringID,
				0,
				string,
				kMaxStr255Len
				);

			unsigned short size = PIUstrlen(string);
			
			returnBlock = PIUCopyStringToBlock
				(
				string, // It's a C String.
				size
				);

			} // No SPBasic or plugInRef.
	
		#endif // Win/Mac
		
		return returnBlock;
	}

//-------------------------------------------------------------------------------
//
//	PIUFixed16ToDouble, PIUDoubleToFixed16
//	
//	16.16 -> double and double -> 16.16, etc.  I'm going to let the compiler
//	auto-inline this, which I'm sure it will do when optimizations are turned
//	on.
//
//-------------------------------------------------------------------------------
double PIUFixed16ToDouble (const unsigned long inValue)
	{
	return (double)(((double)inValue) / (double)65536.0);
	}
	
unsigned long PIUDoubleToFixed16 (const double inValue)
	{
	return (unsigned long)(inValue * 65536);
	}

//-------------------------------------------------------------------------------
//
//	PIUPin
//
//	Pins values to a minimum or maximum value.
//	
//-------------------------------------------------------------------------------
void PIUPinDouble
	(
	/* IN/OUT */	double*			value,
	/* IN */		const double	min,
	/* IN */		const double 	max
	)
	{
	if (*value < min)
		*value = min;
	else if (*value > max)
		*value = max;
	}

//-------------------------------------------------------------------------------
//
//	PIUGetAllKeysForReference.
//
//	Returns a descriptor will all keys available for this container.  It is
// 	the caller's responsibility to Free the descriptor and the incoming
//	reference.
//
//-------------------------------------------------------------------------------
PIActionDescriptor PIUGetAllKeysForReference(const PIActionReference reference)
	{
	PIActionDescriptor descriptor = NULL;

	SPErr error = kSPNoError;
	
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
	else
		{
		// Oops, don't have Get() function available.
		}

	// Descriptor is an opaque ID so we can return it on the stack. But
	// caller must Free it.	
	return descriptor;
	}

//-------------------------------------------------------------------------------
//
//	PIUGetAllKeysForClass
//
//	Returns a descriptor will all keys available for this container.  It is
// 	the caller's responsibility to Free the descriptor.
//
//-------------------------------------------------------------------------------
PIActionDescriptor PIUGetAllKeysForClass(const DescriptorClassID classID)
	{
	PIActionDescriptor descriptor = NULL;
	
	// Need the reference suite and either of the Get() routines to do this.
	if (sPSActionReference.IsValid())
		{
		PIActionReference reference = NULL;
		
		SPErr error = sPSActionReference->Make(&reference);
		
		if (error == kSPNoError && reference != NULL)
			{

			error = sPSActionReference->PutEnumerated
				(
				reference,
				classID,
				typeOrdinal,		// This is the standard for referring to current, previous, etc.
				enumTarget			// Current.
				);
				
			// Can also refer to things by offset (-1 = prev, +1 = next, 0 = this, etc.)
			// Or by index (0 = #1, N = last)
			// Or by ID (UUID)
			
			descriptor = PIUGetAllKeysForReference(reference);
			
			error = sPSActionReference->Free(reference);
			reference = NULL;
			
			} // error && reference
			
		} // valid
				
	return descriptor;
	}

//-------------------------------------------------------------------------------

// end PIUTools.cpp

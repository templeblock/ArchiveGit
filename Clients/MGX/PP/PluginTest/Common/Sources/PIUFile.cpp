//-------------------------------------------------------------------------------
//
//	File:
//		PIUFile.cpp
//
//	Copyright 1997-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the source and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions related to files.
//
//	Use:
//		PIUBasic is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//	Version history:
//		Version 1.0.0	1/11/1998	Created for Photoshop 5.0
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include "PIUFile.h"

//-------------------------------------------------------------------------------
//
//	PIUValidFileSpec.
//
//	Returns whether a SPPlatformFileSpec is valid.
//
//-------------------------------------------------------------------------------
bool PIUValidFileSpec
	(
	/* IN */	const SPPlatformFileSpecification fileSpec
	)
	{
	bool isValid = false;
	
	#ifdef __PIMac__
	
	if (fileSpec.name[0] > 0 &&
		fileSpec.vRefNum != 0)
		isValid = true;
	
	#elif defined(__PIWin__)
	
	if (PIUstrlen(fileSpec.path) > 0)
		isValid = true;
	
	#endif
	
	return isValid;
	}
	
//-------------------------------------------------------------------------------
//
//	PIUClearFileSpec.
//
//	Clears a SPPlatformFileSpec.
//
//-------------------------------------------------------------------------------
SPErr PIUClearFileSpec
	(
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	)
	{
	SPErr error = kSPNoError;
	
	if (fileSpec != NULL)
		{
		#ifdef __PIMac__
			fileSpec->vRefNum = 0;
			fileSpec->parID = 0;
			PIUNullTerminate((char*)&fileSpec->name[0]);
			PIUNullTerminate((char*)&fileSpec->name[1]);
		
		#elif defined(__PIWin__)
			PIUNullTerminate(&fileSpec->path[0]);
			PIUNullTerminate(&fileSpec->path[1]);
			
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
//	PIUForceFileSpec
//
//	Forces a PlatformFileSpec to a default file name, if the spec is not
//	valid.
//
//-------------------------------------------------------------------------------
SPErr PIUForceFileSpec
	(
	/* IN */		const char* const fileName,
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	)
	{
	SPErr error = kSPNoError;

	if (fileSpec != NULL && fileName != NULL)
		{
		if (!PIUValidFileSpec(*fileSpec))
			{
			// Not valid.  Do this.

			unsigned short length = PIUstrlen(fileName);

			#ifdef __PIMac__

				// Check length:
				if (length >= kMacMaxFileSpecNameLength)
					length = kMacMaxFileSpecNameLength-1;

				// Name is pascal string so length byte is byte 0:
				PIUCopy(&fileSpec->name[1], fileName, length);
				fileSpec->name[0] = length;

				// If we have room, put a null at the end for safety:
				if (length < kMacMaxFileSpecNameLength)
					PIUNullTerminate((char*)&fileSpec->name[length+1]);

			#elif defined(__PIWin__)

				// Check length:
				if (length >= kMaxPathLength)
					length = kMaxPathLength-1;

				// Name is a C string:
				PIUCopy(fileSpec->path, fileName, length);
				PIUNullTerminate(&fileSpec->path[length]);

			#endif

			} // ValidFileSpec.
		}
	else
		{
		error = kSPBadParameterError;
		}

	return error;
	}

//-------------------------------------------------------------------------------
//
//	PIUParseFilenameToDisplay.
//
//	Takes a SPPlatformFileSpec and returns the shortened filename.
//
//-------------------------------------------------------------------------------
SPErr PIUParseFilenameToDisplay
	(
	/* IN */	const SPPlatformFileSpecification fileSpec,
	/* OUT */	char displayString[kMaxStr255Len]
	)
	{
	SPErr error = kSPNoError;
	
	#ifdef __PIMac__
	
		PIUCopy(displayString, &fileSpec.name[1], fileSpec.name[0]);
		PIUNullTerminate(&displayString[fileSpec.name[0]]);
	
	#elif defined(__PIWin__)
	
		_splitpath
			(
			fileSpec.path,
			NULL,			// drive,
			NULL,			// directory,
			displayString,	// filename
			NULL			// extension
			);
	#endif
	
	return error;
	
	} // PIUParseFilenameToDisplay
	
//-------------------------------------------------------------------------------
//
//	PIURecordFileSpec
//
//	Stores the filename for Mac and Win in an appropriate descriptor.
//
//-------------------------------------------------------------------------------
SPErr PIURecordFileSpec
	(
	/* IN/OUT */	PIActionDescriptor 					descriptor,
	/* IN */		const DescriptorKeyID				key,
	/* IN */		const SPPlatformFileSpecification 	fileSpec
	)
	{
	SPErr error = kSPBadParameterError;
	
	if (sPSActionDescriptor.IsValid())
		{
		
		#ifdef __PIMac__
		
		// Mac PlatformFileHandles are stored as aliases.  SweetPea creates an
		// SPPlatformFileSpecification from the GetFile dialog that is the
		// equivalent of a Mac FSSpec:
		AliasHandle		alias;
		
		error = NewAlias(nil, (FSSpec *)&fileSpec, &alias);
		
		if (error == noErr)
			{ // Now store it.
		
			error = sPSActionDescriptor->PutAlias
				(
				descriptor,
				key,
				(Handle)alias
				);
			}
			
		#elif defined(__PIWin__)
		
		// Windows PlatformFileHandles are stored as text paths.  SweetPea
		// creates an SPPlatformFileSpecification from the GetFile dialog that
		// is a 300-max character path:
		
		if (sPSHandle.IsValid())
			{

			const unsigned short length = PIUstrlen(fileSpec.path);

			if (length > 0)
				{
				
				Handle h = sPSHandle->New(length+1); // Extra for null.

				if (h != NULL)
					{
					Ptr pointer = NULL;
					Boolean oldLock = false;
					
					sPSHandle->SetLock
						(
						h,
						true,
						&pointer,
						&oldLock
						);

					if (pointer != NULL)
						{
						error = PIUCopy
							(
							pointer,
							fileSpec.path,
							length
							);

						if (error == kSPNoError)
							PIUNullTerminate(&pointer[length]);
						}

					// Whether pointer returned null or not, we'll try to unlock:
					sPSHandle->SetLock
						(
						h,
						oldLock,		// Hopefully false.
						&pointer,
						&oldLock
						);
	
					error = sPSActionDescriptor->PutAlias
						(
						descriptor,
						key,
						h
						);
				
					// Now release the handle, since the host makes a copy:
					sPSHandle->Dispose(h);
					h = NULL;
					} // h
				} // length
			} // handle
			
		#endif	
		
		} // descriptor && handle
		
	return error;
	
	} // PIURecordFilename
		
//-------------------------------------------------------------------------------
//
//	PIURestoreFileSpec
//
//	Reads the file spec from an appropriate descriptor and restores the
//	file spec.
//
//-------------------------------------------------------------------------------
SPErr PIURestoreFileSpec
	(
	/* IN */	PIActionDescriptor 					descriptor,
	/* IN */	const DescriptorKeyID				key,
	/* OUT */	SPPlatformFileSpecification*		fileSpec
	)
	{
	SPErr error = kSPBadParameterError;
	
	if (!sPSActionDescriptor.IsValid())
		return error;
	
	#ifdef __PIMac__
	AliasHandle alias;
	
	error = sPSActionDescriptor->GetAlias
		(
		descriptor,
		key,
		&(Handle)alias
		);
		
	if (error == noErr)
		{
		Boolean wasChanged = false;
		
		error = ResolveAlias(nil, alias, (FSSpec*)fileSpec, &wasChanged);
		
		if (error != noErr || fileSpec->name[0] < 1)
			{
			// Couldn't find it, so make a record with just the name.
			error = GetAliasInfo(alias, asiAliasName, fileSpec->name);
			
			if (error != noErr && fileSpec->name[0] > 0)
				FSMakeFSSpec(0, 0, fileSpec->name, (FSSpec*)fileSpec);
			}
		}
		
	#elif defined(__PIWin__)
		
		if (!sPSHandle.IsValid())
			return error;
			
		Handle h = NULL;
		
		error = sPSActionDescriptor->GetAlias
			(
			descriptor,
			key,
			&h
			);
			
		if (h != NULL)
			{
			unsigned short length = (unsigned short)sPSHandle->GetSize(h);
			
			Boolean oldLock = FALSE;
			Ptr pointer = NULL;
			
			sPSHandle->SetLock
				(
				h,
				true,
				&pointer,
				&oldLock
				);
			
			if (pointer != NULL)
				{
				if (length > kMaxPathLength)
					length = kMaxPathLength-1;
					
				error = PIUCopy
					(
					fileSpec->path,
					pointer,
					length
					);
					
				if (error == kSPNoError)
					PIUNullTerminate(&fileSpec->path[length]);
			
				// Ignoring error:
				sPSHandle->SetLock
					(
					h,
					oldLock,
					&pointer,
					&oldLock
					);
				
				} // pointer
			
			// Ignoring error:
			sPSHandle->Dispose(h);
			h = NULL;
			
			} // Handle

		if (error == kSPNoError)
			{
			// We had to have retrieved some form of string.  Lets
			// go find it:

			char newPath[kMaxPathLength];
			
			LPCTSTR	lpPath = fileSpec->path;	// Search path.
			LPCTSTR lpFileName = NULL;			// Filename (part of path)
			LPCTSTR lpExtension = NULL;			// Extension (part of path)
			DWORD	nBufferLength = kMaxPathLength;
			LPTSTR	lpBuffer = newPath;	// Receiving buffer.
			LPTSTR	lpFilePart;					// Trailing / ends.
			DWORD bufferSize = SearchPath
				(
				lpPath,
				lpFileName,
				lpExtension,
				nBufferLength,
				lpBuffer,
				&lpFilePart
				);
 
			if (bufferSize < 1 || bufferSize > kMaxPathLength)
				{
				// We couldn't find it, so lets just get the filename
				// and store it wherever the current directory is
				// set:
				char filename[_MAX_FNAME];   
				char extension[_MAX_EXT];
   
				_splitpath
					(
					fileSpec->path,
					NULL,
					NULL,
					filename,
					extension
					);

				_makepath
					(
					newPath,
					NULL,
					NULL,
					filename,
					extension
					);
				}
			
			unsigned short length = PIUstrlen(newPath); 
			
			PIUCopy(fileSpec->path, newPath, length);
			PIUNullTerminate(&fileSpec->path[length]);
			
			} // Got no string from scripting system.
		
	#endif
	
	return error;
	
	} // PIURestoreFileSpec

//-------------------------------------------------------------------------------
//
//	PIUFileOpen
//
//	Open existing file or create if none.  Will return a file handle for
//	windows, a file reference number for Mac, or 0 if failed.  Will even
//	try to massage the fileSpec if it can't find the file.
//
//-------------------------------------------------------------------------------
PIUFileRef_t PIUFileOpen
	(
	/* IN/OUT */	SPPlatformFileSpecification* fileSpec
	)
	{
	PIUFileRef_t	fileRefNum = 0;
	
	#ifdef __PIMac__
	
		short fRefNum = 0;
		// To create this completely in the right spot, we need to
		// do some Mac voodoo with the vRefNum and the parID to
		// create a working directory reference:
		short new_vRefNum = 0;
		OSErr error = OpenWD(fileSpec->vRefNum, fileSpec->parID, 0, &new_vRefNum);
		
		if (error != noErr)
			{
			// This directory structure is messed.  Try reseting and doing
			// again.
			fileSpec->vRefNum = 0;
			fileSpec->parID = 0;
			
			error = OpenWD(fileSpec->vRefNum, fileSpec->parID, 0, &new_vRefNum);
			}
		
		if (error != noErr && fileSpec->name[0] > 0)
			{
			// Couldn't do that either.  Then just use the name.
			error = noErr;
			new_vRefNum = 0;
			}
			
		if (error == noErr)
			{
			error = FSOpen(fileSpec->name, new_vRefNum, &fRefNum);
			
			if (error == fnfErr)
				{ // File not found.  Create it:
				error = Create(fileSpec->name, new_vRefNum, 'ttxt', 'TEXT');
		
				// Try to open again:
				if (error == noErr)
					error = FSOpen(fileSpec->name, new_vRefNum, &fRefNum);
				}
			}
		
		if (error == noErr)
			fileRefNum = (long)fRefNum;
	
		// Error or not, I'm going to try to release the working directory:
		if (new_vRefNum != 0)
			CloseWD(new_vRefNum);
			
	#elif defined (__PIWin__)
		LPCSTR lpFileName								= fileSpec->path;
		DWORD dwDesiredAccess							= GENERIC_WRITE | GENERIC_READ;
		DWORD dwShareMode								= 0;
		LPSECURITY_ATTRIBUTES lpSecurityAttributes		= NULL;
		DWORD dwCreationDistribution					= OPEN_ALWAYS;
		DWORD dwFlagsAndAttributes						= FILE_ATTRIBUTE_NORMAL;
		HANDLE hTemplateFile							= NULL;
		
		HANDLE h = CreateFile
			(
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDistribution,
			dwFlagsAndAttributes,
			hTemplateFile
			);
			
		if (h == INVALID_HANDLE_VALUE)
			{
			// Couldn't create.  Try removing all path info and creating
			// the file in the current position:
			_splitpath
				(
				fileSpec->path,
				NULL,			// drive,
				NULL,			// directory,
				fileSpec->path,	// filename
				NULL			// extension
				);
			
			h = CreateFile
				(
				lpFileName,
				dwDesiredAccess,
				dwShareMode,
				lpSecurityAttributes,
				dwCreationDistribution,
				dwFlagsAndAttributes,
				hTemplateFile
				);
			}
			
		if (h != INVALID_HANDLE_VALUE)
			{
			fileRefNum = (long)h;
			}
		
	#endif // Win
	
	return fileRefNum;
	
	} // end PIUFileOpen
	
//-------------------------------------------------------------------------------
//
//	PIUFileSeek
//
//	Seek to a file position, relative to the PIUFilePosition.
//
//-------------------------------------------------------------------------------
SPErr PIUFileSeek
	(
	/* IN */	const PIUFileRef_t 		fileRefNum,
	/* IN */	const PIUFilePosition_t	relativePosition,
	/* IN */	const long				byteOffset
	)
	{
	SPErr error = kSPNoError;
	
	if (fileRefNum != 0)
		{	
		#ifdef __PIMac__
		
			short fRefNum = (short)fileRefNum;
			short posMode = 0;
			
			switch (relativePosition)
				{
				case kFromStart:
					posMode = fsFromStart;
					break;
					
				case kFromPosition:
					posMode = fsFromMark;
					break;
				
				case kFromEnd:
					posMode = fsFromLEOF;
					break;
				
				default:
					error = kSPBadParameterError;
					break;
				}
			
			if (error == kSPNoError)
				error = SetFPos(fRefNum, posMode, byteOffset);
				
		#elif defined(__PIWin__)
		
			HANDLE hFile				= (HANDLE)fileRefNum;
			LONG lDistanceToMove		= byteOffset;
			PLONG lpDistanceToMoveHigh  = NULL;
			DWORD dwMoveMethod = 0;

			switch (relativePosition)
				{
				case kFromStart:
					dwMoveMethod = FILE_BEGIN;
					break;
					
				case kFromPosition:
					dwMoveMethod = FILE_CURRENT;
					break;
				
				case kFromEnd:
					dwMoveMethod = FILE_END;
					break;
				
				default:
					error = kSPBadParameterError;
					break;
				}
			
			if (error == kSPNoError)
				{
				DWORD pos = SetFilePointer
					(
					hFile,
					lDistanceToMove,
					lpDistanceToMoveHigh,
					dwMoveMethod
					);
				
				if (pos == 0xFFFFFFFF)
					error = (long)GetLastError();
				}
			
		#endif
		} // fileRefNum
	else
		{
		error = kSPBadParameterError;
		}
	
	return error;
	
	} // end PIUFileSeek

//-------------------------------------------------------------------------------
//
//	PIUFileWrite
//
//	Write to file at current position, returning the count of
//	how many bytes were written.
//
//-------------------------------------------------------------------------------
SPErr PIUFileWrite
	(
	/* IN */		const PIUFileRef_t		fileRefNum,
	/* IN */		const char* const		string,
	/* IN/OUT */	unsigned long*			inOutCount
	)
	{
	SPErr error = kSPNoError;
	
	if (fileRefNum != 0 && string != NULL && inOutCount != NULL)
		{
		if (*inOutCount > 0)
			{
			#ifdef __PIMac__
		
				short fRefNum = (short)fileRefNum;
				long ioCount = (long)(*inOutCount);
			
				error = FSWrite(fRefNum, &ioCount, string);
				*inOutCount = (unsigned long)ioCount;
			
			#elif defined(__PIWin__)
			
				HANDLE hFile					= (HANDLE)fileRefNum;
				LPCVOID lpBuffer				= string;
				DWORD nNumberOfBytesToWrite		= (DWORD)*inOutCount;
				DWORD nNumberOfBytesWritten		= 0;
				LPDWORD lpNumberOfBytesWritten	= &nNumberOfBytesWritten;
				LPOVERLAPPED lpOverlapped		= NULL;

				BOOL success = WriteFile
					(
					hFile,
					lpBuffer,
					nNumberOfBytesToWrite,
					lpNumberOfBytesWritten,
					lpOverlapped
					);

				if (success)
					{
					*inOutCount = (long)nNumberOfBytesWritten;
					}
				else
					{
					error = (long)GetLastError();
					}
			#endif
			}
		else
			{ // inOutCount < 1:
			error = kSPBadParameterError;
			}
		}
	else
		{ // fileRef | string | inOutCount pointer bad:
		error = kSPBadParameterError;
		}
		
	return error;
	
	} // end PIUFileWrite		
		
//-------------------------------------------------------------------------------
//
//	PIUFileRead
//
//	Reads characters from file at current position, returning the
//	count of how many bytes were read.
//
//-------------------------------------------------------------------------------
SPErr PIUFileRead
	(
	/* IN */		const PIUFileRef_t		fileRefNum,
	/* IN/OUT */	char*					string,
	/* IN/OUT */	unsigned long*			inOutCount
	)
		{
	SPErr error = kSPNoError;
	
	if (fileRefNum != 0 && string != NULL && inOutCount != NULL)
		{
		if (*inOutCount > 0)
			{
			#ifdef __PIMac__
		
				short fRefNum = (short)fileRefNum;		
				long ioCount = (long)(*inOutCount);

				error = FSRead(fRefNum, &ioCount, (char*)string);
				*inOutCount = (unsigned long)ioCount;
				
			#elif defined(__PIWin__)
			
				HANDLE hFile					= (HANDLE)fileRefNum;
				LPVOID lpBuffer					= string;
				DWORD nNumberOfBytesToRead		= (DWORD)*inOutCount;
				DWORD nNumberOfBytesRead		= 0;
				LPDWORD lpNumberOfBytesRead		= &nNumberOfBytesRead;
				LPOVERLAPPED lpOverlapped		= NULL;

				BOOL success = ReadFile
					(
					hFile,
					lpBuffer,
					nNumberOfBytesToRead,
					lpNumberOfBytesRead,
					lpOverlapped
					);

				if (success)
					{
					*inOutCount = (long)nNumberOfBytesRead;
					}
				else
					{
					error = (long)GetLastError();
					}
			#endif
			}
		else
			{
			error = kSPBadParameterError;
			}
		}
	else
		{
		error = kSPBadParameterError;
		}
				
	return error;
	
	} // end PIUFileRead

//-------------------------------------------------------------------------------
//
//	PIUFileClose
//
//	Closes an open file, resetting its ref number:
//
//-------------------------------------------------------------------------------
SPErr PIUFileClose
	(
	/* IN/OUT */	PIUFileRef_t*			fileRefNum
	)
	{
	SPErr error = kSPNoError;
	
	if (*fileRefNum != 0 /* No file */)
		{
		#ifdef __PIMac__
			short fRefNum = (short)(*fileRefNum);
			error = FSClose(fRefNum);
			
		#elif defined __PIWin__
		
			HANDLE hObject = (HANDLE)(*fileRefNum);
			BOOL success = CloseHandle(hObject);
			
			if (!success)
				error = GetLastError();
			
		#endif
		
		*fileRefNum = 0; // Reset
		
		}
	else
		{
		error = kSPBadParameterError;
		}
		
	return error;
	
	} // end PIUFileClose

//-------------------------------------------------------------------------------

// end PIUFile.cpp

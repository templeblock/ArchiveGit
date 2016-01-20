//-------------------------------------------------------------------------------
//
//	File:
//		PIUtilities.h
//
//	Copyright 1993-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This library contains the headers for the macros, and
//		routines to simplify the use of suites and also some
//		helpful common plug-in functions. 
//
//	Use:
//		PIUtilities is intended to group common functions
//		into higher-level macros and routines to simplify
//		plug-in programming.
//
//		Most expect A4-globals to be set-up already, for
//		gStuff to be a valid pointer to your global structure's
//		parameter block, and, on Windows, for hDllInstance to
//		be a global handle reference to your plug-in DLL.
//
//	Version history:
//		Version 1.0.0	3/1/1993	Ace		Created for Photoshop 2.5.
//			Written by Mark Hamburg.
//
//		Version 1.0.1	4/8/1997	Ace		Updated for Photoshop 4.0.1.
//			Comments fattened out.  Reorganized for clarity.
//
//		Version 1.0.2	8/24/1997	Ace		Updated for Photoshop 5.0.
//			Added SPBasicSuiteAvailable macros.
//
//-------------------------------------------------------------------------------
 
#ifndef __PIUtilities_H__
#define __PIUtilities_H__

#include <stddef.h>				// Standard definitions.
#include <Types.h>				// Standard types.

#include "PITypes.h"			// Photoshop types.
#include "PIGeneral.h"			// Photoshop general routines.
#include "PIActions.h"			// Photoshop scripting.
#include "PIAbout.h"			// AboutRecord structure.
#include "PIDefines.h"			// Plug-in definitions.
#include "FileUtilities.h"		// Simple file utilities.

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#ifdef __PIWin__

	#include <stdlib.h>
	#include <winver.h>
	#include "WinUtilities.h"
	#include "WinDialogUtils.h"
	//#include "WinFileUtils.h"
	
	// May not need this, but define it for now and we'll check later:
	Fixed FixRatio(short numer, short denom);

#else // Macintosh or other:

	#include "DialogUtilities.h"
	//#include "FileUtilities.h"
	#include <LowMem.h>
	#include <ToolUtils.h>
	#include <FixMath.h>

	#define PISetRect	SetRect
	
	// Macintosh requires an entrypoint named "main":
	#define ENTRYPOINT main
	
#endif // Mac/Win

//-------------------------------------------------------------------------------
//	Resource string IDs
//-------------------------------------------------------------------------------

// Some of these are generic.  Override if you need to in your .h files:

#ifndef StringResource
	#define StringResource	'STR '
#endif

#ifndef ResourceID
	#define ResourceID		16000
#endif

#ifndef AboutID
	#define AboutID			ResourceID
#endif

#ifndef uiID
	#define uiID			ResourceID+1
#endif

#ifndef AlertID
	#define AlertID			16990
#endif

#ifndef kBadNumberID
	#define kBadNumberID	AlertID
#endif

#ifndef kBadDoubleID
	#define kBadDoubleID	kBadNumberID+1
#endif

#ifndef kNeedVers
	#define kNeedVers		kBadDoubleID+1
#endif

#ifndef kWrongHost
	#define kWrongHost		kNeedVers+1
#endif

#ifndef kStr255MaxLen
	#define kStr255MaxLen	255
#endif

#ifndef kStr255InitLen
	#define kStr255InitLen	kStr255MaxLen+1
#endif

//-------------------------------------------------------------------------------
// C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------------------------
//	HostGetPlatformWindowPtr -- Prototype
//-------------------------------------------------------------------------------

Handle HostGetPlatformWindowPtr (AboutRecordPtr aboutPtr);

//-------------------------------------------------------------------------------
//	HostGetPlatformWindowPtr -- Definition
//-------------------------------------------------------------------------------

#define PIPlatform()	\
	HostGetPlatformWindowPtr((AboutRecordPtr)gStuff)

//-------------------------------------------------------------------------------
//
//	TestAbort and UpdateProgress -- Macro definitions
//
//	These routines assume gStuff is defined to point at the parameter block.
//
//-------------------------------------------------------------------------------

#define TestAbort() ((*gStuff->abortProc) ())

#define UpdateProgress(done,total) ((*gStuff->progressProc) (done, total))

//-------------------------------------------------------------------------------
//
//	BufferProcs -- Prototypes
//
//	The following routines provide shells around the buffer procs routines.
//	These routines allow us to allocate buffers from Photoshop's memory
//	without first telling Photoshop about it in the bufferSpace or maxSpace
//	parameter in the interface record.  This can be useful when we need
//	different sized buffers at different times.
//
//-------------------------------------------------------------------------------

// Are the buffer procs even available?  If not, the plug-in will have to
// put up a warning dialog to indicate that it requires Photoshop 2.5 or
// will have to work around this using the old memory management techniques
// documented in earlier versions of the plug-in kit.  outNewerVersion is set if the
// procs version is newer than the plug-in.  The buffer procs version number
// is unlikely to change, but it is wise to test it anyway.  If outNewerVersion is
// null, it will be ignored:

Boolean HostBufferProcsAvailable (BufferProcs *procs, Boolean *outNewerVersion);

// The following dialog takes care of putting up the warning if the appropriate
// version of the buffer procs is not available:

Boolean WarnHostBufferProcsAvailable (BufferProcs *procs, Handle hDllInstance);

// How much space is available for buffers?  This space may be fragmented:

int32 HostBufferSpace (BufferProcs *procs); 

// Allocate a buffer of the appropriate size setting bufferID to the ID
// for the buffer.  If an error occurs, the error code will be returned
// and bufferID will be set to zero:

OSErr HostAllocateBuffer (BufferProcs *procs,
						  const int32 inSize, 
						  BufferID *outBufferID);

// Free the buffer with the given ID:

void HostFreeBuffer (BufferProcs *procs,
					 const BufferID inBufferID);

// Lock the buffer and return a pointer to its contents:

Ptr HostLockBuffer (BufferProcs *procs, 
					const BufferID inBufferID,
					Boolean inMoveHigh);

// Unlock the buffer.  Lock and unlock calls manipulate a counter, so they
// must balance perfectly:

void HostUnlockBuffer (BufferProcs *procs,
					   const BufferID inBufferID);

// The following routine allocates a buffer which is as tall as possible.  It
// takes as parameters, the desired rowBytes, the minimum height, the
// maximum height, and the fraction of the available buffer space to use
// expressed as 1/numBuffers.  It sets the actual height and bufferID
// parameters if successful:

OSErr HostAllocateStripBuffer (BufferProcs *procs,
							   const int32 inRowBytes,
							   const int16 inMinHeight,
							   const int16 inMaxHeight,
							   const int16 inNumBuffers,
							   int16 *outActualHeight,
							   BufferID *outBufferID);

//-------------------------------------------------------------------------------
//	BufferProcs -- Macro definitions
//-------------------------------------------------------------------------------

#define BufferProcsAvailable(outNewerVersion) \
	HostBufferProcsAvailable (gStuff->bufferProcs, outNewerVersion)

#define WarnBufferProcsAvailable() \
	WarnHostBufferProcsAvailable (gStuff->bufferProcs, hDllInstance)

#define BufferSpace() \
	HostBufferSpace (gStuff->bufferProcs)

#define AllocateBuffer(inSize, inBufferID) \
	HostAllocateBuffer (gStuff->bufferProcs, inSize, inBufferID)

#define FreeBuffer(inBufferID) \
	HostFreeBuffer (gStuff->bufferProcs, inBufferID)

#define LockBuffer(inBufferID, inMoveHigh) \
	HostLockBuffer (gStuff->bufferProcs, inBufferID, inMoveHigh)

#define UnlockBuffer(inBufferID) \
	HostUnlockBuffer (gStuff->bufferProcs, inBufferID)

#define AllocateStripBuffer(inRowBytes,inMinHeight,inMaxHeight,inNumBuffers,outActualHeight,outBufferID) \
	HostAllocateStripBuffer (gStuff->bufferProcs, \
							 inRowBytes, \
							 inMinHeight, \
							 inMaxHeight, \
							 inNumBuffers, \
							 outActualHeight, \
							 outBufferID)

//-------------------------------------------------------------------------------
//
//	AdvanceState -- Prototypes
//
//	Check for the advance state procedure and warn if not present.			 
//	The macros assume that gStuff is defined somewhere as a pointer
//	to the current interface record.
//
//-------------------------------------------------------------------------------

Boolean HostAdvanceStateAvailable (AdvanceStateProc proc);

Boolean WarnHostAdvanceStateAvailable (AdvanceStateProc proc, Handle hDllInstance);
   
//-------------------------------------------------------------------------------
//	AdvanceState -- Macro definitions
//-------------------------------------------------------------------------------

#define AdvanceStateAvailable() \
	HostAdvanceStateAvailable (gStuff->advanceState)

#define WarnAdvanceStateAvailable() \
	WarnHostAdvanceStateAvailable (gStuff->advanceState, hDllInstance)
	
#define AdvanceState() \
	(*(gStuff->advanceState)) ()

//-------------------------------------------------------------------------------
//
//	PseudoResourceProcs -- Prototypes
//
//  Here are the routines and macros for the pseudo-resource callbacks.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------
 
Boolean HostResourceProcsAvailable (ResourceProcs *procs, Boolean *outNewerVersion);

Boolean WarnHostResourceProcsAvailable (ResourceProcs *procs, Handle hDllInstance);

int16 HostCountPIResources (ResourceProcs *procs,
							const ResType inType);

Handle HostGetPIResource (ResourceProcs *procs,
						  const ResType inType,
						  const int16 inIndex);

void HostDeletePIResource (ResourceProcs *procs,
						   const ResType inType,
						   const int16 inIndex);

OSErr HostAddPIResource (ResourceProcs *procs,
						 const ResType inType,
						 const Handle inDataHandle);

//-------------------------------------------------------------------------------
//	PseudoResourceProcs -- Macro definitions
//-------------------------------------------------------------------------------

#define ResourceProcsAvailable(outNewerVersion) \
	HostResourceProcsAvailable (gStuff->resourceProcs, outNewerVersion)
	
#define WarnResourceProcsAvailable() \
	WarnHostResourceProcsAvailable (gStuff->resourceProcs, hDllInstance)
	
#define CountPIResources(type) \
	HostCountPIResources (gStuff->resourceProcs, type)
	
#define GetPIResource(type,index) \
	HostGetPIResource (gStuff->resourceProcs, type, index)
	
#define DeletePIResource(type,index) \
	HostDeletePIResource (gStuff->resourceProcs, type, index)
	
#define AddPIResource(type,data) \
	HostAddPIResource (gStuff->resourceProcs, type, data)

//-------------------------------------------------------------------------------
//
//	HandleProcs -- Prototypes
//
//  Here are the routines and macros for the HandleProcs callbacks.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

Boolean HostHandleProcsAvailable (HandleProcs *procs, 
								  Boolean *outNewerVersion);

Boolean WarnHostHandleProcsAvailable (HandleProcs *procs,
									  Handle hDllInstance);

Handle HostNewHandle (HandleProcs *procs, const int32 inSize);

void HostDisposeHandle (HandleProcs *procs, Handle h);

// Get and set the size of a handle:
int32 HostGetHandleSize (HandleProcs *procs, Handle h);

OSErr HostSetHandleSize (HandleProcs *procs, 
						 Handle h, 
						 const int32 inNewSize);

// Lock a handle, move it high (if told to) and return a pointer to its
// memory:
Ptr HostLockHandle (HandleProcs *procs,
					Handle h, 
					const Boolean inMoveHigh);

void HostUnlockHandle (HandleProcs *procs, Handle h);

// Concatinate one handle to the end of another:
OSErr HostHandleCat (HandleProcs *procs,
					  Handle *outHandle,
					  const Handle inHandle);


// Create a copy of a handle:
OSErr HostHandleCopy (HandleProcs *procs,
					  Handle *outHandle,
					  const Handle inHandle);
					  
// Read a standard resource into a handle:
Handle HostGetResource (HandleProcs *procs,
						Handle hDllInstance,
					    const ResType inType,
					    const int32 inRezID,
					    int32 *outSize);

// Read a standard string resource into a handle and return it in a pascal string:
void HostGetString (HandleProcs *procs,
					Handle hDllInstance,
					const int32 inResourceID,
					Str255 outString);



// Take a handle and returns it as a C string:
void HostHandle2CString (HandleProcs *procs,
						Handle inDataHandle,
						char *outString,
						const size_t outMaxSize);

// Take a C-string and make a handle from it:
Handle HostCString2Handle (HandleProcs *procs,
						  const char *inString);


// Take a handle and return up to 255 of its bytes as a pascal string:
void HostHandle2PString (HandleProcs *procs,
						Handle inDataHandle,
						Str255 outString);

// Take a pascal string and make a handle from it:
Handle HostPString2Handle (HandleProcs *procs,
						  const Str255 inString);

//-------------------------------------------------------------------------------
//	HandleProcs -- Macro definitions
//-------------------------------------------------------------------------------

#define HandleProcsAvailable(outNewerVersion) \
	HostHandleProcsAvailable (gStuff->handleProcs, outNewerVersion)
	
#define WarnHandleProcsAvailable() \
	WarnHostHandleProcsAvailable (gStuff->handleProcs, hDllInstance);
	
#define PINewHandle(size) \
	HostNewHandle (gStuff->handleProcs, size)
	
#define PIDisposeHandle(h) \
	HostDisposeHandle (gStuff->handleProcs, h)
	
#define PIGetHandleSize(h) \
	HostGetHandleSize (gStuff->handleProcs, h)
	
#define PISetHandleSize(h,size) \
	HostSetHandleSize (gStuff->handleProcs, h, size)
	
#define PILockHandle(h,moveHigh) \
	HostLockHandle (gStuff->handleProcs, h, moveHigh)
	
#define PIUnlockHandle(h) \
	HostUnlockHandle (gStuff->handleProcs, h)

#define PIHandleCat(h1, h2)	\
	HostHandleCat(gStuff->handleProcs, h1, h2)
	
// PICloneHandle went away, it didn't work well.  It was
// defined as:
// Handle PICloneHandle (const Handle inHandle)
// and returned a Handle.  Use PIHandleCopy instead.

#define PIHandleCopy(h1, h2) \
	HostHandleCopy(gStuff->handleProcs, h1, h2)

#define PIGetResource(type, index, size) \
	HostGetResource(gStuff->handleProcs, hDllInstance, type, index, size)

#define PIGetString(inResourceID, outString) \
	HostGetString(gStuff->handleProcs, hDllInstance, inResourceID, outString)
 
// PIString2Handle went away, it was not specific.  It was
// defined as:
// Handle String2Handle (HandleProcs handleProcs, Handle h, Str255 string)
// use PIPString2Handle for pascal strings, and PICString2Handle
// for C strings.
	
// PIHandle2String went away, it was not specific.  It was
// defined as:
// void String2Handle (HandleProcs handleProcs, Str255 string)
// use PIHandle2PString for pascal strings, and PIHandle2CString
// for C strings.

#define PICString2Handle(s) \
	HostCStringHandle (gStuff->handleProcs, s)
	
#define PIHandle2CString(h, s, max) \
	HostHandle2CString (gStuff->handleProcs, h, s, max)

#define PIPString2Handle(s) \
	HostPString2Handle (gStuff->handleProcs, s)
	
#define PIHandle2PString(h, s) \
	HostHandle2PString (gStuff->handleProcs, h, s)

//-------------------------------------------------------------------------------
//
//	DisplayPixelsProc -- Prototypes
//
//  Here are the routines and macros for the DisplayPixels callback.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

Boolean HostDisplayPixelsAvailable (DisplayPixelsProc proc);

Boolean WarnHostDisplayPixelsAvailable (DisplayPixelsProc proc,
										Handle hDllInstance);

//-------------------------------------------------------------------------------
//	DisplayPixelsProc -- Macro definitions
//-------------------------------------------------------------------------------

#define DisplayPixelsAvailable() \
	HostDisplayPixelsAvailable (gStuff->displayPixels)

#define	WarnDisplayPixelsAvailable() \
	WarnHostDisplayPixelsAvailable (gStuff->displayPixels, hDllInstance)

//-------------------------------------------------------------------------------
//
//	PropertyProcs -- Prototypes
//
//  Here are the routines and macros for the PropertyProcs callbacks.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

Boolean HostPropertyAvailable (PropertyProcs *procs, Boolean *outNewerVersion);

Boolean WarnHostPropertyAvailable (PropertyProcs *procs, Handle hDllInstance);

//-------------------------------------------------------------------------------
//	PropertyProcs -- Macro definitions
//-------------------------------------------------------------------------------

#define PropertyAvailable(outNewerVersion) \
	HostPropertyAvailable (gStuff->propertyProcs, outNewerVersion)

#define WarnPropertyAvailable()	\
	WarnHostPropertyAvailable (gStuff->propertyProcs, hDllInstance)

#define PIGetProp		gStuff->propertyProcs->getPropertyProc
#define PISetProp		gStuff->propertyProcs->setPropertyProc

#define GetSimple(key, simple)	\
	PIGetProp('8BIM', key, 0, simple, nil)

#define PutSimple(key, simple)  \
	PISetProp('8BIM', key, 0, simple, nil)
	
#define GetComplex(key, index, complex)	\
	PIGetProp('8BIM', key, index, 0, complex)
	
#define PutComplex(key, index, complex)	\
	PISetProp('8BIM', key, index, 0, complex)
	
//-------------------------------------------------------------------------------
//
//	ChannelPortProcs -- Prototypes
//
//  Here are the routines and macros for the ChannelPortProcs callbacks.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------
	
Boolean HostChannelPortAvailable (ChannelPortProcs *procs, Boolean *outNewerVersion);

Boolean WarnHostChannelPortAvailable (ChannelPortProcs *procs, Handle hDllInstance);

//-------------------------------------------------------------------------------
//	ChannelPortProcs -- Macro definitions
//-------------------------------------------------------------------------------

#define ChannelPort \
	(gStuff->channelPortProcs)
	
#define ReadPixels \
	(ChannelPort->readPixelsProc)

#define WritePixels \
    (ChannelPort->writeBasePixelsProc)

#define ReadFromWritePort \
	(ChannelPort->readPortForWritePortProc)

#define ChannelPortAvailable() \
	HostChannelPortAvailable (ChannelPort, false)

#define WarnChannelPortAvailable() \
	WarnHostChannelPortAvailable (ChannelPort, hDllInstance)
	
//-------------------------------------------------------------------------------
//
//	Error reporting function -- Prototypes
//
//  Here are the routines and macros for the Error reporting function.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block.  The Parameter block must also have errorString defined in it.
//
//	To use errorString, simply return hostReportString, and populate
//	the pascal string errorString with whatever response you want
//	displayed.  The host will display:
//
//	Could not complete your request because ^0.
//
//	Where ^0 will be replaced by your pascal string.  This mechanism is
//	also used to create a log during silent operation (see scripting).
//
//-------------------------------------------------------------------------------

short HostReportError(Str255 inSource, Str255 inTarget);

//-------------------------------------------------------------------------------
//	Error reporting function -- Macro definitions
//-------------------------------------------------------------------------------

#define PIReportError(errString) \
	HostReportError(*(gStuff->errorString), errString)

//-------------------------------------------------------------------------------
//
//	PIDescriptorParameters -- Prototypes
//
//  Here are the routines and macros for the PIDescriptorParameters callbacks.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

Boolean HostDescriptorAvailable (PIDescriptorParameters *procs,
								 Boolean *outNewerVersion);

Boolean WarnHostDescriptorAvailable (PIDescriptorParameters *procs, 
									 Handle hDllInstance);

// Close read descriptor and dispose handle:
OSErr	HostCloseReader (PIDescriptorParameters *procs, HandleProcs *hProcs, PIReadDescriptor *token);

// Closes write descriptor, dispose old handle and 
// set recordInfo = plugInDialogOptional:
OSErr	HostCloseWriter (PIDescriptorParameters *procs,
						 HandleProcs *hProcs,
						 PIWriteDescriptor *token);

// Put an object and dispose its handle, returning any error:
OSErr HostPutObj (PIDescriptorParameters *procs,
				  HandleProcs *hProcs, 
				  PIWriteDescriptor token,
				  DescriptorKeyID key,
				  DescriptorTypeID type,
				  PIDescriptorHandle *h);
				  
// Returns whether playInfo is plugInDialogDisplay, meaning to absolutely
// pop your dialog.  NOTE: This does not check for plugInDialogSilent,
// which means to absolutely NOT pop your dialog.
Boolean HostPlayDialog (PIDescriptorParameters *procs);

//-------------------------------------------------------------------------------
//	PIDescriptorParameters -- Macro definitions
//-------------------------------------------------------------------------------

#define NULLID		0 // for ID routines needing null terminator

#define DescParams 	gStuff->descriptorParameters

#define Reader 		DescParams->readDescriptorProcs

#define Writer 		DescParams->writeDescriptorProcs

#define PlayInfo	DescParams->playInfo

#define RecordInfo	DescParams->recordInfo

#define	PlayDialog() \
	HostPlayDialog (DescParams)

#define DescriptorAvailable() \
	HostDescriptorAvailable(DescParams, false)

#define WarnDescriptorAvailable() \
	WarnHostDescriptorAvailable(DescParams, hDllInstance)

#define OpenReadDesc(desc, array) \
	Reader->openReadDescriptorProc(desc, array)

#define	OpenReader(array) \
	OpenReadDesc(DescParams->descriptor, array)
	
#define CloseReadDesc(token) \
	Reader->closeReadDescriptorProc(token)

#define CloseReader(token) \
	HostCloseReader(DescParams, gStuff->handleProcs, token)
	
#define OpenWriter() \
	Writer->openWriteDescriptorProc()

#define CloseWriteDesc(token, handle) \
	Writer->closeWriteDescriptorProc(token, handle)

#define CloseWriter(token) \
	HostCloseWriter(DescParams, gStuff->handleProcs, token)

// These Macros simplify access to all the basic Get and Put routines:

#define PIGetKey(token, key, type, flags) \
	Reader->getKeyProc(token, key, type, flags)
	
#define PIGetEnum(token, value)	\
	Reader->getEnumeratedProc(token, value)
				
#define PIPutEnum(token, key, type, value) \
	Writer->putEnumeratedProc(token, key, type, value)
	
#define PIGetInt(token, value) \
	Reader->getIntegerProc(token, value)

#define PIGetPinInt(token, min, max, value) \
	Reader->getPinnedIntegerProc(token, min, max, value)

#define PIPutInt(token, key, value) \
	Writer->putIntegerProc(token, key, value)
	
#define PIGetFloat(token, value) \
	Reader->getFloatProc(token, value)
	
#define PIGetPinFloat(token, min, max, value) \
	Reader->getPinnedFloatProc(token, min, max, value)

#define PIPutFloat(token, key, value) \
	Writer->putFloatProc(token, key, value)
	
#define PIGetUnitFloat(token, unit, value) \
	Reader->getUnitFloatProc(token, unit, value)

#define PIGetPinUnitFloat(token, min, max, unit, value) \
	Reader->getPinnedUnitFloatProc(token, min, max, unit, value)

#define PIPutUnitFloat(token, key, unit, value) \
	Writer->putUnitFloatProc(token, key, unit, value)
	
#define PIGetBool(token, value) \
	Reader->getBooleanProc(token, value)

#define PIPutBool(token, key, value) \
	Writer->putBooleanProc(token, key, value)

#define PIGetText(token, value) \
	Reader->getTextProc(token, value)
	
#define PIPutText(token, key, value) \
	Writer->putTextProc(token, key, value)
	
#define PIGetAlias(token, value) \
	Reader->getAliasProc(token, value)
	
#define PIPutAlias(token, key, value) \
	Writer->putAliasProc(token, key, value)

#define PIGetEnum(token, value) \
	Reader->getEnumeratedProc(token, value)

#define PIPutEnum(token, key, type, value) \
	Writer->putEnumeratedProc(token, key, type, value)

#define PIGetClass(token, value) \
	Reader->getClassProc(token, value)
	
#define PIPutClass(token, key, value) \
	Writer->putClassProc(token, key, value)
	
#define PIGetRef(token, value) \
	Reader->getSimpleReferenceProc(token,value)
	
#define PIPutRef(token, key, value) \
	Writer->putSimpleReferenceProc(token, key, value)
	
#define PIGetObj(token, type, value) \
	Reader->getObjectProc(token, type, value)

#define PIPutObj(token, key, type, value) \
	HostPutObj(DescParams, gStuff->handleProcs, token, key, type, value)

#define PIPutObjProc(token, key, type, value) \
	Writer->putObjectProc(token, key, type, value)
	
#define PIGetCount(token, value) \
	Reader->getCountProc(token, value)
	
#define PIPutCount(token, key, value) \
	Writer->putCountProc(token, key, value)
	
#define PIGetStr(token, value) \
	Reader->getStringProc(token, value)
	
#define PIPutStr(token, key, value) \
	Writer->putStringProc(token, key, value)

//-------------------------------------------------------------------------------
//
//	ColorServicesProc -- Prototypes
//
//  Here are the routines and macros for the ColorServicesProc callback.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

Boolean HostColorServicesAvailable (ColorServicesProc proc);

Boolean WarnHostColorServicesAvailable (ColorServicesProc proc, Handle hDLLinstance);

// Set a color array from four int16 colors:
OSErr CSSetColor (int16 *outColor,
				  const int16 inColor1, 
				  const int16 inColor2,
				  const int16 inColor3,
				  const int16 inColor4);

// Copies one color array to another:
OSErr CSCopyColor (int16 *outColor, const int16 *inColor);

// Return number of expected color planes for imageMode.  NOTE: This does
// not include any expected mask or alpha planes.
int16 CSPlanesFromMode (const int16 imageMode,
						const int16 currPlanes);

// Map imageMode to color services space.  Returns -1 if notSupported:
int16 CSModeToSpace (const int16 imageMode);


// Convert to and from 8.8 Color Picker plug-in space to int16 (but basically
// an unsigned8) Color Services space:
OSErr CSToPicker (unsigned16 *outColor, const int16 *inColor);
OSErr CSFromPicker (int16 *outColor, const unsigned16 *inColor);

// Convert one color from one space to another, returning the color in
// the same passed components, and any error:
OSErr HostCSConvertColor (ColorServicesProc proc,
						 const int16 sourceSpace,
						 const int16 resultSpace,
						 int16 *ioColor);


// Show the user-chosen color picker and return the space and
// colors picked:
OSErr HostCSPickColor (ColorServicesProc proc,
					   const Str255 inPrompt,
					   int16 *ioSpace,
					   int16 *ioColor);

// Initializes a ColorServicesInfo space with default parameters:
OSErr CSInitInfo (ColorServicesInfo *ioCSinfo);

//-------------------------------------------------------------------------------
//	Color Services -- Macro definitions
//-------------------------------------------------------------------------------

#define ColorServicesAvailable() \
	HostColorServicesAvailable (gStuff->colorServices)

#define WarnColorServicesAvailable() \
	WarnHostColorServicesAvailable (gStuff->colorServices, hDllInstance)
	
#define ColorServices(info) \
	(*(gStuff->colorServices)) (info)

#define CSConvertColor(source, result, color) \
	HostCSConvertColor (gStuff->colorServices, source, result, color)

#define CSPickColor(promptString, ioSpace, ioColor) \
	HostCSPickColor (gStuff->colorServices, promptString, ioSpace, ioColor)

//-------------------------------------------------------------------------------
//
//	Gestalt-like suite -- Definitions
//
//  Here are the routines and macros for a set of utility functions that
//	serve to return gestalt-like information about the OS.
//
//	These macros make the assumption that gStuff is pointing to the parameter
//	block AND hDllInstance is defined as a global variable or NULL on the Mac.
//
//-------------------------------------------------------------------------------

// Basic Information structure for these utilities:
typedef struct OSInfo
{
	ResType		signature;
	unsigned8	majorVersion;
	unsigned8	minorVersion;
	unsigned8	subVersion;
	unsigned8	stage;
	unsigned8	stageVersion;
	unsigned8	os;
} OSInfo, *POSInfo, **HOSInfo;

// "Stage" will be one of these:
enum
{
	OSStageDevelopment,
	OSStageAlpha,
	OSStageBeta,
	OSStageRelease
};

// "os" will be one of these:
enum
{
	OSMacOS6 = 1,
	OSMacOS7,
	OSMacOS8,
	OSWinNT = 10,
	OSWin16,
	OSWin32
};

// Windows version functions return 0 if error:
#define	errFunctionFailed	0 // zero is an error, non-zero is success

// Quick top and tail definitions:
#define OSMacOSStart	OSMacOS6
#define OSMacOSEnd		OSMacOS8

#define OSWinStart		OSWinNT
#define OSWinEnd		OSWin32

// Define specifics for comparisons.
#define kHostNoMatch	0	// Any non-zero value indicates a match.

//-------------------------------------------------------------------------------
//
//	Gestalt-like suite -- Third party hosts definitions
//
//	The Gestalt-like suite maps Windows' version resource's "Internal name"
//	to the Macintosh 4-character OSType equivalent.
//
//	rtX is the Macintosh 4-character OS application signature,
//	fnX is the Windows "Internal name" string.
//
//-------------------------------------------------------------------------------

#define rtCorelDraw6		'Cdrw' // Corel Draw 6.0
#define	fnCorelDraw6		"CorelDrw"

#define rtCorelArtPaint6	'Mprn' // Corel Artisan 6.0 (Mac)
#define fnCorelArtPaint6	"PhotoPnt" // Corel Photo-Paint 6.1 (Win)

#define rtDeBabelizer		'BABL' // Equilibrium DeBabelizer
#define fnDeBabelizer		"DeBabelizer Pro"

/** Fractal Design **

// Until Fractal Design ships products with version resources
// in their Windows resources, these defines won't work regarding
// cross-referencing the Windows internal name.  (Therefore
// they've been commented out.)

#define rtPainter4			'FSX3' // Fractal Design Painter
#define fnPainter4			// Has no Version resource

#define rtDabbler2		'FSDA' // Fractal Design Dabbler
#define fnDabbler2		// Has no Version resource

 // Leave these commented out until Windows ships
 // with Version resources

 ** Fractal Design **/

//-------------------------------------------------------------------------------
//
//	Gestalt-like suite -- Adobe application definitions
//
//	The Gestalt-like suite maps Windows' version resource's "Internal name"
//	to the Macintosh 4-character OSType equivalent.
//
//	rtX is the Macintosh 4-character OS application signature,
//	fnX is the Windows "Internal name" string.
//
//-------------------------------------------------------------------------------

#define rtAfterEffects		'FXTC'
#define fnAfterEffects		"Adobe After Effects"

#define rtIllustrator3		'ART3'
// #define fnIllustrator3	There is no Windows version.

#define rtIllustrator88		'ARTZ'
// #define fnIllustrator88	There is no Windows version.

#define rtIllustrator11		'ARTY'
// #define fnIllustrator11	There is no Windows version.

#define rtIllustrator6		'ART5'
#define fnIllustrator6		"Adobe Illustrator"

#define rtPageMaker5		'ALD5'
#define fnPageMaker5		"PM5"

#define rtPageMaker6		'ALD6'
#define fnPageMaker6		"PM6"

#define rtPageMaker65		'AD65'
#define fnPageMaker65		"PM65"

#define rtPhotoDeluxe		'PHUT'
#define fnPhotoDeluxe		"PhotoDeluxe"

#define rtPhotoshop			kPhotoshopSignature // '8BIM'
#define fnPhotoshop			"Photoshp"

#define rtPremiere			'PrMr'
#define fnPremiere			"Adobe Premiere"

//-------------------------------------------------------------------------------
//	Gestalt-like suite -- Prototypes
//-------------------------------------------------------------------------------

// Initialize an OSInfo structure:
void ClearOSInfo (OSInfo *osInfo);

// Get the current into into an osInfo structure.  If the structure was empty,
// then an error occurred:
OSErr GetOSInfo (HandleProcs *procs, OSInfo *osInfo);

// Match a single or set of signatures, returning a compressed OSInfo
// structure in an unsigned32:
unsigned32 AppMatch (HandleProcs *procs,
					 const ResType *signatures);

// Checks a signature against the list of known hosts, calling AppMatch
// to match a set and return the compressed OSInfo structure:					 
unsigned32 CheckForHost (HandleProcs *procs, 
						 const ResType signature);

// Maps the magic Macintosh and Windows system flags to the above defined
// enums of which OS System is available:
unsigned8 MapSystem (const unsigned32 os);

// Maps the magic Macintosh and Windows system flags for development,
// alpha, beta, and release version numbers:
unsigned8 MapStage (const unsigned32 stageMask, const unsigned32 stage);

// Maps a text string (Windows "Internal Name") to an unsigned32 signature:
unsigned32 MapSignature (const char *inName);

// Given a compressedOSInfo unsigned32, check the OS flag and determine
// if it's Windows:
Boolean		IsWindows (const unsigned32 flag);

// Given a compressedOSInfo unsigned32, check the OS flag and determine
// if it's Macintosh:
Boolean		IsMacOS (const unsigned32 flag);

// Check to see if the host returned is valid, Mac or Win:
Boolean		IsValidHost (const unsigned32 flag);

//-------------------------------------------------------------------------------
//
//	Gestalt-like suite -- Macro definitions for determining Hosts.
//
//	Examples:
//
//		HostIsPhotoshop()
//	To determine if operating in Photoshop;
//
//		IsWindows(HostIsPremiere())
//	To determine if operating in Windows Premiere.
//
//-------------------------------------------------------------------------------

#define HostIsApp(type) \
	CheckForHost (gStuff->handleProcs, type)

#define HostIsCorelDraw() \
	HostIsApp(rtCorelDraw6)

#define HostIsCorelArtOrPaint() \
	HostIsApp(rtCorelArtPaint6)

#define HostIsDeBabelizer() \
	HostIsApp(rtDeBabelizer)

#define HostIsAfterEffects() \
	HostIsApp(rtAfterEffects)
	
#define HostIsIllustrator() \
	HostIsApp(rtIllustrator3)
	
#define HostIsPageMaker() \
	HostIsApp (rtPageMaker5)
	
#define HostIsPhotoDeluxe() \
	HostIsApp (rtPhotoDeluxe)

#define HostIsPhotoshop() \
	HostIsApp (rtPhotoshop)
	
#define kPhotoshop401CompressedInfo	\
	(4 << 24 | 0 << 16 | 1 << 8)
	// Major | Minor   | Sub (do it this way, as opposed
	// to literal 0x04000100 for Windows compatibility)
	
#define kPhotoshop5CompressedInfo \
	(5 << 24 | 0 << 16 | 1 << 8)
	// Major | Minor   | Sub

#define HostIsPhotoshop401() \
	(HostIsApp (rtPhotoshop) & kPhotoshop401CompressedInfo)

#define HostIsPhotoshop5() \
	(HostIsApp (rtPhotoshop) & kPhotoshop5CompressedInfo)
	
#define HostIsPremiere() \
	HostIsApp (rtPremiere)

// Macro to check for a ValidOSInfo structure:
#define ValidOSInfo(osInfo) \
	(osInfo.signature != 0 && osInfo.os != 0)

//-------------------------------------------------------------------------------
//
//	String and number functions -- Prototypes
//
//  Here are the routines and macros for a set of utility functions that
//	do basic string and number conversion and manipulation.
//
//-------------------------------------------------------------------------------

typedef enum StringToNumberResult_t
	{
	kStringToNumberInvalid,		// Didn't find a valid number.
	kStringToNumberValid,		// Found a valid number.
	kStringToNumberNegative		// Reports valid negative just in case this was negative zero.
	} StringToNumberResult_t;

// Convert a string to its long value:
StringToNumberResult_t StringToNumber (Str255 s, long *value);

// Convert a string to its long value:
StringToNumberResult_t StringToDouble (Str255 s, double *value);

// Append one string to a pascal string:
void AppendString (Str255 outTarget,
				   const Str255 inSource,
				   short inSourceStart,
				   short inLength);

// Add a character to a pascal string:
void AppendCharToString (Str255 outString, const unsigned char c);

// Change a double value to a string, with a minimal amount of 
// decimal points (precision):
void DoubleToString (Str255 outString,
					 double inValue,
					 short inPrecision);

// Takes source string and moves everything after a decimal to target,
// removing extra from source:
void DivideAtDecimal(Str255 ioWhole, Str255 outDecimal);

// Faux version of Macintosh ParamText: Takes a string and replaces
// occurences of "^0" with r0, "^1" with r1, and "^2" with r2:
void PIParamText(unsigned char *s,
				 unsigned char *r0, 
				 unsigned char *r1, 
				 unsigned char *r2);

// Raises a base to a power:
double power (const uint32 inBase, const uint16 inRaise);

// String size functions:
int16 PIstrlen (const char *inString);

// Catinate one string onto another:
int16 HostStringCat(char *outTarget, const char *inSource);

// One character Upper to Lowercase:
char UpperToLower(const char inChar);

// String match functions.  Returns with either noMatch, gotMatch, or
// match at char int16:
int16 HostMatch(const char *inSearch, 
				const char *inTarget, 
				const Boolean ignoreCase, 
				const size_t inLength);

// Pascal string version:
int16 HostStringMatch(Str255 s1, 
					  const char *s2,
					  const Boolean ignoreCase);

// int32 ResType resource to 4-chars:
void PIType2Char (const ResType inType, char *outChars);

// 4-chars to int32 resource:
ResType PIChar2Type (const char *inChars);

// Simple memory move function:
void HostBlockMove (Ptr outTarget, 
					const Ptr inSource, 
					const size_t inLength);

// Call memory move function with opaque data:
void HostCopy (void *outTarget, 
			   const void *inSource,
			   const size_t inLength);
			   
// Reset a string:
void PIResetString(unsigned char *string);

//-------------------------------------------------------------------------------
//	String and number functions -- Macro definitions
//-------------------------------------------------------------------------------

#define PIMatch(inSearch, inTarget) \
	HostMatch(inSearch, inTarget, true, PIstrlen(inSearch))
	
#define PISMatch(inSearch, inTarget) \
	HostStringMatch(inSearch, inTarget, true)

// Return values for match function:
enum
{
	noMatch = -1,
	gotMatch = 0
}; // any other number is exact match at offset #

#define PIStringCat(outTarget, inSource) \
	HostStringCat(outTarget, inSource)

#define PIBlockMove(p1, p2, size) \
	HostBlockMove (p1, p2, size)

#define PICopy(s1, s2, size) \
	HostCopy(s1, s2, size)

// Multipliers to move long to fixed:
#define kHard16				(1L << 16) /* 16.16 */
#define kSoft16				kHard16 // use for "other" number systems

// 16.16 (or other) -> double and double -> 16.16 (or other):
#define PIFixedToDouble(x, shift)	(x / (double) shift)
#define PIDoubleToFixed(x, shift)	((int32)(x * shift))
#define Fixed16ToDouble(x) 	(x / (double) 65536.0)
#define DoubleToFixed16(x) 	((int32)(x * 65536))

/* 16.16 -> long and long -> 16.16 */
#define long2fixed(value)	(value << 16)
#define fixed2long(value)	(value >> 16)

/* From stdlib, finds offset of member */
#ifndef offsetof
#define offsetof(T, member)	((_Sizet)&(((T *)0)->member))
#endif

//-------------------------------------------------------------------------------
//
//	Plug-in Entrypoint -- Definitions
//
//	Routines to help set up globals for every type of plug-in and make for
//	easy dispatch to the routines.
//
//-------------------------------------------------------------------------------

// The routines that are dispatched to from the jump list should all be
// defined as
//		void RoutineName (GPtr globals);
// And this typedef will be used as the type to create a jump list:
typedef void (* FProc)();

//-------------------------------------------------------------------------------
//	Plug-in Entrypoint -- Prototypes
//-------------------------------------------------------------------------------

// Requires some odd incoming parameters, but sets up all globals and even
// dispatches to global initializion routine, if necessary.  It returns a
// locked pointer to be used for all your global access:
Ptr AllocateGlobals(const uint32 resultAddr,
					const uint32 paramBlockAddr,
					HandleProcs *procs, 
					const size_t size, 
					long *data, 
					FProc InitGlobals);

//-------------------------------------------------------------------------------
//	C++ wrapper
//-------------------------------------------------------------------------------

#ifdef __cplusplus
} // End of extern "C" block.
#endif // __cplusplus

//-------------------------------------------------------------------------------

#endif // __PIUtilities__

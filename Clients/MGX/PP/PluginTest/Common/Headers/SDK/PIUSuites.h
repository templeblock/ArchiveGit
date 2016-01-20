//-------------------------------------------------------------------------------
//
//	File:
//		PIUSuites.h
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

#ifndef __PIUSuites_h__
#define __PIUSuites_h__

#include <stddef.h>					// Standard definitions.
#include <Types.h>					// Standard types.

#include "PIDefines.h"				// Plug-in definitions.

#include "PITypes.h"				// Photoshop types.
#include "PIGeneral.h"				// Photoshop general routines.
#include "PIActions.h"				// Photoshop scripting.
#include "PIBufferSuite.h"			// Photoshop buffer suite.
#include "PIHandleSuite.h"			// Photoshop handle suite.
#include "PIUIHooksSuite.h"			// Photoshop User Interface suite.
#include "PIChannelPortsSuite.h"	// Photoshop Channel Ports suite.
#include "PIErrorSuite.h"			// Photoshop error suite.

#include "SPBasic.h"				// SweetPea Basic suite.
#include "SPRuntme.h"				// SweetPea Runtime library suite.

#include "ADMBasic.h"				// ADM Required core suite.
#include "ADMDialog.h"				// ADM Required Modal dialog suite.
#include "ADMNotifier.h"			// ADM Notifiers for Dialogs.
#include "ADMTracker.h"				// ADM Trackers.
#include "ADMItem.h"				// ADM Item.
#include "ADMList.h"				// ADM Lists.
#include "ADMEntry.h"				// ADM Entry.
#include "ADMDialogGroup.h"			// ADM Dialog group suite.

#include "ASZStringSuite.h"			// Adobe Standard ZStrings suite.

#include "PIUSuitePointer.h"		// Smart suite pointers.
#include "PIUActionParams.h"		// Additional action parameters.
#include "PIUUIParams.h"			// Additional user interface params.
#include "PIUNotifyUtils.h"			// Additional notify utilities.
#include "PIUActionControlUtils.h"	// Additional action control utilities.
#include "PIUReferenceUtils.h"		// Additional reference utilities.
#include "PIUTools.h"				// Sets of utilities.
#include "PIUCore.h"				// PIUBasic core utilities.

//-------------------------------------------------------------------------------
//	This is the main utility class to load suites.
//-------------------------------------------------------------------------------
class PIUSuites
{
	public:
		// Constructor:
		PIUSuites
			(
			SPBasicSuite* inSPBasic,
			PIActionParameters* actionParams,
			SPPluginRef plugInRef,
			const char* const * optionalSuiteIDs
			);
			
		// Copy constructor:
		PIUSuites
			(
			PIUSuites & source
			);
		
		// Assignment operator:
		PIUSuites & operator=
			(
			const PIUSuites & source
			);
		
		// Destructor:
		~PIUSuites () {};	// Specified as nothing.
		
		// Validate all suites.  If one is missing,
		// returns a pointer to the identifier string
		// for the missing suite.  Otherwise, returns NULL:
		char* IsValid (void) const;

		// Checks for valid suites, and if not, returns an
		// error message:
		SPErr ReportSuitesMissing (void) const;
		
	private:
		PIUSuites();	// Default constructor undefined.
		// None.
	protected:
		SPBasicSuite* sSPBasic_;
		char** optionalSuiteIDs_;
};

//-------------------------------------------------------------------------------
//	SweetPea suite references.
//-------------------------------------------------------------------------------
class SPBasic_t : public PIUSuitePointer<SPBasicSuite>
{
	public:
		SPBasic_t(SPBasicSuite* inSPBasic);
};

class SPRuntime_t : public PIUSuitePointer<SPRuntimeSuite>
{
	public:
		SPRuntime_t(SPBasicSuite* inSPBasic);
};

//-------------------------------------------------------------------------------
//	Action suite references.
//-------------------------------------------------------------------------------
class PSBasicActionControl_t : public PIUSuitePointer<PSBasicActionControlProcs>, public PIUActionControlUtils_t
{
	public:
		PSBasicActionControl_t(SPBasicSuite* inSPBasic);
};

class PSActionControl_t : public PIUSuitePointer<PSActionControlProcs>, public PIUActionControlUtils_t
{
	public:
		PSActionControl_t(SPBasicSuite* inSPBasic);
};

class PSActionDescriptor_t : public PIUSuitePointer<PSActionDescriptorProcs>, public PIUActionParams_t
{
	public:
		PSActionDescriptor_t
			(
			SPBasicSuite* inSPBasic,
			PIActionParameters * actionParams
			);
};

class PSDescriptorRegistry_t : public PIUSuitePointer<PSDescriptorRegistryProcs>
{
	public:
		PSDescriptorRegistry_t(SPBasicSuite* inSPBasic);
};


class PSActionList_t : public PIUSuitePointer<PSActionListProcs>
{
	public:
		PSActionList_t(SPBasicSuite* inSPBasic);
};


class PSActionReference_t : public PIUSuitePointer<PSActionReferenceProcs>, public PIUReferenceUtils_t
{
	public:
		PSActionReference_t(SPBasicSuite* inSPBasic);
};

#ifdef __PIMac__
class PSWindowNotify_t : public PIUSuitePointer<PSWindowNotifyProcs>, public PIUNotifyUtils_t
{
	public:
		PSWindowNotify_t(SPBasicSuite* inSPBasic);
};
#endif

//-------------------------------------------------------------------------------
//	Photoshop memory suites.
//-------------------------------------------------------------------------------
class PSBuffer_t : public PIUSuitePointer<PSBufferSuite1>
{
	public:
		PSBuffer_t(SPBasicSuite* inSPBasic);
};

class PSHandle_t : public PIUSuitePointer<PSHandleSuite1>
{
	public:
		PSHandle_t(SPBasicSuite* inSPBasic);
};

//-------------------------------------------------------------------------------
//	Photoshop user interface and text suites.
//-------------------------------------------------------------------------------
class PSUIHooks_t : public PIUSuitePointer<PSUIHooksSuite1>, public PIUUIParams_t
{
	public:
		PSUIHooks_t
			(
			SPBasicSuite* inSPBasic,
			SPPluginRef plugInRef
			);
};

class PSChannelPorts_t : public PIUSuitePointer<PSChannelPortsSuite1>
{
	public:
		PSChannelPorts_t(SPBasicSuite* inSPBasic);
};

class ASZString_t : public PIUSuitePointer<ASZStringSuite1>
{
	public:
		ASZString_t(SPBasicSuite* inSPBasic);
};

//-------------------------------------------------------------------------------
//	Photoshop error management suite.
//-------------------------------------------------------------------------------
class PSError_t : public PIUSuitePointer<PSErrorSuite1>
{
	public:
		PSError_t(SPBasicSuite* inSPBasic);
};

//-------------------------------------------------------------------------------
//	Adobe Dialog Manager suite references.
//-------------------------------------------------------------------------------
class ADMBasic_t : public PIUSuitePointer<ADMBasicSuite3>
{
	public:
		ADMBasic_t(SPBasicSuite* inSPBasic);
};

class ADMDialog_t : public PIUSuitePointer<ADMDialogSuite>
{
	public:
		ADMDialog_t(SPBasicSuite* inSPBasic);
};

class ADMNotify_t : public PIUSuitePointer<ADMNotifierSuite>
{
	public:
		ADMNotify_t(SPBasicSuite* inSPBasic);
};

class ADMTrack_t : public PIUSuitePointer<ADMTrackerSuite>
{
	public:
		ADMTrack_t(SPBasicSuite* inSPBasic);
};

class ADMItem_t : public PIUSuitePointer<ADMItemSuite>
{
	public:
		ADMItem_t(SPBasicSuite* inSPBasic);
};

class ADMList_t : public PIUSuitePointer<ADMListSuite>
{
	public:
		ADMList_t(SPBasicSuite* inSPBasic);
};

class ADMEntry_t : public PIUSuitePointer<ADMEntrySuite>
{
	public:
		ADMEntry_t(SPBasicSuite* inSPBasic);
};

class ADMDialogGroup_t : public PIUSuitePointer<ADMDialogGroupSuite>
{
	public:
		ADMDialogGroup_t(SPBasicSuite*  inSPBasic);
};

//-------------------------------------------------------------------------------
//	The suites are all declared so that they may be used globally.
//-------------------------------------------------------------------------------

extern SPBasic_t					sSPBasic;
extern SPRuntime_t					sSPRuntime;

extern PSBasicActionControl_t		sPSBasicActionControl;
extern PSActionControl_t 			sPSActionControl;
extern PSActionDescriptor_t 		sPSActionDescriptor;
extern PSDescriptorRegistry_t		sPSDescriptorRegistry;
extern PSActionList_t				sPSActionList;
extern PSActionReference_t			sPSActionReference;

#ifdef __PIMac__
extern PSWindowNotify_t				sPSWindowNotify;
#endif

extern PSBuffer_t					sPSBuffer;
extern PSHandle_t					sPSHandle;

extern PSUIHooks_t					sPSUIHooks;
extern PSChannelPorts_t				sPSChannelPorts;

extern PSError_t					sPSError;

extern ASZString_t					sASZString;

extern ADMBasic_t 					sADMBasic;
extern ADMDialog_t					sADMDialog;
extern ADMNotify_t					sADMNotify;
extern ADMTrack_t					sADMTrack;
extern ADMItem_t					sADMItem;
extern ADMList_t					sADMList;
extern ADMEntry_t					sADMEntry;
extern ADMDialogGroup_t				sADMDialogGroup;

//-------------------------------------------------------------------------------

#endif // __PIUSuites_h__

//-------------------------------------------------------------------------------
//
//	File:
//		PIUSuites.cpp
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

#include "PIUSuites.h"

//-------------------------------------------------------------------------------
//	SweetPea suite definitions.
//-------------------------------------------------------------------------------
SPBasic_t::SPBasic_t(SPBasicSuite*  inSPBasic = NULL) :
	PIUSuitePointer <SPBasicSuite> 
		(
		inSPBasic, 
		kSPBasicSuite, 
		kSPBasicSuiteVersion
		)
	{
	// Intentionally left blank.
	}

SPRuntime_t::SPRuntime_t(SPBasicSuite*  inSPBasic = NULL) :
	PIUSuitePointer <SPRuntimeSuite> 
		(
		inSPBasic, 
		kSPRuntimeSuite, 
		kSPRuntimeSuiteVersion
		)
	{
	// Intentionally left blank.
	}

//-------------------------------------------------------------------------------
//	Action suite definitions.
//-------------------------------------------------------------------------------
PSBasicActionControl_t::PSBasicActionControl_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSBasicActionControlProcs> 
		(
		inSPBasic,
		kPSBasicActionControlSuite,
		kPSBasicActionControlSuiteVersion
		)
	{
	// Intentionally left blank.
	}

PSActionControl_t::PSActionControl_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSActionControlProcs> 
		(
		inSPBasic,
		kPSActionControlSuite,
		kPSActionControlSuiteVersion
		)
	{
	// Intentionally left blank.
	}

PSActionDescriptor_t::PSActionDescriptor_t
	(
	SPBasicSuite* inSPBasic = NULL,
	PIActionParameters * actionParams = NULL
	) :
	PIUSuitePointer <PSActionDescriptorProcs>
		(
		inSPBasic,
		kPSActionDescriptorSuite,
		kPSActionDescriptorSuiteVersion
		)
	{
	SetParamBlock(actionParams);
	}

PSDescriptorRegistry_t::PSDescriptorRegistry_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSDescriptorRegistryProcs> 
		(
		inSPBasic,
		kPSDescriptorRegistrySuite,
		kPSDescriptorRegistrySuiteVersion
		)
	{
	// Intentionally left blank.
	}

PSActionList_t::PSActionList_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSActionListProcs>
		(
		inSPBasic,
		kPSActionListSuite,
		kPSActionListSuiteVersion
		)
	{
	// Intentionally left blank.
	}

PSActionReference_t::PSActionReference_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSActionReferenceProcs> 
		(
		inSPBasic,
		kPSActionReferenceSuite,
		kPSActionReferenceSuiteVersion
		)
	{
	// Intentionally left blank.
	}

#ifdef __PIMac__
PSWindowNotify_t::PSWindowNotify_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSWindowNotifyProcs> 
		(
		inSPBasic,
		kPSWindowNotifySuite,
		kPSWindowNotifySuiteVersion
		)
	{
	// Intentionally left blank.
	}
#endif

//-------------------------------------------------------------------------------
//	Photoshop memory suites.
//-------------------------------------------------------------------------------
PSBuffer_t::PSBuffer_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSBufferSuite1> 
		(
		inSPBasic,
		kPSBufferSuite,
		kPSBufferSuiteVersion1
		)
	{
	// Intentionally left blank.
	}

PSHandle_t::PSHandle_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSHandleSuite1> 
		(
		inSPBasic,
		kPSHandleSuite,
		kPSHandleSuiteVersion1
		)
	{
	// Intentionally left blank.
	}

//-------------------------------------------------------------------------------
//	Photoshop user interface and text suites.
//-------------------------------------------------------------------------------
PSUIHooks_t::PSUIHooks_t
	(
	SPBasicSuite* inSPBasic = NULL,
	SPPluginRef plugInRef = NULL
	) :
	PIUSuitePointer <PSUIHooksSuite1> 
		(
		inSPBasic,
		kPSUIHooksSuite,
		kPSUIHooksSuiteVersion1
		)
	{
	SetPlugInRef(plugInRef);
	}

PSChannelPorts_t::PSChannelPorts_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSChannelPortsSuite1> 
	(
	inSPBasic,
	kPSChannelPortsSuite,
	kPSChannelPortsSuiteVersion2
	)
	{
	// Intentionally left blank.
	}

ASZString_t::ASZString_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ASZStringSuite1> 
		(
		inSPBasic,
		kASZStringSuite,
		kASZStringSuiteVersion1
		)
	{
	// Intentionally left blank.
	}

//-------------------------------------------------------------------------------
//	Photoshop error management suite.
//-------------------------------------------------------------------------------
PSError_t::PSError_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <PSErrorSuite1> 
		(
		inSPBasic,
		kPSErrorSuite,
		kPSErrorSuiteVersion1
		)
	{
	// Intentionally left blank.
	}

//-------------------------------------------------------------------------------
//	Adobe Dialog Manager suite references.
//-------------------------------------------------------------------------------
ADMBasic_t::ADMBasic_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMBasicSuite3> 
		(
		inSPBasic, 
		kADMBasicSuite, 
		kADMBasicSuiteVersion3
		)
	{
	// Intentionally left blank.
	}

ADMDialog_t::ADMDialog_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMDialogSuite> 
	(
	inSPBasic, 
	kADMDialogSuite, 
	kADMDialogSuiteVersion
	)
	{
	// Intentionally left blank.
	}

ADMNotify_t::ADMNotify_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMNotifierSuite> 
		(
		inSPBasic, 
		kADMNotifierSuite, 
		kADMNotifierSuiteVersion
		)
	{
	// Intentionally left blank.
	}
	
ADMTrack_t::ADMTrack_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMTrackerSuite> 
		(
		inSPBasic, 
		kADMTrackerSuite, 
		kADMTrackerSuiteVersion
		)
	{
	// Intentionally left blank.
	}
	
ADMItem_t::ADMItem_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMItemSuite> 
		(
		inSPBasic, 
		kADMItemSuite, 
		kADMItemSuiteVersion
		)
	{
	// Intentionally left blank.
	}

ADMList_t::ADMList_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMListSuite> 
		(
		inSPBasic, 
		kADMListSuite, 
		kADMListSuiteVersion
		)
	{
	// Intentionally left blank.
	}

ADMEntry_t::ADMEntry_t(SPBasicSuite* inSPBasic = NULL) :
	PIUSuitePointer <ADMEntrySuite> 
		(
		inSPBasic, 
		kADMEntrySuite, 
		kADMEntrySuiteVersion
		)
	{
	// Intentionally left blank.
	}

ADMDialogGroup_t::ADMDialogGroup_t
	(
	SPBasicSuite*  inSPBasic = NULL
	) :
	PIUSuitePointer <ADMDialogGroupSuite>
		(
		inSPBasic,
		kADMDialogGroupSuite,
		kADMDialogGroupSuiteVersion
		)
	{
	// Intentionally left blank.
	}

SPBasic_t					sSPBasic(NULL);
SPRuntime_t					sSPRuntime(NULL);

PSBasicActionControl_t		sPSBasicActionControl(NULL);
PSActionControl_t 			sPSActionControl(NULL);
PSActionDescriptor_t 		sPSActionDescriptor(NULL);
PSDescriptorRegistry_t		sPSDescriptorRegistry(NULL);
PSActionList_t				sPSActionList(NULL);
PSActionReference_t			sPSActionReference(NULL);

#ifdef __PIMac__
PSWindowNotify_t			sPSWindowNotify(NULL);
#endif

PSBuffer_t					sPSBuffer(NULL);
PSHandle_t					sPSHandle(NULL);

PSUIHooks_t					sPSUIHooks(NULL);
PSChannelPorts_t			sPSChannelPorts(NULL);

PSError_t					sPSError(NULL);

ASZString_t					sASZString(NULL);

ADMBasic_t 					sADMBasic(NULL);
ADMDialog_t					sADMDialog(NULL);
ADMNotify_t					sADMNotify(NULL);
ADMTrack_t					sADMTrack(NULL);
ADMItem_t					sADMItem(NULL);
ADMList_t					sADMList(NULL);
ADMEntry_t					sADMEntry(NULL);
ADMDialogGroup_t			sADMDialogGroup(NULL);

PIUSuites::PIUSuites
	(
	SPBasicSuite* inSPBasic = NULL,
	PIActionParameters * actionParams = NULL,
	SPPluginRef plugInRef = NULL,
	const char* const * optionalSuiteIDs = NULL
	) :
	sSPBasic_(inSPBasic),
	optionalSuiteIDs_((char**)optionalSuiteIDs)
	{
	// Instantiate the suites:

	SPBasic_t new_sSPBasic(sSPBasic_);
	sSPBasic = new_sSPBasic;
	
	SPRuntime_t new_sSPRuntime(sSPBasic_);
	sSPRuntime = new_sSPRuntime;
	
	PSActionDescriptor_t new_sPSActionDescriptor(sSPBasic_);
	sPSActionDescriptor = new_sPSActionDescriptor;
	if (actionParams != NULL)
		sPSActionDescriptor.SetParamBlock(actionParams);
	
	PSBasicActionControl_t new_sPSBasicActionControl(sSPBasic_);
	sPSBasicActionControl = new_sPSBasicActionControl;
	
	PSActionControl_t new_sPSActionControl(sSPBasic_);
	sPSActionControl = new_sPSActionControl;
	
	PSDescriptorRegistry_t new_sPSDescriptorRegistry(sSPBasic_);
	sPSDescriptorRegistry = new_sPSDescriptorRegistry;
	
	PSActionList_t new_sPSActionList(sSPBasic_);
	sPSActionList = new_sPSActionList;
	
	PSActionReference_t new_sPSActionReference(sSPBasic_);
	sPSActionReference = new_sPSActionReference; 
	
	#ifdef __PIMac__
	PSWindowNotify_t new_sPSWindowNotify(sSPBasic_);
	sPSWindowNotify = new_sPSWindowNotify;
	#endif
	
	PSBuffer_t new_sPSBuffer(sSPBasic_);
	sPSBuffer = new_sPSBuffer;
	
	PSHandle_t new_sPSHandle(sSPBasic_);
	sPSHandle = new_sPSHandle;
	
	// We add some convenience functions to this suite for
	// access to the plugInRef:
	PSUIHooks_t new_sPSUIHooks(sSPBasic_);
	sPSUIHooks = new_sPSUIHooks;
	if (plugInRef != NULL)
		sPSUIHooks.SetPlugInRef(plugInRef);
	
	PSChannelPorts_t new_sPSChannelPorts(sSPBasic_);
	sPSChannelPorts = new_sPSChannelPorts;

	// Error management suite:
	PSError_t new_sPSError(sSPBasic_);
	sPSError = new_sPSError;
	
	// ADM functions:
	ADMBasic_t new_sADMBasic(sSPBasic_);
	sADMBasic = new_sADMBasic;

	ADMDialog_t new_sADMDialog(sSPBasic_);
	sADMDialog = new_sADMDialog;
	
	ADMNotify_t new_sADMNotify(sSPBasic_);
	sADMNotify = new_sADMNotify;
	
	ADMTrack_t new_sADMTrack(sSPBasic_);
	sADMTrack = new_sADMTrack;

	ADMItem_t new_sADMItem(sSPBasic_);
	sADMItem = new_sADMItem;

	ADMList_t new_sADMList(sSPBasic_);
	sADMList = new_sADMList;
	
	ADMEntry_t new_sADMEntry(sSPBasic_);
	sADMEntry = new_sADMEntry;
	
	ADMDialogGroup_t new_sADMDialogGroup(sSPBasic_);
	sADMDialogGroup = new_sADMDialogGroup;
	
	// Adobe Standard ZString suite:
	ASZString_t new_sASZString(sSPBasic_);
	sASZString = new_sASZString;
	
	// We also need to lazy init our static pseudo-suite pointer and its
	// children:
	sPIUBasic->IPIUBasic();
	
	}

PIUSuites::PIUSuites (PIUSuites & source) :
	sSPBasic_(source.sSPBasic_)
	{
	// Intentionally left blank.
	}

PIUSuites & PIUSuites::operator= 
	(
	const PIUSuites & source
	)
	{
	sSPBasic_ = source.sSPBasic_;
	return *this;
	}

char* PIUSuites::IsValid () const
	{
	char* returnPointer = NULL;

	if (!sSPBasic.IsValid() && !sSPBasic.IsOptional(optionalSuiteIDs_))
		returnPointer = sSPBasic.GetID();
	else if (!sSPRuntime.IsValid() && !sSPRuntime.IsOptional(optionalSuiteIDs_))
		returnPointer = sSPRuntime.GetID();

	else if (!sPSBasicActionControl.IsValid() && !sPSBasicActionControl.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSBasicActionControl.GetID();
	else if (!sPSActionControl.IsValid() && !sPSActionControl.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSActionControl.GetID();
	else if (!sPSDescriptorRegistry.IsValid() && !sPSDescriptorRegistry.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSDescriptorRegistry.GetID();
	else if (!sPSActionList.IsValid() && !sPSActionList.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSActionList.GetID();
	else if (!sPSActionReference.IsValid() && !sPSActionReference.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSActionReference.GetID();

	#ifdef __PIMac__
	else if (!sPSWindowNotify.IsValid() && !sPSWindowNotify.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSWindowNotify.GetID();
	#endif

	else if (!sPSBuffer.IsValid() && !sPSBuffer.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSBuffer.GetID();
	else if (!sPSHandle.IsValid() && !sPSHandle.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSHandle.GetID();
	else if (!sPSUIHooks.IsValid() && !sPSUIHooks.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSUIHooks.GetID();
	else if (!sPSChannelPorts.IsValid() && !sPSChannelPorts.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSChannelPorts.GetID();
	else if (!sPSError.IsValid() && !sPSError.IsOptional(optionalSuiteIDs_))
		returnPointer = sPSError.GetID();

	else if (!sADMBasic.IsValid() && !sADMBasic.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMBasic.GetID();
	else if (!sADMDialog.IsValid() && !sADMDialog.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMDialog.GetID();
	else if (!sADMNotify.IsValid() && !sADMNotify.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMNotify.GetID();
	else if (!sADMTrack.IsValid() && !sADMTrack.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMTrack.GetID();
	else if (!sADMItem.IsValid() && !sADMItem.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMItem.GetID();
	else if (!sADMList.IsValid() && !sADMList.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMList.GetID();
	else if (!sADMEntry.IsValid() && !sADMEntry.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMEntry.GetID();
	else if (!sADMDialogGroup.IsValid() && !sADMDialogGroup.IsOptional(optionalSuiteIDs_))
		returnPointer = sADMDialogGroup.GetID();

	else if (!sASZString.IsValid() && !sASZString.IsOptional(optionalSuiteIDs_))
		returnPointer = sASZString.GetID();

	return returnPointer;
	}

SPErr PIUSuites::ReportSuitesMissing (void) const
	{
	SPErr error = kSPNoError;

	// Will return pointer to name of suite that
	// couldn't load:
	char* badSuite = IsValid();

	if (badSuite != NULL)
		{
		// Suites are not valid.  We need to do
		// something about this.

		if (sPSError.IsValid() && sSPBasic.IsValid())
			{
			// The suites aren't valid, but if we at least have
			// the error suite we can return a string.
			// If we use PIUGetString, ADMBasic has to be available, 
			// and that is not likely if we're having a
			// problem with mising suites:
			char* errorString = "the required suite \"^0\" is unavailable"; // "the required suite, \"^0\", is missing."; // This will be moved into the resource fork before I'm through.
			
			char* newBlock = NULL;
			const unsigned short newBlock_size = PIUstrlen(errorString) + PIUstrlen(badSuite) + 1;

			error = sSPBasic->AllocateBlock(newBlock_size, (void**)&newBlock);

			if (error == kSPNoError)
				{
				PIUCopy
					(
					newBlock,
					errorString,
					PIUstrlen(errorString)+1
					);

				// PIUReplace here.
				error = PIUReplaceCharsInBlock
					(
					"^0",
					badSuite,
					kCaseSensitive,
					&newBlock
					);

				if (error == kSPNoError)
					{
					error = sPSError->SetErrorFromCString(newBlock);
					error = (SPErr)errReportString; // What we want to hand back.
					}
				else
					{
					error = (SPErr)errMissingParameter;
					}
				}
			else
				{
				error = (SPErr)errMissingParameter;
				}

			sSPBasic->FreeBlock(newBlock);
			newBlock = NULL;

			} // PSError or SPBasic were missing.
		else
			{
			// Exit ungracefully with a bad parameter error.
			error = (SPErr)errMissingParameter; // Photshop error return.  Should be mapped to SuitePea's kSPBadParameterError;
			}
		} // Not Valid.

	return error;
	}


//-------------------------------------------------------------------------------

// end PIUSuites.cpp

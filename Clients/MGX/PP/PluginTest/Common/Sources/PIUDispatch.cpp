//-------------------------------------------------------------------------------
//
//	File:
//		PIDispatch.cpp
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

#include "PIUDispatch.h"

bool PIUDispatch::Done() const
	{
		bool isDone = false;
		
		if (MatchCaller(kSPAccessCaller))
			{
			if (MatchSelector(kSPAccessUnloadSelector))
				{
				isDone = true;
				}
			}
		
		return isDone;
	}
	
bool PIUDispatch::MatchCaller
	(
	char* inTarget = NULL		// String to match.
	) const
	{
		return (PIUMatch(caller_, inTarget) == kGotMatch);
	}
	
bool PIUDispatch::MatchSelector
	(
	char* inTarget = NULL		// String to match.
	) const
	{
		return (PIUMatch(selector_, inTarget) == kGotMatch);
	}
	
SPErr PIUDispatch::LoadSuites
	(
	SPBasicSuite* inSPBasic,
	PIActionParameters* actionParams,
	SPPluginRef plugInRef,
	const char* const * const optionalSuiteIDs
	)
	{
	sSPBasic_ = inSPBasic;
	plugInRef_ = plugInRef;

	SPErr error = kSPNoError;
	
	if (inSPBasic != NULL)
		{ // Reacquire suites.  When this goes out of local
		  // scope suites will still stay around, since they're
		  // global scope.  This just inits them.
		PIUSuites init_suites
			(
			sSPBasic_,
			actionParams,
			(SPPluginRef)plugInRef,
			optionalSuiteIDs
			);

		error = init_suites.ReportSuitesMissing();
		}

	return error;

	}
	
SPPluginRef PIUDispatch::GetPlugInRef() const
	{
	return plugInRef_;
	}
	
bool PIUDispatch::DispatchRoutine
	(
	long stringID = 0,
	PIUAPI routine = PIUAPI_None,
	SPErr* error = NULL
	)
	{
	bool usedValues = false;
	
	if (routine != PIUAPI_None)
		{ // Use the routine:
		usedValues = true;
		*error = routine();
		}
	else if (stringID != 0)
		{ // We have a string.  Display it in a message.
		usedValues = true;
		char* stringBlock = PIUGetString(stringID);
		if (sADMBasic.IsValid() && stringBlock != NULL)
			{
			sADMBasic->PluginAboutBox("About plug-in...", stringBlock);
			sSPBasic->FreeBlock(stringBlock);
			stringBlock = NULL;
			}
		}
	
	return usedValues;
	}

SPErr PIUDispatch::DispatchAbout(void)
	{
	SPErr error = kSPNoError;
	
	SPInterfaceMessage* interfaceMessage = 
		(SPInterfaceMessage*) data_;
		
	/* Don't need to do this if you used generic
	   reload selector or you've made sure general
	   suites are loaded.
	   error = LoadSuites
		(
		aboutPtr->sSPBasic, 
		NULL,
		aboutPtr->plugInRef,
		optionalSuiteIDs_
		);
	*/
	
	if (error == kSPNoError)
		{
		if (!DispatchRoutine(aboutID_, about_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version:
			if (sADMBasic.IsValid())
				{
				sADMBasic->PluginAboutBox
					(
					"About plug-in...",
					"Copyright 1998 Adobe Systems Incorporated. All Rights Reserved."
					);
				}
			}
		}
	
	return error;
	
	}

SPErr PIUDispatch::DispatchStartup(void)
	{
	SPErr error = kSPNoError;
	
	SPInterfaceMessage* interfaceMessage = 
		(SPInterfaceMessage*) data_;
		
	/* Reload should have loaded our suites,
		so don't need to do this:
		error = LoadSuites
		(
		interfaceMessage->d.basic,
		NULL, // PIActionParameters
		interfaceMessage->d.self,
		optionalSuiteIDs_
		);
	*/

	if (error == kSPNoError)
		{
		if (!DispatchRoutine(0, startup_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version:
			}
		}

	return error;
	
	}

SPErr PIUDispatch::DispatchShutdown(void)
	{
	SPErr error = kSPNoError;
	
	SPInterfaceMessage* interfaceMessage = 
		(SPInterfaceMessage*) data_;
		
	/* Only do this if you need to do some very
	   special cleanup:
	error = LoadSuites
		(
		interfaceMessage->d.basic,
		NULL, // PIActionParameters
		interfaceMessage->d.self,
		optionalSuiteIDs_
		);
		
	*/

	if (error == kSPNoError)
		{
		if (!DispatchRoutine(0, shutdown_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version:
			}
		}
	return error;
	
	}

SPErr PIUDispatch::DispatchReload(void)
	{
	SPErr error = kSPNoError;
	
	SPAccessMessage* accessMessage =
		(SPAccessMessage*) data_;
	
	error = LoadSuites
		(
		accessMessage->d.basic,
		NULL, // PIActionParameters
		accessMessage->d.self,
		optionalSuiteIDs_
		);
		
	if (error == kSPNoError)
		{
		if (!DispatchRoutine(0, reload_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version: 
			}
		}
				
	return error;
	
	}

SPErr PIUDispatch::DispatchUnload(void)
	{
	SPErr error = kSPNoError;
	
	SPAccessMessage* accessMessage =
		(SPAccessMessage*) data_;

	/* Only do this if you need to do some very special
	   cleanup (you should have any suites
	   you need, anyway):
	error = LoadSuites
		(
		accessMessage->d.basic,
		NULL, // PIActionParameters
		accessMessage->d.self,
		optionalSuiteIDs_
		);					
	*/
	
	if (error == kSPNoError)
		{
		// Now check to see if this is a startup reload or
		// a reload during runtime:
		if (!DispatchRoutine(0, unload_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version: 
			}
		}

	return error;
	
	}

SPErr PIUDispatch::DispatchExecute(void)
	{
	SPErr error = kSPNoError;
	
	PSActionsPlugInMessage* actionsMessage =
		(PSActionsPlugInMessage*) data_;

	// We need to set a pointer to the parameter,
	// and our LoadSuites function does that, so
	// reload the suites and set the pointer:
	error = LoadSuites
		(
		actionsMessage->d.basic,
		actionsMessage->actionParameters,
		actionsMessage->d.self,
		optionalSuiteIDs_
		);
	
	if (error == kSPNoError)
		{
		if (!DispatchRoutine(0, execute_, &error))
			{ // No routine or string ID given to dispatch,
			  // so do generic version: 
			
			}
		}

	return error;
		
} // end DoExecute

SPErr PIUDispatch::Dispatch
	(
	const char* const caller,		// Calling message.
	const char* const selector,		// Selector message.
	const void* const data			// Message data.
	)
	{
	SPErr error = kSPNoError;
	
	// Copy to member variables:
	caller_ = (char*)caller;
	selector_ = (char*)selector;
	data_ = (void*)data;

	// Lets see if caller is Sweet Pea Interface suite:
	if (MatchCaller(kSPInterfaceCaller))
		{
		if (MatchSelector(kSPInterfaceAboutSelector))
			{
			error = DispatchAbout();
			}
		else if (MatchSelector(kSPInterfaceStartupSelector))
			{
			error = DispatchStartup();
			}
		else if (MatchSelector(kSPInterfaceShutdownSelector))
			{
			error = DispatchShutdown();
			}
		}
	
	// Lets see if caller is Sweet Pea Access suite:
	else if (MatchCaller(kSPAccessCaller))
		{
		if (MatchSelector(kSPAccessReloadSelector))
			{
			error = DispatchReload();
			}
		else if (MatchSelector(kSPAccessUnloadSelector))
			{
			error = DispatchUnload();
			}
		}	
			
	// Lets see if caller is Photoshop:
	else if (MatchCaller(kPSPhotoshopCaller))
		{ 
		if (MatchSelector(kPSDoIt))
			{
			error = DispatchExecute();
			}
		}
	
	return error;
	
	}

PIUDispatch::PIUDispatch
	(
	// List of optional suite IDs:
	const char* const * const optionalSuiteIDs = NULL,
	long aboutID = 0,						// About string resource ID.
	PIUAPI about = NULL,					// About routine.

	PIUAPI execute = NULL,					// Execute routine.

	PIUAPI reload = NULL,					// Reload routine.
	PIUAPI unload = NULL,					// Unload routine.
	
	PIUAPI startup = NULL,					// Persistent plug-in startup.
	PIUAPI shutdown = NULL					// Persistent plug-in shutdown.
	) :
	optionalSuiteIDs_((char**)optionalSuiteIDs),
	aboutID_(aboutID),
	about_(about),
	execute_(execute),
	reload_(reload),
	unload_(unload),
	startup_(startup),
	shutdown_(shutdown)
	{
	// Intentionally left blank.
	}
	
PIUDispatch::PIUDispatch
	(
	PIUDispatch& source
	) :
	optionalSuiteIDs_(source.optionalSuiteIDs_),
	aboutID_(source.aboutID_),
	about_(source.about_),
	execute_(source.execute_),
	reload_(source.reload_),
	unload_(source.unload_),
	startup_(source.startup_),
	shutdown_(source.shutdown_)
	{
	// Intentionally left blank.
	}

PIUDispatch& PIUDispatch::operator= 
	(
	const PIUDispatch& source
	)
	{
	optionalSuiteIDs_ = source.optionalSuiteIDs_;
	aboutID_ = source.aboutID_;
	about_ = source.about_;
	execute_ = source.execute_;
	reload_ = source.reload_;
	unload_ = source.unload_;
	startup_ = source.startup_;
	shutdown_ = source.shutdown_;
	return *this;
	}

//-------------------------------------------------------------------------------

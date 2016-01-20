//-------------------------------------------------------------------------------
//
//	File:
//		PIUDispatch.h
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

#ifndef __PIUDispatch_h__
#define __PIUDispatch_h__

#include "SPPlugs.h"
#include "PIAbout.h"
#include "PIActions.h"
#include "PIActionsPlugIn.h"
#include "PIUTools.h"
#include "PIUSuites.h"


// Used for PIUDispatch passing routines:
typedef SPErr (*PIUAPI)();
#define PIUAPI_None	((PIUAPI)NULL)

//-------------------------------------------------------------------------------
//	This is the main utility class to dispatch selector information.
//	Add additional constructors for any dispatch mechanisms that need
//	them.
//-------------------------------------------------------------------------------
class PIUDispatch
{
	public:
		// Constructor for Actions plug-in:
		PIUDispatch
			(
			const char* const * const optionalSuiteIDs,		// Optional Suite IDs (others are required.)

			long aboutID,					// About text resource ID, or:
			PIUAPI about,					// About routine.

			PIUAPI execute,					// Routine ID for execute routine.
			
			PIUAPI reload,					// Routine ID for reload routine.
			PIUAPI unload,					// Routine ID for unload routine.
			
			PIUAPI startup,					// Startup plug-in routine.
			PIUAPI shutdown					// Shutdown plug-in routine.
			);
		
		// Copy constructor:
		PIUDispatch 
			(
			PIUDispatch& source
			);
		
		// Assignment operator:
		PIUDispatch& operator= 
			(
			const PIUDispatch& source
			);
			
		// Destructor:
		~PIUDispatch ()
			{
			// Intentionally left blank.
			}
		
		// Dispatches basic info:
		SPErr Dispatch
			(
			const char* const caller,		// Calling message.
			const char* const selector,		// Selector message.
			const void* const data			// Pointer to hang globals off of.
			);
		
		// Returns true if we are done and can destroy class:
		bool Done() const;
			
	private:
		char* caller_;
		char* selector_;
		void* data_;
		char** optionalSuiteIDs_; 
		long aboutID_;
		PIUAPI about_;
		PIUAPI execute_;
		PIUAPI reload_;
		PIUAPI unload_;
		PIUAPI startup_;
		PIUAPI shutdown_;
		SPPluginRef plugInRef_;
		SPBasicSuite* sSPBasic_;

		// Matches caller or selector routines:
		bool MatchCaller(char* inTarget) const;
		bool MatchSelector(char* inTarget) const;
		
		// Triggers PIUSuites to reacquire.
		SPErr LoadSuites
			(
			SPBasicSuite* inSPBasic,
			PIActionParameters* actionParams,
			SPPluginRef plugInRef,
			const char* const * optionalSuiteIDs
			);
			
		// Dispatches to routine, if available.  Returns
		// true if ran the routine or used the string ID:
		bool DispatchRoutine
			(
			long stringID,
			PIUAPI routine,
			SPErr* error
			);
			
		// Dispatches routines or uses generic:
		SPErr DispatchAbout(void);
		SPErr DispatchReload(void);
		SPErr DispatchUnload(void);
		SPErr DispatchStartup(void);
		SPErr DispatchShutdown(void);
		SPErr DispatchExecute(void);
			
		// Returns plug-in reference:
		SPPluginRef GetPlugInRef(void) const;
		
	protected:
		// None.
};

//-------------------------------------------------------------------------------

#endif // __PIUDispatch_h__

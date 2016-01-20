/* ===========================================================================
   Module   :	main.c
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Main entry point of application.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
   

#include      <Quickdraw.h>
#include      <Memory.h>
#include      <Menus.h>
#include      <Fonts.h> 
#include      <Files.h>
#include      <Windows.h>
#include      "HeadlinerIncludes.h"
#include      "init.h"
#include      "event.h"

ASSERTNAME

/**  local prototypes  **/
         void           _InitMacintosh (void);
         void           _ValidateStartup (void);

/**  globals  **/
extern  Boolean              gQuit;


static void RenaissanceNewHandler( );
static void RenaissanceNewHandler( )
{
	throw kMemory;
}
/* ----------------------------------------------------------------------------
   _InitMacintosh 
   Macintosh manager initializations. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
void  _InitMacintosh (void)
{
	set_new_handler( RenaissanceNewHandler );

/**  set up memory  **/
    MaxApplZone();
    MoreMasters();
    MoreMasters();
    MoreMasters();
    MoreMasters();
    MoreMasters();
    MoreMasters();

/**  set up managers  **/
    InitGraf((Ptr)&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(0L);
    InitCursor();
    FlushEvents(everyEvent, 0);
}


/* ----------------------------------------------------------------------------
   _ValidateStartup 
   Check for system 7 during startup sequence. 
   Returns nothing.  Exits program on error.                  
   ---------------------------------------------------------------------------- */
void  _ValidateStartup (void)
{
	if (!fSystem7Available()) 
	{
		InitCursor();
		StopAlert(rSystem7ALRT, nil);
		ExitToShell(); 
	}
}


/* ----------------------------------------------------------------------------
   main 
   Main application entry point. 
   Returns nothing.               
   ---------------------------------------------------------------------------- */
void  main()
{

	_ValidateStartup();
	_InitMacintosh(); 
	SetUpMenus(); 
	InitApplication();
	
    FlushEvents(everyEvent, 0);		//	in case kid clicked during intro
    while (!gQuit) 
		MainEventLoop(); 

	ShutdownApplication();
}



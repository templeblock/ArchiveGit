////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1994 Monaco Systems Inc.
//
//	printmain.c
//
//
//	Create Date:	12/28/93 by Peter
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <stdio.h>
#include <Types.h>
#include <math.h>
#include <AppleEvents.h>

#include "monaco.h"
#include "printmain.h"
#include "printdialog.h"
#include "logowin2.h"
#include "logodialog.h"
#include "aboutdialog.h"
#include "toolbox.h"
#include "mcostat.h"
#include "mcotypes.h"
#include "eve_stuff_2&3.h"
#include "errordial.h"
#include "winevent.h"
#include "mem_dbg.h"

#include "SLFunctions.h"

#ifdef __MOTO__
QDGlobals	qd;
#endif

// an array of window pointers to check against added by JWV
long _WindowPtrs[100];
long _NumWindows = 0;


#define	OBJ(dialog)	((PrintDialog *)(((WindowPeek) dialog)->refCon))

McoStatus 	Show_Logo_check_mem_size(void);

McoStatus initializeMenus(void);
static void adjustmenus(void);
static void enable (MenuHandle, short, Boolean);
static Boolean isenable (MenuHandle, short);


void 	Main_Event_Loop(void);
int16 	Handle_Mouse_Down(void);
int16 	Handle_menu(int32);
int16 	Handle_apple_choice(int32 menu_choice);


//added by Peter to handle New events(James)
static int16	Handle_Mouse_Down_Menu(void);
//end of modification
				 	
/************************************************************************************
*  Define global variables                                                          *
**************************************************************************************/
Boolean  		All_Done;
EventRecord		event;
int32			sleep = 2;

//modified by Peter for new event handling
WindowPeek	nextjobwp = NULL;
//end of modification

// added by James
UserItemUPP  myOK;
UserItemUPP  my3D;
pascal void frame_item(DialogPtr dp, short item);
pascal void DrawNormalSur(DialogPtr dp, short item);

/************************************************************************************
* void main(void)                                                                   *
**************************************************************************************/
void main(void)
{	
	ToolBox*	toolbox;
   	PrintDialog *newdialog;
   	
   	AllWins		*wins;
	ProfileDoc	*doc;
	McoStatus	state;
	Boolean 	finish;
	
	toolbox = new ToolBox;
	McoUse	use;
	int		count = 0;
	Str255	st;

//	setup_debug(30000,30000,30000);
//	inc_level(); 
	
	// Set universals used by error code, added by James 1/15/96
	#if USESROUTINEDESCRIPTORS	
	myOK = NewUserItemProc(frame_item);
	my3D = NewUserItemProc(DrawNormalSur);
	#else
	myOK = frame_item;
	my3D = DrawNormalSur
	#endif

	#ifndef IN_POLAROID	
	if ( !(is_monaco_print())) { McoErrorAlert(MCO_BADEVE); goto bail;}
	use = get_usage_level(&count);
	if (use < P_AlmostNoUse) { McoMessAlert(MCO_NO_USE,st); goto bail;}
	#endif
		
	wins = new AllWins(TRUE);
	wins->InitMenues();
	
	//wins->newProfileDoc();
	
	//wins->openWindow(Startup_Dialog,0,nil);
	wins->AllEvents();
	
	delete wins;
	#if USESROUTINEDESCRIPTORS
	if (myOK != NULL) DisposeRoutineDescriptor(myOK);
	if (my3D != NULL) DisposeRoutineDescriptor(my3D);
	#endif
	
//	dump_debug(1);
//	dec_level();
//	clear_debug(); 

bail:
	close_eve();
	delete toolbox;	
	
}



		




/* ===========================================================================
   Module   :	globals.c
   Project  :	Renaissance graphic headline test utility.
   Abstract :	Application global data definitions.
   Status   :	Under Development
  
   Copyright © 1996, Turning Point Software.  All Rights Reserved.
  
   To Do:
  
  
   =========================================================================== */
#define _GLOBALS_C

#include    "HeadlinerIncludes.h"

ASSERTNAME

#include    "menu.h"
#include    "Psd3Graphic.h"
#include		"HeadlineGraphic.h"


/**  defines  **/
#define  kHeadlineTextSize			::PrinterPointToLogicalUnit( 72 )					// 72pt in twips

/**  menu structures & pointers  **/
MenuHandle           hAppleMenu = nil;
MenuHandle           hFileMenu = nil;
MenuHandle           hEditMenu = nil;
MenuHandle           hScaleMenu = nil;
MenuHandle           hFlipMenu = nil;
MenuHandle           hRotateMenu = nil;
MenuHandle           hShapeMenu = nil;
MenuHandle           hPathMenu = nil;
MenuHandle           hEffectsMenu = nil;
MenuHandle           hOutlineMenu = nil;
MenuHandle           hWackyMenu = nil;
MenuHandle           hFontMenu = nil;

/**  window structures & pointers  **/
CWindowRecord        gMainWindowRec;
WindowPtr            gMainWindow = nil;            // application window
GDHandle             gMainScreen = nil;

/**  globals  **/
BOOLEAN              gQuit = false;                	// quit flag 
BOOLEAN              gInForeground = true;         	// suspend/resume state flag 
sLONG                gSleepTime = 1L;				// time sharing time slice for WaitNextEvent 
Boolean				 	gfCPUIsSlow = FALSE;
THPrint              hPrintInfo = nil;
RHeadlineGraphic*	 	gpHeadline = nil;
RGraphic*			 	gpGraphic = nil;
RPath*		 		 	gpWarpPath = nil;
Handle				 	ghbitGraphic = nil;			// offscreen
BOOLEAN  			 	gfUseOffScreen = false;
short				 		gScaleMenuItem = k100SMENU;
short				 		gFlipMenuItem = kNonePMENU;
float				 		gRotateDegrees = 0;
EFlipDirection		 	gFlipDirection = kFlipNone;
BOOLEAN  			 	gfShowPathPoints = false;
BOOLEAN  			 	gfFeedback = false;
RIntSize					gScroll;
RGradientFill*			gpGradientBackdrop = NULL;
short						gLibraryType = nil;

RFontList            gFontList;
YFontInfo            gpFontChoices[] =
{
	{ 0, kHeadlineTextSize, 0, 0, 0, "AGaramond" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Broadview" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Brush Script" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Caesar" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Cotillion" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Courier" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Diploma" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Fitzgerald" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Gaslight" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Helvetica" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Hobo" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Intrepid" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "McZee" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "MPW" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "New York" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Palatino" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Patrick" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Percival" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Publicity Gothic ICG" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Stencil" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Stonehenge" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Storybook" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Times" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Times New Roman" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Transistor" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Tribune" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Tristan" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Unicorn" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Vagabond" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Venice" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Zapf Chancery" },
	{ 0, kHeadlineTextSize, 0, 0, 0, "Zapf Dingbats" }
} ;
int                 	gnFontChoices = sizeof( gpFontChoices ) / sizeof(YFontInfo);
BOOLEAN					gfTrueTypeAvailable = false;
BOOLEAN					gfATMAvailable = false;


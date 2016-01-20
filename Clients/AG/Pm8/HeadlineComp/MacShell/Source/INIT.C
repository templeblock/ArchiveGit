// ****************************************************************************
//
//  File Name:			init.c
//
//  Project:			Renaissance Graphic Headline Renderer
//
//  Author:				R. Hood
//
//  Description:		Shell application initialization routines
//
//  Portability:		Macintosh specific
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.			
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /Development/HeadlineComp/MacShell/Source/INIT.C $
//	 $Author:: Tony                       $
//	 $Date:: 3/18/97 12:21 PM               $
//  $Revision:: 3                       $
//
// ****************************************************************************
   

#include	<Traps.h> 
#include	<Fonts.h> 
#include	<Errors.h> 
#include	<OSUtils.h> 
#include	<ToolUtils.h> 
#include	<Gestalt.h> 
#include	<Windows.h> 
#include	<Printing.h> 

#include	"HeadlinerIncludes.h"

ASSERTNAME

#include	"GlobalBuffer.h"
#include	"Font.h"
#ifdef	powerc
	#include "ATMInterface.h"
#endif	// power mac

#include	"tpsbit.h"
#include	"init.h"
#include	"menu.h"
#include	"event.h"
#include	"update.h"
#include	"aevent.h"

/**  defines and resource IDs  **/
#define  rMenuBAR                      1000
#define  rMainWNDW                     2100
#define  rMainCLUT                      210
#define  kMinHeapSIZE           (50L * 1024L)
#define  kMinFreeSIZE           (10L * 1024L)
#define  kMainPixelDepth                  8    					// number of bits per pixel 

/**  menu structures & pointers  **/
extern  MenuHandle           hAppleMenu;
extern  MenuHandle           hFileMenu;
extern  MenuHandle           hEditMenu;
extern  MenuHandle           hScaleMenu;
extern  MenuHandle           hPathMenu;
extern  MenuHandle           hFlipMenu;
extern  MenuHandle           hRotateMenu;
extern  MenuHandle           hShapeMenu;
extern  MenuHandle           hEffectsMenu;
extern  MenuHandle           hOutlineMenu;
extern  MenuHandle           hWackyMenu;
extern  MenuHandle           hFontMenu;

/**  window structures & pointers  **/
extern  CWindowRecord        gMainWindowRec;
extern  WindowPtr            gMainWindow;
extern  GDHandle             gMainScreen;

/**  globals  **/
extern  BOOLEAN              gQuit;
extern  BOOLEAN              gInForeground;
extern  sLONG                gSleepTime;
extern  THPrint              hPrintInfo;
extern  RFontList            gFontList;
extern  int                  gnFontChoices;
extern  YFontInfo            gpFontChoices[];
extern  BOOLEAN				  gfTrueTypeAvailable;
extern  BOOLEAN				  gfATMAvailable;


/* ----------------------------------------------------------------------------
   InitApplication 
   Application initializations. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
void  InitApplication (void)
{
OSErr                osErr;
unsigned long        secs;
PaletteHandle        hPalette, hOldPalette;
CTabHandle           hColor;  
long                 total, contig;
Debug( DEBUGPARMS	 DebugParms; )

/**  validations  **/
	if (!fValidateConfiguration())
		ExitToShell(); 
	SetCursor(*GetCursor(watchCursor));

/**  a little memory cleanup  **/
#ifdef powerc
	SetResLoad(false);
	total = Count1Resources('CODE');
	for (int i = 1; i < total; i++)
	{
		Handle	hdl;
		if ((hdl = Get1IndResource('CODE', i)) != nil) 
		{
			HUnlock(hdl);
			ReleaseResource(hdl);
		}
	}
	SetResLoad(true);
	(void)CompactMem(kMinFreeSIZE);
#endif  // powerc

/**  get monitor device  **/
	if ((gMainScreen = GetMainDevice()) == nil)
	{
		StopAlert(rMonitorConfigALRT, nil);
		ExitToShell();
	}

/**  initialize most TPS subsystems  **/
	GetDateTime(&secs);
	qd.randSeed = secs;
#ifdef TPSDEBUG
	if (!tdb_StartupDebugLib( &DebugParms, 0 ))
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
#endif
	if (!StartupGlobalBuffer( ))
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}

/**  we must have a color table  **/
	if ((hColor = GetCTable(rMainCLUT)) == nil)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
	HNoPurge((Handle)hColor);
	(**hColor).ctSeed = (**(**(**gMainScreen).gdPMap).pmTable).ctSeed;	// zap color table seed

/**  create main window  **/
	gMainWindow = GetNewCWindow(rMainWNDW, &gMainWindowRec, (WindowPtr)-1);
	if (gMainWindow == nil)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
    SetPort(gMainWindow);
	ShowWindow(gMainWindow);

/**  set main window color palette  **/
    if ((hPalette = NewPalette((1 << kMainPixelDepth), hColor, pmTolerant, 0)) == nil)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
	CTab2Palette(hColor, hPalette, (pmTolerant + pmExplicit), 0);
	if ((hOldPalette = GetPalette(gMainWindow)) != nil)
		DisposePalette(hOldPalette);
	SetPalette(gMainWindow, hPalette, true);

/**  initialize TPS off-screen bitmap subsystem  **/
	if (!tbitStartup(2, kMainPixelDepth, (HNATIVE)hColor))
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
	tbitSetMainScreen(gMainWindow);
	tbitSetTransparentColor(TBIT_Black);

/**  install core apple event handler  **/
    if ((osErr = AEInstallEventHandler(kCoreEventClass, typeWildCard, NewAEEventHandlerProc(handleAECore), 0, false)) != noErr)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}

/**  display main window  **/
	SetPort(gMainWindow);
	ShowWindow(gMainWindow);   
	SelectWindow(gMainWindow);
	SetCursor(&qd.arrow);

/**  initialize print record  **/
    PrOpen();
    if (PrError())
       hPrintInfo = nil;
    else
    {
       hPrintInfo = (THPrint)NewHandle(sizeof(TPrint));
       if (MemError())
          hPrintInfo = nil;
       else
       {
          PrintDefault(hPrintInfo);
          if (PrError())
             hPrintInfo = nil;
       }
       PrClose();
    }
//    if (hPrintInfo != nil)							// REVIEW RAH why was this here?
//       (*hPrintInfo)->prStl.wDev = 0x0300;

/**  check available memory  **/
	if (((long)GetApplLimit() - (long)ApplicZone()) < kMinHeapSIZE)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
	PurgeSpace(&total, &contig);
	if (total < kMinFreeSIZE)
	{
		StopAlert(rLowMemoryALRT, nil);
		ExitToShell(); 
	}
		
	{
	long	result;
	YFont		font;
		// check for availability of TrueType
		if ( (osErr = Gestalt( gestaltFontMgrAttr, &result )) == noErr )
			gfTrueTypeAvailable = ( (result & 0x01) != 0 );
		// check for availability of ATM
#ifdef	powerc
		if ( initVersionATM( ATMProcs5Version ) != 0 )
			gfATMAvailable = TRUE;
#else
			gfATMAvailable = FALSE;
#endif	// power mac
		//	Add some fonts to the global font list.
		for (int j = 0; j < gnFontChoices; ++j)
		{
			font.info = gpFontChoices[j];
			gFontList.AddFont( font );
		}
	}

	if (hColor != nil)
		DisposCTable(hColor);
}


/* ----------------------------------------------------------------------------
   SetUpMenus 
   Application menu initialization.    
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
void  SetUpMenus()
{
Handle               hMenuBar;

	hMenuBar = GetNewMBar( rMenuBAR );
	SetMenuBar( hMenuBar );
	if ( hMenuBar != nil )
		DisposHandle( hMenuBar );
	hAppleMenu = GetMHandle( kAppleMENU );
	AddResMenu( hAppleMenu, 'DRVR' );
	hFileMenu = GetMHandle( kFileMENU );
	hEditMenu = GetMHandle( kEditMENU );
	hScaleMenu = GetMHandle( kScaleMENU );
	hPathMenu = GetMHandle( kPathMENU );
	hFlipMenu = GetMHandle( kFlipMENU );
	hRotateMenu = GetMHandle( kRotateMENU );
	hShapeMenu = GetMHandle( kShapeMENU );
	hEffectsMenu = GetMHandle( kEffectsMENU );
	hOutlineMenu = GetMHandle( kOutlineMENU );
	hWackyMenu = GetMHandle( kWackyMENU );
	AdjustMenus();
	DrawMenuBar();
	HiliteMenu( 0 );
}


/* ----------------------------------------------------------------------------
   fValidateConfiguration 
   Check system configuration during startup sequence and alerts user. 
   Application requires System 7.0 or greater. 
   Application requires Color QuickDraw. 
   Returns boolean indicating configuration matches application needs.
   ---------------------------------------------------------------------------- */
BOOLEAN  fValidateConfiguration (void)
{
Boolean              result;
long                 response;
	
	if (!(result = fSystem7Available())) 
		StopAlert(rSystem7ALRT, nil);
	else if (!(result = fGestaltAvailable())) 
		StopAlert(rSystem7ALRT, nil);
    else if (!(result = fColorQuickdrawAvailable(false)))
		StopAlert(rQuickDrawALRT, nil);
    else if (!(result = (Gestalt(gestaltHardwareAttr, &response) == noErr)))
		StopAlert(rSystem7ALRT, nil);
	return result;
}


/* ----------------------------------------------------------------------------
   ShutdownApplication 
   Application shutdown. 
   Returns nothing.
   ---------------------------------------------------------------------------- */
void  ShutdownApplication (void)
{

/**  close main/background windows  **/
    CloseWindow(gMainWindow);
    gMainWindow = nil;

/**  shutdown TPS subsystems  **/
	Debug( tdb_ShutdownDebugLib(); )
	ShutdownGlobalBuffer( );
	tbitShutdown();
}

/* ----------------------------------------------------------------------------
   fSystem7Available 
   Check for system 7. 
   Returns boolean indicating if system 7 running.                                        
   ---------------------------------------------------------------------------- */
BOOLEAN  fSystem7Available (void)
{
long			result = 0;

	if (GetToolTrapAddress(_Gestalt) == GetToolTrapAddress(_Unimplemented) || 
		Gestalt(gestaltSystemVersion, &result) != noErr)
		result = 0;

	return(result >= 0x0700);
}


/* ----------------------------------------------------------------------------
   fTrapAvailable 
   Checks to see if the specified trap is available. 
   Returns boolean indicating if trap available.                 
   ---------------------------------------------------------------------------- */
BOOLEAN  fTrapAvailable (short trap)
{
short		type;

	type = sTrapType(trap);
	if (type == ToolTrap)
		if ((trap &= 0x07FF) >= sToolboxTraps())
			trap = _Unimplemented;

	return(NGetTrapAddress(trap, type) != NGetTrapAddress(_Unimplemented, ToolTrap));
}
	

/* ----------------------------------------------------------------------------
   fGestaltAvailable 
   Check for system 7. 
   Returns boolean indicating if _Gestalt trap available.                                        
   ---------------------------------------------------------------------------- */
BOOLEAN  fGestaltAvailable (void)
{
	return fTrapAvailable(_Gestalt);
}


/* ----------------------------------------------------------------------------
   fColorQuickdrawAvailable 
   Checks to see if color quickdraw is available. 
   Returns boolean indicating if color quickdraw available.                 
   ---------------------------------------------------------------------------- */
BOOLEAN  fColorQuickdrawAvailable (BOOLEAN f32Bit)
{
long			response;

    if (Gestalt(gestaltQuickdrawVersion, &response) != noErr)
		return false;
    if ((response & gestalt32BitQD) != 0)
		return true;
    if (((response & gestalt8BitQD) != 0) && !f32Bit)
		return true;
	return false;
}

/* ----------------------------------------------------------------------------
   sTrapType 
   Checks trap to see if it is a tool or OS trap. 
   Returns trap type.                 
   ---------------------------------------------------------------------------- */
short  sTrapType (short trap)
{
	if (trap & 0x0800)
		return ToolTrap;
	else
		return OSTrap;
}

/* ----------------------------------------------------------------------------
   sToolboxTraps 
   Returns the number of tool box traps. 
   Returns number of traps.                 
   ---------------------------------------------------------------------------- */
short  sToolboxTraps (void)
{
	if (NGetTrapAddress(_InitGraf, ToolTrap) == NGetTrapAddress(0xAA6E, ToolTrap))
		return 0x200;
	else
		return 0x400;
}

/*
	Function Name:	tsysMacHasGWorlds
	
	Determines whether the machine supports color GWorlds.
	
	
	Returns:
	
		TRUE if color GWorlds are supported on this machine,
		FALSE if not.
*/
Boolean  tsysMacHasGWorlds (void)
{
long			response;

    if (Gestalt(gestaltQuickdrawFeatures, &response) != noErr)
		return false;
    if (response >= gestaltHasDeepGWorlds)
		return true;
	return false;
}

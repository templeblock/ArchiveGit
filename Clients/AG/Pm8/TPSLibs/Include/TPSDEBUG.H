/****************************************************************************
 *
 * $Header: /PM8/TPSLibs/Include/TPSDEBUG.H 1     3/03/99 6:35p Gbeddow $
 *
 *  Header Name:    TPSDEBUG.H
 *  Description:    Platform Independent Debug/Assertion API
 *  Version:        v0.00
 *  Author:         Originally by Robert L. Pyron, with reorganization
 *                  and consolidation by Michael G. Dunlap.
 *  Date:           7/9/1994
 *
 *
 * Copyright (C) 1994, 1995 by Turning Point Software, Inc. All Rights Reserved.
 *
 * $Log: /PM8/TPSLibs/Include/TPSDEBUG.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 5     12/09/98 1:17p Lwilson
// Converted all references from WIN to _WINDOWS.
 * 
 * 2     3/14/97 7:55a Mgd
 * fix warnings when the test in an assertion resolves to a constant TRUE.
 * The code was declaring the strings, but the compiler optimized out the
 * if and its contents, giving unreferenced locals representing the
 * strings.
 * 
 * 1     11/06/96 7:22a Mike_houle
 * 
 * 2     9/23/96 4:55 PM Richh
 * fixed a warning that had been bugging me
 * 
 * 3     3/15/96 2:37p Bruce
 * Changed TpsAssertAlways to not evaluate expression and compile clean
 * when used at W4
 * 
 * 2     3/14/96 8:13a Mike_houle
 * Change Assert to TpsAssert
    
    17    1/27/95 3:51p Bob
    fixed DebugMsg
    
    16    1/25/95 6:49p Bob
    more header stuff
    debug stuff
    moved READONLY to TDEFS.H so it's always available
 *
 ****************************************************************************
 */

/*===========================================================================
 * AVAILABLE MACROS AND ROUTINES:
 *
 * NOTE:       Unless otherwise noted, the following API results in nul statements
 *             when TPSDEBUG is not defined...
 *
 * Also NOTE:  Some platforms may not have all functionality.  In the cases where
 *             either the functionality makes no sense for that platform, or where
 *             the functionality has yet to be implemented, the API will result in
 *             nul statements regardless of whether or not TPSDEBUG is defined.
 *
 *
 * Debug(x)                      Include code x if TPSDEBUG is defined (any platform)
 * tdb_DebugBeep()               Beep at the user
 * tdb_DebugMsg(str)             Output a string to the debug device
 * tdb_DebugPrintf((fmt,args))   Output a printf style formatted string to the debug
 *                               device. (note the double ())
 * tdb_DebugDump(fmt,ptr,count)  Dump the count items at ptr in the specified format to
 *                               the debug device
 * TpsAssert(expr,msg)              Display an Assertion Failed message if (expr) is FALSE
 * TpsAssertB(expr,msg,pByte,count) Display an Assertion Failed message with count bytes of
 *                               data if (expr) is FALSE
 * TpsAssertW(expr,msg,pWord,count) Display an Assertion Failed message with count words of
 *                               data if (expr) is FALSE
 * TpsAssertL(expr,msg,pLong,count) Display an Assertion Failed message with count longs of
 *                               data if (expr) is FALSE
 * TpsAssertS(expr,msg,pCstr)       Display an Assertion Failed message with the given C string
 *                               if (expr) is FALSE
 * TpsAssertP(expr,msg,pPstr)       Display an Assertion Failed message with the given P string
 *                               if (expr) is FALSE
 * tdb_StartupDebugLib(p,x)      Initialize the debug library (platform dependent args, see below)
 * tdb_ShutdownDebugLib()        Shutdown the debug library
 * tdb_GetDebugFlags()           Get the current debug flags
 * tdb_SetDebugFlags(flags)      Set the specified debug flags, return the previous settings (all)
 * tdb_ClearDebugFlags(flags)    Clear the specified debug flags, return the previous settings (all)
 * tdb_ToggleDebugFlags(flags)   Toggle the specified debug flags, return the new settings (all)
 * UnimplementedCode()           Forces an assert with text "Unimplemented Routine"
 * UntestedCode()                Forces an assert with text "Untested Code"
 *
 *
 * PLATFORM SPECIFIC CONDITIONAL COMPILATION
 *
 * WinCode(x)                    Compile the specified code if _WINDOWS is defined
 * WinDebug(x)                   Compile the specified code if TPSDEBUG is defined (_WINDOWS only).
 * MacCode(x)                    Compile the specified code if MAC is defined
 * MacDebug(x)                   Compile the specified code if TPSDEBUG is defined (MAC only).
 * DosCode(x)                    Compile the specified code if DOS is defined
 * DosDebug(x)                   Compile the specified code if TPSDEBUG is defined (DOS only).
 * MacWinDos(x,y,z)              Compile the specified code that is appropriate for the given machine...
 * MacWinDosDebug(x,y,z)         Compile the specified code if TPSDEBUG is defined (but only for the appropriate machine)
 *
 * LIBRARY CONVENTIONS
 *
 *    TRUE simply means not FALSE, and is not necessarily a specific value.
 *    API functions that start with an underscore are NOT to be called directly
 *
 *
 * USING THE TPSDEBUG/ASSERTION LIBRARY
 *
 * 1. Macros that need to be defined:
 *    _WINDOWS, MAC, or DOS
 *       if _WINDOWS, then DEBUGDLL if application will be using
 *       the DLL version of the debug/assertion code.
 *       also if _WINDOWS, it is expected that WINDOWS.H has been
 *       included prior to this header.
 *
 *    if the test expression is to be displayed in the title of
 *    assertion failed messages, then define ASSERT_EXPR_IN_TITLE
 *
 *    if debug/assert code is to be compiled in, define TPSDEBUG
 *    otherwise it will be removed at compile time.
 *
 * 2. Building your Application
 *    After the standard platform/operating system header files,


 *    add the following includes:
 *
 *       #include "tpstypes.h"
 *       #include "tpsdebug.h"
 *
 *    Build your application with _WINDOWS, MAC, or DOS defined, and
 *    if you want the debug library turned on define TPSDEBUG
 *
 *    If building a Windows application:
 *       If you will be using the DLL version of the library then
 *       link your application with TPDBGDLL.LIB.
 *       If not using the DLL version:
 *          If you want to be able to trace into the debug library
 *          then get a copy of the debug library source and include
 *          it directly in your application.  Otherwise link your
 *          application with <memmodel>TPDBG.LIB.
 *
 *    If building a Dos application:
 *       If you want to be able to trace into the debug library
 *       then get a copy of the debug library source and include it
 *       directly in your application.  Otherwise link your application
 *       with <memmodel>TPDBG.LIB
 *
 *    If building a Macintosh application:

 *       <someone want to give me a hand here?>
 *
 * 3. Application initialization
 *    call tdb_StartupDebugLib( lpDebugParms, ulFlags )
 *
 *    Each platform has it's own version of the DEBUGPARMS
 *    structure, so an application should initialize the
 *    appropriate structure and pass it in to the startup
 *    function tdb_StartupDebugLib().  There are also
 *    system flags that are passed in, but 0 is fine as there
 *    are currently no defined options.
 *
 *    NOTE: The only platform currently requiring DEBUGPARMS
 *          is Windows.  The other platforms may pass NULL as
 *          the pointer to the parm structure, as it will be
 *          ignored anyway.
 *
 *    The debug library startup returns FALSE on failure, and
 *    TRUE on success.
 *
 * 4. Please note that DebugPrintf (if implemented on your platform)
 *    should be used sparingly, as it's string data will ultimately be
 *    mixed in with your application data, which will tend to use up
 *    data space more quickly, and could potentially hide data overwrite
 *    conditions.
 *    NOTE: If your compiler does not provide a means of storing strings
 *          at a specifiable place in memory, separate from the normal data
 *          locations, then this is no worse than Asserts...
 *
 * 5. Application termination
 *    call tdb_ShutdownDebugLib()
 *
 *
 * THINGS TO DO:
 *
 * 1. Add DOS specific interface
 *
 * 2. Identify and deal with platform idiosyncracies:
 *
 *    Determine proper DEBUGPARMS initialization parms for
 *    various platforms.
 *
 *    Find out how to persuade Borland C to store a string
 *    in a separate segment.
 *
 *    Add support for different memory models. Small model may
 *    require near pointers for some system calls (printf, etc)
 *
 * 3. <anyone else have suggestions?>
 *
 *===========================================================================
 */

#ifndef _tpsdebug_h_
#define _tpsdebug_h_

/* Platform Specific Information...
 *
 * NOTE:
 *
 * Since the _DebugPrintf function can have a variable number
 * of arguments, invocations of this macro should have double
 * parentheses. There is no way to extract the format string
 * into a static variable; excessive use of this macro can
 * fill your data segment with junk.
 */

	#if defined(_WINDOWS)
	/*
	 * Windows interface
	 */
		/*
		 * Platform Specific Structures
		 */
		typedef struct _DEBUGPARMS
			{
			HWND			hwndApp;
			HINSTANCE	hinstApp;
			} DEBUGPARMS;
		/*
		 * Conditional code
		 */
		#define	WinCode(x)					x
		#define	WinDebug(x)					Debug(x)
		#define	MacCode(x)
		#define	MacDebug(x)
		#define	DosCode(x)
		#define	DosDebug(x)
		//#define	MacWinDos(x,y,z)			y
		#define	MacWinDosDebug(x,y,z)	Debug(y)
		/*
		 * Debug system interface
		 */
		#define	_tdb_Beep()					MessageBeep(MB_ICONEXCLAMATION)
		#define	_tdb_Break()				__asm {int 3}
		#define	_tdb_MsgOut(x)				OutputDebugString(x)
		#define	_tdb_Printf(x)				_tdb_AuxPrintf x
		#define	_tdb_Dump(fmt,ptr,cnt)	_tdb_AuxDump(fmt,ptr,cnt)

		#if defined(DEBUGDLL)
			#define	TDBAPI(type)		FrameworkLinkage type FAR PASCAL
		#else
			#define	TDBAPI(type)		FrameworkLinkage type FAR PASCAL
		#endif

		/*
		 * Platform Specific Prototypes
		 */
		TPSAPI_DECLC( void )	_tdb_AuxPrintf(char FAR *fmt, ... );
		TPSAPI_DECLP( void )	_tdb_AuxDump(char FAR *fmt, void FAR *ptr, long count);

	#elif defined(MAC)
	/*
	 * Macintosh interface
	 */
		/*
		 * Platform Specific Structures
		 */
		typedef struct _DEBUGPARMS
			{
			int	dummy;
			} DEBUGPARMS;

		/*
		 * Conditional code
		 */
		#define	WinCode(x)
		#define	WinDebug(x)
		#define	MacCode(x)					x
		#define	MacDebug(x)					Debug(x)
		#define	DosCode(x)
		#define	DosDebug(x)
		//#define	MacWinDos(x,y,z)			x
		#define	MacWinDosDebug(x,y,z)	Debug(x)
		/*
		 * Debug system interface
		 */
		#define	_tdb_Beep()					SysBeep(10)
		#if defined(__MWERKS__)
			#define	_tdb_Break()			SysBreak()	//	MetroWorks does not break on Debugger()
		#else
			#define	_tdb_Break()			Debugger()
		#endif
		#define	_tdb_MsgOut(x)				DebugStr(c2pstr(x))
		#define	_tdb_Printf(x)				NULL
		#define	_tdb_Dump(fmt,ptr,cnt)	NULL

		#define	TDBAPI(type)		FAR PASCAL type

		/*
		 * Platform Specific Prototypes
		 */

	#elif defined(DOS)
	/*
	 * DOS interface
	 */
		/*
		 * Platform Specific Structures
		 */
		typedef struct _DEBUGPARMS
			{
			int dummy;
			} DEBUGPARMS;

		/*
		 * Conditional code
		 */
		#define	WinCode(x)
		#define	WinDebug(x)
		#define	MacCode(x)
		#define	MacDebug(x)
		#define	DosCode(x)					x
		#define	DosDebug(x)					Debug(x)
		#define	MacWinDos(x,y,z)			z
		#define	MacWinDosDebug(x,y,z)	Debug(z)
		/*
		 * Debug system interface
		 */
		#define	_tdb_Beep()					NULL
		#define	_tdb_Break()				__asm {int 3}
		#define	_tdb_MsgOut(x)				NULL
		#define	_tdb_Printf(x)				NULL
		#define	_tdb_Dump(fmt,ptr,cnt)	NULL
		
		#define	TDBAPI(type)		type FAR PASCAL

		/*
		 * Platform Specific Prototypes
		 */

	#else
	/*
	 * Unknown platform interface
	 */
		#error Please define _WINDOWS, MAC, or DOS
	#endif /* _WINDOWS, MAC, DOS, or Unknown */




/* Platform independent Constants
 */

	/* Caller may change these flags.
	 */

	/* if set, Assert will print to debug monitor if present.
	 * but no message box will be displayed in either case
	 */
	#define	kDbgAssertDisabled		(((uLONG)1) << 0)
	/* if set, data will be displayed in HEX format
	 */
	#define	kDbgHexDump					(((uLONG)1) << 1)


	/* Caller cannot change these flags.
	 */
	#define	kDbgNearCode				(((uLONG)1) << 16)
	#define	kDbgFarCode					(((uLONG)1) << 17)
	#define	kDbgNearData				(((uLONG)1) << 18)
	#define	kDbgFarData					(((uLONG)1) << 19)
	#define	kDbgInternalDisabled		(((uLONG)1) << 20)

	/* Useful flag groups.
	 */
	#define	kDbgMemoryModelFlags		(kDbgNearCode|kDbgFarCode|kDbgNearData|kDbgFarData)
	#define	kDbgAssertDisabledFlags	(kDbgAssertDisabled|kDbgInternalDisabled)
	#define	kDbgNonModifiableFlags	(kDbgMemoryModelFlags|kDbgInternalDisabled)
	#define	kDbgModifiableFlags		(~kDbgNonModifiableFlags)

	/* Memory model flags.
	 */
	#define	kDbgFlatModel				((uLONG)0)							/* flat model */
	#define	kDbgSmallModel				(kDbgNearCode | kDbgNearData)	/* small model */
	#define	kDbgCompactModel			(kDbgNearCode | kDbgFarData)	/* compact model */
	#define	kDbgMediumModel			(kDbgFarCode | kDbgNearData)	/* medium model */
	#define	kDbgLargeModel				(kDbgFarCode | kDbgFarData)	/* large model */

	/* Define kDbgMemoryModel. This should be passed in to InitDebugLib().
	 */
	#if	defined(_M_I86TM)		
		#define	kDbMemoryModel		kDbgSmallModel		/* tiny model */
	#elif	defined(_M_I86SM)		
		#define	kDbgMemoryModel	kDbgSmallModel		/* Small model */
	#elif	defined(_M_I86CM)		
		#define	kDbgMemoryModel	kDbgCompactModel	/* Compact model */
	#elif	defined(_M_I86MM)		
		#define	kDbgMemoryModel	kDbgMediumModel	/* Medium model */
	#elif	defined(_M_I86LM)		
		#define	kDbgMemoryModel	kDbgLargeModel		/* Large model */
	#elif	defined(_M_I86HM)		
		#define	kDbgMemoryModel	kDbgLargeModel		/* huge model */
	#else
		#define	kDbgMemoryModel	kDbgFlatModel		/* flat model */
	#endif


/* Platform Independent API
 */
	#if defined(__cplusplus)
		extern "C" {
	#endif /* __cplusplus */

	#ifdef ASSERT_EXPR_IN_TITLE
		#define	_DOASSERT_NOEVALUATION(expr,msg,pdata,fmt,cnt)	\
						{											\
						DEBUGSTRING(__msg__,msg)			\
						DEBUGSTRING(__expr__,#expr)		\
						if( !_tdb_AssertProc(__expr__,__msg__,__AssertFile__,__LINE__,pdata,fmt,cnt) )\
							{										\
							tdb_DebugBreak();					\
							}										\
						}
	#else /* ! ASSERT_EXPR_IN_TITLE */
		#define	_DOASSERT_NOEVALUATION(expr,msg,pdata,fmt,cnt)	\
						{											\
						DEBUGSTRING(__msg__,msg)			\
						if( !_tdb_AssertProc(NULL,__msg__,__AssertFile__,__LINE__,pdata,fmt,cnt) )\
							{										\
							tdb_DebugBreak();					\
							}										\
						}
	#endif /* ASSERT_EXPR_IN_TITLE */

	/*
	 * Implementation of _DOASSERT() is the same regardless of ASSERT_EXPR_IN_TITLE
	 */
	#define	_DOASSERT(expr,msg,pdata,fmt,cnt)	\
					{ if( !(expr) ) _DOASSERT_NOEVALUATION(expr,msg,pdata,fmt,cnt) }

	#if defined(TPSDEBUG)
		/* API for debug builds
		 */

		#define	Debug(x)			x

/*--------------------------------------------------------------------------
 * Macro:    tdb_DebugBeep
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  Annoying beep at the user...
 *--------------------------------------------------------------------------
 */
#define tdb_DebugBeep() _tdb_Beep()
#define DebugBeep()     tdb_DebugBeep()

/*--------------------------------------------------------------------------
 * Macro:    tdb_DebugBreak
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  Instantly break into the debugger at the current execution point
 *--------------------------------------------------------------------------
 */
#define tdb_DebugBreak()   _tdb_Break()
#define DebugBreak()       tdb_DebugBreak()

/*--------------------------------------------------------------------------
 * Macro:    tdb_DebugMsg
 * Expects:  char FAR *  lpszMessage
 * Returns:  nothing
 * Purpose:  Write the given string the the debug output device
 *--------------------------------------------------------------------------
 */
#define tdb_DebugMsg(msg)				\
			{									\
			DEBUGSTRING(__msg__,msg)	\
			_tdb_MsgOut(__msg__); 		\
			}
#define DebugMsg(msg)   tdb_DebugMsg(msg)
/*--------------------------------------------------------------------------
 * Macro:    tdb_DebugPrintf
 * Expects:  char_FAR_*  lpszFmt
 *           <var args>
 * Returns:  nothing
 * Purpose:  Output a formatted string to the debug output device.  Note that
 *           since this is a macro, the arguments to it must be enclosed in
 *           parenthesis, as there is no way to deal with a variable number
 *           of arguments in macros. i.e. tdb_DebugPrintf(("i=%d\n",i))
 *--------------------------------------------------------------------------
 */
#define tdb_DebugPrintf(x) _tdb_Printf(x)
#define  DebugPrintf(x)    tdb_DebugPrintf(x)
/*--------------------------------------------------------------------------
 * Macro:    tdb_DebugDump
 * Expects:  const char  szFormat[]
 *           void FAR *  lpvData
 *           long        lDataCount
 * Returns:  nothing
 * Purpose:  Dump the specified data in the specified format to the debug
 *           output device.
 *--------------------------------------------------------------------------
 */
#define tdb_DebugDump(fmt,ptr,count)		\
			{											\
			DEBUGSTRING(__fmt__, fmt)			\
			_tdb_Dump(__fmt__, ptr, count);	\
			}
#define DebugDump(fmt,ptr,count) tdb_DebugDump(fmt,ptr,count)

/*--------------------------------------------------------------------------
 * Macro:    TpsAssert
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char   failmsg[]       reason for failure
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, and the given
 *           reason string.  no extra data is displayed.
 *--------------------------------------------------------------------------
 */
#define TpsAssert(expr, msg)						_DOASSERT(expr,msg,NULL,0,0)
#define TpsAssertAlways(msg)						_DOASSERT_NOEVALUATION(NULL,msg,NULL,0,0)

 /*--------------------------------------------------------------------------
 * Macro:    TpsAssertB
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char   failmsg[]       reason for failure
 *           uBYTE FAR *  lpuByteData     ptr to uBYTE data
 *           uLONG        ulDataCount     number of uBYTE's to display
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, the given
 *           reason string, the ptr to the uBYTE's, and the given ulDataCount.
 *--------------------------------------------------------------------------
 */
#define TpsAssertB(expr, msg, pByte,nBytes)	_DOASSERT(expr,msg,pByte,'b',nBytes)
/*--------------------------------------------------------------------------
 * Macro:    TpsAssertW
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char   failmsg[]       reason for failure
 *           uWORD FAR *  lpuWordData     ptr to uWORD data
 *           uLONG        ulDataCount     number of uWORD's to display
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, the given
 *           reason string, the ptr to the uWORD's, and the given ulDataCount.
 *--------------------------------------------------------------------------
 */
#define TpsAssertW(expr, msg, pWord,nWords)	_DOASSERT(expr,msg,pWord,'w',nWords)
/*--------------------------------------------------------------------------
 * Macro:    TpsAssertL
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char   failmsg[]       reason for failure
 *           sLONG FAR *  lpLongData      ptr to sLONG data
 *           uLONG        ulDataCount     number of sLONG's to display
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, the given
 *           reason string, the ptr to the sLONG's, and the given ulDataCount.
 *--------------------------------------------------------------------------
 */
#define TpsAssertL(expr, msg, pLong, nLongs)	_DOASSERT(expr,msg,pLong,'l',nLongs)
/*--------------------------------------------------------------------------
 * Macro:    TpsAssertS
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char  failmsg[]        reason for failure
 *           char FAR *  lpszStringData   nul-terminated string data
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, the given
 *           reason string, and the given nul-terminated string data to be
 *           displayed.
 *--------------------------------------------------------------------------
 */
#define TpsAssertS(expr, msg, pCstr)			_DOASSERT(expr,msg,pCstr,'s',1)
/*--------------------------------------------------------------------------
 * Macro:    TpsAssertP
 * Expects:  <test_expr>                  test assumed to be TRUE
 *           const char  failmsg[]        reason for failure
 *           char FAR *  lppsStringData   pascal string to display
 * Returns:  nothing
 * Purpose:  if <test_expr> is FALSE, then _tdb_AssertProc is called with
 *           the given expression, the current file/lineno, the given
 *           reason string, and the given pascel-string data to be displayed.
 *           (pascal-strings are prefixed with a length byte)
 *--------------------------------------------------------------------------
 */
#define TpsAssertP(expr, msg, pPstr)			_DOASSERT(expr,msg,pPstr,'p',1)
/*--------------------------------------------------------------------------
 * Function: _tdb_StartupDebugLib
 * Expects:  DEBUGPARMS FAR *   lpvPlatformParms
 *           uLONG              ulSystemFlags
 * Returns:  BOOLEAN            bSuccessful    TRUE if successful else FALSE
 * Purpose:  startup the debug library.  Each platform has its own
 *           requirements for the startup parms, and so applications should
 *           initialize the platform specific structure and pass it in
 *           to this function.  The platform specific implementation of
 *           this function will deal with it as necessary.  The system
 *           flags currently only specify the memory model, which should be
 *           kDbgFlatModel for systems such as the Macintosh, where there
 *           is no such concept.
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_StartupDebugLib( DEBUGPARMS FAR * lpvPlatformParms, uLONG ulSystemFlags );
/*--------------------------------------------------------------------------
 * Macro:    tdb_StartupDebugLib
 * Expects:  DEBUGPARMS FAR *   lpvPlatformParms
 *           uLONG              ulSystemFlags
 * Returns:  BOOLEAN            bSuccessful
 * Purpose:  interface to _tdb_StartupDebugLib that includes the memory
 *           model automatically.  This can be used for any platform...
 *--------------------------------------------------------------------------
 */
#define	tdb_StartupDebugLib(pvArgs,flags)	_tdb_StartupDebugLib(pvArgs,((flags&kDbgModifiableFlags)|kDbgMemoryModel))
/*--------------------------------------------------------------------------
 * Function: tdb_ShutdownDebugLib
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  shutdown the debug library
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) tdb_ShutdownDebugLib( VOID );
/*--------------------------------------------------------------------------
 * Function: tdb_GetDebugFlags
 * Expects:  nothing
 * Returns:  uLONG       ulFlags
 * Purpose:  return the current debug settings
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_GetDebugFlags( VOID );
/*--------------------------------------------------------------------------
 * Function: tdb_SetDebugFlags
 * Expects:  uLONG       ulNewFlags
 * Returns:  uLONG       ulOldFlags
 * Purpose:  set the debug flags, and return the old settings
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_SetDebugFlags( uLONG ulNewFlags );
/*--------------------------------------------------------------------------
 * Function: tdb_ClearDebugFlags
 * Expects:  uLONG       ulFlagsToClear
 * Returns:  uLONG       ulPreviousSettings
 * Purpose:  clear the specified flags, and return the previous settings
 *           of those flags.
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_ClearDebugFlags( uLONG ulFlagsToClear );
/*--------------------------------------------------------------------------
 * Function: tdb_ToggleDebugFlags
 * Expects:  uLONG       ulFlagsToToggle
 * Returns:  uLONG       ulNewFlagStates
 * Purpose:  toggle the specified flags, and return the new states of
 *           those flags
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_ToggleDebugFlags( uLONG ulFlagsToToggle );
/*--------------------------------------------------------------------------
 * Function: _tdb_AssertProc
 * Expects:  char FAR *  lpszFailedTest
 *           char FAR *  lpszTestExplanation
 *           char FAR *  lpszFileContainingText

 *           uLONG       ulSourceLineOfTest
 *           void FAR *  lpvUserDataToDump
 *           uLONG       ulDataTypeAndFormat
 *           uLONG       ulDataCountToDump
 * Returns:  nothing
 * Purpose:  Display a message box with the title "Assert Failed: <lpszTestFailed>",
 *           a single line of "Reason: <lpszTestExplanation>"
 *           and optionally a dump of user data (if lpvUserDataToDump is NULL, or
 *           ulDataCountToDump is 0, then no data is displayed).
 *           The message box contains a stack dump (if implemented on the
 *           current platform), and options to Ignore the assertion, Break into
 *           the debugger, or terminate the application.  There is also a checkbox
 *           to allow the disabling of assertions from that point on.
 *           If assertions are currently disabled the message box will not be
 *           displayed, but if the current platform has the functionality implemented
 *           the file, lineno, and explanation will be dumped to the debug terminal
 *           or file.
 *
 * NOTE: This function may change in format, so it is NOT to be called
 *       directly.  Use the Macro API for assertions instead...
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_AssertProc( LPSZ tst, LPSZ msg, LPSZ fname, uLONG linno,
									 LPVOID pdata, uLONG ifmt, uLONG cnt );

	#else	/* ! defined(TPSDEBUG) */
		/* API for non-debug builds
		 */
		#define	Debug(x)
		#define  tdb_DebugBreak()							{}
		#define	tdb_DebugBeep()							{}
		#define	tdb_DebugMsg(x)							{}
		#define	tdb_DebugPrintf(x)						{}
		#define	tdb_DebugDump(fmt,ptr,count)			{}

		#define	TpsAssert(expr, msg)						{}
		#define	TpsAssertAlways(msg)						{}
		#define	TpsAssertB(expr, msg, pByte, count)	{}
		#define	TpsAssertW(expr, msg, pWord, count)	{}
		#define	TpsAssertL(expr, msg, pLong, count)	{}
		#define	TpsAssertS(expr, msg, pCstr)			{}
		#define	TpsAssertP(expr, msg, pPstr)			{}

		#define	tdb_StartupDebugLib(p,x)				{}
		#define	tdb_ShutdownDebugLib()					{}
		#define	tdb_GetDebugFlags()						{}
		#define	tdb_SetDebugFlags(flags)				{}
		#define	tdb_ClearDebugFlags(flags)				{}
		#define	tdb_ToggleDebugFlags(flags)			{}
		#define	tdb_DisableAsserts()						{}
		#define	tdb_EnableAsserts(n)						{}

		#define	tdb_AssertProc(m,f,l,p,i,c)			{}

	#endif /* TPSDEBUG */


	/* Common defines.
	 */

	/* WARNING: This macro produces a complete variable declaration
	 * and initializer, including the trailing semicolon. You should
	 * not put a semicolon after use of this macro.
	 */
	#define DEBUGSTRING(name,text)	Debug(static char READONLY name[] = text;)

	#define UnimplementedCode()		TpsAssertAlways("Unimplemented Routine")
	#define UntestedCode()				TpsAssertAlways("Untested Code")

	#if defined(__cplusplus)
		}
	#endif /* __cplusplus */


/* Declare __AssertFile__, for use by Assert() macro...
 * We do this instead of using __FILE__ directly, so there
 * will be only one instance of this string per module.
 * Each source file containing Assertions MUST include a line
 * with this macro (preferably immediately following the includes)
 * prior to using the assertion macros...
 */
#define	ASSERTNAME	DEBUGSTRING(__AssertFile__, __FILE__)

#endif /* _tpsdebug_h_ */

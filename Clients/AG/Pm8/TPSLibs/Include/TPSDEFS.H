/****************************************************************************
 *
 *	Header Name:	tpsdefs.h
 *	Description:	TPS standard definitions header.
 *	Version:			v 1.00
 *	Author:			Michael C. Robert
 *	Date:				7/11/1994
 *
 *	Copyright (C) 1994-1995 by Turning Point Software, Inc. All Rights Reserved.
 *
 ****************************************************************************
 */

#ifndef  _TPSDEFS_H_
#define  _TPSDEFS_H_

#include "FrameworkLinkage.h"

//----------------------------------------
// VALIDITY CHECK
//----------------------------------------

#if	defined(WIN16) || defined(WIN32)
	#ifndef	_WINDOWS
	#define	_WINDOWS
	#endif
#endif

#if	!defined( _WINDOWS ) && !defined( DOS ) && !defined( MAC )
	#error - no target OS defined; please define _WINDOWS, DOS, or MAC
#endif

#if	defined( _WINDOWS ) && (!defined( WIN16 ) && !defined( WIN32 ))
	#error - must define WIN16 or WIN32 if using with Windows
#endif

#if		defined( _TPSDEBUG_H_ ) || defined( _TPSERR_H_ )
	#error - you cannot include tpsdebug.h or terrdefs.h prior to tpsdefs.h
#endif

//----------------------------------------
// INCLUDE FILES
//----------------------------------------

#if		defined( MAC )
	
	//	include common set of Mac header files. This set of includes
	//	assumes the universal Mac headers, which are supported by the
	//	current versions of all Mac-hosted compilers.
	
	#include <Types.h>
	#include <Memory.h>
	#include <QuickDraw.h>
	#include <Palettes.h>
	#include <QDOffscreen.h>
	#include <Dialogs.h>
	#include <Controls.h>
	#include <Fonts.h>
	#include <OSUtils.h>
	#include <Events.h>
	#include <Resources.h>
	#include <Packages.h>
	#include <TextUtils.h>
	#include <Traps.h>
	#include <LowMem.h>
	#include <AppleEvents.h>
	#include <SegLoad.h>
	#include <Sound.h>

#elif		defined( _WINDOWS )

	#ifndef	_INC_WINDOWS
		#include <windows.h>
	#endif
	#ifndef	_INC_WINDOWSX
		// 12/10/98 Lance - Removed as it causes several problems
		// with macros matching function names.  The only side
		// effects found was that windowsx.h had to be included
		// in windlg.c
		//
		//#include <windowsx.h>
	#endif

#ifdef WIN16
	#include <toolhelp.h>
#endif // WIN16

#endif	// MAC or _WINDOWS

//----------------------------------------
// SYMBOLIC CONSTANTS
//----------------------------------------

#if		defined( MAC )

	#define	FAR
	#define	NEAR
	#define	HUGE
	#define	EXPORT	
	typedef int BOOLEAN;								 	// f

#elif		defined( WIN16 )

//	#define FAR		_far				//	defined in windows.h
//	#define NEAR	_near				//	defined in windows.h
	#define	HUGE	_huge
	#define EXPORT		__export
	typedef int BOOLEAN;								 	// f

#elif		defined( WIN32 )

//	#define	FAR						// defined in windef.h
//	#define	NEAR
	#define	HUGE
	#define EXPORT
//	typedef int BOOLEAN;				//	Win32 headers define this as a byte

#elif		defined( DOS )

	#define	FAR		_far
	#define	NEAR		_near
	#define	HUGE		_huge
	#define	EXPORT	
	typedef int BOOLEAN;								 	// f

#else

	#error	Neither MAC, WIN16, WIN32 nor DOS defined.

#endif	//	MAC, WIN16, WIN32 or DOS

#if		defined( MAC )

	#define	VOID		void
	typedef	void FAR *LPVOID;

	#define	PASCAL	pascal
	#define	CDECL

	#define	TPSAPI_DECLP( type ) FAR PASCAL type
	#define	TPSAPI_DECLC( type ) FAR CDECL type

	#define	MacCode(x)					x
	#define	WinCode(x)
	#define	DosCode(x)
	#define	MacWinDos(x,y,z)			x

#elif		defined( _WINDOWS )

//	#define	VOID		void				//	defined in windows.h
//	typedef	void FAR *LPVOID;			//	defined in windows.h

//	#define PASCAL		_pascal			//	defined in windows.h
//	#define CDECL		_cdecl			//	defined in windows.h

	#define	TPSAPI_DECLP( type ) FrameworkLinkage type FAR PASCAL
	#define	TPSAPI_DECLC( type ) FrameworkLinkage type FAR CDECL

	#define	MacCode(x)
	#define	WinCode(x)					x
	#define	DosCode(x)
	#define	MacWinDos(x,y,z)			y

#elif		defined( DOS )

	#define	VOID		void
	typedef	void FAR *	LPVOID;

	#define	PASCAL	pascal
	#define	CDECL		cdecl

	#define	TPSAPI_DECLP( type ) type FAR PASCAL
	#define	TPSAPI_DECLC( type ) type FAR CDECL

	#define	MacCode(x)
	#define	WinCode(x)
	#define	DosCode(x)					x
	#define	MacWinDos(x,y,z)			z

#else

	#error	Neither MAC, _WINDOWS, nor DOS defined.

#endif	//	MAC, _WINDOWS or DOS

#undef	FALSE
#define	FALSE		0

#undef	TRUE
#define	TRUE	(!FALSE)

#ifndef	NULL
#define	NULL	0
#endif	//	NULL

//----------------------------------------
// TYPEDEFS
//----------------------------------------

typedef char sBYTE;									// sb
typedef unsigned char uBYTE;						// ub

typedef short sWORD;									// sw
typedef unsigned short uWORD;						// uw

typedef long sLONG;									// sl
typedef unsigned long uLONG;						// ul

//	Add the pointer versions of the above defined types

typedef	void			*PVOID;						//	pv
typedef	void	NEAR	*NPVOID;						//	npv
typedef	void	HUGE	*HPVOID;						//	hpv

typedef	sBYTE			*PSBYTE;						//	psb
typedef	sBYTE NEAR	*NPSBYTE;					//	npsb
typedef	sBYTE FAR	*LPSBYTE;					//	lpsb
typedef	sBYTE HUGE	*HPSBYTE;					//	hpsb

typedef	uBYTE			*PUBYTE;						//	pub
typedef	uBYTE	NEAR	*NPUBYTE;					//	npub
typedef	uBYTE FAR	*LPUBYTE;					//	lpub
typedef	uBYTE HUGE	*HPUBYTE;					//	hpub

//typedef	sWORD			*PSWORD;						//	psw	-- Removed because of conflict with INSO libraries
typedef	sWORD NEAR	*NPSWORD;					//	npsw
typedef	sWORD FAR	*LPSWORD;					//	lpsw
typedef	sWORD HUGE	*HPSWORD;					//	hpsw

//typedef	uWORD			*PUWORD;						//	puw	-- Removed because of conflict with INSO libraries
typedef	uWORD	NEAR	*NPUWORD;					//	npuw
typedef	uWORD FAR	*LPUWORD;					//	lpuw
typedef	uWORD HUGE	*HPUWORD;					//	hpuw

typedef	sLONG			*PSLONG;						//	psl
typedef	sLONG NEAR	*NPSLONG;					//	npsl
typedef	sLONG FAR	*LPSLONG;					//	lpsl
typedef	sLONG HUGE	*HPSLONG;					//	hpsl

typedef	uLONG			*PULONG;						//	pul
typedef	uLONG	NEAR	*NPULONG;					//	npul
typedef	uLONG FAR	*LPULONG;					//	lpul
typedef	uLONG HUGE	*HPULONG;					//	hpul

typedef	BOOLEAN		 *PBOOLEAN;					//	pb
typedef	BOOLEAN NEAR *NPBOOLEAN;				// npb
typedef	BOOLEAN FAR	 *LPBOOLEAN;				//	lpb
typedef	BOOLEAN HUGE *HPBOOLEAN;				//	hpb


//	This definition of SZ must be char,  NOT unsigned char or signed char.
//	this is required for the Macintosh where a hard coded string is of type char *.
//	This if this is other than char, MSC with only give a warning at /W4.
typedef char SZ;										// sz
typedef SZ 			*PSZ;								// psz
typedef SZ NEAR	*NPSZ;							// npsz
typedef SZ FAR		*LPSZ;							// lpsz
typedef SZ HUGE	*HPSZ;							// hpsz

//	Const versions of strings
typedef const SZ FAR*		LPCSZ;
typedef const uBYTE FAR*	LPCUBYTE;
typedef const sBYTE FAR*	LPCSBYTE;

//----------------------------------------
// MACROS
//----------------------------------------

#ifdef WIN32

	#define READONLY

#elif	defined( _WINDOWS ) || defined( DOS )

	#if	!defined( _MSC_VER )	//	this might be Borland...
		#define  READONLY
	#elif	 _MSC_VER == 600							/* _MSC_VER */
		#define 	READONLY	_based( _segname( "_CODE" ) )
	#elif	 _MSC_VER >= 700							/* _MSC_VER */
		#define	READONLY	__based( __segname( "_CODE" ) )
	#else	 // _MSC_VER < 700 && _MSC_VER != 600
		#define  READONLY
	#endif	// _MSC_VER

#else

	#define	READONLY

#endif

#ifndef _TPSDEBUG_H_
#include "tpsdebug.h"
#endif	/* _TPSDEBUG_H_ */

#ifndef  _TPSERR_H_
#include "terrdefs.h"
#endif	/* _TPSERR_H_ */

#ifdef WIN32
	#undef HUGE					// defined in math.h ??!!??
#endif

#endif	/* _TPSDEFS_H_ */

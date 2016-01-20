/**************************************************************************
	Dilaog Box and Item Templates Header File
	Written by Atif Aziz, November 1993
**************************************************************************/

#ifndef _DLGRES_H_
#define _DLGRES_H_

#include "winxext.h"

// Ordinal scheme constant
#define RCH_ORDINAL_SCHEME	((RCHAR) -1)

#if !defined(WIN32)	// Already defined in Win32

	// Dialog box template (dt)

typedef struct tagDLGTEMPLATE
{
	DWORD	style;
	BYTE	cdit;
	WORD	x;
	WORD	y;
	WORD	cx;
	WORD	cy;
}
DLGTEMPLATE;

TYPEDEF_POINTERS(DLGTEMPLATE, DLGTEMPLATE)

#endif

	// Dilaog menu template (vdmt)

typedef union tagDLGMENUTEMPLATE
{
	struct
	{
		RCHAR	rchScheme;
		WORD	wOrd;
	}
		alt;

	RCHAR	szName[];
}
DLGMENUTEMPLATE;

TYPEDEF_POINTERS(DLGMENUTEMPLATE, DLGMENUTEMPLATE)

	// Dialog class template (vdct)

typedef union tagDLGCLASSTEMPLATE
{
	struct
	{
		RCHAR	rchScheme;
		#ifdef WIN32
		WORD	wAtom;
		#endif
	}
		alt;

	RCHAR	szName[];
}
DLGCLASSTEMPLATE;

TYPEDEF_POINTERS(DLGCLASSTEMPLATE, DLGCLASSTEMPLATE)

	// Dialog font template (vdft)

typedef struct tagDLGFONTTEMPLATE
{
	WORD	wSize;
	RCHAR	szName[];
}
DLGFONTTEMPLATE;

TYPEDEF_POINTERS(DLGFONTTEMPLATE, DLGFONTTEMPLATE)

#if !defined(WIN32)	// Already defined in Win32

	// Dialog box item template (dit)

typedef struct tagDLGITEMTEMPLATE
{
	int	x;
	int	y;
	int	cx;
	int	cy;
	WORD	id;
	DWORD	style;
}
DLGITEMTEMPLATE;

TYPEDEF_POINTERS(DLGITEMTEMPLATE, DLGITEMTEMPLATE)

#endif

	// Dialog item class template (dict)

typedef union tagDLGITEMCLASSTEMPLATE
{
	struct
	{
		#ifdef WIN32
		RCHAR	rchScheme;
		#endif
		RCHAR	rchAtom;
	}
		alt;

	RCHAR	szName[];
}
DLGITEMCLASSTEMPLATE;

TYPEDEF_POINTERS(DLGITEMCLASSTEMPLATE, DLGITEMCLASSTEMPLATE)

	// Dialog item title temaplte (ditt)

typedef union tagDLGITEMTITLETEMPLATE
{
	struct
	{
		RCHAR	rchScheme;
		WORD	wOrd;
	}
		alt;

	RCHAR	szTitle[];
}
DLGITEMTITLETEMPLATE;

TYPEDEF_POINTERS(DLGITEMTITLETEMPLATE, DLGITEMTITLETEMPLATE)

	// Dialog item data template (didt)

typedef struct tagDLGITEMDATATEMPLATE
{
	RCHAR	rchSize;
	BYTE	bData[];
}
DLGITEMDATATEMPLATE;

TYPEDEF_POINTERS(DLGITEMDATATEMPLATE, DLGITEMDATATEMPLATE)

#endif 

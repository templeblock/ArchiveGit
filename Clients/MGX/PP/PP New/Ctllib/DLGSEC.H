/**************************************************************************
	Dynamic Dialog Boxes
	Header File for Dynamic Dialog Box Section Management
	Written by Atif Aziz, May 1993
**************************************************************************/

#ifndef _DLGSEC_H_
#define _DLGSEC_H_

#include "dlgcat.h"

	// Constants

#define DLGSEC_ERR		-1

	// Section category flags

#define DSCF_DISCARDABLE	0x0001
#define DSCF_PRELOAD		0x0002

	// Dialog section notification messages

#define DSN_LOADED		0x0001
#define DSN_DISCARDING		0x0002

	// Category wrapper structure (seccat)

typedef struct tagSECTIONCATEGORY
{
	int		nID;
        UINT		uFlags;
	DLGCATEGORY	dlgcat;
}
SECTIONCATEGORY;

TYPEDEF_POINTERS(SECTIONCATEGORY, SECTIONCATEGORY)

	// Dialog section structure (dlgsec)

typedef struct tagDLGSECTION
{
	int			nID;
	HWND			hwndDlg;
	int			nFrameID;
	int			nCount;
	int			nLoaded;
	int			nMaxThreshold;
	int			nMinThreshold;
	int			nCurrentID;
	LPVOID			lpvData;
	LPSECTIONCATEGORY	lpseccatList;
}
DLGSECTION;

TYPEDEF_POINTERS(DLGSECTION, DLGSECTION)

	// Function prototypes

	// Section functions        	

BOOL		InitDlgSection			(LPDLGSECTION, int, HWND, int, LPVOID);
BOOL		DeleteDlgSection		(LPDLGSECTION);
BOOL		SetDlgSectionThreshold		(LPDLGSECTION, int, int);
                                        	
	// Section category functions

int		AddDlgSectionCategory		(LPDLGSECTION, HINSTANCE, LPCTSTR, int, UINT, LPVOID);
int		DeleteDlgSectionCategory	(LPDLGSECTION, int);
BOOL		SelectDlgSectionCategory	(LPDLGSECTION, int);
int		GetDlgSectionCurrentID		(LPCDLGSECTION);
int		GetDlgSectionCount		(LPCDLGSECTION);
LPVOID		GetDlgSectionData		(LPCDLGSECTION);

#endif	// _DLGSEC_H_

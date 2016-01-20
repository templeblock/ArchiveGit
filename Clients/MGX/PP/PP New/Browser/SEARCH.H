// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "types.h"
#include "dbfmgr.h"

typedef struct _searchspec {
	int 	idSearchBy;
	int 	idSearchIn;
	FNAME 	szSearchString;
	BOOL 	bCaseSensitive;
	FNAME	szSearchDir;
	FNAME 	szSelectedAlb;
	HWND	hBrowser;
	DBFS	dbfResult;
	// internals
	HWND	hSearch;
	int 	numSearched;
	int 	numFound;
	}	SEARCHSPEC;
	
// Fill before calling
extern SEARCHSPEC FileSearch;

BOOL WINPROC DlgSearchProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

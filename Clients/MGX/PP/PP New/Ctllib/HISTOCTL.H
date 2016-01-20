// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// mapctl.h for map control

#ifdef CTLLIB
BOOL Histo_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Histo_UnRegisterClass (HINSTANCE hInstance);
#endif
long WINPROC Histo_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL Histo_Init(HWND hHisto, LPDWORD lpHisto, LONG lMaxValue, int nMarks, WORD wStyles);
void Histo_SetColor(HWND hHisto, RGBS Color);
int  Histo_SetMark(HWND hHisto, int nMark, int iValue, BOOL bConstrain);
int  Histo_GetMark(HWND hHisto, int nMark);
int  Histo_GetBrowse(HWND hHisto);


#define HISTO_BAD_MARKER	-1
// styles
#define HTS_FLIPX		0x0001			// flip x axis (histo only.. not marks)

// maximum number of lines
#define MAX_MARKS 3

// notifications 
#define HTN_CHANGED_FIRST  	1			// Change in 1st mark 
#define HTN_CHANGED_LAST	(HTN_CHANGED_FIRST+MAX_MARKS) // Change in last mark
#define HTN_CHANGING_FIRST	(HTN_CHANGED_LAST+1)		// 1st mark Changing 
#define HTN_CHANGING_LAST	(HTN_CHANGING_FIRST+MAX_MARKS) // Last mark Changing
#define HTN_BROWSE	HTN_CHANGING_LAST+1	// just browsing... no mouse down

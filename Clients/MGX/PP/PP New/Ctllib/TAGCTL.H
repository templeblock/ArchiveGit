// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// tagctl.h for tag control

#ifdef CTLLIB
BOOL Tag_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Tag_UnRegisterClass (HINSTANCE hInstance);
#endif

long WINPROC Tag_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
BOOL Tag_Init(HWND hTag, int nMarks, int iMaxValue, LPINT lpIconId);
int  Tag_SetMark(HWND hTag, int nMark, int iValue, BOOL bConstrain);
int  Tag_GetMark(HWND hTag, int nMark);


#define TAG_BAD_MARKER	-1
// styles

// maximum number of lines
#define MAX_TAG_MARKS 3

// notifications 
#define TN_CHANGED_FIRST  	1			// Change in 1st mark 
#define TN_CHANGED_LAST	(TN_CHANGED_FIRST+MAX_TAG_MARKS) // Change in last mark
#define TN_CHANGING_FIRST	(TN_CHANGED_LAST+1)		// 1st mark Changing 
#define TN_CHANGING_LAST	(TN_CHANGING_FIRST+MAX_TAG_MARKS) // Last mark Changing


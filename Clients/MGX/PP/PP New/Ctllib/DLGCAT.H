/**************************************************************************
	Dynamic Dialog Boxes
	Header File for Dynamic Dialog Box Category Management
	Written by Atif Aziz, May 1993
**************************************************************************/

#ifndef _DLGCAT_H_
#define _DLGCAT_H_

#include "dlgres.h"

	// ShowDlgCategory constants

#define SDC_SHOW	(TRUE)
#define SDC_HIDE	(FALSE)

	// Dialog Box Section (dlgcat)

typedef struct tagDLGCATEGORY
{
	HINSTANCE		hinst;
	HGLOBAL			hEditDS;
	LPCTSTR			lpcszTemplateName;
	LPVOID			lpvData;

	// --- the following are used internally

	HGLOBAL			hglbTemplate;
        int			nItemCount;	
	LPDLGITEMTEMPLATE	lpditList;
	HLOCAL			hlocWndList;
}
DLGCATEGORY;

TYPEDEF_POINTERS(DLGCATEGORY, DLGCATEGORY)

	// Function prototypes

BOOL			LoadDlgCategory			(LPDLGCATEGORY);
BOOL			DeleteDlgCategory		(LPDLGCATEGORY);
BOOL			DestroyDlgCategoryControls	(LPDLGCATEGORY);
BOOL			CreateDlgCategoryIndirect	(LPDLGCATEGORY, HWND, int);
BOOL			ShowDlgCategory			(LPDLGCATEGORY, BOOL);

#define GetDlgCategoryData(lpdlgcat)	((dlgcat)->lpvData)

#endif	// _DLGCAT_H_

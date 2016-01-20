/*
// $Header: /PM8/App/ADDON.CPP 1     3/03/99 6:01p Gbeddow $
//
// Glue code between PMW and the CMFAddonDll object.
//
// $Log: /PM8/App/ADDON.CPP $
// 
// 1     3/03/99 6:01p Gbeddow
// 
// 2     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:18:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:24:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:26:18   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:33:28   JAY
// Reverted. New series.
// 
//    Rev 1.11   06 Oct 1994 15:17:20   JAY
// Added IDD_ADDON_FONTS
// 
//    Rev 1.10   19 Sep 1994 09:56:48   JAY
// Now can install free fonts from subdirectory.
// 
//    Rev 1.9   04 Aug 1994 07:35:10   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   26 May 1994 13:48:42   JAY
// Now uses add_slash().
// 
//    Rev 1.7   23 May 1994 09:08:22   JAY
// Needed to include "misc.h" before pathmgr.h
// 
//    Rev 1.6   12 May 1994 07:37:30   JAY
// Now uses path manager instead of APP working_directory.
// 
//    Rev 1.5   07 Feb 1994 12:05:42   JAY
// No longer sets "NeverDeleteOtls" as default case.
// 
//    Rev 1.4   02 Feb 1994 09:32:52   JAY
// Removed the delete_otls routine.
// 
//    Rev 1.3   30 Jan 1994 12:27:04   JAY
//  
// 
//    Rev 1.2   28 Jan 1994 17:12:26   JAY
//  
// 
//    Rev 1.1   26 Jan 1994 16:36:46   JAY
//  
// 
//    Rev 1.0   13 Jan 1994 17:03:10   JAY
//  
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"

#include "addon.h"
#include "cmfaddon.h"
#include "utils.h"
#include "util.h"

#if 0
/*
// This assumes that 'buffer' is of size _MAX_PATH.
// This is currently only used for first-time initialization.
*/

static VOID near
do_delete_otls(char *buffer)
{
	char *end = buffer + strlen(buffer);

	append_slash(buffer);

	char *tail = buffer + strlen(buffer);

	strcpy(tail, "*.*");

	struct find_t fbuffer;
	int ret;

	for (ret = _dos_findfirst(buffer, _A_NORMAL, &fbuffer);
				 ret == 0;
				 ret = _dos_findnext(&fbuffer))
	{
		strcpy(tail, fbuffer.name);
		unlink(buffer);
	}

	*end = '\0';
}
#endif

int add_on_fonts(BOOL allow_free_fonts, BOOL only_free_fonts, LPSTR source, BOOL delete_otls)
{
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();
	CWnd *pMainWnd = pApp->m_pMainWnd;

	IPathManager* pPathManager = GetGlobalPathManager();

	CMFAddonDll dll(pPathManager->ExpandPath("MFADDON.DLL"));

	if (!dll.Valid())
	{
	/* Can't do this. */
		AfxMessageBox("MFADDON.DLL is missing or bad.", MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}

	rAddonProcessFontsData data;

	/* Do this first so we have it. */

	strcpy(data.ApplicationDirectory, pPathManager->GetWorkingDirectory());

	data.RecordLength = sizeof(data);

	data.Flags = delete_otls
					? AddonFlagAlwaysDeleteOtlFonts
					: 0;
//					: AddonFlagNeverDeleteOtlFonts;

	data.AskForSource = AddonAskAlways;

	if (only_free_fonts)
	{
		data.Flags |= AddonFlagOnlyRegistration;

		if (source != NULL)
		{
			data.AskForSource = AddonAskIfNotValid;
		}
	}
	else if (source != NULL)
	{
		data.Flags |= AddonFlagInstallAll;
		data.AskForSource = AddonAskNever;
	}
	else if (allow_free_fonts)
	{
		data.Flags |= AddonFlagIncludeRegistration;
	}

	strcpy(data.TrueTypeDll, data.ApplicationDirectory);
	strcat(data.TrueTypeDll, "MFOTL2TT.DLL");

	data.PostScriptDll[0] = '\0';

	strcpy(data.SymbolOtl, data.ApplicationDirectory);
	strcat(data.SymbolOtl, "SYMBOL.OTL");

	strcpy(data.SymbolSet, data.ApplicationDirectory);
	strcat(data.SymbolSet, "SYMBOL.SET");

	strcpy(data.TempDirectory, data.ApplicationDirectory);

	data.ConversionType = AddonTrueTypeConversion;
	strcpy(data.Source, source == NULL ? "A:\\" : source);

	GetSystemDirectory(data.TTFDirectory, sizeof(data.TTFDirectory));
	data.PFBDirectory[0] = '\0';
	data.PFMDirectory[0] = '\0';

	dll.ProcessFonts(pMainWnd->GetSafeHwnd(), &data);

#if 0
	if (delete_otls && source != NULL)
	{
		do_delete_otls(source);
	}
#endif

	return data.TrueTypeFontsInstalled;
}

/////////////////////////////////////////////////////////////////////////////
// CAddonFontDialog dialog

CAddonFontDialog::CAddonFontDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAddonFontDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddonFontDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAddonFontDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddonFontDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddonFontDialog, CDialog)
	//{{AFX_MSG_MAP(CAddonFontDialog)
	ON_BN_CLICKED(ID_CONTINUE, OnContinue)
	ON_BN_CLICKED(ID_ADD_FREE_FONTS, OnAddFreeFonts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddonFontDialog message handlers

void CAddonFontDialog::OnContinue()
{
	EndDialog(ID_CONTINUE);
}

void CAddonFontDialog::OnAddFreeFonts()
{
	EndDialog(ID_ADD_FREE_FONTS);
}

BOOL CAddonFontDialog::OnInitDialog()
{
/* Make sure we're centered. */

	Util::CenterWindowInWindow(this);

	return CDialog::OnInitDialog();
}

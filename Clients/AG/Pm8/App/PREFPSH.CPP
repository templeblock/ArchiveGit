/*
// $Workfile: PREFPSH.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFPSH.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 12    1/14/99 9:32a Cschende
// added functionality to the Date format tab
// 
// 11    1/07/99 4:41p Cschende
// added the "date" tab property page to the property sheet
// 
// 10    11/20/98 1:33p Mwilson
// added capability to turn off auto instructions in preferences
// 
// 9     10/15/98 3:47p Dennis
// New OCX Connection Manager.
// 
// 8     7/17/98 5:50p Johno
// Added WebPublishWarnPut() in CPreferencesSheet::DoModal()
// 
// 7     6/03/98 1:41p Johno
// Added web page design checker control
// 
// 6     5/28/98 2:54p Hforman
// remove Welcome stuff, add voice for Helpful Hints
// 
// 5     5/27/98 7:01p Hforman
// remove welcome stuff
// 
// 4     2/04/98 10:44a Dennis
// Added Online Preferences.  Removed dead sound code.  Preferences dialog
// code now be used by static method or by normal object construction.
// 
// 3     1/28/98 5:39p Dennis
// Added PreferencesOnlinePage
// 
// 2     12/16/97 5:38p Hforman
// m_WelcomePage now m_HelpPage; added Helpful Hints stuff
// 
//    Rev 1.0   14 Aug 1997 15:24:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:02   Fred
// Initial revision.
// 
//    Rev 1.6   22 May 1997 12:53:06   johno
// 
//    Rev 1.5   21 May 1997 18:14:18   johno
// Changes for welcome dialogs
// 
//    Rev 1.4   21 Apr 1997 16:55:20   johno
// Changes for welcome dialogs
// 
// 
//    Rev 1.3   28 Mar 1997 17:10:58   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.2   09 Jan 1997 16:19:44   dennisp
// Changes to make workspace not shared.
// 
//    Rev 1.1   25 Jul 1996 11:45:38   Fred
// Address argument for starting page
// 
//    Rev 1.0   14 Mar 1996 13:34:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:30   FRED
// Initial revision.
// 
//    Rev 1.3   02 Sep 1995 15:07:22   FRED
// More changes to make help work
// 
//    Rev 1.2   16 Aug 1995 16:22:24   JAY
// New stuff.
// 
//    Rev 1.1   02 Aug 1995 17:01:40   JAY
// Only adds pages for those things which are available.
// 
//    Rev 1.0   16 Jul 1995 15:08:14   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "prefpsh.h"
#include "pmwdlg.h"
#include "pmwini.h"
#include "inifile.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSheet

IMPLEMENT_DYNAMIC(CPreferencesSheet, CPmwPropertySheet)

CPreferencesSheet::CPreferencesSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPmwPropertySheet(nIDCaption, pParentWnd, iSelectPage),
		m_SoundPage(),
		m_WorkspacePage(),
      m_HelpPage()
{
	AddPages();
}

CPreferencesSheet::~CPreferencesSheet()
{
}


BEGIN_MESSAGE_MAP(CPreferencesSheet, CPmwPropertySheet)
	//{{AFX_MSG_MAP(CPreferencesSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPreferencesSheet::AddPages(void)
{
//	if (sound_manager.sounds_available())
	{
		AddPage(&m_SoundPage);
	}

   AddPage(&m_HelpPage);

	if (cd_cache.is_open())
	{
		AddPage(&m_WorkspacePage);
	}

	AddPage(&m_DatePage);
}

int CPreferencesSheet::DoModal()
{
   int         nRetCode;
   CPmwApp*    pApp = GET_PMWAPP();

	// Set initial data for sound page.
   m_SoundPage.m_SoundEnabled = pApp->m_fSoundEffectsState;

	// Set initial data for workspace page.
	CString csWorkspaceINI;

	if (cd_cache.is_open())
	{
		m_WorkspacePage.m_pCache = &cd_cache;
	
		TRY
		{
			pApp->get_workspace_names(NULL, &csWorkspaceINI);
			cd_cache.set_cache_size(pApp->get_workspace_size());
		}
		CATCH_ALL(e)
		{
			m_WorkspacePage.m_pCache = NULL;
		}
		END_CATCH_ALL
	}
	else
	{
		m_WorkspacePage.m_pCache = NULL;
	}

	// Set initial data for Help page.

   CString  INIFileName = pApp->m_pszProfileName;
   CIniFile INIFile (INIFileName);

	m_HelpPage.m_fShowHelpfulHints = pApp->HelpfulHintsEnabled();
	m_HelpPage.m_fShowDesignTips = pApp->DesignTipsEnabled();
	m_HelpPage.m_fPlayVoice = pApp->HintsVoiceEnabled();
	m_HelpPage.m_fAutoInstructions = pApp->AutoInstructionsEnabled();

   // Put up the property 
   if ((nRetCode = CPmwPropertySheet::DoModal()) == IDOK)
	{
		// Unpack new workspace settings
		if (cd_cache.is_open())
		{
			DWORD dwNewSize = m_WorkspacePage.m_dwWorkspaceSize;

			TRY
			{
				// Generate the string.
				char buffer[50];
				sprintf(buffer, "%ld", dwNewSize);

				// Write the setting in users NID .INI file - DGP.
      		pApp->WriteProfileString(SECTION_Workspace, ENTRY_WorkspaceSize, buffer);

				// Now, write it to the cache file.

				switch (cd_cache.set_cache_size(dwNewSize))
				{
					case ERRORCODE_None:
					{
						// This is the good case.
						break;
					}
					default:
					{
						// Some error. The file may be in use.
						AfxMessageBox(IDS_ErrCantShrinkCache, MB_OK | MB_ICONEXCLAMATION);
						break;
					}
				}
			}
			CATCH_ALL(e)
			{
			}
			END_CATCH_ALL
		}

		// Helpful Hints
		pApp->EnableHelpfulHints(m_HelpPage.m_fShowHelpfulHints);
		pApp->EnableDesignTips(m_HelpPage.m_fShowDesignTips);
		pApp->EnableHintsVoice(m_HelpPage.m_fPlayVoice);
		pApp->EnableAutoInstructions(m_HelpPage.m_fAutoInstructions);

      if (pApp->m_fSoundEffectsState != m_SoundPage.m_SoundEnabled)
      {
         TRY
		   {
            INIFile.WriteUnsignedInteger ("Configuration", "Sound Effects", (UINT)m_SoundPage.m_SoundEnabled);
		   }
		   CATCH_ALL(e)
		   {
		   }
		   END_CATCH_ALL
      }

      pApp->m_fSoundEffectsState = m_SoundPage.m_SoundEnabled;


      if (pApp->m_eDateFormat != m_DatePage.m_nDateFormat)
      {
			pApp->m_eDateFormat = (EPDateFormat)m_DatePage.m_nDateFormat;
         TRY
		   {
            INIFile.WriteUnsignedInteger ("Configuration", "Date Format", (UINT)m_DatePage.m_nDateFormat);
		   }
		   CATCH_ALL(e)
		   {
		   }
		   END_CATCH_ALL
      }

   }

   return nRetCode;
}

int CPreferencesSheet::SelectPreferences(int nPage /*=0*/)
{
	CPreferencesSheet Sheet(IDS_PreferencesSheetCaption, NULL, nPage);

   return Sheet.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesSheet message handlers

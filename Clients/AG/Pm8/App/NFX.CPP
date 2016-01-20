/*
// $Workfile: NFX.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/NFX.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:22:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:22   Fred
// Initial revision.
// 
//    Rev 1.7   14 Jul 1997 13:11:42   Jay
// BeginModalState totally disables window in 16-bit
// 
//    Rev 1.6   26 Jun 1997 16:14:32   Jay
// Minor change.
// 
//    Rev 1.5   26 Jun 1997 12:18:22   Jay
// Got rid of a hanging (race) condition.
// 
//    Rev 1.4   22 May 1997 18:09:38   Jay
// Writes CARTOON.INI.
// 
//    Rev 1.3   22 May 1997 16:14:12   Jay
// Renamed TOONOPRO.EXE to CARTOON.EXE
// 
//    Rev 1.2   09 Apr 1997 17:12:58   Jay
// DoWinExec to Util
// 
//    Rev 1.1   31 Mar 1997 17:12:32   Jay
// Start of NFX code
// 
//    Rev 1.0   28 Mar 1997 17:10:20   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "mainfrm.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "inifile.h"
#include "nfx.h"
#include "util.h"
#include "pmwcfg.h"

static const char BASED_CODE cbTerminationMessage[] = "ToonPro2PmTermination";
UINT CCartoonOMatic::m_uTerminationMessage = ::RegisterWindowMessage(cbTerminationMessage);
BOOL CCartoonOMatic::m_fTerminated = FALSE;
BOOL CCartoonOMatic::m_fDisabled = FALSE;

static const char BASED_CODE cbCartoonExe[] = "NFX\\CARTOON.EXE";
static const char BASED_CODE cbCartoonIni[] = "NFX\\CARTOON.INI";

// INI file communication strings.
static char cbNFXTemplate[5+1] = "~xxxN";
static const char BASED_CODE SECTION_NFX[] = "NFX";
static const char BASED_CODE ENTRY_TerminationMessage[] = "TerminationMessage";
static const char BASED_CODE ENTRY_ModelName[] = "ModelName";
static const char BASED_CODE ENTRY_ModelState[] = "ModelState";
static const char BASED_CODE ENTRY_UserResult[] = "UserResult";
static const char BASED_CODE ENTRY_OutputFile[] = "OutputFile";

/////////////////////////////////////////////////////////////////////////////
// CCartoonOMatic

CCartoonOMatic::CCartoonOMatic()
{
	// Nid the filename (in case it has not been yet).
	GET_PMWAPP()->nid_filename(cbNFXTemplate);

	// No result yet.
	m_nUserResult = -1;
}

// Create a new toon.
BOOL CCartoonOMatic::NewToon(void)
{
	m_csModelName.Empty();
	m_csModelState.Empty();

	return DoToon();
}

// Edit an existing toon.
BOOL CCartoonOMatic::EditToon(LPCSTR pszModelName, LPCSTR pszModelState)
{
	m_csModelName = pszModelName;
	m_csModelState = pszModelState;

	return DoToon();
}

CCartoonOMatic::~CCartoonOMatic()
{
	// Delete the ini file.
	ReleaseFile(m_csIniFile);
	// Delete the output file.
	ReleaseFile(m_csOutputFile);
}

void CCartoonOMatic::ReleaseFile(CString& csFile)
{
	if (!csFile.IsEmpty())
	{
		TRY
		{
			CFile::Remove(csFile);
		}
		END_TRY
		csFile.Empty();
	}
}

// Process a toon.
BOOL CCartoonOMatic::DoToon(void)
{
   BOOL fResult = FALSE;

	// Not terminated yet.
	m_fTerminated = FALSE;

	// Not disabled yet.
	m_fDisabled = FALSE;

	// Assume no result.
	m_nUserResult = -1;

	// Generate a tmp name for the communications INI file.
	MakeTempName(m_csIniFile);

	// Create the communications INI file.
	CIniFile IniFile(m_csIniFile);

	// Write the termination message.
	IniFile.WriteString(SECTION_NFX,
							  ENTRY_TerminationMessage,
							  cbTerminationMessage);

	// Generate a tmp name for the output file.
	MakeTempName(m_csOutputFile);

	// Write the output file name.
	IniFile.WriteString(SECTION_NFX,
							  ENTRY_OutputFile,
							  m_csOutputFile);

	IPathManager* pPathManager = GetGlobalPathManager();

	// We need to setup the CARTOON.INI file so that the model path is
	// correct.
#if 0
	// Look for the signature file. This will make sure the home CD is
	// in the drive.
	CString csIDFile = "[[H]]\\";
	csIDFile += GetConfiguration()->GetSignatureFile();
	pPathManager->LocatePath(csIDFile);
#endif

	// Build the ini name.
	CString csIniName;
	csIniName = pPathManager->ExpandPath(cbCartoonIni);
	// Write the string.
	CIniFile CartoonIni(csIniName);
	CartoonIni.WriteString("Paths", "Models1", pPathManager->LocatePath("[[H]]\\NFXMODEL"));

	// Build the exec name.
	CString csExecName;
	csExecName = pPathManager->ExpandPath(cbCartoonExe);
	csExecName += " -i ";
	csExecName += m_csIniFile;

	// Execute the NFX module.
	// m_fTerminated will be set if we get the termination message before
	// the DoWinExec returns.
	if (Util::DoWinExec(csExecName, SW_SHOW, TRUE) && !m_fTerminated)
	{
		((CMainFrame*)AfxGetMainWnd())->BeginModalState(TRUE);
		m_fDisabled = TRUE;

		LONG lIdleCount = 0;
		CWinApp *pApp = AfxGetApp();

		// Wait for the thing to finish.
		for (;;)
		{
			MSG msg;
			if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
            TRACE("NFX message: %u (%u)\n", msg.message, m_uTerminationMessage);
				if (msg.message == m_uTerminationMessage)
				{
					// We are done!
               AfxMessageBox("Got termination message!");
					break;
				}
				else if (m_fTerminated)
				{
					break;
				}
            else
            {
               if (!pApp->PumpMessage())
               {
                  PostQuitMessage(0);
                  break;
               }
            }
			}
			else if (!pApp->OnIdle(lIdleCount++))
			{
				lIdleCount = 0;
				WaitMessage();
			}
		}

		// We have broken out of our loop. See what happened.
		m_nUserResult = IniFile.GetInteger(SECTION_NFX,
														 ENTRY_UserResult,
														 -1);
      if (m_nUserResult != -1 && m_nUserResult)
      {
         m_csModelName = IniFile.GetString(SECTION_NFX, ENTRY_ModelName);
         m_csModelState = IniFile.GetString(SECTION_NFX, ENTRY_ModelState);
         fResult = TRUE;
      }

		if (!m_fTerminated)
		{
			((CMainFrame*)AfxGetMainWnd())->EndModalState(TRUE);
			m_fDisabled = FALSE;
		}
	}

	return fResult;
}

/*
// Make a temporary NFX name.
*/

void CCartoonOMatic::MakeTempName(CString& csName)
{
	TmpFile File(cbNFXTemplate);
	csName = File.get_name();
}

void CCartoonOMatic::GotTerminationMessage(void)
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	m_fTerminated = TRUE;

	if (m_fDisabled)
	{
		pMainFrm->EndModalState(TRUE);
		m_fDisabled = FALSE;
	}

#ifdef WIN32
	pMainFrm->SetForegroundWindow();
#endif
	pMainFrm->BringWindowToTop();
}

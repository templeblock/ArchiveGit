//
// $Workfile: HelpHint.cpp $
// $Revision: 2 $
// $Date: 3/08/99 4:50p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/HelpHint.cpp $
// 
// 2     3/08/99 4:50p Johno
// User's drive now searched for voice files
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 56    11/23/98 1:27p Mwilson
// Added creataparty configuration
// 
// 55    10/12/98 1:25p Mwilson
// added craft deluxe
// 
// 54    9/28/98 7:24p Hforman
// Removed all references to "Daily Tip", since it's no longer used.
// 
// 53    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 52    7/30/98 4:39p Hforman
// 
// 51    7/30/98 4:36p Hforman
// fixed modal hints crash problems
// 
// 50    7/25/98 1:48p Jayn
// Fixed a font selection problem
// 
// 49    7/23/98 8:42a Jayn
// Does not prompt for the CD when playing sounds.
// 
// 48    7/21/98 11:16a Mwilson
// changed character id
// 
// 47    7/20/98 12:43p Psasse
// re-insert deleted lines pertaining to dragging modal helpful hints
// 
// 46    7/19/98 6:25p Jayn
// OK. So hints and tips are the same.
// 
// 45    7/18/98 7:34p Psasse
// Added ON_WM_KEYDOWN  to the message maps of Modal and Modeless classes
// to handle destruction
// 
// 44    7/18/98 6:26p Jayn
// "Don't show hint/tip" addition (from Dennis)
// 
// 42    7/16/98 3:14p Mwilson
// changed string comparision to be case insensitive
// 
// 41    7/08/98 7:52p Hforman
// fixed crash bug: pressing tutorial or help button on Modal hint;
// fixed problem with workspace hints not being shown
// 
// 40    7/01/98 5:29p Mwilson
// changed to use transparent blt
// 
// 39    7/01/98 11:56a Hforman
// 
// 38    6/30/98 6:09p Hforman
// drag preview rect instead of whole window, other tweaks to wnd
// 
// 37    6/24/98 4:40p Dennis
// AG Specific color changes
// 
// 36    6/19/98 6:09p Hforman
// add flag to deal with bogus mouse events
// 
// 35    6/15/98 6:26p Hforman
// BUG: remove helpful hint AFTER showing tutorial or help
// 
// 34    6/11/98 9:40a Dennis
// AG Specific Helpful Hints
// 
// 33    6/11/98 9:39a Dennis
// AG Specific Helpful Hints
// 
// 32    6/10/98 6:40p Hforman
// removing hint when help or tutorial button pressed
// 
// 31    6/10/98 4:35p Psasse
// Fix modal helpful hints placement as well as continue button placement
// 
// 30    6/08/98 2:01p Hforman
// resetting "seen this session" flags in ResetHints();
// remove SetFocus() in OnPaint() - causing weird behaviour
// 
// 29    6/05/98 4:48p Hforman
// fixed turning voice-overs on/off
// 
// 28    6/05/98 2:13p Rlovejoy
// Added flag to avoid getting mouse capture.
// 
// 27    6/04/98 9:40p Psasse
// better looking modal Helpful Hint windows
// 
// 26    6/04/98 7:20p Psasse
// modal helpful hint support
// 
// 25    6/03/98 9:16p Psasse
// Better MouseCapture protocol
// 
// 24    6/03/98 6:35p Hforman
// add DT_NOPREFIX to DrawText
// 
// 23    6/03/98 1:51p Psasse
// Much better mouse and keyboard input closing of helpful hints
// 
// 22    6/02/98 6:13p Psasse
// destroy helpful hint by clicking outside the hint window
// 
// 21    6/01/98 5:15p Psasse
// Better Sound placement
// 
// 20    5/28/98 1:47p Hforman
// add voice-over flag
// 
// 19    5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 18    5/20/98 9:19p Psasse
// Support for larger bitmaps in Helpful hints window and asynchronous
// progress control becomes more like IE
// 
// 17    4/24/98 6:29p Fredf
// Uses MFC functions SaveBarState() and LoadBarState().
// 
// 16    4/20/98 7:39p Fredf
// New 256-color helpful hint artwork. Reworked layout some.
// 
// 15    3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 14    3/06/98 1:08p Hforman
// check if we've initialized hints before saving user file (crashes
// otherwise)
// 
// 13    2/27/98 2:45p Jayn
// Added ReleaseDC for GetDC.
// 
// 12    1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 11    12/22/97 5:26p Hforman
// only show one hint per project type and place per session
// 
// 10    12/19/97 5:36p Hforman
// 
// 9     12/18/97 2:37p Hforman
// fixed problem with SetWindowRgn()
// 
// 8     12/18/97 12:46p Hforman
// more W-I-P
// 
// 7     12/16/97 6:15p Hforman
// 
// 6     12/16/97 11:53a Hforman
// more work in progress
// 
// 5     12/12/97 5:19p Hforman
// work in progress
// 
// 4     12/05/97 5:40p Hforman
// work in progress
// 
// 3     12/04/97 12:50p Hforman
// work in progress
//

#include "stdafx.h"
#include <afxpriv.h> // for WM_DISABLEMODAL
#include "pmw.h"
#include "mainfrm.h"
#include "project.h"
#include "util.h"
#include "colordef.h"
#include "HelpHint.h"
#include "pmwcfg.h"     // For GetConfiguration
#include "rmagcom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CHelpfulHint

// constants
const int cHintWndWidth  = 300;
const char* cHintsFile = "hints.hnt";		// filename of hints file
const char* cUserHintsData = "hints.dat";	// filename of user's hints datafile

CHelpfulHint::CHelpfulHint()
{
	m_fHintsLoaded			= FALSE;
	m_fShowingNamedHint	= FALSE;
	m_fShowWorkspaceHint = FALSE;	// don't show generic workspace hint first
	m_pWndHlpHint			= NULL;

	for (int i = 0; i < PROJECT_TYPE_Last; i++)
		m_ProjectHintSeenThisSession[i] = FALSE;
	for (i = 0; i < PLACE_Last; i++)
		m_PlaceHintSeenThisSession[i] = FALSE;
}

CHelpfulHint::~CHelpfulHint()
{
   
	if (m_fHintsLoaded)
		FreeHints();
}

void CHelpfulHint::FreeHints()
{
	int i;

	for (i = 0; i < m_ProjectHintsArray.GetSize(); i++)
		delete (CHint*)m_ProjectHintsArray[i];
	m_ProjectHintsArray.RemoveAll();

	for (i = 0; i < m_PlaceHintsArray.GetSize(); i++)
		delete (CHint*)m_PlaceHintsArray[i];
	m_PlaceHintsArray.RemoveAll();

	for (i = 0; i < m_NamedHintsArray.GetSize(); i++)
		delete (CHint*)m_NamedHintsArray[i];
	m_NamedHintsArray.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////
// LoadHelpfulHints()
//
// Opens the Helpful Hints file, which should be found in the run directory.
// Also opens the user's hints info file. If all goes well, hints are parsed
// into hint arrays by type, and all necessary settings are set according to
// the user's stored info.
//////////////////////////////////////////////////////////////////////////////
BOOL CHelpfulHint::LoadHelpfulHints()
{
	m_fHintsLoaded = FALSE;

	CString str = GetGlobalPathManager()->ExpandPath(cHintsFile);
	CFile hintsfile;
	if (hintsfile.Open(str, CFile::modeRead) == FALSE)
	{
		// couldn't open the hints file
		return FALSE;
	}

	// set defaults
	m_fUserSetHintPos = FALSE;
	m_fShowHelpfulHints = TRUE;
	m_fShowDesignTips = TRUE;
	m_fPlayVoiceOvers = TRUE;

	// try to open user's "hints data" file
	BOOL fValidUserFile = FALSE;

	UserHintFileInfo hintInfo;

	CFile userfile;
	str = GetGlobalPathManager()->ExpandPath("[[U]]\\");
	str += cUserHintsData;
	if (userfile.Open(str, CFile::modeRead))
	{
		BOOL fArchiveOK = TRUE;
		CArchive ar(&userfile, CArchive::load);
		TRY
		{
			hintInfo.DoSerialize(ar);
		}
		CATCH_ALL(e)
		{
			TRACE("Bad user hints file found!\n");
			fArchiveOK = FALSE;
		}
		END_CATCH_ALL
			
		// check if we have the proper userfile version, and
		// the Hints file date is the same as what we stored
		CTimeStamp HintsFileTime;
		HintsFileTime.SetFileModify((HANDLE)(hintsfile.m_hFile));
		if ( fArchiveOK &&
			  (hintInfo.nVersion == cUserFileVersion) &&
			  (hintInfo.HintsFileTime == HintsFileTime) )
		{
			m_fUserSetHintPos = hintInfo.fUserSetPos;
			m_ptUserHintPos = hintInfo.ptUserPos;
			m_fShowHelpfulHints = hintInfo.fShowHelpfulHints;
			m_fShowDesignTips = hintInfo.fShowDesignTips;
			m_fPlayVoiceOvers = hintInfo.fPlayVoiceOvers;
			fValidUserFile = TRUE;
		}

		ar.Close();
		userfile.Close();
	}

	CArchive ar(&hintsfile, CArchive::load);

	// Ok, let's read in the hints...
	int numproj = 0;
	int numplace = 0;
	int numnamed = 0;
	while (hintsfile.GetPosition() < hintsfile.GetLength())
	{
		CHint* pHint = new CHint;
		if (!pHint)
		{
			// major bad news.
			ASSERT(0);
			return FALSE;
		}

		TRY
		{
			pHint->DoSerialize(ar);
		}
		CATCH_ALL(e)
		{
			AfxMessageBox("Bad hints file!\n");
			ASSERT(0);
			return FALSE;
		}
		END_CATCH_ALL

		ar.Flush();

		pHint->m_fShownInSession = FALSE;
		pHint->m_nTimesShown = 0;

		switch (pHint->m_HintType)
		{
		case PROJECT_HINT:
			if (fValidUserFile)
				pHint->m_nTimesShown = hintInfo.baProjectHintCnt[numproj++];
			m_ProjectHintsArray.Add(pHint);
			break;
		case PLACE_HINT:
			if (fValidUserFile)
				pHint->m_nTimesShown = hintInfo.baPlaceHintCnt[numplace++];
			m_PlaceHintsArray.Add(pHint);
			break;
		default: // everything else
			if (fValidUserFile)
				pHint->m_nTimesShown = hintInfo.baNamedHintCnt[numnamed++];
			m_NamedHintsArray.Add(pHint);
			break;
		}
	}

	m_fHintsLoaded = TRUE;
	return TRUE;
}

void CHelpfulHint::SaveHintsUserConfig()
{
	// Did we even initialize hints? If not, there's nothing to save
	if (!m_fHintsLoaded)
		return;

	CString strHints = GetGlobalPathManager()->ExpandPath(cHintsFile);
	CFile hintsfile;
	if (!hintsfile.Open(strHints, CFile::modeRead))
		return;	// don't save user file if we can't open hints file

	UserHintFileInfo hintInfo;

	// set the version number for the data file
	hintInfo.nVersion = cUserFileVersion;

	// get CTimeStamp value for Hints file
	hintInfo.HintsFileTime.SetFileModify((HANDLE)(hintsfile.m_hFile));

	hintInfo.fUserSetPos = m_fUserSetHintPos;
	hintInfo.ptUserPos = m_ptUserHintPos;

	hintInfo.fShowHelpfulHints = m_fShowHelpfulHints;
	hintInfo.fShowDesignTips = m_fShowDesignTips;
	hintInfo.fPlayVoiceOvers = m_fPlayVoiceOvers;

	// update arrays...
	for (int i = 0; i < m_ProjectHintsArray.GetSize(); i++)
		hintInfo.baProjectHintCnt.Add(m_ProjectHintsArray[i]->m_nTimesShown);
	for (i = 0; i < m_PlaceHintsArray.GetSize(); i++)
		hintInfo.baPlaceHintCnt.Add(m_PlaceHintsArray[i]->m_nTimesShown);
	for (i = 0; i < m_NamedHintsArray.GetSize(); i++)
		hintInfo.baNamedHintCnt.Add(m_NamedHintsArray[i]->m_nTimesShown);

	// write out the data
	CString str = GetGlobalPathManager()->ExpandPath("[[U]]\\");
	str += cUserHintsData;
	CFile datafile;
	if (datafile.Open(str, CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&datafile, CArchive::store);
		hintInfo.DoSerialize(ar);
	}
}

void UserHintFileInfo::DoSerialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << nVersion;
		ar << HintsFileTime;
		ar << fUserSetPos;
		ar << ptUserPos;
		ar << fShowHelpfulHints;
		ar << fShowDesignTips;
		ar << fPlayVoiceOvers;
	}
	else // loading
	{
		ar >> nVersion;
		ar >> HintsFileTime;
		ar >> fUserSetPos;
		ar >> ptUserPos;
		ar >> fShowHelpfulHints;
		ar >> fShowDesignTips;
		ar >> fPlayVoiceOvers;
	}

	baProjectHintCnt.Serialize(ar);
	baPlaceHintCnt.Serialize(ar);
	baNamedHintCnt.Serialize(ar);
}

// Show one of a specified hint type (project hint, place hint)
BOOL CHelpfulHint::ShowHelpfulHint(HINT_TYPE type, int nWhich, CWnd* pParent/*NULL*/, BOOL fModal/*=FALSE*/)
{
	BOOL fOK = FALSE;
	if (m_fHintsLoaded && !AllHintsOff() && !m_fShowingNamedHint)
	{
		CHint* pHint = NULL;
		if (type == PROJECT_HINT)
		{
			if (m_ProjectHintSeenThisSession[nWhich])
				return FALSE;	// already seen hint for this project type

			// show a generic workspace hint every other time
			// we show a project-specific hint
			if (m_fShowWorkspaceHint)
			{
				pHint = FindNextHint(PLACE_HINT, PLACE_Workspace);
				if (pHint)
					m_fShowWorkspaceHint = FALSE;
			}
			else
			{
				// set for next time in
				m_fShowWorkspaceHint = TRUE;
			}
		}
		else if (type == PLACE_HINT)
		{
			if (m_PlaceHintSeenThisSession[nWhich])
				return FALSE;	// already seen hint for this place
		}

		if (!pHint)
			pHint = FindNextHint(type, nWhich);

		// Get this before creating hint window. Used for modal hints only.
		CWnd* pLastPopup = AfxGetMainWnd()->GetLastActivePopup();

		if (pHint &&
			 CreateHint(pHint, pParent, m_fUserSetHintPos ? &m_ptUserHintPos : NULL, fModal))
		{
			pHint->m_fShownInSession = TRUE;
			if (type == PROJECT_HINT)
				m_ProjectHintSeenThisSession[nWhich] = TRUE;
			else if (type == PLACE_HINT)
				m_PlaceHintSeenThisSession[nWhich] = TRUE;
			m_HintsSeenArray.Add(pHint);
			pHint->m_nTimesShown++;
			fOK = TRUE;
			PlaySound(pHint);

			if(fModal)
			{
				if (pLastPopup)
					pLastPopup->EnableWindow(FALSE);

				if (((CWndModalHlpHint*)m_pWndHlpHint)->ExecuteModalLoop())
					fOK = FALSE;

				if (pLastPopup)
					pLastPopup->EnableWindow(TRUE);

				RemoveHelpfulHint();
			}
		}
	}

	return fOK;
}

// show a "named" hint (action hints)
BOOL CHelpfulHint::ShowHelpfulHint(const CString& strWhich, CWnd* pParent/*=NULL*/, BOOL fModal/*=FALSE*/)
{
	BOOL fModalRet = TRUE;
	m_fShowingNamedHint = FALSE;

	if (m_fHintsLoaded && !AllHintsOff())
	{
		// find hint
		CHint* pHint = FindHintByName(strWhich);
		if (pHint && UserWantsHintType(pHint) &&
			 !pHint->m_fShownInSession && pHint->m_nTimesShown < 2)
		{
			// Get this before creating hint window. Used for modal hints only.
			CWnd* pLastPopup = AfxGetMainWnd()->GetLastActivePopup();

			if (CreateHint(pHint, pParent, m_fUserSetHintPos ? &m_ptUserHintPos : NULL, fModal))
			{
				m_HintsSeenArray.Add(pHint);
				pHint->m_fShownInSession = TRUE;
				pHint->m_nTimesShown++;
				m_fShowingNamedHint = TRUE;
				PlaySound(pHint);

				if(fModal)
				{
					if (pLastPopup)
						pLastPopup->EnableWindow(FALSE);

					if (((CWndModalHlpHint*)m_pWndHlpHint)->ExecuteModalLoop())
						fModalRet = FALSE;

					if (pLastPopup)
						pLastPopup->EnableWindow(TRUE);

					RemoveHelpfulHint();
				}
			}
		}
	}

	if(fModal)
		return fModalRet;

	return m_fShowingNamedHint;
}

void CHelpfulHint::ShowRecentHintsDialog()
{
	CRecentHintsDlg dlg(m_HintsSeenArray, this);
	dlg.DoModal();
//	RemoveHelpfulHint();
}

void CHelpfulHint::ResetHints()
{
	m_fUserSetHintPos = FALSE;

	int i;
	for (i = 0; i < PROJECT_TYPE_Last; i++)
		m_ProjectHintSeenThisSession[i] = FALSE;
	for (i = 0; i < PLACE_Last; i++)
		m_PlaceHintSeenThisSession[i] = FALSE;

	for (i = 0; i < m_ProjectHintsArray.GetSize(); i++)
	{
		m_ProjectHintsArray[i]->m_fShownInSession = FALSE;
		m_ProjectHintsArray[i]->m_nTimesShown = 0;
	}
	for (i = 0; i < m_PlaceHintsArray.GetSize(); i++)
	{
		m_PlaceHintsArray[i]->m_fShownInSession = FALSE;
		m_PlaceHintsArray[i]->m_nTimesShown = 0;
	}
	for (i = 0; i < m_NamedHintsArray.GetSize(); i++)
	{
		m_NamedHintsArray[i]->m_fShownInSession = FALSE;
		m_NamedHintsArray[i]->m_nTimesShown = 0;
	}

	SaveHintsUserConfig();
}

BOOL CHelpfulHint::CreateHint(CHint* pHint, CWnd* pParent, CPoint* pPos/*=NULL*/, BOOL fModal/*=FALSE*/, BOOL fGrabCapture/*=TRUE*/)
{
	// make sure we don't already have a hint showing
	RemoveHelpfulHint();

	if(fModal)
	{
		m_pWndHlpHint = new CWndModalHlpHint(this);
		if (!m_pWndHlpHint)
		{
			ASSERT(0);
			return FALSE;
		}
	}
	else
	{
		m_pWndHlpHint = new CWndHlpHint(this);
		if (!m_pWndHlpHint)
		{
			ASSERT(0);
			return FALSE;
		}
	}

	m_pWndHlpHint->SetHint(*pHint);
	if (pPos != NULL)
		m_pWndHlpHint->SetPosition(*pPos);

	// Create the window, with default width.
	// NOTE: final position and height get calculated in window's OnCreate()
	CRect rect(0, 0, cHintWndWidth, 1);
	
	if (pParent == NULL)
		pParent = AfxGetMainWnd();
	m_pWndHlpHint->GrabCapture(fGrabCapture && !fModal);
	BOOL retval = (m_pWndHlpHint->CreateEx(
						NULL,
						::AfxRegisterWndClass(CS_BYTEALIGNWINDOW|CS_SAVEBITS,
													 AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
						NULL, WS_POPUP|WS_VISIBLE, rect, pParent, 0));

	// we don't want the hint window to have the focus
	//pParent->SetFocus();
	m_pWndHlpHint->SetFocus();
	return retval;
}

void CHelpfulHint::RemoveHelpfulHint()
{
	if (m_pWndHlpHint && !m_pWndHlpHint->InModalLoop())
	{
		m_pWndHlpHint->CancelMove();	// just in case
		m_fShowingNamedHint = FALSE;
		m_pWndHlpHint->DestroyWindow();
		delete m_pWndHlpHint;
		m_pWndHlpHint = NULL;
	}
}

void CHelpfulHint::SetUserHintPos(CPoint newpos)
{
	m_fUserSetHintPos = TRUE;
	m_ptUserHintPos = newpos;
}

CHint* CHelpfulHint::FindHintByName(const CString& strName)
{
	for (int i = 0; i < m_NamedHintsArray.GetSize(); i++)
	{
		CHint* pHint = (CHint*)m_NamedHintsArray[i];
		if (pHint->m_strName.CompareNoCase(strName) == 0)
		{
			return pHint;
			break;
		}
	}
	return NULL;
}

CHint* CHelpfulHint::FindNextHint(HINT_TYPE type, int nWhich)
{
	CArray<CHint*,CHint*>* pArray;
	int startpos = 0;
	CHint* pHint = NULL;
	switch (type)
	{
	case PROJECT_HINT:
		if (nWhich == PROJECT_TYPE_HalfCard)
			nWhich = PROJECT_TYPE_Card;

		pArray = &m_ProjectHintsArray;
		break;
	case PLACE_HINT:
		pArray = &m_PlaceHintsArray;
		break;
	default:
		// invalid type
		ASSERT(0);
		return NULL;
	}

	// Search through array looking for correct hint category.
	// Hint must not have been seen before, and must be
	// the style of hint the user wants to see.

	for (int i = startpos; i < pArray->GetSize(); i++)
	{
		CHint* pHint = pArray->GetAt(i);
		if ( pHint->m_ProjectOrPlace == nWhich &&
			 !pHint->m_fShownInSession	&&
			  pHint->m_nTimesShown < 1 &&
			  UserWantsHintType(pHint) )
			return pHint;
	}

	return NULL;
}

BOOL CHelpfulHint::UserWantsHintType(CHint* pHint)
{
	if ((pHint->m_WinType == HELPFULHINT && !m_fShowHelpfulHints) ||
		 (pHint->m_WinType == DESIGNTIP && !m_fShowDesignTips))
		return FALSE;
	else
		return TRUE;
}

void
CHelpfulHint::PlaySound (CHint* pHint)
{
	if (!GET_PMWAPP()->HintsVoiceEnabled() || pHint->m_strWaveTitle == "")
		return;

	IPathManager* pPathManager = GetGlobalPathManager();

   DWORD       RetCode;
	// First, look on user's hard drive
	CString     SoundFile = "soundfx\\";
	SoundFile += pHint->m_strWaveTitle;
	// Turn off CD prompting
	BOOL fOldPrompt = pPathManager->SetPromptForContent(FALSE);
	BOOL fExists = pPathManager->FileExists(SoundFile, TRUE);
	// If not found, look on "Home" CD
	if (fExists == FALSE)
	{
		SoundFile = "[[H]]\\audio\\";
		SoundFile += pHint->m_strWaveTitle;
		fExists = pPathManager->FileExists(SoundFile, TRUE);
	}
	// Restore CD prompting state
	pPathManager->SetPromptForContent(fOldPrompt);
	// If sound file was found, play it
	if (fExists)
	{
		CString csPath = pPathManager->LocatePath(SoundFile);
		CPmwApp *pApp = GET_PMWAPP();
		if ((RetCode = pApp->m_MciWnd.play_sound ((LPSTR)(LPCSTR)csPath)) != 0)
			TRACE2 ("Got error code %i playing %s\n", RetCode, (LPCSTR)csPath);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWndHlpHint class -- the actual helpful hint Window

// constants
const int cEllipseVal	= 30;		// ellipse value for RoundRectRgn
const int cShadowPixels = 5;		// shadow size at bottom/right of wnd

// button IDs
#define ID_HINTCLOSE			1000
#define ID_HINTHELP			1001
#define ID_HINTTUTORIAL		1002
#define ID_HINTCONTINUE		1003
#define ID_HINTS_SHOW		1004

CWndHlpHint::CWndHlpHint()
{
}

CWndHlpHint::CWndHlpHint(CHelpfulHint* pHH)
{
	m_pHelpfulHint = pHH;
	m_fPosSet = FALSE;
	m_fUserLButtonDown = FALSE;
	m_fUserMovedWin = FALSE;
	m_fGrabCapture = TRUE;
}

CWndHlpHint::~CWndHlpHint()
{
}

int CWndHlpHint::GetExtraHeight()
   {
      return 0;
   }

void CWndHlpHint::SetPosition(const CPoint& pt)
{
	m_fPosSet = TRUE;
	m_ptHintPos = pt;
}

void CWndHlpHint::StopSound()
{
	GET_PMWAPP()->m_MciWnd.stop_sound();
	GET_PMWAPP()->m_MciWnd.close_all(0);
}

void CWndHlpHint::GrabCapture(BOOL fGrabCapture)
{
	m_fGrabCapture = fGrabCapture;
}

void CWndHlpHint::CreateFonts()
{
	if (m_fontText.m_hObject == NULL)
	{
		LOGFONT logfont;
		memset(&logfont, 0, sizeof(logfont));
		logfont.lfHeight = -12;
		logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		lstrcpy(logfont.lfFaceName, "Arial");
		logfont.lfWeight = FW_NORMAL;
		m_fontText.CreateFontIndirect(&logfont);
	}

	if (m_fontTitle.m_hObject == NULL)
	{
		LOGFONT logfont;
		memset(&logfont, 0, sizeof(logfont));
		logfont.lfHeight = -14;
		logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		lstrcpy(logfont.lfFaceName, "Arial");
		logfont.lfWeight = FW_BOLD;
		m_fontTitle.CreateFontIndirect(&logfont);
	}
}

BEGIN_MESSAGE_MAP(CWndHlpHint, CWnd)
	//{{AFX_MSG_MAP(CWndHlpHint)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HINTCLOSE, OnHintClose)
	ON_COMMAND(ID_HINTTUTORIAL, OnHintTutorial)
	ON_COMMAND(ID_HINTHELP, OnHintHelp)
	ON_COMMAND(ID_HINTS_SHOW, OnHintsShowAgain)
	ON_MESSAGE(WM_DISABLEMODAL, OnDisableModal)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWndHlpHint message handlers

int CWndHlpHint::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// modify height of window to fit text
	CRect crClient;
	GetClientRect(crClient);

	CString strBitmap;
	if (m_curHint.m_WinType == HELPFULHINT)
	{
		// Helpful Hint
		strBitmap = "HINTHEAD1";
	}
	else
	{
		// Design Tip
		strBitmap = "HINTHEAD2";
	}

   // Show Hints Again objects
   CString  csShowHints;
	LoadConfigurationString(IDS_NO_SHOW_HINTS, csShowHints);
   CRect    crShowHints(20, 0, 0, 0);

	CDC* pDC = GetDC();
	if (pDC == NULL)
	{
		ASSERT(0);
		return -1;
	}

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
      {
         CResourceLoader   rlProductSpecific(GET_PMWAPP()->GetResourceManager());

         rlProductSpecific.LoadBitmap(m_bmFace, CSharedIDResourceManager::riCharacterSmall, pOurPal);
      }
   else
   	Util::LoadResourceBitmap(m_bmFace, strBitmap, pOurPal);
	BITMAP bm;
	if (m_bmFace.m_hObject != NULL)
	{
		m_bmFace.GetBitmap(&bm);
	}
	else
	{
		bm.bmWidth = 0;
		bm.bmHeight = 16;
	}

	m_crTitle.SetRect(8+bm.bmWidth+5, 8, crClient.Width()-25, 8+bm.bmHeight);
	
	if(GET_PMWAPP()->GetHintFullLengthBmp())
	  m_crText.SetRect(m_crTitle.left, bm.bmHeight+15, crClient.Width()-20, crClient.Height());
	else
	  m_crText.SetRect(20, bm.bmHeight+15, crClient.Width()-20, crClient.Height());

	// create fonts to use
	CreateFonts();

	CFont* pOldFont = NULL;
	if (m_fontText.m_hObject)
	{
		pOldFont = pDC->SelectObject(&m_fontText);
	}
	
	// get rect height
	pDC->DrawText(m_curHint.m_strText, m_crText, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);

   // Get Show Hints Again text size
	pDC->DrawText(csShowHints, crShowHints, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);

	if (pOldFont != NULL)
	{
		pDC->SelectObject(pOldFont);
	}

   int height = bm.bmHeight + m_crText.Height() + 60 + GetExtraHeight();

	// set size/position
	if (m_fPosSet)
	{
		// we've been told where to put the window, but make sure it's
		// completely on the screen.
		CPoint pt = m_ptHintPos;
		if (pt.x < 0) pt.x = 0;
		if (pt.y < 0) pt.y = 0;

		int scrnWidth = GetSystemMetrics(SM_CXMAXIMIZED);
		int scrnHeight = GetSystemMetrics(SM_CYMAXIMIZED);

		if (pt.x + crClient.Width() > scrnWidth)
			pt.x = scrnWidth - crClient.Width() - cShadowPixels;
		if (pt.y + height > scrnHeight)
			pt.y = scrnHeight - height - cShadowPixels;

		SetWindowPos(NULL, pt.x, pt.y, crClient.Width(), height, SWP_NOZORDER);
	}
	else if (GetParent()->GetStyle() & WS_POPUP)
	{
		// center over popup
		SetWindowPos(NULL, 0, 0, crClient.Width(), height, SWP_NOZORDER|SWP_NOMOVE);
		CenterWindow();
	}
	else
	{
		// position at bottom right corner of mainframe,
		// inside status/toolbars (if any)
		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
		CRect crApp;
		pMainFrm->GetWindowRect(crApp);
		CPmwStatusBar& statusBar = pMainFrm->StatusBar();
		if (statusBar.IsWindowVisible())
		{
			CRect r;
			statusBar.GetClientRect(r);
			crApp.bottom -= r.Height();
		}

		SetWindowPos(NULL,
						 crApp.right - crClient.Width() - 3,
						 crApp.bottom - height - 3,
						 crClient.Width(), height, SWP_NOZORDER);
	}

	// set window rgn to create a "rounded" window, with a rounded
	// shadow area on the lower right side
	GetClientRect(crClient);

	CRgn rgn1, rgn2, comboRgn;
	rgn1.CreateRoundRectRgn(
				0, 0,
				crClient.Width() - cShadowPixels, crClient.Height() - cShadowPixels,
				cEllipseVal, cEllipseVal);
	rgn2.CreateRoundRectRgn(
				cShadowPixels, cShadowPixels,
				crClient.Width(), crClient.Height(),
				cEllipseVal, cEllipseVal);

	comboRgn.CreateRectRgn(0,0,0,0);
	comboRgn.CombineRgn(&rgn1, &rgn2, RGN_OR);
	SetWindowRgn((HRGN)comboRgn.Detach(), FALSE);

	// create buttons...
	CRect crCloseButton;
	crCloseButton.SetRect(0, 0, 17, 17);
	crCloseButton.OffsetRect(
		crClient.right-cShadowPixels-8-crCloseButton.Width(),
		8);
	m_btnClose.Create(crCloseButton, this, ID_HINTCLOSE);
	m_btnClose.SetBitmaps("HINT_CLOSE_U", "HINT_CLOSE_D", pOurPal);
	m_btnClose.SetTransparentColor(RGB(255,0,255));

	CPoint cpButton(crClient.right-cShadowPixels-8, crClient.bottom-cShadowPixels-8);

	if (m_curHint.m_nTutorialID)
	{
		CRect crTutorialButton;
		crTutorialButton.SetRect(0, 0, 24, 24);
		crTutorialButton.OffsetRect(cpButton.x - crTutorialButton.Width(),
											 cpButton.y - crTutorialButton.Height());
		m_btnTutorial.Create(crTutorialButton, this, ID_HINTTUTORIAL);
		m_btnTutorial.SetBitmaps("HINT_TUT_U", "HINT_TUT_D", pOurPal);
		m_btnTutorial.SetTransparentColor(RGB(255,0,255));
		cpButton.x -= crTutorialButton.Width()+5;
	}

	if (m_curHint.m_nHelpID)
	{
		CRect crHelpButton;
		crHelpButton.SetRect(0, 0, 24, 24);
		crHelpButton.OffsetRect(cpButton.x - crHelpButton.Width(),
										cpButton.y - crHelpButton.Height());
		m_btnHelp.Create(crHelpButton, this, ID_HINTHELP);
		m_btnHelp.SetBitmaps("HINT_HELP_U", "HINT_HELP_D", pOurPal);
		m_btnHelp.SetTransparentColor(RGB(255,0,255));
		cpButton.x -= crHelpButton.Width()+5;
	}

   // Create Show Hints Again Checkbox Control
   // Center it relative to Help and Tutorial buttons
   int   nShowHintsTextOffset = 24 - crShowHints.Height();
   if(nShowHintsTextOffset > 0)
      nShowHintsTextOffset /= 2;
   else
      nShowHintsTextOffset = 0;
   nShowHintsTextOffset += crShowHints.Height();
   crShowHints.right += 20;      // Account for CheckBox width
   crShowHints.OffsetRect(0, cpButton.y - nShowHintsTextOffset);
   BOOL bCreatedShowHints = m_btnShowHints.Create(
      csShowHints,      // Label for CheckBox
      BS_AUTOCHECKBOX,  // Style
      crShowHints,      // Position and size
      this,             // Parent
      ID_HINTS_SHOW);   // ID
   ASSERT(bCreatedShowHints);
   if(bCreatedShowHints)
      {
         // Default to unchecked
         m_btnShowHints.SetCheck(0);
         m_btnShowHints.SetFont(&m_fontText);   // Use same font as text
         m_btnShowHints.ShowWindow(TRUE);       // Make is visible
      }

	ReleaseDC(pDC);
	pDC = NULL;

	if (m_fGrabCapture)
	{
		SetCapture();
	}

	return 0;
}

void CWndHlpHint::OnDestroy() 
{
	if (m_fGrabCapture)
	{
		ReleaseCapture();
	}
	m_fGrabCapture = TRUE;

	StopSound();
	  
	CWnd::OnDestroy();

	m_bmFace.DeleteObject();
	m_fontText.DeleteObject();
	m_fontTitle.DeleteObject();
}

// override default OnDisableModal() since we want
// this window to always be enabled
LRESULT CWndHlpHint::OnDisableModal(WPARAM, LPARAM)
{
	return TRUE;   // don't disable
}

void CWndHlpHint::OnHintClose()
{
	StopSound();
	CancelMove();

	// go through standard remove function
	m_pHelpfulHint->RemoveHelpfulHint();
}

void CWndHlpHint::OnHintTutorial()
{
	StopSound();
	((CMainFrame*)AfxGetMainWnd())->ShowTutorial(m_curHint.m_nTutorialID);
	m_pHelpfulHint->RemoveHelpfulHint();
}

void CWndHlpHint::OnHintHelp()
{
	StopSound();
	AfxGetApp()->WinHelp(m_curHint.m_nHelpID);
	m_pHelpfulHint->RemoveHelpfulHint();
}

void CWndHlpHint::OnHintsShowAgain()
{
   BOOL bEnable = !m_btnShowHints.GetCheck();
	GET_PMWAPP()->EnableHelpfulHints(bEnable);
	GET_PMWAPP()->EnableDesignTips(bEnable);
}

void CWndHlpHint::CancelMove()
{
	if (m_fUserMovedWin)
	{
		// remove last drag rect from screen
		CWnd* pWnd = CWnd::GetDesktopWindow();
		ASSERT(pWnd);
		if (pWnd)
		{
			CDC* pMoveDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE);
			ASSERT(pMoveDC);
			if (pMoveDC)
			{
				pMoveDC->DrawDragRect(CRect(0,0,0,0), CSize(0,0), m_crDragLast, CSize(2,2));
				pWnd->ReleaseDC(pMoveDC);
			}
		}

		if (!m_fGrabCapture)
			ReleaseCapture();

		m_fUserMovedWin = FALSE;
	}
}

void CWndHlpHint::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_fUserLButtonDown = TRUE;

	if(GetCapture() != this)
		SetCapture();
	
	// save current cursor position
	::GetCursorPos(&m_ptLastCursorPos);

	// save current window position, and clear last position
	GetWindowRect(&m_crDragCur);
	m_crDragLast.SetRect(0,0,0,0);

	CRect Rect;
	GetWindowRect(&Rect);

	CPoint PtTemp(m_ptLastCursorPos);
	
	if(!Rect.PtInRect(PtTemp))
	{
		OnHintClose();
	}
	else
	{
		CWnd* pWnd = WindowFromPoint(m_ptLastCursorPos);
		if((pWnd) && (pWnd != this))
		{
			ReleaseCapture();
			LPARAM lParam = MAKELPARAM(point.x, point.y);
			pWnd->SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, lParam);
			GetCapture();
		}
	}
}

void CWndHlpHint::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_fUserMovedWin)
	{
		// remove last xor from screen
		CancelMove();

		// Move the window, and tell parent class the new user position
		CPoint pt;
		::GetCursorPos(&pt);

		CSize offset = pt - m_ptLastCursorPos;
		m_crDragCur.OffsetRect(offset);
		
		MoveWindow(m_crDragCur, TRUE);
		m_pHelpfulHint->SetUserHintPos(CPoint(m_crDragCur.left, m_crDragCur.top));
	}

	//GetParent()->SetFocus();
	//SetFocus();

}
void CWndHlpHint::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (nFlags & MK_LBUTTON)
	{
		// drag window around - actually, just drag xor rect
		CPoint pt;
		::GetCursorPos(&pt);

		if (m_fUserLButtonDown && pt != m_ptLastCursorPos)
		{
			CSize offset = pt - m_ptLastCursorPos;
			m_crDragCur.OffsetRect(offset);
			CRect crDrag = m_crDragCur;
			crDrag.DeflateRect(3,3);

			CWnd* pWnd = CWnd::GetDesktopWindow();
			ASSERT(pWnd);
			if (pWnd)
			{
				CDC* pMoveDC = pWnd->GetDCEx(NULL, DCX_WINDOW|DCX_CACHE);
				ASSERT(pMoveDC);
				if (pMoveDC)
				{
					pMoveDC->DrawDragRect(crDrag, CSize(2,2), m_crDragLast, CSize(2,2));
					pWnd->ReleaseDC(pMoveDC);
				}
			}

			m_fUserMovedWin = TRUE;
			m_ptLastCursorPos = pt;
			m_crDragLast = crDrag;
		}
	}
	else	
		CWnd::OnMouseMove(nFlags, point);
}

void CWndHlpHint::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt;
	::GetCursorPos(&pt);

	CRect Rect;
	GetWindowRect(&Rect);

	if (!Rect.PtInRect(pt))
	{
		OnHintClose();
	}
}

void CWndHlpHint::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// put focus back on parent
	//GetParent()->SetFocus();
	//SetFocus();
	CWnd::OnRButtonUp(nFlags, point);
}

void CWndHlpHint::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	OnHintClose();
}

void CWndHlpHint::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CPalette* pOldPal = dc.SelectPalette(pOurPal, FALSE);
	dc.RealizePalette();

	CRect r;
	GetClientRect(r);

	// draw "balloon" background with shadow
	CBrush brBalloon;
	CPen cpBalloon;
	CBrush brShadow;

   if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
   {
      COLORREF crFillColor;
      switch(GetConfiguration()->ProductLevel())
      {
         case CPrintMasterConfiguration::PrintStandard:
         case CPrintMasterConfiguration::PrintPremium:
            crFillColor = PALETTERGB(204, 204, 255);
            break;
         case CPrintMasterConfiguration::CraftStandard:
         case CPrintMasterConfiguration::CraftDeluxe:
         case CPrintMasterConfiguration::CreataParty:
            crFillColor = PALETTERGB(255, 204, 204);
            break;
         case CPrintMasterConfiguration::SpiritStandard:
            crFillColor = PALETTERGB(204, 255, 255);
            break;
         default:
            ASSERT(0);
            crFillColor = PALETTERGB(153, 153, 255);
            break;
      }
      brBalloon.CreateSolidBrush(crFillColor);
   }
   else
   	brBalloon.CreateSolidBrush((m_curHint.m_WinType == HELPFULHINT) ? HELPFUL_HINT_COLOR : DESIGN_HINT_COLOR);

	cpBalloon.CreateStockObject(BLACK_PEN);

   CBrush* pOldBrush = dc.SelectObject(&brBalloon);
	CPen* pOldPen = dc.SelectObject(&cpBalloon);
	dc.RoundRect(
			0, 0,
			r.Width()-cShadowPixels-1, r.Height()-cShadowPixels,
			cEllipseVal, cEllipseVal);

	if (pOldPen != NULL)
	{
		dc.SelectObject(pOldPen);
		pOldPen = NULL;
	}
	if (pOldBrush != NULL)
	{
		dc.SelectObject(pOldBrush);
		pOldBrush = NULL;
	}

	brBalloon.DeleteObject();
	cpBalloon.DeleteObject();

   // Now draw the offset shadow
	CRgn rgn1, rgn2, shadowRgn;
	rgn1.CreateRoundRectRgn(
			0, 0,
			r.Width() - cShadowPixels, r.Height() - cShadowPixels,
			cEllipseVal, cEllipseVal);
	rgn2.CreateRoundRectRgn(
			cShadowPixels, cShadowPixels,
			r.Width(), r.Height(),
			cEllipseVal, cEllipseVal);
	shadowRgn.CreateRectRgn(0,0,0,0);
	shadowRgn.CombineRgn(&rgn2, &rgn1, RGN_DIFF);

	brShadow.CreateStockObject(BLACK_BRUSH);

	dc.FillRgn(&shadowRgn, &brShadow);

	brShadow.DeleteObject();

	// display PrintMaster head (!) in upper left corner
	BITMAP bm;
	bm.bmWidth = 0;
	bm.bmHeight = 16;
	if (m_bmFace.m_hObject)
	{
		m_bmFace.GetBitmap(&bm);
		CRect rcBmp(8,8, 8 + bm.bmWidth, 8 + bm.bmHeight);
		Util::TransparentBltBitmap(dc, rcBmp, m_bmFace, NULL, pOurPal, 0,0);
	}

	// display text
	int oldBkMode = dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = NULL;

	// title
	if (m_fontTitle.m_hObject)
	{
		CFont* pFont = dc.SelectObject(&m_fontTitle);
		if (pFont != NULL)
		{
			pOldFont = pFont;
		}
	}
	CRect crTitle(m_crTitle);
	dc.DrawText(m_curHint.m_strTitle, crTitle, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT);
	crTitle.OffsetRect(0, (m_crTitle.Height()-crTitle.Height())/2);
	dc.DrawText(m_curHint.m_strTitle, crTitle, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

	// text of hint
	if (m_fontText.m_hObject)
	{
		CFont* pFont = dc.SelectObject(&m_fontText);
		if ((pFont != NULL) && (pOldFont == NULL))		// Not set yet.
		{
			pOldFont = pFont;
		}
	}
	dc.DrawText(m_curHint.m_strText, m_crText, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);

	// restore DC
	if (pOldPal != NULL)
	{
		dc.SelectPalette(pOldPal, FALSE);
		pOldPal = NULL;
	}

	if (pOldFont != NULL)
	{
		dc.SelectObject(pOldFont);
		pOldFont = NULL;
	}

	dc.SetBkMode(oldBkMode);
}

/////////////////////////////////////////////////////////////////////////////
// CWndModalHlpHint class --
// a modal helpful hint used when we need to
// spawn a separate executable after hint is shown

CWndModalHlpHint::CWndModalHlpHint()
{
	m_fGrabCapture = FALSE;
	m_fInModalLoop = FALSE;
}

CWndModalHlpHint::CWndModalHlpHint(CHelpfulHint* pHH)
	:CWndHlpHint(pHH)
{
	m_fGrabCapture = FALSE;
	m_fInModalLoop = FALSE;
	m_nModalRet = 0;
}

CWndModalHlpHint::~CWndModalHlpHint()
{
}

int CWndModalHlpHint::ExecuteModalLoop()
{
	m_fInModalLoop = TRUE;
	return RunModalLoop();
}

BEGIN_MESSAGE_MAP(CWndModalHlpHint, CWndHlpHint)
	//{{AFX_MSG_MAP(CWndModalHlpHint)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HINTCLOSE, OnHintClose)
	ON_COMMAND(ID_HINTCONTINUE, OnHintContinue)
	ON_COMMAND(ID_HINTTUTORIAL, OnHintTutorial)
	ON_COMMAND(ID_HINTHELP, OnHintHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define CONTINUE_BTN_HEIGHT      24
#define CONTINUE_BTN_WHITESPACE  6
int CWndModalHlpHint::GetExtraHeight()
   {
      // Allow for Continue button height plus some whitespace
      return CONTINUE_BTN_HEIGHT + CONTINUE_BTN_WHITESPACE;
   }

/////////////////////////////////////////////////////////////////////////////
// CWndModalHlpHint message handlers

int CWndModalHlpHint::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndHlpHint::OnCreate(lpCreateStruct) == -1)
		return -1;

	// we want the modal ones centered
	CenterWindow();

	CRect crClient;
	GetClientRect(crClient);

#ifdef WANT_SHOWHINTS_CHECKBOX
   // Move ShowHints again checkbox to bottom of window
   CRect crShowHints;
   int nShowHintsHeight;
   m_btnShowHints.GetWindowRect(&crShowHints);
   ScreenToClient(&crShowHints);
   nShowHintsHeight = crShowHints.Height();
   crShowHints.top = (crClient.bottom-cShadowPixels-8) - crShowHints.Height();
   crShowHints.bottom = crShowHints.top + nShowHintsHeight;
   m_btnShowHints.SetWindowPos(NULL, crShowHints.left, crShowHints.top, NULL, NULL, SWP_NOZORDER|SWP_NOSIZE);
#else
	m_btnShowHints.ShowWindow(SW_HIDE);
#endif

	// Create continue button and position it above ShowHints again control
	CRect crContinueButton;
	crContinueButton.SetRect(0, 0, 78, CONTINUE_BTN_HEIGHT);

#ifdef WANT_SHOWHINTS_CHECKBOX
	CPoint cpButton(0, crShowHints.top - (CONTINUE_BTN_WHITESPACE+4));
	crContinueButton.OffsetRect(
		(((crClient.right - crClient.left) - crContinueButton.Width()) / 2),
		cpButton.y - crContinueButton.Height());
#else
	CPoint cpButton(crClient.right-cShadowPixels-8, crClient.bottom-cShadowPixels-8);
	crContinueButton.OffsetRect(
		(((crClient.right - crClient.left) - crContinueButton.Width()) / 2),
		cpButton.y - crContinueButton.Height());
#endif

	m_btnContinue.Create(crContinueButton, this, ID_HINTCONTINUE);
	m_btnContinue.SetBitmaps("HINT_CONT_U", "HINT_CONT_D", pOurPal);
	m_btnContinue.SetTransparentColor(RGB(255,0,255));

	return 0;
}

void CWndModalHlpHint::OnHintContinue()
{
	StopSound();
	EndModalLoop(m_nModalRet);
	m_fInModalLoop = FALSE;
}

void CWndModalHlpHint::OnHintClose()
{
	StopSound();	  
	EndModalLoop(1);
	m_fInModalLoop = FALSE;
}

void CWndModalHlpHint::OnHintTutorial()
{
	StopSound();
	((CMainFrame*)AfxGetMainWnd())->ShowTutorial(m_curHint.m_nTutorialID);
}

void CWndModalHlpHint::OnHintHelp()
{
	StopSound();
	AfxGetApp()->WinHelp(m_curHint.m_nHelpID);
}

void CWndModalHlpHint::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_fUserLButtonDown = TRUE;
	
	if(GetCapture() != this)
		SetCapture();
	
	// save current cursor position
	::GetCursorPos(&m_ptLastCursorPos);

	// save current window position, and clear last position
	GetWindowRect(&m_crDragCur);
	m_crDragLast.SetRect(0,0,0,0);

	CRect Rect;
	GetWindowRect(&Rect);

	CPoint PtTemp(m_ptLastCursorPos);
	
	if(!Rect.PtInRect(PtTemp))
	{
		OnHintContinue();
	}
	else
	{
		CWnd* pWnd = WindowFromPoint(m_ptLastCursorPos);
		if (pWnd && (pWnd != this))
		{
			ReleaseCapture();
			LPARAM lParam = MAKELPARAM(point.x, point.y);
			pWnd->SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, lParam);
			GetCapture();
		}
	}
}

void CWndModalHlpHint::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt;
	::GetCursorPos(&pt);

	CRect Rect;
	GetWindowRect(&Rect);

	if(!Rect.PtInRect(pt))
	{
		OnHintContinue();
	}
}

void CWndModalHlpHint::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar != VK_ESCAPE)
		OnHintContinue();
	else
		OnHintClose();
}

/////////////////////////////////////////////////////////////////////////////
// CRecentHintsDlg dialog

CRecentHintsDlg::CRecentHintsDlg(const CArray<CHint*,CHint*>& hints, CHelpfulHint* pHH)
	: CDialog(CRecentHintsDlg::IDD, NULL)
{
	//{{AFX_DATA_INIT(CRecentHintsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pHelpfulHint = pHH;
	m_HintsArray.Copy(hints);
}


void CRecentHintsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecentHintsDlg)
	DDX_Control(pDX, IDC_HINTSLIST, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecentHintsDlg, CDialog)
	//{{AFX_MSG_MAP(CRecentHintsDlg)
	ON_LBN_DBLCLK(IDC_HINTSLIST, OnDblclkHintslist)
	ON_BN_CLICKED(IDC_SHOWHINT, OnShowHint)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecentHintsDlg message handlers

BOOL CRecentHintsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// show titles in listbox
	for (int i = m_HintsArray.GetSize()-1; i >= 0; i--)
	{
		int index = m_ctrlList.AddString(m_HintsArray[i]->m_strTitle);
		m_ctrlList.SetItemDataPtr(index, m_HintsArray[i]);
	}

	m_ctrlList.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRecentHintsDlg::OnDblclkHintslist() 
{
	OnShowHint();
}

void CRecentHintsDlg::OnShowHint() 
{
	int sel = m_ctrlList.GetCurSel();
	if (sel != LB_ERR)
	{
		CHint* pHint = (CHint*)m_ctrlList.GetItemDataPtr(sel);
		// position hint to right of ourselves, unless we're right of center,
		// in which case position to left.
		CRect rect;
		GetWindowRect(&rect);
		int scrnWidth = GetSystemMetrics(SM_CXFULLSCREEN);
		CPoint pt(rect.right, rect.top);
		if (rect.left > scrnWidth / 2)
			pt.x = rect.left - cHintWndWidth;
		m_pHelpfulHint->CreateHint(pHint, this, &pt, FALSE, FALSE);
	}
}

void CRecentHintsDlg::OnDestroy() 
{
	m_pHelpfulHint->RemoveHelpfulHint();
	CDialog::OnDestroy();
}


HBRUSH CWndHlpHint::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return hbr;
}


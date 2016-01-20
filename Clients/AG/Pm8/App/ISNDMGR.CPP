/*
// $Header: /PM8/App/ISNDMGR.CPP 1     3/03/99 6:06p Gbeddow $
//
// Sound manager implementation.
//
// $Log: /PM8/App/ISNDMGR.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:54   Fred
// Initial revision.
// 
//    Rev 1.5   12 Dec 1996 13:16:38   Fred
// Quick hacks to install 4.0
// 
//    Rev 1.4   17 Sep 1996 08:36:12   Fred
//  
// 
//    Rev 1.3   07 Aug 1996 13:11:14   Fred
//  
// 
//    Rev 1.2   07 May 1996 12:43:58   Jay
// From PMW2
// 
//    Rev 1.6   25 Apr 1996 14:47:14   JAY
// Now handles sound state correctly.
// 
//    Rev 1.5   25 Apr 1996 14:44:26   JAY
// get_state() returns QUIET if the sound manager is turned off.
// 
//    Rev 1.4   19 Apr 1996 08:52:24   JAY
// Won't allow play_sound if sound_manager is not open.
// 
//    Rev 1.3   05 Dec 1995 09:40:34   JAY
// Sound changes.
// 
//    Rev 1.2   17 Oct 1995 16:57:02   JAY
// Some TRACE's.
// 
//    Rev 1.1   13 Oct 1995 14:19:58   FRED
// 32 bit changes
// 
//    Rev 1.0   27 Aug 1995 19:22:38   FRED
// Initial revision.
// 
//    Rev 2.13   27 Aug 1995 18:30:50   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.12   22 Aug 1995 17:29:36   JAY
// Enabled the od for the sound id.
// 
//    Rev 2.11   08 Aug 1995 12:10:34   FRED
// Open databases read-only
// 
//    Rev 2.10   07 Aug 1995 17:12:36   FRED
// Publishing Suite Install
// 
//    Rev 2.9   24 Jul 1995 14:09:12   JAY
// Fixed a sidebar activation bug. One still remains.
// 
//    Rev 2.8   20 Jul 1995 17:23:00   JAY
// Now checks for NULL in a place that it was failing to do so.
// 
//    Rev 2.7   23 Jun 1995 11:46:48   JAY
// Checks for missing pContext in UnhookWindow.
// 
//    Rev 2.6   09 Jun 1995 07:42:58   FRED
// Sound Manager changes
// 
//    Rev 2.5   07 Jun 1995 16:46:16   FRED
// Album dialog and Project album dialog.
// 
// 
//    Rev 2.4   05 Jun 1995 11:34:48   JAY
//  
// 
//    Rev 2.3   19 Apr 1995 12:32:24   JAY
//  
// 
//    Rev 2.2   15 Mar 1995 10:48:38   JAY
// Support for new text editing
// 
//    Rev 2.1   08 Feb 1995 13:35:00   JAY
// Reverted. New series.
// 
//    Rev 1.38   02 Sep 1994 10:11:34   JAY
// New sound mechanism
// 
//    Rev 1.37   04 Aug 1994 07:34:50   JAY
// Large model and 32-bit changes
// 
//    Rev 1.36   19 Jul 1994 09:07:44   JAY
// SoundID of 0 now means "no sound for this entry".
// 
//    Rev 1.35   18 Jul 1994 10:09:50   JAY
// access() -> access_file().
// Can now inspect sound buttons.
// Couldn't find sound #0 because of NULL ambiguity.
// 
//    Rev 1.34   15 Jul 1994 14:07:32   FRED
// Added flag to set_state (default TRUE) that
// specifies whether to save state to INI file.
// 
// 
//    Rev 1.33   15 Jul 1994 12:08:06   JAY
// access_file().
// 
//    Rev 1.32   15 Jul 1994 10:40:02   JAY
// Support for monochrome sound buttons.
// 
//    Rev 1.31   11 Jul 1994 09:05:56   JAY
// Handles final device closings correctly now.
// 
//    Rev 1.30   07 Jul 1994 14:19:46   JAY
// Uses sidebar context id for cases with no dialog on screen.
// 
//    Rev 1.29   24 Jun 1994 14:14:10   JAY
// New path stuff in play_sound().
// 
//    Rev 1.28   21 Jun 1994 13:37:08   JAY
// 
//    Rev 1.27   17 Jun 1994 09:30:04   JAY
// Added bye() method to MCIWnd to allow the window to be destroyed before the
// destructor is called.
// 
//    Rev 1.26   15 Jun 1994 14:20:00   JAY
// Changed args to open to make it more intelligent.
// 
//    Rev 1.25   15 Jun 1994 14:06:30   JAY
// Created PathManager interface and removed other PMW dependencies (SoundManager).
// 
//    Rev 1.24   15 Jun 1994 09:35:24   JAY
// Now sizes a dialog only as much as necessary to fit the sound buttons on it.
// 
//    Rev 1.23   14 Jun 1994 17:04:34   JAY
// Sound inspector now only plays one sound, then drops out of inspect mode.
// Now uses OurGetProfile... to read from both INI files.
// 
//    Rev 1.22   23 May 1994 09:09:24   JAY
// Various cleanups.
// 
//    Rev 1.21   16 May 1994 08:47:52   JAY
// Added support for calls through for AVI files.
// 
//    Rev 1.20   12 May 1994 07:37:30   JAY
// Now uses path manager instead of APP working_directory.
// 
//    Rev 1.19   06 May 1994 17:07:12   JAY
// Converted find_sound_entry() to use a binary search (not linear anymore).
// 
//    Rev 1.18   06 May 1994 10:33:58   JAY
// Added the "Illegal" cursor.
// 
//    Rev 1.17   04 May 1994 12:22:50   FRED
// Disables sound control buttons for "stacked" dialogs.
// 
// 
//    Rev 1.16   03 May 1994 17:05:22   JAY
// SoundContext::initialize wasn't initializing m_wDeviceID to 0 (stuck buttons
// in dialogs).
// 
//    Rev 1.15   03 May 1994 16:20:00   JAY
// Corrected the error number for device not available.
// Got rid of some debugging statements.
// 
//    Rev 1.14   03 May 1994 14:43:52   JAY
// Fixed a bug in find_sound_entry().
// Cleaned up some of the sound inspector stuff.
// Fixed clicking on the play button when there is no current sound.
// 
//    Rev 1.13   03 May 1994 11:21:00   JAY
// Added update_context_buttons() to simplify decision-making process about
// disabled and up/down states of the buttons.
// Simplified play_context_sound() and play_sound() to only use the current
// sound context for the device id (got rid of params).
// Simplified the settings of the current sound context (routine).
// Moved initialize/deinitialize routines into SoundContext.
// 
//    Rev 1.12   03 May 1994 09:54:54   FRED
// Speaker sound control button is now created with
// the proper ID, ID_SOUND_SPEAKER.
// 
// 
//    Rev 1.11   03 May 1994 08:53:46   JAY
// Fixed key stuff.
// Stop button now pops up correctly.
// 
//    Rev 1.10   02 May 1994 17:24:56   FRED
// Supports new style sound buttons where all four
// states are contained in a single bitmap.
// 
// 
//    Rev 1.9   02 May 1994 11:57:00   JAY
// Added m_current_sound_context.
// Added hook code for special sound manager keystrokes.
// 
//    Rev 1.8   02 May 1994 09:08:58   JAY
// Added current_sound_context() and some other tweaks.
// 
//    Rev 1.7   29 Apr 1994 18:46:08   FRED
// Performs more of the sound button control functions.
// 
// 
//    Rev 1.6   29 Apr 1994 17:48:40   FRED
// Added code to expand dialog boxes for
// sound control buttons.
// 
// Added start of code to repsond to buttons.
// 
// 
//    Rev 1.5   29 Apr 1994 17:13:16   JAY
// Added an external callback mechanism.
// 
//    Rev 1.4   29 Apr 1994 14:59:44   JAY
// Sound inspector code.
// 
//    Rev 1.3   29 Apr 1994 14:08:40   FRED
// Adds sound buttons to dialogs specified in
// entering_dialog(), leaving_dialog(), and
// showing_sidebar() methods.
// 
// 
//    Rev 1.2   28 Apr 1994 16:12:58   JAY
// The beginnings of the sound inspector.
// 
//    Rev 1.1   28 Apr 1994 10:44:04   JAY
// New sound code
// 
//    Rev 1.0   27 Apr 1994 14:40:22   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "resource.h"
#include "pmwini.h"
#include "soundmgr.h"
#include "utils.h"
#include "system.h"
#include "mainfrm.h"

#if 0
#include "file.h"
#endif

#include "dlgmgr.h"

#ifdef _WIN32
#include <afximpl.h>
#else
#include <auxdata.h>
#endif

#include <afxpriv.h>

#include <stdlib.h>
#include <io.h>

#ifdef DOSOUNDS

/*
// The current settings and what they mean:
//
// EXPERIENCED_LEVEL		: 50
// GAIN_EXPERIENCE_RATE : 67
// LOSE_EXPERIENCE_RATE : 12		(about 800%)
// FORGET_INTERVAL		: 40 days
//
// Rate		Times to switch	Times to full learn	Interval to forget
// ----		---------------	-------------------	------------------
//  0				infinite				infinite				(1/16)	[2 1/2 days]
// 25				   3						6					1/12		[3 1/3 days]
// 50					2						3					1/8		[5 days]
// 75					1						2					1/4		[10 days]
// >94				1						2					never
*/

/*
// The level (0-100) at which we consider the user experienced.
//
// This is the threshold for determining if a user is experienced or not.
//
// Those with experience at least as large as this are considered experienced.
// Those with experience less than this are not.
*/

#define EXPERIENCED_LEVEL	50

/*
// The scale factor for gaining experience.
//
// This determines how much of the user's learning rate we add to a sound's
// experience level when the user hears it. It is a percent at this point.
//
// Setting it to 100 will apply all of the user's learning rate.
// Setting it to 0 will apply none of the user's learning rate (NOT USEFUL).
//
// The current setting  (67) means that a user with above about a level of 75
// will learn a context upon hearing it for the first time. A user with
// level 50 will need to visit a place two times before it shuts up.
*/

#define GAIN_EXPERIENCE_RATE	67

/*
// Some time constants. time_t values are in seconds.
*/

#define TIME_T_SECOND	1L
#define TIME_T_MINUTE	(60L*TIME_T_SECOND)
#define TIME_T_HOUR		(60L*TIME_T_MINUTE)
#define TIME_T_DAY		(24L*TIME_T_HOUR)

/*
// The "forget" interval. MIN and MAX determine the endpoints of the interval.
*/

#define MIN_FORGET_TIME		0
#define MAX_FORGET_TIME		(40*TIME_T_DAY)
#define FORGET_INTERVAL		(MAX_FORGET_TIME - MIN_FORGET_TIME)

/*
// The "forget" rate. This determines how much of the user's experience rate
// we apply in the negative direction to guess whether the user has forgotten
// or not.
//
// This number is the denominator of the scale; so 100 is 100%, 200 is 50%,
// and 400 is 25%.
//
// What does this number mean? At the MIN_FORGET_TIME, all adjustments are 0.
// At the MAX_FORGET_TIME, the adjustment is (100-learning_rate)/LEP. Assume
// that the LEP is set to 50, which means a 200% forget rate. Then
// at the MAX_FT, a user with learning level 0 will have '200' subtracted
// from his experience. A user with learning level 50 will have '100'
// subtracted from his experience. In order to definitely have a sound
// be considered forgotten (assume its completely learned, with an experience
// of 100), then the "forget" amount must be 50 to make it drop below the
// 50 (EXPERIENCED_LEVEL) level again. So for a fully learned context, someone
// with experience 0 (everything else ignored - could theoretically never
// fully learn something) would have definitely forgotten after 1/4 the
// forget interval since the whole interval maps to 200; someone with
// experience 50 would have forgotten after 1/2 the forget interval since
// the interval maps to 100 at these rates.
//
//   User level	0			25			50			75			100
// Time interval
// MIN (0%)			0			0			0			0			0
//					50%(1/4)
//								50%(1/3)
//											50%(1/2)
//
//
// MAX(100%)		200		150		100		50(1)		0
//
// This means that someone over a level of 75 would never forget - perhaps
// not such a good quality.
//
// The current setting (800) means that a level 0 would (again, in theory)
// forget after 1/16 the time interval, a level 25 would forget after 1/12
// the time interval, a level 50 would lose after 1/8 the time interval, and
// a level 75 would forget after 1/4 the time interval. A person with level
// higher than about 94 will NEVER forget.
//
// The lower the LEP, the sooner people forget. The higher the LEP the longer
// it takes people to forget (since not as much gets subtracted out).
*/

#define LOSE_EXPERIENCE_RATE		12				// A 800% rate.

/*
// A default name place.
*/

char SoundManager::learning_file_template[11+1] = "_xxxPMW.SND";

/* Plays a specified WAVE resource */
static BOOL PlayResource(LPSTR lpName)
{
	HINSTANCE hInst = AfxGetResourceHandle();
	BOOL bRtn = FALSE;

	/* Find the WAVE resource */
	HRSRC hResInfo = FindResource(hInst, lpName, "WAVE");
	if (hResInfo != NULL)
	{
		/* Load the WAVE resource */
		HGLOBAL hRes = LoadResource(hInst, hResInfo);
		if (hRes != NULL)
		{
			/* Lock the WAVE resource and play it */
			LPCSTR lpRes = (LPCSTR)LockResource(hRes);
			if (lpRes != NULL)
			{
				bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
				UnlockResource(hRes);
			}
		}

		/* Free the WAVE resource and return success or failure */
		FreeResource(hRes);
	}

	return bRtn;
}

/*
// Constructor for the sound manager.
*/

SoundManager::SoundManager()
{
	m_iSoundBits = 16;						// Assume 16 until corrected.
	m_bOpen = FALSE;							// Not open yet.

//	m_sidebar_context.m_wContextID = 0;	// No sidebar yet.
//	m_dialog_index = -1;						// No dialogs yet.

	m_state = SMS_LEARNING;					// Learning mode is the standard.
	m_learning_rate = 0;						// But never learn.

/* Hook filter. */

	m_hhook = NULL;

/* Sound map data. */

	m_hSoundMap = NULL;						// No sound map resource handle yet.
	m_sound_map = NULL;						// No sound map yet.

/* Learning data. */

	m_learning_data = NULL;					// No learning data yet.
	m_learning_file = NULL;					// The name of the learning file.

/* Sound inspector variables. */

	m_bSoundInspector = FALSE;
	m_hcurInspect = NULL;
	m_hcurIllegal = NULL;
	m_wLastInspectContext =
		m_wLastInspectSpecific = 0;

/* The current sound context. */

	m_current_sound_context = NULL;
	m_pSidebarContext = NULL;
}

/*
// Destructor for the sound manager.
*/

SoundManager::~SoundManager()
{
	close();
}

/*
// Initialize the sound map.
*/

ERRORCODE SoundManager::init_sound_map(WORD wResourceID)
{
	HINSTANCE hInst = AfxGetResourceHandle();

/* Find the resource. */

	HRSRC hRsrc;

	if ((hRsrc = ::FindResource(hInst, MAKEINTRESOURCE(wResourceID), MAKEINTRESOURCE(RT_SOUNDMAP))) == NULL)
	{
		return ERRORCODE_DoesNotExist;
	}

/* Load the resource. */

	if ((m_hSoundMap = ::LoadResource(hInst, hRsrc)) == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Lock the resource. */

	SoundMapPtr res_map;

	if ((res_map = (SoundMapPtr)::LockResource(m_hSoundMap)) == NULL)
	{
		::FreeResource(m_hSoundMap);
		m_hSoundMap = NULL;
		return ERRORCODE_Memory;
	}

/*
// We now have the sound map loaded.
// Generate the necessary statistics we need to configure the rest of the
// sound manager.
//
// m_wEntryCount: We compute the number of sound entries now to simplify
// the search code later.
//
// m_wContextCount: The largest sound ID of a context entry determines the
// size of the learning data.
*/

	SoundEntryPtr entry;

	for (m_wEntryCount = 0, m_wContextCount = 0, entry = res_map->sound_entry;
					entry->wContextID !=0 || entry->wSpecificID != 0 || entry->wSoundID != 0;
					entry++, m_wEntryCount++)
	{
		if (entry->wFlags & SEF_context)
		{
		/* Make sure we can cover this entry in our context data. */

			if (entry->wSoundID > m_wContextCount)
			{
				m_wContextCount = entry->wSoundID;
			}
		}
	}

/*
// We need to duplicate the data since we want to modify it.
*/

	WORD wSize = m_wEntryCount*sizeof(SoundEntry);

	if ((m_sound_map = (SoundMapPtr)system_heap->allocate(wSize)) == NULL)
	{
		free_sound_map();
		return ERRORCODE_Memory;
	}

	memcpy(m_sound_map, res_map, wSize);

#if 0
/*
// Allocate the learning data.
// Note that when we access this, we subtract one from the sound id
// (i.e. sound ID #1 maps to context data entry #0).
*/

	m_wContextDataSize = m_wContextCount*sizeof(SoundContextData);

	if ((m_learning_data =
		  (SoundContextDataPtr)system_heap->allocate(m_wContextDataSize)) == NULL)
	{
		free_sound_map();
		return ERRORCODE_Memory;
	}

/* Set the learning data to defaults. */

	memset(m_learning_data, 0, m_wContextDataSize);
#endif

	return ERRORCODE_None;
}

/*
// Free the sound map.
*/

VOID SoundManager::free_sound_map(VOID)
{
/* Free any learning data. */

	if (m_learning_data != NULL)
	{
		system_heap->free(m_learning_data);
		m_learning_data = NULL;
	}

/* Free the sound map if we allocated one. */

	if (m_sound_map != NULL)
	{
		system_heap->free(m_sound_map);
		m_sound_map = NULL;
	}

/* Get rid of our sound map resource. */

	if (m_hSoundMap != NULL)
	{
		UnlockResource(m_hSoundMap);
		FreeResource(m_hSoundMap);
		m_hSoundMap = NULL;
	}
}

#if 0
/*
// Read the user's learning file.
*/

ERRORCODE SoundManager::read_learning_file(PCSTR filename)
{
	ERRORCODE error;

/*
// We can simply declare a ReadOnlyFile at this point because ReadOnlyFile
// will validate the non-NULL-ness of the file name at open time.
*/

	ReadOnlyFile file(filename);

/*
// Read the number of entries in the learning map.
*/

	WORD entry_count;

	if ((error = file.read(&entry_count, sizeof(entry_count))) != ERRORCODE_None)
	{
	/* NULL-pointer, file not found or other error. */
		return error;
	}

/*
// Do a validity check. The file from this point on should be just the sound
// entries. Make sure the numbers match up.
*/

	WORD wOldDataSize = entry_count*sizeof(SoundContextData);

	ST_DEV_POSITION here, length;

	if ((error = file.tell(&here)) != ERRORCODE_None
			|| (error = file.length(&length)) != ERRORCODE_None)
	{
		return error;
	}

	if (here+(ST_DEV_POSITION)wOldDataSize != length)
	{
	/* Something is dreadfully wrong. Ignore this bad file. */
		return ERRORCODE_IllegalType;
	}

/*
// We need to read the data. We only read what will fit in our array
// or what is in the file.
*/

	WORD wSizeToRead = __min(m_wContextDataSize, wOldDataSize);

	return file.read(m_learning_data, wSizeToRead);
}

/*
// Get the user's learning file.
*/

ERRORCODE SoundManager::use_learning_file(PCSTR filename)
{
/* Remember the learning file name. */

	m_learning_file = filename;

/* Try to read the lasty settings. */

	read_learning_file(m_learning_file);

/* Always returns no error. Either read or initialized at creation time. */

	return ERRORCODE_None;
}

/*
// Update the user's learning file.
*/

ERRORCODE SoundManager::update_learning_file(VOID)
{
	ERRORCODE error;

/* If we have no file, we can't do much. */

	if (m_learning_file == NULL)
	{
		return ERRORCODE_None;
	}

/* Create a new learning file. */

	StorageFile file(m_learning_file);

	if ((error = file.initialize()) != ERRORCODE_None)
	{
	/* Couldn't create the file for some reason. */
		return error;
	}

/* Write the number of context entries followed by the actual entries. */

	if ((error = file.write(&m_wContextCount, sizeof(m_wContextCount))) != ERRORCODE_None
		 || (error = file.write(m_learning_data, m_wContextDataSize)) != ERRORCODE_None)
	{
	/* Delete the file? */
	/* I guess so... */
		file.zap();
	}

	return error;
}
#endif

/*
// Open routine for the sound manager.
*/

ERRORCODE SoundManager::open(BOOL fMonochrome, SOUND_MANAGER_STATE state, PCSTR sound_path, IPathManager *path_manager, PCSTR learning_file, WORD learning_rate)
{
	return ERRORCODE_DoesNotExist;

	ERRORCODE error;

	if (sound_path == NULL)
	{
		m_sound_path = "";
	}
	else
	{
		m_sound_path = sound_path;
	}
	m_state = state;
	m_learning_rate = learning_rate;
	m_path_manager = path_manager;
	m_fMonochrome = fMonochrome;

/*
// Try to determine if a sound card is available and its bit support.
*/

	if (!PlayResource("BLANK16")
			 && (m_iSoundBits = 8, !PlayResource("BLANK8")))
	{
	/* Unable to play sounds. */
		return ERRORCODE_DoesNotExist;
	}

/*
// Get the sound map.
// If we don't have this, we have nothing.
*/

	if ((error = init_sound_map(IDSM_MAIN_SOUND_MAP)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Remember the learning file name.
*/

#if 0
	if ((error = use_learning_file(learning_file)) != ERRORCODE_None)
	{
		return error;
	}
#endif

	if (!m_mciwnd.init(mci_callback, this))
	{
		close();
		return ERRORCODE_Memory;
	}

	m_bOpen = TRUE;

/*
// This is our hook handle.
*/

#ifdef _WIN32
	m_hhook = ::SetWindowsHookEx(WH_KEYBOARD,
		(HOOKPROC)message_filter,
		NULL,
		::GetCurrentThreadId());
#else
	m_hhook = ::SetWindowsHookEx(WH_KEYBOARD,
		(HOOKPROC)message_filter,
		_AfxGetHookHandle(),
		::GetCurrentTask());
#endif

/*
// Load our help cursor. If this fails, we'll continue anyway.
*/

	m_hcurInspect = AfxGetApp()->LoadCursor(IDC_SOUNDINSPECT);
	m_hcurIllegal = AfxGetApp()->LoadCursor(IDC_ILLEGAL_CURSOR);

	return ERRORCODE_None;
}

/*
// Close the sound manager.
*/

VOID SoundManager::close(VOID)
{
/*
// Stop the current sound.
*/

	if (m_bOpen)
	{
		m_mciwnd.close_all(MCI_WAIT);
	}

/*
// Unhook us.
*/

	if (m_hhook != NULL)
	{
		::UnhookWindowsHookEx(m_hhook);
		m_hhook = NULL;
	}

/*
// Get rid of our inspector cursor if we loaded.
*/

	if (m_hcurInspect != NULL)
	{
		::DestroyCursor(m_hcurInspect);
		m_hcurInspect = NULL;
	}

/*
// Get rid of our illegal cursor if we loaded.
*/

	if (m_hcurIllegal != NULL)
	{
		::DestroyCursor(m_hcurIllegal);
		m_hcurIllegal = NULL;
	}

/*
// Free up any initialized SoundContext structures.
*/

	POSITION Pos = m_Context.GetHeadPosition();
	
	while (Pos != NULL)
	{
		SoundContext* pContext = (SoundContext*)m_Context.GetNext(Pos);
		pContext->deinitialize();
		delete pContext;
	}
	
	m_Context.RemoveAll();
	
#if 0
	while (m_dialog_index >= 0)
	{
		m_dialog_context[m_dialog_index].deinitialize();
		m_dialog_index--;
	}
	
	if (m_sidebar_context.GetContextID() != 0)
	{
		m_sidebar_context.deinitialize();
	}
#endif

#if 0
/*
// We only write the learning file if the sound manager was actually open.
*/
	if (m_bOpen)
	{
		update_learning_file();
	}

#endif
	free_sound_map();

/*
// Say goodbye to the MCI window.
*/

	m_mciwnd.bye();

	m_state = SMS_QUIET;
	m_bOpen = FALSE;
}

/*
// Toggle the sound state.
// THIS IS OBSOLETE.
*/

VOID SoundManager::toggle_sounds()
{
	set_state(m_state == SMS_QUIET ? SMS_LEARNING : SMS_QUIET);
}

/*
// Sound inspector functions.
*/

VOID SoundManager::sound_inspector(VOID)
{
	CWinApp *pApp = AfxGetApp();
	CMainFrame *pFrm = (CMainFrame *)pApp->m_pMainWnd;
	SoundContext* pContext = current_sound_context();

	if (pContext == NULL || pFrm->m_bHelpMode)
	{
	/* Already in help mode. */
		return;
	}

/* Sub in our inspector cursor for the help mode. */

	HCURSOR hcurSave = afxData.hcurHelp;

	afxData.hcurHelp = afxData.hcurArrow;			// As a default

		if (m_hcurInspect != NULL)
		{
			afxData.hcurHelp = m_hcurInspect;
		}
		
#if 0
	if (m_dialog_index < 0)
	{
		if (m_hcurInspect != NULL)
		{
			afxData.hcurHelp = m_hcurInspect;
		}
	}
	else
	{
		if (m_hcurIllegal != NULL)
		{
			afxData.hcurHelp = m_hcurIllegal;
		}
	}
#endif

/* No "lasts" for this time. */

	m_wLastInspectContext =
		m_wLastInspectSpecific = 0;

/* Turn the explain button on. */

	m_bSoundInspector = TRUE;
	update_context_buttons(pContext);

/* Help mode NOW. */

	do
	{
		pFrm->m_bHelpMode = HELP_ENTERING;

		m_bPickedSomething = FALSE;

//		od("(Entering sound inspector...");

		OnContextHelp();

//		od("exiting sound inspector)\r\n");

	} while (m_bPickedSomething);

/* Restore the normal help cursor. */

	afxData.hcurHelp = hcurSave;

/* Restore the previous state. */

	m_bSoundInspector = FALSE;
	update_context_buttons(pContext);
}

/*
// Actually inspect a sound (by ID).
*/

VOID SoundManager::sound_inspect(DWORD dwData, UINT nCmd)
{
	WORD wID = (WORD)(dwData & 0xFFFF);
	SoundContext* pContext = current_sound_context();

#ifdef MULTIPLE_INSPECT
	m_bPickedSomething = TRUE;
#endif

	switch (wID)
	{
#if 0
		case ID_SOUND_EXPLAIN:
		{
			m_bPickedSomething = FALSE;
			break;
		}

		case ID_SOUND_PLAY:
		{
			play_current_sound();
			break;
		}

		case ID_SOUND_STOP:
		{
			stop_sound();
			break;
		}
#endif

		default:
		{
#if 0
		/* Say we picked something so we can stay in the loop. */
			if (m_dialog_index < 0)
			{
			/* No dialog on the screen. Context is '0'. */

//				m_wLastInspectContext = 0;

			/* Use the context for the sidebar. */

				m_wLastInspectContext = pContext->GetContextID();
			}
			else
			{
			/* Only explain controls of dialog. */

				if ((dwData & 0xFFFF0000L) != 0)
				{
					return;
				}

			/* Use the context for the dialog. */

				m_wLastInspectContext = pContext->GetContextID();
			}
#endif
				m_wLastInspectContext = pContext->GetContextID();

			m_wLastInspectSpecific = (WORD)dwData;

			play_context_sound(m_wLastInspectContext,
										SOUND_TYPE_VERBOSE,
										m_wLastInspectSpecific);

#if 1
			od("[sound inspect: %x (%x)]\r\n",
								m_wLastInspectContext,
								m_wLastInspectSpecific);
#endif
		}
	}
}

SoundContext* SoundManager::FindSoundContext(UINT wContextID)
{
	POSITION Pos = m_Context.GetHeadPosition();
	
	while (Pos != NULL)
	{
		SoundContext* pContext = (SoundContext*)m_Context.GetNext(Pos);
		if (pContext->m_wContextID == wContextID)
		{
			return pContext;
		}
	}
	
	return NULL;
}

SoundContext* SoundManager::FindSoundContextByDevice(UINT wDeviceID)
{
	POSITION Pos = m_Context.GetHeadPosition();
	
	while (Pos != NULL)
	{
		SoundContext* pContext = (SoundContext*)m_Context.GetNext(Pos);
		if (pContext->m_wDeviceID == wDeviceID)
		{
			return pContext;
		}
	}
	
	return NULL;
}

/*
// The callback that the MCI window uses to inform us of sound endings.
*/

void SoundManager::mci_callback(WORD wDeviceID, LPVOID pData)
{
/* Set a pointer to the sound manager. */

	SoundManager *pSM = (SoundManager *)pData;

/* Find the context that has the specified device and turn it off. */

	SoundContext *pSC = pSM->FindSoundContextByDevice(wDeviceID);

#if 0
	if (pSM->m_sidebar_context.m_wDeviceID == wDeviceID)
	{
		pSC = &pSM->m_sidebar_context;
//		od("Turning off device for sidebar\r\n");
	}
	else
	{
		for (int i = 0; i <= pSM->m_dialog_index; i++)
		{
			if (pSM->m_dialog_context[i].m_wDeviceID == wDeviceID)
			{
				pSC = &pSM->m_dialog_context[i];
//				od("Turning off device for dialog index %d\r\n", i);
				break;
			}
		}
	}
#endif

/*
// See what we found (or not)...
*/

	if (pSC != NULL)
	{
	/* The device is no longer valid. */

		pSC->m_wDeviceID = 0;

	/* Update the context for these buttons. */

		pSM->update_context_buttons(pSC);
	}
}
 
/*
// Set the stop and play buttons for a given sound context.
*/

void SoundManager::update_context_buttons(SoundContext* pContext)
{
	if (pContext == NULL)
	{
		return;
	}
	
/*
// Rules:
//
// Stop button is always UP.
//
// If a sound is playing in this context (m_wDeviceID != 0),
// then
//		play is enabled and down
//		stop is enabled
// Else
//		stop is disabled
//		If we are sound inspector mode
//		then
//			play is enabled if a there is a last inspector sound.
//			else play is enabled (context sound is always available).
//
// Explain button is active iff in inspect (explain) mode.
*/

	BOOL bSoundPlaying = pContext->m_wDeviceID != 0;

/* Handle the stop button. */

	if (pContext->m_pStopButton != NULL)
	{
		if (bSoundPlaying)
		{
		/* Sound is playing. Stop button should be UP and ENABLED. */
			pContext->m_pStopButton->TurnButtonOff();
		}
		else
		{
		/* Sound is not playing. Stop button should be DISABLED. */
			pContext->m_pStopButton->DisableButton();
		}
	}

/* Handle the play button. */

	if (pContext->m_pPlayButton != NULL)
	{
		if (bSoundPlaying)
		{
		/* Sound is playing. Play button is active (on). */
			pContext->m_pPlayButton->TurnButtonOn();
		}
		else if (!m_bSoundInspector
						|| m_wLastInspectContext != 0
						|| m_wLastInspectSpecific != 0)
		{
		/*
		// Sound inspector is off (general context), or there is a last
		// inspected sound.
		*/
			pContext->m_pPlayButton->TurnButtonOff();
		}
		else
		{
		/* In sound inspector with no last sound. */
			pContext->m_pPlayButton->DisableButton();
		}
	}

/* Handle the explain button. */

	if (pContext->m_pExplainButton != NULL)
	{
		if (m_bSoundInspector)
		{
		/* In sound inspector. */
			pContext->m_pExplainButton->TurnButtonOn();
		}
		else
		{
		/* Not in sound inspector. */
			pContext->m_pExplainButton->TurnButtonOff();
		}
	}
	
	if (pContext->m_pSpeakerButton != NULL)
	{
		pContext->m_pSpeakerButton->TurnButtonOn();
	}
}

void SoundManager::disable_context_buttons(SoundContext* pContext)
{
	if (pContext == NULL)
	{
		return;
	}
	
	/*
	// This function is used to disable all the context buttons.
	// It is called when contexts are stacked... the previous
	// context has all its buttons disabled.
	*/
	
	if (pContext->m_pSpeakerButton != NULL)
	{
		pContext->m_pSpeakerButton->DisableButton();
	}
	
	if (pContext->m_pStopButton != NULL)
	{
		pContext->m_pStopButton->DisableButton();
	}
	
	if (pContext->m_pPlayButton != NULL)
	{
		pContext->m_pPlayButton->DisableButton();
	}
	
	if (pContext->m_pExplainButton != NULL)
	{
		pContext->m_pExplainButton->DisableButton();
	}
}

/*
// Play the current sound.
// This would correspond to the "play" button in our sound controls.
*/

VOID SoundManager::play_current_sound(SOUND_TYPE nType /*=SOUND_TYPE_VERBOSE*/)
{
	SoundContext* pContext = current_sound_context();
	WORD wContextID, wSpecificID;
	
	if (pContext == NULL)
	{
		return;
	}

/* If we're in "sound inspect" mode, we need to do something special. */

	if (m_bSoundInspector)
	{
	/* We're in sound inspect mode! Replay the last sound inspector sound. */
		wContextID = m_wLastInspectContext;
		wSpecificID = m_wLastInspectSpecific;
	}
	else
	{
	/* Not in "sound inspect" mode. */

		wContextID = pContext->GetContextID();
		wSpecificID = 0;
	}

/* Play the full sound for our context. */

	play_context_sound(pContext->GetContextID(), nType, wSpecificID);

/*
// Make doubly sure that the context buttons are correct since the play
// button may have gone down.
// Only do this if we are still open (there may be no sound card which
// could cause a shutdown; pContext would become invalid).
*/

	if (m_bOpen)
	{
		update_context_buttons(pContext);
	}
}

#if 0
VOID SoundManager::play_current_sound(VOID)
{
	SoundContext* pContext = current_sound_context();
	WORD wContextID, wSpecificID;
	
	if (pContext == NULL)
	{
		return;
	}

/* If we're in "sound inspect" mode, we need to do something special. */

	if (m_bSoundInspector)
	{
	/* We're in sound inspect mode! Replay the last sound inspector sound. */
		wContextID = m_wLastInspectContext;
		wSpecificID = m_wLastInspectSpecific;
	}
	else
	{
	/* Not in "sound inspect" mode. */

		wContextID = pContext->GetContextID();
		wSpecificID = 0;
	}

/* Play the full sound for our context. */

	play_context_sound(wContextID, SOUND_TYPE_VERBOSE, wSpecificID);

/*
// Make doubly sure that the context buttons are correct since the play
// button may have gone down.
*/

	update_context_buttons(pContext);
}
#endif

/*
// Set the sound manager state.
*/

VOID SoundManager::set_state(SOUND_MANAGER_STATE state, BOOL fSave /* = TRUE */)
{
	if (!m_bOpen)
	{
	/* Sorry, we're closed. */
		return;
	}

	if (state != m_state)
	{
		CWinApp *pApp = AfxGetApp();

	/* Set the new state. */

		m_state = state;

		if (fSave)
		{
		/* Remember this forever (or until it's changed). */
	
			pApp->WriteProfileInt(SECTION_Sound, ENTRY_SoundState, m_state);
		}
		
	/* Do what we need to do to make the sound state correct. */

		if (m_state == SMS_QUIET)
		{
			stop_sound();
		}
		else
		{
			play_current_sound();
		}
	}
}

/*
// Get the sound manager state.
*/

SOUND_MANAGER_STATE SoundManager::get_state(VOID)
{
	return m_state;
}

#if 0
/*
// Set the learning rate for the sound manager.
// This updates the "[Sound] LearningRate" key in the private INI file.
*/

VOID SoundManager::set_learning_rate(WORD new_rate)
{
	if (!m_bOpen)
	{
	/* Sorry, we're closed. */
		return;
	}

	if (new_rate != m_learning_rate)
	{
		CWinApp *pApp = AfxGetApp();

	/* Adjust the learning data by the rate change. */

		adjust_learning_data(new_rate);

	/* Set the new state. */

		m_learning_rate = new_rate;

	/* Remember this forever (or until it's changed). */

		pApp->WriteProfileInt(SECTION_Sound, ENTRY_LearningRate, m_learning_rate);

	/* Do what we need to do to make the sound state correct. */
		// Probably, this means reset all learning values.
	}
}

/*
// Adjust the learning data to reflect a learning rate change.
*/

VOID SoundManager::adjust_learning_data(WORD new_rate)
{
	if (m_bOpen && m_learning_data == NULL)
	{
	/*
	// For some reason, at this point it makes sense only to adjust the
	// learning data downward. If we're becoming "stupider" we may want more
	// help so we need to lower the current experience values (which may have
	// been ill-gotten). If we're becoming "smarter", we'll just learn faster
	// from now on.
	//
	// It also helps prevent things like "divide by zero" if upping the rate
	// from 0.
	*/

		if (new_rate < m_learning_rate)
		{
			SoundContextDataPtr scdp = m_learning_data;
			for (WORD i = 0; i < m_wContextCount; i++)
			{
				scdp->iExperience = scale_number(scdp->iExperience, new_rate, m_learning_rate);
			}
		}
	}
}

/*
// Reset all learning to the "not-experienced" level.
*/

VOID SoundManager::reset_learning_data(VOID)
{
	if (m_bOpen && m_learning_data != NULL)
	{
	/* Set the learning data to defaults. */
		memset(m_learning_data, 0, m_wContextDataSize);
	}
}
#endif

/*
// Do a comparison between the key and an array element.
//
// Small model:
// Since the array is actually in far memory, we need to put the segment
// on it before using it.
*/

static bFoundSound;

int SoundManager::sound_entry_compare(PCVOID key, PCVOID elem2)
{
	SoundEntry *skey = (SoundEntry *)key;
#if 0
	SoundEntryPtr selem2 =
		(SoundEntryPtr)MAKELP(SELECTOROF(sound_manager.m_sound_map), (WORD)elem2);
#else
	SoundEntryPtr selem2 = (SoundEntryPtr)elem2;
#endif

#if 0
	od("key %u: %u, %u vs. elem: %u, %u\r\n",
			selem2 - sound_manager.m_sound_map->sound_entry,
			skey->wContextID, skey->wSpecificID,
			selem2->wContextID, selem2->wSpecificID);
#endif

	if (skey->wContextID < selem2->wContextID)
	{
//		od("Key context less than...\r\n");
		return -1;
	}
	else if (skey->wContextID > selem2->wContextID)
	{
//		od("Key context greater than...\r\n");
		return 1;
	}
	else
	{
	/*
	// The context is correct. Check if we are done.
	*/

		if (skey->wSpecificID < selem2->wSpecificID)
		{
//			od("Key specific less than...\r\n");
			return -1;
		}
		else if (skey->wSpecificID > selem2->wSpecificID)
		{
//			od("Key specific greater than...\r\n");
			return 1;
		}
		else
		{
//			od("Key equal...\r\n");
			bFoundSound = TRUE;
			return 0;
		}
	}
}

/*
// Get the sound entry for a context/id pair.
*/

SoundEntryPtr SoundManager::find_sound_entry(UINT wContextID, UINT wSpecificID)
{
/* For now... */
	if (m_sound_map == NULL)
	{
		return NULL;
	}

	SoundEntry key;

	key.wContextID = wContextID;
	key.wSpecificID = wSpecificID;

/*
// If the sound map has an offset of 0, we can't trust the "NULL-ness" of the return to indicate
// not found - it may be the zero'th entry. So we use a global flag to definitively indicate
// a successful find.
*/

	bFoundSound = FALSE;

	void *ret = bsearch(&key, m_sound_map,
							  m_wEntryCount, sizeof(SoundEntry),
							  sound_entry_compare);

	if (!bFoundSound)
	{
		return NULL;
	}
#if 0
	return (SoundEntryPtr)MAKELP(SELECTOROF(sound_manager.m_sound_map), (WORD)ret);
#else
	return (SoundEntryPtr)ret;
#endif
}

#if 0
/*
// Return the context data for this entry.
// An entry only has data if it's a "context" sound entry.
// We also provide some additional checks to make sure it's in range.
*/

SoundContextDataPtr SoundManager::data_for_entry(SoundEntryPtr entry)
{
	WORD wSoundID = entry->wSoundID;

	if ((entry->wFlags & SEF_context)
			&& wSoundID > 0
			&& wSoundID <= m_wContextCount)
	{
	/* Sound id's are 1 based. So we make it 0 based for indexing. */
		return m_learning_data + (entry->wSoundID - 1);
	}

/* No context data for this sound. */

	return NULL;
}

/*
// Is the user experienced for this sound entry?
*/

BOOL SoundManager::user_is_experienced(SoundEntryPtr entry)
{
	SoundContextDataPtr pCD;

	if ((pCD = data_for_entry(entry)) != NULL)
	{
	/*
	// Determine if the user is considered experienced for this sound.
	// This is part 1 of the experience heuristic.
	*/

		if (pCD->lLastTime == 0)
		{
		/* First time ever. Definitely no experience. */
			return FALSE;
		}

	/*
	// Check the experience adjusted by the forget value.
	*/

		int iExperience = pCD->iExperience;

	/* Adjust the experience level by the "forget" rate. */

		time_t now = time(NULL);
		time_t interval = now - pCD->lLastTime;

		if (interval >= MIN_FORGET_TIME)
		{
			int forget_probability;

		/* The user may have forgotten something. */

			if (interval > MAX_FORGET_TIME)
			{
			/* The user is considered to have completely forgotten. */
				forget_probability = 100;
			}
			else
			{
				interval -= MIN_FORGET_TIME;
				forget_probability = (int)scale_pcoord(interval, 100, FORGET_INTERVAL);
			}

		/*
		// Adjust the experience (locally) for the time difference.
		// We don't want to assume they've completely lost their experience here,
		// so we only compute the reduced experience for this one time. The
		// saved experience value is not reduced. That way, after this time,
		// they'll be back on track.
		*/

			iExperience -= scale_number(forget_probability,
												100 - m_learning_rate,
												LOSE_EXPERIENCE_RATE);
		}
		return iExperience >= EXPERIENCED_LEVEL;
	}

/* We can't know, so play it safe. Assume they don't know. */

	return FALSE;
}

/*
// Update the learning experience for a sound entry.
*/

VOID SoundManager::update_experience(SoundEntryPtr entry)
{
/* We have now heard this sound. */

	entry->wFlags |= SEF_already_heard;

	SoundContextDataPtr pCD;

	if ((pCD = data_for_entry(entry)) != NULL)
	{
	/*
	// Update the context data.
	// This is part 2 of the experience heuristic.
	*/
	/* Remember when we last heard this. */

		pCD->lLastTime = time(NULL);

	/* Bump the experience as needed. */

		pCD->iExperience += scale_number(m_learning_rate, GAIN_EXPERIENCE_RATE, 100);
		if (pCD->iExperience > 100)
		{
			pCD->iExperience = 100;
		}
	}
}
#endif

/*
// Play a sound by context, type and optional ID.
*/

VOID SoundManager::play_context_sound(WORD wContextID, SOUND_TYPE type, WORD wSpecificID)
{
	if ((type == SOUND_TYPE_DECIDE && m_state == SMS_QUIET)
				|| (wContextID == 0 && wSpecificID == 0))
	{
	/* No talking aloud allowed! */
		return;
	}

/*
// Lookup the sound entry.
*/

	SoundEntryPtr pEntry;

	if ((pEntry = find_sound_entry(wContextID, wSpecificID)) == NULL)
	{
	/*
	// We couldn't find the desired entry.
	// If this is a 'specific' id, see if there is a general context
	// matching it.
	*/

		if (wSpecificID == 0 ||
				(pEntry = find_sound_entry(MATCH_ANY_CONTEXT, wSpecificID)) == NULL)
		{
		/* We don't support this sound. */
			return;
		}
	}

/*
// If the sound id is 0, then this entry has no sound.
*/

	if (pEntry->wSoundID == 0)
	{
	/* No sound for this guy. */
		return;
	}

/*
// Decide on a type to use based on the type passed.
// The case of SOUND_TYPE_VERBOSE is the simplest - we always do it.
// The case of SOUND_TYPE_BRIEF is next simplest - we always do it IF the
// sound entry indicates it has a brief sound.
// The last case is SOUND_TYPE_LEARNING. This case is the most complex,
// being determined by the sound manager's state as well as the state of
// the sound entry (flags and experience).
*/

	switch (type)
	{
		case SOUND_TYPE_BRIEF:
		{
		/* We will use BRIEF unless the entry doesn't support it. */
			if (!(pEntry->wFlags & SEF_has_brief))
			{
				type = SOUND_TYPE_VERBOSE;
			}
			break;
		}
		case SOUND_TYPE_DECIDE:
		{
		/* See what the entry supports. */
			type = SOUND_TYPE_VERBOSE;

#if 0
			if (pEntry->wFlags & SEF_has_brief)
			{
				if (m_state == SMS_BRIEF
						||	((pEntry->wFlags & SEF_once_is_enough) == SEF_once_is_enough))
				{
					type = SOUND_TYPE_BRIEF;
				}
				else if (m_state == SMS_LEARNING)
				{
				/* We are in a learning state and we have two choices. */

					type = user_is_experienced(pEntry)
									? SOUND_TYPE_BRIEF
									: SOUND_TYPE_VERBOSE;
				}
			}
#endif
			break;
		}
		default:
		{
			break;
		}
	}

/*
// Construct the sound name.
*/

	UINT wSoundID = pEntry->wSoundID;

	char name[20];
	sprintf(name, "S%c%c%04x.WAV",
				m_iSoundBits == 16 ? '6' : '8',
				type == SOUND_TYPE_VERBOSE ? 'V' : 'B',
				pEntry->wSoundID);

//	od("Playing sound %s\r\n", (LPCSTR)name);

#if 0
/*
// Update the status for this entry.
// If this is a context entry, it means updating the experience and
// time variables. These variables are accessed through the SoundContextData
// array, which is indexed by the sound id.
*/

	update_experience(pEntry);
#endif

/*
// Play the sound.
*/

	if (play_sound(name, 0, FALSE, m_iSoundBits == 16 ? "16" : "8") == MCIERR_WAVE_OUTPUTSUNSUITABLE)
	{
		if (m_iSoundBits == 16)
		{
		/*
		// There is no device for us to play the 16 bit version on.
		// Switch to the 8-bit samples.
		*/

			m_iSoundBits = 8;
			name[1] = '8';

			if (play_sound(name, 0, FALSE, "8") == MCIERR_WAVE_OUTPUTSUNSUITABLE)
			{
			/*
			// There are no devices available for us to use at all.
			// Shut down.
			*/
				close();
				return;
			}
		}
		else
		{
		/*
		// We failed on the 8-bit samples.
		// Shut-down.
		*/
			close();
			return;
		}
	}
}

/*
// Play a sound by ID.
*/

DWORD SoundManager::play_sound(PCSTR name, UINT wFlags /*=0*/, BOOL normal_path /*=FALSE*/, PCSTR pPathSuffix /*=NULL*/, BOOL fPromptForCD /*=TRUE*/)
{
	if (!m_bOpen)
	{
		return MCIERR_DEVICE_OPEN;
	}

	if (pPathSuffix != NULL
			&& strcmp(pPathSuffix, "16") == 0
			&& m_iSoundBits == 8)
	{
	/* We're in 8-bit mode! */
		return MCIERR_WAVE_OUTPUTSUNSUITABLE;
	}

	DWORD ret = MCIERR_FILE_NOT_FOUND;

/* Construct the name of the sound file from the name and the path. */

	char buffer[_MAX_PATH];
	char path_buffer[_MAX_PATH];

	strcpy(path_buffer, m_sound_path);
	if (pPathSuffix != NULL)
	{
		remove_slash(path_buffer);
		strcat(path_buffer, pPathSuffix);
		append_slash(path_buffer);
	}

	PCSTR n;

	if (m_path_manager == NULL)
	{
		if (normal_path)
		{
			n = name;
		}
		else
		{
			build_path_name(buffer, path_buffer, name);
			n = buffer;
		}
	}
	else
	{
		if (normal_path)
		{
			n = m_path_manager->make_full_path(name, buffer, TRUE);
		}
		else
		{
			build_path_name(buffer, path_buffer, name);
			n = m_path_manager->make_full_path(buffer, NULL, TRUE);
		}
	}

	SoundContext* pContext = current_sound_context();
	
	WORD* pID;
	
	if (pContext != NULL)
	{
		pID = &(pContext->m_wDeviceID);
	}
	else
	{
		pID = &m_wNonContextDeviceId;
	}

/* If it exists... */

	unsigned uRet;

//	od("Play sound %s...\r\n", (LPCSTR)n);

	if ((uRet = access_file(n, 4)) == 0)		// Can access in read mode?
	{
	/* ... play it. */
		ret = m_mciwnd.play_sound((LPSTR)n, wFlags, pID);
//		od("play_sound returned %d\r\n", ret);
	}
	else
	{
//		od("access_file() failed with %u\r\n", uRet);
	/* See if we failed because of a binding failure (path not found or ???). */

		if (uRet != 2)				/* 2 == File not found. */
		{
		/* Don't try and talk anymore. But also, don't remember this... */
			m_state = SMS_QUIET;
		}
	}

/* Make sure the context is correct. */

	if (pContext != NULL)
	{
		update_context_buttons(pContext);
	}

	return ret;
}

DWORD SoundManager::play_midi(PCSTR name, UINT wFlags)
{
/* Construct the name of the sound file from the name and the path. */

	char buffer[_MAX_PATH];

	if (m_path_manager == NULL)
	{
		strcpy(buffer, name);
	}
	else
	{
		m_path_manager->make_full_path((LPCSTR)m_sound_path, buffer);
		strcat(buffer, name);
	}

	SoundContext* pContext = current_sound_context();
	
	if (pContext != NULL)
	{
	/* If it exists... */
	
		if (access_file(buffer, 4) == 0)		// Can access in read mode?
		{
		/* ... play it. */
			m_mciwnd.play_midi(buffer, wFlags, &pContext->m_wDeviceID);
		}
	
	/* Make sure the context is correct. */
	
		update_context_buttons(pContext);
	}

	return MCIERR_FILE_NOT_FOUND;
}

/*
// Play an AVI file.
*/

DWORD SoundManager::play_avi(PCSTR name, HWND hWndDisplay, UINT wFlags)
{
	return m_mciwnd.play_avi((LPSTR)name, hWndDisplay, wFlags, NULL);
}

/*
// Stop an AVI file.
*/

VOID SoundManager::stop_avi(UINT wFlags)
{
	if (m_bOpen)
	{
		m_mciwnd.stop_avi(wFlags);
	}
}

/*
// Stop a sound.
*/

VOID SoundManager::stop_sound(UINT wFlags)
{
	if (m_bOpen)
	{
		m_mciwnd.stop_sound(wFlags);
		update_context_buttons(current_sound_context());
	}
}

VOID SoundManager::stop_current_sound()
{
	if (m_bOpen)
	{
	/* Show the stop button as on briefly. */
		SoundContext* pContext = current_sound_context();
		if (pContext != NULL && pContext->m_pStopButton != NULL)
		{
			pContext->m_pStopButton->TurnButtonOn();
		}

		stop_sound();
	}
}

SoundContext* SoundManager::HookWindow(CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/, BOOL fDialog /*=TRUE*/, BOOL fExpand /*=TRUE*/)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return NULL;
	}

	SoundContext* pContext = NULL;

	if (fDialog || (pContext = m_pSidebarContext) == NULL)
	{
	/*
	// Create the new context.
	*/
		pContext = new SoundContext;
		m_Context.AddHead(pContext);

		if (!fDialog)
		{
			m_pSidebarContext = pContext;
		}
	}
	else
	{
		pContext->deinitialize();
	}

	pContext->initialize(pWnd, wContextID, fDialog, fExpand, m_fMonochrome, pState);

	if (fDialog
		 || m_current_sound_context == NULL
		 || m_pSidebarContext == m_current_sound_context)
	{
	/*
	// Activate the window to make sure the sound buttons are shown properly.
	*/

		ActivateWindow(wContextID);

	/*
	// This is now the context. Play its sound.
	*/

		play_context_sound(wContextID);

	/*
	// Make doubly sure that the context buttons are correct since the buttons
	// are being added now.
	*/

		update_context_buttons(current_sound_context());
	}

	return pContext;
}

void SoundManager::UnhookWindow(CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

/* Turn off the sound. */

	stop_sound();

	SoundContext* pContext = FindSoundContext(wContextID);

//	ASSERT(pContext != NULL);

	if (pContext != NULL)
	{
		ASSERT(pContext != m_pSidebarContext);
		
		if (pContext == m_current_sound_context)
		{
			m_current_sound_context = NULL;
		}
		
		m_Context.RemoveAt(m_Context.Find(pContext));

		pContext->deinitialize();
		delete pContext;
	}
}

void SoundManager::ActivateWindow(UINT wContextID)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

	m_current_sound_context = FindSoundContext(wContextID);
	
	update_context_buttons(current_sound_context());
}

void SoundManager::DeactivateWindow(UINT wContextID)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

/* Turn off the sound. */

	SoundContext* pContext = FindSoundContext(wContextID);
	
	if (m_current_sound_context == pContext)
	{
		stop_sound();
		m_current_sound_context = NULL;
	}
	
	disable_context_buttons(pContext);
}

#if 0
/*
// The application is entering a dialog.
// Dialogs always establish the current context. The context's sound will be
// played.
*/

VOID SoundManager::entering_dialog(CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/, BOOL fExpand /*=TRUE*/)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

//	od("Entering dialog: %d\r\n", wContextID);

	if (m_dialog_index == MAX_SOUND_DIALOGS-1)
	{
		od("(PMW Error) Sound manager dialog list full.\r\n");
		return;
	}

	if (m_dialog_index > MAX_SOUND_DIALOGS/2)
	{
		od("(PMW Warning) Large number of dialogs (%d)\r\n", m_dialog_index);
	}
	
/*
// Disable the buttons of the previous context.
*/

	disable_context_buttons(current_sound_context());
	
/*
// Stack the new context.
*/
	
	m_dialog_index++;
	m_dialog_context[m_dialog_index].initialize(pWnd, wContextID, TRUE, fExpand, m_fMonochrome, pState);

/*
// Dialog boxes always establish the context.
// This is now the context. Play its sound.
*/

	play_context_sound(wContextID);

/*
// Make doubly sure that the context buttons are correct since the buttons
// are being added now.
*/

	update_context_buttons(current_sound_context());
}

/*
// A dialog box is being taken down.
// We need to update our internal state to return to the previous context.
*/

VOID SoundManager::leaving_dialog(CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

//	od("Leaving dialog: %d\r\n", wContextID);

/* Turn off the sound. */

	stop_sound();

/* See if we can remove us from the list. */

	if (m_dialog_index < 0 || wContextID != m_dialog_context[m_dialog_index].m_wContextID)
	{
		od("Enter/leave mismatch (index %d)\r\n", m_dialog_index);
		return;
	}

/* Remove this dialog from the context stack. */

	m_dialog_context[m_dialog_index].deinitialize();
	m_dialog_index--;

	update_context_buttons(current_sound_context());
}

/*
// A sidebar is being shown on the screen (at least internally).
// A sidebar establishes the context in the absence of any dialogs on the
// screen.
*/

VOID SoundManager::showing_sidebar(CWnd *pWnd, UINT wContextID)
{
/* If the sound manager is not open, skip all this. */
	if (!m_bOpen)
	{
		return;
	}

//	od("Showing sidebar: %d\r\n", wContextID);

	if (m_sidebar_context.GetContextID() != 0)
	{
		m_sidebar_context.deinitialize();
	}
	
/* Remember this as the sidebar context. */

	m_sidebar_context.initialize(pWnd, wContextID, FALSE, FALSE, m_fMonochrome);

	if (m_dialog_index < 0)
	{
	/*
	// Make sure we know what our current context.
	*/

		set_current_sound_context();

	/*
	// There is no dialog on the screen.
	// This is now the context. Play its sound.
	*/

		play_context_sound(wContextID);

	/*
	// Make doubly sure that the context buttons are correct since the buttons
	// are being added now.
	*/

		update_context_buttons(current_sound_context());
	}
}
#endif

/*
// A window has been resized, we need to reposition the sound buttons.
*/

VOID SoundManager::resize_dialog(CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/)
{
/* If the sound manager is not open, skip all this. */
	if (m_bOpen)
	{
		SoundContext* pContext = FindSoundContext(wContextID);
		
		if (pContext != NULL)
		{
			pContext->PositionButtons(pWnd, m_fMonochrome, pState);
		}
	}
}

/*
// Get the rectangle defined by the sound buttons. This rectangle includes
// the SOUND_BUTTON_OFFSET padding.
*/

void SoundManager::GetSoundButtonRectangle(CRect* pRect, CWnd *pWnd, UINT wContextID, CDialogManagerState* pState /*=NULL*/)
{
	pRect->SetRectEmpty();
	
/* If the sound manager is not open, skip all this. */
	if (m_bOpen)
	{
		SoundContext* pContext = FindSoundContext(wContextID);
		
		if (pContext != NULL)
		{
			pContext->GetButtonRectangle(pRect, pWnd, pState);
		}
	}
}

void CPmwStopButton::UserTurnedButtonOn()
{
	sound_manager.stop_sound();
}

void CPmwStopButton::UserTurnedButtonOff()
{
}

void CPmwPlayButton::UserTurnedButtonOn()
{
	sound_manager.play_current_sound();
}

void CPmwPlayButton::UserTurnedButtonOff()
{
	sound_manager.stop_sound();
}

void CPmwExplainButton::UserTurnedButtonOn()
{
	sound_manager.sound_inspector();
}

void CPmwExplainButton::UserTurnedButtonOff()
{
}

/*
// The message hook function for trapping sound manager special keys.
*/

LRESULT CALLBACK AFX_EXPORT SoundManager::message_filter(int iCode, WPARAM wParam, LPARAM lParam)
{
	if (iCode >= 0)
	{
		WORD wFlags = HIWORD(lParam);

//		od("Trapped code %d (%x, %lx)\r\n", iCode, wParam, lParam);

		if (!(wFlags & (KF_UP | KF_REPEAT)) && GetKeyState(VK_CONTROL) < 0)
		{
		/* A control key is down. */

			switch (wParam)
			{
				case 'Q':					// Quiet.
				{
//					if (iCode == HC_ACTION)
					{
						sound_manager.stop_current_sound();
					}
					return 1;				// Discard message.
				}
				case 'R':					// Repeat
				{
//					if (iCode == HC_ACTION)
					{
						sound_manager.play_current_sound();
					}
					return 1;
				}
				case 'E':					// Explain
				{
//					if (iCode == HC_ACTION)
					{
						if (sound_manager.m_bSoundInspector)
						{
							AfxGetApp()->m_pMainWnd->PostMessage(WM_EXITHELPMODE, 0, 0);
						}
						else
						{
							sound_manager.sound_inspector();
						}
					}
					return 1;
				}
				default:
				{
				/* Just fall out. */
					break;
				}
			}
		}
	}

/* Pass it on. */

	return ::CallNextHookEx(sound_manager.m_hhook, iCode, wParam, lParam);
}

/*
// Return the current sound context.
// If a dialog is on the screen, its context is current.
// Otherwise, the sidebar context is current.
*/

SoundContext* SoundManager::current_sound_context(VOID)
{
	return m_current_sound_context;
}

/////////////////////////////////////////////////////////////////////////////
// SoundContext

/*
// Return the context id for this sound context.
*/

WORD SoundContext::GetContextID(void) const
{
	return m_pState == NULL ? m_wContextID : (WORD)m_pState->m_lHelpContext;
}

/*
// Initialize a SoundContext.
//
//		1. Save the context ID.
//		2. Create the sound control buttons.
*/

VOID SoundContext::initialize(CWnd *pWnd, UINT wContextID, BOOL fDialog, BOOL fExpand, BOOL fMonochrome, CDialogManagerState* pState /*=NULL*/)
{
	BOOL fDoExpand = (pState == NULL || pState->m_fExpandWindow);
	m_wContextID = wContextID;
	m_fDialog = fDialog;
	m_pSpeakerButton = NULL;
	m_pStopButton = NULL;
	m_pPlayButton = NULL;
	m_pExplainButton = NULL;
	m_wDeviceID = 0;
	m_pState = pState;

	if ((pWnd != NULL) && (m_wContextID != 0))
	{
		if (fExpand)
		{
		/* This is a dialog. See if we really want to expand or not. */
			if (fDoExpand)
			{
				RECT rcClient, rcControl;

				pWnd->GetClientRect(&rcClient);
				pWnd->ClientToScreen(&rcClient);

			/*
			// See if we need to expand the window to make room for our controls.
			// First, compute the total area occupied by the buttons.
			*/

				rcControl.bottom = rcClient.bottom;
				rcControl.top = rcControl.bottom - (SOUND_BUTTON_OFFSET+SOUND_BUTTON_SIZE+SOUND_BUTTON_OFFSET);
				rcControl.right = rcClient.right;
				rcControl.left = rcControl.right - (SOUND_BUTTON_OFFSET+4*SOUND_BUTTON_SIZE-2+SOUND_BUTTON_OFFSET);

			/*
			// Next, enumerate the child windows to see if the buttons will fit
			// where we want them to. We compute the overlap of any controls and
			// the button area.
			*/
				CRect rcOverlap;
				rcOverlap.SetRectEmpty();

				for (CWnd *pWndChild = pWnd->GetWindow(GW_CHILD);
									pWndChild != NULL;
									pWndChild = pWndChild->GetWindow(GW_HWNDNEXT))
				{
					CRect rcChild;

					pWndChild->GetWindowRect(&rcChild);
	//				od("Compare (%d: %d, %d, %d, %d) to (%d, %d, %d, %d)\r\n",
	//										pWndChild->GetDlgCtrlID(), (RECT)rcChild, (RECT)rcControl);

					CRect rcIntersect;
					if (rcIntersect.IntersectRect(&rcChild, &rcControl))
					{
						rcOverlap.UnionRect(&rcOverlap, &rcIntersect);
					}
				}
							
			/* Did we accumulate any overlap? */

				if (!rcOverlap.IsRectEmpty())
				{
	//				od("Overlap is (%d, %d, %d, %d)\r\n", (RECT)rcOverlap);
				/*
				// Overlap is not empty. Expand the window to account for the
				// overlapped area.
				*/
					RECT rcWindow;
					pWnd->GetWindowRect(&rcWindow);

					/*
					// The control areas overlap.
					// Just expand the window and be done.
					*/
						// expand dialog so sound controls are sure to fit in the lower right corner
					pWnd->SetWindowPos(
							NULL,
							0,
							0,
							(rcWindow.right-rcWindow.left),
							(rcWindow.bottom-rcWindow.top)+(rcOverlap.bottom-rcOverlap.top),
							SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
				}
			}
		}
		
		PositionButtons(pWnd, fMonochrome, pState);
	}
}

VOID SoundContext::PositionButtons(CWnd *pWnd, BOOL fMonochrome, CDialogManagerState* pState /*=NULL*/)
{
	// Get the rectangle to position the buttons in.
	
	CRect crPosition;
	pWnd->GetClientRect(&crPosition);
	
	// For dialogs, position the sound buttons in the lower right corner.
	// For other windows (the sidebar), center the sound buttons.
	
	crPosition.bottom -= SOUND_BUTTON_OFFSET;
	crPosition.top = crPosition.bottom - SOUND_BUTTON_SIZE;
	
	if (m_fDialog)
	{
		crPosition.right -= SOUND_BUTTON_OFFSET;
	}
	else
	{
		crPosition.right -= (crPosition.Width()-(4*SOUND_BUTTON_SIZE-2))/2;
	}
	
	crPosition.left = crPosition.right-SOUND_BUTTON_SIZE;
	
	if (m_pExplainButton == NULL)
	{
		if ((m_pExplainButton = new CPmwExplainButton) != NULL)
		{
			if (!m_pExplainButton->CreateButton(
						pWnd,
						crPosition,
						MAKEINTRESOURCE(IDB_SOUNDEXPLAIN),
						ID_SOUND_EXPLAIN,
						0,
						fMonochrome,
						pState == NULL ? NULL : pState->m_hWndInspectSound))
			{
				delete m_pExplainButton;
				m_pExplainButton = NULL;
			}
		}
	}
	else
	{
		m_pExplainButton->MoveWindow(crPosition, TRUE);
	}
	
	if (m_pExplainButton != NULL)
	{
		m_pExplainButton->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
	
	crPosition.right = crPosition.left+1;	
	crPosition.left = crPosition.right-SOUND_BUTTON_SIZE;
	
	if (m_pPlayButton == NULL)
	{
		if ((m_pPlayButton = new CPmwPlayButton) != NULL)
		{
			if (!m_pPlayButton->CreateButton(
						pWnd,
						crPosition,
						MAKEINTRESOURCE(IDB_SOUNDPLAY),
						ID_SOUND_PLAY,
						0,
						fMonochrome,
						pState == NULL ? NULL : pState->m_hWndPlaySound))
			{
				delete m_pPlayButton;
				m_pPlayButton = NULL;
			}
		}
	}
	else
	{
		m_pPlayButton->MoveWindow(crPosition, TRUE);
	}
		
	if (m_pPlayButton != NULL)
	{
		m_pPlayButton->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
		
	crPosition.right = crPosition.left+1;	
	crPosition.left = crPosition.right-SOUND_BUTTON_SIZE;
	
	if (m_pStopButton == NULL)
	{
		if ((m_pStopButton = new CPmwStopButton) != NULL)
		{
			if (!m_pStopButton->CreateButton(
						pWnd,
						crPosition,
						MAKEINTRESOURCE(IDB_SOUNDSTOP),
						ID_SOUND_STOP,
						0,
						fMonochrome,
						pState == NULL ? NULL : pState->m_hWndStopSound))
			{
				delete m_pStopButton;
				m_pStopButton = NULL;
			}
		}
	}
	else
	{
		m_pStopButton->MoveWindow(crPosition, TRUE);
	}
	
	if (m_pStopButton != NULL)
	{
		m_pStopButton->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
		
	crPosition.right = crPosition.left;	
	crPosition.left = crPosition.right-SOUND_BUTTON_SIZE;
	
	if (m_pSpeakerButton == NULL)
	{	
		if ((m_pSpeakerButton = new CPmwSoundButton) != NULL)
		{
			if (!m_pSpeakerButton->CreateButton(
						pWnd,
						crPosition,
						MAKEINTRESOURCE(IDB_SOUNDSPEAKER),
						ID_SOUND_SPEAKER,
						0,
						fMonochrome,
						pState == NULL ? NULL : pState->m_hWndSpeakerIcon))
			{
				delete m_pSpeakerButton;
				m_pSpeakerButton = NULL;
			}
		}
	}
	else
	{
		m_pSpeakerButton->MoveWindow(crPosition, TRUE);
	}
	
	if (m_pSpeakerButton != NULL)
	{
		m_pSpeakerButton->SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}
		
}

/*
// Get the rectangle defined by the sound buttons. This rectangle includes
// the SOUND_BUTTON_OFFSET padding.
*/

void SoundContext::GetButtonRectangle(CRect* pRect, CWnd* pWnd, CDialogManagerState* pState /*=NULL*/)
{
	pRect->SetRectEmpty();
	
	CRect crButton;
	
	if (m_pSpeakerButton != NULL)
	{
		m_pSpeakerButton->GetWindowRect(crButton);
		pWnd->ScreenToClient(crButton);
		*pRect |= crButton;
	}

	if (m_pStopButton != NULL)
	{
		m_pStopButton->GetWindowRect(crButton);
		pWnd->ScreenToClient(crButton);
		*pRect |= crButton;
	}

	if (m_pPlayButton != NULL)
	{
		m_pPlayButton->GetWindowRect(crButton);
		pWnd->ScreenToClient(crButton);
		*pRect |= crButton;
	}

	if (m_pExplainButton != NULL)
	{
		m_pExplainButton->GetWindowRect(crButton);
		pWnd->ScreenToClient(crButton);
		*pRect |= crButton;
	}
	
	if (!pRect->IsRectEmpty())
	{
		pRect->InflateRect(SOUND_BUTTON_OFFSET, SOUND_BUTTON_OFFSET);
	}
}

/*
// Deinitialize a SoundContext.
//
//		1. Zero out the context ID.
//		2. Destroy the sound control buttons.
*/

VOID SoundContext::deinitialize(void)
{
	m_wContextID = 0;

	if (m_pSpeakerButton != NULL)
	{
		delete m_pSpeakerButton;
		m_pSpeakerButton = NULL;
	}

	if (m_pStopButton != NULL)
	{
		delete m_pStopButton;
		m_pStopButton = NULL;
	}

	if (m_pPlayButton != NULL)
	{
		delete m_pPlayButton;
		m_pPlayButton = NULL;
	}

	if (m_pExplainButton != NULL)
	{
		delete m_pExplainButton;
		m_pExplainButton = NULL;
	}
}

// CWindowArray
//
// This is a helper class used to position the sound buttons. It's simply
// an array of pointers to info structures for each window.
//
// FOR THE INSTALL VERSION, THIS IS A DUMMY CLASS!

CWindowArray::CWindowArray()
{
}

CWindowArray::~CWindowArray()
{
}

void CWindowArray::RemoveAll(void)
{
}

void CWindowArray::RemoveAt(int nIndex, int nCount /*=1*/)
{
}
	
void CWindowArray::AddWindow(HWND hWnd)
{
}
		
void CWindowArray::GetUnionRect(CRect& crUnion)
{
}

BOOL CWindowArray::Intersect(const CRect* pcrRect, CRect* pcrIntersection /*=NULL*/)
{
	return FALSE;
}
	
BOOL CWindowArray::IntersectUnion(const CRect* pcrRect, CRect* pcrIntersection /*=NULL*/)
{
	return FALSE;
}

int CWindowArray::WindowGap(int n1, int n2, int nMax /*=0*/)
{
	return 0;
}
	
BOOL CWindowArray::Adjust(int nYNeeded)
{
	return TRUE;
}

BOOL CWindowArray::Move(void)
{
	return FALSE;
}

void CWindowArray::Restore(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// The global sound manager.

SoundManager /*near*/ sound_manager;

#endif					// #ifdef DOSOUNDS

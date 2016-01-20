/*
// $Header: /PM8/App/MCIWND.CPP 1     3/03/99 6:08p Gbeddow $
//
// MCI control window implementation.
//
// $Log: /PM8/App/MCIWND.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:10   Fred
// Initial revision.
// 
//    Rev 1.2   29 May 1997 15:25:52   johno
// Control of interruption of currently playing sound
// 
//    Rev 1.1   24 May 1996 16:11:50   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:32:54   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:51:54   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:29:26   FRED
// Initial revision.
// 
//    Rev 2.9   29 Jan 1996 09:14:48   JAY
// Fixed a bug with CloseQueuedSounds() and notifications.
// 
//    Rev 2.8   26 Jan 1996 15:09:56   JAY
//  
// 
//    Rev 2.7   23 Jan 1996 12:32:38   JAY
//  
// 
//    Rev 2.6   15 Jan 1996 10:26:20   JAY
//  
// 
//    Rev 2.5   17 Oct 1995 16:56:48   JAY
// Now handles all notify messages before closing.
// 
//    Rev 2.4   13 Oct 1995 14:20:02   FRED
// 32 bit changes
// 
//    Rev 2.3   03 Sep 1995 15:54:48   FRED
//  
// 
//    Rev 2.2   27 Aug 1995 18:28:56   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.1   08 Feb 1995 13:37:46   JAY
// Reverted. New series.
// 
//    Rev 1.10   30 Nov 1994 17:02:24   JAY
//  
// 
//    Rev 1.9   02 Sep 1994 10:11:38   JAY
// New sound mechanism
// 
//    Rev 1.8   10 Aug 1994 15:05:18   JAY
// ...
// 
//    Rev 1.7   04 Aug 1994 08:30:46   JAY
// Large model and 32-bit changes
// 
//    Rev 1.6   19 Jul 1994 09:05:36   JAY
// Attempt to eliminate sound device left on on Fred's machine. It failed.
// 
//    Rev 1.5   11 Jul 1994 09:05:58   JAY
// Handles final device closings correctly now.
// 
//    Rev 1.4   17 Jun 1994 09:30:02   JAY
// Added bye() method to MCIWnd to allow the window to be destroyed before the
// destructor is called.
// 
//    Rev 1.3   16 May 1994 08:47:16   JAY
// Added support for "windowed elements" like AviVideo (.AVI) files.
// 
//    Rev 1.2   03 May 1994 16:20:52   JAY
// Removed some debugging messages.
// 
//    Rev 1.1   29 Apr 1994 17:13:00   JAY
// Added an external callback mechanism.
// 
//    Rev 1.0   27 Apr 1994 14:41:12   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "system.h"

#include "mciwnd.h"
#include "digitalv.h"			// Digital video include from VFWDK.

#ifdef _MAC
// TODO:MAC -- this is only here temporarily to compile and link
// this file should be removed from the MAC build entirely
#undef mciSendCommand
#define mciSendCommand(a,b,c,d)   0
#endif // _MAC

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/*
// The basic idea behind all of this:
//
// - Each sound file ("element") gets its own sound device.
// - Any number of devices may (in theory) be open at once.
// - Only one device is currently playing, since only one device CAN be
//   playing at a time.
// - Devices are closed via the MCI_NOTIFY callback. The only times a sound
//   is closed outside of that are (1) if the MCI_PLAY fails, in which case
//   no notify WILL be sent; or (2) in the destructor of the window, in which
//   case, no notify CAN be sent.
// - The ONLY function which we use MCI_NOTIFY for is MCI_PLAY. Getting the
//   notification causes the closing of the device.
// - Stopping a sound will cause its notify to be sent, and the sound device
//   will be closed.
//
// It seems that sounds do not begin playing until PMW yields back to Windows.
// This may be a quirk of my sound card, but it is worth noting. If you need
// to poll status of the sound file (say, to watch its progress), be sure and
// call Yield() or PeekMessage() or somehow return to your message loop;
// otherwise, the sound will never start, and you will simply loop forever
// waiting for it.
*/

/////////////////////////////////////////////////////////////////////////////
// CMciWnd

/*
// The constructor for the sound window.
*/

CMciWnd::CMciWnd()
{
//od("Construct MCI\n\r");
	TRACE0("Construct MCI...\n");
	memset(m_playing_device, 0, sizeof(m_playing_device));
	m_notify_callback = NULL;
}

/*
// The destructor for the sound window.
*/

CMciWnd::~CMciWnd()
{
//od("Destruct MCI\n\r");
	close_all(MCI_WAIT);
}

/*
// Free any pending element for this device type.
*/

void CMciWnd::free_pending(int type)
{
	m_pending[type].m_csName.Empty();
	m_pending[type].m_wWaiting = 0;

	LPWORD lpwDevice = m_pending[type].m_lpwDevice;
	m_pending[type].m_lpwDevice = NULL;

	if (lpwDevice != NULL && *lpwDevice == -1)
	{
		*lpwDevice = 0;
	}
}

/*
// Set the parameters for a pending call.
*/

void CMciWnd::set_pending(int type, LPCSTR lpName, WORD wWaiting, WORD wFlags, LPWORD lpwDevice)
{
//	od("MCI set_pending (%s)\n\r", lpName);
	free_pending(type);

	m_pending[type].m_csName = lpName;
	m_pending[type].m_wWaiting = wWaiting;			// Wait for this guy.
	m_pending[type].m_wFlags = wFlags;
	m_pending[type].m_lpwDevice = lpwDevice;

	if (lpwDevice != NULL && *lpwDevice == 0)
	{
		*lpwDevice = (WORD)-1;
	}
}

BOOL CMciWnd::is_pending(int type, LPWORD lpwDevice)
{
	return m_pending[type].m_lpwDevice == lpwDevice;
}

/*
// Close all devices.
*/

void CMciWnd::close_all(WORD wFlags)
{
	CloseQueuedSounds();

//od("MCI close_all\n\r");

	for (int i = 0; i < MCIWND_devcount; i++)
	{
	/* Make sure no pending element will kick in. */
		free_pending(i);
		m_pending[i].m_csName.Empty();
		close_element((MCIWND_DEVTYPE)i, m_playing_device[i], wFlags);
	}
}

/*
// Initialize a sound window.
*/

BOOL CMciWnd::init(MCI_NOTIFY_CALLBACK notify_callback, LPVOID notify_data)
{
//od("MCI init\n\r");
	RECT r = { 0, 0, 0, 0};
	if (!Create(NULL, NULL, 0, r, GetDesktopWindow(), 0, NULL))
	{
		return FALSE;
	}

/* Establish our callback. */

	m_notify_callback = notify_callback;
	m_notify_data = notify_data;

	return TRUE;
}

void CMciWnd::bye(void)
{
//od("MCI bye\n\r");
	close_all(MCI_WAIT);

	if (m_hWnd != NULL)
	{
		MSG msg;
		while (::PeekMessage(&msg, m_hWnd, MM_MCINOTIFY, MM_MCINOTIFY, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				::PostQuitMessage(msg.wParam);
				break;
			}
			if (msg.message == MM_MCINOTIFY)			/* Let's be really sure... */
			{
				OnMciNotify(msg.wParam, msg.lParam);
			}
		}
	}

	m_notify_callback = NULL;
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CMciWnd, CWnd)
	//{{AFX_MSG_MAP(CMciWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(MM_MCINOTIFY, OnMciNotify)
	ON_MESSAGE(WM_START_SOUND, OnStartSound)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMciWnd message handlers

LRESULT CMciWnd::OnMciNotify(WPARAM wParam, LPARAM lParam)
{
	UINT wDeviceID = LOWORD(lParam);

//	od("MCI notify - device %d\n\r", wDeviceID);

//	TRACE2("MCI notification %d for device %d\n", wParam, wDeviceID);

//	stop_element(MCIWND_devnone, wDeviceID, MCI_WAIT);

/* The device is finished. Close the element for it. */

	close_element(MCIWND_devnone, wDeviceID);

	BOOL fDoCallback = TRUE;

/*
// If there is an element pending for this type, start it now.
*/

	for (int i = 0; i < MCIWND_devcount; i++)
	{
		if (m_pending[i].m_wWaiting == wDeviceID
				&& !m_pending[i].m_csName.IsEmpty())
		{
		/* We were waiting for this guy! */

			LPWORD lpwDevice = m_pending[i].m_lpwDevice;

			fDoCallback = FALSE;

			if (lpwDevice == NULL || *lpwDevice != wDeviceID)
			{
			/* Do the notify now, before the new sound is started. */
				if (m_notify_callback != NULL)
				{
					m_notify_callback(wDeviceID, m_notify_data);
				}
			}
			else
			{
			/*
			// The pending sound will use the same context as the terminating
			// one. Do NOT do the callback so that the play button stays down.
			*/
			}

			do_play_element((MCIWND_DEVTYPE)i,
								 m_pending[i].m_csName,
								 m_pending[i].m_wFlags,
								 m_pending[i].m_lpwDevice);

			free_pending(i);
		}
	}

/* Tell any interested party that the sound is over. */

	if (fDoCallback && m_notify_callback != NULL)
	{
//("MCI notify - callback\n\r");
		m_notify_callback(wDeviceID, m_notify_data);
	}

	return 0;
}

LRESULT CMciWnd::OnStartSound(WPARAM wParam, LPARAM lParam)
{
	CSoundQElement* pElement = (CSoundQElement*)lParam;

	if (start_element(pElement->m_type,
							pElement->m_wDeviceID,
							pElement->m_wFlags) != 0)
	{
		/* Failed! Close the sound. */
		close_element(pElement->m_type, pElement->m_wDeviceID);
	}

	delete pElement;

	return 0;
}

/***************************/
/* Device-level operations */
/***************************/

/*
// Return the name for this device type.
*/

LPSTR CMciWnd::device_name(MCIWND_DEVTYPE type)
{
	switch (type)
	{
		case MCIWND_waveaudio:
		{
			return "waveaudio";
		}
		case MCIWND_sequencer:
		{
			return "sequencer";
		}
		case MCIWND_avivideo:
		{
			return "avivideo";
		}
		default:
		{
			return NULL;
		}
	}
}

/*
// Send a command to the MCI.
*/

DWORD CMciWnd::mci_command(UINT wDeviceID, UINT wCommand, UINT wFlags, LPVOID pParams)
{
//	TRACE3("MCI command - wDeviceID %d wCommand %d wFlags %d\n", wDeviceID, wCommand, wFlags);
	if (wDeviceID == 0)
	{
	/* Nothing to do. */
		return 0;
	}

/* Operate on the channel. */

	DWORD code;

	if ((code = mciSendCommand(wDeviceID,
										wCommand,
										wFlags,
										(DWORD)pParams)) != 0)
	{
		od("Got error %lx for operation %x on device %d\r\n", code, wCommand, wDeviceID);
	}
	return code;
}

/*****************************/
/* Playing device management */
/*****************************/

/*
// Get the device to use.
// A return of 0 means no device has been specified.
*/

UINT CMciWnd::device_to_use(MCIWND_DEVTYPE type, UINT wDeviceID)
{
//od("MCI device_to_use - type %d wDeviceID %d --> ", type, wDeviceID);
/* Validate/find the device to close. */

	if (wDeviceID == 0 && type != MCIWND_devnone)
	{
	/* Use the default for the device type. */
		wDeviceID = m_playing_device[type];
	}

//od("%d\n\r", wDeviceID);
	return wDeviceID;
}

/*
// Reset the playing device if necessay.
*/

VOID CMciWnd::reset_playing_device(MCIWND_DEVTYPE type, UINT wDeviceID)
{
//od("MCI reset_playing_device - type %d wDeviceID %d\n\r", type, wDeviceID);
	if (type == MCIWND_devnone)
	{
	/*
	// We don't know the type of device. Look through the list.
	*/
		for (int i = 0; i < MCIWND_devcount; i++)
		{
			if (m_playing_device[i] == wDeviceID)
			{
//				TRACE0("Zero it #1...\n");
				m_playing_device[i] = 0;
				break;
			}
		}
	}
	else
	{
	/*
	// We know the type of device. Check for a simple match.
	*/
		if (wDeviceID == m_playing_device[type])
		{
//			TRACE0("Zero it #2...\n");
			m_playing_device[type] = 0;
		}
	}
}

/****************************/
/* Element level operations */
/****************************/

/*
// Internal routine for sending a command to an element.
// The command is assumed to take no parameters.
//
// If the device id is 0, the currently playing device for the type is used.
// If the type passed is -1, all device types are searched to see if the
// device is the currently playing one.
*/

DWORD CMciWnd::element_command(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wCommand, UINT wFlags, LPVOID pParams)
{
//	od("MCI element_command - type %d wDeviceID %d wCommand %d wFlags %d\n\r", type, wDeviceID, wCommand, wFlags);
/*
// Get the device ID to use.
*/

	if ((wDeviceID = device_to_use(type, wDeviceID)) == 0)
	{
	/* Nothing to do. */
		return 0;
	}

/*
// We have the device. Operate on it.
*/

//	od("Operation %x on Channel %d\r\n", wCommand, wDeviceID);

	DWORD code = mci_command(wDeviceID, wCommand, wFlags, pParams);

/*
// Reset the playing device for the appropriate type.
*/

	reset_playing_device(type, wDeviceID);

	return code;
}

/*
// Close an element.
//
// If the device id is 0, the currently playing device for the type is used.
// If the type passed is -1, all device types are searched to see if the
// device is the currently playing one.
*/

DWORD CMciWnd::close_element(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wFlags)
{
//	od("MCI close_element - type %d wDeviceID %d wFlags %d\n\r", type, wDeviceID, wFlags);
	return element_command(type, wDeviceID, MCI_CLOSE, wFlags);
}

void CMciWnd::CloseQueuedSounds(void)
{
	if (m_hWnd != NULL)
	{
		MSG Msg;
		while (::PeekMessage(&Msg, m_hWnd, WM_START_SOUND, WM_START_SOUND, PM_REMOVE))
		{
			if (Msg.message == WM_QUIT)
			{
				::PostQuitMessage(Msg.wParam);
				break;
			}
		/* The element for this is open. Close it. */
			if (Msg.message == WM_START_SOUND)		/* Let's be really sure... */
			{
				CSoundQElement* pElement = (CSoundQElement*)Msg.lParam;
//				close_element(pElement->m_type, pElement->m_wDeviceID);
				OnMciNotify(MCI_NOTIFY_ABORTED, (LPARAM)pElement->m_wDeviceID);

				delete pElement;
			}
		}
	}
}

/*
// Stop an element.
//
// If the device id is 0, the currently playing device for the type is used.
// If the type passed is -1, all device types are searched to see if the
// device is the currently playing one.
*/

DWORD CMciWnd::stop_element(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wFlags)
{
//	od("MCI stop_element - type %d wDeviceID %d wFlags %d\n\r", type, wDeviceID, wFlags);
	if (type == MCIWND_waveaudio /*&& wDeviceID != 0*/)
	{
		CloseQueuedSounds();
	}
	return element_command(type, wDeviceID, MCI_STOP, wFlags);
}

/*
// Start an element.
*/

DWORD CMciWnd::start_element(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wFlags)
{
//	od("MCI start_element - type %d wDeviceID %d wFlags %d\n\r", type, wDeviceID, wFlags);
/* Setup the play parms. */

	MCI_PLAY_PARMS mpp;

	mpp.dwCallback = (DWORD)(UINT)GetSafeHwnd();

/* Attempt to start up the playback. */

//	od("Play channel %d on device %s\r\n", wDeviceID, device_name(type));

	DWORD code;

	if ((code = mci_command(wDeviceID,
										MCI_PLAY,
										(UINT)(MCI_NOTIFY | wFlags),
										&mpp)) == 0)
	{
	/* This device is now playing on this driver. */
//		TRACE1("Set it #1 (%d)...\n", wDeviceID);
		m_playing_device[type] = wDeviceID;
	}

	return code;
}

/*
// Actually play an element.
// This assumes that the element is not already playing something else.
*/

DWORD CMciWnd::do_play_element(MCIWND_DEVTYPE type, LPCSTR lpstrName, UINT wFlags, LPWORD lpwDevice)
{
//	od("MCI do_play_element - type %d lpstrName %s wFlags %d\n\r", type, lpstrName, wFlags);
	MCI_OPEN_PARMS mop;
	DWORD code;

	mop.lpstrDeviceType = device_name(type);
	mop.lpstrElementName = (LPSTR)lpstrName;

	if ((code = mciSendCommand(0, MCI_OPEN,
										MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
										(DWORD)(LPVOID)&mop)) != 0)
	{
		od("Got error code %lx\r\n", code);
		return code;
	}

/* Try to start the element playing! */

	if (type == MCIWND_waveaudio)
	{
	/* Delay until later. */
		CSoundQElement* pElement = new CSoundQElement;
		pElement->m_type = type;
		pElement->m_wDeviceID = mop.wDeviceID;
		pElement->m_wFlags = wFlags;

		PostMessage(WM_START_SOUND, 0, (LPARAM)pElement);
	}
	else if ((code = start_element(type, mop.wDeviceID, wFlags)) != 0)
	{
	/* Failed! Close the sound and return. */
		close_element(type, mop.wDeviceID);
		return code;
	}

/* Return the device ID if anybody wants it. */

	if (lpwDevice != NULL)
	{
		*lpwDevice = mop.wDeviceID;
	}

	return 0;
}

/*
// Play a device element.
*/

DWORD CMciWnd::play_element(MCIWND_DEVTYPE type, LPSTR lpstrName, UINT wFlags, LPWORD lpwDevice, BOOL LongSound /*= TRUE*/)
{
//	od("MCI play_element - type %d lpstrName %s wFlags %d\n\r", type, lpstrName, wFlags);
/*
// Get rid of any last sound.
// We can't play more than one sound at the same time.
*/

	WORD wDeviceID;

/*
// Sounds which are waiting to start will not send NOTIFYs back.
// And we need to handle them specially.
*/

	if (type == MCIWND_waveaudio)
	{
		CloseQueuedSounds();
	}

	if ((wDeviceID = device_to_use(type, 0)) != 0)
	{
	/* Something is playing. */
/*
      The Naked Truth Table 
      for Interrupting the Playing Sound

      m_LongSound    LongSound   Interrupt?

      FALSE          TRUE        No
      FALSE          FALSE       Yes
      TRUE           FALSE       Yes
      TRUE           TRUE        Yes

      In other words, long sounds don't interrupt short sounds
      Only another short sound interrupts a short sound

*/
		if ((m_LongSound == TRUE) || (LongSound == FALSE))
         stop_element(type, wDeviceID, MCI_WAIT);

	/* Wait until it's done. Queue up the sound for later. */

		set_pending(type, lpstrName, wDeviceID, wFlags, lpwDevice);

		return 0;
	}
	else if (!m_pending[type].m_csName.IsEmpty())
	{
	/* A sound is already pending. Replace it (wait for the same one). */
		set_pending(type, lpstrName, m_pending[type].m_wWaiting, wFlags, lpwDevice);
		return 0;
	}

/* Nothing playing. Play the sound now. */

	return do_play_element(type, lpstrName, wFlags, lpwDevice);
}

/*
// Play a windowed element. This guy takes a window to play into.
*/

DWORD CMciWnd::play_windowed_element(MCIWND_DEVTYPE type, LPSTR lpstrName, HWND hWndDisplay, UINT wFlags, LPWORD lpwDevice)
{
/*
// Get rid of any last sound.
// We can't play more than one element at the same time.
*/

	stop_element(type, 0, MCI_WAIT);

	MCI_OPEN_PARMS mop;
	DWORD code;

/* Open the channel. */

	mop.lpstrDeviceType = device_name(type);
	mop.lpstrElementName = lpstrName;

	if ((code = mciSendCommand(0, MCI_OPEN,
										MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
										(DWORD)(LPVOID)&mop)) != 0)
	{
		od("Got error code %lx on open\r\n", code);
		return code;
	}

/*
// Set the window.
*/

	MCI_DGV_WINDOW_PARMS mwp;

	mwp.hWnd = hWndDisplay;
	mwp.nCmdShow = SW_SHOW;

	if ((code = mciSendCommand(mop.wDeviceID, MCI_WINDOW,
										MCI_DGV_WINDOW_HWND | MCI_DGV_WINDOW_STATE,
										(DWORD)(LPVOID)&mwp)) != 0)
	{
		od("Got error code %lx on window call\r\n", code);
		close_element(type, mop.wDeviceID);
		return code;
	}

/* Try to start the element playing! */

	if ((code = start_element(type, mop.wDeviceID, wFlags)) != 0)
	{
	/* Failed! Close the sound and return. */
		close_element(type, mop.wDeviceID);
		return code;
	}

/* Return the device ID if anybody wants it. */

	if (lpwDevice != NULL)
	{
		*lpwDevice = mop.wDeviceID;
	}

	return 0;
}

/************************/
/* High-level commands. */
/************************/

/*
// Play a sound.
*/

DWORD CMciWnd::play_sound(LPSTR lpstrName, UINT wFlags, LPWORD lpwDevice, BOOL LongSound /*= TRUE*/)
{
   DWORD d = play_element(MCIWND_waveaudio, lpstrName, wFlags, lpwDevice);
   
   if (d != 0)
      m_LongSound = TRUE;
	else
      m_LongSound = LongSound;
   
   return d;
}

/*
// Stop a sound.
*/

DWORD CMciWnd::stop_sound(UINT wFlags, WORD wDeviceID)
{
	return stop_element(MCIWND_waveaudio, wDeviceID, wFlags);
}

/*
// Play a MIDI file.
*/

DWORD CMciWnd::play_midi(LPSTR lpstrName, UINT wFlags, LPWORD lpwDevice)
{
	return play_element(MCIWND_sequencer, lpstrName, wFlags, lpwDevice);
}

/*
// Stop a MIDI file playback
*/

DWORD CMciWnd::stop_midi(UINT wFlags, WORD wDeviceID)
{
	return stop_element(MCIWND_sequencer, wDeviceID, wFlags);
}

/*
// Play an AVI file.
*/

DWORD CMciWnd::play_avi(LPSTR lpstrName, HWND hWndDisplay, UINT wFlags, LPWORD lpwDevice)
{
	return play_windowed_element(MCIWND_avivideo, lpstrName, hWndDisplay, wFlags, lpwDevice);
}

/*
// Stop an AVI file playback
*/

DWORD CMciWnd::stop_avi(UINT wFlags, WORD wDeviceID)
{
	return stop_element(MCIWND_avivideo, wDeviceID, wFlags);
}

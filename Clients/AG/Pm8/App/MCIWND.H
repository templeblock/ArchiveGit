/*
// $Header: /PM8/App/MCIWND.H 1     3/03/99 6:08p Gbeddow $
//
// MCI control window definitions.
//
// $Log: /PM8/App/MCIWND.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:36   Fred
// Initial revision.
// 
//    Rev 1.1   29 May 1997 15:26:36   johno
// Control of interruption of currently playing sound
// 
// 
//    Rev 1.0   14 Mar 1996 13:28:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:50   FRED
// Initial revision.
// 
//    Rev 2.3   23 Jan 1996 12:32:16   JAY
//  
// 
//    Rev 2.2   03 Sep 1995 15:54:46   FRED
//  
// 
//    Rev 2.1   08 Feb 1995 13:39:46   JAY
// Reverted. New series.
// 
//    Rev 1.7   02 Sep 1994 10:11:36   JAY
// New sound mechanism
// 
//    Rev 1.6   10 Aug 1994 15:05:18   JAY
// ...
// 
//    Rev 1.5   11 Jul 1994 09:06:24   JAY
// Got rid of unused flag.
// 
//    Rev 1.4   11 Jul 1994 09:05:58   JAY
// Handles final device closings correctly now.
// 
//    Rev 1.3   17 Jun 1994 09:29:36   JAY
// Added bye() method to MCIWnd to allow the window to be destroyed before the
// destructor is called.
// 
//    Rev 1.2   16 May 1994 08:46:52   JAY
// Added support for "windowed elements" like AviVideo (.AVI) files.
// 
//    Rev 1.1   29 Apr 1994 17:12:58   JAY
// Added an external callback mechanism.
// 
//    Rev 1.0   27 Apr 1994 14:41:34   JAY
// Initial revision.
*/

#ifndef __MCIWND_H__
#define __MCIWND_H__

#include <mmsystem.h>

#define MCI_DEV_COUNT	(MCI_DEVTYPE_LAST - MCI_DEVTYPE_FIRST + 1)

/////////////////////////////////////////////////////////////////////////////
// CMciWnd window

typedef void (*MCI_NOTIFY_CALLBACK)(WORD wDevice, LPVOID pData);

class CMciWnd : public CWnd
{
// Attributes
public:

	enum MCIWND_DEVTYPE
	{
		MCIWND_devnone		= -1,
		MCIWND_waveaudio	= 0,
		MCIWND_sequencer	= 1,
		MCIWND_avivideo	= 2,
		MCIWND_devcount
	};

private:

	UINT m_playing_device[MCIWND_devcount];
	struct CPending
	{
		CString m_csName;
		WORD m_wWaiting;
		WORD m_wFlags;
		LPWORD m_lpwDevice;
		CPending() : m_wWaiting(0), m_wFlags(0), m_lpwDevice(0)
			{}
	} m_pending[MCIWND_devcount];

// Operations
private:
	DWORD do_play_element(MCIWND_DEVTYPE type, LPCSTR lpstrName, UINT wFlags, LPWORD lpwDevice);
public:

// Implementation
protected:

	MCI_NOTIFY_CALLBACK m_notify_callback;
	LPVOID				  m_notify_data;

	LPSTR device_name(MCIWND_DEVTYPE type);
	DWORD mci_command(UINT wDeviceID, UINT wCommand, UINT wFlags = 0, LPVOID pParams = NULL);

	UINT device_to_use(MCIWND_DEVTYPE type, UINT wDeviceID);
	VOID reset_playing_device(MCIWND_DEVTYPE type, UINT wDeviceID);

	DWORD element_command(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wCommand, UINT wFlags = 0, LPVOID pParams = NULL);
	DWORD start_element(MCIWND_DEVTYPE type, UINT wDeviceID, UINT wFlags = 0);
	DWORD close_element(MCIWND_DEVTYPE type, UINT wDeviceID = 0, UINT wFlags = 0);
	DWORD stop_element(MCIWND_DEVTYPE type, UINT wDeviceID = 0, UINT wFlags = 0);

	DWORD play_element(MCIWND_DEVTYPE type, LPSTR lpstrName, UINT wFlags = 0, LPWORD lpwDevice = NULL, BOOL LongSound = TRUE);
	DWORD play_windowed_element(MCIWND_DEVTYPE type, LPSTR lpstrName, HWND hWndDisplay, UINT wFlags = 0, LPWORD lpwDevice = NULL);

	void set_pending(int type, LPCSTR lpName, WORD wWaiting, WORD wFlags, LPWORD lpwDevice);
	void free_pending(int type);
	
public:
	BOOL is_pending(int type, LPWORD lpwDevice);

public:
// Construction
	CMciWnd();

// Destruction
	virtual ~CMciWnd();

/*
// Call this before using! And check the return value!
*/

	BOOL init(MCI_NOTIFY_CALLBACK notify_callback = NULL, LPVOID notify_data = NULL);

/*
// Call this before deleting.
*/

	void bye(void);

/*
// Useful APIs.
*/

	DWORD play_sound(LPSTR lpstrName, UINT wFlags = 0, LPWORD lpwDevice = NULL, BOOL LongSound = TRUE);
	DWORD stop_sound(UINT wFlags = 0, WORD wDeviceID = 0);
	DWORD play_midi(LPSTR lpstrName, UINT wFlags = 0, LPWORD lpwDevice = NULL);
	DWORD stop_midi(UINT wFlags = 0, WORD wDeviceID = 0);
	DWORD play_avi(LPSTR lpstrName, HWND hWndDisplay, UINT wFlags = 0, LPWORD lpwDevice = NULL);
	DWORD stop_avi(UINT wFlags = 0, WORD wDeviceID = 0);

	void close_all(WORD wFlags);
////#ifdef WIN32
	void CloseQueuedSounds(void);
////#endif

protected:
	// Generated message map functions
	//{{AFX_MSG(CMciWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg LRESULT OnMciNotify(WPARAM wParam, LPARAM lParam);
////#ifdef WIN32
	afx_msg LRESULT OnStartSound(WPARAM wParam, LPARAM lParam);
////#endif
	DECLARE_MESSAGE_MAP()

   BOOL  m_LongSound;
};

////#ifdef WIN32
class CSoundQElement
{
public:
	CMciWnd::MCIWND_DEVTYPE m_type;
	WORD m_wDeviceID;
	WORD m_wFlags;
};

#define WM_START_SOUND	(WM_USER+4321)

////#endif

#endif					// #ifndef __MCIWND_H__

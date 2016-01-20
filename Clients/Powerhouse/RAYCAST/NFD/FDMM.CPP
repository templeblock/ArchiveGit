/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fdmm.cpp
//

#include "stdafx.h"
#include <mmsystem.h>
#include "fd.h"

#include "fddoc.h"
#include "fdview.h"
#include "fdmm.h"
#include "voldlg.h"

#define	SOUNDTOTAL	64
#define	MUSICTOTAL	32

CScrollBar		*volSoundFX, *volMusic;

char					far *midiOut;
char					far *waveOut[SOUNDTOTAL];
int						midiHoldOff;
int						midiNumDevs;
int						waveNumDevs;
int						MusicActive=1, MusicWanted=0, MusicSelection=0;
int						SoundActive=1, SoundWanted=0;
UINT					wTimerID=0;
DWORD					midiVolume, waveVolume;
HGLOBAL				hgmidiOut, hgmhOut;
HGLOBAL				hgwaveOut[SOUNDTOTAL], hgwhOut[SOUNDTOTAL];
HMMIO					hmfr;
HMIDIOUT			hmOut=NULL;
HWAVEOUT			hwOut=NULL;
MMCKINFO			mmParentInfo, mmInfo;
MIDIHDR				far *mhOut=NULL;
PCMWAVEFORMAT	wfOut;
WAVEHDR				far *whOut[SOUNDTOTAL];
TIMECAPS			tCaps;
MIDIOUTCAPS		moCaps;
WAVEOUTCAPS		woCaps;

HINSTANCE			FdDLL=NULL;

static union
{
	void (CALLBACK *lpfnMidiOutputHandler)(UINT,UINT,DWORD,DWORD,DWORD);
	void (CALLBACK *lpfnHandlerOutputMidi)(void);
};

static union
{ 
	void (CALLBACK *lpfnMidiInitialize)(MIDIHDR far *,char far*,long);
	void (CALLBACK *lpfnInitializeMidi)(void);
};

static union
{ 
	void (CALLBACK *lpfnMidiEnable)(BOOL,HMIDIOUT);
	void (CALLBACK *lpfnEnableMidi)(void);
};

char	*SndFileNames[SOUNDTOTAL]={
									"wave\\booup.wav",
									"wave\\machine.wav",
									"wave\\noise.wav",
									"wave\\thump.wav",
									"wave\\grunt1.wav",
									"wave\\ping1.wav",
									"wave\\pistol1.wav",
									"wave\\shotgun1.wav",
									"wave\\chime1.wav",
									"wave\\missle.wav",
									"wave\\gunfire.wav",
									"wave\\explode.wav",
									"wave\\cuckoo.wav",
									"wave\\boodown.wav"
									};

char	*MidFileNames[MUSICTOTAL]={
									"midi\\bachto.mid",
									"midi\\bakson51.mid",
									"midi\\bakson52.mid",
									"midi\\bakson53.mid",
									"midi\\pfv.mid",
									0,0,0,0,0,0,0,0,0,0,0,
									0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
									};

extern CFdView		*OurView;

//		NOTE:	Multi-chunk wave files canNOT be played!
//				All but the first chunk is ignored, at this time.

//	Reset sound resources
//

void SoundReset(void)
{
int			index;
UINT		mmstat;
DWORD		volume;

	if (SoundActive)
	{
//		shut down the wave device
		mmstat = waveOutReset(hwOut);
		TRACE("waveOutReset returned %d\n", mmstat);

		waveOutGetVolume(0, &volume);
		AfxGetApp()->WriteProfileInt("Preferences", "Sound/FXVolume", LOWORD(volume));
		waveOutSetVolume(0,waveVolume);

		for (index=0 ; index < SOUNDTOTAL ; index++)
		{
			if (hgwaveOut[index] != NULL)
			{
				if (whOut[index]->dwFlags & WHDR_PREPARED)
				{
					mmstat = waveOutUnprepareHeader(hwOut, whOut[index], sizeof(WAVEHDR));
					TRACE("waveOutUnprepareHeader returned %d\n", mmstat);
				}
				GlobalUnlock(hgwhOut[index]);
				GlobalUnlock(hgwaveOut[index]);
				GlobalFree(hgwhOut[index]);
				GlobalFree(hgwaveOut[index]);
			}
		}
		mmstat = waveOutClose(hwOut);
		TRACE("waveOutClose returned %d\n", mmstat);
	}
}

//	Reset music resources
//

void MusicReset(void)
{
UINT		mmstat;
DWORD		volume;

	if (MusicActive)
	{
		if (mhOut == NULL) return;
//	shut down the midi device
		mmstat = midiOutReset(hmOut);
		TRACE("midiOutReset returned %d\n", mmstat);

		midiOutGetVolume(0, &volume);
		AfxGetApp()->WriteProfileInt("Preferences", "MusicVolume", LOWORD(volume));
		midiOutSetVolume(0,midiVolume);

		mmstat = midiOutClose(hmOut);
		TRACE("midiOutClose returned %d\n", mmstat);

		GlobalPageUnlock(hgmhOut);
		GlobalPageUnlock(hgmidiOut);

		GlobalUnlock(hgmhOut);
		GlobalUnlock(hgmidiOut);

		GlobalFree(hgmhOut);
		GlobalFree(hgmidiOut);

		mhOut = NULL;

		if (wTimerID) timeKillEvent(wTimerID);
		if (FdDLL) FreeLibrary(FdDLL);
	}
}

void	loadMidiFile(char *musicFile)
{
char		ftype[5]={0,0,0,0,0};
char		mmtext[100], mmerr[88];
UINT		mmstat;
UINT		volume;
long		mmfnum;

	midiHoldOff = 36;	//	wait 2 seconds before starting music
	MusicReset();		//	make sure memory is free if we are re-loading

  //	Load the MIDI library DLL
	FdDLL = LoadLibrary("fddll\\fddll.dll");
	if (FdDLL < HINSTANCE_ERROR)
	{
		AfxMessageBox("Failed to load MIDI DLL", MB_OK|MB_ICONSTOP);
		MusicActive = 0;
		return;
	}
	else
	{
		TRACE("MIDI DLL loaded\n");

    lpfnHandlerOutputMidi = GetProcAddress(FdDLL, "MidiOutputHandler");
    ASSERT(lpfnHandlerOutputMidi != NULL);

    lpfnInitializeMidi = GetProcAddress(FdDLL, "MidiInitialize");
    ASSERT(lpfnInitializeMidi != NULL);

    lpfnEnableMidi = GetProcAddress(FdDLL, "MidiEnable");
    ASSERT(lpfnEnableMidi != NULL);

		//	Set initial state of MusicWanted flag in DLL
		(*lpfnMidiEnable)(MusicWanted, NULL);
	}

	volume = AfxGetApp()->GetProfileInt("Preferences", "MusicVolume", 0x7fff);
	midiOutSetVolume(0,((DWORD)volume << 16) | (DWORD)volume);

	//	initialize some structures
	mmParentInfo.ckid = (FOURCC) 0;
	mmParentInfo.cksize = 0;
	mmParentInfo.fccType = (FOURCC) 0;
	mmParentInfo.dwDataOffset = 0;
	mmParentInfo.dwFlags = 0;
	mmInfo = mmParentInfo;

//	open the midi file with the music
	hmfr = mmioOpen(musicFile, NULL, MMIO_READ);
	if (hmfr == NULL)
	{
		AfxMessageBox("MIDI music file missing", MB_OK|MB_ICONSTOP);
		MusicActive = 0;
		return;
	}
	mmfnum = mmioRead(hmfr, (char huge*)&ftype, 4);
	mmfnum = mmioSeek(hmfr, 0, SEEK_SET);

	if (lstrcmp(ftype,"RIFF") == 0)	//	.RMI file
	{

//		descend to the RIFF
		mmstat = mmioDescend(hmfr, &mmParentInfo, NULL, MMIO_FINDRIFF);

//		descend to the data
		mmstat = mmioDescend(hmfr, &mmInfo, &mmParentInfo, MMIO_FINDCHUNK);
		if (mmInfo.cksize > 65535)
		{
			AfxMessageBox("MIDI data are limited to 65535 bytes", MB_OK|MB_ICONSTOP);
			MusicActive = 0;
			return;
		}
		TRACE("Offset to data in the .RMI file is %ld\n", mmInfo.dwDataOffset);

//		allocate space for the data
		hgmidiOut = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, mmInfo.cksize);
		GlobalPageLock(hgmidiOut);
		midiOut = (char far *)GlobalLock(hgmidiOut);

//		allocate space for the MIDIHDR
		hgmhOut = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(MIDIHDR));
		GlobalPageLock(hgmhOut);
		mhOut = (MIDIHDR far *)GlobalLock(hgmhOut);

//		init the MIDIHDR
		mhOut->lpData = midiOut;
		mhOut->dwBufferLength = mmInfo.cksize;
		mhOut->dwBytesRecorded = 0;
		mhOut->dwUser = 0;
		mhOut->dwFlags = 0;
		mhOut->lpNext = NULL;
		mhOut->reserved = 0;

//		locate and read the data of the music
		mmfnum = mmioSeek(hmfr, mmInfo.dwDataOffset, SEEK_SET);
		mmfnum = mmioRead(hmfr, midiOut, mmInfo.cksize);

//		close the midi file
		mmstat = mmioClose(hmfr, NULL);
	}
	else
	if (lstrcmp(ftype,"MThd") == 0)
	{
		mmfnum = mmioSeek(hmfr, 0, SEEK_END);
		if (mmfnum > 65535)
		{
			AfxMessageBox("MIDI data are limited to 65535 bytes", MB_OK|MB_ICONSTOP);
			MusicActive = 0;
			return;
		}
		mmioSeek(hmfr, 0, SEEK_SET);

//		allocate space for the data
		hgmidiOut = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, mmfnum);
		GlobalPageLock(hgmidiOut);
		midiOut = (char far *)GlobalLock(hgmidiOut);

//		allocate space for the MIDIHDR
		hgmhOut = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(MIDIHDR));
		GlobalPageLock(hgmhOut);
		mhOut = (MIDIHDR far *)GlobalLock(hgmhOut);

//		init the MIDIHDR
		mhOut->lpData = midiOut;
		mhOut->dwBufferLength = mmfnum;
		mhOut->dwBytesRecorded = 0;
		mhOut->dwUser = 0;
		mhOut->dwFlags = 0;
		mhOut->lpNext = NULL;
		mhOut->reserved = 0;

		mmfnum = mmioRead(hmfr, midiOut, mmfnum);

//		close the midi file
		mmstat = mmioClose(hmfr, NULL);
	}
	else
	{
		AfxMessageBox("Music file is not MID or RMI", MB_OK|MB_ICONSTOP);
		MusicActive = 0;
		return;
	}
		
//	open the midi output device
	if ((mmstat = midiOutOpen(&hmOut, 0, MAKELONG(OurView->GetSafeHwnd(),0), NULL, CALLBACK_WINDOW)) != 0)
	{
		midiOutGetErrorText(mmstat, mmerr, sizeof(mmerr));
		lstrcpy(mmtext, "MIDI Error: ");
		lstrcat(mmtext, mmerr);
		AfxMessageBox(mmtext, MB_OK|MB_ICONSTOP);
		MusicActive = 0;
		MusicReset();
		return;
	}

	(*lpfnMidiInitialize)(mhOut,midiOut,mmfnum);

	wTimerID = timeSetEvent(25, 5, lpfnMidiOutputHandler, MAKELONG(hmOut,0), TIME_PERIODIC);
	ASSERT(wTimerID != NULL);
	TRACE("timeSetEvent established for 50 ms\n");

}

//	Initialize music and sound resources
//

void	SoundInit(void)
{
char		*sndFile;
char		mmtext[100], mmerr[88];
int			index;
UINT		mmstat, volume;
long		mmfnum;
CString		tmp;

	midiNumDevs = midiOutGetNumDevs();
	TRACE("Number of MIDI output devices present = %d\n", midiNumDevs);
	if (!midiNumDevs) MusicActive = 0;

	if (MusicActive)
	{
	tmp = AfxGetApp()->GetProfileString("Preferences", "Music", "Yes");
	if (!tmp.CompareNoCase("Yes")) MusicWanted = 1;
	do
	{
//		get device caps, for info only (at this time)
		mmstat = timeGetDevCaps(&tCaps, sizeof(TIMECAPS));
		TRACE("timeGetDevCaps returned %d\n", mmstat);
		TRACE("  Minimum Period = %d\n", tCaps.wPeriodMin);
		TRACE("  Maximum Period = %d\n", tCaps.wPeriodMax);

//		get device caps, for info only (at this time)
		mmstat = midiOutGetDevCaps(0, &moCaps, sizeof(MIDIOUTCAPS));
		TRACE("midiOutGetDevCaps returned %d\n", mmstat);
		TRACE("  Manufacturer ID = %d\n",moCaps.wMid);
		TRACE("  Product ID = %d\n",moCaps.wPid);
		TRACE("  Driver Version = %d.%d\n",moCaps.vDriverVersion >> 8,moCaps.vDriverVersion & 0xff);
		TRACE("  Product Name = %s\n",moCaps.szPname);
		TRACE("  Technology = %04x\n",moCaps.wTechnology);
		TRACE("  Voices = %d\n",moCaps.wVoices);
		TRACE("  Notes = %d\n",moCaps.wNotes);
		TRACE("  Channel mask = %04x\n",moCaps.wChannelMask);
		TRACE("  Support = %08lx\n",moCaps.dwSupport);

		midiOutGetVolume(0,&midiVolume);

		MusicSelection = AfxGetApp()->GetProfileInt("Preferences", "MusicSelection", 0);
		if (MusicSelection < 0 || MusicSelection > MUSICTOTAL) MusicSelection = 0;
		if (MidFileNames[MusicSelection] == 0) MusicSelection = 0;

		loadMidiFile(MidFileNames[MusicSelection]);
 	}
	while (0);
	}

	waveNumDevs = waveOutGetNumDevs();
	TRACE("Number of WAVE output devices present = %d\n", waveNumDevs);
	if (!waveNumDevs) SoundActive = 0;

	if (SoundActive)
	{
	tmp = AfxGetApp()->GetProfileString("Preferences", "Sound/FX", "Yes");
	if (!tmp.CompareNoCase("Yes")) SoundWanted = 1;
	do
	{
//		get device caps, for info only (at this time)
		mmstat = waveOutGetDevCaps(0, &woCaps, sizeof(WAVEOUTCAPS));
		TRACE("waveOutGetDevCaps returned %d\n", mmstat);
		TRACE("  Manufacturer ID = %d\n",woCaps.wMid);
		TRACE("  Product ID = %d\n",woCaps.wPid);
		TRACE("  Driver Version = %d.%d\n",woCaps.vDriverVersion >> 8,woCaps.vDriverVersion & 0xff);
		TRACE("  Product Name = %s\n",woCaps.szPname);
		TRACE("  Formats = %08lx\n",woCaps.dwFormats);
		TRACE("  Channels = %d\n",woCaps.wChannels);
		TRACE("  Support = %08lx\n",woCaps.dwSupport);

		waveOutGetVolume(0,&waveVolume);

		mmParentInfo.ckid = (FOURCC) 0;
		mmParentInfo.cksize = 0;
		mmParentInfo.fccType = (FOURCC) 0;
		mmParentInfo.dwDataOffset = 0;
		mmParentInfo.dwFlags = 0;
		mmInfo = mmParentInfo;
		for (index=0 ; index < SOUNDTOTAL ; index++)
		{
//			open the wave file with the sound effect
			sndFile = SndFileNames[index];
			if ((sndFile == NULL) ||
				((hmfr = mmioOpen(sndFile, NULL, MMIO_READ)) == NULL))
			{
				hgwaveOut[index] = NULL;
				if (sndFile != NULL)
					TRACE("Unable to open sound file %s\n", sndFile);
				continue;
			}
			TRACE("Sound file %d is %s\n",index, sndFile);
//			descend, first to the RIFF
			mmstat = mmioDescend(hmfr, &mmParentInfo, NULL, MMIO_FINDRIFF);
//			descend to the WAVEfmt
			mmInfo.ckid = mmioFOURCC('W','A','V','E');
			mmstat = mmioDescend(hmfr, &mmInfo, &mmParentInfo, NULL);	//	WAVEfmt
			TRACE("Offset to WAVEfmt in .WAV file is %ld\n", mmInfo.dwDataOffset);
//			use the returned offset to access the WAVEFMT structure
			mmfnum = mmioSeek(hmfr, mmInfo.dwDataOffset, SEEK_SET);
//			read the WAVEFMT from the wave file
			mmfnum = mmioRead(hmfr, (char huge *)&wfOut, sizeof(PCMWAVEFORMAT));
			if (hwOut == NULL)
			{
//				open the wave output device
				if ((mmstat = waveOutOpen(&hwOut, 0, (WAVEFORMAT *)&wfOut, NULL, NULL, WAVE_ALLOWSYNC)) != 0)
				{
					waveOutGetErrorText(mmstat, mmerr, sizeof(mmerr));
					lstrcpy(mmtext, "WAVE Error: ");
					lstrcat(mmtext, mmerr);
					AfxMessageBox(mmtext, MB_OK|MB_ICONSTOP);
					SoundActive = 0;
//					close the wave file
					mmstat = mmioClose(hmfr, NULL);
					SoundReset();
					break;
				}
				volume = AfxGetApp()->GetProfileInt("Preferences", "Sound/FXVolume", 0x7fff);
				waveOutSetVolume(0,((DWORD)volume << 16) | (DWORD)volume);
			}
//			ascend back up one level
			mmstat = mmioAscend(hmfr, &mmInfo, NULL);
//			then descend to the data for the wave file
			mmInfo.ckid = mmioFOURCC('d','a','t','a');
			mmstat = mmioDescend(hmfr, &mmInfo, &mmParentInfo, NULL);	//	data
			TRACE("Offset to data in .WAV file is %ld\n", mmInfo.dwDataOffset);

//			allocate space for the wave data
			hgwaveOut[index] = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, mmInfo.cksize);
			if (hgwaveOut[index] == NULL)
			{
				AfxMessageBox("WAVE Error: Insufficient memory to load sound effects", MB_OK|MB_ICONSTOP);
				SoundActive = 0;
//				close the wave file
				mmstat = mmioClose(hmfr, NULL);
				break;
			}
			waveOut[index] = (char far *)GlobalLock(hgwaveOut[index]);

//			locate and read the wave data of the sound effect
			mmfnum = mmioSeek(hmfr, mmInfo.dwDataOffset, SEEK_SET);
			mmfnum = mmioRead(hmfr, waveOut[index], mmInfo.cksize);
//			close the wave file
			mmstat = mmioClose(hmfr, NULL);

//			allocate space for the WAVEHDR
			hgwhOut[index] = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(WAVEHDR));
			whOut[index] = (WAVEHDR far *)GlobalLock(hgwhOut[index]);

//			init the WAVEHDR
			whOut[index]->lpData = waveOut[index];
			whOut[index]->dwBufferLength = mmInfo.cksize;
			whOut[index]->dwBytesRecorded = 0;
			whOut[index]->dwUser = 0;
			whOut[index]->dwFlags = WHDR_DONE;
			whOut[index]->dwLoops = 0;
			whOut[index]->lpNext = NULL;
			mmstat = waveOutPrepareHeader(hwOut, whOut[index], sizeof(WAVEHDR));
		}
	}
	while (0);
	}
	return;
}

//	Play sound
//

void wavePlay(int SndNumber, int Prio)
{
UINT		mmstat;
static int	LastPrio;
static int	LastSnd;
	while (SoundActive && SoundWanted)
	{
		if (Prio < LastPrio)
			if (!(whOut[LastSnd]->dwFlags & WHDR_DONE)) break;
		LastPrio = Prio;
		if (!(whOut[LastSnd]->dwFlags & WHDR_DONE))
			mmstat = waveOutReset(hwOut);
		if (hgwaveOut[SndNumber] == NULL) break;
		whOut[SndNumber]->dwFlags &= ~WHDR_DONE;
		mmstat = waveOutWrite(hwOut, whOut[SndNumber], sizeof(WAVEHDR));
		LastSnd = SndNumber;
		break;
	}
}

//	Play midi
//

void midiPlay(int Selection)
{
	while (MusicActive && MusicWanted)
	{
		loadMidiFile(MidFileNames[Selection]);
		break;
	}
}

void MidiEnable(BOOL Active)
{
	(*lpfnMidiEnable)(Active,hmOut);
}

/////////////////////////////////////////////////////////////////////////////
// CVolDlg dialog

CVolDlg::CVolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CVolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVolDlg, CDialog)
	//{{AFX_MSG_MAP(CVolDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolDlg message handlers

BOOL CVolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
UINT	val;
DWORD	dval;

	volSoundFX = (CScrollBar*)GetDlgItem(IDC_SOUNDFX_VOLUME);
	if (SoundActive)
	{
		waveOutGetVolume(0,&dval);
		val = (UINT)((long)(UINT)LOWORD(dval) * 64L / 65536L);
		volSoundFX->SetScrollRange(0,64,FALSE);
		volSoundFX->SetScrollPos(val,TRUE);
	}
	else
		volSoundFX->EnableScrollBar(ESB_DISABLE_BOTH);


	volMusic = (CScrollBar*)GetDlgItem(IDC_MUSIC_VOLUME);
	if (MusicActive)
	{
		midiOutGetVolume(0,&dval);
		val = (UINT)((long)(UINT)LOWORD(dval) * 64L / 65536L);
		volMusic->SetScrollRange(0,64,FALSE);
		volMusic->SetScrollPos(val,TRUE);
	}
	else
		volMusic->EnableScrollBar(ESB_DISABLE_BOTH);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
int		Pos=(int)nPos;
DWORD	Vol;

	if (nSBCode != SB_THUMBPOSITION && nSBCode != SB_THUMBTRACK)
	{
		if (pScrollBar == volSoundFX)
		{
			waveOutGetVolume(0,&Vol);
		}
		else
		if (pScrollBar == volMusic)
		{
			midiOutGetVolume(0,&Vol);
		}
		Pos = (UINT)((long)(UINT)LOWORD(Vol) * 64L / 65536L);
	}
	switch (nSBCode)
	{
	case SB_LEFT:			//	Scroll to far left.
		Pos = 0;
		break;
	case SB_LINELEFT:		//	Scroll left.
		if (Pos) Pos--;
		break;
	case SB_LINERIGHT:		//	Scroll right.
		if (Pos < 64) Pos++;
		break;
	case SB_PAGELEFT:		//	Scroll one page left.
		Pos -= 8;
		if (Pos < 0) Pos = 0;
		break;
	case SB_PAGERIGHT:		//	Scroll one page right.
		Pos += 8;
		if (Pos > 64) Pos = 64;
		break;
	case SB_RIGHT:			//	Scroll to far right.
		Pos = 64;
		break;
	case SB_THUMBPOSITION:	//	Scroll to absolute position. The current position is specified by the nPos parameter.
	case SB_THUMBTRACK:		//	Drag scroll box to specified position. The current position is specified by the nPos parameter.
		break;
	}
	Vol = (DWORD)((long)Pos * 65536L / 64L);
	if (Vol > 65535) Vol = 65535;
	Vol = (Vol << 16) | Vol;
	pScrollBar->SetScrollPos(Pos,TRUE);
	if (pScrollBar == volSoundFX)
	{
		waveOutSetVolume(0,Vol);
		wavePlay(PISTOL_SHOT, 100);
		AfxGetApp()->WriteProfileInt("Preferences", "Sound/FXVolume", LOWORD(Vol));
	}
	else
	if (pScrollBar == volMusic)
	{
		midiOutSetVolume(0,Vol);
		AfxGetApp()->WriteProfileInt("Preferences", "MusicVolume", LOWORD(Vol));
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

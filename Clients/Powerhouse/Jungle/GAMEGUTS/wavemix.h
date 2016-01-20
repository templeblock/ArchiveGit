#ifndef __wavemix
#define __wavemix

#ifdef __cplusplus
extern "C" {			/* Assume C declarations for C++ */
#endif	/* __cplusplus */

#ifndef NOWMIXSTR
typedef LPVOID LPMIXWAVE;
#endif

/* flag values for play params */
#define WMIX_QUEUEWAVE 0x00
#define WMIX_CLEARQUEUE	 0x01
#define WMIX_USELRUCHANNEL 0x02
#define WMIX_HIPRIORITY 0x04
#define WMIX_WAIT 0x08
typedef struct
{
	WORD wSize;
	HANDLE hMixSession;
	int iChannel;
	LPMIXWAVE lpMixWave;
	HWND hWndNotify;
	DWORD dwFlags;
	WORD wLoops;  /* 0xFFFF means loop forever */
}
MIXPLAYPARAMS, * PMIXPLAYPARAM, FAR * LPMIXPLAYPARAMS;

typedef struct
{
	WORD wSize;
	BYTE bVersionMajor;
	BYTE bVersionMinor;
	char szDate[12]; /* Mmm dd yyyy */
	DWORD dwFormats; /* see waveOutGetDevCaps (wavemix requires synchronous device) */
}
WAVEMIXINFO, *PWAVEMIXINFO, FAR * LPWAVEMIXINFO;

#define WMIX_CONFIG_CHANNELS 0x1
#define WMIX_CONFIG_SAMPLINGRATE 0x2
typedef struct
{
	WORD wSize;
	DWORD dwFlags;
	WORD wChannels;	 /* 1 = MONO, 2 = STEREO */
	WORD wSamplingRate; /* 11,22,44	 (11=11025, 22=22050, 44=44100 Hz) */
}
MIXCONFIG, *PMIXCONFIG, FAR * LPMIXCONFIG;

WORD		WINAPI WaveMixGetInfo(LPWAVEMIXINFO lpWaveMixInfo);

/* return same errors as waveOutOpen, waveOutWrite, and waveOutClose */
HANDLE	WINAPI WaveMixInit(void); /* returns hMixSession - you should keep it and use for subsequent API calls */
HANDLE	WINAPI WaveMixConfigureInit(LPMIXCONFIG lpConfig);
UINT		WINAPI WaveMixActivate(HANDLE hMixSession, BOOL fActivate);

#define WMIX_FILE		0x0001
#define WMIX_RESOURCE	0x0002
LPMIXWAVE	WINAPI WaveMixOpenWave(HANDLE hMixSession, LPSTR szWaveFilename, HINSTANCE hInst, DWORD dwFlags);

#define WMIX_OPENSINGLE 0	/* open the single channel specified by iChannel */
#define WMIX_OPENALL 1 /* opens all the channels, iChannel ignored */
#define WMIX_OPENCOUNT 2 /* open iChannel Channels (eg. if iChannel = 4 will create channels 0-3) */
UINT		WINAPI WaveMixOpenChannel(HANDLE hMixSession, int iChannel, DWORD dwFlags);

UINT		WINAPI WaveMixPlay(LPMIXPLAYPARAMS lpMixPlayParams);

#define WMIX_ALL	 0x0001 /* stops sound on all the channels, iChannel ignored */
#define WMIX_NOREMIX 0x0002 /* prevents the currently submited blocks from being remixed to exclude new channel */
UINT		WINAPI WaveMixFlushChannel(HANDLE hMixSession, int iChannel, DWORD dwFlags);
UINT		WINAPI WaveMixCloseChannel(HANDLE hMixSession, int iChannel, DWORD dwFlags);

UINT		WINAPI WaveMixFreeWave(HANDLE HMixSession, LPMIXWAVE lpMixWave);
UINT		WINAPI WaveMixCloseSession(HANDLE hMixSession);
void		WINAPI WaveMixPump(void);

#ifdef __cplusplus
}			 /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#endif

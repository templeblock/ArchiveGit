#ifndef _SOUND_H_
#define _SOUND_H_

#include "wavemix.h"

#define MAX_MIX_CHANNELS 8

//************************************************************************
// The SOUND class definition
//************************************************************************

typedef class CSound FAR * LPSOUND;

class far CSound // : public BaseClass
{
// CONSTRUCTORS
	public:				
		CSound(BOOL bUseWaveMix = FALSE);

// DESTRUCTOR
	public:
		~CSound();

// ATTRIBUTES
	private:
		HINSTANCE	m_hLib;
		BOOL		m_bUseWaveMix;
		HGLOBAL		m_hWaveSaved;
		BOOL		m_bOff;
		BOOL		m_bLoopSaved;
		BOOL		m_bDefered;
		// stuff for wavemix.dll
		HANDLE		m_ghMixSession;
		LPMIXWAVE	m_mixWave[MAX_MIX_CHANNELS];
		int			m_iCurMixWave;
	public:

// FUNCTIONS
	public:
		BOOL IsOn( void );
		void Toggle( void );
		void SetDefered(BOOL bDefer);
		BOOL GetDefered( void );
		BOOL Open( LPSTR lpSoundLibName, BOOL fActivate = TRUE );
		void Close( void );
		BOOL StartResourceID( ITEMID idSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait = NO, HWND hWndNotify = NULL );
		BOOL StartFile (LPSTR lpSound, BOOL bLoop, int iChannel, BOOL bWait = NO, HWND hWndNotify = NULL );
		BOOL StartResource( LPSTR lpSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait = NO, HWND hWndNotify = NULL );
		void StopAndFree( void );
		void Stop( void );
		void Activate(BOOL fActivate);
		void StopChannel( int iChannel );
	private:
		int	 mixPlay( LPSTR lpSound, UINT uFlags, HINSTANCE hInstance, int iChannel, HWND hWndNotify );
		void mixStopAll( void );
};


#endif // _SOUND_H_

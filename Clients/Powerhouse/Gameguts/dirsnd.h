#ifndef _DIRSND_H_
#define _DIRSND_H_

#include "phtypes.h"

#define MAX_SOUNDS 75

//************************************************************************
// The DIRECT SOUND class definition
//************************************************************************

#include <dsound.h>

class CDirSnd // : public BaseClass
{
// CONSTRUCTORS
	public:				
		CDirSnd();

// DESTRUCTOR
	public:
		~CDirSnd();

// ATTRIBUTES
	private:
		BOOL		m_bOff;
		BOOL		m_bLoopSaved;
		BOOL		m_bDefered;

		float		m_fXRange;
		float		m_fYRange;

		static LPDIRECTSOUND	m_pDirectSound;
		static CDirSnd		   *m_pSndList[MAX_SOUNDS];
		static int				m_nSndCnt;

		LPDIRECTSOUNDBUFFER		m_pBuffer;
		LPDIRECTSOUND3DBUFFER	m_p3dBuff;

		BOOL		m_b3d;
		ITEMID		m_idSound;

	public:

// FUNCTIONS
	public:
		BOOL Init(HWND hWnd);
		void CloseAll(void);
		BOOL Open(ITEMID idSound, HINSTANCE hInstance, BOOL b3D = FALSE);
		//BOOL Open(LPSTR lpSound, HINSTANCE hInstance, BOOL b3D = FALSE);
		BOOL IsOn( void );
		void Toggle( void );
		void SetDefered(BOOL bDefer);
		BOOL GetDefered( void );
		BOOL Open( LPSTR lpSoundLibName, BOOL fActivate = TRUE );
		void Close( void );
		BOOL Play(BOOL bLooping = FALSE,  BOOL bInit3dPos = FALSE, BOOL bReset = TRUE);
		void StopAndFree( void );
		BOOL Stop( void );
		void Activate(BOOL fActivate);
		void StopChannel( int iChannel );
		BOOL SetVolume(LONG lVolume);
		BOOL Stopped(void);
		BOOL SetPosition(int x, int y, int z);
		CDirSnd *Duplicate(void);
		void SetXRange(int nXRange) {m_fXRange = (float)nXRange;}
		void SetYRange(int nYRange) {m_fYRange = (float)nYRange;}
		void ReloadAll(void);

	private:
		int	 mixPlay( LPSTR lpSound, UINT uFlags, HINSTANCE hInstance, int iChannel, HWND hWndNotify );
		void mixStopAll( void );
		BOOL LoadWaveResource( ITEMID idSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait, HWND hWndNotify );
		BOOL LoadWaveResource(LPSTR lpSound, HINSTANCE hInstance, BOOL b3D);
		BOOL ReOpen(void);

	//////////////////////////////////////////

};


#endif // _DIRSND_H_

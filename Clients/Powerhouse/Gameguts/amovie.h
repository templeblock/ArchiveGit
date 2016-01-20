#ifndef _amovie_h
#define _amovie_h

#include <objbase.h>
#include "strmif.h"
#include "controla.h"
#include "uuids.h"
#include "evcode.h"

class CAMovie;
typedef CAMovie * CAMovieP;

typedef enum
{
	Uninitialized,
	Stopped,
	Paused,
	Playing
} AMState;

class CAMovie
{
	public:
	// constructors/destructor
	CAMovie();
	~CAMovie();

	public: // Public Functions

	void Init( void );
	BOOL CreateFilterGraph( void );
	void ReleaseFilterGraph( void );
	void OnGraphNotify( void );
	BOOL Error( HRESULT hr );

	// Static CAMovie event callback function
	static void EventProc( DWORD dwEventData );

	BOOL Open( LPSTR lpFileName, HWND hWndPlay = NULL, long lStartFrame = 0 );
	BOOL AddFilter( void );
	BOOL Attach( HWND hWnd );
	BOOL Close( void );
	BOOL Stop( BOOL fWait = FALSE );
	DWORD GetPosition( void );
	DWORD GetLength( void );
	BOOL SetTimeFormat( BOOL bFrames );
	BOOL Play( long lFrom = -1, long lTo = -1, DWORD dwSpeed = 0L, BOOL bFullScreen = NO, BOOL bRepeat = FALSE );
	DWORD GetFrameRate( void );
	BOOL GetSrcRect( LPRECT lpSrcRect );
	BOOL SetDstRect( LPRECT lpSrcRect );
	BOOL Update( HDC hDC );
	BOOL Pause( void );
	BOOL Resume( void );
	BOOL SetVideoOnOff( BOOL bFlag );
	BOOL SetAudioOnOff( BOOL bFlag );
	BOOL Seek( long lFrame );
	BOOL SeekStop( long lFrame );
	BOOL StepForward( int nFrames );
	BOOL StepReverse( void );
	BOOL SetVideoSpeed( DWORD dwSpeed = 1000L );

	// Small helper functions
	BOOL CanPlay( void );
	BOOL CanStop( void );
	BOOL CanPause( void );
	BOOL IsPlaying( void );
	BOOL IsStopped( void );
	BOOL IsPaused( void );
	BOOL IsInitialized( void );

	private: // Private functions

	private: // Private variables
    AMState				m_State;
    IGraphBuilder *		m_pGraph;
    HANDLE				m_hGraphNotifyEvent;
	DWORD				m_dwFrameRate;
};

#endif

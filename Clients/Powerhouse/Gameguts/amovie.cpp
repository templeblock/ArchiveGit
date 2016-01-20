// Check all HRESULT checking

#include <windows.h>
#include "proto.h"
#include "amovie.h"

// MCI Functions not available in the ActiveMovie class
//
//BOOL MCISetVideoPalette( WORD wDeviceID, HPALETTE hPal );
//BOOL MCISetAudioStream( WORD wDeviceID, DWORD dwStream);
//BOOL MCISeekHome( WORD wDeviceID );
//BOOL MCISeekEnd( WORD wDeviceID );
//BOOL MCIGetDstRect( WORD wDeviceID, LPRECT lpRect );
//BOOL MCICloseAll( void );
//UINT MCIHandleNotify( HWND hWnd, UINT codeNotify, HMCI hDevice );
//void MCILoop( WORD wDeviceID, BOOL bOn, long lFrom = -1, long lTo = -1 );
//BOOL MCIIsLooped( WORD wDeviceID, LPLONG lpFrom = NULL, LPLONG lpTo = NULL );
//DWORD MCIGetVolume( LPSTR lpDeviceType );
//void MCISetVolume( LPSTR lpDeviceType, WORD wVolumeLeft, WORD wVolumeRight );
//void MCIError( DWORD dwResult );
//BOOL MCISetDestination( WORD wDeviceID, int x, int y );

#define DEFAULT_FRAMES_PER_SEC 15

//***********************************************************************
CAMovie::CAMovie()
//***********************************************************************
{
	Init();
}

//***********************************************************************
CAMovie::~CAMovie()
//***********************************************************************
{
	Close();
	ReleaseFilterGraph();
}

//***********************************************************************
void CAMovie::EventProc( DWORD dwEventData )
//***********************************************************************
{ // no "this" pointer because it is static; 1 for the entire class

	// The event data is the 'this' object pointer
	CAMovieP pAMovie = (CAMovieP)(LPSTR)dwEventData;
	if (pAMovie) // Dispatch the event
		pAMovie->OnGraphNotify();
}

//***********************************************************************
void CAMovie::Init( void )
//***********************************************************************
{
    m_State = Uninitialized;
    m_hGraphNotifyEvent = NULL;
    m_pGraph = NULL;
    m_dwFrameRate = DEFAULT_FRAMES_PER_SEC;
}

EXTERN_C const GUID CLSID_ColorFX =
	{ 0xe91eae00, 0x9479, 0x11d0, { 0x91, 0xe6, 0x00, 0xa0, 0x24, 0x59, 0xc4, 0x47 } };
EXTERN_C const GUID IID_IIPEffect =
	{ 0xe91eae02, 0x9479, 0x11d0, { 0x91, 0xe6, 0x00, 0xa0, 0x24, 0x59, 0xc4, 0x47 } };

//***********************************************************************
BOOL CAMovie::Open( LPSTR lpFileName, HWND hWndPlay, long lStartFrame )
//***********************************************************************
{
	if ( !lpFileName )
		return(NO);

	// In case of a re-open
	ReleaseFilterGraph();

	if ( !CreateFilterGraph() )
		return(NO);

	if ( !m_pGraph )
		return(NO);

	FNAME szExpFileName;
	if ( !GetApp()->FindContent( lpFileName, szExpFileName ) )
		return(NO);

	lpFileName = szExpFileName;

	WCHAR wPath[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, lpFileName, -1, wPath, MAX_PATH );
	HRESULT hr = m_pGraph->RenderFile(wPath, NULL);
	if ( Error(hr) )
		return(NO);

#ifdef TEST
	IFilter* pFilter = NULL;
	hr = CoCreateInstance(CLSID_ColorFX,
						NULL,
						CLSCTX_INPROC_SERVER,
						IID_IIPEffect,
						(void **) &pFilter);
	if (SUCCEEDED(hr))
		hr = m_pGraph->AddFilter(pFilter, L"Image Effects");
#endif TEST

	// Store the frame rate as soon as possible
	m_dwFrameRate = GetFrameRate();

	// Enter the paused state (see if this helps smooth out the first play)
	Pause();

	// Seek to the start position before it draws into the window
	Seek( lStartFrame );
	m_State = Stopped;

	Attach( hWndPlay );
	AddFilter();

	return(YES);
}

//***********************************************************************
BOOL CAMovie::AddFilter( void )
//***********************************************************************
{
	if ( !m_pGraph )
		return(NO);

	// Obtain the IEnumFilter interface from our filter graph
	IEnumFilters *pEF;
	HRESULT hr = m_pGraph->EnumFilters(&pEF);
	if ( Error(hr) )
		return(NO);

	while (1)
	{
		// Obtain the IFilter interface from our IEnumFilter interface
		IBaseFilter *pFilter;
		ULONG lCount;
		hr = pEF->Next( 1/*nFilters*/, &pFilter, &lCount );
		if ( lCount != 1 )
			break;
		if ( Error(hr) )
			break;

		FILTER_INFO FilterInfo;
		hr = pFilter->QueryFilterInfo( &FilterInfo );
		pFilter->Release();
		if ( Error(hr) )
			break;

		FNAME szFileName;
		WideCharToMultiByte( CP_ACP, 0, FilterInfo.achName, 128, szFileName, sizeof(szFileName),
		    NULL/*LPCSTR lpDefaultChar*/, NULL/*LPBOOL lpUsedDefaultChar*/);
		Debug( "Filter: %s", szFileName );

		if ( FilterInfo.pGraph )
			FilterInfo.pGraph->Release();
	}

	pEF->Release();
	return(YES);
}

//***********************************************************************
BOOL CAMovie::Attach( HWND hWndPlay )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	if ( !hWndPlay )
		return(NO);

	// Obtain the IVideoWindow interface from our filter graph
	IVideoWindow *pVW;
	HRESULT hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	if ( Error(hr) )
		return(NO);

	// Set the window parameter and release
	hr = pVW->put_Owner((OAHWND)hWndPlay);
	pVW->put_MessageDrain((OAHWND)hWndPlay);
//	pVW->put_Visible(-1/*TRUE*/);

	long dwStyle;
	pVW->get_WindowStyle(&dwStyle);
//	dwStyle &= (~WS_CAPTION); // WS_BORDER | WS_DLGFRAME
	dwStyle &= (~WS_BORDER);
	dwStyle &= (~WS_THICKFRAME);
	dwStyle &= (~WS_DLGFRAME);
	dwStyle &= (~WS_SYSMENU);
	dwStyle &= (~WS_MINIMIZEBOX);
	dwStyle &= (~WS_MAXIMIZEBOX);
	dwStyle &= (~WS_CLIPCHILDREN);
	dwStyle &= (~WS_CLIPSIBLINGS);
	dwStyle &= (~WS_POPUP);
	dwStyle |= (WS_CHILD);
//	dwStyle |= (WS_CLIPCHILDREN);
//	dwStyle |= (WS_CLIPSIBLINGS);
	hr = pVW->put_WindowStyle(dwStyle);
	pVW->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::Close( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	//if looping...
	//	turn it off

	// Obtain the IVideoWindow interface from our filter graph
    IVideoWindow *pVW;
    HRESULT hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	if ( Error(hr) )
		return(NO);

    // Must disconnect from the owner, otherwise nasty things can happen!
    hr = pVW->put_Visible(NO);
    hr = pVW->put_Owner(NULL);
    pVW->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::Stop( BOOL fWait )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

    if ( !CanStop() )
		return(NO);

	// Obtain the IMediaControl interface from our filter graph
	IMediaControl *pMC;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if ( Error(hr) )
		return(NO);

    // Ask the filter graph to stop and release the interface
	hr = pMC->StopWhenReady(); // or use pMC->Stop();
	pMC->Release();
	if ( Error(hr) )
		return(NO);

//	// Obtain the IMediaEvent interface from our filter graph
//	IMediaEvent *pME;
//	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&pME);
//	if ( Error(hr) )
//		return(NO);
//
//	long l;
//	hr = pME->WaitForCompletion(INFINITE, &l);
//	pME->Release();
//	if ( Error(hr) )
//		return(NO);

	m_State = Stopped;
	return(YES);
}

//***********************************************************************
DWORD CAMovie::GetPosition(void)
//***********************************************************************
{
    if ( !m_pGraph )
		return(0);

	// Obtain the IMediaPosition interface from our filter graph
	IMediaPosition * pMP;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if ( Error(hr) )
		return(0);

	REFTIME tPosition;
	hr = pMP->get_CurrentPosition(&tPosition);
	pMP->Release();
	if ( Error(hr) )
		return(0);

	DWORD dwPosition = (DWORD)((tPosition * m_dwFrameRate) + .5);
	return(dwPosition);
}

//***********************************************************************
DWORD CAMovie::GetLength( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	// Obtain the IMediaPosition interface from our filter graph
	IMediaPosition * pMP;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if ( Error(hr) )
		return(NO);

	REFTIME tLength;
	hr = pMP->get_Duration(&tLength);
	pMP->Release();
	if ( Error(hr) )
		return(NO);

	DWORD dwLength = (DWORD)((tLength * m_dwFrameRate) + .5);
	return(dwLength);
}

//***********************************************************************
BOOL CAMovie::SetTimeFormat( BOOL bFrames )
//***********************************************************************
{
	return(YES);
}

//***********************************************************************
BOOL CAMovie::Play( long lFrom, long lTo, DWORD dwSpeed, BOOL bFullScreen, BOOL bRepeat )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

    if ( !CanPlay() )
		return(NO);

	if ( !lFrom && !lTo )
	{ // (lFrom=0, lTo=0) is the same as playing nothing
		lFrom = -1;
		lTo = -1;
	}

	if ( bFullScreen )
		; // Handle this someday

	if (bRepeat)
		; // Handle this someday

	if ( dwSpeed )
		SetVideoSpeed( dwSpeed );

	if ( lFrom >= 0 )
		Seek(lFrom);

	if ( lTo >= 0 )
		SeekStop(lTo);
	else
		SeekStop( GetLength() );

	// Obtain the IMediaControl interface from our filter graph
	IMediaControl *pMC;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if ( Error(hr) )
		return(NO);
		
	// Ask the filter graph to play and release the interface
	hr = pMC->Run();
	pMC->Release();
	if ( Error(hr) )
		return(NO);

	// Obtain the IMediaEvent interface from our filter graph
//	IMediaEvent *pME;
//	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&pME);
//	if ( Error(hr) )
//		return(NO);
//
//	long l;
//	hr = pME->WaitForCompletion(INFINITE, &l);
//	pME->Release();
//	if ( Error(hr) )
//		return(NO);

	m_State = Playing;
	return(YES);
}

//***********************************************************************
DWORD CAMovie::GetFrameRate( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return(DEFAULT_FRAMES_PER_SEC);

	// Obtain the IBasicVideo interface from our filter graph
	IBasicVideo *pVW;
	HRESULT hr = m_pGraph->QueryInterface(IID_IBasicVideo, (void **)&pVW);
	if ( Error(hr) )
		return(DEFAULT_FRAMES_PER_SEC);

	// Get the frame rate of the video and release
	REFTIME tTimePerFrame;
	hr = pVW->get_AvgTimePerFrame(&tTimePerFrame);
	pVW->Release();
	if ( Error(hr) )
		return(DEFAULT_FRAMES_PER_SEC);

	return( (DWORD)((1/tTimePerFrame) + .5) );
}

//***********************************************************************
BOOL CAMovie::GetSrcRect( LPRECT lpSrcRect )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	if ( !lpSrcRect )
		return(NO);

	// Obtain the IBasicVideo interface from our filter graph
	IBasicVideo *pVW;
	HRESULT hr = m_pGraph->QueryInterface(IID_IBasicVideo, (void **)&pVW);
	if ( Error(hr) )
		return(NO);

	// Get the size of the video and release
	long lWidth, lHeight;
	hr = pVW->get_VideoWidth(&lWidth);
		 pVW->get_VideoHeight(&lHeight);
	pVW->Release();
	if ( Error(hr) )
		return(NO);

	lpSrcRect->left = 0;
	lpSrcRect->top  = 0;
	lpSrcRect->right  = lWidth;
	lpSrcRect->bottom = lHeight;
	return(YES);
}

//***********************************************************************
BOOL CAMovie::SetDstRect( LPRECT lpDstRect )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	if ( !lpDstRect )
		return(NO);

	// Note that rect is not a real rect, but a left, top, width and height
	RECT rect;
	rect.left	= lpDstRect->left;
	rect.top	= lpDstRect->top;
	rect.right  = lpDstRect->right  - lpDstRect->left;
	rect.bottom = lpDstRect->bottom - lpDstRect->top;

	// Obtain the IVideoWindow interface from our filter graph
	IVideoWindow *pVW;
	HRESULT hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	if ( Error(hr) )
		return(NO);

	// Set the window parameter and release
	hr = pVW->SetWindowPosition(rect.left, rect.top, rect.right, rect.bottom);
	pVW->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::Update( HDC hDC )
//***********************************************************************
{
	if ( !hDC )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::Pause( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

    if ( !CanPause() )
		return(NO);

	// Obtain the IMediaControl interface from our filter graph
	IMediaControl *pMC;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if ( Error(hr) )
		return(NO);

	// Ask the filter graph to pause and release the interface
	hr = pMC->Pause();
	pMC->Release();
	if ( Error(hr) )
		return(NO);

	m_State = Paused;
	return(YES);
}

//***********************************************************************
BOOL CAMovie::Resume( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

    if ( !IsPaused() )
		return(NO);

	// Obtain the IMediaControl interface from our filter graph
	IMediaControl *pMC;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if ( Error(hr) )
		return(NO);

	// Ask the filter graph to play and release the interface
	hr = pMC->Run();
	pMC->Release();
	if ( Error(hr) )
		return(NO);

	m_State = Playing;
	return(YES);
}

//***********************************************************************
BOOL CAMovie::SetVideoOnOff( BOOL bFlag )
//***********************************************************************
{
	return(YES);
}

//***********************************************************************
BOOL CAMovie::SetAudioOnOff( BOOL bFlag )
//***********************************************************************
{
	return(YES);
}

//***********************************************************************
BOOL CAMovie::Seek( long lFrame )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	if ( lFrame < 0 )
		lFrame = 0;

	// Obtain the IMediaPosition interface from our filter graph
	IMediaPosition * pMP;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if ( Error(hr) )
		return(NO);

	REFTIME tPosition = lFrame;
	tPosition /= m_dwFrameRate;
	hr = pMP->put_CurrentPosition(tPosition);
	pMP->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::SeekStop( long lFrame )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	if ( lFrame < 0 )
		lFrame = 0;

	// Obtain the IMediaPosition interface from our filter graph
	IMediaPosition * pMP;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if ( Error(hr) )
		return(NO);

	REFTIME tPosition = lFrame;
	tPosition /= m_dwFrameRate;
	hr = pMP->put_StopTime(tPosition);
	pMP->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::StepForward( int nFrames )
//***********************************************************************
{
	return( Seek( GetPosition() + nFrames ) );
}

//***********************************************************************
BOOL CAMovie::StepReverse( void )
//***********************************************************************
{
	return( Seek( GetPosition() - 1 ) );
}

//***********************************************************************
BOOL CAMovie::SetVideoSpeed( DWORD dwSpeed )
//***********************************************************************
{
    if ( !m_pGraph )
		return(NO);

	// Obtain the IMediaPosition interface from our filter graph
	IMediaPosition * pMP;
	HRESULT hr = m_pGraph->QueryInterface(IID_IMediaPosition, (void**)&pMP);
	if ( Error(hr) )
		return(NO);

	double dRate = dwSpeed;
	dRate /= 1000;
	hr = pMP->put_Rate(dRate);
	pMP->Release();
	if ( Error(hr) )
		return(NO);

	return(YES);
}

//***********************************************************************
BOOL CAMovie::CanPlay( void )
//***********************************************************************
{
    return (m_State == Playing || m_State == Stopped || m_State == Paused);
}

//***********************************************************************
BOOL CAMovie::CanStop( void )
//***********************************************************************
{
    return (m_State == Playing || m_State == Paused);
}

//***********************************************************************
BOOL CAMovie::CanPause( void )
//***********************************************************************
{
    return (m_State == Playing || m_State == Stopped);
}

//***********************************************************************
BOOL CAMovie::IsPlaying( void )
//***********************************************************************
{
    return(m_State == Playing);
}

//***********************************************************************
BOOL CAMovie::IsStopped( void )
//***********************************************************************
{
    return (m_State == Stopped);
}

//***********************************************************************
BOOL CAMovie::IsPaused( void )
//***********************************************************************
{
    return(m_State == Paused);
}

//***********************************************************************
BOOL CAMovie::IsInitialized( void )
//***********************************************************************
{
    return (m_State != Uninitialized);
}

//***********************************************************************
BOOL CAMovie::CreateFilterGraph( void )
//***********************************************************************
{
    if (m_pGraph) // Filter graph already created
		return FALSE;

    HRESULT hr = CoCreateInstance(CLSID_FilterGraph,	// CLSID of object
			  NULL,										// Outer unknown
			  CLSCTX_INPROC_SERVER,						// Type of server
			  IID_IGraphBuilder,						// Interface wanted
			  (void**)&m_pGraph);						// Returned object
    if ( Error(hr) )
	{
		ReleaseFilterGraph();
		return FALSE;
    }


	// Obtain the IMediaEvent interface from our filter graph
    IMediaEvent *pME;
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&pME);
    if ( Error(hr) )
	{
		ReleaseFilterGraph();
		return FALSE;
    }

    hr = pME->GetEventHandle((OAEVENT*) &m_hGraphNotifyEvent);
    pME->Release();

    if ( Error(hr) )
	{
		ReleaseFilterGraph();
		return FALSE;
    }

	GetApp()->EventAdd( m_hGraphNotifyEvent, CAMovie::EventProc, (DWORD)(LPSTR)this );
    return TRUE;
}

//***********************************************************************
void CAMovie::ReleaseFilterGraph( void )
//***********************************************************************
{
    // this event is owned by the filter graph and is thus invalid
	if ( m_hGraphNotifyEvent )
	{
		GetApp()->EventRemove( m_hGraphNotifyEvent );
		m_hGraphNotifyEvent = NULL;
	}

    if (m_pGraph)
	{
		m_pGraph->Release();
		m_pGraph = NULL;
    }

    m_State = Uninitialized;
}

// If the event handle is valid, then ask the graph if
// anything has happened (eg the graph has stopped...)
//
//EC_ACTIVATE					An audio or video renderer is losing or gaining activation. 
//EC_BUFFERING_DATA 			The buffering status is changing. 
//EC_CLOCK_CHANGED				The filter graph has changed from one reference clock to another. 
//EC_COMPLETE 					All data has been rendered. 
//EC_DISPLAY_CHANGED 			The current display mode has been changed. 
//EC_ERROR_STILLPLAYING 		A playback error has occurred, but the graph is still playing. 
//
//EC_ERRORABORT 				An error forced the termination of a requested operation. 
//EC_FULLSCREEN_LOST 			The video renderer is switching out of full-screen mode. 
//EC_NEED_RESTART 				The current graph must be stopped and restarted. 
//EC_OLE_EVENT 					A filter is passing a text string to the application. 
//EC_OPENING_FILE 				The open file status is changing. 
//EC_PALETTE_CHANGED 			The video palette has changed. 
//EC_QUALITY_CHANGE 			The playback quality has changed. 
//
//EC_REPAINT 					A repaint is required. 
//EC_SHUTTING_DOWN 				The shutdown of the filter graph is starting. 
//EC_STARVATION 				A filter has detected starvation. 
//EC_STREAM_ERROR_STILLPLAYING 	The stream is still playing, but should not be playing. 
//EC_STREAM_ERROR_STOPPED 		The stream has stopped, but should not have stopped. 
//EC_TIME 						The requested reference time occurred. 
//EC_USERABORT 					A user has forced the termination of a requested operation. 
//
//EC_VIDEO_SIZE_CHANGED			The size of the native video has changed. 
//EC_WINDOW_DESTROYED 			The video renderer's filter is being removed or destroyed. 

//***********************************************************************
void CAMovie::OnGraphNotify( void )
//***********************************************************************
{
    if ( !m_pGraph )
		return;
    if ( !m_hGraphNotifyEvent ) // Bad call to OnGraphNotify()
		return;

	// Obtain the IVideoWindow interface from our filter graph
    IVideoWindow *pVW;
    HRESULT hr = m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	if ( Error(hr) )
		return;

	HWND hWnd;
    hr = pVW->get_Owner((OAHWND*)&hWnd);
    pVW->Release();
	if ( Error(hr) )
		return;

	// Obtain IMediaEvent the interface from our filter graph
    IMediaEvent *pME;
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&pME);
    if ( Error(hr) )
		return;

	long lEventCode, lParam1, lParam2;
	hr = pME->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
	if ( Error(hr) )
	{
		pME->Release();
		return;
	}

	pME->FreeEventParams(lEventCode, lParam1, lParam2);
	pME->Release();

	if (lEventCode == EC_COMPLETE)
	{
		if ( hWnd )
			SendMessage( hWnd, MM_MCINOTIFY, (WPARAM)MCI_NOTIFY_SUCCESSFUL, (LPARAM)NULL/*hDevice*/);
	}
	else
	if (lEventCode == EC_USERABORT)
	{
		if ( hWnd )
			SendMessage( hWnd, MM_MCINOTIFY, (WPARAM)MCI_NOTIFY_ABORTED, (LPARAM)NULL/*hDevice*/);
	}
	else
	if (lEventCode == EC_ERRORABORT)
	{
		if ( hWnd )
			SendMessage( hWnd, MM_MCINOTIFY, (WPARAM)MCI_NOTIFY_FAILURE, (LPARAM)NULL/*hDevice*/);
	}
	else
		; //Debug("Unknown event %ld", lEventCode);
}

//extern "C" DWORD WINAPI AMGetErrorTextA( HRESULT hr , char *pbuffer , DWORD MaxLen);

//***********************************************************************
BOOL CAMovie::Error( HRESULT hr )
//***********************************************************************
{
	if ( !FAILED(hr) )
		return(NO);

	//STRING szString;
	//AMGetErrorTextA( hr, szString, sizeof(szString) );
	//Debug( "CAMovie Error 0x%lx: %ls", hr, (LPSTR)szString );
	return(YES);
}

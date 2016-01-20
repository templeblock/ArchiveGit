#include <windows.h>
#include "proto.h"
#include "vreco.h"
#ifdef WIN32
	#include "vmapi.h"
	#include "icwapi32.h"
#else
	#include "icwapi16.h"
#endif

// Global data
UINT ICSSMessageId;

// Static data
static long lContextHandle;
static BOOL bInit;

//******************************************************************************
BOOL VoiceRecoInit(HWND hWnd)
//******************************************************************************
{
	if ( bInit )
		return( TRUE );

	#ifdef WIN32
	// Make voice reco registry entries under Win95
	FNAME szFileName;
	GetModuleFileName( GetApp()->GetInstance(), szFileName, sizeof(szFileName) );
	StripFile( szFileName );
	HKEY hKey;
	RegCreateKey( HKEY_LOCAL_MACHINE, "\\Software\\IBM\\ICSS", &hKey );
	RegSetValueEx( hKey, "InstallDir", NULL, REG_SZ, (CONST BYTE*)szFileName,
		(DWORD)lstrlen(szFileName)+1 );
	#endif

	if ( !ICSSMessageId )
		ICSSMessageId = RegisterWindowMessage(ICSS_USER_MSG_STRING);

	/******************************************************************************/
	/* This section starts the ICSS initialization process. Note that StartConv.  */
	/* is an asynchronous call, so results must be checked both here (for argument*/
	/* errors), and during message loop processing for processing errors.		  */
	/******************************************************************************/
	long lRetCode;
	if ( (lRetCode = ICSSStart(NULL)) != ICSS_SUCCESS )
	{
		ICSSEnd();
		Print( "Failed trying to start voice recognition. (code %ld)", lRetCode );
		return( FALSE );
	}
	if ( (lRetCode = (BOOL)ICSSStartConversation(ICSS_DEFAULT_STRING, ICSS_DEFAULT_INT,
			ICSS_DEFAULT_STRING, ICSS_DIRECT, hWnd)) != ICSS_SUCCESS )
	{
		ICSSEndConversation();
		ICSSEnd();
		Print( "Failed trying to start a voice recognition conversation. (code %ld)", lRetCode );
		return( FALSE );
	}

	return( bInit = TRUE );
}

//******************************************************************************
void VoiceRecoClose(void)
//******************************************************************************
{
	/******************************************************************************/
	/* This section ensures that the ICSS services receive notification of		  */
	/* EndConversation and End whenever the sample application is shutting down.  */
	/* This will occur in either normal or error conditions.					  */
	/******************************************************************************/
	if ( !bInit )
		return;
	bInit = FALSE;
	VoiceRecoOff();
	ICSSEndConversation();
	ICSSEnd();
}

//******************************************************************************
BOOL VoiceRecoOn(HWND hWnd)
//******************************************************************************
{
	long lRetCode = ICSSListen(lContextHandle, ICSS_DEFAULT_INT, ICSS_DEFAULT_STRING);
	if (lRetCode != ICSS_SUCCESS)
	{
		Print( "Failed trying to listen. (code %ld)", lRetCode );
		return( FALSE );
	}

	if ( (lRetCode = ICSSGetSpokenWords(hWnd)) != ICSS_SUCCESS )
	{
		Print( "Failed trying to get spoken words. (code %ld)", lRetCode );
		return( FALSE );
	}

	Yield();
	return( TRUE );
}

//******************************************************************************
void VoiceRecoOff(void)
//******************************************************************************
{
	long lRetCode;
	do
	{
		lRetCode = ICSSInterruptListenState();
		Yield();
	} while ( lRetCode == ICSS_SUCCESS );
}

// Process the reply messages for ICSS asynchronous requests
//******************************************************************************
long ProcessICSSReplyMsg(HWND hWnd, UINT wFuncId, long lData)
//******************************************************************************
{
	ICSS_RETURN_WORDS FAR *pWords;
	ICSS_PARMTYPE parm_val;

	long lRetCode = ICSS_SUCCESS;
	switch (wFuncId)
	{
		case REPLY_START_CONVERSATION:
		{
			/******************************************************************************/
			/* This section processes the asynchronous portion of the StartConversation	  */
			/* call, including error processing and initiating the context loading.		  */
			/******************************************************************************/
			if (lRetCode = lData)
			{
				VoiceRecoClose();
				Print( "Failed trying to startup voice recognition. (code %ld)", lRetCode );
				break;
			}

			lRetCode = ICSSLoadContext (&lContextHandle, "jungle.ctx", ICSS_DEFAULT_FLOAT,
						ICSS_DEFAULT_FLOAT, ICSS_DEFAULT_FLOAT, ICSS_DEFAULT_FLOAT);
			if (lRetCode != ICSS_SUCCESS)
			{
				VoiceRecoClose();
				Print( "Failed trying to load the voice context. (code %ld)", lRetCode );
				break;
			}

			// Set timeout for speech to 5 seconds
			parm_val.ICSS_FLOATParameterData = (float)5.0;
			ICSSSetValue(ICSS_CFLT_UTT_MAX_SIL_DURATION,parm_val);

			// eliminate server, mci and rcp icons
			parm_val.ICSS_INTParameterData = 0L;
			ICSSSetValue(ICSS_CINT_VERBOSE_LEVEL,parm_val);
			break;
		}

		case REPLY_GET_SPOKEN_WORDS:
		{
			/******************************************************************************/
			/*	This section processes the asynchronous results from the GetSpokenWords	  */
			/*	call, including error checking, display of recognized text and the		  */
			/*	automatic start of the ICSS and application termination.				  */
			/******************************************************************************/
			if ( !bInit )
				break;
		#ifdef WIN32
			ICSS_RETURN_WORDS retWords;
			pWords = &retWords;
			lRetCode = ICSSDecodeReturnedWords((int)lData, &retWords);
		#else
			pWords = (ICSS_RETURN_WORDS FAR *)GlobalLock((HANDLE)lData);
		#endif
			if ( !pWords || pWords->return_code != ICSS_SUCCESS)
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 99, SendMessage); // send an invalid responce
			}
			else
			if ( !pWords->spoken_words )
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 99, SendMessage); // send an invalid responce
			}
			else
			if (!lstrcmp((LPSTR)pWords->spoken_words, "AWOWOW"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 0, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "CHAKEECHA"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 1, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "CHEKACHEE"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 2, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "JAKEETA"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 3, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "JATA"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 4, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "KACHEEKEE"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 5, SendMessage);
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "KAKEE"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 6, SendMessage); // treat the same as "kakeeta"
			}
			else 
			if (!lstrcmp((LPSTR)pWords->spoken_words, "KAKEETA"))
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 6, SendMessage); // treat the same as "kakee"
			}
			else 
			{
				FORWARD_WM_COMMAND (FindClassDescendent(hWnd, "scene"),
					IDC_VRECO_MSG, hWnd, 99, SendMessage); // send an invalid responce
			}
		#ifndef WIN32
			GlobalUnlock((HANDLE)lData);
			GlobalFree((HANDLE)lData);
		#endif
			break;
		}

		default:
			break;
	}
	return lRetCode;
}

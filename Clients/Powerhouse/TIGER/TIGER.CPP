#include "windows.h"
#include "..\gameguts\main.h"
#include "..\gameguts\sound.h"
#include "jungle.h"
#include "jungle.p"
 
int iSkill;
static int iAnswer, iQuestion, iCategory, iPlayer;
static long lScore[4];
static ITEMID idQuestion;

//************************************************************************
LOCAL BOOL OnScene1Init( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	iPlayer = 0;
	iQuestion = 0;
	iCategory = 0;
	iAnswer = 0;
	idQuestion = 0;
	lScore[1] = lScore[2] = lScore[3] = lScore[4] = 0;
	iSkill = 100;

	SetDlgItemInt( hWnd, IDC_SKILL, iSkill, NO );

	SetDlgItemText( hWnd, IDC_SCORE1, "0" );
	SetDlgItemText( hWnd, IDC_SCORE2, "0" );
	SetDlgItemText( hWnd, IDC_SCORE3, "0" );
	SetDlgItemText( hWnd, IDC_SCORE4, "0" );

	SetDlgItemText( hWnd, IDC_QUESTION, "" ); 
	SetDlgItemText( hWnd, IDC_CHOICEA, "" );
	SetDlgItemText( hWnd, IDC_CHOICEB, "" );
	SetDlgItemText( hWnd, IDC_CHOICEC, "" );
	SetDlgItemText( hWnd, IDC_CHOICED, "" );

	return( YES );
}

//************************************************************************
LOCAL void OnScene1Command(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	STRING szString;
	int i;
	long lNewScore;
	static long lBaseTime;


	cSOUND::StartResourceID( id, NO/*bLoop*/, -1/*iChannel*/, App.GetInstance() );

	switch (id)
	{
		case IDC_SKILL:
		{
			BOOL Bool;

			if ( codeNotify != EN_CHANGE )
				break;
			iSkill = GetDlgItemInt( hWnd, id, &Bool, NO );
			if ( iSkill < 0 )	iSkill = 0;
			if ( iSkill > 100 )	iSkill = 100;
			break;
		}

		case IDC_CATEGORY1:
		case IDC_CATEGORY2:
		case IDC_CATEGORY3:
		case IDC_CATEGORY4:
		case IDC_CATEGORY5:
		case IDC_CATEGORY6:
		case IDC_CATEGORY7:
		case IDC_CATEGORY8:
		case IDC_CATEGORY9:
		case IDC_CATEGORY10:
		{
			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			iCategory = id - IDC_CATEGORY1;
			cSOUND::StartFile( "game.wav", NO/*bLoop*/, -1/*iChannel*/ );
//			if ( hMCIfile = MCIOpen( App.hDeviceWAV, "game.wav", NULL/*lpAlias*/ ) )
//				if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//					MCIClose( hMCIfile );
			break;
		}

		case IDC_PLAYER1:
		case IDC_PLAYER2:
		case IDC_PLAYER3:
		case IDC_PLAYER4:
		{
			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			iPlayer = id - IDC_PLAYER1;
			FlashWindow( GetDlgItem( hWnd, IDC_SCORE1 + iPlayer), NO );
			break;
		}

		case IDC_ANSWERA:
		case IDC_ANSWERB:
		case IDC_ANSWERC:
		case IDC_ANSWERD:
		{
			if ( !lBaseTime )
			{ // We're not waiting for an answer
				MessageBeep(0);
				break;
			}

			if ( iAnswer != (int)(id - IDC_ANSWERA) )
			{ // Wrong Answer!
				cSOUND::StartFile( "wrong.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "wrong.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
//					else
//						Delay( 2000 );
				break;
			}

			// Right answer! Applaud!
			switch ( GetRandomNumber( 3 ) )
			{
				case 0:
				{
					cSOUND::StartFile( "right1.wav", NO/*bLoop*/, -1/*iChannel*/ );
//					if ( hMCIfile = MCIOpen( App.hDeviceWAV, "right1.wav", NULL/*lpAlias*/ ) )
//						if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//							MCIClose( hMCIfile );
//						else
//							Delay( 3000 );
					break;
				}
				case 1:
				{
					cSOUND::StartFile( "right2.wav", NO/*bLoop*/, -1/*iChannel*/ );
//					if ( hMCIfile = MCIOpen( App.hDeviceWAV, "right2.wav", NULL/*lpAlias*/ ) )
//						if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//							MCIClose( hMCIfile );
//						else
//							Delay( 3000 );
					break;
				}
				case 2:
				{
					cSOUND::StartFile( "right3.wav", NO/*bLoop*/, -1/*iChannel*/ );
//					if ( hMCIfile = MCIOpen( App.hDeviceWAV, "right3.wav", NULL/*lpAlias*/ ) )
//						if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//							MCIClose( hMCIfile );
//						else
//							Delay( 3000 );
					break;
				}
			}

			// Get any additional sound to play
			if ( GetString( idQuestion+6, szString ) )
				cSOUND::StartFile( szString, NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, szString, NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
	   	
			// Figure out the points
			lBaseTime = (long)GetTickCount() - lBaseTime;
			if ( lBaseTime < 0 )
				break;
			if ( lBaseTime > 10000 ) // 10 points for > 10 seconds
				lNewScore = 1000;
			else
				lNewScore = (10000 - lBaseTime) + 1000;
			lNewScore = 100 * (( lNewScore + 50 ) / 100);
			lScore[iPlayer] += lNewScore;
			wsprintf( szString, "%ld", lScore[iPlayer] );
			SetDlgItemText( hWnd, IDC_SCORE1 + iPlayer, szString );

			// No longer waiting for an answer
			lBaseTime = 0;

			// Automatically switch to the next player
			if ( iPlayer == 3 )
				PostMessage( hWnd, WM_COMMAND, IDC_PLAYER1, 0L );
			else
				PostMessage( hWnd, WM_COMMAND, IDC_PLAYER1 + iPlayer + 1, 0L );
			break;
		}

		case IDC_NUMBER0:
		case IDC_NUMBER1:
		case IDC_NUMBER2:
		case IDC_NUMBER3:
		case IDC_NUMBER4:
		case IDC_NUMBER5:
		case IDC_NUMBER6:
		case IDC_NUMBER7:
		case IDC_NUMBER8:
		case IDC_NUMBER9:
		{
			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			i = id - IDC_NUMBER0;
			i = iQuestion * 10 + i;
			if ( i > 1001 )
			{
				MessageBeep(0);
				break;
			}
			iQuestion = i;
			wsprintf( szString, "%d", iQuestion );
			SetDlgItemText( hWnd, IDC_QUESTION, szString );
			SetDlgItemText( hWnd, IDC_CHOICEA, "" );
			SetDlgItemText( hWnd, IDC_CHOICEB, "" );
			SetDlgItemText( hWnd, IDC_CHOICEC, "" );
			SetDlgItemText( hWnd, IDC_CHOICED, "" );
			break;
		}

		case IDC_CLEAR:
		{
			if ( lBaseTime )
			{ // If we were waiting for an answer
				lBaseTime = 0;
				SetDlgItemText( hWnd, IDC_QUESTION, "" );
				SetDlgItemText( hWnd, IDC_CHOICEA, "" );
				SetDlgItemText( hWnd, IDC_CHOICEB, "" );
				SetDlgItemText( hWnd, IDC_CHOICEC, "" );
				SetDlgItemText( hWnd, IDC_CHOICED, "" );
				break;
			}

			iQuestion /= 10;
			wsprintf( szString, "%d", iQuestion );
			SetDlgItemText( hWnd, IDC_QUESTION, iQuestion? szString : "" );
			SetDlgItemText( hWnd, IDC_CHOICEA, "" );
			SetDlgItemText( hWnd, IDC_CHOICEB, "" );
			SetDlgItemText( hWnd, IDC_CHOICEC, "" );
			SetDlgItemText( hWnd, IDC_CHOICED, "" );
			break;
		}

		case IDC_ENTER:
		{
			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			if ( !iQuestion )
			{ // A question number hasn't been entered yet
				MessageBeep(0);
				break;
			}

			idQuestion = IDC_QUESTION_BASE + (iQuestion * 10);
			iQuestion = 0; // Start at zero again

			// Get the question
			if ( !GetString( idQuestion, szString ) )
				lstrcpy( szString, "123: How many questions does Quiz Wiz have?" );
			SetDlgItemText( hWnd, IDC_QUESTION, szString );

			// Get the answer choice A
			if ( !GetString( idQuestion+1, szString ) )
				lstrcpy( szString, "A) 1001" );
			SetDlgItemText( hWnd, IDC_CHOICEA, szString );

			// Get the answer choice B
			if ( !GetString( idQuestion+2, szString ) )
				lstrcpy( szString, "B) 25" );
			SetDlgItemText( hWnd, IDC_CHOICEB, szString );

			// Get the answer choice C
			if ( !GetString( idQuestion+3, szString ) )
				lstrcpy( szString, "C) One quintillion" );
			SetDlgItemText( hWnd, IDC_CHOICEC, szString );

			// Get the answer choice D
			if ( !GetString( idQuestion+4, szString ) )
				lstrcpy( szString, "D) One billion" );
			SetDlgItemText( hWnd, IDC_CHOICED, szString );

			// Get the answer
			if ( !GetString( idQuestion+5, szString ) )
				lstrcpy( szString, "A" );
			iAnswer = szString[0] - 'A';

			// Start the clock ticking...
			lBaseTime = (long)GetTickCount();
			break;
		}

		case IDC_START:
		{
			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			OnScene1Init( hWnd, (HWND)NULL, (LPARAM)0L );
			break;
		}

		case IDC_RANDOM:
		{
			// Try it with a preliminary clear
			SendMessage( hWnd, WM_COMMAND, IDC_CLEAR, 0L );

			if ( lBaseTime )
			{ // We are waiting for an answer
				cSOUND::StartFile( "answer.wav", NO/*bLoop*/, -1/*iChannel*/ );
//				if ( hMCIfile = MCIOpen( App.hDeviceWAV, "answer.wav", NULL/*lpAlias*/ ) )
//					if ( !MCIPlay( hMCIfile, NO/*bWait*/ ) )
//						MCIClose( hMCIfile );
				break;
			}

			iQuestion = 1 + GetRandomNumber( 1001 );
			wsprintf( szString, "%d", iQuestion );
			SetDlgItemText( hWnd, IDC_QUESTION, iQuestion? szString : "" );

			// Just like you hit enter
			PostMessage( hWnd, WM_COMMAND, IDC_ENTER, 0L );
			break;
		}

		case IDC_DIB:
		{
			MessageBeep(0);
			break;
		}
		case IDC_STOP:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, 'S', 0L );
			break;
		}
		case IDC_PLAY:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, VK_SPACE, 0L );
			break;
		}
		case IDC_PAUSE:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, 'P', 0L );
			break;
		}
		case IDC_FORWARD:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, 'F', 0L );
			break;
		}
		case IDC_REVERSE:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, 'R', 0L );
			break;
		}
		case IDC_JUMP:
		{
			SendDlgItemMessage( hWnd, IDC_VIDEO, WM_KEYDOWN, 'J', 0L );
			break;
		}
		default:
			break;
	}
}

/************************************************************************/
BOOL OnSceneInit(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	int iScene = DialogGetID( hWnd );
 	switch (iScene)
	{
		case 1:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		case 2:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		case 3:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		case 4:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		case 5:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		case 6:
		{
			return( OnScene1Init( hWnd, hWndFocus, lParam ) );
		}
		default:
		{
			return( FALSE );
		}
	}
   	return( FALSE );
}

//************************************************************************
void OnSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	int iScene = DialogGetID( hWnd );
	switch (iScene)
	{
		case 1:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		case 2:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		case 3:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		case 4:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		case 5:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		case 6:
		{
			OnScene1Command( hWnd, id, hControl, codeNotify );
			break;
		}
		default:
		{
			break;
		}
	}
}


//************************************************************************
BOOL OnSceneMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
 		default:
		{
			return( FALSE );
		}
	}

	return( TRUE );
}

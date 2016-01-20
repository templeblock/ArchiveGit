#include <windows.h>
#include <mmsystem.h>
#include "proto.h"
#include "control.h"
#include "options.h"
#include "cllsn.h"

// Test file for setting volume
#define TEST_WAVE "driver11.wav"
#define TEST_MIDI "cllsn.mid"

#define MAX_OPTIONS 5
static BOOL bOption[MAX_OPTIONS];
static int iVolumeState;

void ResumeGame( HWND hWnd, int iGameNumber  );

//************************************************************************
BOOL OptionsSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam)
//************************************************************************
{
	StartMidi( NO );
	StartWave( NO );

	// Init. the midi and wave volume slider positions
	// Volume range of each channel -> 0x0000 to 0xffff, right channel volume is
	// in the high word, left channel is in the low word, if high word is zero,
	// the device is mono, so we'll just use the left value and assume they're the same
	// Midi volume
	DWORD dwVol = MCIGetVolume ((LPSTR)MCI_DEVTYPE_SEQUENCER);
	WORD wLeft = LOWORD(dwVol);
	wLeft &= 32767; // for some reason, the 32K bit is always on (in Win95)
	long lPosition = (100L * wLeft) / 32767L;
	SendDlgItemMessage( hWnd, IDC_SLIDER_MIDIVOL, SM_SETPOSITION, 0, lPosition );

	// Wave volume
	dwVol = MCIGetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO);
	wLeft = LOWORD(dwVol);
	wLeft &= 32767; // for some reason, the 32K bit is always on (in Win95)
	lPosition = (100L * wLeft) / 32767L;
	SendDlgItemMessage( hWnd, IDC_SLIDER_WAVVOL, SM_SETPOSITION, 0, lPosition );
		
	iVolumeState = 1;

	// Initialize the options
	bOption[0] = App.bOptionTransitions;
	bOption[1] = App.bOptionAutoTarget;
	bOption[2] = App.bOptionFireBack;
	bOption[3] = App.bOptionMusic;	
	bOption[4] = App.bOptionBarricades;
	//bOption[] = App.bOptionTaunts;	

	for (int i=0; i<MAX_OPTIONS; i++)
	{
		SendDlgItemMessage(hWnd, IDC_OPTION1+(i*2), SM_SETSTATE, bOption[i], 0L);
		SendDlgItemMessage(hWnd, IDC_OPTSTATE1+(i*2), SM_SETSTATE, bOption[i], 0L); 
		EnableWindow(GetDlgItem(hWnd, IDC_OPTSTATE1+(i*2)), FALSE);
	}

	return (TRUE);
}

//************************************************************************
void OptionsSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify )
//************************************************************************
{
	UINT loWord = (UINT)hControl;
	UINT hiWord = codeNotify;

	LPSCENE lpScene = NULL;
	LPOFFSCREEN lpOffScreen = NULL;

	switch (id)
	{
		case IDC_OPTION1:
		case IDC_OPTION2:
		case IDC_OPTION3:
		case IDC_OPTION4:
		case IDC_OPTION5:		
		{	
			int i = id - (int)IDC_OPTION1;			
			BOOL bState = !(1 & (int)SendDlgItemMessage(hWnd, id, SM_GETSTATE, 1, 0L));
			SendDlgItemMessage(hWnd, IDC_OPTSTATE1+i, SM_SETSTATE, bState, 0L);
			SendDlgItemMessage(hWnd, id, SM_SETSTATE, bState, 0L);

			LPSOUND pSound = new CSound();
			if (pSound)
			{
				pSound->StartResourceID( IDC_OPTSWITCH, NO/*bLoop*/, -1/*iChannel*/, GetApp()->GetInstance(), TRUE );
				delete(pSound);
			}

			break;
		}

		case IDC_EXIT:
		case IDC_RETURN:
		{
			for (int i=0; i<MAX_OPTIONS; i++)
				bOption[i] = (BOOL)SendDlgItemMessage(hWnd, IDC_OPTION1+(i*2), SM_GETSTATE, 1, 0L);

			// Assign the options to the values being controlled
			App.bOptionTransitions	= bOption[0];
			App.bOptionAutoTarget	= bOption[1];
			App.bOptionFireBack		= bOption[2];
			App.bOptionMusic		= bOption[3];
			App.bOptionBarricades	= bOption[4];
			//App.bOptionTaunts		= bOption[];	

			if ( App.bOptionsfromGame )
				ResumeGame( hWnd, 0 );
			else
				GetApp()->GotoScene (hWnd, 2); // Return to main screen
			break;
		}

		case IDC_VOLUME0:
		{
		    //iVolumeState = SendDlgItemMessage( hWnd, IDC_VOLUME0, SM_GETSTATE, 0, 0L );
			if (iVolumeState == 4)
			{
				iVolumeState = 0;
				SendDlgItemMessage( hWnd, IDC_VOLUME0, SM_SETSTATE, iVolumeState, 0L );
			}
			else
			{
				iVolumeState++;
				SendDlgItemMessage( hWnd, IDC_VOLUME0, SM_SETSTATE, iVolumeState, 0L);
			}
			break;
		}

		case IDC_SLIDER_MIDIVOL:
		{
			// Start and stop the looping sound
			StartMidi( !loWord ); // loWord is 1, on slider up
			WORD wVolume = (WORD)(( 32767L * (DWORD)hiWord ) / 100L);
			MCISetVolume ((LPSTR)MCI_DEVTYPE_SEQUENCER, wVolume | 0x8000, wVolume | 0x8000);
			break;
		}
      
		case IDC_SLIDER_WAVVOL:
		{
			// Start and stop the looping sound
			StartWave( !loWord ); // loWord is 1, on slider up
			WORD wVolume = (WORD)(( 32767L * (DWORD)hiWord ) / 100L);
			MCISetVolume ((LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO, wVolume | 0x8000, wVolume | 0x8000);
			break;
		}
      
		default:
			break;
	}
}

// Start and stop the wave file
//************************************************************************
static void StartWave( BOOL bTurnOn )
//************************************************************************
{
	static HMCI hPlayWave;

	if ( !bTurnOn )
	{ // turn the sound off
		if ( hPlayWave )
		{ // If playing, stop it
			MCIClose( hPlayWave );
			hPlayWave = NULL;
		}
	}
	else
	{ // turn the sound on
		if ( hPlayWave )
			return;	// If already playing, return

		if ( hPlayWave = MCIOpen (GetApp()->m_hDeviceWAV, TEST_WAVE, NULL/*lpAlias*/) )
		{
			MCILoop( hPlayWave, TRUE );
			if ( !MCIPlay( hPlayWave, GetApp()->GetMainWnd() ) )
			{
				MCIClose( hPlayWave );
				hPlayWave = NULL;
			}
		}
	}
}

// Start and stop the midi file
//************************************************************************
static void StartMidi( BOOL bTurnOn )
//************************************************************************
{
	static HMCI hPlayMidi;

	if ( !bTurnOn )
	{ // turn the sound off
		if ( hPlayMidi )
		{ // If playing, stop it
			MCIClose( hPlayMidi );
			hPlayMidi = NULL;
		}
	}
	else
	{ // turn the sound on
		if ( hPlayMidi )
			return;	// If already playing, return

		if ( hPlayMidi = MCIOpen (GetApp()->m_hDeviceMID, TEST_MIDI, NULL/*lpAlias*/) )
		{
			MCILoop( hPlayMidi, TRUE );
			if ( !MCIPlay( hPlayMidi, GetApp()->GetMainWnd() ) )
			{
				MCIClose( hPlayMidi );
				hPlayMidi = NULL;
			}
		}
	}
}

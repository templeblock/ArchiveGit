#ifndef _options_h_
#define _options_h_

// Source file: options.cpp
BOOL OptionsSceneInit (HWND hWnd, HWND hWndControl, LPARAM lParam);
void OptionsSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );
static void StartWave( BOOL bTurnOn );
static void StartMidi( BOOL bTurnOn );

#endif

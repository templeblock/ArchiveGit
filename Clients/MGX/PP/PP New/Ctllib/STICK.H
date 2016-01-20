// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// stick.h for joystick control
#include "stick.rh"

#ifdef CTLLIB
BOOL Stick_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Stick_UnRegisterClass (HINSTANCE hInstance);
#endif
long WINPROC Stick_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
void Stick_GetValue( HWND hWindow, LPINT lpX, LPINT lpY, int iStick = -1);
void Stick_SetValue( HWND hWindow, int x, int y, int iStick = -1);
void Stick_SetNumSticks( HWND hWindow, int iNumSticks);
void Stick_ActivateStick( HWND hWindow, int iStick);
void Stick_SetColor( HWND hWindow, int iStick, LPRGB lpColor);

// notification 
#define SCN_STICK_CHANGED 1
#define SCN_STICK_CHANGING 2

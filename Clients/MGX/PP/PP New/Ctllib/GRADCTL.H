/* Gradient Control */
// for gradctl.c

#ifndef __gradctl_h_
#define __gradctl_h_

long WINPROC Gradient_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );

void Gradient_SetRGB(HWND hGrad, LPRGB lpRGBFrom, LPRGB lpRGBTo = NULL);
void Gradient_SetHSL(HWND hGrad, LPHSL lpHSLFrom, LPHSL lpHSLTo = NULL);
void Gradient_SetType(HWND hGrad, int type, BOOL bDirection = FALSE);																	  
																	  
BOOL Gradient_RegisterClass (HINSTANCE hInstance, UINT ClassScope);
BOOL Gradient_UnRegisterClass (HINSTANCE hInstance);


// Types
#define GT_HSL	1	// from 1st hsl to 2nd hsl 
#define GT_RGB	3	// rgb from 1st color to 2nd color.


// notifications 
#define GN_LBUTTONDOWN 1
#endif // __gradctl_h_



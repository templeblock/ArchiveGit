#ifndef _TOONCTRL_H_
#define _TOONCTRL_H_

#include "toon.h"

LONG WINPROC EXPORT ToonHolderControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam ) ;
LONG WINPROC EXPORT ToonControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam ) ;
void SetToon(HWND hWindow, PTOON pToon);
PTOON GetToon(HWND hWindow); 

#define GWL_TOONPTR		0
#define TOON_EXTRA 		(GWL_TOONPTR+sizeof(long))
                    
#endif // _TOONCTRL_H_                
#ifndef _MACROS_H_
#define _MACROS_H_

#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define CONTROL ( GetAsyncKeyState( VK_CONTROL )<0 )
#define ALT ( GetAsyncKeyState( VK_ALT )<0 )
#define ESCAPE ( GetAsyncKeyState( VK_ESCAPE )<0 )
#define LBUTTON ( GetAsyncKeyState( GetSystemMetrics( SM_SWAPBUTTON ) ? VK_RBUTTON : VK_LBUTTON ) < 0 )

#define FLAGBIT(FlagNum) (1L<<(FlagNum))

#define RectWidth( lpRect )	 ( (lpRect)->right - (lpRect)->left )
#define RectHeight( lpRect ) ( (lpRect)->bottom - (lpRect)->top )
#define abs( num ) ( (num) < 0	? -(num) : (num) )

#endif // _MACROS_H_

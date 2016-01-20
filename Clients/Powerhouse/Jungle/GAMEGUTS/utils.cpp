#include "windows.h"
#include "proto.h"

/****************************************************************************

	FUNCTION	: f256Capable

	PURPOSE		: determines if the screen dc is capable of at least 8bpp

****************************************************************************/

BOOL f256Capable(void)
{
	//get screen device context
	HDC hdc = GetDC(NULL);

	//is the screen dc capable of at least 8bpp?
	if ((GetDeviceCaps(hdc, NUMCOLORS) >= 256) ||
	(GetDeviceCaps(hdc, BITSPIXEL) >= 8))
		{
		ReleaseDC(NULL, hdc);
		//at least 8bpp
		return TRUE;
		}
	else
		{
		ReleaseDC(NULL, hdc);
		//less than 8bpp
		return FALSE;
		}
}

/****************************************************************************

	FUNCTION	: fNTRunning

	PURPOSE		: determins if application is running under NT.

****************************************************************************/

BOOL fNTRunning(void)
{
	DWORD dwVersion = GetVersion();
	if (dwVersion < 0x80000000L)
		return(TRUE);
		//Control.WinVer = WV_NT;
	else
		return(FALSE);
	//if (LOBYTE(LOWORD(dwVersion)) < 4)
		//Control.WinVer = WV_WIN32S;
	//else
		//Control.WinVer = WV_WINDOWS95;
	//Control.fNewLook = Control.WinVer == WV_WINDOWS95;
}

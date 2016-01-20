#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <string.h>
#include "hotspot.h"
LPSTR FindToken(LPSTR psz, char c)
{
	static int len;
	int i = 0;
	static int start = 0;
	static char far retbuf[129];
	static char far storebuf[512];
	if (psz != NULL)
	{
		len = lstrlen(psz);
		_fmemcpy(storebuf, psz, len);
		i = start = 0;
	}
	i = 0;
	while (storebuf[i+start] != c && i+start < len)
	{
		retbuf[i] = storebuf[i+start];
		++i;
	}
	retbuf[i] = 0;
	start = start + i + 1;
	if (i == 0)
		return NULL;
	else
		return (retbuf);
}
int StrToInt(LPSTR lpstr)
{
	static int ret = 0;
	int ten = 1;
	int i;
	int len = lstrlen(lpstr);
	ret = 0;
	for (i = len - 1; i > -1; i--)
	{
		ret = ret + (ten * (lpstr[i] - '0'));
		ten = ten * 10;
	}
	return (ret);
}

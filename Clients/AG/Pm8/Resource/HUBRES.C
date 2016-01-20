#include <windows.h>

#ifdef  WIN32
#define _export
#endif

int FAR PASCAL LibMain (HANDLE hInst, WORD ds, WORD hs, LPSTR str)
{
/*
   if (hs > 0)
      UnlockData (0);
*/      
   return 1;      
}

int FAR PASCAL _export _WEP (int nParam)
{
   return 1 ;
}

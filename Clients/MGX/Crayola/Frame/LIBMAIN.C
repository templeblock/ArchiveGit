#include <windows.h>

HANDLE hDllInst, hInstAstral;

/************************************************************************/
BOOL FAR PASCAL _export WEP(nParam)  // no clean up required
/************************************************************************/
int nParam;
{
return( TRUE );
}


/************************************************************************/
int FAR PASCAL _export LibMain( hInstance, wDataSeg, wHeapSize, lpCmdLine )
/************************************************************************/
HANDLE hInstance;
WORD wDataSeg, wHeapSize;
LPSTR lpCmdLine;
{
hInstAstral = hDllInst = hInstance;
return( 1 );
}

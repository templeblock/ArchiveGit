#include "vsndmix.h"

int VSndMix::nObjCount = 0;

VSndMix::VSndMix()
{
	if (nObjCount == 0)
	{
		SndMixInit();
	}

	++nObjCount;

	// Create a name based on the number
	// This name will be used for memory map in Win32
	wsprintf(szName, "VSndMix%d", nObjCount);

	hWave = NULL;
	lSize = 0;
}

VSndMix::~VSndMix()
{

	// Free the wave data
	if (hWave)
		SndMixFreeData(hWave);

	--nObjCount;

	// If this was the last object, then free the sndmixer
	if (nObjCount == 0)
		SndMixFree();
}

BOOL VSndMix::Load(int nResourceId)
{
	HGLOBAL hResMem;
	LPBYTE lpWave;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// Find the wave resource
	if ( !(hResMem = (HGLOBAL)FindResource( hInstance, 
		MAKEINTRESOURCE(nResourceId), "WAVE" )) )
		return FALSE;

	// Load the resource
	if ( !(hResMem = LoadResource( hInstance, (HRSRC)hResMem )) )
		return FALSE;

	// Lock it
	lpWave = (LPBYTE)LockResource(hResMem);

	// Get the size
	lSize = SizeofResource(hInstance, (HRSRC)hResMem);  

	// Load the wave memory into 'special' memory that can
	// be mixed
	hWave = SndMixLoadData(szName, lpWave, lSize);

	// Free the resource from memory, we no longer need
	// it since it has been copied
	FreeResource(hResMem);

	if (!hWave)
		return FALSE;

	return TRUE;
}

BOOL VSndMix::Play(void)
{
	if (lSize == 0)
		return FALSE;

	return SndMixPlay( szName, lSize, 0, 1 );
}

BOOL VSndMix::Play(BOOL bLoop, int nPriority)
{
	WORD wFlags = 0;

	if (lSize == 0)
		return FALSE;

	if (bLoop)
		wFlags |= MIXFLAG_LOOP;

	return SndMixPlay( szName, lSize, wFlags, nPriority );
}

BOOL VSndMix::Stop(void)
{
	return SndMixStop( szName );
}

void VSndMix::Reset(void)
{
	SndMixReset();
}

void VSndMix::DebugInfo(void)
{
	SndMixDebugInfo();
}

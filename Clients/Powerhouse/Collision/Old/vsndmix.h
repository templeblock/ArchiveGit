#ifndef _VSndMix
#define _VSndMix

#include <windows.h>

extern "C" {
#include "sndmix\sndmix.p"
}

#define MAX_NAME_SIZE 40

class VSndMix {

public:
	VSndMix();
	~VSndMix();

	BOOL Load(int nResourceId);
	BOOL Play(void);
	BOOL Play(BOOL bLoop, int nPriority);
	BOOL Stop(void);

	static void Reset(void);
	static void DebugInfo(void);

private:
	static int nObjCount;	// Count of VSndMix objects

private:
	HANDLE hWave;				// Handle to wave memory
	long   lSize;				// Size of wave memory
	char szName[MAX_NAME_SIZE];	// Name of sound object (must be unique)
};

#endif
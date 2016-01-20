#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <string.h>
#include <io.h>
#include <assert.h>
#include <vfw.h>
#include "Data3D.h"
#include "mmiojunk.p"
#include "proto.h"
//extern "C" {
//#include "sndmix\sndmix.p"
//}

//#define DEBUG_READ
//#define DEBUG_DATA3D
//debug
static long lFrameCount = 0;
//

//////////////////////////////////////// AVI Index global variables
static AVIINDEXENTRY *g_pIndex = NULL;// Pointer to avi index
static DWORD g_dwNumIdx = 0;		  // Number of index records
////////////////////////////////////////

///////////////////////////////////////// Audio globals
static const int g_iAudBufSize = 2048; // Maximum audio buffer size
BYTE g_AudBuf[g_iAudBufSize];          // Audio buffer
/////////////////////////////////////////

////////////////////////////////////// 3D information read from the AVI file
static int nCurCommand = 0;			// Current command code
static int nCurPlayerZ = 0;			// Current player z position
static int nCurNumWalls = 0;		// Current number of walls
static int yCurGround = 0;			// Current y (ground) position of walls
static WALL CurWalls[nMaxWalls];	// Current walls
static int nCurNumEnemy = 0;		// Current number of enemies
static ENEMY CurEnemy[nMaxEnemies];	// Current enemies
//////////////////////////////////////

typedef struct {
    FOURCC ckid;          // chunk identifier 
    DWORD  cksize;        // chunk size
} CHNKTIP;

#define FOURCC_WB   mmioFOURCC('0', '1', 'w', 'b')
#define FOURCC_DC   mmioFOURCC('0', '0', 'd', 'c')
#define FOURCC_JK   mmioFOURCC('J', 'U', 'N', 'K')

// Local functions
static void StripPlus(LPSTR lpszDest, LPSTR lpszSrc);
static void MMIOSeek(MMIOINFO *pInfo, HFILE hFile, long lNewPos, long lChangeFlag);
static LRESULT MMIOJunkRead(MMIOINFO *pInfo, LPBYTE lpBuf, long lSize);
LRESULT FAR PASCAL MMIOJunkIOProc(LPSTR lpmmioinfo, UINT wMsg, LPARAM lParam1, LPARAM lParam2);


//***********************************************************************
BOOL MMIOJunkInstall()
//***********************************************************************
{
	LPMMIOPROC lpIoProc;

	lpIoProc = mmioInstallIOProc( MAKEFOURCC('A', 'V', 'I', ' '), 
		&MMIOJunkIOProc, MMIO_INSTALLPROC|MMIO_GLOBALPROC);

	if (lpIoProc)
		return TRUE;
	else
		return FALSE;
}

//***********************************************************************
BOOL MMIOJunkRemove()
//***********************************************************************
{
	LPMMIOPROC lpIoProc;

	lpIoProc = mmioInstallIOProc( MAKEFOURCC('A', 'V', 'I', ' '), 
		NULL, MMIO_REMOVEPROC);

	if (lpIoProc)
		return TRUE;
	else
		return FALSE;
}

//***********************************************************************
LRESULT FAR PASCAL MMIOJunkIOProc(LPSTR lpmmioinfo, UINT wMsg, LPARAM lParam1, LPARAM lParam2)
//***********************************************************************
{
	LRESULT lRes = MMSYSERR_NOERROR;
	MMIOINFO *pInfo = (MMIOINFO *)lpmmioinfo;

	switch(wMsg)
	{
	case MMIOM_READ:
		lRes = MMIOJunkRead(pInfo, (LPBYTE)lParam1, lParam2);
		break;

	case MMIOM_SEEK:
		{
			HFILE hFile = pInfo->adwInfo[0];
			MMIOSeek(pInfo, hFile, lParam1, lParam2);
		}
		break;

	case MMIOM_WRITE:
		break;

	case MMIOM_OPEN:
		{
			LPSTR lpszFileName = (LPSTR)lParam1;
			char szNewName[_MAX_PATH];

			// If just parsing the name...
			if (pInfo->dwFlags & MMIO_PARSE)
			{

				StripPlus(szNewName, lpszFileName);
				//lRes = TRUE;
			}

			// Else, assume we are to open the file
			else
			{
				// Get rid of the +abc stuff
				StripPlus(szNewName, lpszFileName);

				// Open the file and stuff the handle in user info
				HFILE hFile = _lopen(szNewName, OF_READ);
				pInfo->adwInfo[0] = hFile;
				pInfo->lDiskOffset = 0;
			}

			lFrameCount = 0;

			//pInfo->hmmio = hFile;
		}
		break;

	case MMIOM_CLOSE:
		{
		HFILE hFile = (HFILE)pInfo->adwInfo[0];
		if (hFile)
			_lclose(hFile);
		}
		break;
	}

	return lRes;
}


//***********************************************************************
void StripPlus(LPSTR lpszDest, LPSTR lpszSrc)
//***********************************************************************
{
	while(*lpszSrc != '+' && *lpszSrc != NULL)
		*lpszDest++ = *lpszSrc++;

	*lpszDest = NULL;

}

//***********************************************************************
void MMIOSeek(MMIOINFO *pInfo, HFILE hFile, long lNewPos, long lChangeFlag)
//***********************************************************************
{
	long lPos;
#ifdef DEBUG_SEEK
	char debug[80];
#endif

/***

	// Is this a game seek?
	if ((lAudioFrame = Video_JumpFrame()) != -1L)
	{
		long lIdx;
		int i;

#ifdef DEBUG_SEEK
		wsprintf(debug, "*** Seek to frame %ld ***\n", lAudioFrame);
		OutputDebugString(debug);
#endif

		//read preceeding 11 audio frames and send them to the audio
		// mixer
		//lAudioFrame -= 11;
		//if (lAudioFrame < 0)
		//	lIdx = (lAudioFrame + 11) * 2 + 1;
		//else
		//	lIdx = 22 + (lAudioFrame * 3) + 2;

		lIdx = lAudioFrame-11;
		if (lIdx < 0)
			lIdx = 0;


		SndMixAddBlockStart();

		for(i=0; i<11; i++)
		{
#ifdef DEBUG_SEEK
			wsprintf(debug, "%ld) %c%c%c%c off=%ld, size=%ld\n", lIdx,
												   (BYTE)g_pIndex[lIdx].ckid,
												   (BYTE)(DWORD)(g_pIndex[lIdx].ckid >> 8),
												   (BYTE)(DWORD)(g_pIndex[lIdx].ckid >> 16),
												   (BYTE)(DWORD)(g_pIndex[lIdx].ckid >> 24),
												   (long)g_pIndex[lIdx].dwChunkOffset,
												   (long)g_pIndex[lIdx].dwChunkLength);
			OutputDebugString(debug);
#endif
			// Make sure there is room in the audio buf before reading it
			if ( g_pIndex[lIdx].dwChunkLength < g_iAudBufSize)
			{
				CHNKTIP Chunk;

				lPos = _llseek(hFile, g_pIndex[lIdx].dwChunkOffset, 0);
				lNum = _hread(hFile, &Chunk, sizeof(Chunk));
				lNum = _hread(hFile, g_AudBuf, Chunk.cksize);
				if (lNum != HFILE_ERROR)
				{
					SndMixAddBlock(g_AudBuf, lNum);

				} // end hread check
			} // end aud buf check


			++lIdx;
			//++lAudioFrame;
			//if (lAudioFrame < 0)
			//	lIdx = (lAudioFrame + 11) * 2 + 1;
			//else
			//	lIdx = 22 + (lAudioFrame * 3) + 2;

		}
		SndMixAddBlockStop();
	}
***/
	// Do the real seek
	lPos = _llseek(hFile, lNewPos, lChangeFlag);
	if (lPos != -1)
		pInfo->lDiskOffset = lPos;

#ifdef DEBUG_SEEK
	wsprintf(debug,"SEEK %ld, pos=%ld\n", lPos, lChangeFlag);
	OutputDebugString(debug);
#endif
}

//***********************************************************************
LRESULT MMIOJunkRead(MMIOINFO *pInfo, LPBYTE lpBuf, long lSize)
//***********************************************************************
{
	HFILE hFile = pInfo->adwInfo[0];
	CHNKTIP *pChnkTip;

	// Make sure these are set to the default in case no
	// data is found
	nCurPlayerZ = 0;
	nCurNumWalls = 0;
	nCurNumEnemy = 0;

#ifdef DEBUG_READ
char debug[80];
wsprintf(debug, "--- Read %ld, at %ld\n", lSize, pInfo->lDiskOffset);
OutputDebugString(debug);
#endif

#ifdef DEBUG_DATA3D
int lDebOffset = pInfo->lDiskOffset;
#endif

	// Read the buffer
	long lNum = _hread(hFile, lpBuf, lSize);
	if (lNum != HFILE_ERROR)
	{
		pInfo->lDiskOffset += lNum;		
		pChnkTip = (CHNKTIP *)lpBuf;

		// Is this the index?
		if (lSize > 1000000)
		{
			g_pIndex = (AVIINDEXENTRY *)lpBuf;
			g_dwNumIdx = lSize / sizeof(AVIINDEXENTRY);

			//for(int i=0; i<20; i++)
			//{

			//wsprintf(debug, "%d) %c%c%c%c off=%ld size=%ld\n", i,
			//									   (BYTE)g_pIndex[i].ckid,
			//									   (BYTE)(DWORD)(g_pIndex[i].ckid >> 8),
			//									   (BYTE)(DWORD)(g_pIndex[i].ckid >> 16),
			//									   (BYTE)(DWORD)(g_pIndex[i].ckid >> 24),
			//									   (long)g_pIndex[i].dwChunkOffset,
			//									   (long)g_pIndex[i].dwChunkLength);
			//OutputDebugString(debug);
			//}
		}

//#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
//		( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
//		( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

		//if (g_pIndex)
		//{
		//	wsprintf(debug, "%c%c%c%c off=%ld size=%ld\n", (BYTE)g_pIndex->ckid,
		//										   (BYTE)(DWORD)(g_pIndex->ckid >> 8),
		//										   (BYTE)(DWORD)(g_pIndex->ckid >> 16),
		//										   (BYTE)(DWORD)(g_pIndex->ckid >> 24),
		//										   (long)g_pIndex->dwChunkLength);
		//	OutputDebugString(debug);
		//}
//typedef struct {
//    DWORD  ckid;
//    DWORD  dwFlags;
//    DWORD  dwChunkOffset;
//    DWORD  dwChunkLength;
//} AVIINDEXENTRY;
		// Is this a wave chunk?
		if (pChnkTip->ckid == FOURCC_WB)
		{
			//wsprintf(debug, "wave size = %ld, frame %ld\n", pChnkTip->cksize, lFrameCount++);
			//OutputDebugString(debug);
		}

		// Not a wave chunk, but there may be wave data in this chunk...let's find out
		else
		{
			//wsprintf(debug, "%c%c%c%c\n", *lpBuf, *(lpBuf+1), *(lpBuf+2), *(lpBuf+3));
			//OutputDebugString(debug);

			// Is this a video chunk?
			if (pChnkTip->ckid == FOURCC_DC)
			{
				
				// Offset to wave buffer (make sure even byte aligned)
				long lOffset = ((pChnkTip->cksize + sizeof(CHNKTIP)) + 1) & 0xfffffffe;
#ifdef DEBUG_DATA3D
lDebOffset += lOffset;
#endif

				// Set pointer to possible wave data
				lpBuf = lpBuf + lOffset;

				pChnkTip = (CHNKTIP *)lpBuf;

				// Is this a wave chunk?
				if (pChnkTip->ckid == FOURCC_WB)
				{
					//wsprintf(debug, "wave size = %ld, frame %ld\n", pChnkTip->cksize, lFrameCount++);
					//OutputDebugString(debug);

					// Offset to junk data
					lOffset = ((pChnkTip->cksize + sizeof(CHNKTIP)) + 1) & 0xfffffffe;

#ifdef DEBUG_DATA3D
lDebOffset += lOffset;
#endif
					// Set pointer to possible junk data
					lpBuf = lpBuf + lOffset;
					pChnkTip = (CHNKTIP *)lpBuf;

					if (pChnkTip->ckid == FOURCC_JK)
					{
						lpBuf += 8;
						char cTag = *lpBuf;
				
						// If 3d data in this junk
						if (cTag == '3')
						{
							int i;

#ifdef DEBUG_DATA3D
{
char debug[80];
lDebOffset += 8;
wsprintf(debug,"--- 3d data offset=%ld ---\n", lDebOffset);
OutputDebugString(debug);
}
#endif
							// Get Command code
							lpBuf += sizeof(cTag);
							nCurCommand = *((int *)lpBuf);

							// Get current player z position
							lpBuf += sizeof(nCurCommand);
							nCurPlayerZ = *((int *)lpBuf);

							// Get the number of walls
							lpBuf += sizeof(nCurPlayerZ);
							nCurNumWalls = *((int *)lpBuf);
							if (nCurNumWalls > nMaxWalls || nCurNumWalls < 0)
							{
#ifdef _DEBUG
								char debug[81];
								wsprintf(debug,"ERROR!!! Too many walls (%d)!\n", nCurNumWalls);
								OutputDebugString(debug);
#endif
								// Bad data, so we need to make it safe and bail
								goto GotBadDataNeedToBail;
							}
							assert(nCurNumWalls <= nMaxWalls);
							
							// Get the y (ground) position
							lpBuf += sizeof(nCurNumWalls);
							yCurGround = *((int *)lpBuf);

#ifdef DEBUG_DATA3D
{
char debug[80];
wsprintf(debug,"nCurPlayerZ=%d,nCurNumWalls=%d,yCurGround=%d\n", nCurPlayerZ, nCurNumWalls, yCurGround);
OutputDebugString(debug);
}
#endif
							// Get the walls (if any)
							lpBuf += sizeof(yCurGround);
							for (i=0; i<nCurNumWalls; i++)
							{
								memcpy( &CurWalls[i], lpBuf, sizeof(SWALL));
								lpBuf += sizeof(SWALL);
							}

							// Get the number of enemies
							nCurNumEnemy = *((int *)lpBuf);
							if (nCurNumEnemy >= nMaxEnemies || nCurNumEnemy < 0)
							{
#ifdef _DEBUG
								char debug[81];
								wsprintf(debug,"ERROR!!! Too many enemies (%d)!\n", nCurNumEnemy);
								OutputDebugString(debug);
								//DebugBreak();
#endif
								goto GotBadDataNeedToBail;
							}
							assert(nCurNumEnemy < nMaxEnemies);

#ifdef DEBUG_DATA3D
{
char debug[80];
wsprintf(debug,"nCurNumEnemy=%d\n", nCurNumEnemy);
OutputDebugString(debug);
}
#endif
							// Get the enemies (if any)
							lpBuf += sizeof(nCurNumEnemy);
							for (i=0; i<nCurNumEnemy; i++)
							{
								memcpy( &CurEnemy[i], lpBuf, sizeof(SENEMY));
								lpBuf += sizeof(SENEMY);
							}

							//wsprintf(debug, "3D Data!\n");
							//OutputDebugString(debug);
						}
					}
				}				
			}
			else
			{
				//char szChunk[5];
				//lstrcpyn(szChunk, (char *)pChnkTip, 4);
				//szChunk[4] = NULL;
				//wsprintf(debug, "chunk? = %s, frame %ld\n", szChunk, lFrameCount++);
				//OutputDebugString(debug);
			}
		}
	}
	return lNum;

GotBadDataNeedToBail:

	nCurNumWalls = 0;
	yCurGround = 0;
	nCurNumEnemy = 0;

	return lNum;

}

//***********************************************************************
void GetCommand(int *nCommand)
//***********************************************************************
{
	*nCommand = nCurCommand;
}

//***********************************************************************
void GetWalls(int *yGround, int *nNumWalls, WALL *pWalls)
//***********************************************************************
{
	// Return the walls for this frame
	*nNumWalls  = nCurNumWalls;
	*yGround = yCurGround;
	memcpy(pWalls, &CurWalls, sizeof(WALL)* (*nNumWalls));
}

//***********************************************************************
void GetEnemy(int *nNumEnemy, ENEMY *pEnemy)
//***********************************************************************
{
	*nNumEnemy  = nCurNumEnemy;
	memcpy(pEnemy, &CurEnemy, sizeof(ENEMY)* (*nNumEnemy));
}


//***********************************************************************
int GetPlayerZ(void)
//***********************************************************************
{
	return nCurPlayerZ;
}
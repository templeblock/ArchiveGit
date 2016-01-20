#include "Vid3d.h"

Vid3dFrame::Vid3dFrame(LPVIDEO lpVideo)
{
	lpVid = lpVideo;

	// Get total number of frames
	lNumFrames = MCIGetLength(lpVideo->hDevice);

	pFrames = NULL;
}

Vid3dFrame::~Vid3dFrame()
{
	if (pFrames)
	{
		// First free up all the enemy boxes
		for (long i=0; i<lNumFrames; i++)
		{
			if (pFrames[i].pEnemy)
				FreeUp(pFrames[i].pEnemy);
		}

		// Now free up the frames
		FreeUp(pFrames);
		pFrames = NULL;
	}
}


BOOL Vid3dFrame::Load(char *szFile)
{ 
	FNAME szFileName;

	if ( !GetApp()->FindContent( szFile, szFileName ) )
		return FALSE;

	if (pFrames)
		FreeUp(pFrames);

	// Allocate frame info array
	pFrames = (FRAMEINFO3D *)AllocX( lNumFrames * sizeof(FRAMEINFO3D), GMEM_ZEROINIT );

	// open file
	HFILE hFile = _lopen(szFileName, OF_READ);
	int iNum;

	// For all frames
	for (long i=0; i<lNumFrames; i++)
	{
		// Read in frame info
		if (_lread(hFile, &pFrames[i], sizeof(FRAMEINFO3D)) != sizeof(FRAMEINFO3D))
			break;

		iNum = pFrames[i].nNumEnemy;

		// If there are enemy boxes for this frame
		if (iNum > 0)
		{
			// Alocate enemy box(s)
			pFrames[i].pEnemy = (ENEMYBOX *)AllocX( iNum * sizeof(ENEMYBOX), GMEM_ZEROINIT );

			// Read in enemy box info
			if (_lread(hFile, &pFrames[i].pEnemy, iNum * sizeof(ENEMYBOX)) != iNum * sizeof(ENEMYBOX))
				break;
		}
	}

	_lclose(hFile);

	return TRUE;
}

BOOL Vid3dFrame::Save(char *szFile)
{ 
	FNAME szFileName;

	if (!pFrames)
		return FALSE;

	if ( !GetApp()->FindContent( szFile, szFileName ) )
		return FALSE;

	// open file
	HFILE hFile = _lopen(szFileName, OF_WRITE);
	int iNum;

	// For all frames
	for (long i=0; i<lNumFrames; i++)
	{
		// Write frame info
		if (_lwrite(hFile, (LPCSTR)&pFrames[i], sizeof(FRAMEINFO3D)) != sizeof(FRAMEINFO3D))
			break;

		iNum = pFrames[i].nNumEnemy;

		// If there are enemy boxes for this frame
		if (iNum > 0)
		{
			// Write enemy box(s) info
			if (_lwrite(hFile, (LPCSTR)&pFrames[i].pEnemy, iNum * sizeof(ENEMYBOX)) != iNum * sizeof(ENEMYBOX))
				break;
		}
	}

	_lclose(hFile);

	return TRUE;
}

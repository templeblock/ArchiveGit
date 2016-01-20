#include <windows.h>
#include "proto.h"
#include "Score.h"

/*
CScore::CScore()
{
	m_lpOffScreen = NULL;
	m_pAnimator = NULL;
}
*/

BOOL CScore::Save()
{
	BOOL bRet = TRUE;

	#ifdef _DEBUG
	// Get the current user name
	char szName[MaxUserName];	
	ULONG cbName = MaxUserName;
	if (!GetUserName(szName, &cbName))
	{
		Print("Could not retrieve your user name. Please make sure you are logged into the network.");
		bRet = FALSE;
	}
	else
	{
		//Print("UserName=%s, Score=%ld", (LPSTR)szName, m_lScore);
		Print("Score = %ld", m_lScore);
		bRet = TRUE;

		//WriteHighScore(szName, m_lScore);
	}
	#else

	Print("Score = %ld", m_lScore);
	bRet = TRUE;

	#endif

	return bRet;	
}

BOOL CScore::Retrieve()
{
	BOOL bRet = FALSE;

	return bRet;
}

void CScore::WriteHighScore(char *szName, long lScore)
{
	char *szFile = "\\\\100 acre woods\\Scores\\score.dat";

	int iNumRecs = 0;
	UINT uRet;

	// Open score file
	HFILE hFile = _lopen(szFile, OF_READWRITE|OF_SHARE_COMPAT);
	if (hFile == -1)
	{
		hFile = _lcreat(szFile, 0);
		if (hFile == -1)
			return;
	}
	else
	{
		// Get number of records
		uRet = _lread(hFile, &iNumRecs, sizeof(iNumRecs));
		if (uRet == HFILE_ERROR)
			iNumRecs = 0;
	}

	// Find last score
	BOOL bFound = FALSE;
	SCORE_REC Rec;
	long lPos = -1;
	for(int i=0; i<iNumRecs; i++)
	{
		lPos = _llseek(hFile, 0, FILE_CURRENT);
		_lread(hFile, &Rec, sizeof(Rec));
		if (lstrcmp(Rec.szName, szName) == 0)
		{
			bFound = TRUE;
			break;
		}
	}

	// If not found then add the record
	if (!bFound)
	{
		++iNumRecs;
		_llseek(hFile, 0, FILE_BEGIN);
		_lwrite(hFile, (LPCSTR)&iNumRecs, sizeof(iNumRecs));
		_llseek(hFile, 0, FILE_END);
		lstrcpy(Rec.szName, szName);
		Rec.lScore = lScore;
		_lwrite(hFile, (LPCSTR)&Rec, sizeof(Rec));
	}
	else
	{
		// If current score is greater
		if (Rec.lScore < lScore)
		{
			Rec.lScore = lScore;

			// Write the new score
			_llseek(hFile, lPos, FILE_BEGIN);
			_lwrite(hFile, (LPCSTR)&Rec, sizeof(Rec));
		}
	}

	// close file
	_lclose(hFile);
}

/****
void CScore::InitDisplay(HWND hWnd)
{
	STRING szString;
	FNAME szDib;
	PTOON pToon;

	// Create the offscreen dib
	m_lpOffScreen = new COffScreen(GetApp()->m_hPal);

	// if we don't have enough memory to instantiate
	// the offscreen, then we are done
	if ( !m_lpOffScreen )
			return;

	// load the read-only background dib if it has not been done already
	if ( !m_lpOffScreen->GetReadOnlyDIB() )
	{
		m_lpOffScreen->Load( GetWindowInstance(hWnd), IDC_SCORE_BACKGROUND );

		// copy the colors to the stage dib to be used with WinG
		m_lpOffScreen->CopyColors();
	}

	// if we were unsuccessful, then clean up and bail out
	if ( !m_lpOffScreen->GetReadOnlyDIB() )
	{
		delete m_lpOffScreen;
		m_lpOffScreen = NULL;
		return;
	}

	// Copy bits from the read-only dib to the WinG bitmap
	m_lpOffScreen->CopyBits();

	// setup animator object to be used with sprite engine
	// if it has not been setup already. 
	if (!m_pAnimator)
		m_pAnimator = new CAnimator(hWnd, m_lpOffScreen);
}
****/

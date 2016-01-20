#include <windows.h>
#include <mmsystem.h>
#include "proto.h"
#include "control.h"
#include "commonid.h"
#include "parser.h"
#include "limits.h"

class CVTableParser : public CParser
{
public:
	CVTableParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};	

/***********************************************************************/
BOOL CVTableParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
/***********************************************************************/
{
	BOOL fError = TRUE; // always keep trying on errors
	BOOL bSubShot, bError;
	LPSTR lpValue;
	long l, limit;
	LPSHOT lpShots;
	
	lpShots = (LPSHOT)dwUserData;
	lpShots += nIndex;
	
	/***********************************************************************/
	// The 'shot-id' command comes first
	/***********************************************************************/
	if (!lpShots->lShotID)
		{
		if ( !(l = GetShotID(lpEntry, lpKey)) )
			return(fError);
	
		lstrcpy(lpShots->szShotName, lpKey);
		lpShots->lShotID = l;
		lpShots->aShotID = (ATOM)l;
	
		if (nValues != 2 && nValues != 3)
			{
			Print("'%s'\n Expected start and end (and disk) values", lpEntry);
			return(fError);
			}
	
		/***********************************************************************/
		// Get the start frame value
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad start frame at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lStartFrame = l;
	
		/***********************************************************************/
		// Get the end frame value
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad end frame at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lEndFrame = l;
		if ( lpShots->lEndFrame < lpShots->lStartFrame )
			{
			Print("'%s'\n End frame is less than the start frame at '%s'", lpEntry, lpValue);
			}
	
		/***********************************************************************/
		// Get the disk value
		/***********************************************************************/
		if ( nValues != 3 )
			lpShots->wDisk = 1;
		else
			{
			lpValue = GetNextValue(&lpValues);
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad disk number at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->wDisk = (WORD)l;
			}
		}
	else
	/***********************************************************************/
	// The end command is second
	/***********************************************************************/
	if (!lpShots->lEndShot)
	{
		/***********************************************************************/
		// See if we have the 'end' command
		/***********************************************************************/
		if (lstrcmpi(lpKey, "end"))
			{
			Print("'%s'\n Expected end command at '%s'", lpEntry, lpKey);
			return(fError);
			}
		// Got an 'end' command
	
		/***********************************************************************/
		// See if 'loop' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		if ( lstrcmpi( lpValue, "loop" ) )
			goto SkipLoop;
		// Got a 'loop' command
	
		/***********************************************************************/
		// Get numeric argument; could be #frames or #seconds
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad loop value at '%s'", lpEntry, lpValue);
			return(fError);
			}
		// Got a numeric argument; could be #frames or #seconds
	
		/***********************************************************************/
		// See if 'frames' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		if ( !lpValue || lstrcmpi( lpValue, "frames" ) )
			goto SkipFrames;
		// Got a 'frames' command
		limit = lpShots->lEndFrame - lpShots->lStartFrame;
		if ( l > limit ) l = limit;
		lpShots->lEndLoop |= (l<<16); // Stuff the number of frames into the HIWORD
	
		/***********************************************************************/
		// See if a second numeric argument; could be #seconds
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues);
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			goto SkipLoop;
		// Got a numeric argument
	
		/***********************************************************************/
		// See if 'seconds' command is specified
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
	SkipFrames:
		// At this point, assume the numeric argument is seconds
		lpShots->lEndLoop |= LOWORD(l); // Stuff the number of seconds into the LOWORD
		if ( !lpValue || lstrcmpi( lpValue, "seconds" ) )
			goto SkipLoop;
		// Got a 'seconds' command
	
		/***********************************************************************/
		// Get 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
	SkipLoop:
		if ( !lpValue ) // if none specified, goto ourselves
			l = lpShots->lShotID;
		else
		{
			if (! (l = GetShotID(lpEntry, lpValue)) )
				return(fError);
		}
		lpShots->lEndShot = l;
	
		/***********************************************************************/
		// See if 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if ( lpValue && !lstrcmpi( lpValue, "count" ) )
			{
			// Got a 'count' command
	
			/***********************************************************************/
			// Get 'count' argument; # of shot to select from
			/***********************************************************************/
			lpValue = GetNextValue(&lpValues); 
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lEndCount = l;
			lpValue = GetNextValue(&lpValues); 
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue )
			{
			Print("'%s'\n Extra 'end' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'left' or 'leftsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "leftsub")) || !lstrcmpi(lpKey, "left") )
		{ // Got a 'left' command
	
		lpShots->bLeftIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get left 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lLeftShot = l;
	
		/***********************************************************************/
		// See if left goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipLeftCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get left goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lLeftCount = l;
	
		/***********************************************************************/
		// See if left 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipLeftCount:
		if ( lpValue )
			{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
				{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lLeftHotspot = l;
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
			{
			Print("'%s'\n Extra 'left' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'right' or 'rightsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "rightsub")) || !lstrcmpi(lpKey, "right") )
		{ // Got a 'right' command
	
		lpShots->bRightIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get right 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lRightShot = l;
	
		/***********************************************************************/
		// See if right goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipRightCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get right goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lRightCount = l;
	
		/***********************************************************************/
		// See if right 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipRightCount:
		if ( lpValue )
			{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
				{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lRightHotspot = l;
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
			{
			Print("'%s'\n Extra 'right' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'up' or 'upsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "upsub")) || !lstrcmpi(lpKey, "up") )
		{ // Got a 'up' command
	
		lpShots->bUpIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get up 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lUpShot = l;
	
		/***********************************************************************/
		// See if up goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipUpCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get up goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lUpCount = l;
	
		/***********************************************************************/
		// See if up 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipUpCount:
		if ( lpValue )
			{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
				{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lUpHotspot = l;
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
			{
			Print("'%s'\n Extra 'up' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'down' or 'downsub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "downsub")) || !lstrcmpi(lpKey, "down") )
		{ // Got a 'down' command
	
		lpShots->bDownIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get down 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lDownShot = l;
	
		/***********************************************************************/
		// See if down goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipDownCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get down goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lDownCount = l;
	
		/***********************************************************************/
		// See if down 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipDownCount:
		if ( lpValue )
			{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
				{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lDownHotspot = l;
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
			{
			Print("'%s'\n Extra 'down' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'home' or 'homesub' command is specified
	/***********************************************************************/
	if ( (bSubShot = !lstrcmpi(lpKey, "homesub")) || !lstrcmpi(lpKey, "home") )
		{ // Got a 'home' command
	
		lpShots->bHomeIsSubShot = bSubShot;
	
		/***********************************************************************/
		// Get home 'goto' argument; the shot id
		/***********************************************************************/
		lpValue = GetNextValue(&lpValues); 
		if (! (l = GetShotID(lpEntry, lpValue)) )
			return(fError);
		lpShots->lHomeShot = l;
	
		/***********************************************************************/
		// See if home goto 'count' command is specified; default is 0
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		if ( !lpValue || lstrcmpi( lpValue, "count" ) )
			goto SkipHomeCount;
		// Got a 'count' command
	
		/***********************************************************************/
		// Get home goto 'count' argument; # of shot to select from
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
		l = latol( lpValue, &bError );
		if ( bError || l < 0 )
			{
			Print("'%s'\n Bad shot count at '%s'", lpEntry, lpValue);
			return(fError);
			}
		lpShots->lHomeCount = l;
	
		/***********************************************************************/
		// See if home 'hot' argument; row and column range
		/***********************************************************************/
		lpValue = GetNextValue( &lpValues );
	SkipHomeCount:
		if ( lpValue )
			{
			BOOL bError;
			l = latol( lpValue, &bError );
			if ( bError )
				{
				Print("'%s'\n Bad hotspot at '%s'", lpEntry, lpValue);
				return(fError);
				}
			lpShots->lHomeHotspot = l;
			}
	
		/***********************************************************************/
		// Check for excess arguments
		/***********************************************************************/
		if ( lpValue = GetNextValue( &lpValues ) )
			{
			Print("'%s'\n Extra 'home' argument at '%s'", lpEntry, lpValue);
			return(fError);
			}
		}
	else
	/***********************************************************************/
	// See if 'flag' command is specified
	/***********************************************************************/
	if (!lstrcmpi(lpKey, "flag"))
		{ // Got a 'flag' command
	
		/***********************************************************************/
		// Get 'flag' argument; any number of flags from 1 to 32
		/***********************************************************************/
		while (--nValues >= 0)
			{
			lpValue = GetNextValue( &lpValues );
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad flag value at '%s'", lpEntry, lpValue);
				break;
				}
			lpShots->lFlags |= FLAGBIT(l);
			}
		}
	else
	/***********************************************************************/
	// See if 'value' command is specified
	/***********************************************************************/
	if (!lstrcmpi(lpKey, "value"))
		{ // Got a 'value' command
	
		/***********************************************************************/
		// Get 'value' argument; any number of values to be OR'd together
		/***********************************************************************/
		while (--nValues >= 0)
			{
			lpValue = GetNextValue( &lpValues );
			l = latol( lpValue, &bError );
			if ( bError || l < 0 )
				{
				Print("'%s'\n Bad value at '%s'", lpEntry, lpValue);
				break;
				}
			lpShots->lFlags |= l;
			}
		}
	else
	/***********************************************************************/
	// Check for extra commands
	/***********************************************************************/
		{
		Print("'%s'\n Bad command at the end of line at '%s'", lpEntry, lpKey);
		return(fError);
		}
	return(TRUE);
}

/***********************************************************************/
static void Video_GetShotTableFileName(ITEMID id, LPSTR lpFileName)
/***********************************************************************/
{
	GetModuleFileName( GetApp()->GetInstance(), lpFileName, MAX_FNAME_LEN );
	StripFile( lpFileName );
//	STRING szName;
//	wsprintf(szName, "scene%03d.tbl", id);
//	lstrcat(lpFileName, szName);
	lstrcat( lpFileName, "videojmp.tbl" );
}

/***********************************************************************/
static LPSHOT Video_LoadShotTable(ITEMID id, LPINT lpNumShots)
/***********************************************************************/
{
	HFILE hFile;
	HPTR hpData;
	UINT uBytes;
	LPSHOT lpShots;
	FNAME szFileName;
	FNAME szModuleName;
	int iResult;

	Video_GetShotTableFileName(id, szFileName);
	GetModuleFileName(GetApp()->GetInstance(), szModuleName, sizeof(szModuleName));
	 
	if (CompareFileTimes(szModuleName, szFileName, &iResult) && (iResult > 0))
	{
		FileDelete(szFileName);
		return(NULL);
	}

	hFile = _lopen(szFileName, OF_READ);
	if ( hFile == HFILE_ERROR )
		return(NULL);
	DWORD dwSize = _llseek(hFile, 0L, 2);
	_llseek(hFile, 0L, 0);
	*lpNumShots = (int)(dwSize / (DWORD)sizeof(SHOT));
	// Allocate the shot table
	if ( !(hpData = (HPTR)AllocX( (long)*lpNumShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
	{
		_lclose(hFile);
		return(NULL);
	}

	lpShots = (LPSHOT)hpData;
	while (dwSize)
	{
		#ifdef WIN32
		uBytes = INT_MAX;
		#else
		uBytes = 32768; // 32K chunks to we don't span 64K
		#endif
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lread(hFile, (LPVOID)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error write to file: '%s'", (LPSTR)szFileName);
			FreeUp(lpShots);
			lpShots = NULL;
			break;
		}
		dwSize -= uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);
	#ifndef WIN32
	long lRem = 65536L % sizeof(SHOT);
	if (lRem) // extra amount to prevent straddling
	{
		--*lpNumShots; // there is really one less shot
		LPTR lp = (LPTR)lpShots; 
		lp += lRem; // bump pointer to prevent straddling
		lpShots = (LPSHOT)lp;
	}
	#endif
	return(lpShots);
}

/***********************************************************************/
BOOL Video_DumpShotTable(ITEMID id, LPSHOT lpShots, int iNumShots)
/***********************************************************************/
{     
	HFILE hFile;
	FNAME szFileName;
	HPTR hpData;
	UINT uBytes;

	Video_GetShotTableFileName(id, szFileName);

	hFile = _lcreat(szFileName, 0);
	if ( hFile == HFILE_ERROR )
		return(FALSE);
	hpData = (HPTR)lpShots;
	#ifndef WIN32
	long lRem = 65536L % sizeof(SHOT);
	if (lRem) // extra amount to prevent segment straddling
	{
		++iNumShots; // one more structure needed for straddling
		hpData -= lRem; // get pointer back to start of buffer
	}
	#endif
	DWORD dwSize = (DWORD)sizeof(SHOT) * (DWORD)iNumShots;
	while (dwSize)
	{
		#ifdef WIN32
		uBytes = INT_MAX;
		#else
		uBytes = 32768; // 32K chunks to we don't span 64K
		#endif
		if (uBytes > dwSize)
			uBytes = (UINT)dwSize;
		if (_lwrite(hFile, (LPCSTR)hpData, uBytes) == HFILE_ERROR)
		{
			Print("Error write to file: '%s'", szFileName);
			_lclose(hFile);
			return(FALSE);
		}
		dwSize -= (DWORD)uBytes;
		hpData += uBytes;
	}
	_lclose(hFile);
	return(TRUE);
}

/***********************************************************************/
void Video_CheckDumpShotTable(ITEMID id, LPSHOT lpShots, int iNumShots)
/***********************************************************************/
{
	FNAME szFileName;

	Video_GetShotTableFileName(id, szFileName);
	if (!FileExistsExpand(szFileName, NULL))
		Video_DumpShotTable(id, lpShots, iNumShots);
}

/***********************************************************************/
LPSHOT Video_GetShotData( ITEMID id, LPINT lpiNumShots, LPBOOL lpfRemap )
/***********************************************************************/
{
	HRSRC hResource;
	HGLOBAL hData;
	LPSTR lpShotData;
	LPSHOT lpShots, lpShot;
	DWORD dwSize;
	int iNumShots, iCount;
	static BOOL bInit;
	
	if (lpShots = Video_LoadShotTable(id, &iNumShots))
	{
		CVTableParser parser(NULL);
		lpShot = lpShots;
		iCount = iNumShots;
		while (--iCount >= 0)
		{
			lpShot->aShotID = (ATOM)parser.MakeShotID(lpShot->szShotName,!bInit);
			++lpShot;
		}
		*lpfRemap = FALSE;
	}
	else
	{
		if ( !(hResource = FindResource( GetApp()->GetInstance(), MAKEINTRESOURCE(id), RT_RCDATA )) )
			return( NULL );
		if ( !(dwSize = SizeofResource( GetApp()->GetInstance(), hResource )) )
			return( NULL );
		if ( !(hData = LoadResource( GetApp()->GetInstance(), hResource )) )
			return( NULL );
		if ( !(lpShotData = (LPSTR)LockResource( hData )) )
			{
			FreeResource( hData );
			return( NULL );
			}
	
		CVTableParser parser(lpShotData);
		iNumShots = parser.GetNumEntries();
		// Allocate the shot table
		#ifdef WIN32
		if ( !(lpShots = (LPSHOT)AllocX( (long)iNumShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
			{
			FreeResource( hData );
			return( NULL );
			}
		#else
		{
			LPTR lp;
			long lRem;
			int iShots;
	
			// For Win16 we make sure none of the structures straddle
			// a segment boundary.  This is done by allocating one more
			// structure than is needed and incrementing the initial
			// pointer so that none straddle.
			lRem = 65536L % sizeof(SHOT); // amount to increment pointer
			iShots = iNumShots;
			if (lRem)
				++iShots; // allocate one more structure than needed
	
			if ( !(lp = AllocX( (long)iShots * sizeof(SHOT), GMEM_ZEROINIT|GMEM_SHARE )) )
				{
				FreeResource( hData );
				return( NULL );
				}
			lp += lRem; // increment pointer to prevent straddling
			lpShots = (LPSHOT)lp;
		}
		#endif
		
		// Process the resource data and stuff the shot table
		parser.ParseTable((DWORD)lpShots);
		
		// Free up the resource memory
		UnlockResource( hData );
		FreeResource( hData );
		*lpfRemap = TRUE;
	}
	
	if ( lpiNumShots )
		*lpiNumShots = iNumShots;
	
	bInit = YES;
	return( lpShots );
}

#include <windows.h>
#include "proto.h"
#include "parser.h"

#define IS_SEPARATOR(c) ( ((c) <= ' ') || ((c) == ',') || ((c) == '='))
#define IS_SPACE(c) ( ((c) <= ' ') )

/***********************************************************************/
CParser::CParser(LPSTR lpTableData)
/***********************************************************************/
{
	m_lpTableData = (HPTR)lpTableData;
	m_nEntries = CountNumEntries();
}

/***********************************************************************/
int CParser::CountNumEntries()
/***********************************************************************/
{
	int iCount;
	HPTR lpData = m_lpTableData;

	if ( !lpData )
		return( 0 );

	// Find the start point for the next call
	iCount = 0;
	while ( *lpData )
	{
		if ( (*lpData) == ';' )
			iCount++;
		lpData++;
	}

	return( iCount );
}

/***********************************************************************/
BOOL CParser::InitTable(DWORD dwUserData)
/***********************************************************************/
{
	InitAtomTable( 0 );
	InitAtomTable( GetNumEntries() );
	return(TRUE);
}

/***********************************************************************/
BOOL CParser::ParseTable(DWORD dwUserData)
/***********************************************************************/
{
	BOOL fRet = TRUE;
	LPSTR lpString, lpEntryString, lpKey, lpValues;
	BYTE szString[512], szEntryString[512];
	int nIndex, nValues;
	HPTR lpData;

	if (!InitTable(dwUserData))
		return(FALSE);

	nIndex = 0;
	lpData = m_lpTableData;
	while ( GetEntryString( &lpData, (LPSTR)szEntryString, sizeof(szEntryString) ) )
	{
		// Keep a copy of the Entry string for error messages
		lpString = (LPSTR)szString;
		lpEntryString = (LPSTR)szEntryString;
		hmemcpy( lpString, lpEntryString, min(sizeof(szString),lstrlen(lpEntryString)+1) );

		// get key
		while (lpKey = GetKey( &lpEntryString ))
		{
			// get value
			if (lpValues = GetValues( &lpEntryString, &nValues ))
			{
				// handle this entry
				if (!HandleKey(lpString, lpKey, lpValues, nValues, nIndex, dwUserData))
				{
					fRet = FALSE;
					break;
				}
			}
		}

		// process next entry
		++nIndex;
	}
	return(fRet);
}

/***********************************************************************/
BOOL CParser::GetEntryString( HPTR FAR * lppData, LPSTR lpReturn, int iLength )
/***********************************************************************/
{
	HPTR lpData;

	if ( !lppData )
		return( NO );

	if ( !(lpData = *lppData) )
		return( NO );

	// Skip white space
	while ( (*lpData) && IS_SEPARATOR(*lpData) )
		lpData++;

	// If at the end of the data, get out
	if ( !(*lpData) )
		return( NO );

	// Copy bytes till we find the start point for the next call
	while ( *lpData && ((*lpData) != ';') )
	{
		if ( --iLength > 0 )
			*lpReturn++ = *lpData;
		lpData++;
	}

	// Null terminate the return string
	*lpReturn = '\0';

	// Update the location for the next call
	if ( *lpData )
		lpData++;
	*lppData = lpData;

	return( YES );
}

/***********************************************************************/
LPSTR CParser::GetKey( LPSTR far * lppData )
/***********************************************************************/
{
	LPSTR lpData, lpReturn;

	if ( !lppData )
		return( NULL );

	if ( !(lpData = *lppData) )
		return( NULL );

	// Skip white space
	while ( (*lpData) && IS_SEPARATOR(*lpData) )
		lpData++;

	// If at the end of the data, get out
	if ( !(*lpData) )
		return( NULL );

	// Save the return value
	lpReturn = lpData;

	// Find the start point for the next call
	while ( !IS_SEPARATOR(*lpData) )
		lpData++;

	// Null terminate the return string and advance to the next call
	if ( *lpData )
		*lpData++ = '\0';

	// Update the location for the next call
	*lppData = lpData;

	return( lpReturn );
}

/***********************************************************************/
LPSTR CParser::GetValues( LPSTR far * lppData, LPINT lpNumValues )
/***********************************************************************/
{
	LPSTR lpData, lpReturn;
	BOOL	fNewValue;
	
	*lpNumValues = 0;
	if ( !lppData )
		return( NULL );

	if ( !(lpData = *lppData) )
		return( NULL );

	// Skip white space
	while ( (*lpData) && IS_SEPARATOR(*lpData) )
		lpData++;

	// If at the end of the data, get out
	if ( !(*lpData) )
		return( NULL );

	// Save the return value
	lpReturn = lpData;

	// Find the start point for the next call
	fNewValue = TRUE;
	while ( *lpData && *lpData != ',')
	{
		if (IS_SPACE(*lpData))
			fNewValue = TRUE;
		else
		if (fNewValue)
		{
			*lpNumValues += 1;
			fNewValue = FALSE;
		}
		lpData++;
	}

	// Null terminate the return string and advance to the next call
	if ( *lpData )
		*lpData++ = '\0';

	// Update the location for the next call
	*lppData = lpData;

	if (*lpNumValues)
		return( lpReturn );
	else
		return( NULL );
}

/***********************************************************************/
SHOTID CParser::MakeShotID( LPSTR lpString, BOOL bDupCheck )
/***********************************************************************/
{
	ATOM atom;
	 
	if ( !lstrcmpi( lpString, "/prev" ) )
		return( SHOT_PREV );
	if ( !lstrcmpi( lpString, "/test" ) )
		return( SHOT_TEST );
	if ( !lstrcmpi( lpString, "/stop" ) )
		return( SHOT_STOP );
	if ( atom = FindAtom(lpString) )
	{
		if ( bDupCheck )
			Print("Duplicate shot id '%s'", lpString);
		return( atom );
	}
	return( (SHOTID)AddAtom(lpString) );
}

/***********************************************************************/
SHOTID CParser::GetShotID(LPSTR lpEntry, LPSTR lpValue)
/***********************************************************************/
{
	long l = 0;

	if (!lpValue)
		Print("'%s'\n No shot id specified", lpEntry);
	else
	if (*lpValue != '/')
		Print("'%s'\n Shot ids must begin with '/'; '%s'", lpEntry, lpValue);
	else
	if ( !(l = MakeShotID(lpValue, NO)) )
		Print("'%s'\n Expected shot id at '%s'", lpEntry, lpValue);
	return(l);
}

/***********************************************************************/
SHOTID CParser::ShotCheck( SHOTID lShot )
/***********************************************************************/
{
	STRING szString;

	if ( !lShot || lShot == SHOT_PREV || lShot == SHOT_TEST )
		return( lShot );

	if ( lShot < 0 )
		return( -lShot );

	GetAtomName((ATOM)lShot, szString, sizeof(STRING) );
	Print("Can't find shot %s", (LPTR)szString );
	return( 0 );
}

/***********************************************************************/
BOOL CParser::GetFilename(LPSTR lpFilename, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	LPSTR lpValue;

	if (nValues != 1)
	{
		Print("'%s'\n Expected filename parameter", lpEntry);
		return(FALSE);
	}
	lpValue = GetNextValue(&lpValues);
	lstrcpy(lpFilename, lpValue);
	return(TRUE);
}

/***********************************************************************/
BOOL CParser::GetString(LPSTR lpString, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	LPSTR lpValue;

	lpString[0] = '\0';
	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		lstrcat(lpString, lpValue);
		if (nValues)
			lstrcat(lpString, " ");
	}
	return(TRUE);
}

/***********************************************************************/
BOOL CParser::GetRect(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	long l;
	LPSTR lpValue;
	LPINT lpRectValue;
	BOOL bError;

	if (nValues != 4)
	{
		Print("'%s'\n Invalid rectangle '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	lpRectValue = (LPINT)lpRect;
	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		*lpRectValue++ = (int)l;
	}
	return(TRUE);
}

/***********************************************************************/
BOOL CParser::GetPoint(LPPOINT lpPoint, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	long l;
	LPSTR lpValue;
	LPINT lpPtValue;
	BOOL bError;

	if (nValues != 2)
	{
		Print("'%s'\n Invalid point '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	// even though the idiots at Microsoft made POINT use
	// INTs in 16-bit and LONGs in 32-bit, using an INT will
	// work fine since it naturally grows from 16 to 32 bits.
	lpPtValue = (LPINT)lpPoint;
	while (--nValues >= 0)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		*lpPtValue++ = (int)l;
	}
	return(TRUE);
}

/***********************************************************************/
BOOL CParser::GetInt(LPINT lpInt, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	long l;
	LPSTR lpValue;
	BOOL bError;

	if (nValues != 1)
	{
		Print("'%s'\n Invalid integer '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	lpValue = GetNextValue(&lpValues);
	l = latol(lpValue, &bError);
	if ( bError || l < 0 )
	{
		Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
		return(FALSE);
	}
	*lpInt = (int)l;
	return(TRUE);
}



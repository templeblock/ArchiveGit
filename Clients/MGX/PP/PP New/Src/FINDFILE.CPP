// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®TP0¯®BT0¯*/
#include "pp.h"
#include "id.h"

// Prototypes
static BOOL Traverse( P_STR lpRootDirectory );

static STRING szFindDirectory;
static LPSTR lpFindFile;
static int depth;

/************************************************************************/
BOOL FindFile( LPSTR lpRootDirectory, LPSTR lpFile, int MaxDepth, LPSTR lpReturnedDirectory )
/************************************************************************/
{
BOOL fRet;
char szRootDirectory[255];

szFindDirectory[0] = '\0';
depth = MaxDepth;
lpFindFile = lpFile;
lstrcpy( szRootDirectory, lpRootDirectory );
fRet = Traverse( szRootDirectory );
lstrcpy( lpReturnedDirectory, szFindDirectory );
return( fRet );
}


/************************************************************************/
static BOOL Traverse( P_STR lpRootDirectory )
/************************************************************************/
{
BOOL fRet;
FINDDATA dir;

lstrcat( lpRootDirectory, "*.*" );
fRet = Dos_FindFirst( lpRootDirectory, ATTRIB_ALL, &dir );
stripfile( lpRootDirectory );
if ( !fRet )
	return( NO );

while ( 1 )
	{
	if ( (dir.dwAttrib & ATTRIB_SUBDIR) && dir.szName[0] != '.' )
		{ // If this is a subdirectory
		if ( --depth )
			{
			lstrcat( lpRootDirectory, dir.szName );
			lstrcat( lpRootDirectory, "\\" );
			fRet = Traverse( lpRootDirectory );
			lpRootDirectory[ lstrlen(lpRootDirectory)-1 ] = '\0';
			stripfile( lpRootDirectory );
			}
		depth++;
		if ( !fRet )
			return( YES );
		}
	else
	if ( dir.dwAttrib & (ATTRIB_NORMAL | ATTRIB_ARCHIVE) )
		{
		if ( StringsEqual( dir.szName, lpFindFile ) )
			{
			lstrcpy( szFindDirectory, lpRootDirectory );
			return( YES );
			}
		}
	if ( !Dos_FindNext( &dir ) )
		break;
	}

return( NO );
}




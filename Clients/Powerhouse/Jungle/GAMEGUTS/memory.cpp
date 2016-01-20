#include "windows.h"
#include "proto.h"

typedef struct _alloctable
{
	LPTR lp;
	DWORD dwSize;
} ATABLE;

static ATABLE at[100];
static int iMaxEntry;

/************************************************************************/
static void TablePrint(void)
/************************************************************************/
{
for ( int i=0; i<100; i++ )
	{
	if ( !at[i].lp )
		continue;
	Debug( "%d: %ld bytes", i, at[i].dwSize );
	}

Debug( "\n" );
}

/************************************************************************/
void TableAdd( LPTR lpMemory )
/************************************************************************/
{
int i;

for ( i=0; i<100; i++ )
	{
	if ( !at[i].lp )
		break;
	}

if ( i >= 100 )
	return;

at[i].lp = lpMemory;
at[i].dwSize = GlobalSize(GlobalPtrHandle(lpMemory));

Debug( "Add slot %d\r", i );

// Print the table
TablePrint();
}

/************************************************************************/
void TableRemove( LPVOID lpMemory )
/************************************************************************/
{
int i;

for ( i=0; i<100; i++ )
	{
	if ( at[i].lp == lpMemory )
		break;
	}

if ( i >= 100 )
	return;

at[i].lp = NULL;
at[i].dwSize = NULL;

Debug( "Free slot %d\r", i );

// Print the table
TablePrint();
}

/************************************************************************/
LPTR Alloc( long lCount )
/************************************************************************/
{
return( AllocX( lCount, 0 ) );
}


/************************************************************************/
LPTR AllocX( long lCount, WORD wFlags )
/************************************************************************/
{
LPTR	 lpMemory;
HGLOBAL	 hMem;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

// Allocate the memory from the global heap - NOT limited to 64K
if ( !(hMem = GlobalAlloc(GMEM_MOVEABLE | wFlags, lCount)) )
	return(NULL);

if ( !(lpMemory = (LPTR)GlobalLock(hMem)) )
	GlobalFree(hMem);

#ifdef _DEBUG_MEMORY
	TableAdd( lpMemory );
#endif

return( lpMemory );
}

/************************************************************************/
LPTR AllocExtend( LPTR lpMemory, long lCount )
/************************************************************************/
{
LPTR lpNewMemory;
HGLOBAL hMem, hNewMem;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

if ( !(hMem = GlobalPtrHandle(lpMemory)) )
	return(NULL);
if ( !(hNewMem = GlobalReAlloc(hMem, lCount, GMEM_MOVEABLE)) )
	return(NULL);

if ( !(lpNewMemory = (LPTR)GlobalLock(hNewMem)) )
	GlobalFree(hNewMem);

#ifdef _DEBUG_MEMORY
	TableAdd( lpMemory );
#endif

return( lpNewMemory );
}

/************************************************************************/
void FreeUp( LPVOID lpMemory )
/************************************************************************/
{
HGLOBAL hMem;

if ( !lpMemory )
	return;

if ( !(hMem = GlobalPtrHandle(lpMemory)) )
	return;

#ifdef _DEBUG_MEMORY
	TableRemove( lpMemory );
#endif

GlobalUnlock(hMem);
GlobalFree(hMem);
}


/************************************************************************/
DWORD AvailableMemory( void )
/************************************************************************/
{
return( GetFreeSpace(0) );
}


/************************************************************************/
BOOL AllocLines( LPPTR lpList, int nLines, int iWidth, int iDepth )
/************************************************************************/
{
long lCount;
LPTR lp;

if ( nLines <= 0 )
	nLines = 1;

if (!iDepth)
	iDepth = 1;

iWidth *= iDepth;
iWidth = ((iWidth + 15) / 16) * 16;
lCount = (long)nLines * iWidth;
if ( !(lp = Alloc( lCount )) )
	return( NO );
while ( --nLines >= 0 )
	{
	*lpList++ = lp;
	lp += iWidth;
	}
return( YES );
}

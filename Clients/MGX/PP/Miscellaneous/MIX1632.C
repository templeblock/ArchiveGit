#include <windows.h>
#include <windowsx.h>
#include <toolhelp.h>

#define BLOCK_SIZE 0x60000

void FAR PASCAL memcpy32( DWORD destSeg, DWORD destOffset,
                          DWORD srcSeg, DWORD srcOffset,
                          DWORD cbCopy );

int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow )
{
    TIMERINFO ti;
    DWORD beforeTickCountFast, afterTickCountFast;
    DWORD beforeTickCountSlow, afterTickCountSlow, i;
    BYTE __huge *hpvSrc;
    BYTE __huge *hpvDest;
    char szBuffer[256];
    
    hpvSrc = GlobalAllocPtr( GMEM_MOVEABLE, BLOCK_SIZE );
    hpvDest = GlobalAllocPtr( GMEM_MOVEABLE, BLOCK_SIZE );
    if ( !hpvSrc || !hpvDest )
    {
        MessageBox(0, "Couldn't allocate src/dest segs", 0, MB_OK);
        return 0;
    }

    // Call the 32 bit function once without timing it to let it switch
    // to a 32 bit segment.
    memcpy32( SELECTOROF(hpvDest), OFFSETOF(hpvDest),
              SELECTOROF(hpvSrc), OFFSETOF(hpvSrc),
              BLOCK_SIZE );

    ti.dwSize = sizeof(ti);
    TimerCount(&ti);
    beforeTickCountFast = ti.dwmsThisVM;

    // Now call the 32 bit function again, this time timing it.
    memcpy32( SELECTOROF(hpvDest), OFFSETOF(hpvDest),
              SELECTOROF(hpvSrc), OFFSETOF(hpvSrc),
              BLOCK_SIZE );
          
    TimerCount( &ti );
    afterTickCountFast = ti.dwmsThisVM;
    
    TimerCount(&ti);
    beforeTickCountSlow = ti.dwmsThisVM;

    for ( i=0; i < BLOCK_SIZE; i++)
        *hpvDest++ = *hpvSrc++;
        
    TimerCount( &ti );
    afterTickCountSlow = ti.dwmsThisVM;
    
    wsprintf(szBuffer,
            "32 bit move:       %lu milliseconds\r\n"
            "huge pointer move: %lu milliseconds",
            afterTickCountFast - beforeTickCountFast,
            afterTickCountSlow - beforeTickCountSlow);
    MessageBox( 0, szBuffer, "MIX1632 - Matt Pietrek 1994", MB_OK );

    return 0;
}

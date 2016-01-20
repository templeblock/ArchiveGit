//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

typedef void (NEAR PASCAL* EXPLOSION)(HWND,int,int);

/***********************************************************************/
static void Pause (WORD wMilliseconds)
/***********************************************************************/
{
    DWORD dwTick = GetTickCount() + wMilliseconds;

    while (GetTickCount() < dwTick)
        ;
}

/***********************************************************************/
static void NEAR PASCAL Checkerboard (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define SIZE 40
    HDC  hDC;
    RECT rClient;
    RECT rInvert;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    for (y = 0; y < rClient.bottom; y += SIZE)
    {
        rInvert.top    = y;
        rInvert.bottom = y + SIZE;
        for (x = (y / SIZE % 2) ? SIZE : 0; x < rClient.right; x += SIZE * 2)
        {
            rInvert.left  = x;
            rInvert.right = x + SIZE;
            InvertRect (hDC,&rInvert);
        }
    }
    for (x = 0; x < 10; x++)
    {
        InvertRect (hDC,&rClient);
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL ClockwiseBox (HWND hWnd,int x,int y)
/***********************************************************************/
{
    HDC  hDC;
    int  nCells;
    RECT rCell;
    RECT rClient;
    RECT rDone;
    RECT rTemp;

    GetClientRect (hWnd,&rClient);
    nCells = (max (rClient.right,rClient.bottom) * 2 + 19) / 20;
    nCells *= nCells;
    rCell.left   = x - 10;
    rCell.right  = x + 10;
    rCell.top    = y - 10;
    rCell.bottom = y + 10;
    hDC = GetDC (hWnd);
    InvertRect (hDC,&rCell);
    nCells--;
    rDone = rCell;
    while (nCells > 0)
    {
        while (rCell.bottom > rDone.top)        // go north
        {
            rCell.top    -= 20;
            rCell.bottom -= 20;
            if (rCell.bottom > rClient.top)
                InvertRect (hDC,&rCell);
            nCells--;
        }
        rTemp.top = rCell.top;
        while (rCell.left < rDone.right)        // go east
        {
            rCell.left   += 20;
            rCell.right  += 20;
            if (rCell.left < rClient.right)
                InvertRect (hDC,&rCell);
            nCells--;
        }
        rTemp.right = rCell.right;
        while (rCell.top < rDone.bottom)        // go south
        {
            rCell.top    += 20;
            rCell.bottom += 20;
            if (rCell.top < rClient.bottom)
                InvertRect (hDC,&rCell);
            nCells--;
        }
        rTemp.bottom = rCell.bottom;
        while (rCell.right > rDone.left)        // go west
        {
            rCell.left   -= 20;
            rCell.right  -= 20;
            if (rCell.right > rClient.left)
                InvertRect (hDC,&rCell);
            nCells--;
        }
        rTemp.left = rCell.left;
        rDone = rTemp;
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL ConcentricSquares (HWND hWnd,int x,int y)
/***********************************************************************/
{
    HDC  hDC;
    RECT rClient;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    rSquare.left   = x;
    rSquare.right  = x;
    rSquare.top    = y;
    rSquare.bottom = y;
    hDC = GetDC (hWnd);
    while (rSquare.left > rClient.left || rSquare.right  < rClient.right ||
           rSquare.top  > rClient.top  || rSquare.bottom < rClient.bottom)
    {
        InflateRect (&rSquare,20,20);
        InvertRect (hDC,&rSquare);
        Pause (10);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL HorzBars (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define HBAR 40
    HDC  hDC;
    RECT rClient;
    RECT rInvert;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    rInvert.left  = 0;
    rInvert.right = rClient.right;
    for (y = 0; y < rClient.bottom; y += HBAR * 2)
    {
        rInvert.top    = y;
        rInvert.bottom = y + HBAR;
        InvertRect (hDC,&rInvert);
    }
    for (x = 0; x < 10; x++)
    {
        InvertRect (hDC,&rClient);
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL HorzMarch (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define XSIZE 60
    #define YSIZE 60
    HBRUSH hBrush;
    HDC    hDC;
    RECT   rClient;
    RECT   rSquare;

    GetClientRect (hWnd,&rClient);
    hBrush = (HBRUSH) GetStockObject (WHITE_BRUSH);
    hDC = GetDC (hWnd);
    for (y = 0; y < rClient.bottom; y += YSIZE)
    {
        rSquare.top    = y;
        rSquare.bottom = y + YSIZE;
        for (x = 0; x < rClient.right; x += XSIZE)
        {
            rSquare.left  = x;
            rSquare.right = x + XSIZE;
            InvertRect (hDC,&rSquare);
            Pause (1);
            InvertRect (hDC,&rSquare);
            FillRect (hDC,&rSquare,hBrush);
        }
    }
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL HorzSqueeze (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define SQUEEZE 40
    HDC  hDC;
    int  z;
    RECT rClient;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    rSquare.left  = rClient.left;
    rSquare.right = rClient.right;
    for (y = 0, z = rClient.bottom; y < rClient.bottom; y += SQUEEZE, z -= SQUEEZE)
    {
        rSquare.top    = y;
        rSquare.bottom = y + SQUEEZE;
        InvertRect (hDC,&rSquare);
        rSquare.top    = z - SQUEEZE;
        rSquare.bottom = z;
        InvertRect (hDC,&rSquare);
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL Ink (HWND hWnd,int x,int y)
/***********************************************************************/
{
    HBRUSH  hBrush;
    HDC     hDC;
    int     i;
    LPDWORD lpCode;
    RECT    rClient;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    hBrush = (HBRUSH) SelectObject (hDC,GetStockObject (BLACK_BRUSH));
    lpCode = (LPDWORD)Pause;    // pick a function early in code segment!
    for (i = 0; i < 200; i++)
    {
        int z;

        x = LOWORD(lpCode[i]) % rClient.right;
        y = HIWORD(lpCode[i]) % rClient.bottom;
        for (z = 1; z < 10; z++)
            Ellipse (hDC,x - z,y - z,x + z,y + z);
    }
    SelectObject (hDC,hBrush);
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL Noise (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define INFLATE 4
    HDC  hDC;
    int  i,z;
    RECT rClient;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    for (i = 0; i < 10; i++)
    {
        for (y = 20; y < rClient.bottom; y += 20)
        {
            z = (y % 40) ? 0 : 10;
            for (x = 20; x < rClient.right; x += 20)
            {
                rSquare.left   = x + z;
                rSquare.right  = x + z;
                rSquare.top    = y;
                rSquare.bottom = y;
                InflateRect (&rSquare,i * INFLATE,i * INFLATE);
                InvertRect (hDC,&rSquare);
            }
        }
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL Polo (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define DELTA 20
    HDC  hDC;
    RECT rClient;
    RECT rInvert;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    rSquare = rClient;
    for (x = 0; x < rClient.right; x += DELTA)
    {
        InflateRect (&rSquare,-DELTA,-DELTA);
        rInvert.left   = 0;
        rInvert.right  = rSquare.left;
        rInvert.top    = rSquare.bottom;
        rInvert.bottom = rClient.bottom;
        InvertRect (hDC,&rInvert);
        rInvert.left   = rSquare.right;
        rInvert.right  = rClient.right;
        rInvert.top    = 0;
        rInvert.bottom = rSquare.top;
        InvertRect (hDC,&rInvert);
        Pause (3);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL Raindrops (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define INFLATE 4
    HDC  hDC;
    int  i;
    RECT rClient;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    for (i = 0; i < 10; i++)
    {
        for (y = 30; y < rClient.bottom; y += 60)
        {
            for (x = 30; x < rClient.right; x += 60)
            {
                rSquare.left   = x;
                rSquare.right  = x;
                rSquare.top    = y;
                rSquare.bottom = y;
                InflateRect (&rSquare,i * INFLATE,i * INFLATE);
                InvertRect (hDC,&rSquare);
            }
        }
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL VertBars (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define VBAR 40
    HDC  hDC;
    RECT rClient;
    RECT rInvert;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    rInvert.top    = 0;
    rInvert.bottom = rClient.bottom;
    for (x = 0; x < rClient.right; x += VBAR * 2)
    {
        rInvert.left  = x;
        rInvert.right = x + VBAR;
        InvertRect (hDC,&rInvert);
    }
    for (x = 0; x < 10; x++)
    {
        InvertRect (hDC,&rClient);
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL VertSqueeze (HWND hWnd,int x,int y)
/***********************************************************************/
{
    #define SQUEEZE 40
    HDC  hDC;
    int  z;
    RECT rClient;
    RECT rSquare;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    rSquare.top    = rClient.top;
    rSquare.bottom = rClient.bottom;
    for (x = 0, z = rClient.right; x < rClient.right; x += SQUEEZE, z -= SQUEEZE)
    {
        rSquare.left  = x;
        rSquare.right = x + SQUEEZE;
        InvertRect (hDC,&rSquare);
        rSquare.left  = z - SQUEEZE;
        rSquare.right = z;
        InvertRect (hDC,&rSquare);
        Pause (1);
    }
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
static void NEAR PASCAL XRay (HWND hWnd,int x,int y)
/***********************************************************************/
{
    HDC  hDC;
    int  i;
    RECT rClient;

    GetClientRect (hWnd,&rClient);
    hDC = GetDC (hWnd);
    for (i = 0; i < 20; i++)
        InvertRect (hDC,&rClient);
    FillRect (hDC,&rClient,(HBRUSH) GetStockObject (WHITE_BRUSH));
    ReleaseDC (hWnd,hDC);
}

/***********************************************************************/
void Explode (HWND hWnd,UINT uiExplosion,int x,int y)
/***********************************************************************/
{
    static EXPLOSION Explosion[] = {
        ConcentricSquares       ,       // 0 = default

        ConcentricSquares       ,       // 1
        ClockwiseBox            ,       // 2
        HorzMarch               ,       // 3
        XRay                    ,       // 4
        Noise                   ,       // 5
        Raindrops               ,       // 6
        Ink                     ,       // 7
        HorzSqueeze             ,       // 8
        VertSqueeze             ,       // 9
        Polo                    ,       // 10
        Checkerboard            ,       // 11
        HorzBars                ,       // 12
        VertBars                ,       // 13
        Ink                     ,       // 14
        Raindrops               ,       // 15
        Noise                   ,       // 16
        XRay                    ,       // 17
        HorzMarch               ,       // 18
        ClockwiseBox            ,       // 19
        ConcentricSquares       ,       // 20
    };
    static UINT uiMaxExp = sizeof Explosion / sizeof Explosion[0] - 1;

    if (uiExplosion > uiMaxExp)         // max explosion
        uiExplosion = 0;
    Explosion[uiExplosion] (hWnd,x,y);
}

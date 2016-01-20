/*****************************************************************************
 *
 *  PlayRLE.C - RLE display code
 *
 ****************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


#include <windows.h>
#include "dib.h"

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2
#define RLE_RUN     3

//
//  DecodeRle   - 'C' version
//
//  Play back a RLE buffer into a DIB buffer
//
//  returns
//      none
//
void WINAPI DecodeRle(LPBITMAPINFOHEADER lpbi, HPBYTE pb, HPBYTE prle)
{
    BYTE    cnt;
    BYTE    b;
    WORD    x;
    WORD    dx,dy;
    WORD    wWidthBytes;

    wWidthBytes = (WORD)lpbi->biWidth+3 & ~3;

    x = 0;

    for(;;)
    {
        cnt = *prle++;
        b   = *prle++;

        if (cnt == RLE_ESCAPE)
        {
            switch (b)
            {
                case RLE_EOF:
                    return;

                case RLE_EOL:
                    pb += wWidthBytes - x;
                    x = 0;
                    break;

                case RLE_JMP:
                    dx = (WORD)*prle++;
                    dy = (WORD)*prle++;

                    pb += (DWORD)wWidthBytes * dy + dx;
                    x  += dx;

                    break;

                default:
                    cnt = b;
                    x  += cnt;
                    while (cnt-- > 0)
                        *pb++ = *prle++;

                    if (b & 1)
                        prle++;

                    break;
            }
        }
        else
        {
            x += cnt;

            while (cnt-- > 0)
                *pb++ = b;
        }
    }
}

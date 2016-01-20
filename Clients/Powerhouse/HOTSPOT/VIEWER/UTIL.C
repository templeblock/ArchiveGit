/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
/******************
    
    util.c --
    
    Utility functions for use in DLL: FindToken, StrToInt
    
******************/ 

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>

#include "hotspot.h"

/*********************************************************************************************
    FUNCTION:   FindToken(LPSTR, char)
    
    PURPOSE:    Like C library strtok() function; 
                accepts one char instead of a string of chars                
*********************************************************************************************/
LPSTR FindToken(LPSTR psz, char c)
{
    static int len;
    int i = 0;
    static int start = 0;
    static char far retbuf[129];
    static char far storebuf[512];
    
    if (psz != NULL)
        {
        len = lstrlen(psz);
        _fmemcpy(storebuf, psz, len);
        i = start = 0;
        }
    i = 0;
    while (storebuf[i+start] != c && i+start < len)
        {   
        retbuf[i] = storebuf[i+start];        
        ++i;
        }
    retbuf[i] = 0;
    start = start + i + 1;
    if (i == 0)
        return NULL;
    else        
        return (retbuf);
}

/*********************************************************************************************
    FUNCTION:   StrToInt(LPSTR)
    
    PURPOSE:    Converts a LPSTR to an int; like C library atol() function
*********************************************************************************************/
int StrToInt(LPSTR lpstr)
{
    static int ret = 0;    
    int ten = 1;    
    int i;            
    int len = lstrlen(lpstr);

    ret = 0;      
    for (i = len - 1; i > -1; i--)
        {
        ret = ret + (ten * (lpstr[i] - '0'));
        ten = ten * 10;
        }
    return (ret);
}



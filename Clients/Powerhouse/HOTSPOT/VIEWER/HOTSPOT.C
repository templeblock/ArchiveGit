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
/*
	hotspot.c:
		DeleteHotspot -- deletes a hotspot from linked list
		DeleteHotspotList -- deletes them all
		AddHotspot -- adds a hotspot to linked list
		InitHotspots -- reads hotspots from HOT file into linked list
*/

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>
#include <direct.h>

#include "hotspot.h"

char szMovieInfo[] = "MovieInfo";


/*
	DeleteHotspot -- give it a pointer to a hotspot structure and
		a pointer to a movieinfo structure, and it will delete the
		hotspot from the movieinfo's hotspot list.  Minor bug: 
		If you give it a hotspot for a different movieinfo,
		it will free it anyway, but if you give it a hotspot and
		a NULL movieinfo, it won't.  The list is doubly linked, so
 		it just switches some pointers.
*/
void DeleteHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot)
{
    PHOTSPOT pPrev, pNext;    
    
    if (!pMovieInfo || !pHotspot)
        return;

    pPrev = pHotspot->pPrev;
    pNext = pHotspot->pNext;
    
    if (pPrev) {
        pPrev->pNext = pNext;
    } else {
        pMovieInfo->pHotspotList = pNext;
    }
    if (pNext) {
        pNext->pPrev = pPrev;
    }

    if (pHotspot->pszCommand)
        FREE(pHotspot->pszCommand);

    if (pHotspot->pszHotspotID)
        FREE(pHotspot->pszHotspotID);
                
    FREE(pHotspot);    
    pHotspot = NULL;
}

/*
	DeleteHotspotList --
		deletes a list of hotspots in a movieinfo -- faster that using
		DeleteHotspot because it doesn't twiddle pointers -- just
		marches down the next pointers and frees everything.
*/
void DeleteHotspotList(PMOVIEINFO pMovieInfo)
{
    PHOTSPOT pHotspot, pNext;
    
    pHotspot = pMovieInfo->pHotspotList;
    while (pHotspot) {
        pNext = pHotspot->pNext;
        DeleteHotspot(pMovieInfo, pHotspot);
        pHotspot = pNext;
    }
    pMovieInfo->pHotspotList = NULL;
}

/*
	AddHotspot -- adds a hotspot (which you give it) to the front of
		a hotspot list in the movieinfo (which you also give it).
		The hotspot should be allocated using ALLOCATE -- see hotspot.h
*/
void AddHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot)
{    

    if (pMovieInfo->pHotspotList) {
        pMovieInfo->pHotspotList->pPrev = pHotspot;
        pHotspot->pNext = pMovieInfo->pHotspotList;
    }
    pMovieInfo->pHotspotList = pHotspot;
}


/* 
	InitHotspots -- you give it a pointer to a movie structure and a
		filename, and it adds a hotspot list to the movie structure from
		the data in the file.  The file is a HOT file, and is stored
		in a binary format: there's a tag indicating a .HOT file, then
		the number of hotspots.  Then it loops through, reading six numbers
		(the rect and the start/end frames) and two strings (the hostpotID
		and the viewer command) for each one.  Strings of length 0 are
		transformed into NULLs.
*/
BOOL InitHotspots(PMOVIEINFO pMovieInfo, LPSTR pszHotFile)
{
	HFILE hFile;
	char szBuf[256];
	int hotcount, i;
	PHOTSPOT pHotspot;
	
    hFile = _lopen(pszHotFile,READ);
    if (hFile == HFILE_ERROR)
    {
    	MessageBox(GetFocus(),pszHotFile,"Can't read this file:",MB_OK);
    }
   
	_lread(hFile,szBuf,2); 
	if (szBuf[0] != 'H' || szBuf[1] != 'H')
	{
		MessageBox(GetFocus(),"File is not a hotspot file","Out of luck",MB_OK);
	}
	_lread(hFile,&hotcount,sizeof(hotcount));
        
    for (i=0;i<hotcount;i++)
    {
        int x;
		if (! (pHotspot = (PHOTSPOT)ALLOCATE(sizeof(HOTSPOT))))
 		{
			MessageBox(GetFocus(),"Unable to allocate HOTSPOT structure",
			"Big Problem Here",MB_OK);
		}
       	
        _lread(hFile,&pHotspot->rc.left,sizeof(pHotspot->rc.left));
        _lread(hFile,&pHotspot->rc.top,sizeof(pHotspot->rc.top));
        _lread(hFile,&pHotspot->rc.right,sizeof(pHotspot->rc.right));
        _lread(hFile,&pHotspot->rc.bottom,sizeof(pHotspot->rc.bottom));
        
    	_lread(hFile,&x,sizeof(x));
    	if (x)
    	{
    		_lread(hFile,szBuf,x); szBuf[x] = '\0';
            pHotspot->pszHotspotID = (LPSTR) ALLOCATE(lstrlen(szBuf) + 1);
            lstrcpy(pHotspot->pszHotspotID, szBuf);                
    	}
    	else pHotspot->pszHotspotID = NULL;
        
    	_lread(hFile,&x,sizeof(x)); szBuf[x] = '\0';
    	if (x) _lwrite(hFile,pHotspot->pszCommand,x);
    	if (x)
    	{
    		_lread(hFile,szBuf,x);
            pHotspot->pszCommand = (LPSTR) ALLOCATE(lstrlen(szBuf) + 1);
            lstrcpy(pHotspot->pszCommand, szBuf);                
    	}
     	else pHotspot->pszCommand = NULL;

        _lread(hFile,&pHotspot->BeginFrame,sizeof(pHotspot->BeginFrame));
        _lread(hFile,&pHotspot->EndFrame,sizeof(pHotspot->EndFrame));
        _lread(hFile,&pHotspot->OnClick,sizeof(pHotspot->OnClick));
        _lread(hFile,&pHotspot->ToFrame,sizeof(pHotspot->ToFrame));
        AddHotspot(pMovieInfo, pHotspot);
    }
    _lclose(hFile);
    return TRUE;
 }

// $Workfile: tinycash.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/tinycash.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/23/98 2:40p Johno
// Created from dib2file
//
#include "stdafx.h"
#include "tinycash.h"

CStorageFileTinyCache::CStorageFileTinyCache(LPCSTR  FileName, WORD BufferSize /*1024*/)
   : StorageFile(FileName)
{
   ErrorCode = ERRORCODE_NotInitialized;
   Position = 0;
   pBuffer = NULL;
	mBufferSize = BufferSize;
}

CStorageFileTinyCache::~CStorageFileTinyCache (void)
{
   FlushBuffer ();
   
   if (pBuffer != NULL)
      delete [] pBuffer;
}

// This is a write cache
void
CStorageFileTinyCache::Put(BYTE *p, WORD size)
{
   BYTE  *buffer;

   if (((buffer = GetBuffer()) == NULL) ||(size >= mBufferSize * 4)) // If the write is a lot bigger than our buffer
   {                                //  just write it out
      FlushBuffer();               
      ERRORCODESavenThrow(INHERITED::write((LPVOID)p, size));
      return;
   }
   // If the write is bigger than our buffer, write it out in buffer chunks
   while(mBufferSize < size + Position)     // 5 + 1 = 6 > 4
   {
      WORD NewSize = mBufferSize - Position; // NewSize = 3 = 4 - 1
      if (NewSize > 0)
         memcpy(&buffer [Position], p, NewSize);
      
      Position += NewSize;
      FlushBuffer();
      size = size - NewSize;                    // size = 2 = 5 - 3 
      p += NewSize;
   }
   // save the remaining bytes
   if (size > 0)
   {
      memcpy(&buffer [Position], p, size);     
      Position += size;
   }
}

void
CStorageFileTinyCache::FlushBuffer (void)
{
   if (pBuffer != NULL)
   {
      if (Position > 0)
      {
         WORD  w = Position;
         Position = 0;
         ERRORCODESavenThrow (INHERITED::write ((LPVOID)pBuffer, w));
      }
   }
}

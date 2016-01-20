#include "stdafx.h"
#include "file.h"
/* 
   This is a simple disk write cache. This class is best used with code 
	that often make many small writes - this caches them for speed by postponing 
	OS calls until required. The methods like 'seek' are overridden, and the
   cache flushed, so that the base class will report the correct file pointer.
   Any other file pointer dependant method will malfunction unless this is done.
*/
class CStorageFileTinyCache : public StorageFile
{
	INHERIT(CStorageFileTinyCache, StorageFile)
public:
	CStorageFileTinyCache(LPCSTR FileName, WORD BufferSize = 1024);
   
	virtual
	~CStorageFileTinyCache (void);

   ERRORCODE 
   write (LPVOID ptr, ST_DEV_IO_SIZE size)
   {
      Put ((BYTE *)ptr, size);

      return ErrorCode;
   }

   ERRORCODE	
   read (LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL)
   {
      FlushBuffer ();
      ERRORCODESavenThrow (INHERITED::read (ptr, size, size_read));

      return ErrorCode;
   }

   ERRORCODE	
   seek (ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
   {
      FlushBuffer ();
      ERRORCODESavenThrow (INHERITED::seek (position, mode));

      return ErrorCode;
   }

	ERRORCODE	
   tell (ST_DEV_POSITION far *position)
   {
      FlushBuffer ();
      ERRORCODESavenThrow (INHERITED::tell (position));

      return ErrorCode;
   }

   ERRORCODE 
   flush ()
   {
      FlushBuffer ();
      ERRORCODESavenThrow (INHERITED::flush ());

      return ErrorCode;
   }

   ERRORCODE
   GetErrorCode (void)
   {
      return ErrorCode;
   }
protected:
   ERRORCODE   ErrorCode;
   WORD        Position;
   BYTE        *pBuffer;
	WORD			mBufferSize;

   void
   ERRORCODESavenThrow (ERRORCODE ec)
   {
      if (ERRORCODESave (ec) != ERRORCODE_None)
         ThrowErrorcodeException (ErrorCode);
   }

   ERRORCODE
   ERRORCODESave (ERRORCODE ec)
   {
      ErrorCode = ec;
      return ec;
   }
   // Allocate a buffer if we need one
   BYTE*
   GetBuffer (void)
   {
      TRY
      {
         if (pBuffer == NULL)
         {
            Position = 0;
            pBuffer = new BYTE [mBufferSize];
         }
      }
      END_TRY
      return pBuffer;
   }

   void
   Put (BYTE *p, WORD size);

   void
   FlushBuffer (void);
};

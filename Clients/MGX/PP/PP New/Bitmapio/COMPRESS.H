/*®FD1¯®PL1¯®TP0¯®BT0¯®RM240¯*/
#ifndef __compress
#define __compress

#include "imgutil.h"
#include "tiffio.h"

typedef struct	{
	int code;
	int chr;
	int first;
	int next;
} CTENTRY;
typedef CTENTRY far *LPCTENTRY;

typedef struct	{
   CFile*		pFile;
   int	      CurrentCode;
   int         CodeLengthBump;
   int         NextCodeIndex;
   LPCTENTRY   CodeTable;
   LPTR	      Outbuf;
   LPTR        lpOutbuf;
   LPTR        lpOutLast;
   LPTR	      Inbuf;
   LPTR        lpInbuf;
   LPTR        lpInLast;
   DWORD	      Accum;
   DWORD       CodesWritten;
   BYTE	      nAccumBits;
   BYTE        CodeLength;
   BYTE        fError;
   int         iClrCode;
   int         iEoiCode;
   int         iCodeLength;
   int         iCodeLenBumpFudge;
   int         iMaxCode;
   BOOL        fIncomplete;
   int         CodeToFinish;
} LZW_STUFF;
typedef LZW_STUFF far *LPLZW_STUFF;

LPLZW_STUFF DecompressLZW( CFile *pFile, LPTIFFHEADER lpTH, LPTR lpOut, int OutCount, LPLZW_STUFF lpLZW );
LPLZW_STUFF DecompressLZW_GIF( CFile *pFile, LPTR lpOut, int OutCount, int codeSize, LPLZW_STUFF lpLZW );
LPLZW_STUFF CompressLZW( CFile *pFile, LPTR lpIn, int InCount, LPLZW_STUFF lpLZW );
LPLZW_STUFF CompressLZW_GIF ( CFile *pFile, LPTR lpIn, int InCount, int codeSize, LPLZW_STUFF lpLZW );

#endif


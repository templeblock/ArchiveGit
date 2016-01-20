/*
// LZW.H
// 
// Definitions for the LZW reading.
//
// $Header: /PM8/App/LZW.H 1     3/03/99 6:07p Gbeddow $
//
// $Log: /PM8/App/LZW.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:10   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:44   JAY
// Initial revision.
// 
//    Rev 1.1   07 Jul 1994 11:27:42   JAY
// Added a device pointer to avoid collisoins.
// 
//    Rev 1.0   16 Dec 1993 16:26:02   JAY
//  
*/

#ifndef __LZW_H__
#define __LZW_H__

FARCLASS LZWParser
{
private:
	VOIDPTR data_block;
	ULONG skip;
	LPBYTE buffer;
	USHORT size;

	VOID near write_current_string(VOID);
	StorageDevicePtr m_pDevice;

public:
	LZWParser(StorageDevicePtr device, SHORT bit_width, BOOL dir);
	~LZWParser();
	VOID reset(VOID);
	ERRORCODE read(ULONG skip_size, LPBYTE read_buffer, USHORT read_size);
	BOOL valid(VOID)
		{ return data_block != NULL; }
};

typedef LZWParser far *LZWParserPtr;

#endif						// #ifndef __LZW_H__

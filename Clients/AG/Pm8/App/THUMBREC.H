/*
// Thumbnail record.
//
// $Workfile: THUMBREC.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/THUMBREC.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:16   Fred
// Initial revision.
// 
//    Rev 1.3   08 Aug 1996 18:10:28   Jay
// JPEG, etc!
// 
//    Rev 1.2   07 Aug 1996 17:50:10   Jay
//  
// 
//    Rev 1.1   10 May 1996 16:03:40   Jay
// New compression method
// 
//    Rev 1.0   14 Mar 1996 13:31:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:42   FRED
// Initial revision.
// 
//    Rev 2.5   14 Jun 1995 07:50:24   JAY
// Passes full RGB colors while drawing (PAL_COLORS didn't work).
// 
//    Rev 2.4   09 Jun 1995 10:49:28   JAY
// Compression of thumbnail.
// 
//    Rev 2.3   06 Jun 1995 16:55:00   JAY
// Project previews in the album.
// 
//    Rev 2.2   06 Jun 1995 10:11:54   JAY
// Moved more stuff over from CATALOG.
// 
//    Rev 1.1   15 Nov 1994 08:37:50   JAY
// Made static variables near to prevent multiple data segments.
// 
//    Rev 1.0   03 Nov 1994 15:53:54   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 12:56:48   JAY
// Initial revision.
*/

class CThumbNailData;

#ifndef __THUMBREC_H__
#define __THUMBREC_H__

#include "cmanstd.h"

class CDrawContext;

/*
// Thumbnail creation data.
*/

class CThumbNailCreationData
{
public:
	int m_nWidth;					// Width in pixels.
	int m_nHeight;					// Height in lines.
	int m_nBPP;						// Bits per pixel.
	LPBYTE m_pBitmap;				// Pointer to DIB bits.

	DWORD m_dwTryCompression;	// What compressions to try (see below).
};

#define TRY_COMPRESSION_ALL	((DWORD)-1)		// All mask bits
#define TRY_COMPRESSION_None	0					// No mask bits

#define TRY_COMPRESSION_Deflate	1				// Deflate mask bit
#define TRY_COMPRESSION_JPEG		2				// JPEG mask bit


/*
// The thumbnail record.
//
// All thumbnails are assumed to use a common palette, and the thumbnail
// data is assumed to be indices into that palette. The palette is also
// assumed to be selected and realized into the destination DC at Draw()
// time.
*/

class CThumbNailData : public CStandardDataRecord
{
public:
	enum
	{
		COMPRESSION_Unknown = -1,
		COMPRESSION_None = 0,				// This MUST be zero.
		COMPRESSION_PackBits,				// Probably not used.
		COMPRESSION_Implode,
		COMPRESSION_Deflate,
		COMPRESSION_JPEG
	};

protected:
/*
// Data!
*/
	struct
	{
		WORD m_wWidth;				/* Width in pixels. */
		WORD m_wHeight;			/* Height in pixels. */
		WORD m_wBPP;				/* Bits per pixel - 8 is standard. */
		SHORT m_nCompression;
		DWORD m_dwCompressedSize;
	} m_Record;

	LPBYTE m_pBitmap;
	ST_DEV_POSITION m_lBitmapPosition;
	DWORD m_dwTryCompression;

/*
// The DIB header.
// Definition and data member...
*/

	struct CDibHeader
	{
		BITMAPINFOHEADER	bmiHeader;
		RGBQUAD				bmiColors[256];			/* 256 palette entries */
	};

	static CDibHeader far Header;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	CThumbNailData();

/*
// Assign method.
*/

	virtual void Assign(const CDataRecord& Record);

/*
// Set the bitmap to the passed data.
*/

	BOOL SetBitmap(CThumbNailCreationData* pData);

	// Get the size of the bitmap (as determined by the main record).
	DWORD BitmapSize(void);

	// Get the uncompressed size of the bitmap.
	DWORD UncompressedSize(void);

	// Get the Deflate compressed size of the bitmap.
	DWORD DeflateSize(void);

	// Get the JPEG compressed size of the bitmap.
	DWORD JPEGSize(void);

	ERRORCODE AllocateBitmap(DWORD dwSize);
	void FreeBitmap(void);

	ERRORCODE AllocateMemory(BYTE** ppMemory, DWORD dwSize);
	void FreeMemory(BYTE* pMemory);
	
	ERRORCODE LoadBitmap(void);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static CStandardDataRecord* Create();
	virtual BOOL Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where);

/*
// The destructor.
*/

	virtual ~CThumbNailData();

/*
// Return the dimensions of the thumbnail.
*/

	int Width(void) const
 		{ return m_Record.m_wWidth; }
	int Height(void) const
		{ return m_Record.m_wHeight; }
	LPBYTE Bitmap(void) const
		{ return m_pBitmap; }

/*
// Draw ourselves into a DC.
*/

	BOOL Draw(CDC* pDC, CDrawContext* pContext, const CRect& crBounds);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	virtual ERRORCODE ReadData(StorageDevicePtr pDevice);

/*
// WriteData()
//
// Write the record.
*/

	virtual ERRORCODE WriteData(StorageDevicePtr pDevice);

/*
// SizeofData()
//
// Return the size of the record.
*/

	virtual ST_MAN_SIZE SizeofData(StorageDevicePtr pDevice);
};

/*
// In-line functions.
*/

#endif					// #ifndef __THUMBREC_H__

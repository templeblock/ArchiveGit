/*
// $Header: /PM8/App/PREVREC.H 1     3/03/99 6:11p Gbeddow $
//
// Project preview record definitions.
//
// $Log: /PM8/App/PREVREC.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:24:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:26   Fred
// Initial revision.
// 
//    Rev 1.1   10 May 1996 14:05:02   Fred
// CDeflate
// 
//    Rev 1.0   14 Mar 1996 13:48:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:12   FRED
// Initial revision.
// 
//    Rev 2.6   15 Sep 1995 14:04:14   JAY
// More 32-bit changes.
// 
//    Rev 2.5   10 Jul 1995 16:24:58   JAY
// Added "AlbumPath" to preview record.
// 
//    Rev 2.4   07 Jun 1995 13:14:50   JAY
// Added function to create HBITMAP from a preview record.
// 
//    Rev 2.3   06 Jun 1995 10:09:28   JAY
// Now uses CImplode.
// 
//    Rev 2.2   05 Jun 1995 16:54:40   JAY
// Now supports Implode (PKWARE) library for better compression.
// 
//    Rev 2.1   05 Jun 1995 11:30:24   JAY
// New color previews.
// 
//    Rev 2.0   07 Feb 1995 15:48:54   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:42   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:14   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:08:10   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:38   JAY
//  
*/

#ifndef __PREVREC_H__
#define __PREVREC_H__

/*
// The project preview record.
*/

FARCLASS ProjectPreviewRecord : public DatabaseRecord
{
public:

	enum
	{
		PREVIEW_TYPE_Monochrome = 0,		// This MUST be zero.
		PREVIEW_TYPE_ColorDIB
	};

	enum
	{
		COMPRESSION_Unknown = -1,
		COMPRESSION_None = 0,				// This MUST be zero.
		COMPRESSION_PackBits,
		COMPRESSION_Implode,
		COMPRESSION_Deflate
	};

protected:

/* This part is written to the file. */

	struct
	{
		SHORT		valid;					/* Is this preview valid? (BOOL) */
		SHORT		width;					/* Absolute width of bitmap. */
		SHORT		height;					/* Absolute height of bitmap. */
		SHORT		x_resolution;			/* X resolution of bitmap. */
		SHORT		y_resolution;			/* Y resolution of bitmap. */
	// Added in ver 2.0
		SHORT		nType;					/* One of the types above. */
		SHORT		nCompression;			/* Type of compression. */
		DWORD		dwCompressedSize;		/* Compressed size. */
		WORD		m_wFlags;
	} record;

/*
// This part is not.
*/

	CString				m_csDescription;	/* A project description. */
	VOIDPTR				bitmap;				/* A pointer to the bitmap. */
	ST_DEV_POSITION	bitmap_position;
/* The last category "path" when stored in an album. */
	CString				m_csAlbumPath;		/* The "path" into the album. */

/*
// Flags.
*/

	enum
	{
		FLAG_HasAlbumPath = 1
	};

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	ProjectPreviewRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

	// Free the bitmap.
	void FreeBitmap(void);

public:

	~ProjectPreviewRecord();

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Read the preview bitmap,
*/

	ERRORCODE read_preview(VOIDPTR far *bitmap);

/*
// Create an HBITMAP for the preview.
// If the preview is a DIB, it uses a screen DC to convert the DIB to a DDB
// (or pDC, if passed).
*/

	HBITMAP GetPreviewBitmap(CDC* pDC = NULL);

/*
// Return the size of the preview bitmap.
*/

	LONG preview_size(VOID);

/*
// Attach a bitmap to the preview.
// The bitmap is used AS IS, becomes property of the preview,
// and SHOULD NOT BE FREED.
*/

	ERRORCODE attach_preview_bitmap(VOIDPTR bm);

/*
// Set the description for the preview.
// A copy is made of the string.
*/

	ERRORCODE set_description(LPCSTR string);

/*
// Get the preview's description.
*/

	LPCSTR get_description(VOID)
		{ return m_csDescription; }

/*
// Set the album path.
*/

	void SetAlbumPath(LPCSTR pAlbumPath)
		{ m_csAlbumPath = pAlbumPath; modified(); }

/*
// Get the album path.
*/

	const CString& GetAlbumPath(void)
		{ return m_csAlbumPath; }

/*
// Return whether the preview is valid or not.
*/

	BOOL is_valid()
		{ return record.valid; }

/*
// Return the width of the preview.
*/

	SHORT get_width()
		{ return record.width; }

/*
// Return the height of the preview.
*/

	SHORT get_height()
		{ return record.height; }

/*
// Return the x resolution of the preview.
*/

	SHORT get_x_resolution()
		{ return record.x_resolution; }

/*
// Return the y resolution of the preview.
*/

	SHORT get_y_resolution()
		{ return record.y_resolution; }

/*
// Return the type of the preview.
*/

	SHORT GetType(void) const
		{ return record.nType; }

/*
// Set the preview parameters. The preview becomes "valid".
*/

	VOID init_preview(SHORT w, SHORT h, SHORT xr, SHORT yr, SHORT nType = PREVIEW_TYPE_Monochrome);

/*
// Remove the preview. The preview becomes "invalid".
*/

	VOID remove_preview(VOID);

/*
// Compressed and uncompressed sizes. Mostly used internally.
*/

	DWORD UncompressedSize(void);
	DWORD DeflateSize(void);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif					// #ifndef __PREVREC_H__

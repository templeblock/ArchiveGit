/*
// $Header: /PM8/App/GRAFREC.H 1     3/03/99 6:05p Gbeddow $
//
// Graphic record definitions.
//
// $Log: /PM8/App/GRAFREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 11    1/11/99 1:32p Gbeddow
// support for reading/writing Print Shop square, row & column vector
// graphics in Pretzel collection files
// 
// 10    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 9     8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 8     3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 7     2/03/98 12:37p Hforman
// cleanup old code, add extern
// 
// 6     2/02/98 9:46a Jayn
// 
// 5     1/29/98 8:08p Hforman
// add GRAPHIC_TYPE_PMO, other cleanup
// 
// 4     1/20/98 8:54a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 3     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 2     12/11/97 9:20a Jstoner
// vector to bitmap conversion for photo enhancer
// 
//    Rev 1.1   17 Sep 1997 09:11:44   Jay
// Bitmap rotation, PNG
// 
//    Rev 1.0   14 Aug 1997 15:21:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:02   Fred
// Initial revision.
// 
//    Rev 1.12   05 Jun 1997 16:44:50   Jay
// HasTransparency
// 
//    Rev 1.11   30 Apr 1997 12:29:58   Jay
// Can now determine type of embedded files
// 
//    Rev 1.10   11 Apr 1997 11:23:44   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.9   06 Dec 1996 10:41:26   Jay
// Will now reinit a graphic even if from the cache.
// 
//    Rev 1.8   01 Nov 1996 17:11:50   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.7   01 Nov 1996 10:26:36   Jay
// Photoshop support
// 
//    Rev 1.6   10 Oct 1996 14:30:36   Jay
// More time stamps
// 
//    Rev 1.5   07 Oct 1996 17:02:36   Jay
// TimeStamp
// 
//    Rev 1.4   20 Sep 1996 13:41:20   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.3   25 Aug 1996 12:59:58   Jay
// New friendly name code.
// 
//    Rev 1.2   21 Aug 1996 16:49:24   Jay
// set_graphic_info() is now public.
// 
//    Rev 1.1   16 May 1996 14:24:34   Jay
// JPEG
// 
//    Rev 1.0   14 Mar 1996 13:48:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:20   FRED
// Initial revision.
// 
//    Rev 2.4   14 Sep 1995 09:32:18   JAY
// 32-bit changes
// 
//    Rev 2.3   18 Aug 1995 15:06:00   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.2   07 Jul 1995 16:10:54   JAY
// Misc changes.
// 
//    Rev 2.1   09 Mar 1995 08:27:32   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:40   JAY
// Initial revision.
// 
//    Rev 1.11   09 Jan 1995 16:26:54   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.10   30 Nov 1994 16:34:54   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.9   04 Aug 1994 07:33:30   JAY
// Large model and 32-bit changes
// 
//    Rev 1.8   13 Jul 1994 11:57:24   JAY
// GCS can now be NULL to init.
// 
//    Rev 1.7   01 Jun 1994 16:35:14   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.6   31 May 1994 12:40:56   JAY
// Now calls the database's external prep before the existing internal ones.
// 
//    Rev 1.5   23 May 1994 08:33:14   JAY
// Added constructor for a GRAPHIC_CREATE_STRUCT.
// Added destroy() method to delete embedded source file upon deletion.
// Added prep_storage_file() to allow accessing the embedded source file
// when it exists.
// 
//    Rev 1.4   11 Mar 1994 14:24:00   JAY
// Additional code for WMF support.
// 
//    Rev 1.3   28 Feb 1994 10:50:22   JAY
// New arg to create_helper() to only allow Unknown or illegal types at certain
// (not general) times.
// 
//    Rev 1.2   13 Jan 1994 17:01:08   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:20   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:50   JAY
//  
*/

#ifndef __GRAFREC_H__
#define __GRAFREC_H__

class ImageDriver;

#include "redisp.h"
#include "file.h"
#include "timestmp.h"

enum GRAPHIC_TYPE
{
	GRAPHIC_TYPE_UNKNOWN = 0,
	GRAPHIC_TYPE_SHP,				/* PrintMaster, NewsMaster */
	GRAPHIC_TYPE_TIFF,			/* Tag Image File Format */
	GRAPHIC_TYPE_PCX,				/* Paintbrush, etc. */
	GRAPHIC_TYPE_OBSOLETE1,		/* Lotus Pic. Not supported here... */
	GRAPHIC_TYPE_OBSOLETE2,		/* New PrintShop (POG) - "old" format */
	GRAPHIC_TYPE_OBSOLETE3,		/* New PrintShop (PNG) - "new" format */
	GRAPHIC_TYPE_OBSOLETE4,		/* New PrintShop (CNG) - "new" color format */
	GRAPHIC_TYPE_OBSOLETE5,		/* PrintShop (DAT) */
	GRAPHIC_TYPE_GIF,				/* Graphics Interchange Format file. */
	GRAPHIC_TYPE_HALO,			/* Dr. Halo file. */
	GRAPHIC_TYPE_EPS,				/* Encapsulated PostScript file. */
	GRAPHIC_TYPE_BMP,				/* Windows bitmap file. */
	GRAPHIC_TYPE_CGM,				/* Computer Graphics Metafile format. */
	GRAPHIC_TYPE_WPG,				/* Word Perfect Graphic file. */
	GRAPHIC_TYPE_WMF,				/* Windows MetaFile format. */
	GRAPHIC_TYPE_PCD,				/* Photo CD format. */
	GRAPHIC_TYPE_JPEG,			/* JPEG format. */
	GRAPHIC_TYPE_PSD,				/* Photoshop format. */
	GRAPHIC_TYPE_PNG,				/* Portable Network Graphic format. */
	GRAPHIC_TYPE_PMO,				/* PrintMaster Objects format. */
	GRAPHIC_TYPE_DPP,				/* Serif Draw Plus file */
	GRAPHIC_TYPE_PS_SQUARE,		/* PrintShop square vector graphic */
	GRAPHIC_TYPE_PS_ROW,			/* PrintShop row vector graphic */
	GRAPHIC_TYPE_PS_COLUMN		/* PrintShop column vector graphic */
};

enum GRAPHIC_STORAGE
{
	GRAPHIC_STORAGE_UNKNOWN,		/* totally unknown */
	GRAPHIC_STORAGE_MEMORY,			/* in memory */
	GRAPHIC_STORAGE_FILE,			/* in the source file */
	GRAPHIC_STORAGE_IMAGE,			/* an image is in memory */
	GRAPHIC_STORAGE_MISSING,		/* on disk but in an unknown place */
	GRAPHIC_STORAGE_BITMAP,			/* in the document at low resolution */
	GRAPHIC_STORAGE_TEMPDIB			/* temporary DIB graphic (in memory) */
};

typedef struct
{
	USHORT				line;
	ST_DEV_POSITION	offset;
} LINE_LOCATOR, far *LINE_LOCATOR_PTR;

/*
// What type of render methods do we do?
*/

enum RENDER_METHOD
{
	ORDERED_DITHER,
	ERROR_DIFFUSION
};

/*
// Possible return types from a method.
*/

enum IMAGE_CODE
{
	IMAGE_CODE_DONE,
	IMAGE_CODE_CONTINUE,
	IMAGE_CODE_INTERRUPTED,
	IMAGE_CODE_ERROR,
	IMAGE_CODE_OPEN,
	IMAGE_CODE_CHANGE_STRIP,					/* Used by TIFF code. */
	IMAGE_CODE_TERMINATED
};

/*
// The creation data for a graphic.
*/

struct GRAPHIC_CREATE_STRUCT
{
	// Constructor to default data.
	GRAPHIC_CREATE_STRUCT();

	// Data.
	CString m_csFileName;
	CString m_csFriendlyName;
	BOOL proof;
	BOOL embed_source;
	BOOL m_fCreateImage;
	BOOL m_fSpecialCrop;		// need to crop copyright at bottom?
	StorageDevicePtr pSourceDevice;
	LPBITMAPINFO m_pTempDIB;
};
typedef GRAPHIC_CREATE_STRUCT far *GRAPHIC_CREATE_STRUCT_PTR;

/*
// A graphic helper.
*/

class GraphicHelper
{
protected:
	GraphicPtr graphic;
public:

	GraphicHelper(GraphicPtr owner) : graphic(owner) {}
	virtual ~GraphicHelper() {}

	virtual ERRORCODE read(StorageDevicePtr device) = 0;
	virtual ERRORCODE write(StorageDevicePtr device) = 0;
	virtual ST_DEV_IO_SIZE size(StorageDevicePtr device) = 0;

	virtual ERRORCODE init(GRAPHIC_CREATE_STRUCT_PTR gcs = NULL) = 0;
	virtual BOOL HasTransparency(void) { return FALSE; }
};

typedef GraphicHelper far *GraphicHelperPtr;

/*
// The graphic data record.
*/

class GraphicRecord : public DatabaseRecord
{
private:
/*
// An object to help with type-specific management.
*/

	GraphicHelperPtr	helper;			/* Per-type object */

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	GraphicRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Return the size of the bitmap portion of this record.
*/

	ST_DEV_IO_SIZE bitmap_size(VOID);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

/*
// Delete the graphic info for this record.
*/

	VOID delete_graphic_info(VOID);

public:

/*
// Set the graphic's info from the graphic create struct passed.
*/

	ERRORCODE set_graphic_info(GRAPHIC_CREATE_STRUCT_PTR gcs);

/*
// Create the helper for this graphic.
*/

	ERRORCODE create_helper(BOOL allow_unknown = FALSE);

/*
// Set the time member of the graphic record.
*/

	void SetTimeStamp(CTimeStamp* pTimeStamp = NULL);
	CTimeStamp GetTimeStamp(void) const
		{ return record.TimeStamp; }
	CTimeStamp ComputeTimeStamp(void);

/*
// The destructor for the record.
*/

	~GraphicRecord();

/*
// Data.
*/
	CString			m_csName;			/* The viewed name of the graphic */
	CString			m_csFileName;		/* The file name of the graphic */
	VOIDPTR 			bitmap;				/* An actual pointer to the data */
	LPBITMAPINFO	m_pTempDIB;			/* Temporary DIB. */
	BOOL				m_fReinitedCached;	/* Have we reinited a cached graphic? */

	static int		m_nCopyrightCropHeight;	// pixels to crop off bottom

	enum eRecordFlag
	{
		FLAG_SpecialCrop = 1				// need to crop bottom copyright?
	};

	struct
	{
		SHORT				type;					/* What type of graphic this is */
		SHORT				sub_type;			/* Differentation type */

		DB_RECORD_NUMBER	source;			/* Source data (if present) */

		USHORT			reference_count;	/* How many references there are */

		USHORT			x_resolution;		/* The x resolution of the data */
		USHORT			y_resolution;		/* The y resolution of the data */

		USHORT			x_size;				/* The x dimension of the data */
		USHORT			y_size;				/* The y dimension of the data */

		SHORT				storage;			/* Where the data is located. */
		SHORT				number;				/* Which number (in library, etc.) */

		// The size of this structure is such that it matches the
		// "where" structure that was here before.
		// 'Short0' is up for grabs if you need it. I believe it's always
		// zero.

		SHORT				Short0;

		// This is the old 32-bit time stamp which overflows in 2038.
		// In PMG 5.0, this has been replaced by the TimeStamp field
		// (a 64-bit FILETIME value.) Basically, this field is no longer
		// used in 5.0 documents, but the ReadData() method checks this
		// value and uses it to initialize the new TimeStamp member
		// if an old document is being read.
		DWORD				dwOldTimeStamp;

		DB_RECORD_NUMBER	image;

		// New for 5.0

		CTimeStamp TimeStamp;		// 64-bit time stamp (year 2000 compliant)

		FLAGS	flags;

	} record;

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Set the default graphic name. This is just the filename.
*/

	ERRORCODE set_default_name(VOID);

/*
// Handle an file open error.
*/

	UpdateStatePtr handle_open_error(UpdateStatePtr ustate);

/*
// Get the helper for this graphic record.
*/

	GraphicHelperPtr my_little_helper() { return helper; }

/*
// Set up a file object to read the graphic data.
// This may just set the file name or it may set up the device on top of the
// database's device.
// It is assumed that the storage device coming is uninitialized (no file
// name of based device set).
*/

	ERRORCODE prep_storage_file(StorageFilePtr file, BOOL fCheckTimeStamp = TRUE);

/*
// Embed the source for this graphic into the document.
*/

	ERRORCODE embed_source(StorageDevice* pSource = NULL);

/*
// Create an image driver usable to read lines of bitmap data.
// This only works on bitmap formats (PCX, JPEG, etc).
*/

	ImageDriver* CreateImageDriver(BOOL fUseImage = TRUE, StorageDevice* pSource = NULL);

/*
// Create an image for this graphic in the document.
// Some graphics (like Photo CD) require this. For others, its optional.
*/

	ERRORCODE CreateImage(StorageDevice* pSource = NULL);

/*
// Write the image data for this object as BMP data.
*/

	ERRORCODE WriteImageAsBMP(StorageDevice* pDevice);


/*
// Set the name, freeing any old and merely assigning it.
*/

	void set_name(LPCSTR pNewName);

/*
// Re-initialize the graphic.
*/

	ERRORCODE reinit(BOOL fCheckTimeStamp = FALSE);

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

extern GRAPHIC_TYPE type_of_art_extension(LPCSTR name);
extern GRAPHIC_TYPE type_of_graphic_extension(LPCSTR name);
extern GRAPHIC_TYPE type_of_graphic(LPCSTR name, StorageDevice* pSource = NULL);

#endif					// #ifndef __GRAFREC_H__

/*
// JPEGIMPL.H
//
// PrintMaster-specific definitions for the JPEG implementation.
//
// $Header: /PM8/App/JPEGIMPL.H 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/JPEGIMPL.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     12/05/97 4:18p Johno
// Added Reversed BOOL to WriteLines
// 
//    Rev 1.0   14 Aug 1997 15:22:00   Fred
// Initial revision.
// 
//    Rev 1.1   14 Aug 1997 13:30:16   Fred
// Removed referenced to .H files that were #ifdefed out and were
// confusing the dependancies.
// 
// 
//    Rev 1.0   14 Aug 1997 09:34:22   Fred
// Initial revision.
// 
//    Rev 1.3   22 May 1997 16:13:18   Jay
// More new JPEG stuff.
// 
//    Rev 1.2   22 May 1997 10:30:24   Jay
// New Pegasus code
// 
//    Rev 1.1   08 Aug 1996 18:10:58   Jay
// New JPEG compress and decompress classes
// 
//    Rev 1.0   16 May 1996 14:24:38   Jay
// JPEG
*/

#if 0
class CJPEGSourceManager;
class CJPEGDestinationManager;
class CJPEGErrorManager;
class CJPEGException;
#endif

class CJPEGCompressor;
class CJPEGDecompressor;

#ifndef __JPEGIMPL_H__
#define __JPEGIMPL_H__

class StorageDevice;

/////////////////////////////////////////////////////////////////////////////
// Start off with the standard include file.

#if 0
extern "C"
{
//#include "jpeg\jpeglib.h"
//#include "jpeg\jerror.h"
};
#endif

#include "pegasus.h"

#if 0
/////////////////////////////////////////////////////////////////////////////
// CJPEGSourceManager: Standard data source manager.

class CJPEGSourceManager : public jpeg_source_mgr
{
public:
	CJPEGSourceManager(j_decompress_ptr cinfo, StorageDevice* pFile);
	~CJPEGSourceManager();
protected:
	StorageDevice* m_pFile;
	BOOL m_fStartOfFile;
	JOCTET* m_pBuffer;

	// Our methods.
	static void InitSource(j_decompress_ptr cinfo);
	static boolean FillInputBuffer(j_decompress_ptr cinfo);
	static void SkipInputData(j_decompress_ptr cinfo, long num_bytes);
	static boolean ResyncToRestart(j_decompress_ptr cinfo, int desired);
	static void TermSource(j_decompress_ptr cinfo);
};

/////////////////////////////////////////////////////////////////////////////
// CJPEGDestinationManager: Standard data destination manager.

class CJPEGDestinationManager : public jpeg_destination_mgr
{
public:
	CJPEGDestinationManager(j_compress_ptr cinfo, StorageDevice* pFile);
	~CJPEGDestinationManager();

	long BytesWritten(void) const
		{ return m_lBytesWritten; }

protected:
	StorageDevice* m_pFile;
	JOCTET* m_pBuffer;
	long m_lBytesWritten;

	// Our methods.
	static void InitDestination(j_compress_ptr cinfo);
	static boolean EmptyOutputBuffer(j_compress_ptr cinfo);
	static void TermDestination(j_compress_ptr cinfo);
};

class CJPEGErrorManager : public jpeg_error_mgr
{
public:
	CJPEGErrorManager();
	~CJPEGErrorManager();

	/* Error exit handler: does not return to caller */
	static void ErrorExit(j_common_ptr cinfo);
	/* Conditionally emit a trace or warning message */
	static void OutputMessage(j_common_ptr cinfo);
};

/*
// A JPEG exception.
*/

class CJPEGException : public CException
{
	DECLARE_DYNAMIC(CJPEGException);
public:
	static void ThrowException(void);
};
#endif

/////////////////////////////////////////////////////////////////////////////
// CJPEGDecompressor

class CJPEGDecompressor
{
public:
	CJPEGDecompressor();
	virtual ~CJPEGDecompressor();

	void FreeAll(void);

	ERRORCODE Init(StorageDevice* pDevice);
	ERRORCODE ReadLines(int nLine, LPBYTE pLines, int nLineCount);
	ERRORCODE Finish(void);

	void Abort(void);

#if 0
	// Old
	int ImageWidth(void) const
		{ return m_pDecompressStruct->image_width; }
	int ImageHeight(void) const
		{ return m_pDecompressStruct->image_height; }
	int ColorComponents(void) const
		{ return m_pDecompressStruct->output_components; }
#else
	// New
	int ImageWidth(void) const
		{ return m_Pegasus.ImageWidth(); }
	int ImageHeight(void) const
		{ return m_Pegasus.ImageHeight(); }
	int ColorComponents(void) const
		{ return m_Pegasus.ColorComponents(); }
#endif

protected:
	StorageDevice* m_pDevice;

#if 0
	jpeg_decompress_struct* m_pDecompressStruct;
	CJPEGErrorManager* m_pErrorMgr;
	CJPEGSourceManager* m_pSource;
#else
	// New Pegasus implementation.
   CPegasus m_Pegasus;
	CPegasusInputDevice m_Input;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CJPEGCompressor

class CJPEGCompressor
{
public:
	CJPEGCompressor();
	virtual ~CJPEGCompressor();

	ERRORCODE Init(StorageDevice* pDevice, int nWidth, int nHeight, int nColorComponents = 3);
	ERRORCODE WriteLines(LPBYTE pLines, int nLineCount, BOOL Reversed = FALSE);
	ERRORCODE Finish(void);

	void Abort(void);


#if 0
	// Old
	long BytesWritten(void) const
		{ return m_pDestination->BytesWritten(); }

	int ImageWidth(void) const
		{ return m_pCompressStruct->image_width; }
	int ImageHeight(void) const
		{ return m_pCompressStruct->image_height; }
	int ColorComponents(void) const
		{ return m_pCompressStruct->input_components; }
#else
	// New
	long BytesWritten(void) const
		{ return m_Output.BytesWritten(); }

	int ImageWidth(void) const
		{ return m_Pegasus.ImageWidth(); }
	int ImageHeight(void) const
		{ return m_Pegasus.ImageHeight(); }
	int ColorComponents(void) const
		{ return m_Pegasus.ColorComponents(); }
#endif

protected:
	void FreeAll(void);

protected:
	StorageDevice* m_pDevice;

#if 0
	jpeg_compress_struct* m_pCompressStruct;
	CJPEGErrorManager* m_pErrorMgr;
	CJPEGDestinationManager* m_pDestination;
#else
	// New Pegasus implementation.
   CPegasus m_Pegasus;
	CPegasusOutputDevice m_Output;
#endif
};

#endif

/*
// JPEGIMPL.CPP
//
// Implementation routines for JPEG support.
//
// $Header: /PM8/App/JPEGIMPL.CPP 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/JPEGIMPL.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     12/05/97 4:18p Johno
// Added Reversed BOOL to WriteLines
// 
//    Rev 1.0   14 Aug 1997 15:22:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:56   Fred
// Initial revision.
// 
//    Rev 1.6   22 May 1997 16:13:18   Jay
// More new JPEG stuff.
// 
//    Rev 1.5   22 May 1997 10:30:24   Jay
// New Pegasus code
// 
//    Rev 1.4   09 Aug 1996 15:04:30   Jay
// Handles 16-bit code with rows straddling 64-K boundaries.
// 
//    Rev 1.3   09 Aug 1996 15:00:00   Jay
// 
//    Rev 1.2   09 Aug 1996 14:13:50   Fred
// 16-bit change
// 
//    Rev 1.1   08 Aug 1996 18:10:58   Jay
// New JPEG compress and decompress classes
// 
//    Rev 1.0   16 May 1996 14:24:38   Jay
// JPEG
*/

#include	"stdafx.h"
#include "jpegimpl.h"			// JPEG implementation classes.

#if 0
/////////////////////////////////////////////////////////////////////////////
// CJPEGSourceManager
// This code is a paraphrase of the JPEG stdio data source using StorageFile
// instead of the C-library FILE. Most of the comments are taken from that
// implementation verbatim.

#define INPUT_BUF_SIZE	4096

CJPEGSourceManager::CJPEGSourceManager(j_decompress_ptr cinfo, StorageDevice* pFile)
{
	ASSERT(pFile != NULL);

	m_pFile = pFile;
	m_pBuffer = NULL;

	/* first time for this JPEG object? */
	ASSERT(cinfo->src == NULL);

	if (cinfo->src == NULL)
	{
		cinfo->src = this;
		m_pBuffer = new JOCTET[INPUT_BUF_SIZE];
	}

	// Set up our function pointers.
	init_source = InitSource;
	fill_input_buffer = FillInputBuffer;
	skip_input_data = SkipInputData;
	resync_to_restart = ResyncToRestart;
	term_source = TermSource;

	// Set up the base struct's variables.
	bytes_in_buffer = 0;		/* forces fill_input_buffer on first read */
	next_input_byte = NULL;	/* until buffer loaded */
}

CJPEGSourceManager::~CJPEGSourceManager()
{
	delete [] m_pBuffer;
}

/*
// Initialize source --- called by jpeg_read_header
// before any data is actually read.
*/

void CJPEGSourceManager::InitSource(j_decompress_ptr cinfo)
{
	CJPEGSourceManager* pThis = (CJPEGSourceManager*)cinfo->src;
	ASSERT(pThis != NULL);

	/*
	// We reset the empty-input-file flag for each image,
	// but we don't clear the input buffer.
	// This is correct behavior for reading a series of images from one source.
	*/

	pThis->m_fStartOfFile = TRUE;
}

/*
// Fill the input buffer --- called whenever buffer is emptied.
//
// In typical applications, this should read fresh data into the buffer
// (ignoring the current state of next_input_byte & bytes_in_buffer),
// reset the pointer & count to the start of the buffer, and return TRUE
// indicating that the buffer has been reloaded.  It is not necessary to
// fill the buffer entirely, only to obtain at least one more byte.
//
// There is no such thing as an EOF return.  If the end of the file has been
// reached, the routine has a choice of ERREXIT() or inserting fake data into
// the buffer.  In most cases, generating a warning message and inserting a
// fake EOI marker is the best course of action --- this will allow the
// decompressor to output however much of the image is there.  However,
// the resulting error message is misleading if the real problem is an empty
// input file, so we handle that case specially.
//
// In applications that need to be able to suspend compression due to input
// not being available yet, a FALSE return indicates that no more data can be
// obtained right now, but more may be forthcoming later.  In this situation,
// the decompressor will return to its caller (with an indication of the
// number of scanlines it has read, if any).  The application should resume
// decompression after it has loaded more data into the input buffer.  Note
// that there are substantial restrictions on the use of suspension --- see
// the documentation.
//
// When suspending, the decompressor will back up to a convenient restart point
// (typically the start of the current MCU). next_input_byte & bytes_in_buffer
// indicate where the restart point will be if the current call returns FALSE.
// Data beyond this point must be rescanned after resumption, so move it to
// the front of the buffer rather than discarding it.
*/

boolean CJPEGSourceManager::FillInputBuffer(j_decompress_ptr cinfo)
{
	CJPEGSourceManager* pThis = (CJPEGSourceManager*)cinfo->src;
	ST_DEV_IO_SIZE nBytes;

	pThis->m_pFile->read((LPVOID)pThis->m_pBuffer, INPUT_BUF_SIZE, &nBytes);

	if (nBytes <= 0)
	{
		if (pThis->m_fStartOfFile)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		pThis->m_pBuffer[0] = (JOCTET) 0xFF;
		pThis->m_pBuffer[1] = (JOCTET) JPEG_EOI;
		nBytes = 2;
	}

	pThis->next_input_byte = pThis->m_pBuffer;
	pThis->bytes_in_buffer = nBytes;
	pThis->m_fStartOfFile = FALSE;

	return TRUE;
}

void CJPEGSourceManager::SkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	CJPEGSourceManager* pThis = (CJPEGSourceManager*)cinfo->src;

	/*
	// Just a dumb implementation for now.
	// Not clear that being smart is worth
	// any trouble anyway --- large skips are infrequent.
	//
	// This should be changed to be more efficient once it's working... (JN)
   */

	if (num_bytes > 0)
	{
		while (num_bytes > (long) pThis->bytes_in_buffer)
		{
			num_bytes -= (long) pThis->bytes_in_buffer;
			FillInputBuffer(cinfo);
			/*
			// note we assume that fill_input_buffer will never return FALSE,
			// so suspension need not be handled.
			*/
		}
		pThis->next_input_byte += (size_t) num_bytes;
		pThis->bytes_in_buffer -= (size_t) num_bytes;
	}
}

/*
// An additional method that can be provided by data source modules is the
// resync_to_restart method for error recovery in the presence of RST markers.
// For the moment, this source module just uses the default resync method
// provided by the JPEG library.  That method assumes that no backtracking
// is possible.
*/

boolean CJPEGSourceManager::ResyncToRestart(j_decompress_ptr cinfo, int desired)
{
	return jpeg_resync_to_restart(cinfo, desired);
}

/*
// Terminate source --- called by jpeg_finish_decompress
// after all data has been read.  Often a no-op.
//
// NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
// application must deal with any cleanup that should happen even
// for error exit.
*/

void CJPEGSourceManager::TermSource(j_decompress_ptr cinfo)
{
	/* no work necessary here */
}

/////////////////////////////////////////////////////////////////////////////
// CJPEGDestinationManager
// This code is a paraphrase of the JPEG stdio data source using StorageFile
// instead of the C-library FILE. Most of the comments are taken from that
// implementation verbatim.

#define OUTPUT_BUF_SIZE	4096

CJPEGDestinationManager::CJPEGDestinationManager(j_compress_ptr cinfo, StorageDevice* pFile)
{
	m_pFile = pFile;
	m_pBuffer = NULL;
	m_lBytesWritten = 0;

	/* first time for this JPEG object? */
	ASSERT(cinfo->dest == NULL);

	if (cinfo->dest == NULL)
	{
		cinfo->dest = this;
		m_pBuffer = new JOCTET[OUTPUT_BUF_SIZE];
	}

	// Set up our function pointers.
	init_destination = InitDestination;
	empty_output_buffer = EmptyOutputBuffer;
	term_destination = TermDestination;

	// Set up the base struct's variables.
	next_output_byte = m_pBuffer;
	free_in_buffer = OUTPUT_BUF_SIZE;
}

CJPEGDestinationManager::~CJPEGDestinationManager()
{
	delete [] m_pBuffer;
}

/*
// Initialize source --- called by jpeg_read_header
// before any data is actually read.
*/

void CJPEGDestinationManager::InitDestination(j_compress_ptr cinfo)
{
	CJPEGDestinationManager* pThis = (CJPEGDestinationManager*)cinfo->dest;
	ASSERT(pThis != NULL);

	pThis->m_lBytesWritten = 0;

	// Reset to empty buffer.
	pThis->next_output_byte = pThis->m_pBuffer;
	pThis->free_in_buffer = OUTPUT_BUF_SIZE;
}

/*
// Empty the output buffer --- called whenever buffer fills up.
//
// In typical applications, this should write the entire output buffer
// (ignoring the current state of next_output_byte & free_in_buffer),
// reset the pointer & count to the start of the buffer, and return TRUE
// indicating that the buffer has been dumped.
//
// In applications that need to be able to suspend compression due to output
// overrun, a FALSE return indicates that the buffer cannot be emptied now.
// In this situation, the compressor will return to its caller (possibly with
// an indication that it has not accepted all the supplied scanlines).  The
// application should resume compression after it has made more room in the
// output buffer.  Note that there are substantial restrictions on the use of
// suspension --- see the documentation.
//
// When suspending, the compressor will back up to a convenient restart point
// (typically the start of the current MCU). next_output_byte & free_in_buffer
// indicate where the restart point will be if the current call returns FALSE.
// Data beyond this point will be regenerated after resumption, so do not
// write it out when emptying the buffer externally.
*/

boolean CJPEGDestinationManager::EmptyOutputBuffer(j_compress_ptr cinfo)
{
	CJPEGDestinationManager* pThis = (CJPEGDestinationManager*)cinfo->dest;
	ASSERT(pThis != NULL);

	pThis->m_lBytesWritten += OUTPUT_BUF_SIZE;

	if (pThis->m_pFile != NULL)
	{
		// Do the write!
		ERRORCODE error = pThis->m_pFile->write((LPVOID)pThis->m_pBuffer, OUTPUT_BUF_SIZE);

		if (error != ERRORCODE_None)
		{
			ERREXIT(cinfo, JERR_FILE_WRITE);
		}
	}

	// Reset to empty buffer.
	pThis->next_output_byte = pThis->m_pBuffer;
	pThis->free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}

/*
// Terminate destination --- called by jpeg_finish_compress
// after all data has been written.  Usually needs to flush buffer.
//
// NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
// application must deal with any cleanup that should happen even
// for error exit.
*/

void CJPEGDestinationManager::TermDestination(j_compress_ptr cinfo)
{
	CJPEGDestinationManager* pThis = (CJPEGDestinationManager*)cinfo->dest;
	ASSERT(pThis != NULL);

	size_t nDataCount = OUTPUT_BUF_SIZE - pThis->free_in_buffer;

	// Write any data remaining in the buffer.
	if (nDataCount > 0)
	{
		pThis->m_lBytesWritten += nDataCount;

		if (pThis->m_pFile != NULL)
		{
			ERRORCODE error = pThis->m_pFile->write((LPVOID)pThis->m_pBuffer, nDataCount);

			if (error != ERRORCODE_None)
			{
				ERREXIT(cinfo, JERR_FILE_WRITE);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJPEGErrorManager

CJPEGErrorManager::CJPEGErrorManager()
{
// Begin with the standard error stuff.
	jpeg_std_error(this);

// Plug in our overrides.
	/* Error exit handler: does not return to caller */
	error_exit = ErrorExit;
	/* Conditionally emit a trace or warning message */
	output_message = OutputMessage;
}

CJPEGErrorManager::~CJPEGErrorManager()
{
}

/* Error exit handler: does not return to caller */
void CJPEGErrorManager::ErrorExit(j_common_ptr cinfo)
{		
	CJPEGException::ThrowException();
}

/* Conditionally emit a trace or warning message */
void CJPEGErrorManager::OutputMessage(j_common_ptr cinfo)
{
#ifdef _DEBUG
	char buffer[JMSG_LENGTH_MAX];

	/* Create the message */
	(*cinfo->err->format_message) (cinfo, buffer);

	/* Send it to stderr, adding a newline */
	TRACE("%s\n", buffer);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CJPEGException

IMPLEMENT_DYNAMIC(CJPEGException, CException)

void CJPEGException::ThrowException(void)
{
	THROW(new CJPEGException);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CJPEGDecompressor

CJPEGDecompressor::CJPEGDecompressor()
{
	m_pDevice = NULL;
#if 0
	m_pDecompressStruct = NULL;
	m_pErrorMgr = NULL;
	m_pSource = NULL;
#endif
}

CJPEGDecompressor::~CJPEGDecompressor()
{
	FreeAll();
}

void CJPEGDecompressor::FreeAll(void)
{
#if 0
	if (m_pDecompressStruct != NULL)
	{
		jpeg_destroy_decompress(m_pDecompressStruct);
		delete m_pDecompressStruct;
		m_pDecompressStruct = NULL;
	}

	delete m_pErrorMgr;
	m_pErrorMgr = NULL;

	delete m_pSource;
	m_pSource = NULL;
#endif
}

ERRORCODE CJPEGDecompressor::Init(StorageDevice* pDevice)
{
	// Make sure we are clear.
	FreeAll();

	// Remember the device.
	m_pDevice = pDevice;

	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old

	TRY
	{
		m_pDecompressStruct = new jpeg_decompress_struct;
		m_pErrorMgr = new CJPEGErrorManager;

		/* Initialize the JPEG decompression object with default error handling. */
		m_pDecompressStruct->err = m_pErrorMgr;
		jpeg_create_decompress(m_pDecompressStruct);

		/* Specify data source for decompression */
		m_pSource = new CJPEGSourceManager(m_pDecompressStruct, pDevice);

		/* Read file header, set default decompression parameters */
		jpeg_read_header(m_pDecompressStruct, TRUE);

		/* Start decompressor */
		jpeg_start_decompress(m_pDecompressStruct);
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Read;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
#else
	// New
	m_Input.SetDevice(pDevice);
	m_Pegasus.SetInput(&m_Input);

	long lStatus = m_Pegasus.StartJPEGDecompression();
	if (lStatus == ERR_NONE)
	{
		error = ERRORCODE_None;
	}
	else
	{
		error = ERRORCODE_Read;
	}
#endif

	return error;
}

ERRORCODE CJPEGDecompressor::ReadLines(int nLine, LPBYTE pLines, int nLineCount)
{
	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old

   int nRowSize = (ImageWidth() * ColorComponents() + 3) & ~3;

#ifndef WIN32
	LPBYTE pAlternateRow = NULL;
#endif
	TRY
	{
		if (m_pDecompressStruct->output_scanline > (JDIMENSION)nLine)
		{
			return ERRORCODE_Read;
		}

#ifdef WIN32
		JSAMPROW pRow = pLines;
		LPBYTE pSkipRow = pRow;
#else
		unsigned char __huge* pRow = pLines;
		pAlternateRow = new BYTE[nRowSize];
		LPBYTE pSkipRow = pAlternateRow;
#endif

		// Seek to where we want to read.
		while (m_pDecompressStruct->output_scanline != (JDIMENSION)nLine)
		{
			if (jpeg_read_scanlines(m_pDecompressStruct, (JSAMPARRAY)&pSkipRow, 1) != 1)
			{
				return ERRORCODE_Read;
			}
		}

		// Do the read.
		while (nLineCount != 0)
		{
#ifdef WIN32
			if (jpeg_read_scanlines(m_pDecompressStruct, (JSAMPARRAY)&pRow, 1) != 1)
#else
			BOOL fCrosses = FALSE;
			if (0x10000 - LOWORD(pRow) < nRowSize)
			{
				// The row crosses into the next segment. Copy the data to a local
				// row that we know is safe.
				if (pAlternateRow == NULL)
				{
					pAlternateRow = new BYTE[nRowSize];
				}
				fCrosses = TRUE;
			}
			if (jpeg_read_scanlines(m_pDecompressStruct,
											 (JSAMPARRAY)(fCrosses ? &pAlternateRow : (LPBYTE*)&pRow),
											 1) == 1)
			{
				if (fCrosses)
				{
					hmemcpy(pRow, pAlternateRow, nRowSize);
				}
			}
			else
#endif
			{
				return ERRORCODE_Read;
			}
			nLineCount--;
			pRow += nRowSize;
		}
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Read;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

#ifndef WIN32
	delete [] pAlternateRow;
#endif
#else
	// New
	if (m_Pegasus.ReadJPEGLines(nLine, pLines, nLineCount) != ERR_NONE)
	{
		error = ERRORCODE_Read;
	}
#endif

	return error;
}

ERRORCODE CJPEGDecompressor::Finish(void)
{
	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old
	TRY
	{
		jpeg_finish_decompress(m_pDecompressStruct);
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Write;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
#else
	// New
	m_Pegasus.FinishJPEGDecompression();
#endif

	return error;
}

void CJPEGDecompressor::Abort(void)
{
#if 0
	// Old
	if (m_pDecompressStruct != NULL)
	{
		jpeg_abort_decompress(m_pDecompressStruct);
	}
#else
	// New
	m_Pegasus.AbortJPEGDecompression();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CJPEGCompressor

CJPEGCompressor::CJPEGCompressor()
{
	m_pDevice = NULL;
#if 0
	// Old
	m_pCompressStruct = NULL;
	m_pErrorMgr = NULL;
	m_pDestination = NULL;
#endif
}

CJPEGCompressor::~CJPEGCompressor()
{
	FreeAll();
}

void CJPEGCompressor::FreeAll(void)
{
#if 0
	// Old
	if (m_pCompressStruct != NULL)
	{
		jpeg_destroy_compress(m_pCompressStruct);
		delete m_pCompressStruct;
		m_pCompressStruct = NULL;
	}

	delete m_pErrorMgr;
	m_pErrorMgr = NULL;

	delete m_pDestination;
	m_pDestination = NULL;
#endif
}

ERRORCODE CJPEGCompressor::Init(StorageDevice* pDevice, int nWidth, int nHeight, int nColorComponents /*=3*/)
{
	// Make sure we are clear.
	FreeAll();

	// Remember the device.
	m_pDevice = pDevice;

	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old
	TRY
	{
		m_pCompressStruct = new jpeg_compress_struct;
		m_pErrorMgr = new CJPEGErrorManager;

		/* Initialize the JPEG compression object with default error handling. */
		m_pCompressStruct->err = m_pErrorMgr;
		jpeg_create_compress(m_pCompressStruct);

		/*
		// Initialize JPEG parameters.
		// Much of this may be overridden later.
		// In particular, we don't yet know the input file's color space,
		// but we need to provide some value for jpeg_set_defaults() to work.
		*/

		/* Return info about the image. */
		m_pCompressStruct->in_color_space = nColorComponents == 3 ? JCS_RGB : JCS_GRAYSCALE;
		m_pCompressStruct->input_components = nColorComponents;
		jpeg_set_defaults(m_pCompressStruct);

		m_pCompressStruct->image_width = nWidth;
		m_pCompressStruct->image_height = nHeight;

		/* Specify data destination for compression */
		m_pDestination = new CJPEGDestinationManager(m_pCompressStruct, pDevice);

		/* Start compressor */
		jpeg_start_compress(m_pCompressStruct, TRUE);
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Read;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
#else
// New
	// New
	m_Output.SetDevice(pDevice);
	m_Pegasus.SetOutput(&m_Output);

	long lStatus = m_Pegasus.StartJPEGCompression(nWidth, nHeight, nColorComponents);
	if (lStatus == ERR_NONE)
	{
		error = ERRORCODE_None;
	}
	else
	{
		error = ERRORCODE_Write;
	}
#endif

	return error;
}

ERRORCODE CJPEGCompressor::WriteLines(LPBYTE pLines, int nLineCount, BOOL Reversed /*FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old
	int nRowSize = (ImageWidth() * ColorComponents() + 3) & ~3;

#ifndef WIN32
	LPBYTE pAlternateRow = NULL;
#endif

	TRY
	{
#ifdef WIN32
		JSAMPROW pRow = pLines;
#else
		unsigned char __huge* pRow = pLines;
#endif

		// Do the write.
		while (nLineCount != 0)
		{
#ifdef WIN32
			if (jpeg_write_scanlines(m_pCompressStruct, (JSAMPARRAY)&pRow, 1) != 1)
#else
			BOOL fCrosses = FALSE;
			if (0x10000 - LOWORD(pRow) < nRowSize)
			{
				// The row crosses into the next segment. Copy the data to a local
				// row that we know is safe.
				if (pAlternateRow == NULL)
				{
					pAlternateRow = new BYTE[nRowSize];
				}
				hmemcpy(pAlternateRow, pRow, nRowSize);
				fCrosses = TRUE;
			}
			if (jpeg_write_scanlines(m_pCompressStruct,
											 (JSAMPARRAY)(fCrosses ? &pAlternateRow : (LPBYTE*)&pRow),
											 1) != 1)
#endif
			{
				return ERRORCODE_Write;
			}
			nLineCount--;
			pRow += nRowSize;
		}
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Write;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

#ifndef WIN32
	delete [] pAlternateRow;
#endif
#else
	// New
	if (Reversed == TRUE)
   {
      if (m_Pegasus.WriteJPEGLinesReversed(pLines, nLineCount) != ERR_NONE)
		   error = ERRORCODE_Write;
   }
   else
   {
      if (m_Pegasus.WriteJPEGLines(pLines, nLineCount) != ERR_NONE)
		   error = ERRORCODE_Write;
   }
#endif
	return error;
}

ERRORCODE CJPEGCompressor::Finish(void)
{
	ERRORCODE error = ERRORCODE_None;

#if 0
	// Old
	TRY
	{
		jpeg_finish_compress(m_pCompressStruct);
	}
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Write;
	}
	AND_CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL
#else
	// New
	m_Pegasus.FinishJPEGCompression();
#endif

	return error;
}

void CJPEGCompressor::Abort(void)
{
#if 0
	// Old
	if (m_pCompressStruct != NULL)
	{
		jpeg_abort_compress(m_pCompressStruct);
	}
#else
	// New
	m_Pegasus.AbortJPEGCompression();
#endif
}

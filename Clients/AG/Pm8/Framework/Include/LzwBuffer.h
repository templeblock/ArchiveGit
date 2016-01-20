/*
// LZW.H
// 
// Definitions for the LZW reading.
//
// $Header: /PM8/Framework/Include/LzwBuffer.h 1     3/03/99 6:15p Gbeddow $
//
// $Log: /PM8/Framework/Include/LzwBuffer.h $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 1     1/25/99 3:37p Rgrenfell
// LZW decompression translator which uses a memory buffer as input
//  
*/

#ifndef LZWBUFFER_H
#define LZWBUFFER_H

/*
// A "Pascal" string. We must use this because we can have zeros in the string.
*/
typedef struct
{
	USHORT		length;
	UCHAR			far *data;
} PSTRING;


class FrameworkLinkage LZWDataBufferStream
{
	public:
		// The constructor.  It takes the raw data pointed to the start of the
		// image data and the relative size of the buffer from the given location.
		LZWDataBufferStream( LPBYTE pRawData, long lBufferSize );

		// Read a block of image data from the buffer stream.
		bool Read( LPBYTE pDataBuffer, long lBufferSize, USHORT *puBytesRead );

		// Retrieve the amount of data read from the buffer.
		inline long GetBytesRead()
			{ return m_lDataRead; }

		// Informs the caller whether the image block terminator was read.
		inline bool HitImageBlockTerminator()
			{ return m_bHitImageTerminator; }

	private:
		// Stores the raw GIF file data.
		LPBYTE m_pRawDataBuffer;
		// Stores the size of the GIF data buffer.
		long m_lBufferSize;
		// Stores the current location within the buffer.
		long m_lDataRead;

		// Stores the size of the block currently being read.
		BYTE m_nBlockSize;
		// Stores how much of the current block has been read so far.
		BYTE m_nBlockDataRead;

		// Track whether the image block terminator was read.
		bool m_bHitImageTerminator;
};



class FrameworkLinkage LZWBufferParser
{
private:
	void * m_pDataBlock;
	ULONG m_lSkip;
	LPBYTE m_pBuffer;
	USHORT m_nSize;

	LZWDataBufferStream m_lzwDataStream;

	void write_current_string();
//	StorageDevicePtr m_pDevice;

//	short (*get_next_code)();
	bool m_bGIFTranslationDirection;

	void init_lzw_table();
	SHORT get_next_rcode();
	SHORT get_next_lcode();
	void string_from_code(SHORT code, PSTRING near * string);

public:
	LZWBufferParser(LPBYTE pInputBuffer, long lBufferSize, SHORT nBitWidth, BOOL bDir);
	~LZWBufferParser();
	void reset();
	int read(ULONG skip_size, LPBYTE read_buffer, USHORT read_size);
	BOOL valid()
		{ return m_pDataBlock != NULL; }
	// Provide the caller with how much data was read from the supplied buffer.
	inline long GetBytesRead()
		{ return m_lzwDataStream.GetBytesRead(); }
	inline bool HitImageBlockTerminator()
		{ return m_lzwDataStream.HitImageBlockTerminator(); }
};

typedef LZWBufferParser far *LZWBufferParserPtr;

#endif						// #ifndef LZWBUFFER_H

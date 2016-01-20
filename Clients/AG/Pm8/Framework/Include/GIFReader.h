/*	@doc
	@module GifReader.h - Definition of the RGifReader class |

			Definition of the <c RGifReader> class.

			This class provides an interface to a GIF file.  At this time it is
			purely a wrapper for reading the transparent index from a memory
			buffer.
	
	<cp>Copyright 1997 Brøderbund Software, Inc., all rights reserved
*/

#ifndef GIFREADER_H
#define GIFREADER_H

#pragma pack(push, 1)	// Single byte alignment necessary for reading GIF data.

/* @enum This is a set of values used to define the GIF block labels.
*/
enum EBlockLabel
{
	GIF_IMAGE_DESCRIPTOR	= 0x2C,	// @emem The image descriptor block.
	GIF_EXT_INTRODUCER	= 0x21,	// @emem A GIF Extension block.
	GIF_TRAILER				= 0x3B,	// @emem The trailer for a GIF image block.
	GIF_GRAPH_CONTRL_EXT	= 0xF9,	// @emem The Graphics control extension.
	GIF_PLAIN_TEXT_EXT	= 0x01,	// @emem A plain text extension.
	GIF_APP_EXT				= 0xFF,	// @emem An application specific extension.
	GIF_COMMENT_EXT		= 0xFE,	// @emem A comment extension.
	GIF_UNKNOWN				= 0x00	// @emem An unknown extension or label.
};

enum EPImageClearState
{
	CS_UNSPECIFIED = 0,
	CS_LEAVE_IMAGE,
	CS_ERASE_TO_BACKGROUND,
	CS_ERASE_TO_PREV_IMAGE
};

/*	@struct Provides a structured storage construct for GIF Header information.
*/
typedef struct
{
	BYTE GIF[3];		// @field Should always read as "GIF" once initialized.
	BYTE Version[3];	// @field Should hold the version of the GIF file.
} SGIFHeader;

/*	@struct Provides a structued storage construct for the global image information.
*/
typedef struct
{
	short nWidth;		// @field The maximum width of the image.
	short nHeight;		// @field The maximum height of the image.
	BYTE cScreenInfo;	// @field Bit flag specifying palette size and other information.
	BYTE cBackground;	// @field The image background index.
	BYTE cAspectRatio;	// @field The image aspect ratio.
} SGIFLogicalScreenDescriptor;

/*	@struct	Provides a structured storage construct for the image descriptor block.
*/
typedef struct
{
	short nLeftPosition;	// @field The left position of the image.
	short nTopPosition;	// @field The top position of the image.
	short nWidth;			// @field The width of the image.
	short nHeight;			// @field The height of the image.
	BYTE cPackedFields;	// @field Packed bits specifying specific attributes.
} SGIFImageDescriptor;

typedef struct
{
	BYTE Red;
	BYTE Green;
	BYTE Blue;
} SGIF_RGB;

typedef struct
{
	SGIF_RGB RGB[256];
} SGIFColorTable;

typedef struct
{
	BYTE cPackedField;
	short nDelayTime;
	BYTE nTransparentColorIndex;
} SGIFGraphicControlExtension;

typedef struct
{
	BYTE bAsciiCode[8];
	BYTE bBinaryCode[3];
} SGIFApplicationExtensionBlock;

typedef struct
{
	BYTE bBlockCode;
	short nLoopCount;
} SGIFNetscapeLoopBlock;

#pragma pack(pop)

#define IDESC_INTERLACED 0x40

/* This class provides a frame of image data and the information neccessary to
	render it to the screen.  Global data is not kept here, only relevant local
	image data.
 */
class FrameworkLinkage RGIFFrame
{
public:
	// Constructors.
	RGIFFrame();
	RGIFFrame(	const SGIFImageDescriptor &Descriptor,
					const SGIFColorTable *pColorTable = NULL,
					const short *pnDelayTime = NULL,
					const BYTE *pnTransparentColorIndex = NULL );

	// Destructor.  Perform clean up.
	~RGIFFrame()
		{ delete m_pColorTable; }

	// Set the image description for this frame.
	inline void SetDescriptor( const SGIFImageDescriptor &Descriptor )
	{ m_Descriptor = Descriptor; }

	// Set a local palette for this frame.
	inline void SetColorTable( const SGIFColorTable &ColorTable )
	{ 
		if (m_pColorTable == NULL)
			m_pColorTable = new SGIFColorTable;
		*m_pColorTable = ColorTable;
	}
	// Provides access to the local color palette.
	inline SGIFColorTable *GetColorTable()
		{ return m_pColorTable; }
	// Inform the caller if the image uses a local palette.
	inline bool HasLocalPalette()
		{ return m_pColorTable != NULL; }
	// Informs the caller of the size of the palette as specified for the frame.
	int GetPaletteSize();

	// Set the image data for the frame.
	inline void SetBitmaps( CBitmap * pBitmap, CBitmap * pMask )
		{ m_pBitmap = pBitmap; m_pMask = pMask; }

	// Provides the image data fro the frame.
	inline CBitmap * GetBitmap()
		{ return m_pBitmap; }
	inline CBitmap * GetMaskBitmap()
		{ return m_pMask; }

	// Provides the starting point of the frame within the logical screen.
	inline CPoint GetPosition()
		{ return CPoint( m_Descriptor.nLeftPosition, m_Descriptor.nTopPosition ); }
	// Provides the width of the dimensions within the logical screen.
	inline CSize GetSize()
		{ return CSize( m_Descriptor.nWidth, m_Descriptor.nHeight ); }

	// Specifies that the frame has a delay and how long it is.
	inline void SetDelay( short nDelay )
		{ m_bHasDelay = TRUE; m_nDelayTime = nDelay; }
	// Returns whether the frame has a delay.
	inline bool HasDelay()
		{ return m_bHasDelay; }
	// Inform the caller if the image has a delay.
	short GetDelay();

	// Specifies that the frame has a transparent index and what its value is.
	inline void SetTransparentIndex( BYTE nTransparentIndex )
		{ m_bHasTransparency = TRUE; m_nTransparentColorIndex = nTransparentIndex; }

	// Inform caller if the frame has transparency.
	inline bool IsTransparent()
		{ return m_bHasTransparency; }
	// Provides the transparent index for the frame.
	inline BYTE GetTransparentIndex()
		{ return m_nTransparentColorIndex; }

	// Specifies what to do with the image prior to showing the next frame.
	inline void SetImageClearState( EPImageClearState eClearState )
		{ m_eClearRequest = eClearState; }
	// Provides the frames clear state.
	inline EPImageClearState GetImageClearState()
		{ return m_eClearRequest; }

	// Informs if the image referenced was stored in interlaced format.
	inline bool InterlacedGIF()
		{ return ((m_Descriptor.cPackedFields & IDESC_INTERLACED) != 0); }

private:
	SGIFImageDescriptor m_Descriptor;
	SGIFColorTable *m_pColorTable;
	bool m_bHasDelay;
	short m_nDelayTime;
	bool m_bHasTransparency;
	EPImageClearState m_eClearRequest;
	BYTE m_nTransparentColorIndex;
	CBitmap *m_pBitmap;
//	LPBYTE m_pBitmap;
	CBitmap *m_pMask;
};



/*	@class	This class provides an interface to a GIF file.  At this time it is
				purely a wrapper for reading the transparent index from a memory
				buffer.
*/
class FrameworkLinkage RGifReader
{
	public:
		// @access Public methods

		// @cmember Default constructor.
		RGifReader();
		// @cmember Constucts a GIF interface and initializes the data buffer.
		RGifReader( void *pGIFData, int nBufferSize );

		// Delete any internally allocated data.
		~RGifReader();

		// @cmember Sets the data buffer to use for the GIF image.
		void SetDataBuffer( void *pGIFData, int nBufferSize );

		// Reads in the GIF data and builds a list of the image frames.
		bool ReadGIF();

		// @cmember Reads the transparent index information from the attached
		//				GIF buffer.
		bool ReadTransparentIndex( int *pnTransparentIndex, int *pnGifPaletteSize );

		// @cmember Restarts the GIF data at the beginning.
		inline void Rewind()
		{ m_nByteCount = 0; m_pGIFData = m_pGIFBuffer; }

		// Checks to see that we are at the beginning of the buffer.
		inline bool AtStartOfBuffer()
			{ return (m_pGIFData == m_pGIFBuffer); }

		// @cmember Returns whether this is an animated GIF image.
		BOOLEAN IsAnimatedGIF();

		// Provides the number of times the animation loops before halting.
		inline short GetLoopCount()
			{ return m_nLoopCount; }

		// Provides the number of frames within the animated GIF.
		inline short GetFrameCount()
			{ return m_FrameList.GetCount(); }

		// Provides the delay for the frame specified before the next frame should be drawn.
		short GetDelay( int nAnimFrame );

		// Provides the frame clear request.
		EPImageClearState GetFrameClearRequest( int nAnimFrame );

		// Provides the size of the GIF "Screen".
		inline CSize GetGIFScreenSize()
			{ return CSize( m_ScreenDescriptor.nWidth, m_ScreenDescriptor.nHeight ); }

		// Get the position of the specified frame.
		CPoint GetFramePosition( int nAnimFrame );
		// Get the size of the specified frame.
		CSize GetFrameSize( int nAnimFrame );

		// Creates a Windows bitmap from the frame's image data.
//		bool CreateBitmap( int nAnimFrame, CBitmap *pbmFrame, CBitmap *pbmMask, double fScalingFactor );
		// Provides a pointer to the bitmaps for the frame.
		bool GetBitmaps( int nAnimFrame, CBitmap **ppBitmap, CBitmap **ppMask );
		// Creates a Windows palette from the frame's color table data.
		bool CreatePalette( int nAnimFrame, CPalette *ppalFrame );

	protected:
		// @access Protected methods

		// @cmember Verifies that the header given is from a GIF89a saved image.
		bool Verify89a( const SGIFHeader &GIFHeader );
		// Verifies that this is a legitamate GIF image file.
		bool VerifyGIF( const SGIFHeader &GIFHeader );

		// @cmember Reads the GIF87a or GIF89a tag from the buffer.
		bool ReadHeader(SGIFHeader *pGIFHeader);
		// @cmember Reads the image description block from the buffer.
		bool ReadLogicalScreenDescriptor( int *pnPaletteSize );
		// @cmember Reads a block label from the file and translates it into a
		//				token we can process on.
		bool ReadToken( EBlockLabel *peToken );
		// @cmember Reads the transparency value from the Graphics Control extension.
		bool ReadTransparency( short *pnTransparentIndex );
		// @cmember Reads the delay value from the Graphics Control extension.
		bool ReadDelay( short *pnDelay );
		// @cmember Reads the block containing the image data.
		bool ReadImageBlock();
		// Reads an extension block from the buffer.
		bool ReadExtension();
		// @cmember Reads a color map from the buffer.
		bool ReadColorMap( SGIFColorTable *pColorMap, int nPaletteSize );
		// @cmember Reads a block not recognized by this application.
		bool ReadUnsupportedBlock();
		// @cmember Reads the image descriptor data from the stream.
		bool ReadImageDescriptor( SGIFImageDescriptor *pImageDescriptor );
		// Reads the graphic control extension data from the buffer.
		bool ReadGraphicControlExtension();
		// Reads the application extension block from the buffer.
		bool ReadApplicationExtension();

		// Creates a windows color palette from the color table provided.
		bool BuildPalette( CPalette *pPalette, SGIFColorTable *pColorTable, int nTransIndex, int nPalSize );
		// Provides the size of the global palette
		int GetGlobalPaletteSize();

		// Creates the bitmap for the current frame.
		void CreateFrameBitmaps( LPBYTE pRawImageData );

	private:
		// @access Private data

		// Stores the screen information, That is the viewing area, etc.
		SGIFLogicalScreenDescriptor m_ScreenDescriptor;

		// @cmember Stores the fixed pointer to the buffer.
		BYTE *m_pGIFBuffer;
		// @cmember Our active buffer pointing to the current data byte to be processed.
		BYTE *m_pGIFData;
		// @cmember Stores the size of the buffer.
		int m_nGIFBufferSize;
		// @cmember Stores the number of Bytes we've read so far.
		int m_nByteCount;

		// The global color palette
		SGIFColorTable *m_pGlobalColorTable;

		// Stores the current GIF Frame in construction.
		RGIFFrame *m_pGifFrame;
		// Stores the list of frame images for the GIF.
		CPtrList m_FrameList;
		// Specifies the number of times the animation should loop.  0 is forever.
		short m_nLoopCount;

		// Stores whether a loop block was found.
		bool m_bFoundLoopBlock;

		// @access Pivate methods

		// @cmember Read one byte of data from the buffer.
		BYTE ReadOneByteOfData();
		// @cmember Reads a block of data from the buffer.
		bool ReadXBytesOfData( BYTE *pBuffer, int nBytesToRead );
		// @cmember Reads a 16bit integer worth of data from the buffer.
		bool ReadOneShortOfData( short *pShortValue );

		// Skips the specified number of bytes without bothering to read them.
		bool SkipXBytesOfData( int nBytesToSkip );

		// @cmember Checks to see if there is at least one byte of data to read.
		bool OutOfData();
		// @cmember Checks to see if there is the desired number of bytes to read.
		bool OutOfData(int nBytesDesired);

		// Returns the amount of space left in the GIF buffer.
		inline int SpaceLeftInBuffer()
			{ return (m_nGIFBufferSize - m_nByteCount); }
};

#endif	// GIFREADER_H

//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _MGXBM_H_
#define _MGXBM_H_

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes

#include "appdata.h"

typedef TCHAR FILENAME[MAX_PATH];

#define CLASS_EXPORT __declspec(dllexport)

typedef enum 		// File Types
{
	BFT_UNKNOWN = 0,
	BFT_PPCOMPOUND,
	BFT_PP,
	BFT_TIFF,
	BFT_TARGA,
	BFT_GIF,
	BFT_BMP,
	BFT_DIB,
	BFT_PCX,
	BFT_EPS,
	BFT_DCS,
	BFT_JPEG,
	BFT_PHOTOCD,
	BFT_SCITEX,
	BFT_AVI,
	BFT_PP4
} BITMAP_FILE_TYPE;

typedef enum
{
	BEC_errNone = 0,
	BEC_errOpen,
	BEC_errMemory,
	BEC_errFileRGBPTCMYK,
	BEC_errFileCMYKPTRGB,
	BEC_errPTNoMon,
	BEC_errCompress,
	BEC_errFileRead,
	BEC_errFrameRead,
	BEC_errCompressFastBits,
	BEC_errBadTIFF,
	BEC_errTIFFVersion,
	BEC_errUnsupportedTIFFCompress,
	BEC_errUnsupportedTIFFPhoto,
	BEC_errUnsupportedTIFFFill,
	BEC_errUnsupportedTIFFOrient,
	BEC_errTIFFSamples,
	BEC_errFrameOpen,
	BEC_errBadFormat,
	BEC_errDiskSpace,
	BEC_errWriteProtect,
	BEC_errWrite,
	BEC_errWriteFile,
	BEC_errPTMismatch,
	BEC_errGettingPT,
	BEC_errBadVersion,
	BEC_errBadEncoding,
	BEC_errBadBitsPerPixel,
	BEC_errBadNumColorImages,
	BEC_errInvalidOpCode,
	BEC_errBadSignature,
	BEC_errNoData,
	BEC_errBadColorMapSize,
	BEC_errBadInterleave,
	BEC_errPCDToolkit,
} BITMAP_ERROR_CODE;  

class CLASS_EXPORT CReadBitmap
{
public:
	enum SampleMode {
		modeNoSampling,
		modeSampleRect,
		modeSampleRes};

    // Constructors
    CReadBitmap(LPTSTR lpFileName, BITMAP_FILE_TYPE FileType = BFT_UNKNOWN, LPCMSINFO pcmsInfo = NULL);
    CReadBitmap(CFile *pFile, BITMAP_FILE_TYPE FileType, BOOL bEscapable = FALSE, LPCMSINFO lpCmsInfo = NULL);
	//CReadBitmap(const CReadBitmap &other, CFile *pFile = NULL, BITMAP_FILE_TYPE FileType = BFT_UNKNOWN);

    // Destructor
    ~CReadBitmap();                       

	// overridables
	virtual void ProgressBegin(int nPasses) {} 
	// current progress function - return TRUE to cancel operation
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return FALSE; }
	virtual void ProgressEnd() {}
	// internal set error code
	virtual void SetError(BITMAP_ERROR_CODE errCode) { m_errCode = errCode; }
			 
	// PP specific overridables
	virtual BOOL ReadCmdList(	CFile *pFile,
								LPLIST lpPacketList,
								DWORD dwSize,
								BOOL fReadLoadPacketOnly) { return TRUE; }
	virtual void DestroyPacketList(LPLIST lpPacketList) {}
	virtual void PP5CheckCmsInfo(	LPCMSINFO pCmsInfo,
									LPLIST lpPacketList,
									FRMTYPEINFO SrcTypeInfo,
									BOOL fWantOriginal,
									BOOL fReadNativeType) {}
 	virtual BOOL DoPCDResOverride(LPLIST lpPacketList, int PhotoCDResOverride) { return FALSE; }

	// read in header information
	BOOL		ReadHeader();
	BOOL		ReadPacketList(BOOL fReadLoadPacketOnly = FALSE);

    // Read in an object list
    // lpFileType and lpDataType are for return values
    BOOL    ReadObjectList(LPOBJECTLIST lpObjList);
    // TypeInfo specifies the data type to create when reading
    BOOL    ReadObjectList(LPOBJECTLIST lpObjList, FRMTYPEINFO TypeInfo);

    // Read in an object
    // lpFileType and lpDataType are for return values
    LPOBJECT    ReadObject();
    // TypeInfo specifies the data type to create when reading
    LPOBJECT    ReadObject(FRMTYPEINFO TypeInfo);

    // Read in a frame
    // lpFileType and lpDataType are for return values
    LPFRAME     ReadFrame();
    // TypeInfo specifies the data type to create when reading
    LPFRAME     ReadFrame(FRMTYPEINFO TypeInfo);

	// read a PhotoCD thumbnail
	LPFRAME ReadPCDThumbnail( short nImgNo = 0);
	LPFRAME ReadPCDThumbnail( FRMTYPEINFO outType, short nImgNo = 0 );

	// read a tiff header
	struct _tiffhdr * ReadTiffHeader(CFile *pTheFile = NULL);
	void FreeTiffHeader( struct _tiffhdr * lpHeader );
	BOOL WriteTiffData( LPFRAME lpSrcFrame, int xSrc, int ySrc);
	struct _tiffhdr * ReadObjHeader();
	struct _tiffhdr * PP5ReadTiffHeader();

    // capability functions
    BOOL CanReaderSample();
    BOOL CanCrop();

	// Get Error Code
	BITMAP_ERROR_CODE GetError() { return m_errCode; }

	BITMAP_FILE_TYPE GetFileType() { return m_FileType; } // file type
	FRMDATATYPE GetDataType() { return m_cmsInfo.src.DataType; } // file data type
	int GetBitsPerPixel() { return m_BitsPerPixel; };
	int GetFileWidth() { return m_iWidth; }	// file width
	int GetFileHeight(){ return m_iHeight; }	// file height
	int GetFileResolution() { return m_iRes; }	// file resolution
	int GetSampledResolution() { return m_iCurrentRes; }	// file resolution
	int GetFileDepth();		// file depth
	BOOL HasCurrentVersion() { return m_fHasCurrent; }
	BOOL HasOriginalVersion() { return m_fHasOriginal; }

	// get packet list read in
	void GetPacketList(LPLIST PacketList);

	void GetCMSSrcTypeInfo(LPFRMTYPEINFO lpSrcTypeInfo) { *lpSrcTypeInfo = m_cmsInfo.src; }

    // set sampling rectangle for low-res
    void SetSampling(LPRECT lpRect);
    // set sampling resolution for low-res
    void SetSampling(int res);
    // set cropping rectangle for fastbits
    void SetCropping(LPRECT lpRect);
    // set reader specific options
    void SetOptions(LPVOID lpOptions) { m_lpOptions = lpOptions; }
	// to enable/disable command list reading
	void SetReadCmdList(BOOL fEnable) { m_fReadCmdList = fEnable; }
	// to set the photo cd resolution override value
	void SetPhotoCDResOverride(int PhotoCDResOverride) { m_PhotoCDResOverride = PhotoCDResOverride; }
	// to set whether to open line art as grayscale
	void SetLineArtAsGray(BOOL fLineArtAsGray) { m_fLineArtAsGray = fLineArtAsGray; }
	// to set whether to create undos with the undo flag
	void SetNoUndo(BOOL fNoUndo) { m_fNoUndo = fNoUndo; }
	// set one bit masks for line art
	void SetOneBitMask(BOOL fOneBitMask) { m_fOneBitMask = fOneBitMask; }
	// set storage to be used for a file read
	void SetStorage(LPSTORAGE pStorage) { m_pStorage = pStorage; }

	// set read combine data option
	// only used for ReadFrame() and ReadObject()
	// if fReadCombinedData is FALSE, the first object we be use
	// otherwise the the objects will be combined with the base
	void SetReadCombinedData(BOOL fReadCombinedData);

protected:
	CFile *OpenFile(UINT nOpenFlags = CFile::modeRead);
	void CloseFile(CFile *pFile);

	// initialize member variables
	void Init(LPCMSINFO pcmsInfo);

	BOOL SetupTypes(LPFRMTYPEINFO pInType,
					LPFRMTYPEINFO pOutType,
					BOOL fGraymap = FALSE,
 					LPVOID lpXform = NULL);
	BOOL CheckCmsInfo(CMSINFO *pCmsInfo, FRMTYPEINFO inType, LPVOID lpXform = NULL);

	BOOL ReadData(FRMTYPEINFO TypeInfo);
	LPOBJECT CreateObjectFromFrame(LPFRAME lpFrame);
	LPOBJECT GetSingleObject();

	// useful function for combining an object list into a frame
	LPOBJECT CombineToBase(LPOBJECTLIST lpObjList);
	LPFRAME SampleFrame(LPFRAME lpFrame);
	LPOBJECT SampleObject(LPOBJECT lpObject);
   	LPOBJECT ReadObjectBlock(CFile *pFile, LPTR lpData, DWORD dwBytes,
								BOOL fBase, FRMTYPEINFO SrcTypeInfo);
	LPFRAME ReadFrameBlock(CFile *pFile, DWORD dwBytes, UINT SampleMode,
						LPRECT lpSampleRect, int SampleRes,
						FRMTYPEINFO outType, BOOL bEscapable, LPCMSINFO lpCmsInfo);
	BOOL pp5_read_header(CFile *pFile, LPTR lpData,
						LPBOOL lpReadObjectBlock,
						LPINT lpNumObjects, LPFRMTYPEINFO lpSrcTypeInfo);
 	BOOL PP5ReadHdr();
	BOOL PP5ReadPacketList(BOOL fReadLoadPacketOnly = FALSE);
	BOOL pp5_read_packet_list(CFile *pFile, LPLIST lpPacketList, BOOL fReadLoadPacketOnly);

	// the readers
	BOOL CTRead();
 	BOOL PP5Read();
	BOOL PP4Read();
	BOOL TiffRead();
	BOOL BMPRead();
	BOOL GIFRead();
	BOOL TGARead();
	BOOL JPGRead();
	BOOL PCDRead();
	BOOL PCXRead();

protected:
    LPTSTR          m_lpFileName;           // in - file to read
	CFile*			m_pFile;			    // in - file to read from

    LPVOID          m_lpOptions;			// in - write options
	CMSINFO			m_cmsInfo;				// in - Color Management Info.
	BOOL			m_bEscapable;		    // in - is this function escapable

    UINT		    m_SampleMode;           // in - sample data to rArea
    BOOL            m_fCrop;                // in - crop data to rArea
    RECT            m_rArea;                // in - for fSample or fCrop
    DITHER_TYPE     m_DitherType;    		// in - default dithering
    BITMAP_FILE_TYPE m_FileType;			// in - file type
	BOOL			m_fReadCombined;		// in - combine or take first object
	LIST			m_PacketList;		    // in - packet list read
	BOOL			m_fReadCmdList;		    // in - whether to read cmd list
	BOOL			m_fWantOriginal;	    // in - want original image
	int				m_SampleRes;		    // in - sampling resolution
	int				m_PhotoCDResOverride;	// in - PhotoCD resolution override

    BOOL            m_fLineArtAsGray;       // in - open line art as grayscale
	BOOL			m_fNoUndo;				// in - Setup Undos
	BOOL			m_CMSEnabled;			// in - CMS turned on?
	BOOL			m_fOneBitMask;			// in - use 1-bit mask for line art
	BOOL			m_bNoHiResRebuild;		// in - do hi-res rebuilds
	BOOL			m_fTGACMYK;				// in - are 32-bit targas CMYK?

	int				m_BitsPerPixel; 		// out - bit depth in file
    LPFRAME         m_lpFrame;              // out - newly created frame
	OBJECTLIST		m_ObjList;		        // out - new object list
    int             m_iWidth;               // out - width of image in file
    int             m_iHeight;              // out - height of image in file
    int             m_iRes;                 // out - resolution of image in file
	BOOL			m_fReadOriginal;	    // out - whether the original image was read
	int				m_iCurrentRes;		    // out - current image resolution
	BOOL			m_fHasCurrent;			// out - does file contain a current version?
	BOOL			m_fHasOriginal;			// out - does file contain an original version?

	BITMAP_ERROR_CODE 	m_errCode;			// error code
	LPSTORAGE		m_pStorage;				// storage to read from
	LFIXED			m_xrate;		    	// internal, for scaling
	LFIXED			m_yrate;				
	BOOL			m_fReadNativeType; 		// Want a specific type?
	BOOL			m_fOpenedStorage;		// did we open a storage?

	friend class CReadTiff;
};

// write bitmap class
class CLASS_EXPORT CWriteBitmap
{
public:
    // Constructors
    CWriteBitmap(LPTSTR lpFileName, BITMAP_FILE_TYPE FileType = BFT_UNKNOWN);
    CWriteBitmap(CFile *pFile, BITMAP_FILE_TYPE FileType, BOOL bEscapable = FALSE);

    // Destructor
    ~CWriteBitmap() {};                      

	// overridables
	virtual void ProgressBegin(int nPasses) {} 
	// current progress function - return TRUE to cancel operation
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return FALSE; }
	virtual void ProgressEnd() {}
	virtual void SetError(BITMAP_ERROR_CODE errCode) { m_errCode = errCode; }

	// PP specific overridables
	virtual BOOL EPSWrite(BOOL fDoDCS) { return FALSE; }
	virtual BOOL WriteCmdList(CFile *pFile, LPLIST lpPacketList) { return TRUE; }
	virtual LONG GetXformDataSize( LONG lCrc, LPTSTR pMandM ) { return 0; }
	virtual BOOL GetXformData( LONG lCrc, LPTR pData ) { return FALSE; }
	virtual void GetCommandInfo(LPLIST lpPacketList, LPVOID lpOptions, LPBOOL fLowRes, LPBOOL fFastBits, LPBOOL fPhotoCD, LPBOOL bSaveCmdList)
	{
		*fLowRes = FALSE;
		*fFastBits = FALSE;
		*fPhotoCD = FALSE;
		*bSaveCmdList = FALSE;
	}

    // Write out an object list
    BOOL    WriteObjectList(LPOBJECTLIST lpObjList, FRMDATATYPE DataType = FDT_NONE);
    // Write out an object
    BOOL    WriteObject(LPOBJECT lpObject, FRMDATATYPE DataType = FDT_NONE);
    // Write out a frame
    BOOL    WriteFrame(LPFRAME lpFrame, FRMDATATYPE DataType = FDT_NONE);
	// Update the packet list for a file
	BOOL 	UpdatePacketList(LPLIST lpPacketList);

    // capability functions
    BOOL CanWriteDestArea();

	// Get Error Code
	BITMAP_ERROR_CODE GetError() { return m_errCode; }
	// get storage used for a file write
	LPSTORAGE GetStorage() { return m_pStorage; }

    // set area of source to write
    void SetSourceArea(LPRECT lpRect);
    // set reader specific options
    void SetOptions(LPVOID lpWriteOptions);
	// setup a packet list to write
	void SetPacketList(LPLIST PacketList);
	// Should we update/create a thumbnail... default false
 	void SetUpdateThumb(BOOL bUpdateThumb){m_bUpdateThumb = bUpdateThumb;}
	// to set colormap to be used when write palette color images
	void SetColorMap(LPFRMCOLORMAP lpColorMap) { m_lpColorMap = lpColorMap; }
	// set bits per pixel for palette color save
	void SetBitsPerPixel(int nBitsPerPixel) { m_BitsPerPixel = nBitsPerPixel; }
	// set storage to be used for a file write
	void SetStorage(LPSTORAGE pStorage) { m_pStorage = pStorage; }
	// set dither type for line art and palette color save
	void SetDitherType(DITHER_TYPE DitherType) { m_DitherType = DitherType; }

protected:
	// initialize member variables
	void Init();

	CFile *OpenFile(UINT nOpenFlags = CFile::modeWrite|CFile::modeCreate);
	void CloseFile(CFile *pFile, BOOL fSuccess);

	BOOL SetDefaultOptions();

    // Write out a frame or object
    BOOL    WriteData(LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType = FDT_NONE);
    FRMDATATYPE  GetSaveDataType(FRMTYPEINFO TypeInfo);
	BOOL WriteObjectBlock(CFile *pFile, LPTR lpBuf, LPOBJECT lpObject,
	  		BOOL fSaveAlpha, BOOL fCompressed, BOOL bWritePT, int res, LPFRMCOLORMAP lpColorMap);
	BOOL pp5_write_image(CFile *pFile, LPTR lpData, BOOL fOriginal, BOOL fLowRes);
	BOOL WriteBlockInfo(CFile *pFile, DWORD dwBytes, int OpCode);
	BOOL WriteTextBlock(CFile *pFile, LPTR lpBuf,
								CTextObject *pText);
	BOOL WriteFrameBlock(CWriteBitmap *pWriter, CFile *pFile, LPFRAME lpFrame,
			  		FRMDATATYPE DataType, BOOL fCompressed,
			  		int OpCode, BOOL bEscapable, BOOL bWritePT,
					int res, LPCOLORMAP lpColorMap);

	// The writers
	BOOL CTWrite();
	BOOL PP5Write();
	BOOL PPFWrite();
	BOOL PP4Write();
	BOOL TiffWrite();
	BOOL BMPWrite();
	BOOL GIFWrite();
	BOOL TGAWrite();
	BOOL JPGWrite();
	BOOL PCXWrite();
	// special update function for PP5
	BOOL PP5Update();

protected:
    LPTSTR           	m_lpFileName;     	// in - file to write
	CFile*				m_pFile;		  	// in - file to write to
	LPSTORAGE			m_pStorage;		   	// in - storage to write to

    BITMAP_FILE_TYPE 	m_FileType;	  		// in - file type to write
    LPVOID          	m_lpOptions;		// in - write options
    BOOL            	m_bEscapable;     	// in - only if using oFile
    LPFRAME         	m_lpFrame;        	// in - frame to write
    LPOBJECTLIST    	m_lpObjList;      	// in - object list to write
    FRMDATATYPE     	m_DataType;     	// in - data type in file
    BOOL            	m_fSrcArea;       	// in - do we have a source area
    RECT            	m_rSrcArea;       	// in - area to write of source
	LIST				m_PacketList;	  	// in - packet list to write
	BOOL 				m_bUpdateThumb;	  	// in - save a thumbnail also??
	LPFRMCOLORMAP 		m_lpColorMap;	  	// in - colormap to write palette color
	int					m_BitsPerPixel;		// in - for 4-bit palette color

	DITHER_TYPE			m_DitherType;		// in - dither type for use when
											// saving line art of palette color
	BOOL				m_CMSEnabled;		// in - CMS turned on?

	BITMAP_ERROR_CODE	m_errCode;
	BOOL				m_fOpenedStorage;		// did we open a storage?

	friend class CWriteTiff;
};

/********************************************************************* 
 **** PhotoCD Options:
 *********************************************************************/
typedef struct _pcdoptions
{
	int PhotoCDFormat; // IDC_PCDSINGLE or IDC_PCDRGB
	int PhotoCDRes;		// IDC_PCD_BASE, ...
	short	BrightButton;
	short	RedButton;
	short	GreenButton;
	short	BlueButton;
	short	SatButton;
	BOOL	OutOfGamut;
	BOOL	SubtractSBA;
	FILENAME LutName;    // extended name (not needed by reader)
	FILENAME LutFileName;// filename
	BOOL	fHaveArea;
	FRECT	rArea;
	CMSINFO cmsInfo;
} PCDOPTIONS;
typedef PCDOPTIONS FAR *LPPCDOPTIONS;

/********************************************************************* 
 **** EPS/DCS Options:
 *********************************************************************/
typedef struct _epsoptions
{
	BOOL 	bEPSPromptUser;		//prompt user for options before saving?
	BOOL 	bDCSPromptUser;		//prompt user for options before saving?
	int		PreviewBPP;
	int		CompositeBPP;
	FILENAME	ClipPath;
} EPSOPTIONS;
typedef EPSOPTIONS FAR *LPEPSOPTIONS;

/********************************************************************* 
 **** TIFF Options:
 *********************************************************************/
typedef struct _tiffoptions{
BOOL 	bTIFFPromptUser;		// prompt user for options before saving?
BOOL 	TIFFCompress;			// use compression
BOOL 	TIFFDiff;				// use delta compression
BOOL	bWritePT;				// Write the Precision transform.
BOOL	bSaveMask;				// Save Mask?
} TIFFOPTIONS;
typedef TIFFOPTIONS FAR *LPTIFFOPTIONS;

/********************************************************************* 
 **** TARGA Options:
 *********************************************************************/
typedef struct _tgaoptions{
BOOL 	bTGAPromptUser;		// prompt user for options before saving?
BOOL	bSaveMask;				// Save Mask?
} TGAOPTIONS;
typedef TGAOPTIONS FAR *LPTGAOPTIONS;

/********************************************************************* 
 **** AVI Options:
 *********************************************************************/
typedef struct _avioptions{
BOOL 	bAVIPromptUser;		// prompt user for options before saving?
int		iAVIPosType;		// Add to end, insert replace frame
int 	iAVISizingType;		// Size or Clip
int 	AVIPosition;		// Frame position choosen (not to be saved, for passing only)
int 	AVILastPosition;	// Last Frame position possible (not to be saved, passing only)
} AVIOPTIONS;
typedef AVIOPTIONS FAR *LPAVIOPTIONS;

/********************************************************************* 
 **** JPEG Options:
 *********************************************************************/
typedef struct _jpegoptions{
BOOL 	bJPEGPromptUser;		// prompt user for options before saving?
UINT	nCompressionFactor;		// ammount of compression from 2-255
UINT	nSubsampling;			// indicates the type of subsampling desired
} JPEGOPTIONS;
typedef JPEGOPTIONS FAR *LPJPEGOPTIONS;

/********************************************************************* 
 **** PPFF Options:
 *********************************************************************/
typedef struct _pp40options{
BOOL 	bPPFFPromptUser;		// prompt user for options before saving?
BOOL 	PPFFCompress;
BOOL	bSaveMask;
BOOL	bSaveProxy;
BOOL	bSaveCmdList;
BOOL	bWritePT;				// Write the Precision transform.
BOOL	bSaveLink;
BOOL    bSaveAsCompoundStg;     // Applicable Only if >=PP6
} PPFFOPTIONS;
typedef PPFFOPTIONS FAR *LPPPFFOPTIONS;

typedef struct _pp50openoptions{
int		PhotoCDResOverride;
} PPFFOPENOPTIONS;
typedef PPFFOPENOPTIONS FAR *LPPPFFOPENOPTIONS;

typedef struct _tiffhdr {
	// Generic bitmap values
	int npix, nlin, bpp, InPlanes, InDepth, OutDepth, FileBPL;
	BOOL bNegate, bCompressed, bColormap, bOKtoPreload;
	long lResolution;
	LPTR lpFileLine;
	LPTR lpConvertLine;
	void (FAR *lpExpandProc)(struct _tiffhdr FAR *lpTH, LPTR lpIn, int iInByteCount, LPTR lpAlphaLine);
	void (FAR *lpSampleProc)( LPTR input, int inskip, LPTR output, int outskip, int count, LFIXED rate );
	// TIFF Specific values
	void (FAR *lpPredictorProc)( LPTR lpImage, int iCount, int SamplesPerPixel );
	long StripsPerImage;
	LPLONG StripByteCounts;
	LPLONG StripOffsets;
	long ImagePos;
	long tof;
	FRMTYPEINFO outType;
	FRMTYPEINFO inType;
	LPVOID		pXform;
	LONG		size;
	TCHAR		szXformName[256];
	int 		SamplesPerPixel;
	int			ExtraSamples;
	LPTR		lpAlphaLine;

} TIFFHEADER, FAR * LPTIFFHEADER;

#endif // _MGXBM_H_


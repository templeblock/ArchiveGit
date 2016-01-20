#if !defined(_TIFFTAGS)
#define _TIFFTAGS

/*Baseline Tiff:*/
#define TAG_Artist 0x13b
#define TAG_BitsPerSample 0x102
#define TAG_CellLength 0x109
#define TAG_CellWidth 0x108
#define TAG_ColorMap 0x140
#define TAG_Compression 0x103
#define TAG_Copyright 0x8298
#define TAG_DateTime 0x132
#define TAG_ExtraSamples 0x152
#define TAG_FillOrder 0x10a
#define TAG_FreeByteCounts 0x121
#define TAG_FreeOffsets 0x120
#define TAG_GrayResponseCurve 0x123
#define TAG_GrayResponseUnit 0x122
#define TAG_HostComputer 0x13c
#define TAG_ImageDescription 0x10e
#define TAG_ImageLength 0x101
#define TAG_ImageWidth 0x100
#define TAG_Make 0x10f
#define TAG_MaxSampleValue 0x119
#define TAG_MinSampleValue 0x118
#define TAG_Model 0x110
#define TAG_NewSubfileType 0xfe
#define TAG_Orientation 0x112
#define TAG_PhotometricInterpretation 0x106
#define TAG_PlanarConfiguration 0x11c
#define TAG_ResolutionUnit 0x128
#define TAG_RowsPerStrip 0x116
#define TAG_SamplesPerPixel 0x115
#define TAG_Software 0x131
#define TAG_StripByteCounts 0x117
#define TAG_StripOffsets 0x111
#define TAG_SubfileType 0xff
#define TAG_Threshholding 0x107
#define TAG_XResolution 0x11a
#define TAG_YResolution 0x11b

/*CCITT BiLevel Encodings:*/
#define TAG_T4Options 0x124
#define TAG_T6Options 0x125
	
/*Document Storage and Retrieval:*/
#define TAG_DocumentName 0x10d
#define TAG_PageName 0x11d
#define TAG_PageNumber 0x129
#define TAG_XPosition 0x11e
#define TAG_YPosition 0x11f

/*Differencing Predictor:*/
#define TAG_Predictor 0x13d
	
/*Tiled Images:*/
#define TAG_TileWidth 0x142
#define TAG_TileLength 0x143
#define TAG_TileOffsets 0x144
#define TAG_TileByteCounts 0x145
		
/*CMYK Images:*/
#define TAG_InkSet 0x14c
#define TAG_NumberOfInks 0x14e
#define TAG_InkNames 0x14d
#define TAG_DotRange 0x150
#define TAG_TargetPrinter 0x151

/*Halftone Hints:*/
#define TAG_HalftoneHints 0x141

/*Data Sample Format:*/
#define TAG_SampleFormat 0x153
#define TAG_SMinSampleValue 0x154
#define TAG_SMaxSampleValue 0x155

/*RGB Image Colorimetry:*/
#define TAG_WhitePoint 0x13e
#define TAG_PrimaryChromaticities 0x13f
#define TAG_TransferFunction 0x12d
#define TAG_TransferRange 0x156
#define TAG_ReferenceBlackWhite 0x214

/*YCbCr Images:*/
#define TAG_YCbCrCoefficients 0x211
#define TAG_YCbCrSubSampling 0x212
#define TAG_YCbCrPositioning 0x213

/*JPEG Compression:*/
#define TAG_JPEGProc 0x200
#define TAG_JPEGInterchangeFormat 0x201
#define TAG_JPEGInterchangeFormatLength 0x202
#define TAG_JPEGRestartInterval 0x203
#define TAG_JPEGLosslessPredictors 0x205
#define TAG_JPEGPointTransforms 0x206
#define TAG_JPEGQTables 0x207
#define TAG_JPEGDCTTables 0x208
#define TAG_JPEGACTables 0x209

#define TIF_BYTE        (1)
#define TIF_ASCII       (2)
#define TIF_SHORT       (3)
#define TIF_LONG        (4)
#define TIF_RATIONAL    (5)
#define TIF_SBYTE       (6)
#define TIF_UNDEFINED   (7)
#define TIF_SSHORT      (8)
#define TIF_SLONG       (9)
#define TIF_SRATIONAL   (10)
#define TIF_FLOAT       (11)
#define TIF_DOUBLE      (12)

typedef struct {
	BYTE Format[2];		/*II or MM*/
	WORD Zaphod;		/*42*/
	DWORD IFDOffset;	/*where the IFD is*/
} TIFF_HEAD;

typedef struct {
	WORD tag;
	WORD type;
	DWORD count;
	DWORD val_offset;
} DIR_ENTRY;
	/* a directory entry in the ifd*/

#endif

/*®PL1¯®FD1¯®BT0¯®TP0¯*/
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// The format of the 'MGX_IMAGE' clipboard type is simply the following:
//
//	MGX_IMAGEHDR			The MGX_IMAGE header
//	MGX_IMAGENAME			The header for the image filename
//	A Null terminated path name	The image filename string
//	MGX_IMAGENAME			The header for the mask filename
//	A Null terminated path name	The mask filename string
//

// The MGX_IMAGE header
typedef struct tagMGX_IMAGEHDR
	{
	WORD	wHeaderSize;	// size of the header in bytes (10)
	WORD	wVersion;	// The version number MGX_IMAGE_VERSION
	WORD	wImageType;	// The Image type (MGX_IMAGEMASKTYPE,
				// MGX_IMAGETYPE, MGX_MASKTYPE)
	LONG	lDataSize;	// bytes of data following header
	} MGX_IMAGEHDR;
typedef MGX_IMAGEHDR far *LPMGX_IMAGEHDR;

// The name header
typedef struct tagMGX_IMAGENAME
	{
	WORD	wNameHeaderSize; // size of the name header in bytes (6)
	WORD	wDataType;	// The data type (MGX_IMAGEBMP, MGX_IMAGETIFF,
				// MGX_IMAGEPCX, MGX_IMAGEGIF, MGX_IMAGETARGA,
				// MGX_IMAGEEPS, MGX_MASKPP
	WORD	wNameLength;	// Length of the filename
	} MGX_IMAGENAME;
typedef MGX_IMAGENAME far *LPMGX_IMAGENAME;

// Current MGX_IMAGE version
#define MGX_IMAGE_VERSION	1

// MGX_IMAGE types
#define MGX_IMAGEMASKTYPE	0 // Image and mask names. Image always first.
#define MGX_IMAGETYPE		1 // Image name only.
#define MGX_MASKTYPE		2 // Mask name only.

// Image data types
#define MGX_IMAGEBMP		0
#define MGX_IMAGETIFF		1
#define MGX_IMAGEPCX		2
#define MGX_IMAGEGIF		3
#define MGX_IMAGETARGA		4
#define MGX_IMAGEEPS		5

// Mask data types
#define MGX_MASKPP		100


//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _OBJIO_H_
#define _OBJIO_H_

#define OBJ_HDR_SIZE	14
//#define OBJ_DATA_SIZE   41
#define OBJ_DATA_SIZE   46
#define OBJ_VERSION     0x0103

typedef struct {
	WORD	wByteOrder;		// TIF_II or TIF_MM
	WORD	wVersion;		// File Version
	WORD	wNumObjects;	// Number of Objects in file
	LONG	lObjectSize;	// Size of object data structure
	LONG	lObjectStart;	// Offset to where objects start
} OBJHEADER, _far * LPOBJHEADER;

typedef struct {
	WORD	wObjectType;
	RECT	rObject;
	WORD	wOpacity;
	WORD	wMergeMode;
	WORD	wGroupID;
	WORD	wFileType;
	WORD	wInvertAlpha;
	RECT	rAlpha;
	LONG	lDataOffset;
	LONG	lAlphaOffset;
	BOOL	fSelected;
} OBJDATA, _far * LPOBJDATA;

typedef struct {
	BOOL	bInvert;
	RECT	rMask;
	LONG	lDataOffset;
	LONG	lAlphaOffset;
} OBJOFFSETS, _far *LPOBJOFFSETS;

#endif // _OBJIO_H_

/*
// $Workfile: THUNKMSG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// The definitions needed to use the PMW 16 bit DLL thunking server.
// This file (and only this file) should be included in clients.
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/THUNKMSG.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:02   FRED
// Initial revision.
// 
//    Rev 1.0   22 Dec 1995 16:46:54   FRED
// Initial revision.
// 
//    Rev 1.0   22 Dec 1995 16:27:58   FRED
// Initial revision.
// 
//    Rev 1.6   29 Sep 1995 13:03:48   FRED
// Removed PMWTT thunking support... it's too slow!
// 
//    Rev 1.5   27 Sep 1995 14:42:00   FRED
// BDE Thunking
// 
//    Rev 1.4   25 Sep 1995 11:37:10   FRED
// TrueType speedups
// 
//    Rev 1.3   22 Sep 1995 10:46:22   FRED
// Address and Sender Books
// 
//    Rev 1.2   21 Sep 1995 12:27:48   FRED
// Position Dialog
// 
//    Rev 1.1   20 Sep 1995 09:57:40   FRED
// PMWTT.DLL support
// 
//    Rev 1.0   15 Sep 1995 14:58:50   FRED
// Initial revision.
*/ 

#ifndef __THUNKMSG_H__
#define __THUNKMSG_H__

#pragma warning(disable: 4200)
#pragma pack(1)

#ifdef WIN32
#define __FLAT__
#endif

#include "idapi.h"

// These strings are used to construct the class and window
// names for the thunk communication window.

#define PmwThunkWindowName		"PmwThunkWindow"
#define PmwThunkClassName		"PmwThunkWindow"

// These strings are used to construct the class and window
// names for the client communication window. The number filled
// in is the context number assigned to the client.

#define PmwClientWindowName	"PmwThunkClientWindow%d"
#define PmwClientClassName		"PmwThunkClientWindow%d"

// Define WM_COPYDATA and COPYDATASTRUCT. They are not automatically
// defined in a WIN 3.1 environment. The WM_COPYDATA message only
// works user WOW or WIN32s.

#ifndef WIN32

#ifndef WM_COPYDATA
#define WM_COPYDATA 0x004A
#endif

#ifndef COPYDATASTRUCT
typedef struct tagCOPYDATASTRUCT
{
    DWORD dwData;
    DWORD cbData;
    LPVOID lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;
#endif

#endif

// Message parsers.

#define	THUNK_MESSAGE_CONTEXT(m)	((WORD)(((m)>>16)&0xffff))
#define	THUNK_MESSAGE_CLASS(m)		((WORD)(((m)>>8)&0xff))
#define	THUNK_MESSAGE_OP(m)			((WORD)(((m)>>0)&0xff))
#define	THUNK_MESSAGE(x, c, o)		((DWORD)((((x)&0xffff)<<16)|(((c)&0xff)<<8)|(((o)&0xff)<<0)))

// Defined message classes.

enum
{
	THUNK_CLASS_Control					=		0,
	THUNK_CLASS_Pmwdlgs					=		1,
	THUNK_CLASS_Bde						=		2
};

// Defined Control operations.

enum
{
	THUNK_OP_CONTROL_Nop					=		0,

	THUNK_OP_CONTROL_Connect			=		1,
	THUNK_OP_CONTROL_Disconnect		=		2
};
	
// Defined Pmwdlgs operations.

enum
{
	THUNK_OP_PMWDLGS_Nop								=		0,
	
	THUNK_OP_PMWDLGS_Initialize					=		1,
	THUNK_OP_PMWDLGS_Terminate						=		2,
	THUNK_OP_PMWDLGS_CreatePositionDialog		=		3,
	THUNK_OP_PMWDLGS_DestroyPositionDialog		=		4,
	THUNK_OP_PMWDLGS_CreateAddressBookDialog	=		5,
	THUNK_OP_PMWDLGS_DestroyAddressBookDialog	=		6,
	THUNK_OP_PMWDLGS_CreateSenderBookDialog	=		7,
	THUNK_OP_PMWDLGS_DestroySenderBookDialog	=		8,
	
	THUNK_OP_PMWDLGS_EndDialog						=		100,
	THUNK_OP_PMWDLGS_NewPositionData				=		101,
	THUNK_OP_PMWDLGS_NewAddressBookData			=		102,
	THUNK_OP_PMWDLGS_NewSenderBookData			=		103
};
	
// Data structures for Pmwdlgs operations.

struct THUNK_DATA_PMWDLGS_EndDialog
{
	LONG lResult;
};

// Defined Bde operations.

enum
{
	THUNK_OP_BDE_Nop								=		0,
	
	THUNK_OP_BDE_DbiGetErrorContext			=		1,
	THUNK_OP_BDE_DbiGetErrorEntry				=		2,
	THUNK_OP_BDE_DbiGetErrorInfo				=		3,
	THUNK_OP_BDE_DbiGetErrorString			=		4,
	THUNK_OP_BDE_DbiInit							=		5,
	THUNK_OP_BDE_DbiExit							=		6,
	THUNK_OP_BDE_DbiOpenDatabase				=		7,
	THUNK_OP_BDE_DbiCloseDatabase				=		8,
	THUNK_OP_BDE_DbiOpenTable					=		9,
	THUNK_OP_BDE_DbiGetCursorProps			=		10,
	THUNK_OP_BDE_DbiGetFieldDescs				=		11,
	THUNK_OP_BDE_DbiCloseCursor				=		12,
	THUNK_OP_BDE_DbiGetNextRecord				=		13,
	THUNK_OP_BDE_DbiGetPriorRecord			=		14,
	THUNK_OP_BDE_DbiSetToBegin					=		15,
	THUNK_OP_BDE_DbiSetToKey					=		16,
	THUNK_OP_BDE_DbiGetField					=		17,
	THUNK_OP_BDE_DbiPutField					=		18,

	THUNK_OP_BDE_ReadNextLabelRecord			=		100
};
	
// Data structures for Bde operations.

typedef struct
{                          // Virtual Table properties
   DBITBLNAME     szName;                 // table name (no extension, if it can be derived)
   UINT16         iFNameSize;             // Full file name size 
   DBINAME        szTableType;            // Driver type 
   UINT16         iFields;                // No of fields in Table
   UINT16         iRecSize;               // Record size (logical record)
   UINT16         iRecBufSize;            // Record size (physical record)
   UINT16         iKeySize;               // Key size
   UINT16         iIndexes;               // Number of indexes
   UINT16         iValChecks;             // Number of val checks
   UINT16         iRefIntChecks;          // Number of Ref Integrity constraints
   UINT16         iBookMarkSize;          // Bookmark size
   BOOL16         bBookMarkStable;        // Stable book marks
   short          eOpenMode;              // ReadOnly / RW
   short          eShareMode;             // Excl / Share
   BOOL16         bIndexed;               // Index is in use
   INT16          iSeqNums;               // 1: Has Seqnums; 0: Has Record#
                                          // <0 (-1, -2, ...): None (e.g. SQL)
   BOOL16         bSoftDeletes;           // Supports soft deletes
   BOOL16         bDeletedOn;             // If above, deleted recs seen
   UINT16         iRefRange;              // Not used
   short          exltMode;               // Translate Mode
   UINT16         iRestrVersion;          // Restructure version number
   BOOL16         bUniDirectional;        // Cursor is uni-directional
   short          eprvRights;             // Table  rights
   UINT16         iFmlRights;             // Family rights      
   UINT16         iPasswords;             // Number of Aux passwords
   UINT16         iCodePage;              // Codepage (0 if unknown)
   BOOL16         bProtected;             // Table is protected by password
   UINT16         iTblLevel;              // Driver dependent table level
   DBINAME        szLangDriver;           // Language driver name
   BOOL16         bFieldMap;              // Field map active
   UINT16         iBlockSize;             // Physical file blocksize in K 
   BOOL16         bStrictRefInt;          // Strict referential integrity
   UINT16         iFilters     ;          // Number of filters
   BOOL16         bTempTable   ;          // Table is a temporary table
   UINT16         iUnUsed[16];
} CURProps16;

typedef CURProps16 far *pCURProps16;

typedef struct
{                          // Field Descriptor
   UINT16         iFldNum;                // Field number (1..n)
   DBINAME        szName;                 // Field name
   UINT16         iFldType;               // Field type
   UINT16         iSubType;               // Field subtype (if applicable)
   INT16          iUnits1;                // Number of Chars, digits etc
   INT16          iUnits2;                // Decimal places etc.
   UINT16         iOffset;                // Offset in the record (computed)
   UINT16         iLen;                   // Length in bytes (computed)
   UINT16         iNullOffset;            // For Null bits (computed)
   short          efldvVchk;              // Field Has vcheck (computed)
   short          efldrRights;            // Field Rights (computed)
} FLDDesc16;

typedef FLDDesc16 far *pFLDDesc16;

typedef struct
{                          // Record properties
   UINT32         iSeqNum;                // When Seq# supported only
   UINT32         iPhyRecNum;             // When Phy Rec#s supported only
   BOOL16         bRecChanged;            // Not used
   BOOL16         bSeqNumChanged;         // Not used
   BOOL16         bDeleteFlag;            // When soft delete supported only
} RECProps16;

typedef RECProps16 far *pRECProps16;


struct THUNK_DATA_BDE_DbiGetErrorContext
{
	INT16 eContext;
	DBIMSG szContext;
};

struct THUNK_DATA_BDE_DbiGetErrorEntry
{
	UINT16 uEntry;
	UINT32 ulNativeError;
	DBIMSG szError;
};

struct THUNK_DATA_BDE_DbiGetErrorInfo
{
	BOOL16 bFull;
	DBIErrInfo ErrInfo;
};

struct THUNK_DATA_BDE_DbiGetErrorString
{
	DBIResult rslt;
	DBIMSG szError;
};

struct THUNK_DATA_BDE_DbiOpenDatabase
{
	DBITBLNAME szDbName;
	DBITBLNAME szDbType;
	short eOpenMode;
	short eShareMode;
	hDBIDb hDb;
};

struct THUNK_DATA_BDE_DbiCloseDatabase
{
	hDBIDb hDb;
};

struct THUNK_DATA_BDE_DbiOpenTable
{
	hDBIDb hDb;
	DBITBLNAME szTableName;
	DBITBLNAME szDriverType;
	DBITBLNAME szIndexName;
	DBITBLNAME szIndexTagName;
	UINT16 iIndexId;
	short eOpenMode;
	short eShareMode;
	short eXltMode;
	BOOL16 bUniDirectional;
	hDBICur hCursor;
};

struct THUNK_DATA_BDE_DbiGetCursorProps
{
	hDBICur hCursor;
	CURProps16 CurProps;
};

struct THUNK_DATA_BDE_DbiGetFieldDescs
{
	hDBICur hCursor;
	FLDDesc16 FldDesc[0];
};

struct THUNK_DATA_BDE_DbiCloseCursor
{
	hDBICur hCursor;
};

struct THUNK_DATA_BDE_DbiGetNextRecord
{
	hDBICur hCursor;
	short eLock;
	RECProps16 RecProps;
	BYTE RecBuf[0];
};

struct THUNK_DATA_BDE_DbiGetPriorRecord
{
	hDBICur hCursor;
	short eLock;
	RECProps16 RecProps;
	BYTE RecBuf[0];
};

struct THUNK_DATA_BDE_DbiSetToBegin
{
	hDBICur hCursor;
};

struct THUNK_DATA_BDE_DbiSetToKey
{
	hDBICur hCursor;
	short eSearchCond;
	BOOL16 bDirectKey;
	UINT16 iFields;
	UINT16 iLen;
	BYTE Buf[0];
};

struct THUNK_DATA_BDE_DbiGetField
{
	hDBICur hCursor;
	UINT16 iField;
	BOOL16 bBlank;
	UINT16 wRecLen;
	BYTE Buf[0];
};

struct THUNK_DATA_BDE_DbiPutField
{
	hDBICur hCursor;
	UINT16 iField;
	UINT16 wRecLen;
	BYTE Buf[0];
};

struct THUNK_DATA_BDE_ReadNextLabelRecord
{
	hDBICur hCursor;
	WORD wRecordLength;
	char szName[42];
	short nPaperType;
	short nOrientation;
	short nPageWidth;
	short nPageHeight;
	short nCornerType;
	short nColumns;
	short nRows;
	short nLeftMargin;
	short nTopMargin;
	short nWidth;
	short nHeight;
	short nWidthPlusGap;
	short nHeightPlusGap;
	short nRadiusX;
	short nHalfPagePerfSkip;
	short nHalfPageRowCount;
	short nInternational;
	char szDescription[42];
};

#ifdef WIN32
#undef __FLAT__
#undef BOOL
#endif

#pragma pack()
#pragma warning(default: 4200)

#endif

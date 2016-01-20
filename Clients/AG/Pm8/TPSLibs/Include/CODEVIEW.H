/****************************************************************************
 *
 * $Header: /PM8/TPSLibs/Include/CODEVIEW.H 1     3/03/99 6:35p Gbeddow $
 *
 *	Header Name: CODEVIEW.H
 *	Description: Definitions for some internal CodeView structures.
 *	Version:     v0.00
 *	Author:      Robert L. Pyron
 *	Date:        950203
 *	Copyright:   (c) 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *	Revision History:
 *
 * $Log: /PM8/TPSLibs/Include/CODEVIEW.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 4     12/09/98 1:17p Lwilson
// Converted all references from WIN to _WINDOWS.
 * 
 * 1     11/06/96 7:21a Mike_houle
    
    2     2/22/95 10:19a Mgd
    windows requires packed structures
    
    1     2/03/95 5:51p Bob
    find procname from CodeView information
 *
 ****************************************************************************
 */

#ifndef  _codeview_h
#define  _codeview_h

/*===========================================================================
 * INCLUDE FILES
 *===========================================================================
 */

/*===========================================================================
 * SYMBOLIC CONSTANTS
 *===========================================================================
 */
 
//
// Windows requires packed structures...
#if defined(_WINDOWS)
#pragma pack(1)
#endif

	//
 	// EWM_ = Executable Walker Metric
 	//

#define  EWM_VALIDNB          0x3930424eL		/* Supported signature */
#define  EWM_XREADCURRENT     0x7fffffffL		/* Read from current position */
#define  EWM_MAXEXESIZE       0x7ffffffeL		/* Maximum executable file size */
#define	EMW_VALIDSYMSEG		0x00000001L

	//
 	// sstXXX = SubSection Type
 	//

#define  sstModule            0x120
#define  sstTypes             0x121
#define  sstPublic            0x122
#define  sstPublicSym         0x123
#define  sstSymbols           0x124
#define  sstAlignSym          0x125 
#define  sstSrcLnSeg          0x126
#define  sstSrcModule         0x127 
#define  sstLibraries         0x128
#define  sstGlobalSym         0x129
#define  sstGlobalPub         0x12a
#define  sstGlobalTypes       0x12b
#define  sstMPC               0x12c
#define  sstSegMap            0x12d
#define  sstSegName           0x12e
#define  sstPreComp           0x12f
#define  sstOffsetMap16       0x131
#define  sstOffsetMap32       0x132
#define  sstFileIndex         0x133
#define  sstStaticSym         0x134

	//
 	// S_ = Symbol Type
 	//

#define	S_COMPILE		0x0001	// Compile flags symbol
#define	S_REGISTER		0x0002	// Register variable
#define	S_CONSTANT		0x0003	// Constant symbol
#define	S_UDT				0x0004	// User-defined Type
#define	S_SSEARCH		0x0005	// Start search
#define	S_END				0x0006	// End block, procedure, with, or thunk
#define	S_SKIP			0x0007	// Skip - Reserve symbol space
#define	S_CVRESERVE		0x0008	// Reserved for CodeView internal use
#define	S_OBJNAME		0x0009	// Specify name of object file
#define	S_ENDARG			0x000a	// Specify end of arguments in function symbols
#define	S_COBOLUDT		0x000b	// Microfocus COBOL user-defined type
#define	S_MANYREG		0x000c	// Many register symbol
#define	S_RETURN			0x000d	// Function return description
#define	S_ENTRYTHIS		0x000e	// Description of this pointer at entry
#define	S_BPREL16		0x0100	// BP relative 16:16
#define	S_LDATA16		0x0101	// Local data 16:16
#define	S_GDATA16		0x0102	// Global data 16:16
#define	S_PUB16			0x0103	// Public symbol 16:16
#define	S_LPROC16		0x0104	// Local procedure start 16:16
#define	S_GPROC16		0x0105	// Global procedure start 16:16
#define	S_THUNK16		0x0106	// Thunk start 16:16
#define	S_BLOCK16		0x0107	// Block start 16:16
#define	S_WITH16			0x0108	// With start 16:16
#define	S_LABEL16		0x0109	// Code label 16:16
#define	S_CEXMODEL16	0x010a	// Change execution model 16:16
#define	S_VFTPATH16		0x010b	// Virtual function table path descriptor 16:16
#define	S_REGREL16		0x010c	// Specify 16:16 offset relative to arbitrary register.
#define	S_BPREL32		0x0200	// BP relative 16:32
#define	S_LDATA32		0x0201	// Local data 16:32
#define	S_GDATA32		0x0202	// Global data 16:32
#define	S_PUB32			0x0203	// Public symbol 16:32
#define	S_LPROC32		0x0204	// Local procedure start 16:32
#define	S_GPROC32		0x0205	// Global procedure start 16:32
#define	S_THUNK32		0x0206	// Thunk start 16:32
#define	S_BLOCK32		0x0207	// Block start 16:32
#define	S_WITH32			0x0208	// With start 16:32
#define	S_LABEL32		0x0209	// Label 16:32
#define	S_CEXMODEL32	0x020a	// Change execution model 16:32
#define	S_VFTPATH32		0x020b	// Virtual function table path descriptor 16:32
#define	S_REGREL32		0x020c	// 16:32 offset relative to arbitrary register
#define	S_LTHREAD32		0x020d	// Local Thread Storage data
#define	S_GTHREAD32		0x020e	// Global Thread Storage data
#define	S_LPROCMIPS		0x0300	// Local procedure start MIPS
#define	S_GPROCMIPS		0x0301	// Global procedure start MIPS
#define	S_PROCREF		0x0400	// Reference to a procedure
#define	S_DATAREF		0x0401	// Reference to data
#define	S_ALIGN			0x0402	// Page align symbols

//-------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------

typedef struct _EXESYMSTART
{
   DWORD        dwSignature;           // NBnn signature
   DWORD        dwStartOffsetFromEOF;  // Start of symbolic information 
                                       // as an offset from the end of
                                       // the executable file
}
   EXESYMSTART;   // structure definition: Start of symbolic information 

typedef struct _EXESYMHDR
{
   DWORD        dwSignature;        // NBnn signature 
   DWORD        dwDirOffset;        // Subsection directory offset 
}
   EXESYMHDR;  // structure definition: Executable symbols header

typedef struct _DIRHDR
{
   WORD         wDirHdrLen;         // Directory header length 
   WORD         wDirEntryLen;       // Directory entry length 
   DWORD        dwDirEntries;       // Directory entry count 
   DWORD        dwNextDirOffset;    // Offset of the next subsection
                                    // directory from base (currently not used)
   DWORD        dwFlags;            // Flags (currently not used) 
}
   DIRHDR;     // structure definition: Subsection directory header

typedef struct _DIRENTRY
{
   WORD         wSst;               // Sub-section type 
   WORD         wModIdx;            // Module index (1 based, 
                                    // 0xFFFF non-module specific) 
   DWORD        dwSubSectionOffset; // Table offset from lfaBase 
   DWORD        dwSubSectionLength;  // Number of bytes in subsection 
}
   DIRENTRY;   // structure definition: Subsection directory entry


//
// (0x0120) sstModule
//
// This describes the basic information about an object module including
// code segments, module name, and the number of segments for the modules
// that follow.  Directory entries for sstModules precede all other
// subsection directory entries.
// 
//    2           2     2     2     *           *
//    ovlNumber   iLib  cSeg  Style SegInfo[]   Name[]
//
// NOTE BOB -- This structure describes only the fixed portion at the
//	start of the directory entry.  Remaining data must be extracted
//	under program control.
//
typedef struct _SST_MODULE 
{
	WORD	ovlNumber;		// Overlay number
	WORD	iLib;				// Index into sstLibraries subsection if this module
								// was linked from a library
	WORD	cSeg;				// Count of the number of code segments this module
								// contributes to
	WORD	Style;			// Debugging style for this module.  Currently only 
								// "CV" is defined.  A module can have only one 
								// debugging style.  If a module contains debugging
								// information in an unrecognized style, the
								// information will be discarded.
//	MYSEGINFO	SegInfo[];	// Detailed information about each segment that code
								// is contributed to.   This is an array of cSeg count
								// segment information descriptor structures.
//	BYTE	Name[];			// Length-prefixed name of module

}
	SST_MODULE ;


//
// MYSEGINFO
//
// SegInfo is a structure that describes each segment to which a module
// contributes code.  It is formatted as follows:
//
//		2		2		4			4
//		Seg	pad	offset	cbSeg
//
typedef struct _MYSEGINFO
{
	WORD	wSegment;	// Segment that this structure describes
	WORD	pad;			// Padding to maintain alignment  This field is reserved
							// for future use and must be emitted as zeroes.
	DWORD	wOffset;		// Offset in segment where the code starts
	DWORD	cbSeg;		// Count of the number of bytes of code in the segment
}
	MYSEGINFO;


//
// (0x0125) sstAlignSym
// 
// CVPACK writes the remaining unpacked symbols for a module back to the
// executable in a subsection of this type.  All symbols have been padded
// to fall on a long word boundary and the lexical scope linkage fields
// have been initialized.
//
// NOTE BOB -- No structure definition necessary. This section has the
// format of a $$SYMBOLS segment (see CodeView documentation).
//


//
// (0x0127) sstSrcModule
// 
// This table describes the source line number to addressing mapping
// information for a module.  The table permits the description of a
// module containing multiple source files with each source file
// contributing code to one or more code segments.  The base addresses of
// the tables described below are all relative to the beginning of the 
// sstSrcModule table.
// 
// 	Module header
// 	Information for source file 1
// 	Information for segment 1
// 	Information for segment 2
// 	...
// 
// 	Information for source file 2
// 	Information for segment 1
// 	Information for segment 2
// 	...
// 
// The module header structure describes the source file and code segment
// organization of the module.
// 
//    2     2     4*cFile       8*cSeg        2*cSeg
//    cFile cSeg  baseSrcFile[] start/end[]   seg[]
//
// NOTE BOB -- This structure describes only the fixed portion at the
//	start of the directory entry.  Remaining data must be extracted
//	under program control.
//
typedef struct _SST_SRCMODULE
{
   WORD        wFiles;             // Source files in table 
   WORD        wSegments;          // Segments receiving code from module
}
   SST_SRCMODULE;  // structure definition: Source module header 

typedef struct _SRCFILHDR
{
   WORD         wSegments;		// Segments receive code from this source file
   WORD         wReserved;    // Reserved (must be 0) 
}
   SRCFILHDR;  // structure definition: Source file header 

typedef struct _SRCLINHDR
{
   WORD         wSegment;     // Segment index for this list 
   WORD         wLinePairs;   // Line segment offset pairs in list
}
   SRCLINHDR;  // structure definition: Source line header 


// 
// (0x0129) sstGlobalSym
// 
// This subsection contains globally compacted symbols.  The format of the
// table is a header specifying the symbol and address hash functions, the
// length of the symbol information, the length of the symbol hash
// function data, and the length of address hash function data.
// 
// This is followed by the symbol information followed by the symbol hash
// tables followed by the address hash tables.  When the pack utility
// writes the sstGlobals subsection,  each symbol is zero-padded such that
// the following symbol starts on a long boundary and the length field is
// adjusted by the pad count.  Note that symbol and/or address hash data
// can be discarded and the globally packed symbols linearly searched. 
// Hash function index 0 means no hash data.  See Section 7.5 for more
// information about the hashing functions.
// 
// The header has the following format:
// 
//    2        2        4        4           4
//    symhash  addrhash cbSymbol cbSymHash   cbAddrHash
// 
// NOTE BOB -- Remaining data must be extracted under program control.
// 
typedef struct _SST_GLOBALSYM 
{
	WORD			wSymHash;
	WORD			wAddrHash;
	DWORD			cbSymbol;
	DWORD			cbSymHash;
	DWORD			cbAddrHash;
}
	SST_GLOBALSYM;  // structure definition: GlobalSym header 


// (0x012d) sstSegMap
// 
// This table contains the mapping between the logical segment indices used
// in the symbol table and the physical segments where the program was
// loaded There is one sstSegMap per executable or DLL.
// 
// 
//       2     cSeg        count of the number of segment descriptors in table
//       2     cSegLog     count of the number of logical segment descriptors
//       20    SegDesc 0   First segment descriptor
//       ...
//       20    SegDesc N   cSeg'th segment descriptor
// 
typedef struct _SST_SEGMAP 
{
	WORD			cSeg;			// The total number of segment descriptors
	WORD			cSegLog;		// The total number of logical segments.  All
									// group descriptors follow the logical segment
									// descriptors.  The number of group descriptors
									// is given by cSeg - cSegLog.
//	MYSEGDESC	SegDescN[];	// Array of segment descriptors.  Information
									// about a logical segment can be found by using
									// logical segment number - 1 as an index into
									// this array.  The 1 is subtracted because the
									// logical segment number is 1 based.
}
	SST_SEGMAP ;


// 
// MYSEGDESC
// 
// Each element of the segment descriptor array has the following format:
// 
// 	2		2		2		2		2			2				4			4
// 	flags	ovl	group	frame	iSegName	iClassName	offset	cbseg
//
// The descriptor flags bit field flags are described in CodeView
// documentation.
// 
typedef struct _MYSEGDESC
{
	WORD	flags;		// descriptor flags bit field.  See below for details.
	WORD	ovl;			// The logical overlay number.
	WORD	group;		// The group index into the descriptor array.  The group
							// index must either be 0 or cSegLog <= group < cSeg.
	WORD	frame;		// This value has different meanings depending upon the
							// values of fAbs and fSel in the flags bit array and ovl
							// (see CodeView documentation)
	WORD	iSegName;	// The byte index of the segment or group name in the
							// sstSegName table.  A value of  0xffff indicates
							// there is no name.
	WORD	iClassName;	// The byte index of the class name in the sstSegName
							// table.  A value of  0xffff indicates there is no name.
	DWORD	offset;		// Byte offset of the logical segment within the
							// specified physical segment.  If fGroup is set in
							// flags, offset is the offset of the group in the
							// physical segment. Currently, all groups define
							// physical segments so offset will be zero for groups.
	DWORD	cbSeg;		// Byte count of the logical segment or group.
}
	MYSEGDESC;


//	
// (0x0133) sstFileIndex
// 
// This subsection contains an list of all of the sources files that
// contribute code to any module (compiland) in the executable.  File
// names are partially qualified relative to the compilation directory.
// 
//    2     2     2 * cMod    2 * cModules   4 * cRef    *
//    cMod  cRef  ModStart    cRefCnt        NameRef     Names
//	
// NOTE BOB -- This structure describes only the fixed portion at the
//	start of the directory entry.  Remaining data must be extracted
//	under program control.
//	
typedef struct _SST_FILEINDEX
{
	WORD		cMod;			// Count of the number of modules in the executable
	WORD		cRef;			// Count of the total number of file name references
//	WORD		ModStart[ cMod ];		// Array of indices into the NameOffset
											// table for each module.  Each index is
											// the start of the file name references
											// for each module.
//	WORD		cRefCount[ cMod ];	// Number of file name references per module
//	DWORD		NameRef[ cRef ];		// Array of offsets into the Names
											// table.  For each module, the offset
											// to first referenced file name is at
											// NameRef[ModStart] and continues for
											// cRefCnt entries.
//	char		Names[];					// List of zero terminated file names. 
											// Each file name is partially
											// qualified relative to the
											// compilation directory.
}
	SST_FILEINDEX;


//---------------------------------------------------------------------------
// Structures for extracting data from a $$SYMBOLS segment.
//---------------------------------------------------------------------------

typedef	struct _SYMSEGMENTHDR
{
   DWORD        dwSignature;
}
	SYMSEGMENTHDR;

typedef	struct _SYMRECORDHDR
{
	WORD	wLength;		// Length of record, excluding the length field
	WORD	wType;		// Type of symbol
// byte	data[];		// Data specific to each symbol format
}
	SYMRECORDHDR;


//
// (0x0005) Start Search
// 
// These records are always the first symbol records in a module's $$SYMBOL
// section.
// 
// There is one Start Search symbol for each segment (PE section) to which
// the module contributes code. Each Start Search symbol contains the
// segment (PE section) number and $$SYMBOL offset of the record of the
// outermost lexical scope in this module that physically appears first in
// the specified segment of the load image.  This referenced symbol is the
// symbol used to initiate context searches within this module.  The Start
// Search symbols are inserted into the $$SYMBOLS table by CVPACK and must
// not be emitted by the language processor.
// 
//    2        2           4        2
//    length   S_SSEARCH   sym off  segment
//
typedef struct _SYM_SSEARCH      // 0x0005   S_SSEARCH      Start search
{
	WORD	wLength;			// Length of record, excluding the length field
	WORD	wType;			// S_SSEARCH      
	DWORD	dwFirst;			// $$SYMBOL offset of the procedure or thunk
								// record for this module that has the lowest
								// offset for the specified segment.  See
								// Section 1.2 on lexical scope linking.
	WORD	wSegment;		// Segment (PE section) that this Start Search refers to
}
	SYM_SSEARCH;


// 
// (0x0009) Object File Name
// 
// This symbol specifies the name of the object file for this module.
// 
//    2        2           4           *
//    length   S_OBJNAME   signature   name
// 
typedef struct _SYM_OBJNAME      // 0x0009   S_OBJNAME      Specify name of object file
{
	WORD	wLength;			// Length of record, excluding the length field
	WORD	wType;			// S_OBJNAME      
	DWORD	dwSignature;	// Signature for the CodeView information contained
								// in this module.  If the object file contains
								// precompiled types, then the signature will be
								// checked against the signature in the LF_PRECOMP
								// type record contained in the $$TYPES table for the
								// user of the precompiled types. The signature check
								// is used to detect recompilation of the supplier of
								// the precompiled types without recompilation of all
								// of the users of the precompiled types. The method
								// for computing the signature is unspecified. It
								// should be sufficiently robust to detect failures to
								// recompile.
	BYTE	name[1];			// Length prefixed name of the object file without any
								// path information prepended to the name.
}
	SYM_OBJNAME;


// 
// (0x0101) Local Data 16:16
// 
// These symbols are used for data that is not exported from a module.  In
// C/C++, symbols that are declared static are emitted as Local Data
// symbols.  Symbols that are emitted as Local Data cannot be moved by
// CVPACK into the global symbol table for the executable file.
// 
//    2        2           2        2        2     *
//    length   S_LDATA16   offset   segment  @type name
// 
// 
// (0x0102) Global Data Symbol 16:16
// 
// This symbol record has the same format as the Local Data 16:16 except
// that the record type is S_GDATA16.    For C/C++, symbols that are not
// specifically declared static are emitted as Global Data Symbols and can
// be compacted by CVPACK into  the global symbol table.
// 
// 
// (0x0103) Public Symbol 16:16
// 
// This symbol has the same format as the Local Data 16:16 symbol.  Its use
// is reserved to symbols in the Publics table that is emitted by the
// linker  into the Symbol and Type OMF portion of the executable file. 
// 
typedef struct _SYM_LDATA16      // 0x0101   S_LDATA16      Local data 16:16
{
	WORD	wLength;			// Length of record, excluding the length field
	WORD	wType;			// S_LDATA16 or S_GDATA16 or S_PUB16        
	WORD	wOffset;			// Offset portion of the symbol address
	WORD	wSegment;		// Segment portion of the symbol address
	WORD	wTypeIndex;		// Type index of the symbol
	BYTE	name[1];			// Length-prefixed name of symbol
}
	SYM_LDATA16;

typedef SYM_LDATA16 SYM_GDATA16; // 0x0102   S_GDATA16      Global data 16:16
typedef SYM_LDATA16 SYM_PUB16;   // 0x0103   S_PUB16        Public symbol 16:16


// 
// (0x0104) Local Procedure Start 16:16
// 
// This symbol record defines local (file static) procedure definitions. 
// For C/C++, functions that are declared static to a module are emitted
// as Local Procedure symbols.  Functions not specifically declared static
// are emitted as Global Procedures (see below).
// 
//    2        2        4        4     4     2           2  
//    length   symbol   pParent  pEnd  pNext procLength  debugStart  -->
// 
//       2        2        2        2           1     *
//       debugEnd offset   segment  @proctype   flags name
// 
// 
// (0x0105) Global Procedure Start 16:16
// 
// This symbol is used for procedures that are not specifically declared
// static to a module.
// 
// The format is the same as the Local Procedure Start 16:16 symbol (see above.)
// 
typedef struct _SYM_LPROC16      // 0x0104   S_LPROC16      Local procedure start 16:16
{
	WORD	wLength;			// Length of record, excluding the length field
	WORD	wType;			// S_LPROC16 or S_GPROC16      
	DWORD	pParent;			// See the section on lexical scope linking
	DWORD	pEnd;				// See the section on lexical scope linking
	DWORD	pNext;			// See the section on lexical scope linking
	WORD	wProcLength;	// Length in bytes of this procedure
	WORD	wDebugStart;	// Offset in bytes from the start of the procedure to
								// the point where the stack frame has been set up. 
								// Frame and register variables can be viewed at this
								// point.
	WORD	wDebugEnd;		// Offset in bytes from the start of the procedure to
								// the point where the procedure is ready to return and
								// has calculated its return value, if any.  Frame and
								// register variables can still be viewed.
	WORD	wOffset;			// Offset portion of the procedure address
	WORD	wSegment;		// Segment portion of the procedure address
	WORD	wProcTypeIndex;	// Type index of the procedure type record
	BYTE	flags;			// Procedure flags:
								// 	 fpo			:1	 true if function has frame pointer omitted
								// 	 interrupt	:1	 true if function is interrupt routine
								// 	 return	 	:1	 true if function performs far return
								// 	 never	 	:1	 true if function never returns
								// 	 unused	 	:4	 
	BYTE	cbName;			// Length-prefixed ...
//	BYTE	name[];			// ... name of procedure
}
	SYM_LPROC16;

typedef SYM_LPROC16 SYM_GPROC16; // 0x0105   S_GPROC16      Global procedure start 16:16


// 
// (0x0204) Local Procedure Start 16:32
// 
// This symbol record defines local (file static) procedure definition. 
// For C/C++, functions that are declared static to a module are emitted
// as Local Procedure symbols.  Functions not specifically declared static
// are emitted as Global Procedures (see below.)
// 
//    2        2        4        4     4     4           4  
//    length   symbol   pParent  pEnd  pNext procLength  debugStart  -->
// 
//       4        4        2        2           1     *
//       debugEnd offset   segment  @proctype   flags name
// 
// 
// (0x0205) Global Procedure Start 16:32
// 
// This symbol is used for procedures that are not specifically declared
// static to a module.
// 
// The format is the same as the Local Procedure Start 16:32 symbol (see
// above.)
// 
// NOTE BOB -- These structures are very similar to SYM_LPROC16, with
// the exception that the dwProcLength, dwDebugStart, and dwDebugEnd
// fields are DWORD instead of WORD.
// 
typedef struct _SYM_LPROC32      // 0x0204   S_LPROC32      Local procedure start 16:32
{
	WORD	wLength;			// Length of record, excluding the length field
	WORD	wType;			// S_LPROC32 or S_GPROC32      
	DWORD	pParent;			// See the section on lexical scope linking
	DWORD	pEnd;				// See the section on lexical scope linking
	DWORD	pNext;			// See the section on lexical scope linking
	DWORD	dwProcLength;	// Length in bytes of this procedure
	DWORD	dwDebugStart;	// Offset in bytes from the start of the procedure to
								// the point where the stack frame has been set up. 
								// Frame and register variables can be viewed at this
								// point.
	DWORD	dwDebugEnd;		// Offset in bytes from the start of the procedure to
								// the point where the procedure is ready to return and
								// has calculated its return value, if any.  Frame and
								// register variables can still be viewed.
	WORD	wOffset;			// Offset portion of the procedure address
	WORD	wSegment;		// Segment portion of the procedure address
	WORD	wProcTypeIndex;	// Type index of the procedure type record
	BYTE	flags;			// Procedure flags:


								// 	 fpo			:1	 true if function has frame pointer omitted
								// 	 interrupt	:1	 true if function is interrupt routine
								// 	 return	 	:1	 true if function performs far return
								// 	 never	 	:1	 true if function never returns
								// 	 unused	 	:4	 
//	WORD	cbName;			// Length-prefixed ...
//	BYTE	name[];			// ... name of procedure
}
	SYM_LPROC32;

typedef SYM_LPROC32 SYM_GPROC32; // 0x0205   S_GPROC32      Global procedure start 16:32

//
// restore default packing...
#if defined(_WINDOWS)
#pragma pack()
#endif

/*===========================================================================
 * FUNCTION PROTOTYPES
 *===========================================================================
 */
				

#endif  /* _codeview_h */


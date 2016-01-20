//              idapi.h - main include file for idapi clients
//
// $Revision: 1 $


#ifndef DBI_H
#define DBI_H
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus


#define DBIINTFVER         119                  // Version of the interface
#define DBIENGVERSION      120                  // Version of the engine (1.20)


#ifndef NODBITYPES

//-----------------------------------------------------------------------
//     DBI types
//-----------------------------------------------------------------------


// sdk debug layer defines
#define         DEBUGON         0x1
#define         OUTPUTTOFILE    0x2
#define         FLUSHEVERYOP    0x8
#define         APPENDTOLOG     0x10




// Constants

#define DBIMAXNAMELEN         31            // Name limit (table, field etc)
#define DBIMAXTBLNAMELEN     127            // Max table name length
#define DBIMAXFLDSINKEY       16            // Max fields in a key
#define DBIMAXKEYEXPLEN      220            // Max Key expression length
#define DBIMAXPATHLEN         81            // Max path+file name len (excluding zero termination)
#define DBIMAXEXTLEN           3            // Max file extension len, not incl. dot (excluding zero termination)

#define DBIMAXDRIVELEN         2            // Max drive length
#define DBIMAXMSGLEN         127            // Max message len (allocate 128)
#define DBIMAXVCHKLEN        255            // Max val check len
#define DBIMAXPICTLEN        175            // Max picture len   //Verify this - VJ
#define DBIMAXFLDSINSEC      256            // Max fields in security spec

#define DBIMAXSCFIELDS        16            // max nbr of fields in a config section
#define DBIMAXSCFLDLEN       128            // max field length
#define DBIMAXSCRECSIZE     2048            // max rec size

#define DBIMAXUSERNAMELEN      14           // Max user name (general)
#define DBIMAXXBUSERNAMELEN    12           // Max user name length (xBASE)
#define DBIMAXBOOKMARKLEN    4104           // Max Bookmark length

#if defined(__FLAT__)
#ifdef far
#undef far
#endif
#define far
#endif // defined __FLAT__

#ifndef NODBIBASETYPES

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif  // NODBIBASETYPES

//For pointers
#ifndef NULL
#  define NULL (0)
#endif

#ifndef VOID
#  define VOID    void
#endif

#ifndef CHAR
#  define CHAR    char
#endif

#ifndef BYTE
#  define BYTE    unsigned char
#endif

#ifndef INT8
#  define INT8    char
#endif

#ifndef UINT8
#  define UINT8   unsigned char
#endif

#ifndef INT16
#if defined(__FLAT__)
#  define INT16   short
#else
#  define INT16   int
#endif
#endif

#ifndef UINT16
#if defined(__FLAT__)
#  define UINT16  unsigned short
#else
#  define UINT16  unsigned int
#endif
#endif

#ifndef INT32
#  define INT32   long
#endif
#ifndef UINT32
#  define UINT32  unsigned long
#endif

#ifndef BOOL
#if defined(__FLAT__)
#  define BOOL    short
#else
#  define BOOL    int
#endif // __FLAT__

#endif

typedef unsigned char BOOL8;
typedef short         BOOL16;

#endif // NODBIBASETYPES

#define FLOAT     double
#define DATE      long
#define TIME      long
#define TIMESTAMP double

typedef UINT32    hDBICfg;

typedef VOID      far *pVOID;
typedef pVOID     far *ppVOID;
typedef CHAR      far *pCHAR;
typedef BYTE      far *pBYTE;
typedef INT8      far *pINT8;
typedef UINT8     far *pUINT8;
typedef INT16     far *pINT16;
typedef UINT16    far *pUINT16;
typedef INT32     far *pINT32;
typedef UINT32    far *pUINT32;
typedef FLOAT     far *pFLOAT;
typedef DATE      far *pDATE;
typedef TIME      far *pTIME;
typedef BOOL      far *pBOOL;
typedef TIMESTAMP far *pTIMESTAMP;


typedef pBYTE     far *ppBYTE;
typedef pCHAR     far *ppCHAR;
typedef pBOOL     far *ppBOOL;

typedef hDBICfg   far *phDBICfg;


// Constants

//===========================================================================
//                            G e n e r a l
//===========================================================================

#ifdef _NEWPORTABLE
#define DBIFN      WINAPI          // All functions are this type
#else
#define DBIFN      pascal far          // All functions are this type
#endif
typedef UINT16          DBIResult;     // Function result
typedef DBIResult  far *pDBIResult;    // Function result pointer

typedef UINT32           hDBIObj;      // Generic object handle

typedef hDBIObj          hDBIDb;       // Database handle
typedef hDBIObj          hDBIQry;      // Query handle
typedef hDBIObj          hDBIStmt;     // Statement handle ("new query")
typedef hDBIObj          hDBICur;      // Cursor handle
typedef hDBIObj          hDBISes;      // Session handle
typedef hDBIObj          hDBIXlt;      // Translation handle

typedef hDBIObj    far *phDBIObj;      // pointer to generic object handle
typedef hDBIDb     far *phDBIDb;       // pointer to Database  handle
typedef hDBIQry    far *phDBIQry;      // pointer to Query     handle
typedef hDBIStmt   far *phDBIStmt;     // pointer to Statement handle
typedef hDBICur    far *phDBICur;      // pointer to Cursor    handle
typedef hDBISes    far *phDBISes;      // pointer to Session   handle
typedef hDBIXlt    far *phDBIXlt;      // pointer to Translation handle

typedef UINT32          hDBIXact;      // Transaction handle
typedef hDBIXact   far *phDBIXact;     // pointer to Transaction handle

// typedefs for buffers of various common sizes:
//
typedef CHAR   DBIPATH    [DBIMAXPATHLEN+1];     // holds a DOS path
typedef CHAR   DBINAME    [DBIMAXNAMELEN+1];     // holds a name
typedef CHAR   DBIEXT     [DBIMAXEXTLEN+1];      // holds an extension EXT
typedef CHAR   DBIDOTEXT  [DBIMAXEXTLEN+2];      // holds an extension EXT including '.'
typedef CHAR   DBIDRIVE   [DBIMAXDRIVELEN+1];    // holds a drive name
typedef CHAR   DBITBLNAME [DBIMAXTBLNAMELEN+1];  // holds a table name
typedef CHAR   DBIUSERNAME[DBIMAXUSERNAMELEN+1]; // holds a user name
typedef UINT16 DBIKEY     [DBIMAXFLDSINKEY];     // holds list of fields in a key
typedef CHAR   DBIKEYEXP  [DBIMAXKEYEXPLEN+1];   // holds a key expression
typedef BYTE   DBIVCHK    [DBIMAXVCHKLEN+1];     // holds a validity check
typedef CHAR   DBIPICT    [DBIMAXPICTLEN+1];     // holds a picture (Pdox)
typedef CHAR   DBIMSG     [DBIMAXMSGLEN+1];      // holds an error message


//===========================================================================
//                            Environmental
//===========================================================================

typedef struct                            // Struct for defining the environ.
   {
      DBIPATH        szWorkDir;                 // Working directory
      DBIPATH        szIniFile;                 // Configuration file
      BOOL           bForceLocalInit;           // Force local initialization
      DBINAME        szLang;                    // System language driver 
      DBINAME        szClientName;              // Client name (documentary)
   } DBIEnv;
typedef DBIEnv far   *pDBIEnv;

//============================================================================
//                   System Info
//============================================================================

typedef struct                         // System Version Info
   {
      UINT16         iVersion;            // Engine version
      UINT16         iIntfLevel;          // Client Interface level
      DATE           dateVer;             // Version date (Compile/Release)
      TIME           timeVer;             // Version time (Compile/Release)
   } SYSVersion;
typedef SYSVersion far *pSYSVersion;

typedef struct                         // System configuration (basic)
   {
      BOOL           bLocalShare;         // If Local files will be shared
      UINT16         iNetProtocol;        // Net Protocol (35, 40 etc.)
      BOOL           bNetShare;           // True if connected to network
      DBINAME        szNetType;           // Network type
      DBIUSERNAME    szUserName;          // Network user name
      DBIPATH        szIniFile;           // Configuration file
      DBINAME        szLangDriver;        // System language driver
   } SYSConfig;
typedef SYSConfig far *pSYSConfig;

typedef struct                         // System Status/Info
   {
      UINT16         iBufferSpace;           // in K
      UINT16         iHeapSpace;             // in K
      UINT16         iDrivers;               // Active/Loaded drivers
      UINT16         iClients;               // Active clients 
      UINT16         iSessions;              // Number of sessions (For all clients)
      UINT16         iDatabases;             // Open databases
      UINT16         iCursors;               // Number of cursors 
   } SYSInfo; 
typedef SYSInfo far * pSYSInfo;

typedef struct
   {
      DBINAME        szName;                 // Documentary name
      UINT16         iSessions;              // Number of sessions
      DBIPATH        szWorkDir;              // Working directory
      DBINAME        szLang;                 // System language driver (Client supplied)
   } CLIENTInfo;
typedef CLIENTInfo far * pCLIENTInfo;

typedef struct
   {
      UINT16         iSession;               // Session id (1..n)
      DBINAME        szName;                 // Documentary name
      UINT16         iDatabases;             // Open databases
      UINT16         iCursors;               // Open cursors
      INT16          iLockWait;              // Lock wait time (in seconds)
      DBIPATH        szNetDir;               // Net directory location
      DBIPATH        szPrivDir;              // Current Private directory
   } SESInfo; 
typedef SESInfo far * pSESInfo;

//============================================================================
//                             Table / Field Types
//============================================================================

// Driver Types

#define szPARADOX       "PARADOX"
#define szDBASE         "DBASE"
#define szASCII         "ASCIIDRV"


// Field Types (Logical)

#define fldUNKNOWN      0
#define fldZSTRING      1                 // Null terminated string
#define fldDATE         2                 // Date     (32 bit)
#define fldBLOB         3                 // Blob
#define fldBOOL         4                 // Boolean  (16 bit)
#define fldINT16        5                 // 16 bit signed number
#define fldINT32        6                 // 32 bit signed number
#define fldFLOAT        7                 // 64 bit floating point
#define fldBCD          8                 // BCD 
#define fldBYTES        9                 // Fixed number of bytes
#define fldTIME         10                // Time        (32 bit)
#define fldTIMESTAMP    11                // Time-stamp  (64 bit)
#define fldUINT16       12                // Unsigned 16 bit integer
#define fldUINT32       13                // Unsigned 32 bit integer
#define fldFLOATIEEE    14                // 80-bit IEEE float
#define fldVARBYTES     15                // Length prefixed var bytes
#define fldLOCKINFO     16                // Look for LOCKINFO typedef

#define MAXLOGFLDTYPES  17                // Number of logical fieldtypes

// Sub Types (Logical)

// fldFLOAT subtype

#define fldstMONEY      21                // Money

// fldBLOB subtypes

#define fldstMEMO          22             // Text Memo
#define fldstBINARY        23             // Binary data
#define fldstFMTMEMO       24             // Formatted Text
#define fldstOLEOBJ        25             // OLE object (Paradox)
#define fldstGRAPHIC       26             // Graphics object
#define fldstDBSOLEOBJ     27             // dBASE OLE object
#define fldstTYPEDBINARY   28             // Typed Binary data

// fldZSTRING subtype

#define fldstPASSWORD      1              // Password

// fldINT32 subtype

#define  fldstAUTOINC      29

// Paradox types (Physical)

#define  fldPDXCHAR        0x101        // Alpha    (string)
#define  fldPDXNUM         0x102        // Numeric     
#define  fldPDXMONEY       0x103        // Money 
#define  fldPDXDATE        0x104        // Date
#define  fldPDXSHORT       0x105        // Short
#define  fldPDXMEMO        0x106        // Text Memo       (blob)
#define  fldPDXBINARYBLOB  0x107        // Binary data     (blob)
#define  fldPDXFMTMEMO     0x108        // Formatted text  (blob)
#define  fldPDXOLEBLOB     0x109        // OLE object      (blob)
#define  fldPDXGRAPHIC     0x10A        // Graphics object (blob)
#define  fldPDXBLOB        fldPDXMEMO    
#define  fldPDXLONG        0x10B        // Long
#define  fldPDXTIME        0x10C        // Time
#define  fldPDXDATETIME    0x10D        // Time Stamp
#define  fldPDXBOOL        0x10E        // Logical
#define  fldPDXAUTOINC     0x10F        // Auto increment (long)
#define  fldPDXBYTES       0x110        // Fixed number of bytes
#define  fldPDXBCD         0x111        // BCD (32 digits)
#define  fldPDXUNICHAR     0x112        // not supported yet

// xBASE types (Physical)

#define  fldDBCHAR         0x201        // Char string
#define  fldDBNUM          0x202        // Number
#define  fldDBMEMO         0x203        // Memo          (blob)
#define  fldDBBOOL         0x204        // Logical
#define  fldDBDATE         0x205        // Date
#define  fldDBFLOAT        0x206        // Float
#define  fldDBLOCK         0x207        // Logical type is LOCKINFO
#define  fldDBOLEBLOB      0x208        // OLE object    (blob)
#define  fldDBBINARY       0x209        // Binary data   (blob)
#define  fldDBBYTES        0x20A        // Only for TEMPORARY tables

// xBASE key types (Cannot be used as field types)

#define  fldDBKEYNUM       0x210
#define  fldDBKEYBCD       0x211


// Ascii types (Physical)

#define  fldASCCHAR         0x301       // Char string
#define  fldASCNUM          0x302       // Number
#define  fldASCBOOL         0x303       // Logical
#define  fldASCDATE         0x304       // Date
#define  fldASCFLOAT        0x305       // Float
#define  fldASCLOCK         0x306       // Not used
#define  fldASCTIMESTAMP    0x307       // TimeStamp
#define  fldASCTIME         0x308       // Time
#define  fldASCLONG         0x309       // Long
#define  fldASCMONEY        0x30A       // Money


//============================================================================
//                    Field descriptor
//============================================================================


typedef enum                              // Field Val Check type
   {
      fldvNOCHECKS     = 0,                  // Does not have explicit val checks
      fldvHASCHECKS    = 1,                  // One or more val checks on the field
      fldvUNKNOWN      = 2                   // Dont know at this time
   } FLDVchk;

typedef enum                              // Field Rights
   {
      fldrREADWRITE     = 0,                 // Field can be Read/Written
      fldrREADONLY      = 1,                 // Field is Read only
      fldrNONE          = 2,                 // No Rights on this field
      fldrUNKNOWN       = 3                  // Dont know at this time
   } FLDRights;

typedef struct {                          // Field Descriptor
      UINT16         iFldNum;                // Field number (1..n)
      DBINAME        szName;                 // Field name
      UINT16         iFldType;               // Field type
      UINT16         iSubType;               // Field subtype (if applicable)
      INT16          iUnits1;                // Number of Chars, digits etc
      INT16          iUnits2;                // Decimal places etc.
      UINT16         iOffset;                // Offset in the record (computed)
      UINT16         iLen;                   // Length in bytes (computed)
      UINT16         iNullOffset;            // For Null bits (computed)
      FLDVchk        efldvVchk;              // Field Has vcheck (computed)
      FLDRights      efldrRights;            // Field Rights (computed)
   } FLDDesc;
typedef FLDDesc far *pFLDDesc;

//============================================================================
//                    Index descriptor
//============================================================================


typedef struct                            // Index description
   {
      DBITBLNAME     szName;                 // Index name
      UINT16         iIndexId;               // Index number
      DBINAME        szTagName;              // Tag name (for dBASE)
      DBINAME        szFormat;               // Optional format (BTREE, HASH etc)
      BOOL           bPrimary;               // True, if primary index
      BOOL           bUnique;                // True, if unique keys
      BOOL           bDescending;            // True, for descending index
      BOOL           bMaintained;            // True, if maintained index
      BOOL           bSubset;                // True, if subset index
      BOOL           bExpIdx;                // True, if expression index
      UINT16         iCost;                  // Not used
      UINT16         iFldsInKey;             // Fields in the key (1 for Exp)
      UINT16         iKeyLen;                // Phy Key length in bytes (Key only)
      BOOL           bOutofDate;             // True, if index out of date 
      UINT16         iKeyExpType;            // Key type of Expression
      DBIKEY         aiKeyFld;               // Array of field numbers in key
      DBIKEYEXP      szKeyExp;               // Key expression
      DBIKEYEXP      szKeyCond;              // Subset condition
      BOOL           bCaseInsensitive;       // True, if case insensitive index
      UINT16         iBlockSize;             // Block size in bytes
      UINT16         iRestrNum;              // Restructure number                  
      UINT16         iUnUsed[16];
   } IDXDesc; 
typedef IDXDesc far *pIDXDesc;

//============================================================================
//                    Validity check, Referential integrity descriptors
//============================================================================



// Subtypes for Lookup

typedef enum                              // Paradox Lookup type
   {
      lkupNONE          = 0,                 // Has no lookup
      lkupPRIVATE       = 1,                 // Just Current Field + Private
      lkupALLCORRESP    = 2,                 // All Corresponding + No Help
      lkupHELP          = 3,                 // Just Current Fld + Help and Fill
      lkupALLCORRESPHELP= 4                  // All Corresponging + Help
   } LKUPType;

#define TODAYVAL  2                       // for Min/Max/Def val flags

   // In VCHKDesc below, if any of bHasMinVal/bHasMaxVal/bHasDefVal
   // = TODAYVAL, then TODAY is assumed.

typedef struct                            // Val Check structure
   {
      UINT16         iFldNum;                // Field number
      BOOL           bRequired;              // If True, value is required 
      BOOL           bHasMinVal;             // If True, has min value (
      BOOL           bHasMaxVal;             // If True, has max value
      BOOL           bHasDefVal;             // If True, has default value
      DBIVCHK        aMinVal;                // Min Value
      DBIVCHK        aMaxVal;                // Max Value
      DBIVCHK        aDefVal;                // Default value 
      DBIPICT        szPict;                 // Picture string
      LKUPType       elkupType;              // Lookup/Fill type
      DBIPATH        szLkupTblName;          // Lookup Table name
   } VCHKDesc;
typedef VCHKDesc far *pVCHKDesc;

typedef enum                              // Ref integrity type
   {
      rintMASTER     = 0,                    // This table is Master
      rintDEPENDENT  = 1                     // This table is Dependent
   } RINTType;

typedef enum                              // Ref integrity action/qualifier
   {
      rintRESTRICT   = 0,                    // Prohibit operation
      rintCASCADE    = 1                     // Cascade operation
   } RINTQual;

typedef struct                            // Ref Integrity Desc
   {  
      UINT16         iRintNum;               // Ref integrity number
      DBINAME        szRintName;             // A name to tag this integegrity constraint
      RINTType       eType;                  // Whether master/dependent
      DBIPATH        szTblName;              // Other table name
      RINTQual       eModOp;                 // Modify qualifier
      RINTQual       eDelOp;                 // Delete qualifier
      UINT16         iFldCount;              // Fields in foreign key 
      DBIKEY         aiThisTabFld;           // Fields in this table
      DBIKEY         aiOthTabFld;            // Fields in other table
   } RINTDesc;
typedef RINTDesc far *pRINTDesc;


//============================================================================
//                    Security descriptor
//============================================================================
// Family rights

#define NOFAMRIGHTS     0x00                 // No Family rights
#define FORMRIGHTS      0x01                 // Can change form
#define RPTRIGHTS       0x02                 // Can change reports
#define VALRIGHTS       0x04                 // Can change valchecks
#define SETRIGHTS       0x08                 // Can change settings
#define ALLFAMRIGHTS    (FORMRIGHTS | RPTRIGHTS | VALRIGHTS | SETRIGHTS)
                                             // All family rights

typedef enum  {                           // Privileges
      prvNONE        = 0,                    // No privilege
      prvREADONLY    = 0x01,                 // Read only Table or Field
      prvMODIFY      = 0x03,                 // Read and Modify fields (non-key)
      prvINSERT      = 0x07,                 // Insert + All of above
      prvINSDEL      = 0x0F,                 // Delete + All of above
      prvFULL        = 0x1F,                 // Full Writes
      prvUNKNOWN     = 0xFF                  // Unknown
   } PRVType;


typedef struct {                             // Security description
      UINT16         iSecNum;                   // Nbr to identify desc
      PRVType        eprvTable;                 // Table privileges
      UINT16         iFamRights;                // Family rights      
      DBINAME        szPassword;                // Null terminated string
      PRVType        aprvFld[DBIMAXFLDSINSEC];  // Field level privileges (prvNONE/prvREADONLY/prvFULL)
   } SECDesc;
typedef SECDesc far *pSECDesc;


//======================================================================
//                         Miscellaneous
//======================================================================

// Index Id used to open table without a default index (i.e. no order)

#define  NODEFAULTINDEX    0xFFFF


//============================================================================
//                    Object types
//============================================================================

typedef enum
   {
       objSYSTEM                 = 1,      // System object
       objSESSION                = 2,      // Session object
       objDRIVER                 = 3,      // Driver object
       objDATABASE               = 4,      // Database object
       objCURSOR                 = 5,      // Cursor object
       objSTATEMENT              = 6,      // Statement object
       objCLIENT                 = 7       // Client object
   }  DBIOBJType;

//============================================================================
//                    Cursor properties 
//============================================================================


typedef enum                              // Database/Table Share type
   {
      dbiOPENSHARED     = 0,                 // Open shared  (Default)
      dbiOPENEXCL       = 1,                 // Open exclusive
   } DBIShareMode;

typedef enum                              // Database/Table Access type
   {
      dbiREADWRITE      = 0,                 // Read + Write   (Default)
      dbiREADONLY       = 1                  // Read only
   } DBIOpenMode;

typedef enum                              // Lock types (Table level)
   {
      dbiNOLOCK         = 0,                 // No lock   (Default)
      dbiWRITELOCK      = 1,                 // Write lock
      dbiREADLOCK       = 2                  // Read lock
   } DBILockType;

typedef enum                              // Field translate mode
   {
      xltNONE        = 0,                    // No translation  (Physical Types)
      xltRECORD      = 1,                    // Record level translation (not supported)
      xltFIELD       = 2,                    // Field level translation (Logical types)
   } XLTMode;


// Table levels 

#define TBLLEVEL3    3              // Paradox level 3 and dBASE level 3+
#define TBLLEVEL4    4              // Paradox level 4 and dBASE level 4
#define TBLLEVEL5    5              // Paradox level 5 and dBASE/Win
#define FOXLEVEL25   25             // Fox Table (Ver 2.5)

typedef struct {                          // Virtual Table properties
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
      BOOL           bBookMarkStable;        // Stable book marks
      DBIOpenMode    eOpenMode;              // ReadOnly / RW
      DBIShareMode   eShareMode;             // Excl / Share
      BOOL           bIndexed;               // Index is in use
      INT16          iSeqNums;               // 1: Has Seqnums; 0: Has Record#
                                             // <0 (-1, -2, ...): None (e.g. SQL)
      BOOL           bSoftDeletes;           // Supports soft deletes
      BOOL           bDeletedOn;             // If above, deleted recs seen
      UINT16         iRefRange;              // Not used
      XLTMode        exltMode;               // Translate Mode
      UINT16         iRestrVersion;          // Restructure version number
      BOOL           bUniDirectional;        // Cursor is uni-directional
      PRVType        eprvRights;             // Table  rights
      UINT16         iFmlRights;             // Family rights      
      UINT16         iPasswords;             // Number of Aux passwords
      UINT16         iCodePage;              // Codepage (0 if unknown)
      BOOL           bProtected;             // Table is protected by password
      UINT16         iTblLevel;              // Driver dependent table level
      DBINAME        szLangDriver;           // Language driver name
      BOOL           bFieldMap;              // Field map active
      UINT16         iBlockSize;             // Physical file blocksize in K 
      BOOL           bStrictRefInt;          // Strict referential integrity
      UINT16         iFilters     ;          // Number of filters
      BOOL           bTempTable   ;          // Table is a temporary table
      UINT16         iUnUsed[16];
     } CURProps;
typedef CURProps far *pCURProps;

//============================================================================
//                   Record Properties
//============================================================================


typedef struct {                          // Record properties
      UINT32         iSeqNum;                // When Seq# supported only
      UINT32         iPhyRecNum;             // When Phy Rec#s supported only
      BOOL           bRecChanged;            // Not used
      BOOL           bSeqNumChanged;         // Not used
      BOOL           bDeleteFlag;            // When soft delete supported only
   } RECProps;
typedef RECProps far *pRECProps;


//============================================================================
//                   General properties  DbiGetProp/DbiSetProp
//============================================================================


// Cursor properties
// General

#define  curMAXPROPS         0x00050000l   //ro UINT16   , Number of defined properties
#define  curTABLENAME        0x00050001l   //ro pTBLNAME , Table name 
#define  curTABLETYPE        0x00050002l   //ro pDBINAME , Table type
#define  curTABLELEVEL       0x00050003l   //ro UINT16   , Table level 1..n
#define  curFILENAME         0x00050004l   //ro pPATH    , Full filename  
#define  curXLTMODE          0x00050005l   //rw XLTMode  , Translate mode
#define  curSEQREADON        0x00050006l   //rw BOOL     , Sequential read mode hint ON
#define  curONEPASSON        0x00050007l   //rw BOOL     , One pass mode hint ON
#define  curUPDATETS         0x00050008l   //ro TIMESTAMP, Update timestamp
#define  curSOFTDELETEON     0x00050009l   //rw BOOL     , Softdelete ON
#define  curLANGDRVNAME      0x0005000Al   //ro pCHAR    , Symbolic language drv. name

// Paradox specific
#define  curPDXMAXPROPS      0x01050000l   //ro UINT16   , Number of defined properties

// DBase specific
#define  curDBMAXPROPS       0x02050000l   //ro UINT16   , Number of defined properties
#define  curINEXACTON        0x02050001l   //rw BOOL     , InExact match ON

// Text Driver specific
#define  curDELIMITALLFLDS   0x03050000l   //rw BOOL   , Delimit all fields.


// Driver properties
// General
#define  drvMAXPROPS         0x00030000l   //ro UINT16   , Number of defined properties
#define  drvDRIVERTYPE       0x00030002l   //ro pDBINAME , Driver type
#define  drvDRIVERVERSION    0x00030003l   //ro UINT16   , Driver version

// Database properties
// General
#define  dbMAXPROPS         0x00040000l   //ro UINT16   , Number of defined properties
#define  dbDATABASENAME     0x00040001l   //ro pDBINAME , Database name/alias
#define  dbDATABASETYPE     0x00040002l   //ro pDBINAME , Database type

// Session properties
// General
#define  sesMAXPROPS         0x00020000l   //ro UINT16   , Number of defined properties
#define  sesSESSIONNAME      0x00020001l   //ro pDBINAME , Name of seesion
#define  sesNETFILE          0x00020002l   //rw pCHAR		, Full filename of netfile (MAXPATHLEN)
#define  sesCFGMODE          0x00020003l   //rw CFGMode, Mode of configuration file.

// System properties
// General
#define   sysMAXPROPS        0x00010000l   // ro UINT16  , Number of defined properties
#define   sysLOWMEMUSAGE     0x00010001l   // ro UINT16  , Low memory usage in (k)

// Statement properties
// General
#define  stmtMAXPROPS        0x00060001l   //ro UINT16      Number of defined properties
#define  stmtPARAMETERCOUNT  0x00060002l   //ro UINT16      Count of parameters
#define  stmtPARAMETERDESCS  0x00060003l   //ro aFLDDesc    Array of parameters
#define  stmtUNIDIRECTIONAL  0x00060010l   //rw BOOL        Cursor Unidirectional

// specific to QBE or local SQL
#define  stmtANSTYPE         0x00060020l   //rw pBYTE       Table Type of Answer set
#define  stmtLIVENESS        0x00060021l   //rw LIVENESS    Preference for canned/live answers
#define  stmtQRYMODE         0x00060022l   //rw QryEvalMode Execute on server or local or either
#define  stmtBLANKS          0x00060023l   //rw BOOL        True if blanks to be regarded as zeros. 
#define  stmtDATEFORMAT      0x00060024l   //rw FMTDate     Date format
#define  stmtNUMBERFORMAT    0x00060025l   //rw FMTNumber   Number format
#define  stmtAUXTBLS         0x00060026l   //rw BOOL        True if QBE to create CHANGED, etc.
#define  stmtTBLVECTOR       0x00060027l   //ro UINT16      Vector of tables generated by query.
#define  stmtALLPROPS        0x00060028l   //rw QueryLowProps
#define  stmtALLPROPSSIZE    0x00060029l   //rw INT16       size of QueryLowProps
#define  stmtANSNAME         0x00060030l   //rw pBYTE       Answer Table Name.


//============================================================================
//                    Transactions
//============================================================================

// Transaction support

typedef enum                           // Transaction isolation levels
   {
      xilDIRTYREAD,                    // Uncommitted changes read
      xilREADCOMMITTED,                // Committed changes, no phantoms
      xilREPEATABLEREAD                // Full read repeatability
   } eXILType;

typedef enum                           // Transaction end control
   {
      xendCOMMIT,       // Commit transaction
      xendCOMMITKEEP,   // Commit transaction, keep cursors
      xendABORT         // Rollback transaction
   } eXEnd;

typedef enum                           // Transaction end control
   {
      xsINACTIVE,       // Transaction inactive
      xsACTIVE          // Transaction active
   } eXState;

typedef struct
   {
      eXState        exState;          // xsActive, xsInactive
      eXILType       eXIL;             // Xact isolation level
      UINT16         uNests;           // Xact children
   } XInfo, *pXInfo;


//============================================================================
//                         BookMark compares
//============================================================================

typedef enum                              // Compare BookMark Results
   {
      CMPLess           = -1,             // Bkm1 < Bkm2
      CMPEql            = 0,              // BookMarks are exactly the same
      CMPGtr            = 1,              // Bkm1 > Bkm2
      CMPKeyEql         = 2               // Only Bkm1.key_val = Bkm2.key_val
   } CMPBkMkRslt;
typedef CMPBkMkRslt far *pCMPBkMkRslt;


//===========================================================================
//                            Key searches
//===========================================================================
               
typedef enum  {                           // Search condition for keys
      keySEARCHEQ       = 0,                 // =
      keySEARCHGT       = 1,                 // >
      keySEARCHGEQ      = 2,                 // >=
    } DBISearchCond;


//============================================================================
//                      Create/Restructure descriptor
//============================================================================

typedef enum                              // Create/Restruct Operation type
   {
      crNOOP         = 0,                 
      crADD          = 1,                    // Add a new element.
      crCOPY         = 2,                    // Copy an existing element.
      crMODIFY       = 3,                    // Modify an element.
      crDROP         = 4,                    // Removes an element.
      crREDO         = 5,                    // Reconstruct an element.
      crTABLE        = 6,                    // Not used
      crGROUP        = 7,                    // Not used
      crFAMILY       = 8,                    // Not used
      crDONE         = 9,                    // Used internally
      crDROPADD      = 10                    // Used internally
   } CROpType;

typedef CROpType far *pCROpType;

typedef struct                            // Create/Restruct Table descr
   {
      DBITBLNAME     szTblName;                 // TableName incl. optional path & ext
      DBINAME        szTblType;                 // Driver type (optional)
      DBIPATH        szErrTblName;              // Error Table name (optional)
      DBINAME        szUserName;                // User name (if applicable)
      DBINAME        szPassword;                // Password (optional) 
      BOOL           bProtected;                // Master password supplied in szPassword
      BOOL           bPack;                     // Pack table (restructure only) 

      UINT16         iFldCount;                 // Number of field defs supplied
      pCROpType      pecrFldOp;                 // Array of field ops
      pFLDDesc       pfldDesc;                  // Array of field descriptors

      UINT16         iIdxCount;                 // Number of index defs supplied
      pCROpType      pecrIdxOp;                 // Array of index ops
      pIDXDesc       pidxDesc;                  // Array of index descriptors

      UINT16         iSecRecCount;              // Number of security defs supplied
      pCROpType      pecrSecOp;                 // Array of security ops
      pSECDesc       psecDesc;                  // Array of security descriptors

      UINT16         iValChkCount;              // Number of val checks
      pCROpType      pecrValChkOp;              // Array of val check ops
      pVCHKDesc      pvchkDesc;                 // Array of val check descs

      UINT16         iRintCount;                // Number of ref int specs
      pCROpType      pecrRintOp;                // Array of ref int ops
      pRINTDesc      printDesc;                 // Array of ref int specs

      UINT16         iOptParams;                // Number of optional parameters
      pFLDDesc       pfldOptParams;             // Array of field descriptors
      pBYTE          pOptData;                  // Optional parameters

   } CRTblDesc;

typedef CRTblDesc far *pCRTblDesc;


//============================================================================
//                      Batch 
//============================================================================


typedef struct                            // Batch Table definition struct
   {
      hDBIDb         hDb;                       // Database
      DBIPATH        szTblName;                 // Table name
      DBINAME        szTblType;                 // Optional Driver type
      DBINAME        szUserName;                // Optional User name
      DBINAME        szPassword;                // Optional Pass word
   } BATTblDesc;

typedef BATTblDesc far *pBATTblDesc;

typedef enum                   // Batch mode for DBIAppendTable
   {
      batAPPEND =       0,
      batUPDATE =       1,
      batAPPENDUPDATE = 2,
      batSUBTRACT =     3,
      batCOPY     =     4
   } eBATMode;
 
typedef enum                              // Sort Order
   {
      sortASCEND     = 0,                    // ascending (default)
      sortDESCEND    = 1,                    // descending
   } SORTOrder;

typedef SORTOrder far *pSORTOrder;

typedef INT16 (DBIFN  * pfSORTCompFn)     // pntr to client compare fn
   (
      pVOID          pLdObj,                 // Language driver, if needed
      pVOID          pValue1,                // first value
      pVOID          pValue2,                // second value
      INT16          iLen                    // Length, if needed
   );
   // Returns: -1 if (Value 1 < Value 2), 
   //           0 if (Value 1 == Value 2),
   //          +1 if (Value 1 > Value 2)


//===========================================================================
//                      Types/Structs for Capabilities 
//===========================================================================

typedef enum                           // Driver Category
   {
      drvFILE           = 1,              // File based (Paradox, xBASE)
      drvOTHERSERVER    = 2,              // Other kind of server (IMS ?)
      drvSQLBASEDSERVER = 3               // SQL Based Server
   } DRVCat; 

typedef enum                           // Config info & Optional Parameters
   {
      opDBCREATE        = 3,
      opDBOPEN          = 4,
      opTBLCREATE       = 5,
      opTBLOPEN         = 6
   } OPType;

typedef struct                         // Driver Description/Capabilities
   {
      DBINAME        szType;                 // Symbolic name to identify
      DBINAME        szText;                 // Descriptive text
      DRVCat         edrvCat;                // Driver category
      BOOL           bTrueDb;                // Supports True Db concept
      DBINAME        szDbType;               // Db Type to be used
      BOOL           bMultiUser;             // Supports Multi-user access
      BOOL           bReadWrite;             // Read Write or Read-only
      BOOL           bTrans;                 // Supports Transactions 
      BOOL           bPassThruSQL;           // Supports Pass-thru SQL
      BOOL           bLogIn;                 // Requires explicit login
      BOOL           bCreateDb;              // Can reate a Database
      BOOL           bDeleteDb;              // Can drop database
      BOOL           bCreateTable;           // Can create a Table
      BOOL           bDeleteTable;           // Can delete a Table 
      BOOL           bMultiplePWs;           // Multiple passwords
      UINT16         iDriverVersion;         // Driver version 1..n
      BOOL           bSQLRowid;              // Supports SQL rowid
      UINT16         iUnUsed[15];
   } DRVType;
typedef DRVType far *pDRVType;


typedef struct                            // Table Capabilities
   {
      UINT16         iId;                    // Id of Table Type
      DBINAME        szName;                 // Symbolic name; eg "dBASE"
      DBINAME        szText;                 // Descriptive text
      DBINAME        szFormat;               // Format; eg "HEAP"
      BOOL           bReadWrite;             // User can Read/Write
      BOOL           bCreate;                // Can create new tables
      BOOL           bRestructure;           // Can restructure this table
      BOOL           bValChecks;             // Val Checks can be specified
      BOOL           bSecurity;              // Can be protected
      BOOL           bRefIntegrity;          // Can participate in ref integrity
      BOOL           bPrimaryKey;            // Supports primary key concept
      BOOL           bIndexing;              // Can have other indexes
      UINT16         iFldTypes;              // Number of Phy Field types supported
      UINT16         iMaxRecSize;            // Max record size
      UINT16         iMaxFldsInTable;        // Max fields in a table
      UINT16         iMaxFldNameLen;         // Maximum field name length
      UINT16         iTblLevel;              // Driver dependent table level (version)
      UINT16         iUnUsed[16];
    } TBLType;
typedef TBLType far *pTBLType;


typedef struct 
   {
      UINT16         iId;                    // Id of Index Type
      DBINAME        szName;                 // Symbolic name
      DBINAME        szText;                 // Descriptive text
      DBINAME        szFormat;               // Optional format(BTREE, HASH etc)
      BOOL           bComposite;             // Supports composite keys 
      BOOL           bPrimary;               // True, if primary index
      BOOL           bUnique;                // True, No duplicates supported
      BOOL           bKeyDescending;         // If whole key can be descending
      BOOL           bFldDescending;         // Field level descending
      BOOL           bMaintained;            // Supports maintained option
      BOOL           bSubset;                // Supports Subset expression
      BOOL           bKeyExpr;               // If Key can be expres
      BOOL           bCaseInsensitive;       // Supports Caseinsensitive indexes 
      UINT16         iUnUsed[16];
   } IDXType;
typedef IDXType far *pIDXType;

typedef struct
   {
      UINT16         iId;                    // Id of Field Type
      DBINAME        szName;                 // Symbolic name; eg "ALPHA"
      DBINAME        szText;                 // Descriptive text
      UINT16         iPhyType;               // Physical/Native type
      UINT16         iXltType;               // Default xlated type
      UINT16         iXltSubType;            // Default xlated sub type
      UINT16         iMaxUnits1;             // Max units allowed (1)
      UINT16         iMaxUnits2;             // Max units allowed (2)
      UINT16         iPhySize;               // Physical size in bytes (per unit)
      BOOL           bRequired;              // Supports 'required' option
      BOOL           bDefaultVal;            // Supports user specified 'default'
      BOOL           bMinVal;                // Supports MIN Val constraint
      BOOL           bMaxVal;                // Supports MAX Val constraint
      BOOL           bRefIntegrity;          // Can participate in ref integrity
      BOOL           bOtherChecks;           // Supports other kinds of checks
      BOOL           bKeyed;                 // The field type can be keyed
      BOOL           bMultiplePerTable;      // Table can have more than 1 of this type
      UINT16         iMinUnits1;             // Min units required (1)
      UINT16         iMinUnits2;             // Min units required (2)
      BOOL           bCreateable;            // Type can be created
      UINT16         iUnUsed[16];
    } FLDType;
typedef FLDType far *pFLDType;


typedef enum                           // PROP Validity
   {
      epropINVALID     = 0,            // Property is INVALID (not supported)
      epropCANGET      = 1,            // You can GET value of the property
      epropCANSET      = 2             // You can GET and SET value of the property
   } PROPValid;


//===========================================================================
//                               Call Backs
//===========================================================================


typedef enum                              // Call back type
   {
       cbGENERAL      = 0,                    // General purpose
       cbRESERVED1    = 1,                    // 
       cbRESERVED2    = 2,                    // 
       cbRESERVED3    = 3,                    // 
       cbRESERVED4    = 4,                    // 
       cbRESERVED5    = 5,                    // 
       cbBATCHRESULT  = 6,                    // Batch processing rslts
       cbRESERVED7    = 7,                    // 
       cbRESTRUCTURE  = 8,                    // Restructure
       cbRESERVED9    = 9,                    // 
       cbRESERVED10   = 10,                   // 
       cbRESERVED11   = 11,                   // 
       cbRESERVED12   = 12,                   // 
       cbRESERVED13   = 13,                   // 
       cbRESERVED14   = 14,                   // 
       cbRESERVED15   = 15,                   // 
       cbRESERVED16   = 16,                   // 
       cbRESERVED17   = 17,                   // 
       cbTABLECHANGED = 18,                   // Table changed notification
       cbRESERVED19   = 19,                   // 
       cbRESERVED20   = 20,                   // 
       cbRESERVED21   = 21,                   // 
       cbRESERVED22   = 22,                   // 
       cbGENPROGRESS  = 23,                   // Generic Progress report.
       cbNBROFCBS     = 24                    // Number of cbs
   } CBType;
typedef CBType far *pCBType;


typedef enum                              // Call-back return type
   {
      cbrUSEDEF        = 0,                  // Take default action
      cbrCONTINUE      = 1,                  // Continue
      cbrABORT         = 2,                  // Abort the operation
      cbrCHKINPUT      = 3,                  // Input given
      cbrYES           = 4,                  // Take requested action
      cbrNO            = 5,                  // Do not take requested action
      cbrPARTIALASSIST = 6                   // Assist in completing the job
   } CBRType;
typedef CBRType far *pCBRType;

typedef CBRType (DBIFN  * pfDBICallBack)  // Call-back funtion pntr type
   (
      CBType         ecbType,                // Callback type
      UINT32         iClientData,            // Client callback data
      pVOID          pCbInfo                 // Call back info/Client Input
   );

typedef pfDBICallBack far *ppfDBICallBack;

// Progress callback structure
typedef struct
   {
      INT16         iPercentDone;          // Percentage done 
      DBIMSG        szMsg;                 // Message to display
   } CBPROGRESSDesc;

typedef  CBPROGRESSDesc far * pCBPROGRESSDesc;


typedef enum        // type of restructure object (restructure callback)
   {
      restrNONE        = 0,
      restrNEWFLD      = 1,
      restrOLDFLD      = 2,
      restrNEWINDEX    = 3,
      restrOLDINDEX    = 4,
      restrNEWVCHK     = 5,
      restrOLDVCHK     = 6,   
      restrNEWRINT     = 7,
      restrOLDRINT     = 8,
      restrNEWSEC      = 9,
      restrOLDSEC      = 10,
      restrNEWTABLE    = 11
   } RESTErrObjType;

typedef struct       // restructure callback info
   {
      DBIResult      iErrCode;
      UINT16         iTblNum;
      UINT16         iObjNum;       
      // for old objects iObjNum is the sequence or field number,
      // for new objects it is the order in CRTblDesc
      RESTErrObjType   eRestrObjType;
      union 
  	      {
            FLDDesc     fldDesc;
            IDXDesc     idxDesc;
            VCHKDesc    vchkDesc;
            RINTDesc    rintDesc;
            SECDesc     secDesc;
         } uObjDesc;
   } RESTCbDesc;


//============================================================================
//                         Basic Query Types
//============================================================================

typedef enum DBIQryLang
{
   qrylangUNKNOWN = 0,  // UNKNOWN (Error)
   qrylangQBE = 1,      // QBE
   qrylangSQL = 2       // SQL
} DBIQryLang;

//============================================================================
//                      Date, Time, Number Formats
//============================================================================


typedef struct                         // Date Format
   { 
      CHAR        cDecimalSeparator;         // Default "."
      CHAR        cThousandSeparator;        // Default ","
      INT8        iDecimalDigits;            // Default 2
      INT8        bLeadingZero;              // Default TRUE.
   } FMTNumber;
typedef FMTNumber far *pFMTNumber;

typedef struct
   {
      CHAR        szDateSeparator[4];        // Default "/"
      INT8        iDateMode;                 // 0 = MDY (Def), 1 = DMY, 2 = YMD
      INT8        bFourDigitYear;            // Write Year as 4 digits (FALSE)
      INT8        bYearBiased;               // On input add 1900 to year (TRUE)
      INT8        bMonthLeadingZero;         // Month with Leading Zero (TRUE)
      INT8        bDayLeadingZero;           // Day with Leading Zero (TRUE)
   } FMTDate;
typedef FMTDate far *pFMTDate;

typedef struct
   {
     CHAR         cTimeSeparator;            // Default ":" 
     INT8         bTwelveHour;               // Represent as 12 Hour time (FALSE)
     CHAR         szAmString[6];             // Default Null (Only for 12 Hr)
     CHAR         szPmString[6];             // Default Null (Only for 12 Hr)
     INT8         bSeconds;                  // Show Seconds (TRUE)
     INT8         bMilSeconds;               // Show Milli Seconds (FALSE)
   } FMTTime;
typedef FMTTime far *pFMTTime;


typedef struct
{
    BYTE    iPrecision;          // 1..64 considered valid
    BYTE    iSignSpecialPlaces;  // sign:1, special:1, places:6
    BYTE    iFraction [32];      // bcd nibbles, 00..99 per byte, high nibble 1st
} FMTBcd;

typedef FMTBcd far * pFMTBcd;


//============================================================================
//                              Error Info
//============================================================================

typedef struct {                       // Error info   
      DBIResult      iError;                 // Last error code returned
      DBIMSG         szErrCode;              // Error Code
      DBIMSG         szContext1;             // Context dependent info
      DBIMSG         szContext2;             // More context
      DBIMSG         szContext3;             // Even more context
      DBIMSG         szContext4;             // Yet some more context
    } DBIErrInfo;

typedef DBIErrInfo far *pDBIErrInfo;

// Error contexts (To be used with DbiGetErrorContext)

#define    ecTOKEN         1              // Token (For QBE)
#define    ecTABLENAME     3              // Table name
#define    ecFIELDNAME     4              // Field Name 
#define    ecIMAGEROW      5              // Image Row (For QBE)
#define    ecUSERNAME      6              // eg, In lock conflicts, user involved
#define    ecFILENAME      7              // File Name
#define    ecINDEXNAME     8              // Index Name
#define    ecDIRNAME       9              // Directory Name
#define    ecKEYNAME       10             // Key Name 
#define    ecALIAS         11             // Alias
#define    ecDRIVENAME     12             // Drive name ('c:')
#define    ecNATIVECODE    13             // Native error code
#define    ecNATIVEMSG     14             // Native error message
#define    ecLINENUMBER    15             // Line Number
#define    ecCAPABILITY    16             // Capability

//============================================================================
//                  Schema info structures       
//============================================================================
//                    Database descriptor
//============================================================================


// DbiOpenDbList 
typedef struct                               // A given Database Description
   {
      DBINAME        szName;                 // Logical name (Or alias)
      DBINAME        szText;                 // Descriptive text
      DBIPATH        szPhyName;              // Physical name/path
      DBINAME        szDbType;               // Database type
   } DBDesc;
typedef DBDesc far *pDBDesc;


//============================================================================
//                    User info descriptor
//============================================================================


//DbiOpenUserList()
typedef struct                               // User description 
   {
      DBIUSERNAME    szUserName; 
      UINT16         iNetSession;            // Net level session number
      UINT16         iProductClass;          // Product class of user
      CHAR           szSerialNum[22];        // Serial number
   } USERDesc;
typedef USERDesc far *pUSERDesc;

//============================================================================
//                    Table descriptor
//============================================================================


// DbiOpenTableList()
typedef struct                            // Table description (Base)
   {
      DBITBLNAME     szName;                 // Table name(No extension or Dir)
      DBITBLNAME     szFileName;             // File name 
      DBIEXT         szExt;                  // File extension
      DBINAME        szType;                 // Driver type
      DATE           dtDate;                 // Date on the table
      TIME           tmTime;                 // Time on the table
      UINT32         iSize;                  // Size in bytes
      BOOL           bView;                  // If this a view
   } TBLBaseDesc;
typedef TBLBaseDesc far *pTBLBaseDesc;

typedef struct                            // Table description (Extended part)
   {
      DBINAME        szStruct;               // Physical structure
      UINT16         iRestrVersion;          // Version #
      UINT16         iRecSize;               // Physical record size
      UINT16         iFields;                // Number of fields
      UINT16         iIndexes;               // Number Indexes   
      UINT16         iValChecks;             // Number of field validity checks
      UINT16         iRintChecks;            // Number of ref. integrity checks
      UINT32         iRecords;               // Number of records in table
      BOOL           bProtected;             // If the table is prot
      BOOL           bValidInfo;             // Info available for this table
                                             // FALSE, if all or part of the
                                             // extended data not available.
   } TBLExtDesc;
typedef TBLExtDesc far *pTBLExtDesc;

typedef struct                            // Table description (Base + Ext)
   {
      TBLBaseDesc    tblBase;                // Base      description
      TBLExtDesc     tblExt;                 // Extended  description
   } TBLFullDesc;
typedef TBLFullDesc far *pTBLFullDesc;

//============================================================================
//                    File descriptor
//============================================================================


// DbiOpenFileList()
typedef struct                            // File description
   {
      DBIPATH        szFileName;             // File name (No Dir or ext)
      DBIEXT         szExt;                  // Extension
      BOOL           bDir;                   // True, if directory
      UINT32         iSize;                  // File size in bytes
      DATE           dtDate;                 // Date on the file
      TIME           tmTime;                 // Time on the file
   } FILEDesc;
typedef FILEDesc far *pFILEDesc;



//============================================================================
//                   Configuration Info Descriptor
//============================================================================

typedef enum
   {
     cfgPersistent  = 0,   // Persistent only
     cfgSession     = 1,   // Session relative only
     cfgAll         = 2    // All (union)
   } CFGMode;


// DbiOpenCfgInfoList()
typedef struct                            // Config description
   {
      DBINAME        szNodeName;                // Node name 
      CHAR           szDescription[DBIMAXSCFLDLEN]; // Node description
      UINT16         iDataType;                 // Value type
      CHAR           szValue[DBIMAXSCFLDLEN];   // Value
      BOOL           bHasSubnodes;              // True, if not leaf node
   } CFGDesc;                                     
typedef CFGDesc far *pCFGDesc;


//============================================================================
//                    Family descriptor
//============================================================================


typedef enum                              // Family member types
   {
      fmlUNKNOWN     = 0,
      fmlTABLE       = 1,
      fmlINDEX       = 2,
      fmlFORM        = 3,
      fmlREPORT      = 4,
      fmlVALCHECK    = 5,
      fmlSECINDEX    = 6,
      fmlSECINDEX2   = 7,
      fmlBLOBFILE    = 8
   } FMLType;
typedef FMLType far *pFMLType;

// DbiOpenFamilyList()
typedef struct {                          // Family record structure
      DBINAME        szName;                 // Member name (documentary)
      UINT16         iId;                    // Id (if applicable)
      FMLType        eType;                  // Member type
      DBIPATH        szFileName;             // File name of member
   } FMLDesc;
typedef FMLDesc far *pFMLDesc;


//============================================================================
//                    Language driver descriptor
//============================================================================


#define DBIOEM_CP                    1      // (dos)
#define DBIANSI_CP                   2      // (win)
#define DBIOS2_CP                    3
/* UNIX etc. */
#define DBISUNOS_CP                  4
#define DBIVMS_CP                    5
#define DBIHPUX_CP                   6
#define DBIULTRIX_CP                 7
#define DBIAIX_CP                    8
#define DBIAUX_CP                    9
#define DBIXENIX_CP                 10
#define DBIMAC_CP                   11
#define DBINEXT_CP                  12

// DbiOpenLdList()
typedef struct                         // Lang Driver description
   {
      DBINAME        szName;               // Driver symbolic name
      DBINAME        szDesc;               // Description
      UINT16         iCodePage;
      UINT16         PrimaryCpPlatform;
      UINT16         AlternateCpPlatform;
   } LDDesc;
typedef LDDesc far *pLDDesc;

//============================================================================
//                    Lock descriptor
//============================================================================

// Lock types in LOCKDesc:

#define lckRECLOCK         0                    // Normal Record lock (Write)
#define lckRRECLOCK        1                    // Special Pdox Record lock (Read)
#define lckGROUPLOCK       2                    // Pdox Group lock
#define lckIMGAREA         3                    // Pdox Image area
#define lckTABLEREG        4                    // Table registration/Open (No lock)
#define lckTABLEREAD       5                    // Table Read lock
#define lckTABLEWRITE      6                    // Table Write lock
#define lckTABLEEXCL       7                    // Table Exclusive lock
#define lckUNKNOWN         9                    // Unknown lock


// DbiOpenLockList()
typedef struct                               // Lock Description 
   {
      UINT16         iType;                     // Lock type (0 for rec lock)            
      DBIUSERNAME    szUserName;                // Lock owner
      UINT16         iNetSession;               // Net level Session number
      UINT16         iSession;                  // Idapi session#, if our lock
      UINT32         iRecNum;                   // If a record lock
      UINT16         iInfo;                     // Info for table locks
   } LOCKDesc;
typedef LOCKDesc far *pLOCKDesc;

//============================================================================
//                    Filter description
//============================================================================

typedef enum {

      // Relational operators.

      canNOTDEFINED        = 0,        //                                  (*)
      canISBLANK,                      // CANUnary;  is operand blank.     (*)
      canNOTBLANK,                     // CANUnary;  is operand not blank. (*)

      canEQ,                           // CANBinary, CANCompare; equal.                (*)
      canNE,                           // CANBinary; NOT equal.            (*)
      canGT,                           // CANBinary; greater than.         (*)
      canLT,                           // CANBinary; less than.            (*)
      canGE,                           // CANBinary; greater or equal.     (*)
      canLE,                           // CANBinary; less or equal.        (*)

      // Logical operators.

      canNOT,                          // CANUnary; NOT                    (*)
      canAND,                          // CANBinary; AND                   (*)
      canOR,                           // CANBinary; OR                    (*)

      // Operators identifing leaf operands.

      canTUPLE,                        // CANUnary; Entire record is operand.
      canFIELD,                        // CANUnary; operand is field       (*)
      canCONST,                        // CANUnary; operand is constant    (*)

      // Arithmetic operators.

      canMINUS,                        // CANUnary;  minus.
      canADD,                          // CANBinary; addition.
      canSUB,                          // CANBinary; subtraction.
      canMUL,                          // CANBinary; multiplication.
      canDIV,                          // CANBinary; division.
      canMOD,                          // CANBinary; modulo division.
      canREM,                          // CANBinary; remainder of division.

      // Aggregate type operators.

      canSUM,                          // CANBinary, accumulate sum of.
      canCOUNT,                        // CANBinary, accumulate count of.
      canMIN,                          // CANBinary, find minimum of.
      canMAX,                          // CANBinary, find maximum of.
      canAVG,                          // CANBinary, find average of.

      // Miscellaneous operators.

      canCONT,                         // CANBinary; provides a link between two
                                       // expression subtrees of a tree.
      canUDF,                          // CANBinary; invokes a User defined fn

      canCONTINUE,                     // CANUnary; Stops evaluating records
                                       // when operand evaluates to false (forward sequential access only)

      canLIKE                          // CANCompare, extended binary compare       (*)

   } CANOp;

typedef CANOp far *pCANOp;

typedef enum {                      // Node Class

      nodeNULL          = 0,           // Null node                  (*)
      nodeUNARY,                       // Node is a unary            (*)
      nodeBINARY,                      // Node is a binary           (*)
      nodeCOMPARE,                     // Node is a compare          (*)
      nodeFIELD,                       // Node is a field            (*)
      nodeCONST,                       // Node is a constant         (*)
      nodeTUPLE,                       // Node is a record
      nodeCONTINUE,                    // Node is a continue node    (*)
      nodeUDF                          // Node is a UDF node
   } NODEClass;

// NODE definitions including misc data structures
//-------------------------------------------------

typedef struct {                    // Header part common to all     (*)
      NODEClass      nodeClass;
      CANOp          canOp;
   } CANHdr;
typedef CANHdr far *pCANHdr;

typedef struct {                    // Unary Node                    (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iOperand1;        // Byte offset of Operand node
   } CANUnary;
typedef CANUnary far * pCANUnary;

typedef struct {                    // Binary Node                   (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iOperand1;        // Byte offset of Op1
      UINT16         iOperand2;        // Byte offset of Op2
   } CANBinary;
typedef CANBinary far * pCANBinary;

typedef struct {                    // Field
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iFieldNum;
      UINT16         iNameOffset;      // Name offset in Literal pool
   } CANField;
typedef CANField far * pCANField;

typedef struct {                    // Constant
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iType;            // Constant type.
      UINT16         iSize;            // Constant size. (in bytes)
      UINT16         iOffset;          // Offset in the literal pool.
   } CANConst;
typedef CANConst far * pCANConst;

typedef struct {                    // Tuple (record)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iSize;            // Record size. (in bytes)
   } CANTuple;
typedef CANTuple far * pCANTuple;

typedef struct {                    // Break Node                    (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iContOperand;     // Continue if operand is true.
                                       // else stop evaluating records.
   } CANContinue;
typedef CANContinue far * pCANContinue;

typedef struct {                    // Extended compare Node (text fields) (*)
      NODEClass      nodeClass;
      CANOp          canOp;               // canLIKE, canEQ 
      BOOL           bCaseInsensitive;    // 3 val: UNKNOWN = "fastest", "native"
      UINT16         iPartialLen;         // Partial fieldlength (0 is full length)
      UINT16         iOperand1;           // Byte offset of Op1
      UINT16         iOperand2;           // Byte offset of Op2
   } CANCompare;
typedef CANCompare far * pCANCompare;


//This is the node to be used to pass User defined functions
#define iLangSQL     0              // Common SQL dialect
#define iDbaseExpr   2              // This is also the driver ID for dBASE

typedef struct {                    // A user defined function
      NODEClass      nodeClass;
      CANOp          canOp;
      
      UINT16         iOffSzFuncName;   // Offset in literal pool to Function Name string(0 terminated)

      UINT16         iOperands;        // Byte offset of Operands (concatenated using canCONT)

      //The fields below are hints that specific drivers can make use of
      UINT16         iDrvDialect;      // Driver Dialect ID for UDF string supplied
      UINT16         iOffSzUDF;        // Offset in literal pool to UDF string (0 terminated)
   } CANUdf;
typedef CANUdf far * pCANUdf;


typedef union {
      CANHdr         canHdr;
      CANUnary       canUnary;
      CANBinary      canBinary;
      CANField       canField;
      CANConst       canConst;
      CANTuple       canTuple;
      CANContinue    canContinue;
      CANCompare     canCompare;
   } CANNode;
typedef CANNode far *pCANNode;

// Linear exression tree
//----------------------

# define CANEXPRVERSION   2

typedef struct {                    // Expression Tree
      UINT16         iVer;             // Version tag of expression.
      UINT16         iTotalSize;       // Size of this structure
      UINT16         iNodes;           // Number of nodes
      UINT16         iNodeStart;       // Starting offet of Nodes in this
      UINT16         iLiteralStart;    // Starting offset of Literals in this
      // Nodes and Literals are here
   } CANExpr;
typedef CANExpr far * pCANExpr;
typedef pCANExpr far *ppCANExpr;


//pfGENFilter returns TRUE, FALSE or ABORT
#define ABORT     -2
typedef INT16 (DBIFN * pfGENFilter)
   (
      UINT32            ulClientData,
      pBYTE             pRecBuf,
      UINT32            iPhyRecNum
   ) ;

typedef UINT32 hDBIFilter ;
typedef hDBIFilter far *phDBIFilter ;



typedef struct
   {
      UINT16          iFilterId;    // Id for filter 
      hDBIFilter      hFilter;      // Filter handle
      UINT32          iClientData;  // Client supplied data
      UINT16          iPriority;    // 1..N with 1 being highest
      BOOL            bCanAbort;    // TRUE : pfFilter can return ABORT
      pfGENFilter     pfFilter;     // Client filter function
      pVOID           pCanExpr;     // Supplied expression
      BOOL            bActive ;     // TRUE : filter is active
   } FILTERInfo;
typedef FILTERInfo far *pFILTERInfo;

#ifndef NODBIQBE

//---------------------------------------------------------------------------
//   DBI Query related types
//---------------------------------------------------------------------------

#define MAXQBEEXPRSIZE  300             // size of one QBE expr


typedef struct  _DBIQryProp
{
   DBINAME    szQryName;       // Name of query
   DBIQryLang eLang;           // Language
   INT16      iQryPrice;       // Query price 1..100 (1 = cheap, 100 = expensive)
   INT16      iNumTables;      // Number of tables in join.  0 = unknown.
   BOOL       bHasAnswer;
   BOOL       bInsert;
   BOOL       bDelete;
   BOOL       bChange;
}  DBIQryProp, far *pDBIQryProp;


#define DBIQBE_ANSWERBIT    (0x1)    // Answer table bit flag
#define DBIQBE_INSERTEDBIT  (0x2)    // Inserted table bit flag
#define DBIQBE_DELETEDBIT   (0x4)    // Deleted table bit flag
#define DBIQBE_CHANGEDBIT   (0x8)    // Changed table bit flag
#define DBIQBE_ERRORINSBIT  (0x10)   // Error inserted table bit flag
#define DBIQBE_ERRORDELBIT  (0x20)   // Error deleted table bit flag
#define DBIQBE_ERRORCHGBIT  (0x40)   // Error changed table bit flag


// answer cursor properties:

#define bAnsHasLiveFields  0x1
#define bAnsHasFilter      0x2
#define bAnsHasFieldMap    0x4
#define bAnsHasCalcField   0x8
#define bAnsHasLiveBlob    0x10

// answer field properties:

#define bIsAnsFieldLive    0x1



typedef enum 
{
   dbiqryDEFAULT = 0,
   dbiqryDIRTY   = 1,
   dbiqryCLEAN   = 2,
   dbiqryRESTART = 3
} DBIQryType, far *pDBIQryType;


//---------------------------------------------------------------------------
typedef struct
{
   UINT16    stepsInQry;      // Total steps in query.
   UINT16    stepsCompleted;  // Steps completed out of total (steps may be skipped).
   UINT32    totElemInStep;   // Total elements in current step.
   UINT32    elemCompleted;   // Elements completed in current step.
} DBIQryProgress, far *pDBIQryProgress;


typedef enum
{
   qryModeNONE     = 0,       // Reserved
   qryModeLOCAL    = 1,
   qryModeSERVER   = 2,
   qryModeEITHER   = 3,
   qryModeNOWLOCAL = 4        // used only in call back, when failed on server
} QryEvalMode;


// values for client indicating live/canned preference about query execution
typedef enum
{
   wantDEFAULT     = 0,        // Default , same as wantCANNED
   wantLIVE        = 1,        // Want live data even if extra effort (no guarantee)
   wantCANNED      = 2,        // Want canned data even if extra effort (guaranteed)
   wantSPEED       = 3         // Let query manager decide, find out afterwards
} LIVENESS;


typedef struct
{
   INT16       length;        // Length in bytes of this structure
   BOOL        blankzeroes;   // TRUE if blanks to be regarded as zeros
   FMTDate     dateFormat;    // Date format
   FMTNumber   numberFormat;  // Number format
   BOOL        bNeedAuxTbls;  // If FALSE, don't bother with DELETED/ERRDEL, etc.
   QryEvalMode qryMode;       // qryModeSERVER, qryModeLOCAL or qryModeEITHER.
   BOOL        perQrySqlMode;
   LIVENESS    livenessDesired;
} QueryLowProps, far *pQueryLowProps;

#endif  // NODBIQBE

#endif  // NODBITYPES

#ifndef NODBISYMBOLS
//=======================================================================================
//                      DBI symbols
//=======================================================================================

# define DBIMOD_BEGIN         (0x3F00U)

# define DBIMOD_QBE           (DBIMOD_BEGIN + 1)
# define DBIMOD_SQLG          (DBIMOD_BEGIN + 2)
# define DBIMOD_LEGO          (DBIMOD_BEGIN + 3)
# define DBIMOD_LOCKMNGR      (DBIMOD_BEGIN + 4)
# define DBIMOD_SQLDRIVER     (DBIMOD_BEGIN + 5)
# define DBIMOD_OS            (DBIMOD_BEGIN + 6)
# define DBIMOD_DBASEDRV      (DBIMOD_BEGIN + 7)

# define DBIMOD_END           (DBIMOD_BEGIN + 8)

//---------------------------------------------------------------------------

# define DBISYM_BEGIN         (DBIMOD_END + 1)

# define DBISYM_TOKEN         (DBISYM_BEGIN + ecTOKEN)
# define DBISYM_TABLENAME     (DBISYM_BEGIN + ecTABLENAME)
# define DBISYM_FIELDNAME     (DBISYM_BEGIN + ecFIELDNAME)
# define DBISYM_IMAGEROW      (DBISYM_BEGIN + ecIMAGEROW)
# define DBISYM_USERNAME      (DBISYM_BEGIN + ecUSERNAME)
# define DBISYM_FILENAME      (DBISYM_BEGIN + ecFILENAME)
# define DBISYM_INDEXNAME     (DBISYM_BEGIN + ecINDEXNAME)
# define DBISYM_DIRNAME       (DBISYM_BEGIN + ecDIRNAME)
# define DBISYM_KEYNAME       (DBISYM_BEGIN + ecKEYNAME)
# define DBISYM_ALIAS         (DBISYM_BEGIN + ecALIAS)
# define DBISYM_DRIVENAME     (DBISYM_BEGIN + ecDRIVENAME)
# define DBISYM_NATIVECODE    (DBISYM_BEGIN + ecNATIVECODE)
# define DBISYM_NATIVEMSG     (DBISYM_BEGIN + ecNATIVEMSG)
# define DBISYM_LINENUMBER    (DBISYM_BEGIN + ecLINENUMBER)
# define DBISYM_CAPABILITY    (DBISYM_BEGIN + ecCAPABILITY)

# define DBISYM_BASEEND       (DBISYM_BEGIN + 100)

//-----------------------------------------------------------------------------

# define DBISYM_MISC          (DBISYM_BASEEND + 1)

# define DBISYM_WORK          (DBISYM_MISC + 1)
# define DBISYM_PRIV          (DBISYM_MISC + 2)
# define DBISYM_COPY          (DBISYM_MISC + 3)
# define DBISYM_APPEND        (DBISYM_MISC + 4)

# define DBISYM_END           (DBIMOD_BEGIN + 0x3FFF)



//=======================================================================================
//                      DBI Config symbols
//=======================================================================================



// Categories

#define szCFGSYSTEM        "SYSTEM"
#define szCFGDRIVER        "DRIVERS"
#define szCFGDATABASE      "DATABASES"

//--------------------------------------------------------------------------
// System Fields
//--------------------------------------------------------------------------

#define szCFGSYSVERSION       "VERSION"
#define szCFGSYSNETTYPE       "NET TYPE"
#define szCFGSYSNETDIR        "NET DIR"
#define szCFGSYSLOCALSHARE    "LOCAL SHARE"
#define szCFGSYSLANGDRV       "LANGDRIVER"
#define szCFGSYSLANGDRVDIR    "LANGDRVDIR"
#define szCFGSYSMINBUF        "MINBUFSIZE"
#define szCFGSYSMAXBUF        "MAXBUFSIZE"
#define szCFGSYSLOCKRETRY     "LOCKRETRY"
#define szCFGSYSFLAGS         "SYSFLAGS"
#define szCFGMAXFILEHANDLES   "MAXFILEHANDLES"
#define szCFGSQLQRYMODE       "SQLQRYMODE"
#define szCFGLOWMEMLIMIT      "LOW MEMORY USAGE LIMIT"  // Use this instead of NOLOWMEMBUF


//--------------------------------------------------------------------------
// Driver Fields
//--------------------------------------------------------------------------

#define szCFGDRVVERSION       "VERSION"
#define szCFGDRVTYPE          "TYPE"
#define szCFGDRVDLL           "DLL"
#define szCFGDRVFLAGS         "DRIVER FLAGS"
#define szCFGDRVLANGDRIVER    "LANGDRIVER"
#define szCFGDRVFILLFACTOR    "FILL FACTOR"
#define szCFGDRVBLOCKSIZE     "BLOCK SIZE"
#define szCFGDRVLOCKPROTOCOL  "LOCKPROTOCOL"
#define szCFGDRVLEVEL         "LEVEL"
#define szCFGDRVVENDINIT      "VENDOR INIT"

//--------------------------------------------------------------------------
// Dbase Driver fields
//--------------------------------------------------------------------------

#define szCFGDRVMEMOBLOCKSIZE "MEMO FILE BLOCK SIZE"
#define szCFGDRVMDXBLOCKSIZE  "MDX BLOCK SIZE"


//--------------------------------------------------------------------------
// Driver Nodes
//--------------------------------------------------------------------------

#define szCFGDRVINIT         "INIT"
#define szCFGDBCREATE        "DB CREATE"
#define szCFGDBOPEN          "DB OPEN"
#define szCFGTBLCREATE       "TABLE CREATE"
#define szCFGTBLOPEN         "TABLE OPEN"

//--------------------------------------------------------------------------
// Database Nodes
//--------------------------------------------------------------------------

#define szCFGDBINFO          "DB INFO"

//--------------------------------------------------------------------------
// Database fields
//--------------------------------------------------------------------------

#define szCFGDBTYPE          "TYPE"
#define szCFGDBPATH          "PATH"

//--------------------------------------------------------------------------
// Others
//--------------------------------------------------------------------------

#define szCFGINIT            "INIT"
#define szTYPE               "TYPE"
#define szCFGDBSTANDARD      "STANDARD"
#define szCFGTRUE            "TRUE"
#define szCFGFALSE           "FALSE"
#define szOPENMODE           "OPEN MODE"
#define szREADWRITE          "READ/WRITE"
#define szREADONLY           "READ ONLY"
#define szSHAREMODE          "SHARE MODE"
#define szEXCLUSIVE          "EXCLUSIVE"
#define szSHARED             "SHARED"
#define szUSERNAME           "USER NAME"
#define szSERVERNAME         "SERVER NAME"
#define szDATABASENAME       "DATABASE NAME"
#define szSCHEMASIZE         "SCHEMA CACHE SIZE"
#define szCFGSTRICTINTEGRITY "STRICTINTEGRTY"
#define szSQLPASSMODE        "SQLPASSTHRU MODE"
#define szNOTSHARED          "NOT SHARED"
#define szSHAREDAUTOCOMMIT   "SHARED AUTOCOMMIT"
#define szSHAREDNOAUTOCOMMIT "SHARED NOAUTOCOMMIT"
#define szSCHEMATIME         "SCHEMA CACHE TIME"



//--------------------------------------------------------------------------
// SYSTEM DATE/TIME/NUMBER FORMATS
// SYSTEM nodes:
//--------------------------------------------------------------------------
#define  szCFGFORMAT          "FORMATS"

//--------------------------------------------------------------------------
// Format nodes:
//--------------------------------------------------------------------------
#define  szCFGDATE           "DATE"
#define  szCFGTIME           "TIME"
#define  szCFGNUMBER         "NUMBER"

//--------------------------------------------------------------------------
// DATE and/or TIME fields:
//--------------------------------------------------------------------------
#define szCFGSEPARATOR        "SEPARATOR"
#define szCFGMODE             "MODE"
#define szCFGFOURDIGITYEAR    "FOURDIGITYEAR"
#define szCFGYEARBIASED       "YEARBIASED"
#define szCFGLEADINGZEROM     "LEADINGZEROM"
#define szCFGLEADINGZEROD     "LEADINGZEROD"
#define szCFGTWELVEHOUR       "TWELVEHOUR"
#define szCFGAMSTRING         "AMSTRING"
#define szCFGPMSTRING         "PMSTRING"
#define szCFGSECONDS          "SECONDS"
#define szCFGMILSECONDS       "MILSECONDS"

//--------------------------------------------------------------------------
// Number fields:
//--------------------------------------------------------------------------
#define szCFGDECIMALSEPARATOR    "DECIMALSEPARATOR"
#define szCFGTHOUSANDSEPARATOR   "THOUSANDSEPARATOR"
#define szCFGDECIMALDIGITS       "DECIMALDIGITS"
#define szCFGLEADINGZERON        "LEADINGZERON"



// String resoure id's for each string listed above

#define   DBICFG_BASE              0x3A00

//--------------------------------------------------------------------------
// Categories
//--------------------------------------------------------------------------
#define iCFGSYSTEM               (DBICFG_BASE + 1)
#define iCFGDRIVER               (DBICFG_BASE + 2)
#define iCFGDATABASE             (DBICFG_BASE + 3)

//--------------------------------------------------------------------------
// System Fields
//--------------------------------------------------------------------------
#define iCFGSYSVERSION           (DBICFG_BASE + 5)
#define iCFGSYSNETTYPE           (DBICFG_BASE + 6)
#define iCFGSYSNETDIR            (DBICFG_BASE + 7)
#define iCFGSYSLOCALSHARE        (DBICFG_BASE + 8)
#define iCFGSYSLANGDRV           (DBICFG_BASE + 9)
#define iCFGSYSLANGDRVDIR        (DBICFG_BASE + 10)
#define iCFGSYSMINBUF            (DBICFG_BASE + 11)
#define iCFGSYSMAXBUF            (DBICFG_BASE + 12)
#define iCFGSYSLOCKRETRY         (DBICFG_BASE + 13)
#define iCFGSYSFLAGS             (DBICFG_BASE + 14)
#define iCFGMAXFILEHANDLES       (DBICFG_BASE + 15)
#define iCFGSQLQRYMODE           (DBICFG_BASE + 16)
#define iCFGLOWMEMLIMIT          (DBICFG_BASE + 17)


//--------------------------------------------------------------------------
// Driver Fields
//--------------------------------------------------------------------------
#define iCFGDRVVERSION           (DBICFG_BASE + 20)
#define iCFGDRVTYPE              (DBICFG_BASE + 21)
#define iCFGDRVLANGDRIVER        (DBICFG_BASE + 22)
#define iCFGDRVFILLFACTOR        (DBICFG_BASE + 23)
#define iCFGDRVBLOCKSIZE         (DBICFG_BASE + 24)
#define iCFGDRVLOCKPROTOCOL      (DBICFG_BASE + 25)
#define iCFGDRVLEVEL             (DBICFG_BASE + 26)
#define iCFGDRVFLAGS             (DBICFG_BASE + 27)

//--------------------------------------------------------------------------
// Dbase Driver fields      
//--------------------------------------------------------------------------
#define iCFGDRVMEMOBLOCKSIZE     (DBICFG_BASE + 30 )
#define iCFGDRVMDXBLOCKSIZE      (DBICFG_BASE + 31 )


//--------------------------------------------------------------------------
// Driver Nodes
//--------------------------------------------------------------------------
#define iCFGDRVINIT              (DBICFG_BASE + 40 )
#define iCFGDBCREATE             (DBICFG_BASE + 41 )
#define iCFGDBOPEN               (DBICFG_BASE + 42 )
#define iCFGTBLCREATE            (DBICFG_BASE + 43 )
#define iCFGTBLOPEN              (DBICFG_BASE + 44 )

//--------------------------------------------------------------------------
// Database Nodes
//--------------------------------------------------------------------------
#define iCFGDBINFO               (DBICFG_BASE + 50 )

//--------------------------------------------------------------------------
// Database fields
//--------------------------------------------------------------------------
#define iCFGDBTYPE               (DBICFG_BASE + 60)
#define iCFGDBPATH               (DBICFG_BASE + 61)

//--------------------------------------------------------------------------
// Others
//--------------------------------------------------------------------------
#define iCFGINIT                 (DBICFG_BASE + 70)
#define iTYPE                    (DBICFG_BASE + 71)
#define iCFGDBSTANDARD           (DBICFG_BASE + 72)
#define iCFGTRUE                 (DBICFG_BASE + 73)
#define iCFGFALSE                (DBICFG_BASE + 74)
#define iOPENMODE                (DBICFG_BASE + 75)
#define iREADWRITE               (DBICFG_BASE + 76)
#define iREADONLY                (DBICFG_BASE + 77)
#define iSHAREMODE               (DBICFG_BASE + 78)
#define iEXCLUSIVE               (DBICFG_BASE + 79)
#define iSHARED                  (DBICFG_BASE + 80)
#define iUSERNAME                (DBICFG_BASE + 81)
#define iSERVERNAME              (DBICFG_BASE + 82)
#define iDATABASENAME            (DBICFG_BASE + 83)
#define iSCHEMASIZE              (DBICFG_BASE + 84)
#define iCFGSTRICTINTEGRITY      (DBICFG_BASE + 85)

//--------------------------------------------------------------------------
// System node:
//--------------------------------------------------------------------------
#define  iCFGFORMAT              (DBICFG_BASE + 130)

//--------------------------------------------------------------------------
// Format nodes:
//--------------------------------------------------------------------------
#define  iCFGDATE                (DBICFG_BASE + 131)
#define  iCFGTIME                (DBICFG_BASE + 132)
#define  iCFGNUMBER              (DBICFG_BASE + 133)

//--------------------------------------------------------------------------
// DATE and/or TIME fields:
//--------------------------------------------------------------------------
#define iCFGSEPARATOR            (DBICFG_BASE + 140)
#define iCFGMODE                 (DBICFG_BASE + 141)
#define iCFGFOURDIGITYEAR        (DBICFG_BASE + 142)
#define iCFGYEARBIASED           (DBICFG_BASE + 143)
#define iCFGLEADINGZEROM         (DBICFG_BASE + 144)
#define iCFGLEADINGZEROD         (DBICFG_BASE + 145)
#define iCFGTWELVEHOUR           (DBICFG_BASE + 146)
#define iCFGAMSTRING             (DBICFG_BASE + 147)
#define iCFGPMSTRING             (DBICFG_BASE + 148)
#define iCFGSECONDS              (DBICFG_BASE + 149)
#define iCFGMILSECONDS           (DBICFG_BASE + 150)

//--------------------------------------------------------------------------
// Number fields:
//--------------------------------------------------------------------------
#define iCFGDECIMALSEPARATOR     (DBICFG_BASE + 160)
#define iCFGTHOUSANDSEPARATOR    (DBICFG_BASE + 161)
#define iCFGDECIMALDIGITS        (DBICFG_BASE + 162)
#define iCFGLEADINGZERON         (DBICFG_BASE + 163)

#define iCFGDEFLANGDRV           (DBICFG_BASE + 165)
#define iCFGDBASEDEFLANGDRV      (DBICFG_BASE + 166)

//--------------------------------------------------------------------------
//Formats
//--------------------------------------------------------------------------
#define iCFGDEFSEPARATOR         (DBICFG_BASE + 170)
#define iCFGDEFMODE              (DBICFG_BASE + 171)
#define iCFGDEFFOURDIGITYEAR     (DBICFG_BASE + 172)
#define iCFGDEFYEARBIASED        (DBICFG_BASE + 173)
#define iCFGDEFLEADINGZEROM      (DBICFG_BASE + 174)
#define iCFGDEFLEADINGZEROD      (DBICFG_BASE + 175)
#define iCFGDEFTWELVEHOUR        (DBICFG_BASE + 176)
#define iCFGDEFAMSTRING          (DBICFG_BASE + 177)
#define iCFGDEFPMSTRING          (DBICFG_BASE + 178)
#define iCFGDEFSECONDS           (DBICFG_BASE + 179)
#define iCFGDEFMILSECONDS        (DBICFG_BASE + 180)
#define iCFGDEFDECIMALSEPARATOR  (DBICFG_BASE + 181)
#define iCFGDEFTHOUSANDSEPARATOR (DBICFG_BASE + 182)
#define iCFGDEFLEADINGZERO       (DBICFG_BASE + 183)


// Categories
extern const char far SZCFGSYSTEM[];
extern const char far SZCFGDRIVER[];
extern const char far SZCFGDATABASE[];
				      		       
// System Fields
extern const char far SZCFGSYSVERSION[];
extern const char far SZCFGSYSNETTYPE[];
extern const char far SZCFGSYSNETDIR[];
extern const char far SZCFGSYSLOCALSHARE[];
extern const char far SZCFGSYSLANGDRV[];
extern const char far SZCFGSYSLANGDRVDIR[];
extern const char far SZCFGSYSMINBUF[]; 
extern const char far SZCFGSYSMAXBUF[];
extern const char far SZCFGSYSLOCKRETRY[];
extern const char far SZCFGSYSFLAGS[]; 	 
extern const char far SZCFGMAXFILEHANDLES[];
extern const char far SZCFGSQLQRYMODE[];
extern const char far SZCFGLOWMEMLIMIT[];
								 
// Driver Fields
extern const char far SZCFGDRVVERSION[];
extern const char far SZCFGDRVTYPE[]; 	
extern const char far SZCFGDRVDLL[]; 	
extern const char far SZCFGDRVFLAGS[];
extern const char far SZCFGDRVLANGDRIVER[];
extern const char far SZCFGDRVFILLFACTOR[];
extern const char far SZCFGDRVBLOCKSIZE[]; 
extern const char far SZCFGDRVLOCKPROTOCOL[];
extern const char far SZCFGDRVLEVEL[];	     
					     
// Dbase Driver fields
extern const char far SZCFGDRVMEMOBLOCKSIZE[];
extern const char far SZCFGDRVMDXBLOCKSIZE[]; 
					      


// Driver Nodes
extern const char far SZCFGDRVINIT[];
extern const char far SZCFGDBCREATE[];
extern const char far SZCFGDBOPEN[]; 
extern const char far SZCFGTBLCREATE[];
extern const char far SZCFGTBLOPEN[]; 
				       
// Database Nodes
extern const char far SZCFGDBINFO[]; 
				     
// Database fields
extern const char far SZCFGDBTYPE[]; 
extern const char far SZCFGDBPATH[]; 
				     
// Others
extern const char far SZCFGINIT[]; 
extern const char far SZTYPE[];    
extern const char far SZCFGDBSTANDARD[];
extern const char far SZCFGTRUE[]; 	
extern const char far SZCFGFALSE[]; 	
extern const char far SZOPENMODE[]; 
extern const char far SZREADWRITE[];
extern const char far SZREADONLY[]; 
extern const char far SZSHAREMODE[];
extern const char far SZEXCLUSIVE[];
extern const char far SZSHARED[];   
extern const char far SZUSERNAME[]; 
extern const char far SZSERVERNAME[]; 
extern const char far SZDATABASENAME[];
extern const char far SZSCHEMASIZE[];  
extern const char far SZCFGSTRICTINTEGRITY[];
					     
			 

// SYSTEM DATE/TIME/NUMBER FORMATS
// SYSTEM nodes:
extern const char far  SZCFGFORMAT[];
				     
// Format nodes:
extern const char far  SZCFGDATE[];
extern const char far  SZCFGTIME[];
extern const char far  SZCFGNUMBER[];
				     
// DATE and/or TIME fields:
extern const char far SZCFGSEPARATOR[];
extern const char far SZCFGMODE[];
extern const char far SZCFGFOURDIGITYEAR[];
extern const char far SZCFGYEARBIASED[];
extern const char far SZCFGLEADINGZEROM[]; 
extern const char far SZCFGLEADINGZEROD[]; 
extern const char far SZCFGTWELVEHOUR[]; 
extern const char far SZCFGAMSTRING[]; 
extern const char far SZCFGPMSTRING[]; 
extern const char far SZCFGSECONDS[]; 
extern const char far SZCFGMILSECONDS[]; 
					 		
// Number fields:
extern const char far SZCFGDECIMALSEPARATOR[];
extern const char far SZCFGTHOUSANDSEPARATOR[];
extern const char far SZCFGDECIMALDIGITS[];  
extern const char far SZCFGLEADINGZERON[];   


extern const char far SZCFGDEFLANGDRV[]; 
extern const char far SZCFGDBASEDEFLANGDRV[];
extern const char far SZCFGDEFSEPARATOR[];   
extern const char far SZCFGDEFMODE[]; 	     
extern const char far SZCFGDEFFOURDIGITYEAR[];
extern const char far SZCFGDEFYEARBIASED[];   
extern const char far SZCFGDEFLEADINGZEROM[]; 
extern const char far SZCFGDEFLEADINGZEROD[]; 
extern const char far SZCFGDEFTWELVEHOUR[];   
extern const char far SZCFGDEFAMSTRING[];     
extern const char far SZCFGDEFPMSTRING[];     
extern const char far SZCFGDEFSECONDS[];      
extern const char far SZCFGDEFMILSECONDS[];   
extern const char far SZCFGDEFDECIMALSEPARATOR[];
extern const char far SZCFGDEFTHOUSANDSEPARATOR[];
extern const char far SZCFGDEFLEADINGZERO[];
					    

#endif  // NODBISYMBOLS


#ifndef NODBIERRS
#define ErrCat(rslt)  ((UINT16)(rslt) >> 8)
#define ErrCode(rslt) ((UINT16)(rslt) & 0x00FF)


//============================================================================
//                            Error Categories
//============================================================================

#define ERRCAT_NONE                0   //  0   No error
#define ERRCAT_SYSTEM            0x21  //  33  System related (Fatal Error)
#define ERRCAT_NOTFOUND          0x22  //  34  Object of interest Not Found
#define ERRCAT_DATACORRUPT       0x23  //  35  Physical Data Corruption
#define ERRCAT_IO                0x24  //  36  I/O related error
#define ERRCAT_LIMIT             0x25  //  37  Resource or Limit error
#define ERRCAT_INTEGRITY         0x26  //  38  Integrity Violation
#define ERRCAT_INVALIDREQ        0x27  //  39  Invalid Request
#define ERRCAT_LOCKCONFLICT      0x28  //  40  Locking/Contention related
#define ERRCAT_SECURITY          0x29  //  41  Access Violation - Security related
#define ERRCAT_INVALIDCONTEXT    0x2A  //  42  Invalid context
#define ERRCAT_OS                0x2B  //  43  Os Error not handled by Idapi
#define ERRCAT_NETWORK           0x2C  //  44  Network related
#define ERRCAT_OPTPARAM          0x2D  //  45  Optional parameter related
#define ERRCAT_QUERY             0x2E  //  46  Query related 
#define ERRCAT_VERSION           0x2F  //  47  Version Mismatch Category
#define ERRCAT_CAPABILITY        0x30  //  48  Capability not supported
#define ERRCAT_CONFIG            0x31  //  49  System configuration error
#define ERRCAT_WARNING           0x32  //  50
#define ERRCAT_OTHER             0x33  //  51  Miscellaneous
#define ERRCAT_COMPATIBILITY     0x34  //  52  Compatibility related

#define ERRCAT_DRIVER            0x3E  //  62  Driver related
#define ERRCAT_RC                0x3F  //  63  Internal


#define ERRBASE_NONE              0     // No error
#define ERRBASE_SYSTEM          0x2100  // System related (Fatal Error)
#define ERRBASE_NOTFOUND        0x2200  // Object of interest Not Found
#define ERRBASE_DATACORRUPT     0x2300  // Physical Data Corruption
#define ERRBASE_IO              0x2400  // I/O related error
#define ERRBASE_LIMIT           0x2500  // Resource or Limit error
#define ERRBASE_INTEGRITY       0x2600  // Integrity Violation
#define ERRBASE_INVALIDREQ      0x2700  // Invalid Request
#define ERRBASE_LOCKCONFLICT    0x2800  // Locking/Contention related
#define ERRBASE_SEC             0x2900  // Access Violation - Security related
#define ERRBASE_IC              0x2A00  // Invalid context
#define ERRBASE_OS              0x2B00  // Os Error not handled by Idapi
#define ERRBASE_NETWORK         0x2C00  // Network related
#define ERRBASE_OPTPARAM        0x2D00  // Optional Parameter related
#define ERRBASE_QUERY           0x2E00  // Query related 
#define ERRBASE_VERSION         0x2F00  // Version Mismatch Category
#define ERRBASE_CAPABILITY      0x3000  // Capability not supported
#define ERRBASE_CONFIG          0x3100  // System configuration error
#define ERRBASE_WARNING         0x3200  //
#define ERRBASE_OTHER           0x3300  // Miscellaneous
#define ERRBASE_COMPATIBILITY   0x3400  // Compatibility related

#define ERRBASE_DRIVER          0x3E00  // Driver related
#define ERRBASE_RC              0x3F00  // Internal 


//=============================================================================
//                           Error Codes By Category
//=============================================================================

// ERRCAT_NONE                  (0)
// ===========

#define ERRCODE_NONE             0

#define DBIERR_NONE                (ERRBASE_NONE + ERRCODE_NONE)

//  ERRCAT_SYSTEM               
//  =============

#define  ERRCODE_SYSFILEOPEN     1   // Cannot open a system file
#define  ERRCODE_SYSFILEIO       2   // I/O error on a system file
#define  ERRCODE_SYSCORRUPT      3   // Data structure corruption
#define  ERRCODE_NOCONFIGFILE    4   // Cannot find config file
#define  ERRCODE_CFGCANNOTWRITE  5   // Cannot write config file (READONLY)
#define  ERRCODE_CFGMULTIFILE    6   // Initializing with different ini file 
#define  ERRCODE_REENTERED       7   // System has been illegally re-entered
#define  ERRCODE_CANTFINDODAPI   8   // Cannot locate ODAPIxx.DLL
#define  ERRCODE_CANTLOADODAPI   9   // Cannot load ODAPIxx.DLL
#define  ERRCODE_CANTLOADLIBRARY 10  // Cannot load a service DLL

#define  DBIERR_SYSFILEOPEN      (ERRBASE_SYSTEM + ERRCODE_SYSFILEOPEN)
#define  DBIERR_SYSFILEIO        (ERRBASE_SYSTEM + ERRCODE_SYSFILEIO)
#define  DBIERR_SYSCORRUPT       (ERRBASE_SYSTEM + ERRCODE_SYSCORRUPT)
#define  DBIERR_NOCONFIGFILE     (ERRBASE_SYSTEM + ERRCODE_NOCONFIGFILE)
#define  DBIERR_CFGCANNOTWRITE   (ERRBASE_SYSTEM + ERRCODE_CFGCANNOTWRITE)
#define  DBIERR_CFGMULTIFILE     (ERRBASE_SYSTEM + ERRCODE_CFGMULTIFILE)
#define  DBIERR_REENTERED        (ERRBASE_SYSTEM + ERRCODE_REENTERED)
#define  DBIERR_CANTFINDODAPI    (ERRBASE_SYSTEM + ERRCODE_CANTFINDODAPI)
#define  DBIERR_CANTLOADODAPI    (ERRBASE_SYSTEM + ERRCODE_CANTLOADODAPI)
#define  DBIERR_CANTLOADLIBRARY  (ERRBASE_SYSTEM + ERRCODE_CANTLOADLIBRARY)

//  ERRCAT_NOTFOUND
//  ===============

#define  ERRCODE_BOF             1        // Beginning of Virtual table
#define  ERRCODE_EOF             2        // End of Virtual table
#define  ERRCODE_RECMOVED        3        // Fly-away
#define  ERRCODE_KEYORRECDELETED 4        // Record Deleted/Key Modified
#define  ERRCODE_NOCURRREC       5        // No current record
#define  ERRCODE_RECNOTFOUND     6        // Record was not found
#define  ERRCODE_ENDOFBLOB       7        // End of Blob reached
#define  ERRCODE_OBJNOTFOUND     8        // Generic Not found
#define  ERRCODE_FMLMEMBERNOTFOUND 9      // Family member not found
#define  ERRCODE_BLOBFILEMISSING 10       // 0x0a Blob file for table is missing
#define  ERRCODE_LDNOTFOUND      11       // 0x0b Language driver not found

#define  DBIERR_BOF              (ERRBASE_NOTFOUND + ERRCODE_BOF)
#define  DBIERR_EOF              (ERRBASE_NOTFOUND + ERRCODE_EOF)
#define  DBIERR_RECMOVED         (ERRBASE_NOTFOUND + ERRCODE_RECMOVED)
#define  DBIERR_RECDELETED       (ERRBASE_NOTFOUND + ERRCODE_KEYORRECDELETED)
#define  DBIERR_KEYORRECDELETED  (ERRBASE_NOTFOUND + ERRCODE_KEYORRECDELETED)
#define  DBIERR_NOCURRREC        (ERRBASE_NOTFOUND + ERRCODE_NOCURRREC)
#define  DBIERR_RECNOTFOUND      (ERRBASE_NOTFOUND + ERRCODE_RECNOTFOUND)
#define  DBIERR_ENDOFBLOB        (ERRBASE_NOTFOUND + ERRCODE_ENDOFBLOB)
#define  DBIERR_OBJNOTFOUND      (ERRBASE_NOTFOUND + ERRCODE_OBJNOTFOUND)
#define  DBIERR_FMLMEMBERNOTFOUND (ERRBASE_NOTFOUND + ERRCODE_FMLMEMBERNOTFOUND)
#define  DBIERR_BLOBFILEMISSING (ERRBASE_NOTFOUND + ERRCODE_BLOBFILEMISSING)
#define  DBIERR_LDNOTFOUND (ERRBASE_NOTFOUND + ERRCODE_LDNOTFOUND)

// ERRCAT_DATACORRUPT
// ==================

#define  ERRCODE_HEADERCORRUPT   1        // Corrupt Header
#define  ERRCODE_FILECORRUPT     2        // File corrupt - other than header
#define  ERRCODE_MEMOCORRUPT     3        // Memo file corrupted
#define  ERRCODE_BMPCORRUPT      4        // BitMap is corrupt (Internal error)
#define  ERRCODE_INDEXCORRUPT    5        // Index is corrupt
#define  ERRCODE_CORRUPTLOCKFILE 6        // Corrupt lock file
#define  ERRCODE_FAMFILEINVALID  7        // Corrupt family file
#define  ERRCODE_VALFILECORRUPT  8        // Val file is missing or corrupt


#define  DBIERR_HEADERCORRUPT    (ERRBASE_DATACORRUPT + ERRCODE_HEADERCORRUPT)
#define  DBIERR_FILECORRUPT      (ERRBASE_DATACORRUPT + ERRCODE_FILECORRUPT)
#define  DBIERR_MEMOCORRUPT      (ERRBASE_DATACORRUPT + ERRCODE_MEMOCORRUPT)
#define  DBIERR_BMPCORRUPT       (ERRBASE_DATACORRUPT + ERRCODE_BMPCORRUPT)
#define  DBIERR_INDEXCORRUPT     (ERRBASE_DATACORRUPT + ERRCODE_INDEXCORRUPT)
#define  DBIERR_CORRUPTLOCKFILE  (ERRBASE_DATACORRUPT + ERRCODE_CORRUPTLOCKFILE)
#define  DBIERR_FAMFILEINVALID   (ERRBASE_DATACORRUPT + ERRCODE_FAMFILEINVALID)
#define  DBIERR_VALFILECORRUPT   (ERRBASE_DATACORRUPT + ERRCODE_VALFILECORRUPT)


// ERRCAT_IO
// =========

#define  ERRCODE_READERR         1        // Read failure (not expected)
#define  ERRCODE_WRITEERR        2        // Write failure (not expected)
#define  ERRCODE_DIRNOACCESS     3        // No access to dir
#define  ERRCODE_FILEDELETEFAIL  4        // File delete failed
#define  ERRCODE_FILENOACCESS    5        // No access to file
#define  ERRCODE_ACCESSDISABLED  6        // Access to table disabled (previous error)

#define  DBIERR_READERR          (ERRBASE_IO + ERRCODE_READERR)
#define  DBIERR_WRITEERR         (ERRBASE_IO + ERRCODE_WRITEERR)
#define  DBIERR_DIRNOACCESS      (ERRBASE_IO + ERRCODE_DIRNOACCESS)
#define  DBIERR_FILEDELETEFAIL   (ERRBASE_IO + ERRCODE_FILEDELETEFAIL)
#define  DBIERR_FILENOACCESS     (ERRBASE_IO + ERRCODE_FILENOACCESS)
#define  DBIERR_ACCESSDISABLED   (ERRBASE_IO + ERRCODE_ACCESSDISABLED)

// ERRCAT_LIMIT
// ============

#define  ERRCODE_NOMEMORY        1        // Not enough Memory for this op
#define  ERRCODE_NOFILEHANDLES   2        // Not enough File handles
#define  ERRCODE_NODISKSPACE     3        // Not enough Disk space
#define  ERRCODE_NOTEMPTBLSPACE  4        // Temporary Table resource limit
#define  ERRCODE_RECTOOBIG       5        // Too big a record size for table
#define  ERRCODE_CURSORLIMIT     6        // Too many open cursors
#define  ERRCODE_TABLEFULL       7        // Table is full
#define  ERRCODE_WSSESLIMIT      8        // Too many sessions from this WS
#define  ERRCODE_SERNUMLIMIT     9        // Serial number limit (paradox)
#define  ERRCODE_INTERNALLIMIT   10       // 0x0a Some internal limit (see context)
#define  ERRCODE_OPENTBLLIMIT    11       // 0x0b Too many open tables
#define  ERRCODE_TBLCURSORLIMIT  12       // 0x0c Too many cursors per table
#define  ERRCODE_RECLOCKLIMIT    13       // 0x0d Too many record locks on table
#define  ERRCODE_CLIENTSLIMIT    14       // 0x0e Too many clients
#define  ERRCODE_INDEXLIMIT      15       // 0x0f Too many indexes (also in Table Create)
#define  ERRCODE_SESSIONSLIMIT   16       // 0x10 Too many sessions
#define  ERRCODE_DBLIMIT         17       // 0x11 Too many databases
#define  ERRCODE_PASSWORDLIMIT   18       // 0x12 Too many passwords
#define  ERRCODE_DRIVERLIMIT     19       // 0x13 Too many active drivers
#define  ERRCODE_FLDLIMIT        20       // 0x14 Too many Fields in Table Create
#define  ERRCODE_TBLLOCKLIMIT    21       // 0x15 Too many table locks
#define  ERRCODE_OPENBLOBLIMIT   22       // 0x16 Too many open blobs 
#define  ERRCODE_LOCKFILELIMIT   23       // 0x17 Lock file has grown too big

#define  DBIERR_NOMEMORY         (ERRBASE_LIMIT + ERRCODE_NOMEMORY)
#define  DBIERR_NOFILEHANDLES    (ERRBASE_LIMIT + ERRCODE_NOFILEHANDLES)
#define  DBIERR_NODISKSPACE      (ERRBASE_LIMIT + ERRCODE_NODISKSPACE)
#define  DBIERR_NOTEMPTBLSPACE   (ERRBASE_LIMIT + ERRCODE_NOTEMPTBLSPACE)
#define  DBIERR_RECTOOBIG        (ERRBASE_LIMIT + ERRCODE_RECTOOBIG)
#define  DBIERR_CURSORLIMIT      (ERRBASE_LIMIT + ERRCODE_CURSORLIMIT)
#define  DBIERR_TABLEFULL        (ERRBASE_LIMIT + ERRCODE_TABLEFULL)
#define  DBIERR_WSSESLIMIT       (ERRBASE_LIMIT + ERRCODE_WSSESLIMIT)
#define  DBIERR_SERNUMLIMIT      (ERRBASE_LIMIT + ERRCODE_SERNUMLIMIT)
#define  DBIERR_INTERNALLIMIT    (ERRBASE_LIMIT + ERRCODE_INTERNALLIMIT)
#define  DBIERR_OPENTBLLIMIT     (ERRBASE_LIMIT + ERRCODE_OPENTBLLIMIT)
#define  DBIERR_TBLCURSORLIMIT   (ERRBASE_LIMIT + ERRCODE_TBLCURSORLIMIT)
#define  DBIERR_RECLOCKLIMIT     (ERRBASE_LIMIT + ERRCODE_RECLOCKLIMIT)
#define  DBIERR_CLIENTSLIMIT     (ERRBASE_LIMIT + ERRCODE_CLIENTSLIMIT)
#define  DBIERR_INDEXLIMIT       (ERRBASE_LIMIT + ERRCODE_INDEXLIMIT)
#define  DBIERR_SESSIONSLIMIT    (ERRBASE_LIMIT + ERRCODE_SESSIONSLIMIT)
#define  DBIERR_DBLIMIT          (ERRBASE_LIMIT + ERRCODE_DBLIMIT)
#define  DBIERR_PASSWORDLIMIT    (ERRBASE_LIMIT + ERRCODE_PASSWORDLIMIT)
#define  DBIERR_DRIVERLIMIT      (ERRBASE_LIMIT + ERRCODE_DRIVERLIMIT)
#define  DBIERR_FLDLIMIT         (ERRBASE_LIMIT + ERRCODE_FLDLIMIT)
#define  DBIERR_TBLLOCKLIMIT     (ERRBASE_LIMIT + ERRCODE_TBLLOCKLIMIT)
#define  DBIERR_OPENBLOBLIMIT    (ERRBASE_LIMIT + ERRCODE_OPENBLOBLIMIT)
#define  DBIERR_LOCKFILELIMIT    (ERRBASE_LIMIT + ERRCODE_LOCKFILELIMIT)


// ERRCAT_INTEGRITY
// ================

#define ERRCODE_KEYVIOL             1     // Key violation
#define ERRCODE_MINVALERR           2     // Min val check failed
#define ERRCODE_MAXVALERR           3     // Max val check failed
#define ERRCODE_REQDERR             4     // Field value required
#define ERRCODE_FORIEGNKEYERR       5     // Master record missing
#define ERRCODE_DETAILRECORDSEXIST  6     // Cannot MODIFY or DELETE this Master record
#define ERRCODE_MASTERTBLLEVEL      7     // Master Table Level is incorrect
#define ERRCODE_LOOKUPTABLEERR      8     // Field value out of lookup tbl range
#define ERRCODE_LOOKUPTBLOPENERR    9     // Lookup Table Open failed
#define ERRCODE_DETAILTBLOPENERR   10     // 0x0a Detail Table Open failed
#define ERRCODE_MASTERTBLOPENERR   11     // 0x0b Master Table Open failed
#define ERRCODE_FIELDISBLANK       12     // 0x0c Field is blank

#define ERRCODE_MASTEREXISTS       13     // 0x0d Master Table exists
#define ERRCODE_MASTERTBLOPEN      14     // 0x0e Master Table is open

#define ERRCODE_DETAILTABLESEXIST    15   // 0x0f Detail Tables exist ( cannot delete, rename ... )
#define ERRCODE_DETAILRECEXISTEMPTY  16   // 0x10 Cannot empty because details exist
#define ERRCODE_MASTERREFERENCEERR   17   // 0x11 Cannot modify while adding self referencing Referential Integrity
#define ERRCODE_DETAILTBLOPEN        18   // 0x12 Detail Table is opened
#define ERRCODE_DEPENDENTSMUSTBEEMPTY  19 // 0x13 Cannot make a master a detail of another table if its details are not empty !
#define ERRCODE_RINTREQINDEX         20   // 0x14 Ref. integrity fields must be indexed
#define ERRCODE_LINKEDTBLPROTECTED   21   // 0x15 Master Table is protected ( requires password to open)
#define ERRCODE_FIELDMULTILINKED     22   // 0x16 Field has more than one master
					  
               
#define DBIERR_KEYVIOL             (ERRBASE_INTEGRITY + ERRCODE_KEYVIOL)
#define DBIERR_MINVALERR           (ERRBASE_INTEGRITY + ERRCODE_MINVALERR)
#define DBIERR_MAXVALERR           (ERRBASE_INTEGRITY + ERRCODE_MAXVALERR)
#define DBIERR_REQDERR             (ERRBASE_INTEGRITY + ERRCODE_REQDERR)
#define DBIERR_FORIEGNKEYERR       (ERRBASE_INTEGRITY + ERRCODE_FORIEGNKEYERR)
#define DBIERR_DETAILRECORDSEXIST  (ERRBASE_INTEGRITY + ERRCODE_DETAILRECORDSEXIST)
#define DBIERR_MASTERTBLLEVEL      (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLLEVEL)
#define DBIERR_LOOKUPTABLEERR      (ERRBASE_INTEGRITY + ERRCODE_LOOKUPTABLEERR)
#define DBIERR_LOOKUPTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_LOOKUPTBLOPENERR)
#define DBIERR_DETAILTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_DETAILTBLOPENERR)
#define DBIERR_MASTERTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLOPENERR)
#define DBIERR_FIELDISBLANK        (ERRBASE_INTEGRITY + ERRCODE_FIELDISBLANK)
#define DBIERR_MASTEREXISTS        (ERRBASE_INTEGRITY + ERRCODE_MASTEREXISTS)
#define DBIERR_MASTERTBLOPEN       (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLOPEN)
#define DBIERR_DETAILTABLESEXIST   (ERRBASE_INTEGRITY + ERRCODE_DETAILTABLESEXIST)
#define DBIERR_DETAILRECEXISTEMPTY (ERRBASE_INTEGRITY + ERRCODE_DETAILRECEXISTEMPTY)
#define DBIERR_MASTERREFERENCEERR  (ERRBASE_INTEGRITY + ERRCODE_MASTERREFERENCEERR)
#define DBIERR_DETAILTBLOPEN       (ERRBASE_INTEGRITY + ERRCODE_DETAILTBLOPEN)
#define DBIERR_DEPENDENTSMUSTBEEMPTY  (ERRBASE_INTEGRITY + ERRCODE_DEPENDENTSMUSTBEEMPTY)
#define DBIERR_RINTREQINDEX        (ERRBASE_INTEGRITY + ERRCODE_RINTREQINDEX)
#define DBIERR_LINKEDTBLPROTECTED  (ERRBASE_INTEGRITY + ERRCODE_LINKEDTBLPROTECTED)
#define DBIERR_FIELDMULTILINKED    (ERRBASE_INTEGRITY + ERRCODE_FIELDMULTILINKED)



// ERRCAT_INVALIDREQ
// =================

#define  ERRCODE_OUTOFRANGE           1     // Number out of range (e.g field no)
#define  ERRCODE_INVALIDPARAM         2     // Generic invalid parameter
#define  ERRCODE_INVALIDFILENAME      3     // Invalid file name
#define  ERRCODE_NOSUCHFILE           4     // No such file
#define  ERRCODE_INVALIDOPTION        5     // Invalid option for a parameter
#define  ERRCODE_INVALIDHNDL          6     // Invalid handle to the function
#define  ERRCODE_UNKNOWNTBLTYPE       7     // Table type given not known
#define  ERRCODE_UNKNOWNFILE          8     // Dont know how to open file
#define  ERRCODE_PRIMARYKEYREDEFINE   9     // Cannot redefine primary key
#define  ERRCODE_INVALIDRINTDESCNUM   10    // 0x0a Cannot change this RINTDesc
#define  ERRCODE_KEYFLDTYPEMISMATCH   11    // 0x0b Foreign & Primary Key Mismatch
#define  ERRCODE_INVALIDMODIFYREQUEST 12    // 0x0c Invalid modify request
#define  ERRCODE_NOSUCHINDEX          13    // 0x0d Index does not exist
#define  ERRCODE_INVALIDBLOBOFFSET    14    // 0x0e Invalid Offset into the Blob
#define  ERRCODE_INVALIDDESCNUM       15    // 0x0f Invalid descriptor number
#define  ERRCODE_INVALIDFLDTYPE       16    // 0x10 Invalid field type
#define  ERRCODE_INVALIDFLDDESC       17    // 0x11 Invalid field descriptor
#define  ERRCODE_INVALIDFLDXFORM      18    // 0x12 Invalid field transform
#define  ERRCODE_INVALIDRECSTRUCT     19    // 0x13 Invalid record structure
#define  ERRCODE_INVALIDDESC          20    // 0x14 Generic: invalid descriptor
#define  ERRCODE_INVALIDINDEXSTRUCT   21    // 0x15 Invalid array of indexes descriptors
#define  ERRCODE_INVALIDVCHKSTRUCT    22    // 0x16 Invalid array of  val. check descriptors
#define  ERRCODE_INVALIDRINTSTRUCT    23    // 0x17 Invalid array of ref. integrity descriptors
#define  ERRCODE_INVALIDRESTRTBLORDER 24    // 0x18 Invalid ordering of tables during restructure
#define  ERRCODE_NAMENOTUNIQUE        25    // 0x19 Name not unique in this context
#define  ERRCODE_INDEXNAMEREQUIRED    26    // 0x1a Index name required
#define  ERRCODE_INVALIDSESHANDLE     27    // 0x1b Invalid ses handle
#define  ERRCODE_INVALIDRESTROP       28    // 0x1c Invalid restructure operation
#define  ERRCODE_UNKNOWNDRIVER        29    // 0x1d Driver not known to system
#define  ERRCODE_UNKNOWNDB            30    // 0x1e Unknown db
#define  ERRCODE_INVALIDPASSWORD      31    // 0x1f Invalid password given
#define  ERRCODE_NOCALLBACK           32    // 0x20 No callback function
#define  ERRCODE_INVALIDCALLBACKBUFLEN 33   // 0x21 Invalid callback buffer length
#define  ERRCODE_INVALIDDIR            34   // 0x22 Invalid directory
#define  ERRCODE_INVALIDXLATION        35   // 0x23 Translate Error - Translate DID NOT happen 
#define  ERRCODE_DIFFERENTTABLES       36   // 0x24 Cannot Set Cursor of one Table to another 
#define  ERRCODE_INVALIDBOOKMARK       37   // 0x25 Bookmarks does not match table, etc.
#define  ERRCODE_INVALIDINDEXNAME      38   // 0x26 Index/Tag Name is invalid
#define  ERRCODE_INVALIDIDXDESC        39   // 0x27 Invalid index descriptor
#define  ERRCODE_NOSUCHTABLE           40   // 0x28 No such table
#define  ERRCODE_USECOUNT              41   // 0x29 Table has too many users
#define  ERRCODE_INVALIDKEY            42   // 0x2a Key does not pass filter condition
#define  ERRCODE_INDEXEXISTS           43   // 0x2b Index already exists
#define  ERRCODE_INDEXOPEN             44   // 0x2c Index is open
#define  ERRCODE_INVALIDBLOBLEN        45   // 0x2d Invalid Blob Length
#define  ERRCODE_INVALIDBLOBHANDLE     46   // 0x2e Invalid Blob handle (in record buffer)
#define  ERRCODE_TABLEOPEN             47   // 0x2f Table is open
#define  ERRCODE_NEEDRESTRUCTURE       48   // 0x30 Need to do (hard) restructure
#define  ERRCODE_INVALIDMODE           49   // 0x31 Invalid mode
#define  ERRCODE_CANNOTCLOSE            50  // 0x32 Cannot close index
#define  ERRCODE_ACTIVEINDEX            51  // 0x33 Index is being used to order tbl
#define  ERRCODE_INVALIDUSRPASS         52  // 0x34 Bad user name or password
#define  ERRCODE_MULTILEVELCASCADE      53  // 0x35 Multi level Cascade not supported
#define  ERRCODE_INVALIDFIELDNAME       54  // 0x36 Invalid field name
#define  ERRCODE_INVALIDTABLENAME       55  // 0x37 Invalid table name
#define  ERRCODE_INVALIDLINKEXPR        56  // 0x38 Invalid linked cursor expression
#define  ERRCODE_NAMERESERVED           57  // 0x39 Name is reserved
#define  ERRCODE_INVALIDFILEEXTN        58  // 0x3a Invalid file extention
#define  ERRCODE_INVALIDLANGDRV         59  // 0x3b Invalid language driver
#define  ERRCODE_ALIASNOTOPEN           60  // 0x3c Requested alias in not open
#define  ERRCODE_INCOMPATRECSTRUCTS     61  // 0x3d Incompatible record structures
#define  ERRCODE_RESERVEDDOSNAME        62  // 0x3e Reserved dos name
#define  ERRCODE_DESTMUSTBEINDEXED      63  // 0x3f Destination must be indexed
#define  ERRCODE_INVALIDINDEXTYPE       64  // 0x40 Invalid index type
#define  ERRCODE_LANGDRVMISMATCH        65  // 0x41 Language driver of table and index do not match
#define  ERRCODE_NOSUCHFILTER           66  // 0x42 Filter handle is invalid
#define  ERRCODE_INVALIDFILTER          67  // 0x43 Invalid filter

#define  ERRCODE_INVALIDTABLECREATE     68  // 0x44 Bad table create request (exact prob unknown)
#define  ERRCODE_INVALIDTABLEDELETE     69  // 0x45 Bad table delete request (exact prob unknown)
#define  ERRCODE_INVALIDINDEXCREATE     70  // 0x46 Bad index create request (exact prob unknown)
#define  ERRCODE_INVALIDINDEXDELETE     71  // 0x47 Bad index delete request (exact prob unknown)
#define  ERRCODE_INVALIDTABLE           72  // 0x48 Invalid table name specified
#define  ERRCODE_MULTIRESULTS           73  // 0X49 Multi results
#define  ERRCODE_INVALIDTIME            74  // 0X4A Multi results
#define  ERRCODE_INVALIDDATE            75  // 0X4B Multi results
#define  ERRCODE_INVALIDTIMESTAMP       76  // 0X4C Multi results
#define  ERRCODE_DIFFERENTPATH          77  // 0X4d Tables in different paths 
#define  ERRCODE_MISMATCHARGS           78  // 0x4e MisMatch in the # of arguments
#define  ERRCODE_FUNCTIONNOTFOUND       79  // 0x4f Loaderlib cant find a func in the DLL (bad version?)
#define  ERRCODE_MUSTUSEBASEORDER       80  // 0x50 Must use baseorder for this operation

				 								  			
#define  DBIERR_OUTOFRANGE           (ERRBASE_INVALIDREQ + ERRCODE_OUTOFRANGE)
#define  DBIERR_INVALIDPARAM         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDPARAM)
#define  DBIERR_INVALIDFILENAME      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILENAME)
#define  DBIERR_NOSUCHFILE           (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHFILE)
#define  DBIERR_INVALIDOPTION        (ERRBASE_INVALIDREQ + ERRCODE_INVALIDOPTION)
#define  DBIERR_INVALIDHNDL          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDHNDL)
#define  DBIERR_UNKNOWNTBLTYPE       (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNTBLTYPE)
#define  DBIERR_UNKNOWNFILE          (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNFILE)
#define  DBIERR_PRIMARYKEYREDEFINE   (ERRBASE_INVALIDREQ + ERRCODE_PRIMARYKEYREDEFINE)
#define  DBIERR_INVALIDRINTDESCNUM   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRINTDESCNUM)
#define  DBIERR_KEYFLDTYPEMISMATCH   (ERRBASE_INVALIDREQ + ERRCODE_KEYFLDTYPEMISMATCH)
#define  DBIERR_INVALIDMODIFYREQUEST (ERRBASE_INVALIDREQ + ERRCODE_INVALIDMODIFYREQUEST)
#define  DBIERR_NOSUCHINDEX          (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHINDEX)
#define  DBIERR_INVALIDBLOBOFFSET    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBOFFSET)
#define  DBIERR_INVALIDDESCNUM       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDESCNUM)
#define  DBIERR_INVALIDFLDTYPE       (ERRBASE_INVALIDREQ +  ERRCODE_INVALIDFLDTYPE)
#define  DBIERR_INVALIDFLDDESC       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFLDDESC)
#define  DBIERR_INVALIDFLDXFORM      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFLDXFORM)
#define  DBIERR_INVALIDRECSTRUCT     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRECSTRUCT)
#define  DBIERR_INVALIDDESC          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDESC)
#define  DBIERR_INVALIDINDEXSTRUCT   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXSTRUCT)
#define  DBIERR_INVALIDVCHKSTRUCT    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDVCHKSTRUCT)
#define  DBIERR_INVALIDRINTSTRUCT    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRINTSTRUCT)
#define  DBIERR_INVALIDRESTRTBLORDER (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRESTRTBLORDER)
#define  DBIERR_NAMENOTUNIQUE        (ERRBASE_INVALIDREQ + ERRCODE_NAMENOTUNIQUE)
#define  DBIERR_INDEXNAMEREQUIRED    (ERRBASE_INVALIDREQ + ERRCODE_INDEXNAMEREQUIRED)
#define  DBIERR_INVALIDSESHANDLE     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDSESHANDLE)
#define  DBIERR_INVALIDRESTROP       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRESTROP)
#define  DBIERR_UNKNOWNDRIVER        (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNDRIVER)
#define  DBIERR_UNKNOWNDB            (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNDB)
#define  DBIERR_INVALIDPASSWORD      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDPASSWORD)
#define  DBIERR_NOCALLBACK           (ERRBASE_INVALIDREQ + ERRCODE_NOCALLBACK)
#define  DBIERR_INVALIDCALLBACKBUFLEN \
  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDCALLBACKBUFLEN )
#define  DBIERR_INVALIDDIR          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDIR)
#define  DBIERR_INVALIDXLATION      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDXLATION)
#define  DBIERR_DIFFERENTTABLES     (ERRBASE_INVALIDREQ + ERRCODE_DIFFERENTTABLES)
#define  DBIERR_INVALIDBOOKMARK     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBOOKMARK)
#define  DBIERR_INVALIDINDEXNAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXNAME)
#define  DBIERR_INVALIDIDXDESC      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDIDXDESC)
#define  DBIERR_NOSUCHTABLE         (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHTABLE)
#define  DBIERR_USECOUNT            (ERRBASE_INVALIDREQ + ERRCODE_USECOUNT)
#define  DBIERR_INVALIDKEY          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDKEY)
#define  DBIERR_INDEXEXISTS         (ERRBASE_INVALIDREQ + ERRCODE_INDEXEXISTS)
#define  DBIERR_INDEXOPEN           (ERRBASE_INVALIDREQ + ERRCODE_INDEXOPEN)
#define  DBIERR_INVALIDBLOBLEN      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBLEN)
#define  DBIERR_INVALIDBLOBHANDLE   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBHANDLE)
#define  DBIERR_TABLEOPEN           (ERRBASE_INVALIDREQ + ERRCODE_TABLEOPEN)
#define  DBIERR_NEEDRESTRUCTURE     (ERRBASE_INVALIDREQ + ERRCODE_NEEDRESTRUCTURE)
#define  DBIERR_INVALIDMODE         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDMODE)
#define  DBIERR_CANNOTCLOSE         (ERRBASE_INVALIDREQ + ERRCODE_CANNOTCLOSE)
#define  DBIERR_ACTIVEINDEX         (ERRBASE_INVALIDREQ + ERRCODE_ACTIVEINDEX)
#define  DBIERR_INVALIDUSRPASS      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDUSRPASS)
#define  DBIERR_MULTILEVELCASCADE   (ERRBASE_INVALIDREQ + ERRCODE_MULTILEVELCASCADE)
#define  DBIERR_INVALIDFIELDNAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFIELDNAME)
#define  DBIERR_INVALIDTABLENAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLENAME)
#define  DBIERR_INVALIDLINKEXPR     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDLINKEXPR)
#define  DBIERR_NAMERESERVED        (ERRBASE_INVALIDREQ + ERRCODE_NAMERESERVED)
#define  DBIERR_INVALIDFILEEXTN     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILEEXTN)
#define  DBIERR_INVALIDLANGDRV      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDLANGDRV)
#define  DBIERR_ALIASNOTOPEN        (ERRBASE_INVALIDREQ + ERRCODE_ALIASNOTOPEN)
#define  DBIERR_INCOMPATRECSTRUCTS  (ERRBASE_INVALIDREQ + ERRCODE_INCOMPATRECSTRUCTS)
#define  DBIERR_RESERVEDOSNAME      (ERRBASE_INVALIDREQ + ERRCODE_RESERVEDDOSNAME)
#define  DBIERR_DESTMUSTBEINDEXED   (ERRBASE_INVALIDREQ + ERRCODE_DESTMUSTBEINDEXED)
#define  DBIERR_INVALIDINDEXTYPE    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXTYPE)
#define  DBIERR_LANGDRVMISMATCH     (ERRBASE_INVALIDREQ + ERRCODE_LANGDRVMISMATCH)
#define  DBIERR_NOSUCHFILTER        (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHFILTER)
#define  DBIERR_INVALIDFILTER       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILTER)
#define  DBIERR_INVALIDTABLECREATE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLECREATE)
#define  DBIERR_INVALIDTABLEDELETE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLEDELETE)
#define  DBIERR_INVALIDINDEXCREATE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXCREATE)
#define  DBIERR_INVALIDINDEXDELETE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXDELETE)
#define  DBIERR_INVALIDTABLE        (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLE)
#define  DBIERR_MULTIRESULTS        (ERRBASE_INVALIDREQ + ERRCODE_MULTIRESULTS)
#define  DBIERR_INVALIDTIME         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTIME)
#define  DBIERR_INVALIDDATE         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDATE)
#define  DBIERR_INVALIDTIMESTAMP    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTIMESTAMP)
#define  DBIERR_DIFFERENTPATH       (ERRBASE_INVALIDREQ + ERRCODE_DIFFERENTPATH)
#define  DBIERR_MISMATCHARGS        (ERRBASE_INVALIDREQ + ERRCODE_MISMATCHARGS)
#define  DBIERR_FUNCTIONNOTFOUND    (ERRBASE_INVALIDREQ + ERRCODE_FUNCTIONNOTFOUND)
#define  DBIERR_MUSTUSEBASEORDER    (ERRBASE_INVALIDREQ + ERRCODE_MUSTUSEBASEORDER)
						   											  		 
// ERRCAT_LOCKCONFLICT
// ===================

#define ERRCODE_LOCKED           1
#define ERRCODE_UNLOCKFAILED     2
#define ERRCODE_FILEBUSY         3
#define ERRCODE_DIRBUSY          4
#define ERRCODE_FILELOCKED       5
#define ERRCODE_DIRLOCKED        6
#define ERRCODE_ALREADYLOCKED    7
#define ERRCODE_NOTLOCKED        8
#define ERRCODE_LOCKTIMEOUT      9
#define ERRCODE_GROUPLOCKED      10    // 0x0a
#define ERRCODE_LOSTTBLLOCK      11    // 0x0b
#define ERRCODE_LOSTEXCLACCESS   12    // 0x0c
#define ERRCODE_NEEDEXCLACCESS   13    // 0x0d
#define ERRCODE_RECGROUPCONFLICT 14    // 0x0e
#define ERRCODE_DEADLOCK         15
#define ERRCODE_ACTIVETRAN       16
#define ERRCODE_NOACTIVETRAN     17
#define ERRCODE_RECLOCKFAILED    18
#define ERRCODE_OPTRECLOCKFAILED 19
#define ERRCODE_OPTRECLOCKRECDEL 20
#define ERRCODE_LOCKEDRECS       21
#define ERRCODE_NEEDWRITELOCK    22

#define DBIERR_LOCKED            (ERRBASE_LOCKCONFLICT + ERRCODE_LOCKED)
#define DBIERR_UNLOCKFAILED      (ERRBASE_LOCKCONFLICT + ERRCODE_UNLOCKFAILED)
#define DBIERR_FILEBUSY          (ERRBASE_LOCKCONFLICT + ERRCODE_FILEBUSY)
#define DBIERR_DIRBUSY           (ERRBASE_LOCKCONFLICT + ERRCODE_DIRBUSY)
#define DBIERR_FILELOCKED        (ERRBASE_LOCKCONFLICT + ERRCODE_FILELOCKED)
#define DBIERR_DIRLOCKED         (ERRBASE_LOCKCONFLICT + ERRCODE_DIRLOCKED)
#define DBIERR_ALREADYLOCKED     (ERRBASE_LOCKCONFLICT + ERRCODE_ALREADYLOCKED)
#define DBIERR_NOTLOCKED         (ERRBASE_LOCKCONFLICT + ERRCODE_NOTLOCKED)
#define DBIERR_LOCKTIMEOUT       (ERRBASE_LOCKCONFLICT + ERRCODE_LOCKTIMEOUT)
#define DBIERR_GROUPLOCKED       (ERRBASE_LOCKCONFLICT + ERRCODE_GROUPLOCKED)
#define DBIERR_LOSTTBLLOCK       (ERRBASE_LOCKCONFLICT + ERRCODE_LOSTTBLLOCK)
#define DBIERR_LOSTEXCLACCESS    (ERRBASE_LOCKCONFLICT + ERRCODE_LOSTEXCLACCESS)
#define DBIERR_NEEDEXCLACCESS    (ERRBASE_LOCKCONFLICT  + ERRCODE_NEEDEXCLACCESS)
#define DBIERR_RECGROUPCONFLICT  (ERRBASE_LOCKCONFLICT + ERRCODE_RECGROUPCONFLICT)
#define DBIERR_DEADLOCK          (ERRBASE_LOCKCONFLICT + ERRCODE_DEADLOCK)
#define DBIERR_ACTIVETRAN        (ERRBASE_LOCKCONFLICT + ERRCODE_ACTIVETRAN)
#define DBIERR_NOACTIVETRAN      (ERRBASE_LOCKCONFLICT + ERRCODE_NOACTIVETRAN)
#define DBIERR_RECLOCKFAILED     (ERRBASE_LOCKCONFLICT + ERRCODE_RECLOCKFAILED)
#define DBIERR_OPTRECLOCKFAILED  (ERRBASE_LOCKCONFLICT + ERRCODE_OPTRECLOCKFAILED)
#define DBIERR_OPTRECLOCKRECDEL  (ERRBASE_LOCKCONFLICT + ERRCODE_OPTRECLOCKRECDEL)

// ERRCAT_SECURITY
// ===============

#define ERRCODE_NOTSUFFFIELDRIGHTS   1        // Not sufficient field  rights for operation
#define ERRCODE_NOTSUFFTABLERIGHTS   2        // Not sufficient table  rights for operation
#define ERRCODE_NOTSUFFFAMILYRIGHTS  3        // Not sufficient family rights for operation
#define ERRCODE_READONLYDIR          4        // Is a read-only directory
#define ERRCODE_READONLYDB           5        // Database is read-only
#define ERRCODE_READONLYFLD          6        // Trying to modify read-only field
#define ERRCODE_TBLENCRYPTED         7        // Table is encrypted (dBASE only)
#define ERRCODE_NOTSUFFSQLRIGHTS     8        // Not sufficient sql rights for operation


#define DBIERR_NOTSUFFFIELDRIGHTS  (ERRBASE_SEC + ERRCODE_NOTSUFFFIELDRIGHTS)
#define DBIERR_NOTSUFFTABLERIGHTS  (ERRBASE_SEC + ERRCODE_NOTSUFFTABLERIGHTS)
#define DBIERR_NOTSUFFFAMILYRIGHTS (ERRBASE_SEC + ERRCODE_NOTSUFFFAMILYRIGHTS)
#define DBIERR_READONLYDIR         (ERRBASE_SEC + ERRCODE_READONLYDIR)
#define DBIERR_READONLYDB          (ERRBASE_SEC + ERRCODE_READONLYDB)
#define DBIERR_READONLYFLD         (ERRBASE_SEC + ERRCODE_READONLYFLD)
#define DBIERR_TBLENCRYPTED        (ERRBASE_SEC + ERRCODE_TBLENCRYPTED)
#define DBIERR_NOTSUFFSQLRIGHTS    (ERRBASE_SEC + ERRCODE_NOTSUFFSQLRIGHTS) 


// ERRCAT_INVALIDCONTEXT
// =====================

#define ERRCODE_NOTABLOB         1        // Field is not a blob
#define ERRCODE_BLOBOPENED       2        // Blob already opened
#define ERRCODE_BLOBNOTOPENED    3        // Blob not opened
#define ERRCODE_NA               4        // Operation not applicable
#define ERRCODE_NOTINDEXED       5        // Table is not indexed
#define ERRCODE_NOTINITIALIZED   6        // Engine not initialized
#define ERRCODE_MULTIPLEINIT     7        // Attempt to re-initialize engine
#define ERRCODE_NOTSAMESESSION   8        // Attempt to mix objs from diff ses
#define ERRCODE_PDXDRIVERNOTACTIVE 9      // Paradox driver not active
#define ERRCODE_DRIVERNOTLOADED  10       // 0x0a Driver not loaded
#define ERRCODE_TABLEREADONLY    11       // 0x0b Table is read only
#define ERRCODE_NOASSOCINDEX     12       // 0x0c No index associated with the cursor
#define ERRCODE_HASOPENCURSORS   13       // 0x0d Has open cursors
#define ERRCODE_NOTABLESUPPORT   14       // 0x0e Op cannot be done on this table
#define ERRCODE_INDEXREADONLY    15       // 0x0f Index is read only 
#define ERRCODE_NOUNIQUERECS     16       // 0x10 Records are not unique
#define ERRCODE_NOTCURSESSION    17       // 0x11 Not the current/active session
#define ERRCODE_INVALIDKEYWORD   18       // 0x12 Invalid use of keyword.


#define DBIERR_NOTABLOB           (ERRBASE_IC + ERRCODE_NOTABLOB)
#define DBIERR_BLOBOPENED         (ERRBASE_IC + ERRCODE_BLOBOPENED)
#define DBIERR_BLOBNOTOPENED      (ERRBASE_IC + ERRCODE_BLOBNOTOPENED)
#define DBIERR_NA                 (ERRBASE_IC + ERRCODE_NA)
#define DBIERR_NOTINDEXED         (ERRBASE_IC + ERRCODE_NOTINDEXED)
#define DBIERR_NOTINITIALIZED     (ERRBASE_IC + ERRCODE_NOTINITIALIZED)
#define DBIERR_MULTIPLEINIT       (ERRBASE_IC + ERRCODE_MULTIPLEINIT)
#define DBIERR_NOTSAMESESSION     (ERRBASE_IC + ERRCODE_NOTSAMESESSION)
#define DBIERR_PDXDRIVERNOTACTIVE (ERRBASE_IC + ERRCODE_PDXDRIVERNOTACTIVE)
#define DBIERR_DRIVERNOTLOADED    (ERRBASE_IC + ERRCODE_DRIVERNOTLOADED)
#define DBIERR_TABLEREADONLY      (ERRBASE_IC + ERRCODE_TABLEREADONLY)
#define DBIERR_NOASSOCINDEX       (ERRBASE_IC + ERRCODE_NOASSOCINDEX)
#define DBIERR_HASOPENCURSORS     (ERRBASE_IC + ERRCODE_HASOPENCURSORS)
#define DBIERR_NOTABLESUPPORT     (ERRBASE_IC + ERRCODE_NOTABLESUPPORT)
#define DBIERR_INDEXREADONLY      (ERRBASE_IC + ERRCODE_INDEXREADONLY)
#define DBIERR_NOUNIQUERECS       (ERRBASE_IC + ERRCODE_NOUNIQUERECS)
#define DBIERR_NOTCURSESSION      (ERRBASE_IC + ERRCODE_NOTCURSESSION)
#define DBIERR_INVALIDKEYWORD     (ERRBASE_IC + ERRCODE_INVALIDKEYWORD)


// ERRCAT_OS
// =========
// DOS extended errors:

#define ERRCODE_OSEINVFNC     1        // Invalid function number
#define ERRCODE_OSENOENT      2        // No such file or directory
#define ERRCODE_OSENOPATH     3        // Path not found
#define ERRCODE_OSEMFILE      4        // Too many open files
#define ERRCODE_OSEACCES      5        // Permission denied
#define ERRCODE_OSEBADF       6        // Bad file number
#define ERRCODE_OSECONTR      7        // Memory blocks destroyed
#define ERRCODE_OSENOMEM      8        // Not enough core
#define ERRCODE_OSEINVMEM     9        // Invalid memory block address
#define ERRCODE_OSEINVENV     10       // 0x0a Invalid environment
#define ERRCODE_OSEINVFMT     11       // 0x0b Invalid format
#define ERRCODE_OSEINVACC     12       // 0x0c Invalid access code
#define ERRCODE_OSEINVDAT     13       // 0x0d Invalid data
#define ERRCODE_OSENODEV      15       // 0x0f No such device
#define ERRCODE_OSECURDIR     16       // 0x10 Attempt to remove curdir
#define ERRCODE_OSENOTSAM     17       // 0x11 Not same device
#define ERRCODE_OSENMFILE     18       // 0x12 No more files
#define ERRCODE_OSEINVAL      19       // 0x13 Invalid argument
#define ERRCODE_OSE2BIG       20       // 0x14 Arg list too long
#define ERRCODE_OSENOEXEC     21       // 0x15 Exec format error
#define ERRCODE_OSEXDEV       22       // 0x16 Cross-device link
#define ERRCODE_OSEDOM        33       // 0x21 Math argument
#define ERRCODE_OSERANGE      34       // 0x22 Result to large
#define ERRCODE_OSEEXIST      35       // 0x23 File already exists
#define ERRCODE_OSUNKNOWN     39       // 0x27 Unkown | illegal error from rtl 

#define ERRCODE_OSSHAREVIOL   50       // 0x32 Share viol, ext. err 0x20
#define ERRCODE_OSLOCKVIOL    51       // 0x33 Lock viol, ext. err 0x21
#define ERRCODE_OSINT24FAIL   52       // 0x34 INT24 called
#define ERRCODE_OSDRIVENOTREADY   53   // 0x35 Drive not ready



// OTHER Os errors:
// 1. odapi errors
// 2. errors from non-dos systems ( i.e. NOVELL )

#define ERRCODE_NOTEXACT          100       // 0x64 Not exact read/write
#define ERRCODE_OSNETERR          101       // 0x65 Generic network error 
#define ERRCODE_OSUNKNOWNSRVERR   102       // 0x66 Error from file server
#define ERRCODE_SERVERNOMEMORY    103       // 0x67 Server out of memory
#define ERRCODE_OSALREADYLOCKED   104       // 0x68 Record already locked (by you)
#define ERRCODE_OSNOTLOCKED       105       // 0x69 Record not locked



#define   DBIERR_OSEINVFNC       ( ERRBASE_OS + ERRCODE_OSEINVFNC )
#define   DBIERR_OSENOENT        ( ERRBASE_OS + ERRCODE_OSENOENT )
#define   DBIERR_OSENOPATH       ( ERRBASE_OS + ERRCODE_OSENOPATH )
#define   DBIERR_OSEMFILE        ( ERRBASE_OS + ERRCODE_OSEMFILE )
#define   DBIERR_OSEACCES        ( ERRBASE_OS + ERRCODE_OSEACCES )
#define   DBIERR_OSEBADF         ( ERRBASE_OS + ERRCODE_OSEBADF )
#define   DBIERR_OSECONTR        ( ERRBASE_OS + ERRCODE_OSECONTR )
#define   DBIERR_OSENOMEM        ( ERRBASE_OS + ERRCODE_OSENOMEM )
#define   DBIERR_OSEINVMEM       ( ERRBASE_OS + ERRCODE_OSEINVMEM )
#define   DBIERR_OSEINVENV       ( ERRBASE_OS + ERRCODE_OSEINVENV )
#define   DBIERR_OSEINVFMT       ( ERRBASE_OS + ERRCODE_OSEINVFMT )
#define   DBIERR_OSEINVACC       ( ERRBASE_OS + ERRCODE_OSEINVACC )
#define   DBIERR_OSEINVDAT       ( ERRBASE_OS + ERRCODE_OSEINVDAT )
#define   DBIERR_OSENODEV        ( ERRBASE_OS + ERRCODE_OSENODEV )
#define   DBIERR_OSECURDIR       ( ERRBASE_OS + ERRCODE_OSECURDIR )
#define   DBIERR_OSENOTSAM       ( ERRBASE_OS + ERRCODE_OSENOTSAM )
#define   DBIERR_OSENMFILE       ( ERRBASE_OS + ERRCODE_OSENMFILE )
#define   DBIERR_OSEINVAL        ( ERRBASE_OS + ERRCODE_OSEINVAL )
#define   DBIERR_OSE2BIG         ( ERRBASE_OS + ERRCODE_OSE2BIG )
#define   DBIERR_OSENOEXEC       ( ERRBASE_OS + ERRCODE_OSENOEXEC )
#define   DBIERR_OSEXDEV         ( ERRBASE_OS + ERRCODE_OSEXDEV )
#define   DBIERR_OSEDOM          ( ERRBASE_OS + ERRCODE_OSEDOM )
#define   DBIERR_OSERANGE        ( ERRBASE_OS + ERRCODE_OSERANGE )
#define   DBIERR_OSEEXIST        ( ERRBASE_OS + ERRCODE_OSEEXIST )
#define   DBIERR_OSUNKNOWN       ( ERRBASE_OS + ERRCODE_OSUNKNOWN )
#define   DBIERR_OSSHAREVIOL     ( ERRBASE_OS + ERRCODE_OSSHAREVIOL )
#define   DBIERR_OSLOCKVIOL      ( ERRBASE_OS + ERRCODE_OSLOCKVIOL )
#define   DBIERR_OSNETERR        ( ERRBASE_OS + ERRCODE_OSNETERR )
#define   DBIERR_OSINT24FAIL     ( ERRBASE_OS + ERRCODE_OSINT24FAIL )
#define   DBIERR_OSDRIVENOTREADY ( ERRBASE_OS + ERRCODE_OSDRIVENOTREADY )


#define   DBIERR_NOTEXACT        ( ERRBASE_OS + ERRCODE_NOTEXACT )
#define   DBIERR_OSUNKNOWNSRVERR ( ERRBASE_OS + ERRCODE_OSUNKNOWNSRVERR )
#define   DBIERR_SERVERNOMEMORY  ( ERRBASE_OS + ERRCODE_SERVERNOMEMORY )
#define   DBIERR_OSALREADYLOCKED ( ERRBASE_OS + ERRCODE_OSALREADYLOCKED )
#define   DBIERR_OSNOTLOCKED     ( ERRBASE_OS + ERRCODE_OSNOTLOCKED )

// ERRCAT_NETWORK
// ==============

#define ERRCODE_NETINITERR       1        // Net init failed
#define ERRCODE_NETUSERLIMIT     2        // Net user limit exceeded
#define ERRCODE_NETFILEVERSION   3        // Wrong net file version
#define ERRCODE_NETFILELOCKED    4        // Not able to lock net file
#define ERRCODE_DIRNOTPRIVATE    5        // 
#define ERRCODE_NETMULTIPLE      6        // Multiple net files in use
#define ERRCODE_NETUNKNOWN       7        // Unknown net error
#define ERRCODE_SHAREDFILE       8        // Cannot access a shared file
#define ERRCODE_SHARENOTLOADED   9        // Share not loaded
#define ERRCODE_NOTONANETWORK    10       // 0x0a Not an Network
#define ERRCODE_SQLCOMMLOST      11       // 0x0b Lost Communication with SQL server

#define DBIERR_NETINITERR        (ERRBASE_NETWORK + ERRCODE_NETINITERR)
#define DBIERR_NETUSERLIMIT      (ERRBASE_NETWORK + ERRCODE_NETUSERLIMIT)
#define DBIERR_NETFILEVERSION    (ERRBASE_NETWORK + ERRCODE_NETFILEVERSION)
#define DBIERR_NETFILELOCKED     (ERRBASE_NETWORK + ERRCODE_NETFILELOCKED)
#define DBIERR_DIRNOTPRIVATE     (ERRBASE_NETWORK + ERRCODE_DIRNOTPRIVATE)
#define DBIERR_NETMULTIPLE       (ERRBASE_NETWORK + ERRCODE_NETMULTIPLE)
#define DBIERR_NETUNKNOWN        (ERRBASE_NETWORK + ERRCODE_NETUNKNOWN)
#define DBIERR_SHAREDFILE        (ERRBASE_NETWORK + ERRCODE_SHAREDFILE)
#define DBIERR_SHARENOTLOADED    (ERRBASE_NETWORK + ERRCODE_SHARENOTLOADED)
#define DBIERR_NOTONANETWORK     (ERRBASE_NETWORK + ERRCODE_NOTONANETWORK)
#define DBIERR_SQLCOMMLOST       (ERRBASE_NETWORK + ERRCODE_SQLCOMMLOST)

// ERRCAT_DRIVER
// =============

#define ERRCODE_WRONGDRVNAME    1       // Wrong driver name
#define ERRCODE_WRONGSYSVER     2       // Wrong system version
#define ERRCODE_WRONGDRVVER     3       // Wrong driver version
#define ERRCODE_WRONGDRVTYPE    4       // Wrong driver type
#define ERRCODE_CANNOTLOADDRV   5       // Can not load driver
#define ERRCODE_CANNOTLOADLDDRV   6     // Can not load language driver
#define ERRCODE_VENDINITFAIL    7       // Vendor init failure


#define DBIERR_WRONGDRVNAME     (ERRBASE_DRIVER + ERRCODE_WRONGDRVNAME)
#define DBIERR_WRONGSYSVER      (ERRBASE_DRIVER + ERRCODE_WRONGSYSVER)
#define DBIERR_WRONGDRVVER      (ERRBASE_DRIVER + ERRCODE_WRONGDRVVER)
#define DBIERR_WRONGDRVTYPE     (ERRBASE_DRIVER + ERRCODE_WRONGDRVTYPE)
#define DBIERR_CANNOTLOADDRV    (ERRBASE_DRIVER + ERRCODE_CANNOTLOADDRV)
#define DBIERR_CANNOTLOADLDDRV  (ERRBASE_DRIVER + ERRCODE_CANNOTLOADLDDRV)
#define DBIERR_VENDINITFAIL     (ERRBASE_DRIVER + ERRCODE_VENDINITFAIL)


// ERRCAT_QUERY
// ============



#define DBICODE_AMBJOASY       1 // obsolete
#define DBICODE_AMBJOSYM       2 // obsolete
#define DBICODE_AMBOUTEX       3
#define DBICODE_AMBOUTPR       4 // obsolete
#define DBICODE_AMBSYMAS       5 // obsolete
#define DBICODE_ASETOPER       6
#define DBICODE_AVENUMDA       7
#define DBICODE_BADEXPR1       8
#define DBICODE_BADFLDOR       9
#define DBICODE_BADVNAME      10                              // 0x0a
#define DBICODE_BITMAPER      11                              // 0x0b
#define DBICODE_CALCBADR      12                              // 0x0c
#define DBICODE_CALCTYPE      13                              // 0x0d
#define DBICODE_CHGTO1TI      14                              // 0x0e
#define DBICODE_CHGTOCHG      15                              // 0x0f
#define DBICODE_CHGTOEXP      16                              // 0x10
#define DBICODE_CHGTOINS      17                              // 0x11
#define DBICODE_CHGTONEW      18                              // 0x12
#define DBICODE_CHGTOVAL      19                              // 0x13
#define DBICODE_CHKMRKFI      20                              // 0x14
#define DBICODE_CHNAMBIG      21                              // 0x15
#define DBICODE_CHUNKERR      22                              // 0x16
#define DBICODE_COLUM255      23                              // 0x17
#define DBICODE_CONAFTAS      24                              // 0x18
#define DBICODE_DEL1TIME      25                              // 0x19
#define DBICODE_DELAMBIG      26                              // 0x1a
#define DBICODE_DELFRDEL      27                              // 0x1b
#define DBICODE_EGFLDTYP      28                              // 0x1c
#define DBICODE_EXAMINOR      29                              // 0x1d
#define DBICODE_EXPRTYPS      30                              // 0x1e
#define DBICODE_EXTRACOM      31                              // 0x1f
#define DBICODE_EXTRAORO      32                              // 0x20
#define DBICODE_EXTRAQRO      33                              // 0x21
#define DBICODE_FIND1ATT      34                              // 0x22
#define DBICODE_FINDANST      35                              // 0x23
#define DBICODE_GRPNOSET      36                              // 0x24
#define DBICODE_GRPSTROW      37                              // 0x25
#define DBICODE_IDFINLCO      38                              // 0x26
#define DBICODE_IDFPERLI      39                              // 0x27
#define DBICODE_INANEXPR      40                              // 0x28
#define DBICODE_INS1TIME      41                              // 0x29
#define DBICODE_INSAMBIG      42                              // 0x2a
#define DBICODE_INSDELCH      43                              // 0x2b
#define DBICODE_INSEXPRR      44                              // 0x2c
#define DBICODE_INSTOINS      45                              // 0x2d
#define DBICODE_ISARRAY       46                              // 0x2e
#define DBICODE_LABELERR      47                              // 0x2f
#define DBICODE_LINKCALC      48                              // 0x30
#define DBICODE_LNGVNAME      49                              // 0x31
#define DBICODE_LONGQURY      50                              // 0x32
#define DBICODE_MEMVPROC      51                              // 0x33
#define DBICODE_MISNGCOM      52                              // 0x34
#define DBICODE_MISNGRPA      53                              // 0x35
#define DBICODE_MISSRTQU      54                              // 0x36
#define DBICODE_NAMTWICE      55                              // 0x37
#define DBICODE_NOCHKMAR      56                              // 0x38
#define DBICODE_NODEFOCC      57                              // 0x39
#define DBICODE_NOGROUPS      58                              // 0x3a
#define DBICODE_NONSENSE      59                              // 0x3b
#define DBICODE_NOPATTER      60                              // 0x3c
#define DBICODE_NOSUCHDA      61                              // 0x3d
#define DBICODE_NOVALUE       62                              // 0x3e
#define DBICODE_ONLYCONS      63                              // 0x3f
#define DBICODE_ONLYSETR      64                              // 0x40
#define DBICODE_OUTSENS1      65                              // 0x41
#define DBICODE_OUTTWIC1      66                              // 0x42
#define DBICODE_PAROWCNT      67                              // 0x43
#define DBICODE_PERSEPAR      68                              // 0x44
#define DBICODE_PROCPLSW      69                              // 0x45
#define DBICODE_PWINSRTS      70                              // 0x46
#define DBICODE_PWMODRTS      71                              // 0x47
#define DBICODE_QBEFLDFOUND   72                              // 0x48
#define DBICODE_QBENOFENCE    73                              // 0x49
#define DBICODE_QBENOFENCET   74                              // 0x4a
#define DBICODE_QBENOHEADERT  75                              // 0x4b
#define DBICODE_QBENOTAB      76                              // 0x4c
#define DBICODE_QBENUMCOLS    77                              // 0x4d
#define DBICODE_QBEOPENTAB    78                              // 0x4e
#define DBICODE_QBETWICE      79                              // 0x4f
#define DBICODE_QRYNOANSWER   80                              // 0x50
#define DBICODE_QRYNOTPREP    81                              // 0x51
#define DBICODE_QUAINDEL      82                              // 0x52
#define DBICODE_QUAININS      83                              // 0x53
#define DBICODE_RAGININS      84                              // 0x54
#define DBICODE_RAGINSET      85                              // 0x55
#define DBICODE_ROWUSERR      86                              // 0x56
#define DBICODE_SETEXPEC      87                              // 0x57
#define DBICODE_SETVAMB1      88                              // 0x58
#define DBICODE_SETVBAD1      89                              // 0x59
#define DBICODE_SETVDEF1      90                              // 0x5a
#define DBICODE_SUMNUMBE      91                              // 0x5b
#define DBICODE_TBLISWP3      92                              // 0x5c
#define DBICODE_TOKENNOT      93                              // 0x5d
#define DBICODE_TWOOUTR1      94                              // 0x5e
#define DBICODE_TYPEMISM      95                              // 0x5f
#define DBICODE_UNRELQ1       96                              // 0x60
#define DBICODE_UNUSEDST      97                              // 0x61
#define DBICODE_USEINSDE      98                              // 0x62
#define DBICODE_USEOFCHG      99                              // 0x63
#define DBICODE_VARMUSTF     100                              // 0x64
#define DBICODE_REGISTER     101                              // 0x65
#define DBICODE_LONGEXPR     102                              // 0x66
#define DBICODE_REFRESH      103                              // 0x67
#define DBICODE_CANCEXCEPT   104                              // 0x68
#define DBICODE_DBEXCEPT     105                              // 0x69
#define DBICODE_MEMEXCEPT    106                              // 0x6a
#define DBICODE_FATALEXCEPT  107                              // 0x6b
#define DBICODE_QRYNIY       108                              // 0x6c
#define DBICODE_BADFORMAT    109                              // 0x6d
#define DBICODE_QRYEMPTY     110                              // 0x6e
#define DBICODE_NOQRYTOPREP  111                              // 0x6f
#define DBICODE_BUFFTOOSMALL 112                              // 0x70
#define DBICODE_QRYNOTPARSE  113                              // 0x71
#define DBICODE_NOTHANDLE    114                              // 0x72
#define DBICODE_QRYSYNTERR   115                              // 0x73
#define DBICODE_QXFLDCOUNT   116                              // 0x74
#define DBICODE_QXFLDSYMNOTFOUND 117                          // 0x75
#define DBICODE_QXTBLSYMNOTFOUND 118                          // 0x76
#define DBICODE_BLOBTERM     119                              // 0x77
#define DBICODE_BLOBERR      120                              // 0x78
#define DBICODE_RESTARTQRY   121                              // 0x79
#define DBICODE_UNKNOWNANSTYPE 122                            // 0x7a

// Internal QBE use Only.
   #define DBICODE_SQLG_MDIST 123                           // 0x7b
   #define DBICODE_SQLG_NOARI 124                           // 0x7c
   #define DBICODE_SQLG_LIKEN 125                           // 0x7d
   #define DBICODE_SQLG_ALPHO 126                           // 0x7e
   #define DBICODE_SQLG_DATEO 127                           // 0x7f
   #define DBICODE_SQLG_RELOP 128                           // 0x80
   #define DBICODE_SQLG_ONLYC 129                           // 0x81
   #define DBICODE_SQLG_CNTLN 130                           // 0x82
   #define DBICODE_SQLG_CHINI 131                           // 0x83
   #define DBICODE_SQLG_UNION 132                           // 0x84
   #define DBICODE_SQLG_SLFIN 133                           // 0x85
   #define DBICODE_SQLG_OTJVR 134                           // 0x86
   #define DBICODE_SQLG_STROW 135                           // 0x87
   #define DBICODE_SQLG_QUANT 136                           // 0x88
   #define DBICODE_SQLG_REGSO 137                           // 0x89
   #define DBICODE_SQLG_COUNT 138                           // 0x8a
   #define DBICODE_SQLG_AVERA 139                           // 0x8b
   #define DBICODE_SQLG_DATEA 140                           // 0x8c
   #define DBICODE_SQLG_BADPT 141                           // 0x8d
   #define DBICODE_SQLG_RELPA 142                           // 0x8e
   #define DBICODE_SQLG_PATRN 143                           // 0x8f
   #define DBICODE_SQLG_FNDSU 144                           // 0x90
   #define DBICODE_SQLG_IFDCS 145                           // 0x91
   #define DBICODE_SQLG_IDCCO 146                           // 0x92
   #define DBICODE_SQLG_ONLYI 147                           // 0x93
   #define DBICODE_SQLG_SQLDIALECT 148                      // 0x94
   #define DBICODE_SQLG_NOQUERY 149                         // 0x95
// End of Internal.

#define DBICODE_BLOBGROUP 150                               // 0x96
#define DBICODE_QRYNOPROP  151                              // 0x97
#define DBICODE_ANSTYPNOTSUP  152                           // 0x98
#define DBICODE_ANSALIASNOTSUP  153                         // 0x99
#define DBICODE_INSBLOBREQ      154                         // 0x9a
#define DBICODE_CHGUNIQUENDXREQ 155                         // 0x9b
#define DBICODE_DELUNIQUENDXREQ 156                         // 0x9c
#define DBICODE_SQLNOFULLUPDATE 157                         // 0x9d
#define DBICODE_CANTEXECREMOTE  158                         // 0x9e
#define DBICODE_UNEXPECTEDEOC   159                         // 0x9f
#define DBICODE_SQLPARAMNOTSET  160                         // 0xA0


#define DBIERR_AMBJOASY    (ERRBASE_QUERY+DBICODE_AMBJOASY)
#define DBIERR_AMBJOSYM    (ERRBASE_QUERY+DBICODE_AMBJOSYM)
#define DBIERR_AMBOUTEX    (ERRBASE_QUERY+DBICODE_AMBOUTEX)
#define DBIERR_AMBOUTPR    (ERRBASE_QUERY+DBICODE_AMBOUTPR)
#define DBIERR_AMBSYMAS    (ERRBASE_QUERY+DBICODE_AMBSYMAS)
#define DBIERR_ASETOPER    (ERRBASE_QUERY+DBICODE_ASETOPER)
#define DBIERR_AVENUMDA    (ERRBASE_QUERY+DBICODE_AVENUMDA)
#define DBIERR_BADEXPR1    (ERRBASE_QUERY+DBICODE_BADEXPR1)
#define DBIERR_BADFLDOR    (ERRBASE_QUERY+DBICODE_BADFLDOR)
#define DBIERR_BADVNAME    (ERRBASE_QUERY+DBICODE_BADVNAME)
#define DBIERR_BITMAPER    (ERRBASE_QUERY+DBICODE_BITMAPER)
#define DBIERR_CALCBADR    (ERRBASE_QUERY+DBICODE_CALCBADR)
#define DBIERR_CALCTYPE    (ERRBASE_QUERY+DBICODE_CALCTYPE)
#define DBIERR_CHGTO1TI    (ERRBASE_QUERY+DBICODE_CHGTO1TI)
#define DBIERR_CHGTOCHG    (ERRBASE_QUERY+DBICODE_CHGTOCHG)
#define DBIERR_CHGTOEXP    (ERRBASE_QUERY+DBICODE_CHGTOEXP)
#define DBIERR_CHGTOINS    (ERRBASE_QUERY+DBICODE_CHGTOINS)
#define DBIERR_CHGTONEW    (ERRBASE_QUERY+DBICODE_CHGTONEW)
#define DBIERR_CHGTOVAL    (ERRBASE_QUERY+DBICODE_CHGTOVAL)
#define DBIERR_CHKMRKFI    (ERRBASE_QUERY+DBICODE_CHKMRKFI)
#define DBIERR_CHNAMBIG    (ERRBASE_QUERY+DBICODE_CHNAMBIG)
#define DBIERR_CHUNKERR    (ERRBASE_QUERY+DBICODE_CHUNKERR)
#define DBIERR_COLUM255    (ERRBASE_QUERY+DBICODE_COLUM255)
#define DBIERR_CONAFTAS    (ERRBASE_QUERY+DBICODE_CONAFTAS)
#define DBIERR_DEL1TIME    (ERRBASE_QUERY+DBICODE_DEL1TIME)
#define DBIERR_DELAMBIG    (ERRBASE_QUERY+DBICODE_DELAMBIG)
#define DBIERR_DELFRDEL    (ERRBASE_QUERY+DBICODE_DELFRDEL)
#define DBIERR_EGFLDTYP    (ERRBASE_QUERY+DBICODE_EGFLDTYP)
#define DBIERR_EXAMINOR    (ERRBASE_QUERY+DBICODE_EXAMINOR)
#define DBIERR_EXPRTYPS    (ERRBASE_QUERY+DBICODE_EXPRTYPS)
#define DBIERR_EXTRACOM    (ERRBASE_QUERY+DBICODE_EXTRACOM)
#define DBIERR_EXTRAORO    (ERRBASE_QUERY+DBICODE_EXTRAORO)
#define DBIERR_EXTRAQRO    (ERRBASE_QUERY+DBICODE_EXTRAQRO)
#define DBIERR_FIND1ATT    (ERRBASE_QUERY+DBICODE_FIND1ATT)
#define DBIERR_FINDANST    (ERRBASE_QUERY+DBICODE_FINDANST)
#define DBIERR_GRPNOSET    (ERRBASE_QUERY+DBICODE_GRPNOSET)
#define DBIERR_GRPSTROW    (ERRBASE_QUERY+DBICODE_GRPSTROW)
#define DBIERR_IDFINLCO    (ERRBASE_QUERY+DBICODE_IDFINLCO)
#define DBIERR_IDFPERLI    (ERRBASE_QUERY+DBICODE_IDFPERLI)
#define DBIERR_INANEXPR    (ERRBASE_QUERY+DBICODE_INANEXPR)
#define DBIERR_INS1TIME    (ERRBASE_QUERY+DBICODE_INS1TIME)
#define DBIERR_INSAMBIG    (ERRBASE_QUERY+DBICODE_INSAMBIG)
#define DBIERR_INSDELCH    (ERRBASE_QUERY+DBICODE_INSDELCH)
#define DBIERR_INSEXPRR    (ERRBASE_QUERY+DBICODE_INSEXPRR)
#define DBIERR_INSTOINS    (ERRBASE_QUERY+DBICODE_INSTOINS)
#define DBIERR_ISARRAY     (ERRBASE_QUERY+DBICODE_ISARRAY)
#define DBIERR_LABELERR    (ERRBASE_QUERY+DBICODE_LABELERR)
#define DBIERR_LINKCALC    (ERRBASE_QUERY+DBICODE_LINKCALC)
#define DBIERR_LNGVNAME    (ERRBASE_QUERY+DBICODE_LNGVNAME)
#define DBIERR_LONGQURY    (ERRBASE_QUERY+DBICODE_LONGQURY)
#define DBIERR_MEMVPROC    (ERRBASE_QUERY+DBICODE_MEMVPROC)
#define DBIERR_MISNGCOM    (ERRBASE_QUERY+DBICODE_MISNGCOM)
#define DBIERR_MISNGRPA    (ERRBASE_QUERY+DBICODE_MISNGRPA)
#define DBIERR_MISSRTQU    (ERRBASE_QUERY+DBICODE_MISSRTQU)
#define DBIERR_NAMTWICE    (ERRBASE_QUERY+DBICODE_NAMTWICE)
#define DBIERR_NOCHKMAR    (ERRBASE_QUERY+DBICODE_NOCHKMAR)
#define DBIERR_NODEFOCC    (ERRBASE_QUERY+DBICODE_NODEFOCC)
#define DBIERR_NOGROUPS    (ERRBASE_QUERY+DBICODE_NOGROUPS)
#define DBIERR_NONSENSE    (ERRBASE_QUERY+DBICODE_NONSENSE)
#define DBIERR_NOPATTER    (ERRBASE_QUERY+DBICODE_NOPATTER)
#define DBIERR_NOSUCHDA    (ERRBASE_QUERY+DBICODE_NOSUCHDA)
#define DBIERR_NOVALUE     (ERRBASE_QUERY+DBICODE_NOVALUE)
#define DBIERR_ONLYCONS    (ERRBASE_QUERY+DBICODE_ONLYCONS)
#define DBIERR_ONLYSETR    (ERRBASE_QUERY+DBICODE_ONLYSETR)
#define DBIERR_OUTSENS1    (ERRBASE_QUERY+DBICODE_OUTSENS1)
#define DBIERR_OUTTWIC1    (ERRBASE_QUERY+DBICODE_OUTTWIC1)
#define DBIERR_PAROWCNT    (ERRBASE_QUERY+DBICODE_PAROWCNT)
#define DBIERR_PERSEPAR    (ERRBASE_QUERY+DBICODE_PERSEPAR)
#define DBIERR_PROCPLSW    (ERRBASE_QUERY+DBICODE_PROCPLSW)
#define DBIERR_PWINSRTS    (ERRBASE_QUERY+DBICODE_PWINSRTS)
#define DBIERR_PWMODRTS    (ERRBASE_QUERY+DBICODE_PWMODRTS)
#define DBIERR_QBEFLDFOUND (ERRBASE_QUERY+DBICODE_QBEFLDFOUND)
#define DBIERR_QBENOFENCE  (ERRBASE_QUERY+DBICODE_QBENOFENCE)
#define DBIERR_QBENOFENCET (ERRBASE_QUERY+DBICODE_QBENOFENCET)
#define DBIERR_QBENOHEADERT (ERRBASE_QUERY+DBICODE_QBENOHEADERT)
#define DBIERR_QBENOTAB    (ERRBASE_QUERY+DBICODE_QBENOTAB)
#define DBIERR_QBENUMCOLS  (ERRBASE_QUERY+DBICODE_QBENUMCOLS)
#define DBIERR_QBEOPENTAB  (ERRBASE_QUERY+DBICODE_QBEOPENTAB)
#define DBIERR_QBETWICE    (ERRBASE_QUERY+DBICODE_QBETWICE)
#define DBIERR_QRYNOANSWER (ERRBASE_QUERY+DBICODE_QRYNOANSWER)
#define DBIERR_QRYNOTPREP  (ERRBASE_QUERY+DBICODE_QRYNOTPREP)
#define DBIERR_QUAINDEL    (ERRBASE_QUERY+DBICODE_QUAINDEL)
#define DBIERR_QUAININS    (ERRBASE_QUERY+DBICODE_QUAININS)
#define DBIERR_RAGININS    (ERRBASE_QUERY+DBICODE_RAGININS)
#define DBIERR_RAGINSET    (ERRBASE_QUERY+DBICODE_RAGINSET)
#define DBIERR_ROWUSERR    (ERRBASE_QUERY+DBICODE_ROWUSERR)
#define DBIERR_SETEXPEC    (ERRBASE_QUERY+DBICODE_SETEXPEC)
#define DBIERR_SETVAMB1    (ERRBASE_QUERY+DBICODE_SETVAMB1)
#define DBIERR_SETVBAD1    (ERRBASE_QUERY+DBICODE_SETVBAD1)
#define DBIERR_SETVDEF1    (ERRBASE_QUERY+DBICODE_SETVDEF1)
#define DBIERR_SUMNUMBE    (ERRBASE_QUERY+DBICODE_SUMNUMBE)
#define DBIERR_TBLISWP3    (ERRBASE_QUERY+DBICODE_TBLISWP3)
#define DBIERR_TOKENNOT    (ERRBASE_QUERY+DBICODE_TOKENNOT)
#define DBIERR_TWOOUTR1    (ERRBASE_QUERY+DBICODE_TWOOUTR1)
#define DBIERR_TYPEMISM    (ERRBASE_QUERY+DBICODE_TYPEMISM)
#define DBIERR_UNRELQ1     (ERRBASE_QUERY+DBICODE_UNRELQ1)
#define DBIERR_UNUSEDST    (ERRBASE_QUERY+DBICODE_UNUSEDST)
#define DBIERR_USEINSDE    (ERRBASE_QUERY+DBICODE_USEINSDE)
#define DBIERR_USEOFCHG    (ERRBASE_QUERY+DBICODE_USEOFCHG)
#define DBIERR_VARMUSTF    (ERRBASE_QUERY+DBICODE_VARMUSTF)
#define DBIERR_REGISTER    (ERRBASE_QUERY+DBICODE_REGISTER)
#define DBIERR_LONGEXPR    (ERRBASE_QUERY+DBICODE_LONGEXPR)
#define DBIERR_REFRESH     (ERRBASE_QUERY+DBICODE_REFRESH)
#define DBIERR_CANCEXCEPT  (ERRBASE_QUERY+DBICODE_CANCEXCEPT)
#define DBIERR_DBEXCEPT    (ERRBASE_QUERY+DBICODE_DBEXCEPT)
#define DBIERR_MEMEXCEPT   (ERRBASE_QUERY+DBICODE_MEMEXCEPT)
#define DBIERR_FATALEXCEPT (ERRBASE_QUERY+DBICODE_FATALEXCEPT)
#define DBIERR_QRYNIY      (ERRBASE_QUERY+ DBICODE_QRYNIY)
#define DBIERR_BADFORMAT   (ERRBASE_QUERY+ DBICODE_BADFORMAT)
#define DBIERR_QRYEMPTY    (ERRBASE_QUERY+ DBICODE_QRYEMPTY)
#define DBIERR_NOQRYTOPREP (ERRBASE_QUERY+ DBICODE_NOQRYTOPREP)
#define DBIERR_BUFFTOOSMALL (ERRBASE_QUERY+ DBICODE_BUFFTOOSMALL)
#define DBIERR_QRYNOTPARSE (ERRBASE_QUERY+ DBICODE_QRYNOTPARSE)
#define DBIERR_NOTHANDLE   (ERRBASE_QUERY+ DBICODE_NOTHANDLE)
#define DBIERR_QRYSYNTERR  (ERRBASE_QUERY+ DBICODE_QRYSYNTERR)
#define DBIERR_QXFLDCOUNT  (ERRBASE_QUERY+ DBICODE_QXFLDCOUNT)
#define DBIERR_QXFLDSYMNOTFOUND (ERRBASE_QUERY+ DBICODE_QXFLDSYMNOTFOUND)
#define DBIERR_QXTBLSYMNOTFOUND (ERRBASE_QUERY+ DBICODE_QXTBLSYMNOTFOUND)
#define DBIERR_BLOBTERM    (ERRBASE_QUERY+ DBICODE_BLOBTERM)
#define DBIERR_BLOBERR     (ERRBASE_QUERY+ DBICODE_BLOBERR)
#define DBIERR_RESTARTQRY  (ERRBASE_QUERY+ DBICODE_RESTARTQRY)
#define DBIERR_UNKNOWNANSTYPE (ERRBASE_QUERY+ DBICODE_UNKNOWNANSTYPE)
#define DBIERR_SQLG_MDIST  (ERRBASE_QUERY+ DBICODE_SQLG_MDIST)
#define DBIERR_SQLG_NOARI  (ERRBASE_QUERY+ DBICODE_SQLG_NOARI)
#define DBIERR_SQLG_LIKEN  (ERRBASE_QUERY+ DBICODE_SQLG_LIKEN)
#define DBIERR_SQLG_ALPHO  (ERRBASE_QUERY+ DBICODE_SQLG_ALPHO)
#define DBIERR_SQLG_DATEO  (ERRBASE_QUERY+ DBICODE_SQLG_DATEO)
#define DBIERR_SQLG_RELOP  (ERRBASE_QUERY+ DBICODE_SQLG_RELOP)
#define DBIERR_SQLG_ONLYC  (ERRBASE_QUERY+ DBICODE_SQLG_ONLYC)
#define DBIERR_SQLG_CNTLN  (ERRBASE_QUERY+ DBICODE_SQLG_CNTLN)
#define DBIERR_SQLG_CHINI  (ERRBASE_QUERY+ DBICODE_SQLG_CHINI)
#define DBIERR_SQLG_UNION  (ERRBASE_QUERY+ DBICODE_SQLG_UNION)
#define DBIERR_SQLG_SLFIN  (ERRBASE_QUERY+ DBICODE_SQLG_SLFIN)
#define DBIERR_SQLG_OTJVR  (ERRBASE_QUERY+ DBICODE_SQLG_OTJVR)
#define DBIERR_SQLG_STROW  (ERRBASE_QUERY+ DBICODE_SQLG_STROW)
#define DBIERR_SQLG_QUANT  (ERRBASE_QUERY+ DBICODE_SQLG_QUANT)
#define DBIERR_SQLG_REGSO  (ERRBASE_QUERY+ DBICODE_SQLG_REGSO)
#define DBIERR_SQLG_COUNT  (ERRBASE_QUERY+ DBICODE_SQLG_COUNT)
#define DBIERR_SQLG_AVERA  (ERRBASE_QUERY+ DBICODE_SQLG_AVERA)
#define DBIERR_SQLG_DATEA  (ERRBASE_QUERY+ DBICODE_SQLG_DATEA)
#define DBIERR_SQLG_BADPT  (ERRBASE_QUERY+ DBICODE_SQLG_BADPT)
#define DBIERR_SQLG_RELPA  (ERRBASE_QUERY+ DBICODE_SQLG_RELPA)
#define DBIERR_SQLG_PATRN  (ERRBASE_QUERY+ DBICODE_SQLG_PATRN)
#define DBIERR_SQLG_FNDSU  (ERRBASE_QUERY+ DBICODE_SQLG_FNDSU)
#define DBIERR_SQLG_IFDCS  (ERRBASE_QUERY+ DBICODE_SQLG_IFDCS)
#define DBIERR_SQLG_IDCCO  (ERRBASE_QUERY+ DBICODE_SQLG_IDCCO)
#define DBIERR_SQLG_ONLYI  (ERRBASE_QUERY+ DBICODE_SQLG_ONLYI)
#define DBIERR_SQLG_SQLDIALECT (ERRBASE_QUERY+ DBICODE_SQLG_SQLDIALECT)
#define DBIERR_SQLG_NOQUERY (ERRBASE_QUERY+ DBICODE_SQLG_NOQUERY)
#define DBIERR_BLOBGROUP   (ERRBASE_QUERY+ DBICODE_BLOBGROUP)
#define DBIERR_QRYNOPROP   (ERRBASE_QUERY+DBICODE_QRYNOPROP)
#define DBIERR_ANSTYPNOTSUP   (ERRBASE_QUERY+DBICODE_ANSTYPNOTSUP)
#define DBIERR_ANSALIASNOTSUP (ERRBASE_QUERY+DBICODE_ANSALIASNOTSUP)
#define DBIERR_INSBLOBREQ     (ERRBASE_QUERY+DBICODE_INSBLOBREQ     )                         // 0x9a
#define DBIERR_CHGUNIQUENDXREQ (ERRBASE_QUERY+DBICODE_CHGUNIQUENDXREQ)                         // 0x9b
#define DBIERR_DELUNIQUENDXREQ (ERRBASE_QUERY+DBICODE_DELUNIQUENDXREQ)                         // 0x9c
#define DBIERR_SQLNOFULLUPDATE (ERRBASE_QUERY+DBICODE_SQLNOFULLUPDATE)                         // 0x9d
#define DBIERR_CANTEXECREMOTE  (ERRBASE_QUERY+DBICODE_CANTEXECREMOTE)                          // 0x9e
#define DBIERR_UNEXPECTEDEOC   (ERRBASE_QUERY+DBICODE_UNEXPECTEDEOC)
#define DBIERR_SQLPARAMNOTSET  (ERRBASE_QUERY+DBICODE_SQLPARAMNOTSET)



// END_OF_QUERY_MESSAGES

// ERRCAT_VERSION
// ===============

#define  ERRCODE_INTERFACEVER    1        // Interface mismatch
#define  ERRCODE_INDEXOUTOFDATE  2        // Index is out of date
#define  ERRCODE_OLDVERSION      3        // Older version (see context)
#define  ERRCODE_VALFILEINVALID  4        // Val. file is out of date
#define  ERRCODE_BLOBVERSION     5        // Old Blob file version
#define  ERRCODE_ENGQRYMISMATCH  6        // Query and ODAPI are mismatched

#define  DBIERR_INTERFACEVER     (ERRBASE_VERSION + ERRCODE_INTERFACEVER)
#define  DBIERR_INDEXOUTOFDATE   (ERRBASE_VERSION + ERRCODE_INDEXOUTOFDATE)
#define  DBIERR_OLDVERSION       (ERRBASE_VERSION + ERRCODE_OLDVERSION)
#define  DBIERR_VALFILEINVALID   (ERRBASE_VERSION + ERRCODE_VALFILEINVALID)
#define  DBIERR_BLOBVERSION      (ERRBASE_VERSION + ERRCODE_BLOBVERSION)
#define  DBIERR_ENGQRYMISMATCH   (ERRBASE_VERSION + ERRCODE_ENGQRYMISMATCH)

// ERRCAT_CAPABILITY
// =================

#define  ERRCODE_NOTSUPPORTED    1        // Capability not supported
#define  ERRCODE_NIY             2        // Not Implemented Yet
#define  ERRCODE_TABLESQL        3        // Cannot access SQL replica
#define  ERRCODE_SEARCHCOLREQD   4        // Searchable (Non-blob column) required
#define  ERRCODE_NOMULTCONNECT   5        // Multiple connections not supported

#define  DBIERR_NOTSUPPORTED     (ERRBASE_CAPABILITY + ERRCODE_NOTSUPPORTED)
#define  DBIERR_NIY              (ERRBASE_CAPABILITY + ERRCODE_NIY)
#define  DBIERR_TABLESQL         (ERRBASE_CAPABILITY + ERRCODE_TABLESQL)
#define  DBIERR_SEARCHCOLREQD    (ERRBASE_CAPABILITY + ERRCODE_SEARCHCOLREQD)
#define  DBIERR_NOMULTCONNECT    (ERRBASE_CAPABILITY + ERRCODE_NOMULTCONNECT)

// ERRCAT_CONFIG            
// =============

#define ERRCODE_INVALIDDBSPEC    1
#define ERRCODE_UNKNOWNDBTYPE    2
#define ERRCODE_INVALIDSYSDATA   3
#define ERRCODE_UNKNOWNNETTYPE   4
#define ERRCODE_NOTONTHATNET     5
#define ERRCODE_INVALIDCFGPARAM  6        // Generic invalid config param


#define DBIERR_INVALIDDBSPEC     (ERRBASE_CONFIG + ERRCODE_INVALIDDBSPEC)
#define DBIERR_UNKNOWNDBTYPE     (ERRBASE_CONFIG + ERRCODE_UNKNOWNDBTYPE)
#define DBIERR_INVALIDSYSDATA    (ERRBASE_CONFIG + ERRCODE_INVALIDSYSDATA)
#define DBIERR_UNKNOWNNETTYPE    (ERRBASE_CONFIG + ERRCODE_UNKNOWNNETTYPE)
#define DBIERR_NOTONTHATNET      (ERRBASE_CONFIG + ERRCODE_NOTONTHATNET)
#define DBIERR_INVALIDCFGPARAM   (ERRBASE_CONFIG + ERRCODE_INVALIDCFGPARAM)

// ERRCAT_WARNING  non-fatal warnings:
// warn user of action, or ask for optional behavior
// ==============
#define ERRCODE_OBJIMPLICITLYDROPPED      1
#define ERRCODE_OBJMAYBETRUNCATED         2
#define ERRCODE_OBJIMPLICITLYMODIFIED     3
#define ERRCODE_VALIDATEDATA              4
#define ERRCODE_VALFIELDMODIFIED          5
#define ERRCODE_TABLELEVELCHANGED         6
#define ERRCODE_COPYLINKEDTABLES          7
#define ERRCODE_OTHERSERVERLOADED         8
#define ERRCODE_OBJIMPLICITLYTRUNCATED    9
#define ERRCODE_VCHKMAYNOTBEENFORCED      10
#define ERRCODE_MULTIPLEUNIQRECS          11
#define ERRCODE_FIELDMUSTBETRIMMED        12

#define DBIERR_OBJIMPLICITLYDROPPED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYDROPPED)
#define DBIERR_OBJMAYBETRUNCATED (ERRBASE_WARNING + ERRCODE_OBJMAYBETRUNCATED)
#define DBIERR_OBJIMPLICITLYMODIFIED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYMODIFIED)
#define DBIERR_VALIDATEDATA              \
   ( ERRBASE_WARNING + ERRCODE_VALIDATEDATA)
#define DBIERR_VALFIELDMODIFIED          \
   ( ERRBASE_WARNING + ERRCODE_VALFIELDMODIFIED)
#define DBIERR_TABLELEVELCHANGED         \
   ( ERRBASE_WARNING + ERRCODE_TABLELEVELCHANGED)
#define DBIERR_COPYLINKEDTABLES          \
   ( ERRBASE_WARNING + ERRCODE_COPYLINKEDTABLES)
#define DBIERR_OTHERSERVERLOADED          \
   ( ERRBASE_WARNING + ERRCODE_OTHERSERVERLOADED)
#define DBIERR_OBJIMPLICITLYTRUNCATED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYTRUNCATED)
#define DBIERR_VCHKMAYNOTBEENFORCED       \
   ( ERRBASE_WARNING + ERRCODE_VCHKMAYNOTBEENFORCED )
#define DBIERR_MULTIPLEUNIQRECS    \
   ( ERRBASE_WARNING + ERRCODE_MULTIPLEUNIQRECS )
#define DBIERR_FIELDMUSTBETRIMMED        \
   ( ERRBASE_WARNING + ERRCODE_FIELDMUSTBETRIMMED )


// ERRCAT_OTHER
// ============

#define  ERRCODE_FILEEXISTS      1        // File already exsits
#define  ERRCODE_BLOBMODIFIED    2        // Another user modified Blob
#define  ERRCODE_UNKNOWNSQL      3        // Unknown SQL error
#define  ERRCODE_TABLEEXISTS     4        // Table already exsits
#define  ERRCODE_PDX10TABLE      5        // Paradox 1.0 tables not supported

#define  DBIERR_FILEEXISTS       (ERRBASE_OTHER + ERRCODE_FILEEXISTS)
#define  DBIERR_BLOBMODIFIED     (ERRBASE_OTHER + ERRCODE_BLOBMODIFIED)
#define  DBIERR_UNKNOWNSQL       (ERRBASE_OTHER + ERRCODE_UNKNOWNSQL)
#define  DBIERR_TABLEEXISTS      (ERRBASE_OTHER + ERRCODE_TABLEEXISTS)
#define  DBIERR_PDX10TABLE       (ERRBASE_OTHER + ERRCODE_PDX10TABLE)

//

// ERRCAT_COMPATIBILITY
// =====================

#define  ERRCODE_DIFFSORTORDER    1        // Sortorders not compatible
#define  ERRCODE_DIRINUSEBYOLDVER 2        // Directory in use by old version
#define  ERRCODE_PDX35LDDRIVER    3        // Needs Pdox 3.5 compatible language driver

#define  DBIERR_DIFFSORTORDER     (ERRBASE_COMPATIBILITY + ERRCODE_DIFFSORTORDER)
#define  DBIERR_DIRINUSEBYOLDVER  (ERRBASE_COMPATIBILITY + ERRCODE_DIRINUSEBYOLDVER)
#define  DBIERR_PDX35LDDRIVER     (ERRBASE_COMPATIBILITY + ERRCODE_PDX35LDDRIVER)

// ERRCAT_OPTPARAM
// ===============

#define ERRCODE_REQOPTPARAM     1       // Required optional parameter missing
#define ERRCODE_INVALIDOPTPARAM 2       // Optional param out-of-range or bad


#define DBIERR_REQOPTPARAM      (ERRBASE_OPTPARAM + ERRCODE_REQOPTPARAM)
#define DBIERR_INVALIDOPTPARAM  (ERRBASE_OPTPARAM + ERRCODE_INVALIDOPTPARAM)

#endif  // NODBIERRS




#ifndef NODBIPROTOTYPES
//============================================================================
//                          DBI prototypes
//============================================================================

//                          Environmental
//----------------------------------------------------------------------------


DBIResult DBIFN DbiInitFn (           // Initialize the Engine
      UINT16          iVer,             // Interface Version
      pDBIEnv         pEnv              // Environment Structure/NULL
   );

    // DO NOT CALL THIS DIRECTLY. Use the 'DbiInit' define below.

    // Initializes the engine environment. Default settings can be overwritten
    // by supplying the appropriate settings. Defaults are read from the system
    // configuration file. pEnv can be NULLP to get the system defautls.

#define DbiInit(pEnv)     DbiInitFn(DBIINTFVER, pEnv)


DBIResult DBIFN DbiExit(VOID);        // Exit engine


//============================================================================
//                         System Level Info
//============================================================================

DBIResult DBIFN DbiGetSysVersion (    // Get system version info
      pSYSVersion    psysVersion
   );

DBIResult DBIFN DbiGetSysConfig (     // System configuration
      pSYSConfig     psysConfig
   );

DBIResult DBIFN DbiGetClientInfo (    // Get Client info
      pCLIENTInfo    pclientInfo
   );

DBIResult DBIFN DbiGetSysInfo (       // Get system status/info
      pSYSInfo       psysInfo
   );

DBIResult DBIFN DbiLoadDriver (       // Load a given driver
      pCHAR          pszDriverType      // Driver name
   );

//============================================================================
//                            Sessions
//============================================================================

DBIResult DBIFN DbiStartSession (     // Start a new session
      pCHAR          pszName,           // Name (Optional)
      phDBISes       phSes  ,           // Session
      pCHAR          pNetDir		       // Netfile directory for session (opt)
   );

DBIResult DBIFN DbiGetCurrSession (   // Get the current session
      phDBISes       phSes              // Session
   );

DBIResult DBIFN DbiSetCurrSession (   // Set the current session
      hDBISes        hSes               // Session/NULL
   );

DBIResult DBIFN DbiCloseSession (     // Close the current session
      hDBISes        hSes               // Session
   );

DBIResult DBIFN DbiGetSesInfo (       // Get current session info
      pSESInfo       psesInfo
   );

DBIResult DBIFN DbiSetPrivateDir (    // Set Private Directory for session
      pCHAR          pszDir             // Directory name/NULL
   );


//============================================================================
//                     Datababase, Schema and File inquiry
//============================================================================



DBIResult DBIFN DbiOpenDatabase (     // Open a database
      pCHAR          pszDbName,         // Database name
      pCHAR          pszDbType,         // Database type (NULL: Universal)
      DBIOpenMode    eOpenMode,         // Open type
      DBIShareMode   eShareMode,        // Share type
      pCHAR          pszPassword,       // Password
      UINT16         iOptFlds,          // Number of optional Params
      pFLDDesc       pOptFldDesc,       // Optional Field Descriptors
      pBYTE          pOptParams,        // Optional Params
      phDBIDb        phDb               // Returnd database handle
    );

DBIResult DBIFN DbiSetDirectory (     // Set the current directory
      hDBIDb         hDb,               // Universal database handle
      pCHAR          pszDir             // Directory/NULL
    );

DBIResult DBIFN DbiGetDirectory (     // Get the current/default directory
      hDBIDb         hDb,               // Universal database handle
      BOOL           bDefault,          // True for default
      pCHAR          pszDir             // Returned directory 
    );

DBIResult DBIFN DbiOpenTableList (    // Open a cursor on "Tables"
      hDBIDb         hDb,               // Database handle
      BOOL           bExtended,         // True for extended info
      BOOL           bSystem,           // True to include system tables
      pCHAR          pszWild,           // Wild card name
      phDBICur       phCur              // Returned cursor
    );

DBIResult DBIFN DbiOpenFileList (     // Open a cursor on "Files"
      hDBIDb         hDb,               // Universal database handle
      pCHAR          pszWild,           // Wild card name
      phDBICur       phCur              // Returned cursor
    );

DBIResult DBIFN DbiOpenIndexList (    // Return "Indexes" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur);            // Returned cursor on "Indexes"
                                      
DBIResult DBIFN DbiOpenFieldList (    // Return "Fields" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      BOOL           bPhyTypes,         // True, for physical types
      phDBICur       phCur);            // Returned cursor on "Fields"

DBIResult DBIFN DbiOpenVchkList (     // Return "Checks" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver Type
      phDBICur       phChkCur);         // Returned cursor on "Checks"

DBIResult DBIFN DbiOpenRintList (     // Return Integrity checks
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phChkCur);         // Returned cursor on "Ref Int".

DBIResult DBIFN DbiOpenSecurityList ( // Return security descriptions
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phSecCur           // Returned cursor on sec list
   );

DBIResult DBIFN DbiOpenFamilyList (   // Return family members
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phFmlCur           // Returned cursor on "Family"
   );

DBIResult DBIFN DbiCloseDatabase (     // Close a database
      phDBIDb        phDb                // Pointer to database handle
    );

//============================================================================
//                                  Capabilities
//============================================================================

DBIResult DBIFN DbiOpenDriverList (   // Get a list of driver names
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetDriverDesc (    // Get description for a given type
      pCHAR          pszDriverType,     // Symbolic name for driver type
      pDRVType       pdrvType           // Driver type description
   );

DBIResult DBIFN DbiOpenDatabaseList ( // Get a list of registered databases
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetDatabaseDesc (  // Get a description of a logical db
      pCHAR          pszName,           // Name of logical database
      pDBDesc        pdbDesc            // Database description
   );

DBIResult DBIFN DbiOpenTableTypesList ( // Get a list of table types
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetTableTypeDesc ( // Get Table capabilities
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTableType,      // Table type
      pTBLType       ptblType           // Table Capabilities
   );


DBIResult DBIFN DbiOpenFieldTypesList ( // Get a list of field types
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTblType,        // Table type (Optional)
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetFieldTypeDesc ( // Get list of field types
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTableType,      // Table type
      pCHAR          pszFieldType,      // Field type  (Physical only)
      pFLDType       pfldType           // Field type description
   );

DBIResult DBIFN DbiOpenIndexTypesList ( // Get list of index types
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetIndexTypeDesc ( // Get description of given idx type
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszIndexType,      // Index type
      pIDXType       pidxType           // Index description
   );

DBIResult DBIFN DbiOpenLdList (       // Get a list of Lang Drivers
      phDBICur       phCur              // Returned cursor
   );

//===========================================================================
//                      Table Open, Properties & Structure
//===========================================================================

DBIResult DBIFN DbiOpenTable (        // Open a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name or file name
      pCHAR          pszDriverType,     // Driver type                 /NULL
      pCHAR          pszIndexName,      // Index to be used for access /NULL
      pCHAR          pszIndexTagName,   // Index tag name              /NULL
      UINT16         iIndexId,          // Index number                /0
      DBIOpenMode    eOpenMode,         // Read or RW
      DBIShareMode   eShareMode,        // Excl or Share
      XLTMode        exltMode,          // Xlate mode
      BOOL           bUniDirectional,   // Uni or Bi directional 
      pBYTE          pOptParams,        // Optional params /NULL
      phDBICur       phCursor           // Returns Cursor handle
    );


DBIResult DBIFN DbiGetCursorProps (   // Get Cursor properties
      hDBICur        hCursor,           // Cursor handle
      pCURProps      pcurProps          // Cursor properties
    );

DBIResult DBIFN DbiGetObjFromName (   // Get object from name
      DBIOBJType     eObjType   ,       // Object handle
      pCHAR          pszObjName ,       // Name of object /NULL
      phDBIObj       phObj              // Returned object handle
   );

DBIResult DBIFN DbiGetObjFromObj (    // Get associated object
      hDBIObj        hObj       ,       // Object handle
      DBIOBJType     eObjType   ,       // Type of associated object
      phDBIObj       phObj              // Returns object of eObjType
   );

DBIResult DBIFN DbiGetProp (          // Get property
      hDBIObj        hObj   ,           // Object handle
      UINT32         iProp  ,           // Property to retrieve
      pVOID          pPropValue,        // == NULL, validate iProp for getting
      UINT16         iMaxLen,           // Length of buffer pPropValue
      pUINT16        piLen              // Returns required length
   );

DBIResult DBIFN DbiSetProp (          // Set property
      hDBIObj        hObj   ,           // Object handle
      UINT32         iProp  ,           // Property to set
      UINT32         iPropValue         // Property value
   );

DBIResult DBIFN DbiGetFieldDescs (    // Get field descriptions
      hDBICur        hCursor,           // Cursor handle
      pFLDDesc       pfldDesc           // Array of field descriptors
    );

DBIResult DBIFN DbiGetCursorForTable (// Find cursor for a given table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type / NULL 
      phDBICur       phCursor           // Returned cursor
   );

DBIResult DBIFN DbiCloneCursor (      // Return a duplicate cursor
      hDBICur        hCurSrc,           // Source cursor
      BOOL           bReadOnly,         // If TRUE, read only mode
      BOOL           bUniDirectional,   // If TRUE, Uni directional
      phDBICur       phCurNew           // Destination cursor address
    );

DBIResult DBIFN DbiCloseCursor (      // Closes cursor
      phDBICur       phCursor           // Pntr to Cursor handle
    );


//============================================================================
//                      Index Manipulation & Usage
//============================================================================

DBIResult DBIFN DbiOpenIndex (        // Open an index
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index Name
      UINT16         iIndexId           // Index number (if applicable)
   );

DBIResult DBIFN DbiCloseIndex (       // Close an index
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index Name
      UINT16         iIndexId           // Index number
   );

DBIResult DBIFN DbiSwitchToIndex (    // Change index order of access
      phDBICur       phCursor,          // Cursor handle (In/Out)
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszTagName,        // Tag name (if applicable)
      UINT16         iIndexId,          // Index number 
      BOOL           bCurrRec           // Position at current rec
    );

DBIResult DBIFN DbiGetIndexDesc (     // Get index properties
      hDBICur        hCursor,           // Cursor handle
      UINT16         iIndexSeqNo,       // Index number
      pIDXDesc       pidxDesc           // Returned index description
   );

DBIResult DBIFN DbiGetIndexDescs (    // Get index properties
      hDBICur        hCursor,           // Cursor handle
      pIDXDesc       pidxDesc           // Returned index descriptors
   );

DBIResult DBIFN DbiGetIndexForField ( // Get index desc for given field
      hDBICur        hCursor,           // Cursor handle
      UINT16         iFld,              // Field Number (1..N)
      BOOL           bProdTagOnly,      // If TRUE, only xBASE prod tags will be returned
      pIDXDesc       pidxDesc           // (Optional)
   );

DBIResult DBIFN DbiGetIndexSeqNo(     // Get index sequence number
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszTagName,        // Tag name (if applicable)
      UINT16         iIndexId,          // Index number
      pUINT16        piIndexSeqNo       // Index number
   );

DBIResult DBIFN DbiSetToKey  (        // Set key condition
      hDBICur           hCursor,        // Cursor handle
      DBISearchCond     eSearchCond,    // Search condition (default is =)
      BOOL              bDirectKey,     // Key is supplied directly
      UINT16            iFields,        // No of full fields to match
      UINT16            iLen,           // Partial key len of last field
      pBYTE             pBuff           // Either Record buffer or Key itself
    );

DBIResult DBIFN DbiExtractKey (       // Get the key value of current record
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pRecBuf,        // Record buffer (optional)
      pBYTE             pKeyBuf         // Returned. Key bytes.
   );

DBIResult DBIFN DbiSetRange (         // Set cursor to a range
      hDBICur           hCursor,        // Cursor
      BOOL              bKeyItself,     // Whether Key or Record buffer
      UINT16            iFields1,       // Key fields to be mathced in full
      UINT16            iLen1,          // Key length to compare
      pBYTE             pKey1,          // Top/Left key in Range
      BOOL              bKey1Incl,      // If Inclusive of Key1
      UINT16            iFields2,       // Key fields to be mathced in full
      UINT16            iLen2,          // Key length to compare
      pBYTE             pKey2,          // Bottom/Right key in Range
      BOOL              bKey2Incl       // If Inclusive of Key2
   );

DBIResult DBIFN DbiResetRange (       // Reset range
      hDBICur        hCursor            // cursor handle
   );

DBIResult DBIFN DbiCompareKeys (      // Compare two keys
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pKey1,          // Key buffer 1 to compare
      pBYTE             pKey2,          // Key buffer 2 (Or NULL)
      UINT16            iFields,        // Fields to compare in full
      UINT16            iLen,           // Partial key to compare
      pINT16            piResult        // Compare result
   );

DBIResult DBIFN DbiGetRecordForKey (  // Find a record matching key
      hDBICur           hCursor,        // Cursor handle
      BOOL              bDirectKey,     // Key is supplied directly
      UINT16            iFields,        // No of full fields to match
      UINT16            iLen,           // Partial key len of last field
      pBYTE             pKey,           // Either Record buffer or Key itself
      pBYTE             pRecBuff        // (Optional) Record buffer
   );

//=============================================================================
//                          Validity check and referential integrity
//=============================================================================

DBIResult DBIFN DbiGetVchkDesc (      // Get valcheck descriptor
      hDBICur        hCursor,           // Cursor handle
      UINT16         iValSeqNo,         // Valcheck sequence number
      pVCHKDesc      pvalDesc           // Returned valcheck description
   );

DBIResult DBIFN DbiGetRintDesc (      // Get referential integrity descriptor
      hDBICur        hCursor,           // Cursor handle
      UINT16         iRintSeqNo,        // Rint sequence number
      pRINTDesc      printDesc          // Returned rint description
   );

//=============================================================================
//                              Cursor Maintenance
//=============================================================================


DBIResult DBIFN DbiSetToBegin (       // Reset cursor to beginning
      hDBICur        hCursor            // Cursor handle
   );

DBIResult DBIFN DbiSetToEnd (         // Reset cursor to ending
      hDBICur        hCursor            // Cursor handle
    );

DBIResult DBIFN DbiSetToCursor (      // Set cursor to another cursor position
      hDBICur        hDest,             // Destination cursor
      hDBICur        hSrc               // Source cursor
   );

DBIResult DBIFN DbiGetBookMark (      // Get a book-mark
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark          // Pointer to Book-Mark
   );

DBIResult DBIFN DbiSetToBookMark (    // Position to a Book-Mark
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark          // Pointer to Book-Mark
   );

DBIResult DBIFN DbiCompareBookMarks ( // Compare two Book-marks
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark1,        // Book mark 1
      pBYTE          pBookMark2,        // Book mark 2
      pCMPBkMkRslt   pCmpBkmkResult     // Compare result
   );

//============================================================================
//                      Data Access: Logical Record Level
//============================================================================


DBIResult DBIFN DbiGetNextRecord (    // Find/Get the next record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetPriorRecord (   // Find/Get the prior record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer (client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetRecord (        // Gets the current record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetRelativeRecord (// Find/Get a record by record number
      hDBICur           hCursor,        // Cursor handle
      INT32             iPosOffset,     // offset from current position
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiInitRecord (       // Initialize record area
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuff           // Record buffer
    );

DBIResult DBIFN DbiInsertRecord (     // Inserts a new record
      hDBICur        hCursor,           // Cursor handle
      DBILockType    eLock,             // Optional lock on this rec
      pBYTE          pRecBuff           // New Record (client)
    );

DBIResult DBIFN DbiModifyRecord (     // Updates the current record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Modified record
      BOOL           bFreeLock          // Free record lock
    );

DBIResult DBIFN DbiDeleteRecord (     // Deletes the current record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf            // Copy of deleted record
    );

DBIResult DBIFN DbiReadBlock (        // Read a block of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecords,         // Number of records to read
      pBYTE          pBuf               // Buffer
   );

DBIResult DBIFN DbiWriteBlock (       // Write a block of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecords,         // Number of records to write/written
      pBYTE          pBuf               // Buffer
   );

DBIResult DBIFN DbiAppendRecord (     // Inserts a new record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuff           // New Record (client)
    );

DBIResult DBIFN DbiUndeleteRecord (   // Undeletes the current record
      hDBICur        hCursor            // Cursor handle
    );

DBIResult DBIFN DbiGetSeqNo (         // Get logical record number
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piSeqNo            // Pointer to sequence number
   );

DBIResult DBIFN DbiSetToSeqNo (       // Position to a logical record number
      hDBICur        hCursor,           // Cursor handle
      UINT32         iSeqNo             // Sequence number
   );

DBIResult DBIFN DbiGetRecordCount (   // Get the current number of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecCount         // Number of records
   );


DBIResult DBIFN DbiSetToRecordNo (    // Position to Physical Rec#
      hDBICur        hCursor,           // Cursor handle
      UINT32         iRecNo             // Physical record number
   );

DBIResult DBIFN DbiSaveChanges (      // Flush all buffered changes
      hDBICur        hCursor            // Cursor handle
   );
DBIResult DBIFN DbiForceReread (      // Force Reread of buffers from Disk
      hDBICur        hCursor            // Cursor
   );

DBIResult DBIFN DbiCheckRefresh (VOID); // Check refresh for session

DBIResult DBIFN DbiMakePermanent (    // Make temporary table permanent
      hDBICur     hCursor,              // Cursor handle
      pCHAR       pszName,              // Rename temporary table
      BOOL        bOverWrite            // Overwrite existing file
   );


//============================================================================
//                            Field Level Access
//============================================================================

DBIResult DBIFN DbiGetField(          // Get Field value
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pRecBuff,          // Record buffer
      pBYTE          pDest,             // Destination field buffer
      pBOOL          pbBlank            // Returned : is field blank
   );

DBIResult DBIFN DbiPutField(          // Put a value in the record buffer
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pRecBuff,          // Record buffer
      pBYTE          pSrc               // Source field buffer
   );

DBIResult DBIFN DbiVerifyField(       // Verifies the field value
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pSrc,              // Field Value
      pBOOL          pbBlank            // Field is Blank (Returned)
   );

DBIResult DBIFN DbiOpenBlob (         // Open a blob for access
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number (1..n)
      DBIOpenMode    eOpenMode          // Open for Read or RW
    );

DBIResult DBIFN DbiGetBlobSize (      // Gets the size of a blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      pUINT32        piSize             // Blob size in bytes
    );

DBIResult DBIFN DbiGetBlob (          // Read bytes from blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iOffSet,           // Starting position   
      UINT32         iLen,              // No of bytes to be read   
      pBYTE          pDest,             // Destination
      pUINT32        piRead             // Actual no of bytes read
    );

DBIResult DBIFN DbiPutBlob (          // Write bytes to blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iOffSet,           // Starting position   
      UINT32         iLen,              // No of bytes to put   
      pBYTE          pSrc               // pntr to Source
    );

DBIResult DBIFN DbiTruncateBlob (     // Reduces the blob size
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iLen               // New blob length
    );

DBIResult DBIFN DbiFreeBlob (         // Closes the blob
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pRecBuf,        // Record Buffer
      UINT16            iField          // Field number of blob (0..n)
    );

DBIResult DBIFN DbiGetBlobHeading (   // Get Blob Heading
      hDBICur           hCursor,        // Cursor handle
      UINT16            iField,         // Field number of blob (1..n)
      pBYTE             pRecBuf,        // Record buffer of owner record
      pBYTE             pDest           // Destination buffer
   );

DBIResult DBIFN DbiSetFieldMap(       // Set a fieldmap
      hDBICur        hCur,              // Cursor handle
      UINT16         iFields,           // Number of fields
      pFLDDesc       pFldDesc           // Array of field descriptions
   );


//=============================================================================
//                                TRANSACTIONS
//=============================================================================

DBIResult DBIFN DbiBeginTran (        // Begin a transaction
   hDBIDb         hDb,                  // Database handle
   eXILType       eXIL,                 // Transaction isolation level
   phDBIXact      phXact                // Returned Xact handle
);

DBIResult DBIFN DbiEndTran (          // End a transaction
   hDBIDb         hDb,                  // Database handle
   hDBIXact       hXact,                // Xact handle
   eXEnd          eEnd                  // Xact end type
);

DBIResult DBIFN DbiGetTranInfo (      // Get transaction info
   hDBIDb         hDb,                  // Database handle
   hDBIXact       hXact,                // Xact handle
   pXInfo         pxInfo                // Xact info
);

//=============================================================================
//                                  LOCKING
//=============================================================================

DBIResult DBIFN DbiAcqTableLock (     // Lock a table
      hDBICur        hCursor,           // Cursor handle
      DBILockType    eLockType          // Lock type
    );

DBIResult DBIFN DbiAcqPersistTableLock (// Get a persistent lock
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type / NULL
   );

DBIResult DBIFN DbiRelPersistTableLock (// Releases a persistent lock
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type / NULL
   );

DBIResult DBIFN DbiRelTableLock (     // Unlocks Table level locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAll,              // True for all table level locks
      DBILockType    eLockType          // Specific lock type
    );

DBIResult DBIFN DbiRelRecordLock (    // Releases record level locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAll               // True for all. Default Current.
    );

DBIResult DBIFN DbiIsRecordLocked (   // Check if current record is locked
      hDBICur        hCursor,           // Cursor handle
      pBOOL          pbLocked           // Rec lock status
   );

DBIResult DBIFN DbiIsTableLocked (    // Verify if Table is locked
      hDBICur        hCursor,           // Cursor handle
      DBILockType    epdxLock,          // Lock type to verify
      pUINT16        piLocks            // Nbr of locks of the given type
   );

DBIResult DBIFN DbiIsTableShared (    // Verify if this is a shared table
      hDBICur        hCursor,           // Cursor handle
      pBOOL          pbShared           // Shared status
   );

DBIResult DBIFN DbiOpenLockList (     // Get a list of locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAllUsers,         // True, for all Users locks
      BOOL           bAllLockTypes,     // True, for all lock types
      phDBICur       phLocks            // Returned cursor on Lock list
   );

DBIResult DBIFN DbiOpenUserList (     // Get a list of users loggedin
      phDBICur       phUsers            // Returned cursor on user list
   );

DBIResult DBIFN DbiSetLockRetry (     // Set Lock wait time
      INT16          iWait              // Time in seconds
   );

//============================================================================
//                              Batch Operations
//============================================================================


DBIResult DBIFN DbiBatchMove (        // Copy records to destination table
   pBATTblDesc    pSrcTblDesc,          // Source table identification,
   hDBICur        hSrcCur,              //  OR source cursor  ( one must be NULL )
   pBATTblDesc    pDstTblDesc ,         // Destination table identification,
   hDBICur        hDstCur,              //  OR destination cursor ( one must be NULL )
   eBATMode       ebatMode,             // Batch mode 
   UINT16         iFldCount,            // Size of field maps
   pUINT16        pSrcFldMap,           // Array of source field numbers
   pCHAR          pszIndexName,         // If update mode, used to match records
   pCHAR          pszIndexTagName,      // Index tag name
   UINT16         iIndexId,             // Index  id
   pCHAR          pszKeyviolName,       // Keyviol table name  (optional)
   pCHAR          pszProblemsName,      // Problems table name (optional)
   pCHAR          pszChangedName,       // Changed table name (optional)
   pUINT32        plProbRecs,           // Number of records to read from source
   pUINT32        plKeyvRecs,           // Number of records to read from source
   pUINT32        plChangedRecs,        // Number of records to read from source
   BOOL           bAbortOnFirstProb,    // If TRUE, abort on first problem rec
   BOOL           bAbortOnFirstKeyviol, // If TRUE, abort on first keyviol rec
   pUINT32        plRecsToMove,         // Number of records to read from source
		                     		       // Returns number actually read
	BOOL 				bTransliterate		    // If TRUE, transliterate character data
												    // in fields between src&dst char sets
);

DBIResult DBIFN DbiCopyTable (        // Copy one table to another
      hDBIDb         hDb,               // Database handle
      BOOL           bOverWrite,        // True, to overwrite existing file
      pCHAR          pszSrcTableName,   // Source table name
      pCHAR          pszSrcDriverType,  // Source driver type
      pCHAR          pszDestTableName   // Destination table name
   );

DBIResult DBIFN DbiEmptyTable (       // Deletes all records
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type /NULL
    );

DBIResult DBIFN DbiPackTable (        // Pack a table
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      BOOL           bRegenIdxs         // Regenerate indexes
    );

DBIResult DBIFN DbiRegenIndex (       // Regenerate an index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszIndexTagName,   // Index tagname (xbase MDX)
      UINT16         iIndexId           // Index number
    );

DBIResult DBIFN DbiRegenIndexes (     // Regenerate all indexes
      hDBICur         hCursor           // Cursor
   );

DBIResult DBIFN DbiSortTable (        // Sort table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name of source
      pCHAR          pszDriverType,     // Driver type /NULL
      hDBICur        hSrcCur,           // OR cursor of table to sort
      pCHAR          pszSortedName,     // Destination table (NULL if sort to self)
      phDBICur       phSortedCur,       // If non-null, return cursor on destination
      hDBICur        hDstCur,           // OR cursor of destination 
      UINT16         iSortFields,       // Number of sort fields
      pUINT16        piFieldNum,        // Array of field numbers
      pBOOL          pbCaseInsensitive, // Which fields should sort c-i (Opt)
      pSORTOrder     pSortOrder,        // Array of Sort orders (Opt)
      pfSORTCompFn   *ppfSortFn,        // Array of compare fn pntrs (Opt)
      BOOL           bRemoveDups,       // TRUE : Remove duplicates
      hDBICur        hDuplicatesCur,    // Cursor to duplicates table (Opt)
      pUINT32        plRecsSort         // In/out param. - sort this number 
                                        // of records from current position, return 
 					                         // number actually output to dest.  (Opt)
);


//============================================================================
//                           Create & Restructure
//============================================================================

DBIResult DBIFN DbiCreateTable (      // Create a new table
      hDBIDb         hDb,               // Database handle
      BOOL           bOverWrite,        // True, to overwrite existing file.
      pCRTblDesc     pcrTblDsc          // Table description
   );

DBIResult DBIFN DbiCreateInMemTable ( // Create a temporary table (Logical)
      hDBIDb         hDb,               // Database handle
      pCHAR          pszName,           // Logical Name
      UINT16         iFields,           // No of fields
      pFLDDesc       pfldDesc,          // Array of field descriptors
      phDBICur       phCursor           // Returned cursor handle
   );

DBIResult DBIFN DbiCreateTempTable (  // Create temporary table (Physical)
      hDBIDb         hDb,               // Database handle
      pCRTblDesc     pcrTblDsc,         // Table description
      phDBICur       phCursor           // Returned cursor on table
   );

DBIResult DBIFN DbiDoRestructure (    // Restructure a table
      hDBIDb         hDb,               // Database handle
      UINT16         iTblDescCount,     // Number of table descriptors (1)
      pCRTblDesc     pTblDesc,          // Array of table descs
      pCHAR          pszSaveAs,         // Restructure to this table
      pCHAR          pszKeyviolName,    // Keyviol table name  (optional)
      pCHAR          pszProblemsName,   // Problems table name (optional)
      BOOL           bAnalyzeOnly       // Analyze restructure
   );

DBIResult DBIFN DbiRenameTable (      // Rename table & family
      hDBIDb         hDb,               // Database handle
      pCHAR          pszOldName,        // Old name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszNewName         // New name
   );

DBIResult DBIFN DbiDeleteTable (      // Delete a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Name including any path
      pCHAR          pszDriverType      // Driver type /NULL
   );

DBIResult DBIFN DbiAddIndex (         // Add a new index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name including any path
      pCHAR          pszDriverType,     // Driver type /NULL
      pIDXDesc       pIdxDesc    ,      // Description of the index
		pCHAR          pszKeyviolName	    // Keyviol table name (optional)
   );

DBIResult DBIFN DbiDeleteIndex (      // Delete index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name 
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszIndexTagName,   // Index tagname (xbase MDX)
      UINT16         iIndexId           // Index number
   );

//===========================================================================
//                            Error Info
//===========================================================================

DBIResult DBIFN DbiGetErrorEntry (    // Get error entry
      UINT16         uEntry,            // Error stack entry
      pUINT32        pulNativeError,    // Returned. Native error code, if any
      pCHAR          pszError           // Returned. Error string, if any
   );

DBIResult DBIFN DbiGetErrorInfo (     // Return info on last error
      BOOL           bFull,             // If Full details
      pDBIErrInfo    pErrInfo           // Error Info
   );

DBIResult DBIFN DbiGetErrorString (   // Get message for error code
      DBIResult      rslt,              // Engine error code
      pCHAR          pszError           // Error string for the error
   );

DBIResult DBIFN DbiGetErrorContext (  // Get specific Context if available
      INT16          eContext,          // Context type
      pCHAR          pszContext         // Context string (MAXMSGLEN +1)
   );

//============================================================================
//                              Dbi Services
//============================================================================

DBIResult DBIFN DbiDateEncode (       // Encode Date components into Date
      UINT16         iMon,              // Month    (1..12)
      UINT16         iDay,              // Day      (1..31)
      INT16          iYear,             // Year     (0..2**16-1)
      pDATE          pdateD             // Encoded date
   );

DBIResult DBIFN DbiDateDecode (       // Decode Date into components
      DATE           dateD,             // Encoded Date
      pUINT16        piMon,             // Month
      pUINT16        piDay,             // Day
      pINT16         piYear             // Year
   );

DBIResult DBIFN DbiTimeEncode (       // Encode Time components into TIME
      UINT16         iHour,             // Hours (0..23)
      UINT16         iMin,              // Minutes (0..59)
      UINT16         iMilSec,           // Milli Seconds (0..59999)
      pTIME          ptimeT             // Encoded Time
   );

DBIResult DBIFN DbiTimeDecode (       // Decode TIME into components
      TIME           timeT,             // Encoded Time
      pUINT16        piHour,            // Hours (0..23)
      pUINT16        piMin,             // Minutes (0..59)
      pUINT16        piMilSec           // Milli Seconds (0..59999)
   );

DBIResult DBIFN DbiTimeStampEncode (  // Encode Date & TIme into Date+Time
      DATE           dateD,             // Encoded Date
      TIME           timeT,             // Encoded Time
      pTIMESTAMP     ptsTS              // Encoded Date+Time
   );

DBIResult DBIFN DbiTimeStampDecode (  // Decode Date & Time from Date+Time
      TIMESTAMP      tsTS,              // Encoded Date+Time
      pDATE          pdateD,            // Encoded Date
      pTIME          ptimeT             // Encoded Time
   );


DBIResult DBIFN DbiBcdFromFloat(      // Converts FLOAT number into FMTBcd format
       pFLOAT      piVal   ,            // Float to convert
       UINT16      iPrecision,          // Precision of BCD
       UINT16      iPlaces ,            // Number of decimals
       pFMTBcd     pBcd                 // returns Bcd number (length = iPrecision +2)
    );

DBIResult DBIFN DbiBcdToFloat(        // Converts FMTBcd number to FLOAT
      pFMTBcd      pBcd,                // Bcd number to convert
      pFLOAT       piVal                // Returns converted float
   );

//===========================================================================
//                           CallBacks
//===========================================================================

DBIResult DBIFN DbiRegisterCallBack ( // Register a call back fn
      hDBICur        hCursor,           // Cursor (Optional)
      CBType         ecbType,           // Type of call back
      UINT32         iClientData,       // Pass-thru client data
      UINT16         iCbBufLen,         // Callback buffer len
      pVOID          pCbBuf,            // Pointer to callback buffer
      pfDBICallBack  pfCb               // Call back fn being registered
   );

DBIResult DBIFN DbiGetCallBack (      // Register a call back fn
      hDBICur        hCursor,           // Cursor (Optional)
      CBType         ecbType,           // Type of call back
      pUINT32        piClientData,      // Pass-thru client data
      pUINT16        piCbBufLen,        // Callback buffer len
      ppVOID         ppCbBuf,           // Pointer to callback buffer
      ppfDBICallBack ppfCb              // Call back fn being registered
   );

//============================================================================
//                       	 Date, time formats
//============================================================================


DBIResult DBIFN DbiGetDateFormat (    // Get current date format
      pFMTDate       pfmtDate
   );

DBIResult DBIFN DbiSetDateFormat (    // Set current date format
      pFMTDate       pfmtDate
   );

DBIResult DBIFN DbiGetTimeFormat (    // Get current time format
      pFMTTime       pfmtTime
   );

DBIResult DBIFN DbiSetTimeFormat (    // Set current time format
      pFMTTime       pfmtTime
   );

DBIResult DBIFN DbiGetNumberFormat (  // Get current number format
      pFMTNumber     pfmtNumber
   );

DBIResult DBIFN DbiSetNumberFormat (  // Set current number format
      pFMTNumber     pfmtNumber
   );

//============================================================================
//                      Conversions      
//============================================================================

DBIResult DBIFN DbiNativeToAnsi(      // Convert from native to Ansi
      pVOID 	pLdObj  ,                // Language driver
      pCHAR    pAnsiStr,                // Destination buffer (opt)
      pCHAR 	pNativeStr ,             // Source buffer
      UINT16 	iLen    ,                // Length of buffer (opt)
      pBOOL 	pbDataLoss               // Returns TRUE if conversion will loose data (opt)
   );

DBIResult DBIFN DbiAnsiToNative(      // Convert from Ansi to native
      pVOID	pLdObj  ,                   // Language driver
      pCHAR 	pNativeStr ,             // Destination buffer (opt)
      pCHAR 	pAnsiStr,                // Source buffer
      UINT16	iLen    ,                // Length of buffer (opt)
      pBOOL 	pbDataLoss               // Returns TRUE if conversion will loose data (opt)
   );

//============================================================================
//                            Filters
//============================================================================

DBIResult DBIFN DbiAddFilter(         // Add a filter to the cursor
      hDBICur           hCursor,        // Cursor handle
      UINT32            iClientData,    // Client supplied data      (opt)
      UINT16            iPriority,      // 1..N with 1 being highest (opt)
      BOOL              bCanAbort,      // TRUE if pfFiltercan return ABORT (opt)
      pCANExpr          pcanExpr,       // Expression tree        (opt)
      pfGENFilter       pfFilter,       // ptr to filter function (opt)
      phDBIFilter       phFilter        // Returns filter handle
   );

DBIResult DBIFN DbiDropFilter(        // Drop a filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

DBIResult DBIFN DbiActivateFilter(    // Activate a Filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

DBIResult DBIFN DbiDeactivateFilter(  // Deactivate Filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

DBIResult DBIFN  DbiGetFilterInfo(    // Get filter information
      hDBICur           hCur    ,       // Cursor handle
      hDBIFilter        hFilter ,       // Filter handle          /NULL
      UINT16            iFilterId,      // Filter id              /0
      UINT16            iFilterSeqNo,   // Filter sequence number /0
      pFILTERInfo       pFilterinfo     // Returns filter info
  );

//============================================================================
//                            Linked Cursors
//============================================================================

DBIResult DBIFN DbiBeginLinkMode(     // Convert cursor to a link cursor
      phDBICur          phCursor        // In/Out : returns new cursor
    );

DBIResult DBIFN DbiEndLinkMode (      // Convert cursor back to normal cursor
      phDBICur       phCursor           // In/Out : returns original cursor
   );

DBIResult DBIFN DbiLinkDetail(        // Link detail to master
      hDBICur        hMstrCursor,       // Master cursor
      hDBICur        hDetlCursor,       // Detail cursor
      UINT16         iLnkFields,        // Number of link fields
      pUINT16        piMstrFields,      // Array of fields in master
      pUINT16        piDetlFields       // Array of fields in detail
    );

DBIResult DBIFN DbiLinkDetailToExp(   // Link detail to a master using exp
        hDBICur         hCursorMstr,    // Master cursor
        hDBICur         hCursorDetl,    // Detail cursor
        UINT16          iKeyLen,        // Key length to match
        pCHAR           pszMstrExp      // Expression string
    );

DBIResult DBIFN DbiUnlinkDetail (     // Unlink detail from master
      hDBICur        hDetlCursor        // Detail cursor to unlink
    );

DBIResult DBIFN DbiGetLinkStatus(     // Query linkage info for table
        hDBICur         hCursor,        // Cursor handle
        phDBICur        phCursorMstr,   // Returns master cursor, if any   (opt)
        phDBICur        phCursorDet,    // Returns first detail cursor, if any (opt)
        phDBICur        phCursorSib     // Returns next sibling detail cursor, if any (opt)
    );


//===========================================================================
//                            Translation
//===========================================================================


DBIResult DBIFN DbiTranslateRecordStructure ( // Translate a record
      pCHAR          pszSrcDriverType,  // Source driver type
      UINT16         iFlds,             // Number of fields
      pFLDDesc       pfldsSrc,          // Array of source fields: logical or physical types
      pCHAR          pszDstDriverType,  // Destination driver type
      pCHAR          pszLangDriver,     // Language driver for destination
      pFLDDesc       pfldsDst           // Array of dest. fields returned
   );

DBIResult DBIFN DbiOpenFieldXlt (     // Open translation object
      pCHAR          pszSrcTblType,     // NULL for Logical
      pCHAR          pszSrcLangDrv,     // NULL if no tranliteration 
      pFLDDesc       pfldSrc,           // source field descriptor
      pCHAR          pszDestTblType,    // NULL for Logical
      pCHAR          pszDstLangDrv,     // NULL if no tranliteration 
      pFLDDesc       pfldDest,          // Source field descriptor
      pBOOL          pbDataLoss,        // Set to TRUE, for data loss
      phDBIXlt       phXlt              // Returned translate handle
   );

DBIResult DBIFN DbiTranslateField (   // Translate a field
      hDBIXlt        hXlt,              // Translation handle
      pBYTE          pSrc,              // Source field
      pBYTE          pDest              // Destination field
   );

DBIResult DBIFN DbiCloseFieldXlt (    // Close translation object
      hDBIXlt        hXlt               // Translation handle
   );


//===========================================================================
//                                 MISC.
//===========================================================================


DBIResult DBIFN DbiGetTableOpenCount (   //  Get local cursor count
      hDBIDb         hDb,                    // Database
      pCHAR          pszTableName,           // Table name
      pCHAR          pszDriverType,          // Driver type /NULL
      pUINT16        piOpenCount             // returned number of cursors
);

DBIResult DBIFN DbiUseIdleTime (      // Use Idle time
      VOID
   );

DBIResult DBIFN DbiGetLdObj (         // Get language driver
      hDBICur        hCursor,           // Cursor handle
      pVOID         *ppLdObj            // Returned language driver object
   );

DBIResult DBIFN DbiGetLdName (        // Get language driver name from table
      pCHAR         pszDriver,          // Driver name
      pCHAR         pObjName,           // Name of object, i.e. table name
      pCHAR         pLdName             // Returned language driver name
   );

DBIResult DBIFN DbiFormFullName (     // Form Full Name
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszFullName        // Returns full name
   );

DBIResult DBIFN DbiAddPassword (      // Add a password to current session
      pCHAR          pszPassword        // Password
   );

DBIResult DBIFN DbiDropPassword (     // Drop a password from current session
      pCHAR          pszPassword        // password/NULL
   );

DBIResult DBIFN DbiGetNetUserName (   // Get network username
      pCHAR          pszNetUserName     // Returns username
   );


DBIResult DBIFN DbiDebugLayerOptions (// Get SDK debug layer options
      UINT16         iOption,           // Option
      pCHAR          pDebugFile         // 
   );

DBIResult DBIFN DbiOpenCfgInfoList (  // Open a cursor on "Config"
      hDBICfg	      hCfg,              // NULL
      DBIOpenMode    eOpenMode,         // ReadWrite or readonly
      CFGMode        eConfigMode,       // Config mode
      pCHAR	         pszCfgPath,        // Path
      phDBICur	      phCur     			 // Returned cursor
  );

#ifndef NODBIQBE
//===========================================================================
//                      Query Management
//===========================================================================

DBIResult DBIFN DbiQExecDirect (      // Execute query
      hDBIDb         hDb,               // Database handle
      DBIQryLang     eQryLang,          // Query language
      pCHAR          pszQuery,          // Query 
      phDBICur       phCur              // Returned cursor on result set
   );

DBIResult DBIFN DbiQPrepare (         // Prepare a query
      hDBIDb         hDb,               // Database handle
      DBIQryLang     eQryLang,          // Query language
      pCHAR          pszQuery,          // Query
      phDBIStmt      phStmt             // Returned statment handle
   );

DBIResult DBIFN DbiQExec (            // Execute prepared query
      hDBIStmt       hStmt,             // Statement handle
      phDBICur       phCur              // Returned handle on result set
   );

DBIResult DBIFN DbiQFree (            // Free statement handle
      phDBIStmt      phStmt             // Statement handle
   );

DBIResult DBIFN DbiQSetParams (       // Set query options
      hDBIStmt       hStmt,             // Statement handle
      UINT16         uFldDescs,         // Number of parameter field descriptors
      pFLDDesc       paFldDescs,        // Array of parameter field descriptors
      pBYTE          pRecBuff           // Record buffer
   );


#endif  // NODBIQBE

#endif  // NODBIPROTOTYPES

#ifdef __cplusplus
   }
#endif // __cplusplus

#endif



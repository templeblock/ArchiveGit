/***************************************************************************
*                   Copyright 1995 INSO Corporation                        *
*                Personal Dictionary Component Software                    *
*                                                                          *
* This work is protected by United States and international copyright laws *
* and contains certain trade secret information.  All use of this work is  *
* governed by the terms of agreements between INSO Corporation and its     *
* licensees or potential licensees.  Disclosure or disassembly of the      *
* contents of embodied programs or algorithms is prohibited.  All rights   *
* are reserved worldwide.                                                  *
****************************************************************************
 
*-----------------------------------------------------------------------*
*                                                                       *
*        PDdefine - Personal Dictonary Defines                          *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
        Description:
        Contains all of the Compile time defines and error codes        

        Cautions:
        None.

        Creation Date:
        01-Jul-93   DLB     Ver. 1.0.0

*------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*    Error returns at the API level                                     */
/*-----------------------------------------------------------------------*/

#define PDERRRANGE    999
#define PDSYSERR     5000  /* An operating system error has occured      */
#define PDAPPERR     5050  /* An application error exists                */
#define PDDATAERR    5100  /* An error occured in data, (IE; file read)  */
#define PDDATAWRN    5150  /* An warning occured in data, (IE; file read)*/

#define PD_STRUCTMEM_ERR   PDAPPERR+1   /* Bad value in structure member */

#define PD_DATA_ERR        PDDATAERR+1  /* File corruption error */
#define PD_RANGE_ERR       PDDATAERR+2  /* Invalid input word */
#define PD_MAXPID_ERR      PDDATAERR+3  /* Invalid number of PID's */
#define PD_PID_ERR         PDDATAERR+4  /* Invalid PID */
#define PD_DID_ERR         PDDATAERR+5  /* Invalid DID */
#define PD_READ_ERR        PDDATAERR+6  /* Read Error */
#define PD_FULL_ERR        PDDATAERR+7  /* The personal database is full */
#define PD_MAXDID_ERR      PDDATAERR+8  /* Invalid number of PID's */
#define PD_TRUN_ERR        PDDATAERR+9  /* Word(s) have exceeded max-length */
#define PD_NOTSAVED_ERR    PDDATAERR+10 /* Database(s) are not saved */
#define PD_PROC_ERR        PDDATAERR+11 /* General internal Processing error */
#define PD_NOTFND_ERR      PDDATAERR+12 /* Process could not find word in database */
#define PD_MAXRET_ERR      PDDATAERR+13 /* Too many returns requested */
#define PD_LOCK_ERR        PDDATAERR+14 /* File locking problem */
#define PD_HEAD_ERR        PDDATAERR+15 /* Error in PD header */
#define PD_OUTTYPE_ERR     PDDATAERR+16 /* Invalid output file type */
#define PD_READ_ONLY       PDDATAERR+17 /* File was opened read only */

/* DATA LEVEL WARNINGS */
#define PD_DUP_WRN         PDDATAWRN+1  /* Word exists in the personal database */
#define PD_FULL_WRN        PDDATAWRN+2  /* The Personal database is almost full */
#define PD_DATA_WRN        PDDATAWRN+3  /* Possible data error */
#define PD_HEAD_WRN        PDDATAWRN+4  /* Invalid type Personal database */
#define PD_DB_BUSY         PDDATAWRN+5  /* Personal database is locked */
#define PD_AUTOEXP_WRN     PDDATAWRN+6  /* Dictionary has been expanded */
#define PD_AUTODEL_WRN     PDDATAWRN+7  /* Dictionary words removed auto delete */
#define PD_NOALT_WRN       PDDATAWRN+8  /* Alternate Word not added */
#define PD_NOMORE_RETS     PDDATAWRN+9  /* All done in CORSTEP for SD's */

/* PD - Comp error Codes */
#define PD_SDINIT_ERR      PDDATAERR+18 /* Supplemental Database init. error */
#define PD_COMP_ERR        PDDATAERR+19 /* Compression error */
#define PD_EXISTS_ERR      PDDATAERR+20 /* Compression error */
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*  PD RAM DICTIONARY STORAGE IN MEMORY                                  *


The RAM dictionary is made up of 3 data structures: 

These are all in PDRDCTLBUF the RD control buffer

ofhash_index_tab 
----------------
The hash table, contains offsets into the actual data table

ofram_buff
----------
This is a table of the actual words as well as for each entry an index into
the extra info table, and the size of that entry.

The actual RD index in memory looks as follows:
    byte1 - used for indexing into the array of extra information 
    byte2 - used for indexing into the array of extra information
    byte3 - the length of the string
    byte4 - can be up to 512 characters of the index entry
  
   To get the index into the array of extra information you multiply byte1 by 255 and add byte 2

extraInfo
---------
A table with the extra information for each entry:
- A flag that indicates whether the table entry is in use
- The neg/alt info, 0x00 = pos word no alt entry, 0x01 = neg word, 0x02 = has alt entry
- The index into the altbuf for this entry
- The size of the altbuf for this entry
- PD_EXTRA_BYTES_MAX of extra info



 *-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/


/* Bit masks used                             */

/* Data byte #1 */
#define PD_BIT5         32    /* Used in PDsearch routine  - Ignores CAPS*/
#define PD_FUT_USE20  0x20    /* Future Use */
#define PD_FUT_USE10  0x10    /* Future Use */

#define PD_FREQ_MASK          0x0F    /* Used to extract the Frequency Information */

#define PD_DEL_FLAG     0xFFFE    /* Deleted word bit */
#define PD_EMPTY_FLAG   0xFFFF    /* empty entry    */  
#define PDNOFILTER      0xFFFF    /* No filter being used */


#define SUFFIX_TABLE    0x01
#define CAP_MAP         0x02
#define SOFT_HYP_MAP    0x04
#define ALT_ENTRY_OFF   0x08
#define NEG_WORD        0x10
#define PROD_SPECIFIC   0x20

#define SUFFIX_TABLE_LENGTH    1
#define CAP_MAP_LENGTH         8
#define SOFT_HYP_MAP_LENGTH    8
#define ALT_ENTRY_OFF_LENGTH   6
#define NEG_WORD_LENGTH        0

/* PDdb() API file use modes.
*/
#define PD_USE_FD       1   /* Use the file descriptor (handle) that is passed in.        */
#define PD_USE_PATH     0   /* Use the path and filename passed in. */


/* Rd hash defines */

#define PD_MAX_REHASH  32     /* Max. no. of rehashes allowed     */
                              /* exclusive of size                */ 
#define PD_MAX_POWER  0x8000  /* Flag used in finding the largest */

/* Correction Defines */
#define PDSTRTABSIZE 3000     /* Initial default value for correction string table */
#define PDSTPTABSIZE 275      /* Initial Default value for correction step table */
#define PDCLITTABSIZ 1024     /* Initial default value for clitic table */

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*  PD internal defines                                                  */
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

/* Internal Errors
*/
#define PD_CONFNONE     301 /* WARNING - No Confidence in Formatted File */
#define PD_CONFLOW      302 /* WARNING - Very Low confidence formatted file is correct */
#define PD_CONFGOOD     303 /* WARNING - Good confidence this is a formatted file */
                               /* Problem might just be an error in Header syntax */
#define PD_RETCORSTP    304 /* Cor sort return */

/* PD Misc Defines
*/
#define PD_ALLOCATED    9090   /* Signature for allocated block */
#define PD_UNALLOCATED  0

#define PDWRDMAX        SLWORDMAX   /* Maximum Size of Literal Dictionary Word */
#define PDASCIIMAX      128       /* Maximum intererl ASCII Line size */ 

#define PD_MAXFILENAME  (SLFNAMEMAX + 1)
#define PD_MAXPATHNAME  SLPATHMAX

#define PD_EXTRA_BYTES_MAX 12    /* Maximum possible length for the extra bytes */

#define PDMAXRETS       20

/* defines for filtering on negative/positive/both.
*/
#define FILTER_BOTH     0
#define FILTER_NEG	    1
#define FILTER_POS      2

/* RAM dictionary defines  */
/* Version number          */

#define PD_SVERSION     "2"        /* String representation of VERSION */
#define PD_VERSION      CHAR_2     /* ASCII representation of VERSION */

#define HEADER_END      "HEADER END#"    /* Literal for Formatted ASCII file */
#define HEADER_ESIZE    11         /* Size of the HEADER keyword */
#define HEADER_START    "#HEADER START"
#define HEADER_SSIZE    13
#define HEADER_COMP     "#HEADER COMP START"
#define HEADER_CSIZE    18



#define PD_ARBLK_SIZE    2000       /* Size of the Ascii Read Buffer */
#define PD_AWBLK_SIZE    2000       /* Size of the Ascii Write Buffer (LONG) */

/* PD Operation codes
*/
#define PD_START         1
#define PD_STOP          2
#define PD_CONTINUE      5

/* New PD Operation codes */
#define PD_OPEN_RO         1
#define PD_OPEN_RW         2
#define PD_CLOSE           3
#define PD_CLOSE_NOSAVE    4
#define PD_SAVE            5
#define PD_SAVE_NOSORT     6

/* PD TASK FLAGS */

#define PD_VERIFY            1
#define PD_ADD               2
#define PD_DELETE            3
#define PD_WLDCRD            4
#define PD_CORRECT           5   
#define PD_ANAGRM            6
#define PD_GETALT            7
#define PD_GETWORD           8   
#define PD_DEFRAG            9 
#define PD_CORSTEP          10
#define PD_GETWORD_NOSORT   11
#define PD_LOCK             12
#define PD_UNLOCK           13
#define PD_UNCOMP           14 /* Internal Decompression PDalt skip task */


/* Dictionary Otypes & Ctypes  */

#define PD_OPEN_FILE 1 /* Existing Database */
#define PD_NEW_FILE  2 /* An empty RAM database */
#define PD_RAW_FILE  3 /* Existing RAM Database containing a list-formatted
                          ASCII wordlist */
#define PD_FORM_FILE 4 /* Existing RAM Database containing a Personal 
                          Dictionary-formatted ASCII wordlist*/
#define PD_COMP_FILE 5 /* Existing compressed supplemental Database */
#define PD_TEMP_FILE 6 /* Temporary file not to be saved */

#define PD_OCOMP_FILE  7  /* Old compressed SD's */

#define PD_ALLDBS    0  /* Select all PID's dictionaries */
#define PD_SDDB_COMP 99999

/* Sd Defines */

#define PDHSIZE 1024

#define PDHEADDS         SFPRODSPECIFS + 0
#define PDHEADIQ         SFPRODSPECIFS + 8
#define PDHEADPQ         SFPRODSPECIFS + 16
#define PDHEADSUFFIX     SFPRODSPECIFS + 24
#define PDHEADHUFF       SFPRODSPECIFS + 32
#define PDHEADALTS       SFPRODSPECIFS + 40
#define PDHEADNEGWRDS    SFPRODSPECIFS + 48
#define PDWDBID          SFPRODSPECIFS + 56
#define PDLOCKEDFLAG     SFPRODSPECIFS + 68
#define PDTYPE           SFPRODSPECIFS + 60
#define PDEBLENGTH       SFPRODSPECIFS + 62

/* Constants used with old format SD's. */
#define PDHEADONEGTAB    PDHEADNEGWRDS
#define PDHEADONEGWRDS   PDWDBID
/****************************************/

/* RD header file positions */
#define HEADER_FIELD_START  0
#define HEADERSTART         1
#define VERSION_FIELD       2
#define VERSION             3
#define LANGUAGE_FIELD      4
#define LANGUAGE            5
#define PRODID_FIELD        6
#define PRODID              7
#define SUBPRODID_FIELD     8
#define SUBPRODID           9
#define SOFT_HYPHEN_FIELD   10
#define SOFT_HYPHEN         11
#define DATABASE_ID_FIELD   12
#define DATABASE_ID         13
#define LOCKED_FIELD        14
#define LOCKED              15
#define FILTER_FIELD        16
#define FILTER              17
#define NEGWORDS_FIELD      18
#define NEGWORDS            19
#define EBFLAG_FIELD        20   
#define EBFLAG              21
#define HEADER_FIELD_END    22
#define HEADEREND           23
#define LAST_FIELD          24

#define PD_20_FIRST_FIELD DATABASE_ID_FIELD

#define END_OF_STREAM 256


#define SD_HEADER   0

#define HUFF_THRESHOLD  1000

#define PD_SORTBLOCK_SIZE  0xFD00 /* Determines How Much Memory is allocated for Internal Sorting */
                                  /* The larger this number, the faster the performance */
                                  /* 0xFD00 is the largest for segmented machines (ie; dos) */

#define SD_MAX_WORDS PD_SORTBLOCK_SIZE/2
#define SD_MAX_BLOCKS SD_MAX_WORDS/2

#define SD_DATA_PREAMBLE_SIZE   1  /* Number of bytes in front of string */
#define SD_DATA_TERM_SIZE       1  /* Number of bytes for string terminator */
#define SD_DATA_SUFFIX_SIZE     1
#define SD_DATA_CAP_MAP_SIZE    4
#define SD_DATA_HYP_MAP_SIZE    4
#define SD_DATA_ALT_ENTRY_SIZE  6 
                                 
#define SD_SECTOR_PAD           0x14
#define SD_WRITESORT_BLOCK_SIZE 128
#define SD_BLOCK_EMPTY          0xFFFF

#define SD_PROGRESS_METER       100

/* File Compressor Defines */

#define SD_CORAW     0
#define SD_COFORM    1
#define SD_UNCORAW   2
#define SD_UNCOFORM  3

#define PD_RAW_BIT   1
#define PD_FORM_BIT  2
#define PD_COMP_BIT  3
#define PD_OCOMP_BIT 4

/* infile1 bits 1, 2 and 3, infile2 bits 3, 4 and 5, outfile bits 6 and 7 */
#define PD_FILE1MASK(x) ((x) & 0x07)
#define PD_FILE2MASK(x) (((x) >> 3) & 0x07)
#define PD_OUTFILEMASK(x) ((x) >> 6)

/* Macros to set the SL_STATUS structures through the PD. */

/* The process ID is in the high word and 
   the dictionary ID is in the low word of dwERRINFO 
*/
#define PDSETSTATUS(pSTATUS, nRETCODE, wERROR, wPID, wDID) \
            SLSETSTATUS (pSTATUS, nRETCODE, wERROR,        \
            (dwERRINFO)(((UINT4B)wPID << 16) | (wDID)))

#define PDCLEARSTATUS(pSTATUS) SLCLEARSTATUS(pSTATUS)

/* The process ID is in the high word of dwERRINFO */
#define PDGETPID(dwERRINFO) (UINT2B)((((UINT4B)dwERRINFO) >> 16) & 0xffff)

/* The dictionary ID is in the high word of dwERRINFO */
#define PDGETDID(dwERRINFO) (UINT2B)(dwERRINFO)


/* End of File - PDdefine.h
*/

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
*    PDctrl.h - Typedef for all the control structures.                 *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *

    Creation Date:
        01-Jul-93   DLB                

*-----------------------------------------------------------------------*/

SLF_TYPDEF_OR (SLF_HDL_OBJ, PFPD_RET, _ORF_PFPD_RET) ORF_PFPD_RET;


typedef struct extrainfo
{
    UCHAR uEntryInUse;  /* if true then this table entry is in use */
    UCHAR uNegAltInfo;  /* 0x00 = pos word no alt entry, 0x01 = neg word, 0x02 = has alt entry*/
    UINT4B wAltInfo;    /* the actual offset into the Alt buf  */
    UINT2B wAltSize;    /* the size of this alt entry */
    UCHAR aExtraBytes[PD_EXTRA_BYTES_MAX]; /* the extra byte info */
}EXTRAINFO, FAR *PFEXTRAINFO;

#define POSWORD 0x00
#define NEGWORD 0x01
#define HASALT  0x02
/* #define NOALT 0x04  not needed */


/* The actual RD index in memory looks as follows:
    byte1 - used for indexing into the array of extra information 
    byte2 - used for indexing into the array of extra information
    byte3 - the length of the string
    byte4 - can be up to 512 characters of the index entry
  
   To get the index into the array of extra information you multiply byte1 by 255 and add byte 2
*/ 
 
#define EXTRAINFOINDEX1 0
#define EXTRAINFOINDEX2 1
#define STRINGLENGTH    2
#define STRINGBEGIN     3


typedef struct PDRDCTLBUF /* RD Control Buffer */
{

   

   FLAG modified;              /* If true, then DB has been modified - Save to disk */
   FLAG file_read;             /* If true, reading in a file, ignore autodelete, FULL ERR */

   UINT2B hash_siz;            /* Size of the hash table              */
   UINT2B ram_ent;             /* Number of 1-byte entries in table    */
   UINT2B hash_ent;            /* Number of 2-byte entries on table    */
   UINT2B ram_siz;             /* Size of the character list        */
   UINT2B size_siz;            /* Size of the size list        */

   UINT4B dict_size;           /* This is the actual block size of      */
                                                                    
   UINT2B max_ent;             /* Maximum number of entries allowed    */
   UINT2B rehash;              /* Maximum rehashes in table             */
   UINT2B indx_mask;           /* Used to suppress the index          */
   UINT2B last;                /* Number of the last word processed with Getwrd*/
   UINT2B used;                /* # words getwrd has processed */
   UINT2B stored;              /* Total number of words stored          */
   UINT2B next;                /* Next available character in the list  */ 
   ORF_2BUINT ofhash_index_tab;/* The has table of RAM dictionariy entry offsets (old hash_tab)*/  
   ORF_BUFF ofram_buff;        /* This is the actual RAM dictionary  containing the orginal caped-hyphened word (old list_tab)*/ 
   PFEXTRAINFO extraInfo;      /* This is the extra information we store for an entry */
   UINT2B extraTableSize;      /* This is the number of entries in the extra table */
   UINT2B nextAvailableSlot;   /* This allows us to cache the next available entry in the extraInfo table */
   PFINT2B temp_index_tab;     /* Used to temp store index_tab for autoexpand & sort */
#ifdef PD_ALTWORD_SUPPORT
   UCHAR temp_altname[PD_MAXFILENAME];  /* Name of the ALternative File */
   SL_FHANDLE alt_fd;                      /* Filehandle of the aAlternative TEMP file */
#endif

} PDRDCTRL, *PPDRDCTRL, FAR *PFPDRDCTRL;

/* Huffman Structures */

typedef struct bit_buff {
    PUCHAR huff_buff;
    UINT2B buff_pos;
    UCHAR mask;
    INT2B rack;
} BIT_BUFF, *PBIT_BUFF, FAR *PFBIT_BUFF;


typedef struct pd_bit_file {
    FILE *file;
    UCHAR mask;
    INT2B rack;
} PD_BIT_FILE, FAR *PFPD_BIT_FILE;

typedef struct tree_node {
    UINT2B count;
    UINT2B saved_count;
    INT2B child_0;
    INT2B child_1;
} NODE, * PNODE, FAR * PFNODE;

typedef struct hode {
    UINT2B hcode;
    INT2B hcode_bits;
} HCODE, * PHCODE, FAR * PFHCODE;

typedef struct PDSCTLBUF  /* SD control Buffer */
{
   UINT4B sd_pos;         /* Position of Compressed Sectors */
   UINT4B sd_size;
   UINT4B iq_pos;         /* Position of INIT quads */
   UINT2B iq_size;
   UINT4B pq_pos;         /* Position of POINTER table */
   UINT2B pq_size; 
   UINT4B suf_pos;        /* Position of Suffix Table */
   UINT2B suf_size; 
   UINT4B cnt_pos;        /* Position of Huffman Count Table */
   UINT2B cnt_size; 
   UINT4B alt_pos;        /* Position of Alternative Entries */
   UINT4B alt_size; 
   PFNODE nodes;          /* Huffman Nodes */
   PFHCODE codes;         /* Huffman Code */
   INT2B root_node;       /* Huffman Root */
   PFBIT_BUFF hufbuff;    /* Compressed Huffman BUffer */

   UINT4B neg_pos;        /* Position of Neg Word Table in old format SD */

   ORF_BUFF ofsd_buffer;  /* Uncompressed Delta/Suffix buffer */
   ORF_BUFF ofsuffix_tab; /* Suffix Table */
   PFUCHAR negHashTable; /* Hash table of negative words */
   UINT2B hashTableSize;  /* Size of the hash table */
   UINT2B lastBlockRead;  /* used when listing out words */
   UINT2B lastWordRead;   /* used when listing out words */
   CHAR   listWords;      /* this tells us that the current task is really a PD_GETWORD */
} PDSCTRL, *PPDSCTRL, FAR *PFPDSCTRL;

typedef struct PD_buff
{
   UINT2B  wPID;          /* ID of calling process */
   UCHAR  byProdID;       /* Product Identification Code */
   UCHAR  byProdSubID;    /* Product Identification Sub Code */
#ifdef MAC_HANDLES
   FLAG bPDHndlLck;       /* True if handles are currently locked */
#endif
 
} PDbuff,
  *PPDbuff,
  FAR * PFPDWRDBUFF;



typedef struct PDIbuff
{

 CHAR       o_type;                     /* Type of dictionary opened     INPUT & OUTPUT */
 SLPATH     OpenPath;                   /* Path of opened dictionary       INPUT */
 UCHAR      OpenFilename[PD_MAXFILENAME];/*  name of opened dictionary      INPUT */
 UINT4B     dwOpenFileOffset;           /* offset in file of this dictionary */ 
 UINT4B     dwOpenFileLength;           /* offset in file of the end of this dictionary */ 
 SL_FHANDLE openFileHandle;             /* used for embedded files */
 FLAG       bFileAlreadyOpen;           /* file was already open */
 FLAG       bOpenedReadOnly;            /* file was opened read only */
 CHAR       c_type;                     /* Type of dictionary to save    INPUT & OUTPUT */
 SLPATH     ClosePath;                  /* Pathname of file to save        INPUT & OUTPUT */
 UCHAR      CloseFilename[PD_MAXFILENAME];/* name of file to save/ convert   INPUT & OUTPUT */
 SLPATH     AltPath;                    /* Path of alternative disk file */
 FLAG       nFile;                      /* Write access allowed flag       OUTPUT */
 FLAG       nLocked;                    /* read only - Used when get word has been started */
 FLAG       corstepMode;                /* Dictionary is in a corsort stream mode */
 UINT2B     nDictSize;                  /* Dictionary Size - */
 CHAR       nEOL;                       /* # of EOL characters to write out - 1 or 2 */
 FLAG       nDupWords;                  /* There are Duplicate words in this dictionary */
                                        /* This forces Verification to continue past its 1st */
                                        /* Match so it can find the other duplicates */

 UCHAR soft_hyp_char;                   /* Input soft hyphen character */
 UCHAR dict_lang;                       /* Language of this dictionary character */
 UINT4B lang_mask;                      /* bit mask of language */
 SF_MAP mHypMap;
 UCHAR  byProdID;                       /* Product Identification Code */
 UCHAR  byProdSubID;                    /* Product Identification Sub Code */
 UINT2B wDBID;                          /* The ID of the database this was created for */
 UINT2B wPrecedenceNumber;              /* The Precedence number for this database */
 FLAG   bLockedFlag;                    /* Is this personal database locked */
 UCHAR  uPDType;                        /* PD type information */
 UCHAR  uEBLength;                      /* The length of the extra byte field */     
 UINT2B wNumNegWords;                   /* The number of negative words in this database */
#ifdef SF_ICS
#ifdef SF_CLITIC_PROCESS
 PFUCHAR pCliticTab;                    /* Local Copy of Clitic Table */
#endif
#endif
 UINT2B free;
 UINT2B mem;  
 UINT2B mem_org;                        /* Starting RAM memory size, used in percent calculation */
 UINT2B out_ct;
 UINT2B nNumwrds;                       /* Number of words in Dictionary - This might be a TEMP */
 UINT2B out_exact; 
 INT2B  max_alt_ent;                    /* This is the maximum # of entrys allowed */
 UCHAR task;
 ORF_PFPD_RET ofpTemp;                  /* Temp Pointer used in Correction sorting */
 ORF_BUFF ofpSearch_wrd;                /* Temp Pointer used in Search */
 ORF_BUFF ofpSav_word;                  /* Temp Pointer used in Search */
 ORF_BUFF ofpSav_word2;                 /* Temp Word Holder for PD searching */
 ORF_BUFF ofpAscii_wrd;                 /* Temp Word Holder for PD */
    
 
 UINT2B wPID;                           /* ID of  process        INPUT */
 UINT2B wDID;                           /* ID of this dictionary      OUTPUT */

 PFPDRDCTRL pfCTRLbuff;                 /* Pointer to RD control buffer     OUTPUT */
 PFPDSCTRL pfCCTRLbuff;                 /* Pointer to SD control buffer     OUTPUT */

 PFSF_DCTRL  pDCtrl;                    /* Pointer to the Correction Ctrl Structure */
 PFSF_DVCTRL pDVCtrl;                   /* Pointer to the Correction Virtual Conrtols */

} PDIbuff, *PPDibuff,
FAR * PFPDIBUFF;


typedef struct PD_header_info
{
   UCHAR soft_hyp_char;                   /* Input soft hyphen character */
   UCHAR dict_lang;                       /* Language of this dictionary character */
   UCHAR  byProdID;                       /* Product Identification Code */
   UCHAR  byProdSubID;                    /* Product Identification Sub Code */
   UINT2B wDBID;                          /* The ID of the database this was created for */
   FLAG   bLockedFlag;                    /* Is this personal database locked */
   UCHAR  uPDType;                        /* PD type information */
   UCHAR  uEBLength;                      /* The length of the extra byte field */     
   UINT2B wNumNegWords;                   /* The number of negative words in this database */
} PD_HEADER_INFO, FAR * PFPD_HEADER_INFO;

/* End of file - PDctrl.h
*/

/****************************************************************************
 *                     Copyright 1995 INSO Corporation                      *
 *                    IntelliFinder(TM) Reference Engine                    *
 *                                                                          *
 * This work is protected by United States and international copyright laws *
 * and contains certain trade secret information.  All use of this work is  *
 * governed by the terms of agreements between INSO Corporation and its     *
 * licensees or potential licensees.  Disclosure or disassembly of the      *
 * contents of embodied programs or algorithms is prohibited.  All rights   *
 * are reserved worldwide.                                                  *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 *                                                                          *
 * REdbhead.h - Defines for the Reference Engine database headers.          *
 *                                                                          *
 *--------------------------------------------------------------------------*


    Description:

    RE header and file organization:
         SFHEADER
             SF IDX header
             Product-specific header (head header)
         ICS tables
         ICS-encoded index data
      then for each logical file (ENT, Bsrch, EFAT, RE add'l ICS index info):
         RE logical file header
         RE tables
         logical file (ENT data, ICS-encoded Bsrch data, EFAT data, but not
            ICS-encoded index data)

    To comply with SF specifications of product file headers, RE headers must
    start with an ICS-readable header, which expects ICS data to start 1K
    after the start of the file.  RE may use the product-specific area to
    mention non ICS-compatible data, which will include addtional tables &
    descriptions of the ICS logical file and a list of other logical files &
    pointers to those files' headers.  The RE refers to the product-
    specific area as the "head header".

    Note: look for REHFILLER_* defines to insert new fields where obsolete
    fields were removed.  When header fields are no longer needed, they are
    sometimes replaced with an RE_FILLER_* define rather than slide up all
    following fields' values (which would make recompilation of code and
    regeneration of databases necessary).

    Creation Date:
    17-jul-89   JGH

    Modification History:

    Date        Author  Comment
    23-jun-92   JCC     Major rewrite, with SF header & logical files
    12-aug-92   JCC     BISKEY & BISLMT derived from table-table

*---------------------------------------------------------------------------*/

#ifndef REDBHEAD_H

#define REDBHEAD_H

/* Size of the SF header for all RE databases.  This is unlikely to change.
*/
#define RE_SF_HEADER_SIZ    1024

/* Product-specific area defines in the SL header
*/

/* Fields of the head header, which describe an RE reference work as a whole
 * and therefore need to appear in only one header.  This stuff won't be in
 * the RE logical file headers, only in the product-specific zone of the SL 
 * header.
*/
#define RERWSERIAL      700     /* Serial number of compressed refwork      */
#define RERWTYPE        704     /* Type of ref. work (almanac, dict., etc.  */
#define RERWNAME        706     /* Name of printed work (text description)  */
#define RERWMODEL       738     /* Model # of refwork (int ID for above)    */
#define RERWEDITION     740     /* Edition of printed work                  */
#define RERWPUBLISHER   742     /* Publisher's name                         */
#define REHFILLER_2     762     /* Use the next 8 bytes for new fields      */
#define REGLUED_CRDATE  770     /* Creation date of glued refwork           */
#define REGLUED_VER     776     /* Version number of glued refwork          */
#define REEFATGEN       786     /* EFATed generation                        */
#define RECHARSET       787     /* Character set used in lexicon            */
#define RERETWORDTYP    788     /* Returned-word type (SF_RETWORD et al.)   */
#define RESIZ_LOGFILTAB 790     /* Number of element in logical files table */
#define RELOC_LOGFILTAB 792     /* Location of logical files table          */
#define RELOGHDRSIZ     796     /* Size of logical headers in SF std blocks */
#define REDIALECTS      798     /* Available dialects in refwork            */
#define REHEADFUTURE    800     /* Future expansion                         */

#define REHEADHDREND    810     /* Head header end                          */

/* Logical file header fields.  A logical file header is RE-specific and is 
 * pointed to from within the product-specific area of the SL header.  
 * Logical headers appear before .ENT and Bsrch .IDX data, and other 
 * RE-specific logical files.
*/

/* Fields of every logical file header but unique value for each logical file 
 * comprising an RE refwork.
 * 
 * Note: The REFILEVER field should be updated if the master lexicon is 
 * edited or the lexicon is recompressed.  The RELOGFILVER field should be
 * updated whenver any information in the logical file changes.  Therefore, 
 * the RELOGFILVER field should change when REFILEVER changes since the disk 
 * file to which REFILEVER refers is included in the logical file.  The 
 * version in RELOGFILVER should therefore always be >= REFILEVER.
 * RELOGFILVER is the field used by RE init software for version (backward
 * compatibility) checking.
*/
#define REFILETYPE       0      /* File type (entry,index)                  */
#define REFILECONT       2      /* Content (gen'l semantic desc: "defs")    */
#define REFILEDESC       4      /* Text description of content ("main defs")*/
#define RECONTATTRIB    24      /* Attributes of content (how to parse)     */
#define REDATAFORM      26      /* Data format (compressed (how)/text)      */
#define REENCODEVER     28      /* Version of encoding (compression) format */
#define REFILESIZ       30      /* Size of logical file in SF blocks, w/hdr */
#define REFILEVER       32      /* Version number of LF's disk file         */
#define REHCRDATE       42      /* Creation date of disk file (when cmprssd)*/
#define RESOURCEID      48      /* Source ID (ASCII file filename)          */
#define RELOGFILVER     60      /* Version # of logical file (disk + tables)*/
#define RECODEVER       70      /* Source code version current when LF made */
#define RESIZ_TABTAB    80      /* Size of table-table                      */
#define RELOC_TABTAB    82      /* Location of " (pos. in LF, is inside hdr)*/
#define RELFCRDATE      86      /* Creation date of LF (when REtabtab run)  */
#define REDATAATTRIB    92      /* Related to REDATAFORM, this contians     */
                                /* data compression attributes.             */
#define RELOGFUTURE     94      /* Future expansion                         */

#define RELOGHDREND     100     /* LF-specific fields start at 102          */

/* Fields unique to a particular logical file type.  All start after the
 * common logical file header area.  The next chunk in log. headers contains 
 * different data depending on the logical file type, which is specified in
 * the REFILETYPE field.  Note that the area is remapped - the ENT and IDX
 * fields have many of the same values.
*/

/* Defines relating specifically to the entry files of reference works
 * 
 * Note: the REMAXENTSTRLEN field refers to the string returned with an ST
 * number.  It applies only to works that get the returned word from the
 * entry file, whose IDXENTMAP field is SF_HEADWORD, SF_DEFNWORD, or 
 * SF_RETDEF.
*/
#define REMAXCOMPSTR    102     /* Maximum length of compression code's text*/
#define REMAXENTNUM     104     /* Number of entries in file (max ST#)      */
#define REMAXENTSIZ     108     /* Maximum entry size                       */
#define REAVEENTSIZ     110     /* Average entry size                       */
#define REAVECOMPRATIO  112     /* Average compression ratio                */
#define RESBLOCKSIZ     114     /* Sblock size                              */
#define REDECOMPSTKSIZ  116     /* Decompression stack size                 */
#define RECODBITS       118     /* Bits per compression code                */
#define REMAXLINLEN     120     /* Maximum line length in file              */
#define REMAXENTRETWRD  122     /* Maximum string returned w/ST#, if applcbl*/

#ifdef RE_FN_TOCHA /* RE_NEWTOCHA */

/* Note: the foll could also be moved to index log file, left here for consistency */
#define RETOCBMAPCOMPOFFSET 124 /* offset in toc bitmap file to compressed data */
#define RETOCLONGESTBMAP    128 /* longest bm in toc bitmap file*/
#endif /* old not RE_NEWTOCHA */

#define REENTFUTURE     130     /* Future expansion                         */

#define REENTHDREND     144     /* ENT header end                           */


/* Defines relating specifically to ICS-encoded logical files of reference 
 * works (both the ICS-compatible file and the Boolean search file).
 *
 * Note: the REMAXIDXSTRLEN field is not a duplication of information in the
 * REHMAXLEN (max length of word in wordlist, i.e., a verifiable word) field 
 * because it refers to the returned word, i.e., the word handed back with an 
 * ST#, which might not be a verifiable word.
*/
#define REWORDLISTSIZ   102     /* Amt of words in wordlist (wordform cnt)  */
#define REUNQWORDSIZ    106     /* Amt of unique wordforms in wordlist      */
#define REBASEWORDSIZ   110     /* Amt. of base forms in wordlist           */
#define REAVEWORDLEN    114     /* Average wordform length                  */
#define REMAXENTRET     116     /* Max # ST #s returned with wordform       */
#define REMAXIDXRETWRD  118     /* Maximum string returned w/ST#, if applcbl*/
#define REIDXFUTURE     120     /* Future expansion                         */

#define REIDXHDREND     138     /* End of IDX header                        */

/* JCCRE8 */ 
/* Defines for fields that, for the ICS-compatible file (main
 * index list), are found in the standard header (see SFHEADER.H).  The main
 * index list shouldn't use these fields if it is to conform to the
 * no-redundant-data rule of the SF header spec.  Since there's only 1 SF
 * header per project, non-ICS-compatible logical files (Boolean search)
 * can't use SF header fields.  RE duplicates the here because the Boolean
 * search file needs them.  The main index list uses them, rather than the SF
 * header fields, for ease of implementation
*/

/* Note: the following field, REHBLOCKSIZ, could be computed by dividing the
 * length of the disk file and dividing by the .wNumElems value for the 
 * pointer table (ptr_tab).
*/
#define REHBLOCKSIZ     140     /* Logical block size of encoded data       */
#define REFILLER_1      142     /* Use these 4 bytes for new fields         */
#define REHMAXLEN       146     /* Maximum length of word in wordlist       */
#define REHBITS         148     /* Bits per compression code                */
#define REICSIDXFUTURE  150     /* Future expansion                         */

/* Defines for fields that pertain only to the main index list.
 * Note: The String Translation Table is currently attached to the main index
 * list logical file, which is arbitrary.  It pertains to the entire 
 * reference work; could be attached to the entry logical file as well.
*/
#define REMAXSTRXLTSTR  160     /* Longest string in string translation tab */
#define REMAINIDXFUTURE 162     /* Future expansion                         */

#define REICSIDXHDREND  170     /* End of ICS IDX header                    */

/* Logical Files Table record.  Stored in the product-specific area of the
 * SF header, this indicates where .IDX, .ENT, Bool .IDX, and other data are
 * located.
*/
typedef struct _LOGFILTAB_RE
{

    INT2B nFileType;    /* entry, ICS index, EFAT */
    INT2B nFileCont;    /* definitions, Boolean search, ... */
    INT4B lOffset;      /* to logical file from the beg. of SF header */
    INT4B lDiskDataOff; /* offset to disk data within LF (skip hdr & tabs) */

} LOGFILTAB_RE, FAR *PF_LOGFILTAB_RE, NEAR *PN_LOGFILTAB_RE;

SLF_TYPDEF_OR( SLF_HDL_OBJ, PF_LOGFILTAB_RE, _OF_LOGFILTAB_RE ) OF_LOGFILTAB_RE;
SLN_ITYPDEF_OR( PN_LOGFILTAB_RE, _IN_LOGFILTAB_RE ) IN_LOGFILTAB_RE;

/* Logical file type defines.  These go into the REFILETYPE header field.
 * They are used unmodified as subscripts into the gEncodVer array.
 * 
*/
#define NUM_LFTYPES_RE  3   
#define ENTRY_RE        1
#define ICSINDEX_RE     2
#define EFAT_RE         3

/* Logical file content defines (qualifier of file type).  If a reference
 * work holds 2 or more of any type of file, what kind is each?  These go
 * into the REFILECONT header field.
*/
/* Entry files
*/
#define MAINENTRIES_RE  1

/* Index files
*/
#define STANDALONE_RE   1   /* verify/correct, no .ENT links */
#define VER_ENT_RE      2   /* verify/correct & map to .ENT file */
#define SEARCH_RE       3   /* Search indexing of .ENT file */

/* Masks for the content attributes field.  The field's value is dependent
 * upon the logical file type & content.
*/
/* Entry file masks
*/
#define PARSER2_CMASK_RE    0x0001

/* Main index file masks
*/
/* Index field (state) masks.  Do records in the index list have these fields
 * in them?  If so, need to switch to these states.  Use the SF values.
*/
#define POS_CMASK_RE        POS_FMASK_SF        /* part of speech */
#define BASEFORM_CMASK_RE   BASEFORM_FMASK_SF   /* baseforms */
#define POI_CMASK_RE        POI_FMASK_SF        /* parts of inflection */
#define IRRINF_CMASK_RE     IRRINF_FMASK_SF     /* irregular inflections */
#define SYN_CMASK_RE        SYN_FMASK_SF        /* synonyms */

/* Table-table.  This structure is used to locate RAM tables in logical 
 * files.  It allows them to be order-independent in a file and makes it
 * easier to remove a table from the file if it isn't needed.
 * The logical files might not be at beginning of physical file.  In that
 * case an offset (from the LogFilTab, for instance) may be added to the 
 * .lOffset member to seek to a table.
*/
typedef struct _TABTAB_RE
{

    INT2B  nTabID;      /* unique ID for a particular table */
    UINT4B lOffset;     /* from beginning of logical file */
    UINT4B lElemSiz;    /* each element of table is this size, in bytes */
    UINT2B wNumElems;   /* number of elements in table */
    UINT4B lTabSiz;     /* total size in bytes of table */

} TABTAB_RE, FAR *PF_TABTAB_RE, NEAR *PN_TABTAB_RE;

SLF_ITYPDEF_OR( PF_TABTAB_RE, _IF_TABTAB_RE ) IF_TABTAB_RE;
SLN_ITYPDEF_OR( PN_TABTAB_RE, _IN_TABTAB_RE ) IN_TABTAB_RE;

/* Types of ORs used to hold tables to be initialized.  The table init 
 * routine can take any one of these types of ORs for a table it's supposed
 * to init, switching off different types of memory management during
 * alloc/lock.
*/
#define OF_ORTYPE   1   /* far regular object referencer */
#define GF_ORTYPE   2   /* far plug */
#define IF_ORTYPE   3   /* far immediate */

/* Table IDs.  These go in the .nTabID member of the table-table.
*/
/* Entry file tables
*/
#define DSTARTS_TABID_RE        10      /* Pointers into the Entry File */
#define LDECOMP_TABID_RE        20      /* Left Decomp Codes.           */
#define RDECOMP_TABID_RE        30      /* Right Decomp Codes.          */
#define TOCDESC_TABID_RE        40      /* Table Of Contents Structure  */

#ifdef RE_FN_TOCHA /* RE_NEWTOCHA */
/* the new bitmaps, uncompressed & compressed */
#define TOCBITFIELD_TABID_RE       50
#else /* old not RE_NEWTOCHA */
#define TOCTITLE_TABID_RE       50  /* unused */
#endif /* old not RE_NEWTOCHA */

#define PREDECOMP_TABID_RE      60      /* Explicit Strings for codes.  */
#define FSDESC_TABID_RE         70      /*                              */
#define TOKENIZER_TABID_RE      80      /* Tokenizer FSA table          */
#define PARSER_TABID_RE         90      /* Parsing Table                */
#define ACTION_TABID_RE         100     /* Parse action table           */

/* Index file tables
*/
#define STRINGS_TABID_RE        200     /* String Block for Explicit str*/
#define ISTARTS_TABID_RE        210     /* Start Offsets for each str.  */
#define LENGTHS_TABID_RE        220     /* Length of each string.       */
#define PTR_TABID_RE            230     /* Start Quads for each block   */
#define NPTR_TABID_RE           310     /* Index reccord # for each blk */
#define STR_TABID_RE            240     /* Spelling String Xlations     */
#define QUAD_TABID_RE           250     /* Table of all Quadgrams       */
#define RIGGED_TABID_RE         260     /* Spelling Correction Rigged   */
#define STEP_TABID_RE           270     /* Spelling Correction Steps.   */
#define CLIT_TABID_RE           280     /* Clitics for the language.    */
#define STRXLT_TABID_RE         290     /* String Translation Struct.   */
#define STRBLK_TABID_RE         300     /* String Text Block.           */

/* JCCRE11 */

#define RE_SUB_BUF      1000    /* Maximum no. of string substitution   */
#define RE_LETTER_BUF_SZ 256    /* Size of letters buffer               */
#define RE_STEP_SZ  13          /* No. of bytes per step in step table  */

/* parts of speech and number of fields that terminate a part of speech
 * field.  Used in searching for p.o.s. in an entry of the definition db.
*/
#define NUM_PARTS       11
#define NUM_ENDS        1

/* number of different languages for which there are separate databases.
*/
#define RE_NUM_LANGS    16

/* length of database version strings
*/
#define RE_DBVER_LEN    9

#endif /* REDBHEAD_H */

/* End of file - REdbhead.h
*/

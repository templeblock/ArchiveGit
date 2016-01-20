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
 * REdmhdr.h - Defines for file manager internal operations.                *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   Typedefs defined for AHED file manager internal operations.

Creation Date:
   27-aug-1991  ECH

Change Record:
    Date        Author  Comment
    15-nov-91   JCC     TOCHA added
    07-jan-92   ECH     Added app_id to the real database structure to keep
                        track of who owns the memory.
    21-feb-92   RAS     ED names -> RE
    10-jul-92   JCC     Single file refworks - no bool files

*---------------------------------------------------------------------------*/

#ifndef REDMHDR_H

#define REDMHDR_H

/* Defines for the Database Manager.
*/

#define DM_STOP_ACTION      0
#define DM_PAUSE_ACTION     2
#define DM_BUSY_ACTION      4

#define DM_NO_ACTION        10
#define DM_DBINIT_ACTION    12
#define DM_ALTERN_ACTION    14
#define DM_ANAGRM_ACTION    16
#define DM_WILDCH_ACTION    18
#define DM_BOLSCH_ACTION    20
#define DM_GETDAT_ACTION    22
#define DM_CLOMCH_ACTION    24
#define DM_HRTINI_ACTION    26
#define DM_TOCHA_ACTION     28
#define DM_FLDSEL_ACTION    30
#define DM_DBEXIT_ACTION    32
#define DM_STRTAB_ACTION    34
#define DM_REINFLEC_ACTION  36

/* Bad real and virtual database id numbers.
*/
#define DM_BAD_DB_ID        -1
#define DM_BAD_VDB_ID       -1

#ifdef RE_SF_1RDB
/* The max. number of (virtual) database instances
 * when supporting only 1 real database
*/

#define DM_MAX_VDBS          3
#endif

/* Immediate declarations for real entry objects.
*/
SLF_ITYPDEF_OR (struct DM_db_ent FAR *, OR_idb_entry_dm) ORFDB_IENTRY_DM;

/* Structure declarations for real entries.
*/
typedef struct DM_db_ent
{
    INT2B       nDb_id;          /* The real db's ID number              */
    INT2B       nApp_id;         /* The ID app that owns the memory.     */
    FILEINFO_RE file_info;      /* The directory path for the db files  */
    INT2B       nUsage_count;    /* The number of times database is used */
    INT2B       nLock_count;     /* The number of times its memory is    */
                                /* locked.                              */
    ORFDB_CTRL_RE
                octrl;           /* Object reference to the control stru */
    ORFDB_IENTRY_DM
                onext;           /* Object reference to the next entry.  */
}
DB_ENTRY_DM,
FAR *PFDB_ENTRY_DM;


/* Immediate declarations for virtual entry objects.
*/
SLF_ITYPDEF_OR (struct DM_vdb_ent FAR *, OR_ivdb_entry_dm) ORFVDB_IENTRY_DM;

/* Structure declarations for virtual entries.
*/
typedef struct DM_vdb_ent
{
    INT2B       nVdb_id;         /* The virtual db's ID number (index)   */
    INT2B       nApp_id;         /* The ID of the owning application     */
    INT2B       nAction;         /* The Current Action                   */
    INT2B       nStatus;         /* The status of the current action     */
    ORFDB_IENTRY_DM
                oreal;           /* Object reference to the real element */
    ORFVDB_CTRL_RE
                ovctrl;          /* Object reference to the control stru */
    ORFVDB_IENTRY_DM
                onext;           /* Object reference to the next entry.  */
}
VDB_ENTRY_DM,
FAR *PFVDB_ENTRY_DM;

#endif /* REDMHDR_H */

/* End of file - REdmhdr.h
*/

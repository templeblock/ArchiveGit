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
 * REpddmhdr.h - Defines for PD file manager internal operations.                *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
   Typedefs defined for PD file manager internal operations.

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

#ifndef REPDDMHDR_H

#define REPDDMHDR_H

#ifdef RE_SF_PDICTIONARY

typedef SLF_HDL_OBJ HFPD_CTRL_RE;
typedef SLF_HDL_OBJ HFVPD_CTRL_RE;

SLF_TYPDEF_OR (HFPD_CTRL_RE, PFPD_CTRL_RE, OR_pd_ctrl_re) ORFPD_CTRL_RE;
SLF_TYPDEF_OR (HFVPD_CTRL_RE, PFVPD_CTRL_RE, OR_vpd_ctrl_re) ORFVPD_CTRL_RE;

/* Immediate declarations for real entry objects.
*/
SLF_ITYPDEF_OR (struct DM_pd_ent FAR *, OR_ipd_entry_dm) ORFPD_IENTRY_DM;

/* Structure declarations for real entries.
*/
typedef struct DM_pd_ent
{
    INT2B       nIFPDfileID;          /* The real db's ID number              */
    INT2B       nIFDBID;       /* The ID of the parent IF DB.     */
    FILEINFO_RE PDfile_info;    /* The directory path for the PD files  */
    INT2B       nMode;    /* READ_WRITE or READ_ONLY */
    INT2B       nUsage_count;    /* The number of times database is used */
    INT2B       nLock_count;     /* The number of times its memory is    */
                                /* locked.                              */
    ORFPD_CTRL_RE
                octrl;           /* Object reference to the PD control stru */
    ORFPD_IENTRY_DM
                onext;           /* Object reference to the next entry.  */
}
PD_ENTRY_DM,
FAR *PFPD_ENTRY_DM;


/* Immediate declarations for virtual entry objects.
*/
SLF_ITYPDEF_OR (struct DM_vpd_ent FAR *, OR_ivpd_entry_dm) ORFVPD_IENTRY_DM;

/* Structure declarations for virtual entries.
*/
typedef struct DM_vpd_ent
{
    INT2B       nIFPDdbID;         /* The virtual db's ID number (index)   */
    INT2B       nIFDBID;       /* The ID of the parent IF DB.     */
	INT2B       nIFPDfileID; /* input - id of real pd to attach */
    INT2B       nAction;         /* The Current Action                   */
    INT2B       nStatus;         /* The status of the current action     */
    ORFPD_IENTRY_DM
                oreal;           /* Object reference to the real element */
    ORFVPD_CTRL_RE
                ovctrl;          /* Object reference to the control stru */
    ORFVPD_IENTRY_DM
                onext;           /* Object reference to the next entry.  */
}
VPD_ENTRY_DM,
FAR *PFVPD_ENTRY_DM;

#endif /* RE_SF_PDICTIONARY */
#endif /* REPDDMHDR_H */

/* End of file - REpddmhdr.h
*/

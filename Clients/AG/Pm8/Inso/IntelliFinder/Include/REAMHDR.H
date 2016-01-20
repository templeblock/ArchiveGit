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
 * REamhdr.h - Defines for file manager internal operations.                *
 *                                                                          *
 *--------------------------------------------------------------------------*

Description:
    Typedefs defined for Reference Engine file manager internal operations.

Creation Date:
    27-aug-91   ECH

Change Record:
    Date        Author  Comment
    21-feb-92	RAS	ED names -> RE

*---------------------------------------------------------------------------*/

#ifndef REAMHDR_H

#define REAMHDR_H

/* Bad application ID definition.
*/
#define AM_BAD_APP_ID        -1

/* the RE application control structure; 
 * right now only contains the PD app ID
*/

typedef struct RE_app_ctrl
{
    INT2B nPDAPPID;
}
APP_CTRL_RE,
FAR *PFAPP_CTRL_RE;

/* Immediate declarations for application entry objects.
*/
SLF_ITYPDEF_OR (struct AM_app_ent FAR *, OR_iapp_entry_am) ORFAPP_IENTRY_AM;

/* Structure declarations for application entries.
*/
typedef struct AM_app_ent
{
    INT2B       nApp_id;         /* The ID of the owning application     */    
    APP_CTRL_RE ectrl;           /* embedded app control struct */    
    ORFAPP_IENTRY_AM
                onext;           /* Object reference to the next entry.              */
}
APP_ENTRY_AM,
FAR *PFAPP_ENTRY_AM;

#endif /* REAMHDR_H */

/* End of file - REamhdr.h
*/

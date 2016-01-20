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
 * REfields.h - Defines relating to field selection.                        *
 *                                                                          *
 *--------------------------------------------------------------------------*

Creation Date:
 16-sep-90    JCC

Change Record:
 01-may-91   JCC        handles for subfield lists, all ptrs FAR
 07-nov-91   ECH,JCC    use SLMM
 07-nov-91   JCC        chucked old stuff for defined field names
 21-feb-92	 RAS	      ED names -> RE 

 *--------------------------------------------------------------------------*/

#ifndef REFIELDS_H

#define REFIELDS_H

#ifdef RE_SF_FLDSEL

/* OEM specifies the value of the active flag.  If not defined, all
 * fields default to active status.  Otherwise, need to add 'active'
 * to parameter list of REsel*() in .C and .H.
*/
/*#define OEM_HAS_ACTIVE    */

/* put following define in DB header
*/
#define REMAXFLDTAG 8   /* Longest field marker: </WD> or </PSSEG>, e.g., w/o EOS */
#define REMAXFLDTAGSTR REMAXFLDTAG + 1  /* ditto, w/EOS */
#define RE_MAXFLDNAME   20  /* string identifier for a field, w/EOS */

/* this structure is used to identify fields during field selection.  Each
 * database has a unique set of these.
*/

/* This structure contains both dynamic and static members.  To work true to
 * the database manager design, this structure should be split into two 
 * structures: one for the static, real database related data and one for 
 * the dynamic, virtual database related data.
 *
 * Links between fields are implemented as pointers, allowing field selection
 * code to walk the tree of grammar.  A field's dynamic and static data must
 * be linked to other fields.  During processing, field selection code must
 * access both dynamic and static data quickly and repeatedly.  Without links 
 * from virtual database objects and associated real database objects, these 
 * data are best kept together.  Pointers are assigned prior to field 
 * processing for quick access.
 *
 * NOTICE: The field selection algorithms and data structures have been
 *      through a wave of optimization.  With the advent of stricter 
 *      adherence to SGML format, they may again be evaluated and optimized
 *      for speed and memory usage.
*/

typedef SLF_HDL_OBJ HF_FIELD_RE;

/* Plug decalration for field selection objects.
*/

SLF_PTYPDEF_OR (HF_FIELD_RE, struct _FIELD_RE  FAR *, OP_field_re) OPF_FIELD_RE;
SLF_TYPDEF_OR (HF_FIELD_RE, struct _FIELD_RE  FAR *, ORF_field_re) ORF_FIELD_RE;
SLF_TYPDEF_OR (HF_FIELD_RE, struct _FIELD_RE  FAR * FAR *, ORF_fldlst_re) ORF_FLDLST_RE;

typedef struct _FIELD_RE
{
    FLAG        status;         /* state of processing - recognition, inside, outside */
    FLAG        exclude;        /* include or exclude this field from output? */
    FLAG        active;         /* parse for this field or ignore it completely? */
    FLAG        endByParent;    /* field's end marked by parent's end   */
    FLAG        maybeSub;       /* a subfield is in the MAYBE_BEG state */
    UINT2B      twin;           /* if !0, index of field's twin         */
    UCHAR       temp[REMAXFLDTAGSTR];     /* buffer for saving during identification */
    UCHAR       temp2[REMAXFLDTAGSTR];    /* buf for saving during subfield identification */
    UCHAR       nextTagChar;              /* index into above buffer    */
    UCHAR       nextTag2Char;             /* index into above buffer    */
    UCHAR       begTag[REMAXFLDTAGSTR];   /* min. sequence of chars to uniquely ID field's beg */
    UCHAR       endTag[REMAXFLDTAGSTR];   /* min. sequence of chars to uniquely ID field's end */
    struct _FIELD_RE FAR
                *inSubfield;    /* a subfield is current - points to that field */

    INT2B       numSubFields;   /* size of subfield list                */
    ORF_2BUINT  subFldIdxs;     /* List of subfields' indexes (links).  */
	ORF_FLDLST_RE
                subFields;      /* far list of far addresses of subfields (links).  */

    INT2B       numEndFields;   /* size of endfield list                */
    ORF_2BUINT  endFldIdxs;     /* list of indexes of fields that end this one (links). */
	ORF_FLDLST_RE
                endFields;      /* sibling fields that terminate this one (links). */
}
FIELD_RE,
FAR *PF_FIELD_RE;



/* this structure is used to associate field names to their indexes into
 * an array of REFIELDSEL elements.
*/
typedef struct _FLDNAME_RE
{
    UCHAR       name[RE_MAXFLDNAME];/* predetermined string, represents a field */
    INT2B       index;              /* into pctlbuff's ->fields array   */
}
FLDNAME_RE,
FAR *PF_FLDNAME_RE;

typedef SLF_HDL_OBJ HF_FLDNAME_RE;

/* Object reference for the field names stru.
*/
SLF_TYPDEF_OR (HF_FLDNAME_RE, PF_FLDNAME_RE, ORF_fldname_re) ORF_FLDNAME_RE;

/* field status flag values
*/
#define LOOKING_FOR_BEG 1   /* no characters recognized yet, not in field   */
#define MAYBE_BEG       2   /* building beginning tag, might be in field    */
#define LOOKING_FOR_END 3   /* inside field, no end chars recognized yet    */
#define MAYBE_END       4   /* inside field, building ending tag            */

#endif  /* RE_SF_FLDSEL */

#endif /* REFIELDS_H */

/* End of file - REfields.h
*/

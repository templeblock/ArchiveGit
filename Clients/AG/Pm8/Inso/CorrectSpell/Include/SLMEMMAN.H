/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
* SLMEMMAN.H - typedefs, defines, and macros to support cross-platform  *
*              memory management.                                       *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:
            slmemman.h contains all the supporting defines to implement
            a multi-platform memory management function set.  Object 
            referencers, object management macros, object accessing macros, 
            and standard types are created here for each type of platform.
            See external documentation for definitions of these terms and
            explanations of macro use.

            All object management macros (alloc,lock,unlock, and free) return
            non-zero values when successful, zero upon failure.  Some
            operations naturally return 0 when they fail; others must be 
            coerced (such as ANSI C free(), which returns nothing).  The
            behavior of each O/S function call upon return has been examined
            to determine if coersion is necessary.

            No of our products should check the results of lock, unlock, or
            free statements.  See the description of SL_MEM_DEBUG below for 
            automatic lock/unlock/free checks.

            This file uses the attribute definition system.  It should be
            included with other standard library header files after
            SL.H.  It also uses the standardized function defines (e.g.,
            Cfree instead of free), so include it after SLCALLS.H.

            SL_MEM_DEBUG:
            slmemman.h has a compile-time-optional debug mode which clears
            all invalid pointers and handles.  That is, after a handle is
            unlocked, its associated pointer will be nulled, and after a
            block is freed, its associated handle will be nulled.  This
            aids in debugging - help catch uses of invalid pointers
            and handles, which are not normally cleared upon free/unlock.
            The explicit clearing causes overhead, however.

            SL_MEM_DEBUG also checks the results of allocs, locks, unlocks,
            and frees by using assert calls within the lock/unlock macros.

            This debug code is included during compilation if SL_MEM_DEBUG is
            defined in a product.  After source code is debugged, 
            SL_MEM_DEBUG can be #undef'd to remove unecessary overhead.

        Cautions:
            New Macros:
            Any additions, whether for new operations or supported 
            environments, should be designed such that they:

                -> Operate with as little overhead as possible
                -> Require little change to the source code
                -> Allocs return SL_ERR_ALLOC upon failure & non-zero upon 
                    success
                -> Have debug versions if appropriate
            and
                -> NEVER require #ifdefs in source code

            Attributes:
            No particular environment (MAC, WINDOWS, etc.) should 
            be mentioned in this file.  Find the appropriate attribute in
            one of the SLATT???.H files if some code requires an ifdef.

        Creation Date:
            02-oct-91   JCC     From test files for SLMM

        Change Record:
            15-apr-92   JES     Removed MoveHHi calls from MAC macros.
            29-may-92   JCC     SL_LOCK_CHK CTO; redefine macros for debug 
                                (nondeb has no ?:); non-zero == OK, 0 == ERR
            08-jul-92   JCC     Windows unlock & free returns !ed for ^
            06-oct-92   JCC     Support for hardware virtual memory (Win 3.1)
            29-oct-92   RCN     Support for huge unsigned buffers
            06-jan-93   JCC     Free macros don't return standard value
            06-dec-93   KB      Fix for Borland LMM with SLN_?ALLOC

*-----------------------------------------------------------------------*/

#ifndef SLMEMMAN_H

#define SLMEMMAN_H

/*                  Defines common to all environments
*/


/* standard type/constant declarations
*/
#define NULLS   0                   /* null scalar                          */
#define NULLNP  (VOID NEAR *) 0     /* null near pointer                    */
#define NULLFP  (VOID FAR *) 0      /* null far pointer                     */
#define NULLHP  (VOID HUGE *) 0     /* null huge pointer                    */
#define NULLXP  (VOID *) 0          /* null memory model-varying pointer    */

/* Error values, to test result of object manipulation macros.  Any error
 * condition is 0, any macro returns non-zero if OK.  Alloc macros are
 * constructed to return 0 upon error.  Source code may explicitly test for 
 * the alloc error define [e.g., "if ( SLF_ALLOC_OBJ(thing,10) == SL_ERR_ALLOC )"]
 * or just a non-zero result [e.g., "if ( !SLF_ALLOC_OBJ(thing,10) )"].
 * See note above about checking return values of locks, unlocks, and frees.
*/
#define SL_ERR_ALLOC    0
#define SL_ERR_LOCK     0
#define SL_ERR_UNLOCK   0
#define SL_ERR_FREE     0

/* Data types used for casting, defining data types, and declaring variables.
 * These are used in this file and in source code.
*/

#define SLF_PLG_OBJ     SLF_HDL_OBJ  /* Handle w/o embedded far pointer */
#define SLN_PLG_OBJ     SLN_HDL_OBJ  /* Handle w/o embedded near pointer */

#define SLH_PTR         HUGE *       /* Huge pointer */
#define SLF_PTR         FAR *        /* Far pointer */
#define SLN_PTR         NEAR *       /* Near pointer */
#define SLH_PTR_OBJ     VOID SLH_PTR /* Void Huge pointer */
#define SLF_PTR_OBJ     VOID SLF_PTR /* Void Far pointer */
#define SLN_PTR_OBJ     VOID SLN_PTR /* Void Near pointer */


/* Object Referencer manipulation macros.  Pass an OR variable to this 
 * macro so you can get at its data.  These appear in source code.
*/
#define SLH_USE_OBJ(o)  (o).hptr
#define SLF_USE_OBJ(o)  (o).fptr
#define SLN_USE_OBJ(o)  (o).nptr
#define SLF_IUSE_OBJ(o) (o).fiptr
#define SLN_IUSE_OBJ(o) (o).niptr
/* shorter versions of above - helps in readability.
*/
#define SLH_USE(o)      (o).hptr
#define SLF_USE(o)      (o).fptr
#define SLN_USE(o)      (o).nptr
#define SLF_IUSE(o)     (o).fiptr
#define SLN_IUSE(o)     (o).niptr

/*                  Defines that vary with the environment
*/


/* Defines for handle environments.
*/

#ifdef SLA_HDL_MM

#ifdef  SLA_HDLPTR_MM       /* Pointer Handle Types                         */
/* Defines for pointer handle environments. (mac)
*/


/* object declaration macros
*/
#define SLF_TYPE_OR(h,p)        struct{h fhdl;p fptr;}
#define SLN_TYPE_OR(h,p)        struct{h nhdl;p nptr;}
#define SLH_TYPDEF_OR(h,p,s)    typedef struct s {h hhdl;p hptr;}
#define SLF_TYPDEF_OR(h,p,s)    typedef struct s {h fhdl;p fptr;}
#define SLN_TYPDEF_OR(h,p,s)    typedef struct s {h nhdl;p nptr;}

#define SLF_ITYPE_OR(p)         struct{p fiptr;}
#define SLN_ITYPE_OR(p)         struct{p niptr;}
#define SLF_ITYPDEF_OR(p,s)     typedef struct s {p fiptr;}
#define SLP_ITYPDEF_OR(p,s)     typedef struct s {p iptr;}
#define SLN_ITYPDEF_OR(p,s)     typedef struct s {p niptr;}

#define SLF_PTYPE_OR(h,p)       struct{h fphdl;}
#define SLN_PTYPE_OR(h,p)       struct{h nphdl;}
#define SLF_PTYPDEF_OR(h,p,s)   typedef struct s {h fphdl;}
#define SLN_PTYPDEF_OR(h,p,s)   typedef struct s {h nphdl;}

#define SLP_TYPDEF_OR(h,p,s)  typedef struct s {h hdl;  p ptr;}
#define SLP_ITYPDEF_OR(p,s)   typedef struct s {p iptr;}
#define SLP_HDL_OBJ  Handle  /* the generic handle type */
#define SLP_PTR  /* Plain pointer */
/* #define SLP_HDLDEF  SLP_PTR SLP_PTR */


/* Test if object exists (has been allocated).
*/
#define SLH_TEST_OBJ(o)     ((o).hhdl != NULLHH )
#define SLF_TEST_OBJ(o)     ((o).fhdl != NULLFH )
#define SLN_TEST_OBJ(o)     ((o).nhdl != NULLNH )
#define SLF_ITEST_OBJ(o)    ((o).fiptr != NULLFP)   /* immediate far   */
#define SLN_ITEST_OBJ(o)    ((o).niptr != NULLNP)   /* immediate near  */
#define SLF_PTEST_OBJ(o)    ((o).fphdl != NULLFH)   /* plug far        */
#define SLN_PTEST_OBJ(o)    ((o).nphdl != NULLNH)   /* plug near       */

/* set-object-to-null macros.
 * N.B.: do not substitute the longhand assignment expressions below with
 * the debug macros such as SLF_NULPTR_DBG.  These macros must clear the
 * objects in debug AND non-debug mode.
*/
#define SLH_NULL_OBJ(o)     (o).hhdl = NULLHH, (o).hptr = NULLHP
#define SLF_NULL_OBJ(o)     (o).fhdl = NULLFH, (o).fptr = NULLFP
#define SLN_NULL_OBJ(o)     (o).nhdl = NULLNH, (o).nptr = NULLNP
#define SLF_INULL_OBJ(o)    (o).fiptr = NULLFP  /* immediate far */
#define SLN_INULL_OBJ(o)    (o).niptr = NULLNP  /* immediate near */
#define SLF_PNULL_OBJ(o)    (o).fphdl = NULLFH  /* plug far */
#define SLN_PNULL_OBJ(o)    (o).nphdl = NULLNH  /* plug near */


/* standard type/constant declarations
*/
#define NULLHH          NULLHP
#define NULLFH          NULLFP
#define NULLNH          NULLNP

#define SLF_HDL_OBJ     Handle          /* the generic handle type          */
#define SLN_HDL_OBJ     Handle
#define SLH_HDL_OBJ     Handle
#define SLP_HDL_OBJ     Handle


/* define the nature of this environment's handles.  handles in this 
 * environment are actually pointers to pointers.
*/
#define SLF_HDLDEF      SLF_PTR SLF_PTR
#define SLN_HDLDEF      SLN_PTR SLN_PTR
#define SLP_HDLDEF      SLN_PTR SLN_PTR

/* used for specifying 'handle to' types in-line.  t param is the type, such
 * as UCHAR.  Use this in place of a defined type.  For example, use
 * "SLF_TYPE_OR( SL_HDLTYP(UCHAR,SLF_HDLDEF), PFUCHAR ) orUCHAR;" to define
 * the variable orUCHAR as an object referencer, with the handle part of
 * type 'handle to a UCHAR'.
*/
#define SL_HDLTYP(t,hd)             t hd

/* used for 'handle to' typdefs.  t param is the type, such as UCHAR.  Use
 * these in place of a typedef statement.  For example, use
 * "SL_HDLTYPDEF(INT2B,SLF_HDLDEF,HINT2B);" to make HINT2B a type that is a 
 * far handle to an INT2B. t = type, hd = handle definition, ht = handle
 * type.
*/
#define SL_HDL_TYPDEF(t,hd,ht)      typedef t hd ht


/* object manipulation macros
*/
#define SLH_ALLOC_OBJ(o,s)  ((o).hhdl = (SLH_HDL_OBJ) NewHandle(s))
#define SLH_LOCK_OBJ(o)     HLock((SLH_HDL_OBJ) (o).hhdl),MemError() != noErr ? SL_ERR_LOCK : \
      ( (o).hptr = (SLH_PTR_OBJ)StripAddress((SLH_PTR_OBJ) *((o).hhdl) )) 
#define SLH_UNLOCK_OBJ(o)   (HUnlock((SLH_HDL_OBJ) (o).hhdl),MemError() == noErr)
#define SLH_FREE_OBJ(o)     ( DisposHandle((SLH_HDL_OBJ) (o).hhdl) )

#define SLF_ALLOC_OBJ(o,s)  ((o).fhdl = (SLF_HDL_OBJ) NewHandle(s))
#define SLF_LOCK_OBJ(o)     HLock((SLF_HDL_OBJ) (o).fhdl),MemError() != noErr ? SL_ERR_LOCK : \
      ( (o).fptr = (SLF_PTR_OBJ)StripAddress((SLF_PTR_OBJ) *((o).fhdl)) ) 
#define SLF_UNLOCK_OBJ(o)   (HUnlock((SLF_HDL_OBJ) (o).fhdl),MemError() == noErr)
#define SLF_FREE_OBJ(o)     ( DisposHandle((SLF_HDL_OBJ) (o).fhdl) )

#define SLN_ALLOC_OBJ(o,s)  ((o).nhdl = (SLN_HDL_OBJ) NewHandle(s))
#define SLN_LOCK_OBJ(o)     HLock((SLN_HDL_OBJ) (o).nhdl),MemError() != noErr ? SL_ERR_LOCK : \
      ( (o).nptr = (SLN_PTR_OBJ)StripAddress((SLN_PTR_OBJ) *((o).nhdl) ))
#define SLN_UNLOCK_OBJ(o)   (HUnlock((SLN_HDL_OBJ) (o).nhdl),MemError() == noErr)
#define SLN_FREE_OBJ(o)     ( DisposHandle((SLN_HDL_OBJ) (o).nhdl) )

#define SLF_IALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) NewPtr(s))
#define SLF_IFREE_OBJ(o)    ( DisposPtr((SLF_PTR_OBJ) (o).fiptr) )

#define SLN_IALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ) NewPtr(s))
#define SLN_IFREE_OBJ(o)    ( DisposPtr((SLN_PTR_OBJ) (o).niptr) )

#define SLF_PALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) NewHandle(s))
#define SLF_PLOCK_OBJ(o,t)   HLock((SLF_HDL_OBJ) (o).fphdl),MemError() != noErr ? SL_ERR_LOCK : \
      ( t = (SLF_PTR_OBJ)StripAddress((SLF_PTR_OBJ) *((o).fphdl)) )
#define SLF_PUNLOCK_OBJ(o,t) (HUnlock((SLF_HDL_OBJ) (o).fphdl),MemError() == noErr)
#define SLF_PFREE_OBJ(o)    ( DisposHandle((SLF_HDL_OBJ) (o).fphdl) )

#define SLN_PALLOC_OBJ(o,s) ((o).nphdl = (SLN_PLG_OBJ) NewHandle(s))
#define SLN_PLOCK_OBJ(o,t)   HLock((SLN_HDL_OBJ) (o).nphdl),MemError() != noErr ? SL_ERR_LOCK : \
      ( t = (SLN_PTR_OBJ)StripAddress((SLN_PTR_OBJ) *((o).nphdl)) )
#define SLN_PUNLOCK_OBJ(o,t) (HUnlock((SLN_HDL_OBJ) (o).nphdl),MemError() == noErr)
#define SLN_PFREE_OBJ(o)    ( DisposHandle((SLN_HDL_OBJ) (o).nphdl) )

#define SLF_REALLOC_OBJ(o,s) ( HUnlock((o).fhdl), ((SetHandleSize((o).fhdl, (Size) s), \
      (MemError() ==noErr)) ? (MoveHHi((o).fhdl), SLF_LOCK_OBJ(o)) : SL_ERR_ALLOC) ) 

/* Clearing object allocating macros
*/
#define SLF_CALLOC_OBJ(o,s)  ((o).fhdl = (SLF_HDL_OBJ) NewHandleClear(s))
#define SLN_CALLOC_OBJ(o,s)  ((o).nhdl = (SLN_HDL_OBJ) NewHandleClear(s))
#define SLF_ICALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) NewPtrClear(s))
#define SLN_ICALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ) NewPtrClear(s))
#define SLF_PCALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) NewHandleClear(s))
#define SLN_PCALLOC_OBJ(o,s) ((o).nphdl = (SLN_PLG_OBJ) NewHandleClear(s))

/* Object Referencer handle manipulation macros.  Pass an OR variable to this 
 * macro so you can get at its handle.  These appear in source code.
*/
#define SLH_USE_HDL(o)  (o).hhdl
#define SLF_USE_HDL(o)  (o).fhdl
#define SLN_USE_HDL(o)  (o).nhdl
#define SLF_PUSE_HDL(o) (o).fphdl
#define SLN_PUSE_HDL(o) (o).nphdl

#else                       /* Scalar Handle Types                          */
/* Defines for Scaler handle environments - (windows)
*/


/* Object declaration macros.
 *
 * No handles for near objects - everything near is fixed in Windows.  Near
 * plugs use a pointer rather than a handle.
*/
#define SLF_TYPE_OR(h,p)         struct{h fhdl;p fptr;}
#define SLN_TYPE_OR(h,p)         struct{p nptr;}
#define SLH_TYPDEF_OR(h,p,s)     typedef struct s {h hhdl;p hptr;}
#define SLF_TYPDEF_OR(h,p,s)     typedef struct s {h fhdl;p fptr;}
#define SLN_TYPDEF_OR(h,p,s)     typedef struct s {p nptr;}

#define SLF_ITYPE_OR(p)          struct{p fiptr;}
#define SLN_ITYPE_OR(p)          struct{p niptr;}
#define SLF_ITYPDEF_OR(p,s)      typedef struct s {p fiptr;}
#define SLN_ITYPDEF_OR(p,s)      typedef struct s {p niptr;}
#define SLP_ITYPDEF_OR(p,s)      typedef struct s {p iptr;}


#define SLF_PTYPE_OR(h,p)        struct{h fphdl;}
#define SLN_PTYPE_OR(h,p)        struct{p npptr;}
#define SLF_PTYPDEF_OR(h,p,s)    typedef struct s {h fphdl;}
#define SLN_PTYPDEF_OR(h,p,s)    typedef struct s {h npptr;}
#define SLP_TYPDEF_OR(h,p,s)     typedef struct s {h hdl;  p ptr;}


/* Test if object exists (has been allocated).
*/
#define SLH_TEST_OBJ(o)     ((o).hhdl != NULLHH )
#define SLF_TEST_OBJ(o)     ((o).fhdl != NULLFH )
#define SLN_TEST_OBJ(o)     ((o).nptr != NULLNP )
#define SLF_ITEST_OBJ(o)    ((o).fiptr != NULLFP)   /* immediate far   */
#define SLN_ITEST_OBJ(o)    ((o).niptr != NULLNP)   /* immediate near  */
#define SLF_PTEST_OBJ(o)    ((o).fphdl != NULLFH)   /* plug far        */
#define SLN_PTEST_OBJ(o)    ((o).npptr != NULLNH)   /* plug near       */

/* set-object-to-null macros.
 * N.B.: do not substitute the longhand assignment expressions below with
 * the debug macros such as SLF_NULPTR_DBG.  These macros must clear the
 * objects in debug AND non-debug mode.
*/
#define SLH_NULL_OBJ(o)     (o).hhdl = NULLHH, (o).hptr = NULLHP
#define SLF_NULL_OBJ(o)     (o).fhdl = NULLFH, (o).fptr = NULLFP
#define SLN_NULL_OBJ(o)     (o).nptr = NULLNP
#define SLF_INULL_OBJ(o)    (o).fiptr = NULLFP  /* immediate far */
#define SLN_INULL_OBJ(o)    (o).niptr = NULLNP  /* immediate near */
#define SLF_PNULL_OBJ(o)    (o).fphdl = NULLFH  /* plug far */
#define SLN_PNULL_OBJ(o)    (o).npptr = NULLNH  /* plug near */


/* standard type/constant declarations
*/
#define NULLHH          NULLS
#define NULLFH          NULLS
#define NULLNH          NULLS

#define SLH_HDL_OBJ     HANDLE              /* the generic handle type      */
#define SLF_HDL_OBJ     HANDLE
#define SLP_HDL_OBJ     HANDLE
#define SLN_HDL_OBJ     LOCALHANDLE

/* Define the nature of this environment's handles.  Handles in this 
 * environment are scalars - not true declarators & so can't be used to make
 * handles to specific types.  All handles are the same, then: SLF_HDL_OBJ
 * or SLN_HDL_OBJ.
 * 
*/
#define SLF_HDLDEF      SLF_HDL_OBJ
#define SLN_HDLDEF      SLN_HDL_OBJ
#define SLP_HDLDEF      SLP_HDL_OBJ

/* used for specifying 'handle to' types in-line.  See description in 
 * pointer-type handles.
 * 
 * Note: the t param is ignored, because hd can't be used as a type modifier
 * since it's not a true declarator like * is.  Net effect: all handle types
 * will be the same, the generic handle type.
*/
#define SL_HDLTYP(t,hd)             hd
/* used for 'handle to' typdefs.  See note above Re: ignorance of t param.
*/
#define SL_HDL_TYPDEF(t,hd,ht)      typedef hd ht

/* Object Referencer handle manipulation macros.  Pass an OR variable to this 
 * macro so you can get at its handle.  These appear in source code.
*/
#define SLH_USE_HDL(o)  (o).hhdl
#define SLF_USE_HDL(o)  (o).fhdl
#define SLN_USE_HDL(o)  (o).nptr
#define SLF_PUSE_HDL(o) (o).fphdl
#define SLN_PUSE_HDL(o) (o).npptr

/* When true virtual memory management is available on a machine, handles are
 * not necessary for memory management (handles are a software kludge to 
 * provide memory management on non-virtual machines).  However, they are 
 * still required for interface with the Windows API memory management 
 * functions.  With true virtual memmgt. the handles can be locked at
 * allocation time & they can remain locked for the duration of the handle's
 * life.  
 * Therefore, the alloc and lock operations are combined in the alloc macro, 
 * lock and unlock operations disappear, and unlock & free operations are
 * combined in the free macro, except where noted.
*/
#ifdef SLA_VIRTMEM_AM

/* object manipulation macros
*/
#define SLH_ALLOC_OBJ(o,s)  (((o).hhdl = (SLH_HDL_OBJ) GlobalAlloc( (WORD) GHND, s )) != NULLS \
    ? (o).hptr = (SLH_PTR_OBJ) GlobalLock( (SLH_HDL_OBJ) (o).hhdl ) : SL_ERR_ALLOC )
#define SLH_LOCK_OBJ(o)
#define SLH_UNLOCK_OBJ(o)
#define SLH_FREE_OBJ(o)     ( GlobalUnlock( (SLH_HDL_OBJ) (o).hhdl ), \
    GlobalFree( (SLH_HDL_OBJ) (o).hhdl ) )

#define SLF_ALLOC_OBJ(o,s)  (((o).fhdl = (SLF_HDL_OBJ) GlobalAlloc( (WORD) GHND, s )) != NULLS \
    ? (o).fptr = (SLF_PTR_OBJ) GlobalLock( (SLF_HDL_OBJ) (o).fhdl ) : SL_ERR_ALLOC )
#define SLF_LOCK_OBJ(o)
#define SLF_UNLOCK_OBJ(o)
#define SLF_FREE_OBJ(o)     ( GlobalUnlock( (SLF_HDL_OBJ) (o).fhdl ), \
    GlobalFree( (SLF_HDL_OBJ) (o).fhdl ) )

#define SLN_ALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR, s ))
#define SLN_LOCK_OBJ(o)
#define SLN_UNLOCK_OBJ(o)
#define SLN_FREE_OBJ(o)     (LocalFree( (SLN_HDL_OBJ) (o).nptr ))

/* Same as untrue virtual memory version
*/
#define SLF_IALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLF_IFREE_OBJ(o)    (Cffree( (SLF_PTR_OBJ) (o).fiptr ))

/* Same as untrue virtual memory version
*/
#define SLN_IALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc((WORD) LPTR, s ))
#define SLN_IFREE_OBJ(o)    (LocalFree( (SLN_HDL_OBJ) (o).niptr ))

/* Plugs have only a handle.  If the plug is locked at alloc time, there's no
 * place to store the result of the lock.  Therefore, same as untrue virtual
 * memory version.  JCCRE32
*/
#define SLF_PALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) GlobalAlloc((WORD) GHND, s ))
#define SLF_PLOCK_OBJ(o,t)  (t = (SLF_PTR_OBJ) GlobalLock( (SLF_PLG_OBJ) (o).fphdl ))
#define SLF_PUNLOCK_OBJ(o,t) (GlobalUnlock( (SLF_PLG_OBJ) (o).fphdl ))
#define SLF_PFREE_OBJ(o)     (GlobalFree( (SLF_PLG_OBJ) (o).fphdl ))


/* Same as untrue virtual memory version
*/
#define SLN_PALLOC_OBJ(o,s) ((o).npptr = (SLN_PLG_OBJ)(NPSTR)LocalAlloc((WORD) LPTR, s ))
#define SLN_PLOCK_OBJ(o,t)  (t = (SLN_PTR_OBJ)(NPSTR)(o).npptr)
#define SLN_PUNLOCK_OBJ(o,t)
#define SLN_PFREE_OBJ(o)     (LocalFree( (SLN_PLG_OBJ) (o).npptr ))

/* Clearing object allocating macros
*/
#define SLF_REALLOC_OBJ(o,s)  (((o).fhdl = (SLF_HDL_OBJ) GlobalReAlloc((SLF_HDL_OBJ) (o).fhdl , (WORD) s,  GHND | GMEM_ZEROINIT)) != NULLS \
    ? (o).fptr = (SLF_PTR_OBJ) GlobalLock( (SLF_HDL_OBJ) (o).fhdl ) : SL_ERR_ALLOC )
#define SLF_CALLOC_OBJ(o,s)  (((o).fhdl = (SLF_HDL_OBJ) GlobalAlloc( (WORD) GHND | GMEM_ZEROINIT, s )) != NULLS \
    ? (o).fptr = (SLF_PTR_OBJ) GlobalLock( (SLF_HDL_OBJ) (o).fhdl ) : SL_ERR_ALLOC )
#define SLN_CALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))    /* Same as untrue virt. version */
#define SLF_ICALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( s, 1 ))                               /* Same as untrue virt. version */
#define SLN_ICALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))   /* Same as untrue virt. version */
#define SLF_PCALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) GlobalAlloc( (WORD) GHND | GMEM_ZEROINIT, s ))  /* Same as untrue virt. version */
#define SLN_PCALLOC_OBJ(o,s) ((o).npptr = (SLN_PLG_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))   /* Same as untrue virt. version */


#else   /* SLA_VIRTMEM_AM */


/* object manipulation macros
*/
#define SLH_ALLOC_OBJ(o,s)  ((o).hhdl = (SLH_HDL_OBJ) GlobalAlloc( (WORD) GHND, s ))
#define SLH_LOCK_OBJ(o)     ((o).hptr = (SLH_PTR_OBJ) GlobalLock( (SLH_HDL_OBJ) (o).hhdl ))
#define SLH_UNLOCK_OBJ(o)   (GlobalUnlock( (SLH_HDL_OBJ) (o).hhdl ))
#define SLH_FREE_OBJ(o)     (GlobalFree( (SLH_HDL_OBJ) (o).hhdl ))

#define SLF_ALLOC_OBJ(o,s)  ((o).fhdl = (SLF_HDL_OBJ) GlobalAlloc( (WORD) GHND, s ))
#define SLF_LOCK_OBJ(o)     ((o).fptr = (SLF_PTR_OBJ) GlobalLock( (SLF_HDL_OBJ) (o).fhdl ))
#define SLF_UNLOCK_OBJ(o)   (GlobalUnlock( (SLF_HDL_OBJ) (o).fhdl ))
#define SLF_FREE_OBJ(o)     (GlobalFree( (SLF_HDL_OBJ) (o).fhdl ))

#define SLN_ALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR, s ))
#define SLN_LOCK_OBJ(o)
#define SLN_UNLOCK_OBJ(o)
#define SLN_FREE_OBJ(o)     (LocalFree( (SLN_HDL_OBJ) (o).nptr ))

#define SLF_IALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLF_IFREE_OBJ(o)    (Cffree( (SLF_PTR_OBJ) (o).fiptr ))

#define SLN_IALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR, s ))
#define SLN_IFREE_OBJ(o)    (LocalFree( (SLN_HDL_OBJ) (o).niptr ))

#define SLF_PALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) GlobalAlloc( (WORD) GHND, s ))
#define SLF_PLOCK_OBJ(o,t)  (t = (SLF_PTR_OBJ) GlobalLock( (SLF_PLG_OBJ) (o).fphdl ))
#define SLF_PUNLOCK_OBJ(o,t) (GlobalUnlock( (SLF_PLG_OBJ) (o).fphdl ))
#define SLF_PFREE_OBJ(o)     (GlobalFree( (SLF_PLG_OBJ) (o).fphdl ))

/* Local plugs are alloc fixed and the lock macro simply assigns the OR's
 * pointer to the local pointer.  The unlock does nothing.  Local plugs have
 * a pointer member, not a handle.
*/
#define SLN_PALLOC_OBJ(o,s) ((o).npptr = (SLN_PLG_OBJ)(NPSTR)LocalAlloc((WORD) LPTR, s ))
#define SLN_PLOCK_OBJ(o,t)  (t = (SLN_PTR_OBJ)(NPSTR)(o).npptr)
#define SLN_PUNLOCK_OBJ(o,t)
#define SLN_PFREE_OBJ(o)     (LocalFree( (SLN_PLG_OBJ) (o).npptr ))

/* Clearing object allocating macros
*/
#define SLF_REALLOC_OBJ(o,s)  ((o).fhdl = (SLF_HDL_OBJ) GlobalReAlloc((SLF_HDL_OBJ) (o).fhdl ,(WORD) s,  GHND | GMEM_ZEROINIT))
#define SLF_CALLOC_OBJ(o,s)  ((o).fhdl = (SLF_HDL_OBJ) GlobalAlloc( (WORD) GHND | GMEM_ZEROINIT, s ))
#define SLN_CALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))
#define SLF_ICALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( s, 1 ))
#define SLN_ICALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))
#define SLF_PCALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) GlobalAlloc( (WORD) GHND | GMEM_ZEROINIT, s ))
#define SLN_PCALLOC_OBJ(o,s) ((o).npptr = (SLN_PLG_OBJ)(NPSTR)LocalAlloc( (WORD) LPTR | LMEM_ZEROINIT, s ))

#endif  /* else of ifdef SLA_VIRTMEM_AM */


#endif  /* ifdef SLA_HDLPTR_MM */

#else                       /* pointer-only environments                    */
/* Defines for pointer-only environments.
*/


/* standard type/constant declarations
*/
#define NULLHH          NULLHP
#define NULLFH          NULLFP
#define NULLNH          NULLNP

#define SLH_HDL_OBJ     VOID HUGE *         /* used to coerce, generic hddl */
#define SLF_HDL_OBJ     VOID FAR *
#define SLN_HDL_OBJ     VOID NEAR *
#define SLP_HDL_OBJ     VOID *

/* Define the nature of this environment's handles.  Since this environment
 * doesn't really have handles, void is substituted.  That is, a handle is
 * a pointer to a void.  All handles are the same, then: SLF_HDL_OBJ
 * or SLN_HDL_OBJ.
 * 
*/
#define SLF_HDLDEF      SLF_HDL_OBJ
#define SLN_HDLDEF      SLN_HDL_OBJ
#define SLP_HDLDEF      SLP_HDL_OBJ

/* used for specifying 'handle to' types in-line.  See description in 
 * pointer-type handles.
 * 
 * Note: the t param is ignored, because hd can't be used as a type modifier
 * since it's not a true declarator like * is.  Net effect: all handle types
 * will be the same, the generic handle type.
*/
#define SL_HDLTYP(t,hd)             hd

/* used for 'handle to' typdefs.
 * See note on t param above.
*/
#define SL_HDL_TYPDEF(t,hd,ht)      typedef hd ht   /* to define type       */


/* object declaration macros.  Ignore the handle type parameter.
*/
#define SLF_TYPE_OR(h,p)         struct{p fptr;}
#define SLN_TYPE_OR(h,p)         struct{p nptr;}
#define SLH_TYPDEF_OR(h,p,s)     typedef struct s {p hptr;}
#define SLF_TYPDEF_OR(h,p,s)     typedef struct s {p fptr;}
#define SLN_TYPDEF_OR(h,p,s)     typedef struct s {p nptr;}

#define SLF_ITYPE_OR(p)          struct{p fiptr;}
#define SLN_ITYPE_OR(p)          struct{p niptr;}
#define SLF_ITYPDEF_OR(p,s)      typedef struct s {p fiptr;}
#define SLN_ITYPDEF_OR(p,s)      typedef struct s {p niptr;}
#define SLP_ITYPDEF_OR(p,s)      typedef struct s {p iptr;}

#define SLF_PTYPE_OR(h,p)        struct{p fphdl;}
#define SLN_PTYPE_OR(h,p)        struct{p nphdl;}
#define SLF_PTYPDEF_OR(h,p,s)    typedef struct s {p fphdl;}
#define SLN_PTYPDEF_OR(h,p,s)    typedef struct s {p nphdl;}
                     
/* test if object exists (has been allocated).
*/
#define SLH_TEST_OBJ(o)        ((o).hptr != NULLHP)
#define SLF_TEST_OBJ(o)        ((o).fptr != NULLFP)
#define SLN_TEST_OBJ(o)        ((o).nptr != NULLNP)
#define SLF_ITEST_OBJ(o)       ((o).fiptr != NULLFP)  /* immediate far */
#define SLN_ITEST_OBJ(o)       ((o).niptr != NULLNP)  /* immediate near */
#define SLF_PTEST_OBJ(o)       ((o).fphdl != NULLFH)  /* plug far */
#define SLN_PTEST_OBJ(o)       ((o).nphdl != NULLNH)  /* plug near */

/* set-object-to-null macros.
 * N.B.: do not substitute the longhand assignment expressions below with
 * the debug macros such as SLF_NULPTR_DBG.  These macros must clear the
 * objects in debug AND non-debug mode.
*/
#define SLH_NULL_OBJ(o)        (o).hptr = NULLHP
#define SLF_NULL_OBJ(o)        (o).fptr = NULLFP
#define SLN_NULL_OBJ(o)        (o).nptr = NULLNP
#define SLF_INULL_OBJ(o)       (o).fiptr = NULLFP     /* immediate far */
#define SLN_INULL_OBJ(o)       (o).niptr = NULLNP     /* immediate near */
#define SLF_PNULL_OBJ(o)       (o).fphdl = NULLFH     /* plug far */
#define SLN_PNULL_OBJ(o)       (o).nphdl = NULLNH     /* plug near */


/* object manipulation macros
*/

#define SLH_ALLOC_OBJ(o,s)  ((o).hptr = (SLH_PTR_OBJ) Challoc( s, 1 ))
#define SLH_LOCK_OBJ(o)
#define SLH_UNLOCK_OBJ(o)
#define SLH_FREE_OBJ(o)     (Chfree( (SLH_PTR_OBJ) (o).hptr ))

#define SLF_ALLOC_OBJ(o,s)  ((o).fptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLF_LOCK_OBJ(o)
#define SLF_UNLOCK_OBJ(o)
#define SLF_FREE_OBJ(o)     (Cffree( (SLF_PTR_OBJ) (o).fptr ))

#define SLN_ALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ) Cnmalloc( s ))
#define SLN_LOCK_OBJ(o)
#define SLN_UNLOCK_OBJ(o)
#define SLN_FREE_OBJ(o)     (free( (SLN_PTR_OBJ) (o).nptr ))

#define SLF_IALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLF_IFREE_OBJ(o)    (Cffree( (SLF_PTR_OBJ) (o).fiptr ))

#define SLN_IALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ) Cnmalloc( s ))
#define SLN_IFREE_OBJ(o)    (Cnfree( (SLN_PTR_OBJ) (o).niptr ))

#define SLF_PALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) Cfcalloc( s, 1 ))
#define SLF_PLOCK_OBJ(o,t)  (t = (o).fphdl)
#define SLF_PUNLOCK_OBJ(o,t)
#define SLF_PFREE_OBJ(o)     (Cffree( (o).fphdl ))

#define SLN_PALLOC_OBJ(o,s) ((o).nphdl = (SLN_PLG_OBJ) Cnmalloc( s ))
#define SLN_PLOCK_OBJ(o,t)  (t = (o).nphdl)
#define SLN_PUNLOCK_OBJ(o,t)
#define SLN_PFREE_OBJ(o)    (Cnfree( (o).nphdl ))

/* Clearing object allocating macros
*/
#define SLF_REALLOC_OBJ(o,s)  ((o).fptr = (SLF_PTR_OBJ) Cfrealloc( o.fptr, s ))
#define SLF_CALLOC_OBJ(o,s)  ((o).fptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLN_CALLOC_OBJ(o,s)  ((o).nptr = (SLN_PTR_OBJ) Cnmalloc( s ),(o).nptr == NULLNP ? SL_ERR_ALLOC : Cmemset((o).nptr,'\0',s))
#define SLF_ICALLOC_OBJ(o,s) ((o).fiptr = (SLF_PTR_OBJ) Cfcalloc( 1, s ))
#define SLN_ICALLOC_OBJ(o,s) ((o).niptr = (SLN_PTR_OBJ) Cnmalloc( s ),(o).niptr == NULLNP ? SL_ERR_ALLOC \
    : Cmemset((o).niptr,'\0',s))
#define SLF_PCALLOC_OBJ(o,s) ((o).fphdl = (SLF_PLG_OBJ) Cfcalloc( s, 1 ))
#define SLN_PCALLOC_OBJ(o,s) ((o).nphdl = (SLN_PLG_OBJ) Cnmalloc( s ),(o).nphdl == NULLNP ? SL_ERR_ALLOC \
    : Cmemset((o).nphdl,'\0',s))

/* Object Referencer handle manipulation macros.  Pass an OR variable to this 
 * macro so you can get at its handle.  These appear in source code.
*/

#define SLH_USE_HDL(o)  (o).hptr
#define SLF_USE_HDL(o)  (o).fptr
#define SLN_USE_HDL(o)  (o).nptr
#define SLF_PUSE_HDL(o) (o).fphdl
#define SLN_PUSE_HDL(o) (o).nphdl

#endif  /* else of ifdef SLA_HDL_MM */



/* Redefine any of the above macros for a debug version if the debug CTO
 * is defined.
*/

#ifdef SL_MEM_DEBUG
#include "slmemdbg.h"
#endif

/* Redefine underlying allocs and frees to reroute through integrity-checking
 * routines if that debug CTO is defined.
*/
#ifdef SL_MEM_UDEBUG

/* Distance-independent (no near/far distinction) alloc/free functions
*/
#undef  Ccalloc
#undef  Cmalloc
#undef  Cfree

#define Ccalloc(s,n)    SLud_calloc(s,n)
#define Cmalloc         SLud_malloc
#define Cfree           SLud_free

/* Far mem functions.  Define far or plain depending on segmented 
 * architecture.
*/
#undef  Cfcalloc
#undef  Cfmalloc
#undef  Cffree

#ifdef  SLA_SEGMENT_AM
#define Cfcalloc(s,n)   SLud_fcalloc(s,n)
#define Cfmalloc        SLud_fmalloc
#define Cffree          SLud_ffree
#else
#define Cfcalloc(s,n)   SLud_calloc(s,n)
#define Cfmalloc        SLud_malloc
#define Cffree          SLud_free
#endif  /* end SLA_SEGMENT_AM */

/* Huge mem functions.  Define huge or plain depending on segmented 
 * architecture.
 */
#undef  Challoc
#undef  Chfree

#ifdef SLA_HUGEPTR_DT
#define Challoc         SLud_halloc
#define Chfree          SLud_hfree
#else
#define Challoc         SLud_calloc
#define Chfree          SLud_free
#endif  /* end SLA_HUGEPTR_DT */

#endif  /* end SL_MEM_UDEBUG */

#endif /* SLMEMMAN_H */

/* End of file - slmemman.h */

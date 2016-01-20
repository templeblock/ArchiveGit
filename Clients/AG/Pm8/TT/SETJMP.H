/*
$Header: /PM8/TT/SETJMP.H 1     3/03/99 6:35p Gbeddow $
$Log: /PM8/TT/SETJMP.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:15:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:45:20   Fred
// Initial revision.
// 
//    Rev 1.1   01 Jul 1997 09:08:30   Fred
// More changes for PMG 4.0
// 
//    Rev 1.0   14 Mar 1996 13:21:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:42:04   FRED
// Initial revision.
// 
//    Rev 1.0   29 Sep 1995 13:37:08   FRED
// Initial revision.
 * 
 *    Rev 1.0   10 Jul 1992 10:54:16   ADAM
 * Initial revision.
*/

/*  setjmp.h

    Defines typedef and functions for setjmp/longjmp.

    Copyright (c) 1987, 1991 by Borland International
    All Rights Reserved.
*/

#ifndef __SETJMP_H__
#define __SETJMP_H__

#if !defined( __DEFS_H )
#include <_defs.h>
#endif

typedef struct __jmp_buf {
    unsigned    j_sp;
    unsigned    j_ss;
    unsigned    j_flag;
    unsigned    j_cs;
    unsigned    j_ip;
    unsigned    j_bp;
    unsigned    j_di;
    unsigned    j_es;
    unsigned    j_si;
    unsigned    j_ds;
}   jmp_buf[1];

#ifdef __cplusplus
extern "C" {
#endif

#if !defined( _Windows )
void    _CType longjmp(jmp_buf __jmpb, int __retval);
int     _CType setjmp(jmp_buf __jmpb);
#else
void far pascal longjmp( struct __jmp_buf far *__jmpb, int __retval );
int far pascal setjmp( struct __jmp_buf far *__jmpb);
#endif

#ifdef __cplusplus
}
#endif

#endif


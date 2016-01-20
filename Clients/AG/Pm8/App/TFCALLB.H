/*
// $Header: /PM8/App/TFCALLB.H 1     3/03/99 6:12p Gbeddow $
//
// Definitions for typeface callback.
// Included by (at least) PMGDB.H and TFENGINE.H
//
// $Log: /PM8/App/TFCALLB.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:54   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:04   JAY
// Initial revision.
// 
//    Rev 1.0   01 Jun 1994 16:44:06   JAY
// Initial revision.
*/

#ifndef __TFCALLB_H__
#define __TFCALLB_H__

typedef int (*tTypefaceCallback)(int, LPLOGFONT, StorageFilePtr);
const int TC_RESULT_failure = 0;
const int TC_RESULT_file = 1;
const int TC_RESULT_logfont = 2;

#endif						/* #ifndef __TFCALLB_H__ */

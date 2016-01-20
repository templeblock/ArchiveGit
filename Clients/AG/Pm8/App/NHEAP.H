/*
// NHEAP.H
//
// Copyright (C) 1993 MicroLogic Software, Inc.
//
// $Header: /PM8/App/NHEAP.H 1     3/03/99 6:08p Gbeddow $
// $Log: /PM8/App/NHEAP.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:16   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:46   JAY
// Initial revision.
// 
//    Rev 1.0   25 Feb 1994 17:07:02   MACDUFF
// Initial revision.
*/

extern int heap_init(void FAR *heap, uint16 size);
extern void FAR *heap_alloc(void FAR *heap, uint16 size);
extern void heap_free(void FAR *heap, void FAR *block);
extern uint16 heap_available(void FAR *heap);
extern BOOL heap_valid(void FAR *heap);


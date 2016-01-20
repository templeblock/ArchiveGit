/*
// FLEXMATH.H
//
//		Copyright (c) 1993 MicroLogic Software, Inc.
// 
//	Interface to "flex-point" math in PrintMaster Gold.
//
// $Log: /PM8/App/FLEXMATH.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:46   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:32   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:18   JAY
//  
// 
//    Rev 1.0   02 Jun 1993 23:51:24   MACDUFF
// Initial revision.
//
// $Header: /PM8/App/FLEXMATH.H 1     3/03/99 6:05p Gbeddow $
*/

typedef uint32 FLEX;

extern VOID FlexToFrac(FLEX aFlex, uint16 *aNum, uint16 *aDenom);
extern FLEX ShiftedLongToFlex(uint32 aLong, uint16 aShift);
extern FLEX ScaleFlex(FLEX aFlex, uint16 aNum, uint16 aDenom);
extern VOID SplitFlex(FLEX aFlex, USHORT *aIntPart, USHORT *aFracPart);
extern FLEX UnsplitFlex(USHORT aIntPart, USHORT aFracPart);

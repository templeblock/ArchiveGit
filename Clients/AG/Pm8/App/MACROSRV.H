/*
// $Workfile: MACROSRV.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MACROSRV.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     3/18/98 11:14a Jayn
// Page numbers
// 
//    Rev 1.0   14 Aug 1997 15:22:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:42:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:05:48   FRED
// Initial revision.
// 
//    Rev 1.2   02 May 1995 15:37:50   JAY
// Improved macro support.
// 
//    Rev 1.1   01 May 1995 15:24:46   JAY
// Restructured the macro stuff
// 
//    Rev 1.0   01 May 1995 08:42:26   JAY
// Initial revision.
// 
//    Rev 2.0   25 Apr 1995 09:02:02   JAY
// Initial revision.
*/

class CMacroServer;

#ifndef __MACROSRV_H__
#define __MACROSRV_H__

class MacroListRecord;

/*
// Abstract class which supports macros ("placeholders" / "smart fields").
*/

class CMacroServer
{
public:
	virtual ~CMacroServer() {}
	virtual CHARACTER* GetMacroText(CHARACTER c, DB_RECORD_NUMBER lPage = 0) = 0;

	virtual MacroListRecord* GetMacroList(void) = 0;
};

#endif				// #ifndef __MACROSRV_H__

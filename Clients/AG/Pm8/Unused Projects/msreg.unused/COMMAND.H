/*
// $Workfile: COMMAND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/COMMAND.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:00   FRED
// Initial revision.
// 
//    Rev 1.0   16 Nov 1995 10:31:34   FRED
//  
*/

#ifndef __COMMAND_H__
#define __COMMAND_H__

class CCommandParser
{
public:
	CCommandParser(void);
	~CCommandParser(void);

public:
	BOOL Init(LPCSTR pszString);

	LPCSTR Command(void);
	BOOL Option(char cOption);

protected:
	LPSTR m_pszCommand;
	LPSTR m_pszOptions;
};

#endif

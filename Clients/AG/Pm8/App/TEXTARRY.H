/*
// TEXTARRY.H
//
//		Copyright (c) 1994 MicroLogic Software, Inc.
//
//		Objects to manage arrays of text records whose size has changed since
// PMW 1.00.06.
//
// $Header: /PM8/App/TEXTARRY.H 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TEXTARRY.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:42:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:12   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:02   JAY
// Initial revision.
// 
//    Rev 1.0   16 Mar 1994 16:04:04   MACDUFF
// Initial revision.
*/

#ifndef _TEXTARRY_H_
#define _TEXTARRY_H_

#include "spltarry.h"
#include "text.h"

class TextWordArray : public SplitArray
{
public:
	TextWordArray(SHORT first_element = 0) :
		SplitArray(sizeof (TEXT_WORD), SizeOfOldTextWord(), first_element)
		{}

	virtual void set_new_data_defaults();
};

class TextLineArray : public SplitArray
{
public:
	TextLineArray(SHORT first_element = 0) :
		SplitArray(sizeof (LINE), SizeOfOldLine(), first_element)
		{}

	virtual void set_new_data_defaults();
};

#endif



/*
// TEXTARRY.CPP
//
//		Copyright (c) 1994 MicroLogic Software, Inc.
//
//		Code to manage arrays of text records whose size has changed since
// PMW 1.00.06.
//
// $Header: /PM8/App/TEXTARRY.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TEXTARRY.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:36   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:12   JAY
// Initial revision.
// 
//    Rev 1.1   30 Nov 1994 16:38:18   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   16 Mar 1994 16:04:06   MACDUFF
// Initial revision.
*/

#include "stdafx.h"

#include <string.h>

#include "error.h"
#include "pagedefs.h"
#include "spltarry.h"
#include "textarry.h"
#include "text.h"

void TextWordArray::set_new_data_defaults()
{
	TEXT_WORD_PTR wp;
	int i;

	for (i = 0, wp = (TEXT_WORD_PTR)data; i < count(); i++, wp++)
	{
		wp->line_spacing = wp->ascend + wp->descend;
	}
}	

void TextLineArray::set_new_data_defaults()
{
	LINE_PTR lp;
	int i;

	for (i = 0, lp = (LINE_PTR)data; i < count(); i++, lp++)
	{
		lp->line_spacing = lp->ascend + lp->descend;
	}
}	

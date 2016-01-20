/*
// $Header: /PM8/App/COLOR.CPP 1     3/03/99 6:04p Gbeddow $
//
// Implementation for color list objects.
//
// $Log: /PM8/App/COLOR.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:19:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:34   FRED
// Initial revision.
// 
//    Rev 2.2   16 Jan 1996 09:42:34   JAY
// Fixed color reading problem (32-bit verdsion). I think it's a compiler bug.
// 
//    Rev 2.1   08 Feb 1995 13:35:46   JAY
// Reverted. New series.
// 
//    Rev 1.2   13 May 1994 13:36:36   JAY
// Now sets a read buffer so it doesn't read the file 1 byte at a time.
// 
//    Rev 1.1   14 Feb 1994 11:29:06   JAY
// Made system_color_list near.
# 
#    Rev 1.0   16 Dec 1993 16:28:32   JAY
#  
*/

#include "stdafx.h"
#include "pmw.h"

#include "system.h"
#include "color.h"
#include "file.h"

/*
// The global color list.
*/

PUBLIC ColorList near system_color_list;

/************************/
/* ColorElement methods */
/************************/

/*
// Color element constructor.
*/

ColorElement::ColorElement(LPCSTR pszName, COLOR color)
{
	m_csName = pszName;
	m_color = color;
}

/*
// Color element destructor.
*/

ColorElement::~ColorElement()
{
}

/*********************/
/* ColorList methods */
/*********************/

/*
// Color list constructor.
*/

ColorList::ColorList()
	: List(0, TRUE)
{
}

/*
// Read an error list from a specified file.
*/

ERRORCODE ColorList::read(LPCSTR pszName)
{
	TextReadFile file(pszName);
	CHAR buffer[80];
	ERRORCODE error;

	file.set_read_buffer(2048);

	while ((error = file.read_line(buffer,
									sizeof(buffer),
 									TRF_skip_blank_lines | TRF_skip_comments)) == ERRORCODE_None)
	{
		int cyan, magenta, yellow, black;

		if (sscanf(buffer, "%d %d %d %d;",
					&cyan, &magenta, &yellow, &black) < 4)
		{
			break;
		}

		char *p;

		if ((p = strchr(buffer, ';')) == NULL)
		{
			break;
		}

	/* Create the color. */

		ColorElementPtr element;

		if ((element = new ColorElement(p+1, MAKE_COLOR(cyan, magenta, yellow, black))) != NULL)
		{
		/* Add it to the list. */
			append(element);
		}
		else
		{
		/* Bag outta' here. */
			error = ERRORCODE_Memory;
			break;
		}
	}

/* Only good error is Read. */

	return error == ERRORCODE_Read
 					? ERRORCODE_None
					: error;
}

/*
// $Header: /PM8/App/VFL.CPP 1     3/03/99 6:13p Gbeddow $
//
// Virtual file list object implementation.
// This is the object that manages the VFL file.
//
// $Log: /PM8/App/VFL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:20   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 10:31:52   Jay
// Changes from 2.1
// 
//    Rev 2.3   27 Feb 1996 16:46:44   JAY
// Wasn't setting the version by default.
// 
//    Rev 2.2   31 Jan 1996 08:21:22   JAY
// More configuration stuff
// 
//    Rev 2.1   08 Feb 1995 13:35:22   JAY
// Reverted. New series.
// 
//    Rev 1.4   04 Aug 1994 07:35:08   JAY
// Large model and 32-bit changes
// 
//    Rev 1.3   10 Jun 1994 17:31:16   JAY
// Some string case problems fixed.
// 
//    Rev 1.2   10 Jun 1994 10:06:28   JAY
// Added handling of line flags for file lists.
// 
//    Rev 1.1   01 Jun 1994 16:45:28   JAY
// Extended VirtualFileList in both directions to encompass KeyedFileList and
// FontFileList.
// 
//    Rev 1.0   23 May 1994 09:19:14   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "vfl.h"

#include <stdlib.h>				// For atoi().
#include <string.h>
#include <ctype.h>				// For isspace().

#define VIRTUAL_FILE_KEY		"MicroLogic Virtual File List"
#define FONT_FILE_KEY			"MicroLogic Font File List"

///////////////////////////////////////////////////////////////////////////
// KeyedFileList support.

/*
// Create a KFL.
*/

KeyedFileList::KeyedFileList(PCSTR name, BOOL upper_case_lines)
			: m_file((LPSTR)name), m_upper_case_lines(upper_case_lines)
{
}

/*
// Read a line from the KFL file.
*/

ERRORCODE KeyedFileList::read_a_line(void)
{
	return m_file.read_line((LPSTR)m_line_buffer,
							  sizeof(m_line_buffer),
							  (TRF_skip_comments | TRF_skip_blank_lines));
}

/*
// Open and validate the KFL.
*/

ERRORCODE KeyedFileList::open(void)
{
	ERRORCODE error;

/* Set the read buffer. */

	m_file.set_read_buffer(2048);

/* Read the first line. */

	if ((error = read_a_line()) != ERRORCODE_None)
	{
		return error;
	}

/* See if it's good. */

	if (stricmp(m_line_buffer, file_key_string()) != 0)
	{
	/* Not a good file, I guess. */
		return ERRORCODE_IllegalType;
	}

/* This seems OK so far. */

	return ERRORCODE_None;
}

/*
// Read the virtual path from the file.
*/

PCSTR KeyedFileList::read_virtual_path(void)
{
	if (read_a_line() == ERRORCODE_None)
	{
	/* Make sure the path is upper-case. */
		return strupr(m_line_buffer);
	}
	return NULL;
}

/*
// Read the next name from the file.
*/

PCSTR KeyedFileList::read_next_name(void)
{
	m_last_line_flags = 0;
	m_last_line_version = 0;

	if (read_a_line() != ERRORCODE_None)
	{
		return NULL;
	}

	LPSTR p;
	LPSTR pname = m_line_buffer;

	/*
	// Line format is:
	//    (1) filename
	// or (2) #,filename
	// or (3) #.#,filename
	*/

	if ((p = strchr(pname, ',')) != NULL)
	{
	/* Get flags field. */

		*p++ = '\0';		// Terminate it!

		LPSTR p2 = strchr(pname, '.');
		if (p2 == NULL)
		{
		/* # = flags only */
			m_last_line_flags = atoi(pname);
			m_last_line_version = 0;
		}
		else
		{
		/* #.# = version.flags */ 
			*p2++ = '\0';
			m_last_line_flags = atoi(p2);
			m_last_line_version = atoi(pname);
		}

	/* Skip ','. Start looking for filename there. */

		pname = p;
	}

/* Advance to filename past white space. */

	while (*pname != '\0' && isspace(*pname))
	{
		pname++;
	}

	if (m_upper_case_lines)
	{
		strupr(pname);
	}

	return pname;
}

///////////////////////////////////////////////////////////////////////////
// VirtualFileList support.

VirtualFileList::VirtualFileList(PCSTR name) : KeyedFileList(name, TRUE)
{
}

PCSTR VirtualFileList::file_key_string(void)
{
	return VIRTUAL_FILE_KEY;
}

///////////////////////////////////////////////////////////////////////////
// FontFileList support.

FontFileList::FontFileList(PCSTR name) : KeyedFileList(name, FALSE)
{
}

PCSTR FontFileList::file_key_string(void)
{
	return FONT_FILE_KEY;
}

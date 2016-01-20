/*
// $Header: /PM8/App/VFL.H 1     3/03/99 6:13p Gbeddow $
//
// Virtual file list object definitions.
// This is the object that manages the VFL file.
//
// $Log: /PM8/App/VFL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     4/29/98 4:27p Fredf
// New PMW fonts. Required changes to PMW.FFL.
// 
//    Rev 1.0   14 Aug 1997 15:26:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:52   FRED
// Initial revision.
// 
//    Rev 2.2   31 Jan 1996 08:21:42   JAY
// More configuration stuff
// 
//    Rev 2.1   08 Feb 1995 13:36:46   JAY
// Reverted. New series.
// 
//    Rev 1.4   30 Nov 1994 16:59:28   JAY
//  
// 
//    Rev 1.3   10 Jun 1994 17:31:08   JAY
// Some string case problems fixed.
// 
//    Rev 1.2   10 Jun 1994 10:06:14   JAY
// Added handling of line flags for file lists.
// 
//    Rev 1.1   01 Jun 1994 16:44:58   JAY
// Extended VirtualFileList in both directions to encompass KeyedFileList and
// FontFileList.
// 
//    Rev 1.0   23 May 1994 09:19:14   JAY
// Initial revision.
*/

#ifndef __VFL_H__
#define __VFL_H__

#include "system.h"
#include "file.h"

/*
// A keyed file list.
//
// To use this:
// (1) construct the VirtualFileList object with the file name
// (2) call open()
// (3) call read_virtual_path()
// (4) call read_next_name() until it returns NULL (or until you no longer
//     need to read names).
//
// At this time, calling this with any other sequence will produce
// unpredictable results!
//
// Be sure to check all error conditions (ERROR from open, NULL string
// from read methods).
//
// If a line has a comma in it, the line is assumed to begin with a flags
// number. The line returned is the text after the comma (and white space).
// The flags value is retrievable via last_line_flags();
*/

class KeyedFileList
{
private:
	TextReadFile m_file;
	BOOL m_upper_case_lines;
	char m_line_buffer[256];
	WORD m_last_line_flags;
	WORD m_last_line_version;

	ERRORCODE read_a_line(void);

/* You must override this to make it work. */

	virtual PCSTR file_key_string() = 0;

public:

	KeyedFileList(PCSTR name, BOOL upper_case_lines = FALSE);

/*
// Open the VFL file and validate it.
*/

	ERRORCODE open(void);

/*
// Read the virtual path line.
*/

	PCSTR read_virtual_path(void);

/*
// Read the next name from the file.
*/

	PCSTR read_next_name(void);

/*
// Return the flags for the last line.
*/

	WORD last_line_flags(void)
		{ return m_last_line_flags; }

/*
// Return the version for the last line.
*/

	WORD last_line_version(void)
		{ return m_last_line_version; }

};

/*
// A virtual file list.
//
// This is the data file with a ".VFL" extension storing info about virtual
// files in the directory.
*/

class VirtualFileList : public KeyedFileList
{
private:
	virtual PCSTR file_key_string();
public:
	VirtualFileList(PCSTR name);

};

/*
// A font file list.
//
// This is the data file with a ".FFL" extension storing info about virtual
// fonts.
*/

class FontFileList : public KeyedFileList
{
private:
	PCSTR file_key_string();
public:
	FontFileList(PCSTR name);
};

#endif					// #ifndef __VFL_H__

/*
// $Header: /PM8/Download/COMPFN.H 1     3/03/99 6:15p Gbeddow $
// MISC.H
//
// Composite file name definitions
//
// $Log: /PM8/Download/COMPFN.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 2     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:19:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:18   FRED
// Initial revision.
// 
//    Rev 2.1   05 Jun 1995 11:31:10   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:24   JAY
// Initial revision.
// 
//    Rev 1.1   04 Aug 1994 07:34:04   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   23 May 1994 09:15:20   JAY
// Initial revision.
*/

#ifndef __COMPFN_H__
#define __COMPFN_H__

/*
// A "composite filename".
//
// This is a single string which contains (hopefully) all the information
// we will need to identify and locate files.
//
// History:
//
// In the original Avagio and PMG code (UWS), filenames of shapes from PMP
// shape libraries were stored as "shape name \n lib name". Code existed to
// skip the shape name and extract the lib name (the real file name) which
// was at the end.
//
// In PMG (MLS), this mechanism was extended to include references to
// collections (.PCT files) which were a sort of gateway references to other
// subdirectories. All collection files were separated by '\n' characters,
// with the final string being the full "direct" path name to the file.
//
// PMW doesn't support collections. Previous versions (<=109) merely skip all
// '\n' characters in the file name and treat the last string as a full direct
// path name.
//
// Now: (post PMW 109).
//
// Since the only prior products which can read documents created by this
// version are earlier versions of PMW, we only need to make sure THEY can
// parse the filenames correctly. This will happen if the last component in a
// '\n'-delimited filename is a full file name.
//
// To that end, a "composite filename" is defined as a '\n'-delimited string
// with the following (current) fields:
//
//		field 1:			dynamic filename		[\n]
//		field 2:			virtual filename		[\n]
//		field last:		full file name (the original or best guess name)
//
// Note that the full file name is not defined as "field 3", but "field last".
// This implies that the mechanism can be extended by adding additional fields
// between field 2 and the last field (which must ALWAYS be the best guess
// file name). It is important that code dealing with composite filenames do
// not assume that the full file name is the third field (use the path manager
// routines to extract the components to be safe).
//
// - A "dynamic filename" is a coded description of how to find the file.
// - A "virtual filename" makes virtual directories work correctly from the
// user perspective. In this case the virtual filename is the name the user
// believed he picked, and the full file name is the alias name that came out
// of the VFL file.
// - A "full file name" is a name which is more or less usable directly to
// open the file, after handling any relativeness.
//
// A correctly constructed composite filename has at least three fields, and
// the first field can not end in a PCT extension (which qualifies it as an
// old style collection filename). Any field which has no pertinent data should
// be empty (immediately terminated by its '\n'), except the last field which
// currently MUST be non-blank for compatability reasons.
//
// NOTE:
// Any field which is missing defaults to the full file name which MUST exist.
//
// WARNING:
// Since multiple MAX_PATH elements can exist in a composite filename, DO NOT
// rely on it fitting in a single buffer of size _MAX_PATH characters. This
// is NOT a simple DOS path name.
*/

class CompositeFileName
{
protected:
	CString m_csDynamicName;
	CString m_csVirtualName;
	CString m_csFileName;

	CString m_csCompositeName;

	void extract_components(void);

public:
/*
// Construct a CompositeFileName from a single composite file name.
// The composite file name is broken down into components.
*/
	CompositeFileName(LPCSTR pszCompositeName);

/*
// Construct a CompositeFileName from the components.
// The composite file name is created from the components.
*/

	CompositeFileName(LPCSTR file_name, LPCSTR dynamic_name, LPCSTR virtual_name);

/*
// Destroy a CompositeFileName.
// All strings within it are freed.
*/

	~CompositeFileName();

/*
// Does the CFN have a dynamic name?
*/

	BOOL has_dynamic_name(void)
		{ return !m_csDynamicName.IsEmpty(); }

/*
// Return the dynamic name.
// If it has no dynamic name and file_name_as_default is TRUE,
// the file name will be returned.
*/

	LPCSTR get_dynamic_name(BOOL file_name_as_default = FALSE);

/*
// Does the CFN have a virtual name?
*/

	BOOL has_virtual_name(void)
		{ return !m_csVirtualName.IsEmpty(); }

/*
// Return the virtual name.
// If it has no virtual name and file_name_as_default is TRUE,
// the file name will be returned.
*/

	LPCSTR get_virtual_name(BOOL file_name_as_default = FALSE);

/*
// Return the file name.
*/

	LPCSTR get_file_name(void) const
		{ return m_csFileName; }

/*
// Return the full composite name.
*/

	LPCSTR get_composite_name(void) const
		{ return m_csCompositeName; }

/*
// Construct a composite path from components.
// The path is allocated from the system heap and the caller must free it.
*/

	static BOOL construct_composite_path(CString& csCompPath,
													 LPCSTR file_name,
													 LPCSTR dynamic_name = NULL,
													 LPCSTR virtual_name = NULL);

/*
// Skip all embedded information on the front of a path.
*/

	static CString SkipEmbeddedInfo(LPCSTR name);
};

#endif			// #ifndef __COMPFN_H__

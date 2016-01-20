/*
// $Header: /PM8/Download/COMPFN.CPP 1     3/03/99 6:15p Gbeddow $
//
// Composite filename routines.
//
// $Log: /PM8/Download/COMPFN.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 3     9/15/98 2:52p Johno
// Removed ASSERT in construct_composite_path()
// 
// 2     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:19:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:48   FRED
// Initial revision.
// 
//    Rev 2.1   05 Jun 1995 11:31:02   JAY
// Got rid of DGROUP objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:36   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:35:42   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   04 Aug 1994 07:33:46   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   23 May 1994 09:15:22   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "compfn.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
// CompositeFileName routines.

/*
// Construct a CompositeFileName from a composite name.
*/

CompositeFileName::CompositeFileName(LPCSTR pszCompositeName)
{
/* Copy the composite name. */

	m_csCompositeName = pszCompositeName;

/* And set pointers to the components. */

	extract_components();
}

/*
// Construct a CompositeFileName from components.
// Any component past file_name can be NULL.
*/

CompositeFileName::CompositeFileName(LPCSTR file_name, LPCSTR dynamic_name, LPCSTR virtual_name)
{
	// Build the composite name from the components.
	construct_composite_path(m_csCompositeName, file_name, dynamic_name, virtual_name);

	// And set pointers to the components in the constructed name.
	extract_components();
}

/*
// Set up pointers to the components for later use.
*/

void CompositeFileName::extract_components(void)
{
	// Extract the components from the composite name.

	m_csFileName = m_csCompositeName;
	m_csDynamicName.Empty();
	m_csVirtualName.Empty();

	// See if we have any components.

	int nBreak = m_csCompositeName.Find('\n');
	if (nBreak != -1)
	{
		// Try to break the strings out.
		CString cs1 = m_csCompositeName.Left(nBreak);
		CString cs2 = m_csCompositeName.Mid(nBreak+1);
		nBreak = cs2.Find('\n');
		if (nBreak != -1)
		{
			// We have at least two breaks. Split out the components.
			m_csDynamicName = cs1;
			m_csVirtualName = cs2.Left(nBreak);
			m_csFileName = cs2.Mid(nBreak+1);
		}
	}

	// Skip all the remaining '\n's in the name for the file name.
	m_csFileName = SkipEmbeddedInfo(m_csFileName);
}

/*
// Destroy a CompositeFileName.
*/

CompositeFileName::~CompositeFileName()
{
}

/*
// Construct a composite path.
// The path is allocated from the system heap and must be freed by the caller.
*/

BOOL CompositeFileName::construct_composite_path(CString& csCompPath,
																 LPCSTR file_name,
																 LPCSTR dynamic_name,
																 LPCSTR virtual_name)
{
	static char blank[] = "";

	if (file_name == NULL || *file_name == '\0')
	{
		// Must have a reasonably valid file name.
//		ASSERT(FALSE);
		// JRO (but not worth ASSERTING over)
		return FALSE;
	}

	if (dynamic_name == NULL)
	{
		dynamic_name = blank;
	}
	if (virtual_name == NULL)
	{
		virtual_name = blank;
	}

	csCompPath.Format("%s\n%s\n%s", dynamic_name, virtual_name, file_name);

	return TRUE;
}

/*
// Return the virtual name.
// If it has no virtual name and file_name_as_default is TRUE,
// the file name will be returned.
*/

LPCSTR CompositeFileName::get_virtual_name(BOOL file_name_as_default)
{
/* Do we have a virtual name? */

	if (m_csVirtualName.IsEmpty())
	{
		if (file_name_as_default && !m_csFileName.IsEmpty())
		{
			// Use the file name.
			return m_csFileName;
		}
		return NULL;
	}

	// Return the virtual name part.
	return m_csVirtualName;
}

/*
// Return the dynamic name.
// If it has no dynamic name and file_name_as_default is TRUE,
// the file name will be returned.
*/

LPCSTR CompositeFileName::get_dynamic_name(BOOL file_name_as_default)
{
	// Do we have a dynamic name?

	if (m_csDynamicName.IsEmpty())
	{
		if (file_name_as_default && !m_csFileName.IsEmpty())
		{
			// Use the file name.
			return m_csFileName;
		}
		return NULL;
	}

	// Return the dynamic name part.
	return m_csDynamicName;
}

/*
// Skip all embedded info on the front of a name.
// We return a CString so we can assign back to the same CString we passed in.
*/

CString CompositeFileName::SkipEmbeddedInfo(LPCSTR name)
{
	LPCSTR p;

	while ((p = strchr(name, '\n')) != NULL)
	{
		name = p+1;
	}
	return name;
}


//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/COLLIMP.CPP 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/COLLIMP.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 1     9/22/98 4:03p Jayn
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"

#include "collimp.h"
#include "pmwcoll.h"
#include "grafrec.h"			// for type_of_art_extension() and types
#include "browsupp.h"		// for CBrowserProgressDialog
#include "compfn.h"

/////////////////////////////////////////////////////////////////////////////
// CPMWCollectionImportContext

CPMWCollectionImportContext::CPMWCollectionImportContext()
{
}

CPMWCollectionImportContext::~CPMWCollectionImportContext()
{
}

void CPMWCollectionImportContext::ImportContent(void)
{
	// Create a progress dialog.
	int nCount = m_csaImport.GetSize();
	CBrowserProgressDialog ProgressDialog(IDD_BROWSER_IMPORT_PROGRESS, nCount, AfxGetMainWnd(), IDS_CONFIRM_CANCEL_BROWSER_IMPORT);

	// get the default category name
	CString strCategory;
	if (m_pCollection->GetType() == CPMWCollection::typeArt)
		strCategory.LoadString(IDS_DEF_ART_CATEGORY);
	else // we'll assume typeProjects
		strCategory.LoadString(IDS_DEF_PROJECT_CATEGORY);

	for (int nFile = 0; nFile < nCount; nFile++)
	{
		CString csFile = m_csaImport.GetAt(nFile);

		// Update the progress dialog.
		CompositeFileName cfn(csFile);
		CString csBaseName;
		Util::SplitPath(cfn.get_file_name(), NULL, &csBaseName);
		ProgressDialog.SetPosition(nFile, csBaseName);
		if (ProgressDialog.CheckForAbort())
		{
			break;
		}

		m_pCollection->ImportItem(csFile, NULL, NULL, strCategory);
	}
}

void CPMWCollectionImportContext::CountContent(void)
{
   CString csSearchString;
	Util::ConstructPath(csSearchString, m_csDirectory, "*.*");
	CFileIterator fit;

	if (fit.FindFirst(csSearchString, _A_NORMAL|_A_RDONLY|_A_SUBDIR))
	{
		do
		{
			CString csName;

			Util::ConstructPath(csName, m_csDirectory, fit.Name());

			if (fit.Attributes() & _A_SUBDIR)
			{
            if (strcmp(fit.Name(), ".") != 0
                  && strcmp(fit.Name(), "..") != 0)
            {
				   // Recurse into this directory.
               CString csDirSave = m_csDirectory;
               m_csDirectory = csName;
				   CountContent();
               m_csDirectory = csDirSave;
            }
			}
			else
			{
				// A file! If this matches the criteria, include it in the list.
				if (IsMember(csName))
				{
					m_csaImport.Add(csName);
				}
			}
		} while (fit.FindNext());
	}
}

//
// Import content from a given directory.
//

void CPMWCollectionImportContext::ImportFromDirectory(void)
{
	// Create the content criteria.
	CountContent();
	if (m_csaImport.GetSize() > 0)
	{
		// We found something!
		if (AfxMessageBox(m_uConfirmMessage, MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			ImportContent();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPMWProjectImportContext

CPMWProjectImportContext::CPMWProjectImportContext()
{
}

BOOL CPMWProjectImportContext::IsMember(LPCSTR pszFile)
{
	return (GET_PMWAPP()->GetMatchingTemplate(pszFile, TRUE) != NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CPMWArtImportContext

CPMWArtImportContext::CPMWArtImportContext()
{
}

BOOL CPMWArtImportContext::IsMember(LPCSTR pszFile)
{
	// We only auto-import things of the correct extension.
	int nArtType = type_of_art_extension(pszFile);
	return ( nArtType == GRAPHIC_TYPE_PMO ||
				nArtType == GRAPHIC_TYPE_DPP ||
				nArtType != GRAPHIC_TYPE_UNKNOWN );
}


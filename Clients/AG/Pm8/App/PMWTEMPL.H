/*
// $Workfile: PMWTEMPL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWTEMPL.H $
// 
// 1     3/03/99 6:09p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:24:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:18   Fred
// Initial revision.
// 
//    Rev 1.2   08 Apr 1997 16:29:36   johno
// Added IDResource ()
// 
//    Rev 1.1   07 May 1996 12:44:48   Jay
// From PMW2
// 
//    Rev 1.2   06 May 1996 16:45:46   JAY
// Project type support for New dialog.
// 
//    Rev 1.1   18 Jan 1996 17:09:52   JAY
// New project configuration class
// 
//    Rev 1.0   20 Jul 1995 12:52:54   JAY
// Initial revision.
*/

#ifndef __PMWTEMPL_H__
#define __PMWTEMPL_H__

class CPmwDocTemplate : public CMultiDocTemplate
{
public:
	CPmwDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	
	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	// Overridden.
	virtual Confidence MatchDocType(const char* pszPathName,
					CDocument*& rpDocMatch);

	void ProjectType(int nProjectType)
		{ m_nProjectType = nProjectType; }
	int ProjectType(void) const
		{ return m_nProjectType; }

   int IDResource (void)
   {
      return m_nIDResource;
   }

protected:
	void CreateMenu(void);
	void DeleteMenu(void);
	int m_nProjectType;
};

#endif

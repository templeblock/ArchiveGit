/*
// $Workfile: COLLBLD.H $
// $Revision: 2 $
// $Date: 3/08/99 3:36p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLLBLD.H $
// 
// 2     3/08/99 3:36p Gbeddow
// support for asking at start of an online collection build whether to
// compress the online data files
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 15    1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 14    1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 13    12/18/98 4:52p Gbeddow
// support for writing grapic types into art collection files
// 
// 12    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 11    8/10/98 3:06p Hforman
// add CropCopyright code
// 
// 10    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 9     5/29/98 8:51p Fredf
// Support for download items with subdirectories.
// 
// 8     5/25/98 4:43p Fredf
// Support for building online collections.
// 
// 7     5/19/98 11:32a Dennis
// Made StringIsEmpty() a public static method.  DoBuild() now takes
// parameter to indicate if it should run silent.
// 
// 6     4/24/98 7:41a Jayn
// Builder changes to support "inclusion column" and "external file".
// 
// 5     4/15/98 1:19p Dennis
// Enhanced progress bar logic to exclude empty spreadsheet lines.
// 
// 3     4/14/98 10:04a Dennis
// a) Removed MEDIA and STYLE as required fields.
// b) Added FREE SAMPLE, HIDE ITEM, and THUMBNAIL fields.
// c) Removed obsolete thumbnail code.
// d) Removed Media and Style.
// e) Added Custom Flags.
// f) Updated GetData() with AddSubdirectory changes from PMG4 source.
// 
// 2     12/08/97 2:38p Dennis
// Added error handling for missing columns.  Added progress bar with
// cancel.  Disabled main window while building.
// 
//    Rev 1.0   14 Aug 1997 15:19:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:08   Fred
// Initial revision.
// 
//    Rev 1.10   16 Jul 1997 13:40:50   dennisp
// Added error statistics to build.
// 
//    Rev 1.9   10 Jul 1997 14:06:58   dennisp
// Added PVCS header
// 
// 
*/


#ifndef _COLLBLD_INC
#define _COLLBLD_INC

#include <string.h>  // for memset()
#include <stdlib.h>  // for toupper()
#include <ctype.h>

#include "ipathmgr.h"
#include "collctg.h"
#include "error.h"
#include "sortarry.h"
#include "collfile.h"
#include "pmwcoll.h"
#include "BaseBld.h"
#include "ssHeaderArray.h"
#include "compdb.h"

#pragma pack(1)   // Pack all structures used in this file

#ifdef WIN32
class CCollectionBuilder
   {
public:
      enum BuildTypes
         {
            projects,
            clipart,
            sentiments
         };
      CCollectionBuilder(IPathManager* pPathManager, const CString &strINIFile, const CString &strSection,
         const CString &strSpreadSheetFileName, const CString* pstrCollectionPath = NULL,
			BOOL bOnline = FALSE, BOOL bCompressOnlineDataFiles = TRUE);
      ~CCollectionBuilder();
      int DoBuild(CWnd *pParent, BOOL bValidateOnly=FALSE, BOOL bRunSilent=TRUE);
      int DumpCategoryTree();


   // Static members
      static BOOL  StringIsEmpty(LPCSTR szString);
   
    protected:
		IPathManager*		m_pPathManager;
      CPMWCollection    m_Collection;
      CPMWCollectionData   m_CollectionData;
      CString           m_strFileNamePrefix;
      CString           m_strSpreadSheetFileName;
      CStdioFile        m_stdioLogFile;
		BOOL					m_bOnline;
		BOOL					m_bCompressOnlineDataFiles;

		CString				m_strOnlineCollectionName;
		CString				m_strOnlinePath;
		CString				m_strOnlineFtpPath;
		CString				m_strOnlineSqlPath;
      CStdioFile        m_stdioSqlInstallFile;
      CStdioFile        m_stdioSqlInstallList;
      CStdioFile        m_stdioSqlDeleteFile;

      // Data for databases
      int               m_nTone;
      USHORT            m_shProductFlags;
      CString           m_strFullDataFileName, m_strCategory, m_strFileName;
      CString           m_strProductCode, m_strThumbnailFileName;
		CString				m_strExternalFile;
      LPVOID            m_pContentData;
      CCategoryDataItem m_cdItem;

      
      // Members for spreadsheet processing
      TextReadFile      m_trFileSpreadSheet;
   	CCollHeaderArray	m_csaHeaderFields[NUM_SPREADSHEET_HEADER_LINES];
   	CMlsStringArray   m_csaFields, m_csaStopList;
      char              m_chLine[MAX_SPREADSHEET_LINE];
      int               m_nCurLine; // one based index to current line
      int               m_nCurItemNum, m_nInitRetVal;
      CPMWCollection::CollectionTypes   m_BuildType;
      int               m_nNonEmptySpreadSheetLineCount;
      BOOL              m_bValidationError, m_bValidateOnly;
      CString           m_strBuildPath, m_strSentiment, m_strFullContentPath;
      BOOL              m_bFreeSample, m_bHideItem, m_bLineIsEmpty;
		CCategoryDataItem::CustomFlags	m_GraphicTypeFlags;
		BOOL              m_bCropCopyright;
      CCollectionBuilderStats m_BuildStats;
		int					m_nInclusionColumn;

      CMapStringToPtr   m_cmspContentFileNames, m_cmspCategoryNames;

      // Window controls
      CWnd *            m_pParent;

   // Internal helpers
   protected:
      int   ReadHeaderLines();
      int   ProcessLine();
      int   ValidateFields();
      DWORD RangeToFlagValue(int nStartColumn, int nEndColumn);
      DWORD BuildFlags(int nStartColumn, int nEndColumn);
		CCategoryDataItem::CustomFlags SetGraphicTypeFlags(int nStartColumn, int nEndColumn);
      
      int   OpenAll();
      int   CloseAll();
      int   StandardizeFields();
      int   GetData();
		CString MajorCategoryFromBoth( CString csMajorAndMinorCategory );
		CString MinorCategoryFromBoth( CString csMajorAndMinorCategory );
		char*	ToneEnumToString(CCategoryDataItem::Tones tone);
		char*	GraphicTypeEnumToString(CCategoryDataItem::CustomFlags graphicType);
      void  ReleaseData();
      int   AddKeywords();
      int   AddItems();
      int   GetSpreadSheetLineCount();
      int   CheckForDuplicateContent();
      int   CheckForCategoryOrderError();
      void  ReportError(const CString &strError, CCollectionBuilderStats::StatisticTypes type=CCollectionBuilderStats::typeWarning);
  };
#endif 

#pragma pack()   // back to default packing

#endif      // end if _BROWBLD_INC



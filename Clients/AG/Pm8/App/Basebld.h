//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/Basebld.h 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/Basebld.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     2/05/99 11:29a Mwilson
// added classes for new collection handling for CreataCard
// 
// 2     1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 1     1/27/99 3:41p Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __BASEBLD_H__
#define __BASEBLD_H__

#include <string.h>  // for memset()
#include <stdlib.h>  // for toupper()
#include <ctype.h>
#include "compdb.h"

#include "ipathmgr.h"
#include "collctg.h"
#include "error.h"
#include "sortarry.h"
#include "collfile.h"
#include "ssHeaderArray.h"

#define  MAX_SPREADSHEET_LINE          1000 // max line length in spreadsheet
#define  NUM_SPREADSHEET_HEADER_LINES  2

#pragma pack(1)   // Pack all structures used in this file


#ifdef WIN32
class CCollectionBuilderStats
{
public:
   enum StatisticTypes
   {
      typeNone, 
      typeWarning, 
      typeError,
      typeEnd
   };
// Get Methods
   long GetWarningCount(void) const
   {
      return m_lWarningCount;
   }
   long GetErrorCount(void) const
   {
      return m_lErrorCount;
   }
   long GetNoneCount(void) const
   {
      return m_lNoneCount;
   }

// Set / Assignment Methods
   void Empty(void)
   {
      m_lNoneCount = m_lWarningCount = m_lErrorCount = 0;
   }
   void IncrementCount(enum StatisticTypes type)
   {
      switch(type)
      {
         case typeNone:
            IncrementNoneCount();
            break;
         case typeWarning:
            IncrementWarningCount();
            break;
         case typeError:
            IncrementErrorCount();
            break;
      }
   }
   void IncrementNoneCount()
   {
      m_lNoneCount++;
   }
   void IncrementWarningCount()
   {
      m_lWarningCount++;
   }
   void IncrementErrorCount()
   {
      m_lErrorCount++;
   }
protected:
   long m_lWarningCount;
   long m_lErrorCount;
   long m_lNoneCount;
};

#endif //WIN32

class CPBaseCollBuilder
{
public:

   CPBaseCollBuilder(	const CString &strSpreadSheetFilePath,
							const CString &strCollectionPath);
	
	virtual ~CPBaseCollBuilder();

	virtual int DoBuild() = 0;
protected:

	virtual int		ValidateFields() = 0;
	virtual CString GetCollectionName() = 0;
	virtual int		ProcessLine();
   virtual int		OpenAll();
   virtual int		CloseAll();

	void	RemoveExtraQuotes(char* pData);
	int   GetSpreadSheetLineCount();
	int   ReadHeaderLines();
	BOOL	StringIsEmpty(LPCSTR szString);
	DWORD RangeToFlagValue(int nStartColumn, int nEndColumn);
   void  ReportError(const CString &strError, CCollectionBuilderStats::StatisticTypes type=CCollectionBuilderStats::typeWarning);


   CString           m_strSpreadSheetFilePath;
   CStdioFile        m_stdioLogFile;
	BOOL					m_bOnline;
   CString           m_strFullDataFileName;
	CString				m_strFileName;
	CString				m_strCollectionPath;
   
	// Members for spreadsheet processing
   TextReadFile      m_trFileSpreadSheet;
   CSpreadSheetHeaderArray*	m_pcsaHeaderFields;
   CMlsStringArray   m_csaFields;
   char              m_chLine[MAX_SPREADSHEET_LINE];
   int               m_nCurLine; // one based index to current line
   int               m_nCurItemNum;
	int					m_nInitRetVal;
   int               m_nNonEmptySpreadSheetLineCount;
   BOOL              m_bValidationError;
	BOOL					m_bValidateOnly;
	BOOL					m_bLineIsEmpty;
   CString           m_strBuildPath;

   CCollectionBuilderStats m_BuildStats;

};

#pragma pack()   // back to default packing

#endif

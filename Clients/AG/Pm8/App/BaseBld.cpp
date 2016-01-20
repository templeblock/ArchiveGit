//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/BaseBld.cpp 1     3/03/99 6:03p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/BaseBld.cpp $
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
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseBld.h"

CPBaseCollBuilder::CPBaseCollBuilder(	const CString &strSpreadSheetFilePath,
												const CString &strCollectionPath):
			m_trFileSpreadSheet(strSpreadSheetFilePath),
			m_pcsaHeaderFields(NULL)
{
   ASSERT(!strSpreadSheetFilePath.IsEmpty());
   ASSERT(!strCollectionPath.IsEmpty());

   m_nCurLine = 1;
   m_strSpreadSheetFilePath = strSpreadSheetFilePath;
   Util::SplitPath(m_strSpreadSheetFilePath, &m_strBuildPath, NULL);
	m_strCollectionPath = strCollectionPath;
}

CPBaseCollBuilder::~CPBaseCollBuilder()
{
	if(m_pcsaHeaderFields)
		delete[] m_pcsaHeaderFields;
}

int CPBaseCollBuilder::ProcessLine()
{
	CString strError;

   if (m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE) != ERRORCODE_None)
		return ERRORCODE_Done;

	RemoveExtraQuotes(m_chLine);

   // Check if empty
   m_bLineIsEmpty = FALSE;
   if(StringIsEmpty(m_chLine))
   {
      // Skip it if empty
      m_bLineIsEmpty = TRUE;
      return ERRORCODE_None;
   }

   m_strFullDataFileName = m_strBuildPath;
   // Remove trailing backslash
	Util::RemoveBackslashFromPath(m_strFullDataFileName);
	
	// Extract the fields for this line.
   m_csaFields.SetNames(m_chLine, '\t'); // Set delimiter to Tabs

	// Check for incomplete fields entered in spreadsheet 09/08/97-DGP
	if(m_csaFields.GetSize() < m_pcsaHeaderFields[0].GetLastUsedColumn())
	{
		strError.Empty();
		strError.Format("Line %05i: COLUMN COUNT MISMATCH - Line has missing fields\n",
			m_nCurLine);
		ReportError(strError);

		return ERRORCODE_None;
	}

	TRACE(">>> Processing line: %s <<<\n", (LPCSTR)m_strFileName);
 
   return ERRORCODE_None;
}

int CPBaseCollBuilder::GetSpreadSheetLineCount()
{
   int   nLineCount = 0;

   // Seek to top of file
   m_trFileSpreadSheet.seek(0, ST_DEV_SEEK_SET);
   while(m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE) == ERRORCODE_None)
      {
         // Don't count it if empty
         if(!StringIsEmpty(m_chLine))
            nLineCount++;
      }
   // Seek back to top of file
   m_trFileSpreadSheet.seek(0, ST_DEV_SEEK_SET);
   return nLineCount;
}

int CPBaseCollBuilder::ReadHeaderLines()
{
	while(m_nCurLine <= NUM_SPREADSHEET_HEADER_LINES)
	{
		if (m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE) != ERRORCODE_None)
			return ERRORCODE_Read;

		m_pcsaHeaderFields[m_nCurLine-1].SetNames(m_chLine, '\t'); // Set delimiter to Tabs
		if(m_nCurLine == 1)
		{
			if(m_pcsaHeaderFields[m_nCurLine-1].ValidateFields(&m_stdioLogFile) !=
				ERRORCODE_None)
				{
					AfxMessageBox("Error Occurred in Validating Header Fields\nCheck Log File");
					return ERRORCODE_Fail;
				}
		}
		m_nCurLine++;
	}
   return ERRORCODE_None;
}

DWORD CPBaseCollBuilder::RangeToFlagValue(int nStartColumn, int nEndColumn)
{
   int      nCurField, nNumSet=0;
   DWORD    dwValue = 0;
   BOOL     bOn;

   for(nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++)
   {
      bOn = atoi(m_csaFields.GetAt(nCurField));
      if(bOn)
      {
         dwValue |= (1 << (nCurField-nStartColumn));
         nNumSet++;
      }
   }
   if(nNumSet != 1)
   {
      CString  strError;

      strError.Empty();
      strError.Format("Line %05i: ", m_nCurLine);
      if(nNumSet > 1)
         strError += "More than one selection for TYPE";
      else
         strError += "No selection in TYPE";
      strError += "\n";
      ReportError(strError);
      m_bValidationError = TRUE;
   }
   return dwValue;
}


void CPBaseCollBuilder::ReportError(const CString &strError, CCollectionBuilderStats::StatisticTypes statType)
{
	m_stdioLogFile.WriteString(strError);
	m_stdioLogFile.Flush();    // Flush buffers 
	m_BuildStats.IncrementCount(statType);
}

// Checks if passed string is empty
BOOL CPBaseCollBuilder::StringIsEmpty(LPCSTR szString)
{
   BOOL  bIsEmpty = TRUE;
   int   i;

   for(i=0;szString[i] && bIsEmpty;i++)
   {
		if(szString[i] != ' ' && szString[i] != '\t')
		{
			bIsEmpty = FALSE;
			continue;
		}
   }
   return bIsEmpty;
}

int CPBaseCollBuilder::OpenAll()
{
   CString              strLogFileName;

	Util::ConstructPath(strLogFileName, m_strCollectionPath, (GetCollectionName() + ".log"));

   // Create LOG file
   if (!m_stdioLogFile.Open(strLogFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		CString	csError;
		csError.Format("Failed to open LOG file %s.\nCheck if file is in use.\nBuild has been aborted", strLogFileName);
		AfxMessageBox(csError);
      return ERRORCODE_Open;
	}
	return ERRORCODE_None;
}

int CPBaseCollBuilder::CloseAll()
{
   // Close LOG file
   m_stdioLogFile.Close();

   return ERRORCODE_None;
}

void CPBaseCollBuilder::RemoveExtraQuotes(char* pData)
{
	// Excel puts quotes around things with commas, and doubles up quotes in things
	// with quotes. We don't want the extra quotes in the collection, so we have to
	// strip them out.
	char szBuffer[MAX_SPREADSHEET_LINE];
	int nBufferIndex = 0;
	for (int i = 0; ; i++)
	{
		if (pData[i] != '"')
		{
			szBuffer[nBufferIndex++] = pData[i];
			if (pData[i] == '\0')
			{
				break;
			}
		}
		else
		{
			// Current character is a quote. Eliminate single quotes,
			// collapse double quotes to a single quote. Note that
			// "single" and "double" refer to the number of quote
			// characters not the single and double quote characters
			// (' and ").

			if (pData[i+1] == '"')
			{
				szBuffer[nBufferIndex++] = '"';
				i++;
			}
		}
	}

	// Copy the result back to the original line.
	strcpy(pData, szBuffer);

}

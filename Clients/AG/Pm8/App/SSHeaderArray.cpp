//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/SSHeaderArray.cpp 1     3/03/99 6:11p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/SSHeaderArray.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 5     2/17/99 4:27p Lwu
// Added a few columns to handle btnType and other control types
// 
// 4     2/16/99 5:28p Mwilson
// added tile flag
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
#include "ssheaderarray.h"


static struct
{
	LPCSTR m_pszName;
	BOOL m_bRequired;
} CollHeaderInfo[] = 
{
   { "SUPER CATEGORY", TRUE },
   { "CATEGORY", TRUE },
   { "SUB CATEGORY", TRUE },
   { "DIRECTORY", TRUE },
   { "FILE NAME", TRUE },
   { "FRIENDLY NAME", TRUE },
   { "TONE", TRUE },
   { "KEYWORDS", TRUE },
   { "PRODUCT", TRUE },
   { "P CODE", TRUE },
   { "FRONT", TRUE },
   { "INSIDE", TRUE },
   { "THUMBNAIL", FALSE },
   { "FREE SAMPLE",  FALSE },
   { "HIDE ITEM", FALSE},
	{ "CROP COPYRIGHT", FALSE },
	{ "EXTERNAL FILE", FALSE },
	{ "GR", FALSE },
   { NULL, FALSE },
};

static struct
{
	LPCSTR m_pszName;
	BOOL m_bRequired;
} CompDBHeaderInfo[] = 
{
   { "TYPE", TRUE },
   { "DIRECTORY", TRUE },
   { "FILE NAME", TRUE },
   { "RESOLUTION", TRUE },
   { NULL, FALSE },
};

static struct
{
	LPCSTR m_pszName;
	BOOL m_bRequired;
} CompPageHeaderInfo[] = 
{
   { "PAGENAME", FALSE },
   { "CONTROLNAME", FALSE },
   { "CONTROL", TRUE },
	{ "BTNTYPE", FALSE },
   { "POSITION", TRUE },
   { "SIZE", TRUE },
   { "IMAGE FILE", TRUE },
   { "TILED", FALSE },
   { "CAPTION", TRUE },
   { "TOOLTIP", TRUE },
   { "FONT NAME", TRUE },
   { "FONT SIZE", TRUE },
   { NULL, FALSE },
};


// CSpreadSheetHeaderArray methods
CSpreadSheetHeaderArray::CSpreadSheetHeaderArray(int nSize) 
{ 
   m_pusEndColumn = NULL;
   m_nLastUsedColumn = 0;
   m_pusStartColumn=new unsigned short [nSize];
}

CSpreadSheetHeaderArray::~CSpreadSheetHeaderArray() 
   { 
      if(m_pusEndColumn)
         delete m_pusEndColumn; 
      if(m_pusStartColumn)
         delete m_pusStartColumn;
   }

void CSpreadSheetHeaderArray::DetectRanges()
{
   int   nPrevNotEmptyField = 0;

	m_nFieldCount = GetSize();
   if(m_nFieldCount <= 0)
      return;
   if(m_pusEndColumn)
      delete m_pusEndColumn;
   m_pusEndColumn = new unsigned short [m_nFieldCount];

   ASSERT(m_pusEndColumn);
   if(m_pusEndColumn == NULL)
      return;

   m_pusEndColumn[0] = 0;
	for (int nCurName = 1; nCurName < m_nFieldCount; nCurName++)
	{
		if (GetAt(nCurName).IsEmpty())
         continue;
      // Update end column info for previous field that has a name
      m_pusEndColumn[nPrevNotEmptyField] = nCurName-1;
      nPrevNotEmptyField = nCurName;
	}
   // Set last one to itself
   m_pusEndColumn[nPrevNotEmptyField] = m_nFieldCount-1;
}

int CSpreadSheetHeaderArray::GetStartColumn(int nFieldType)
{
   return (short)m_pusStartColumn[nFieldType];		// can be -1
}

int CSpreadSheetHeaderArray::GetEndColumn(int nFieldType)
{
   return m_pusEndColumn[GetStartColumn(nFieldType)];
}


//
//CCollHeaderArray class
//////////////////////////////////////////////
CCollHeaderArray::CCollHeaderArray():
	CSpreadSheetHeaderArray(CCollHeaderArray::ListEnd)
{
	
	
}

CCollHeaderArray::~CCollHeaderArray()
{
}

int CCollHeaderArray::ValidateFields(CStdioFile *pLogFile)
{
   BOOL  bReportedError = FALSE;

   DetectRanges();

   if(m_pusStartColumn==NULL)
      return ERRORCODE_Fail;
   for(FieldTypes curField=SuperCategory;curField < ListEnd;curField++)
   {
      int nItem = CMlsStringArray::FindName(GetName(curField), CMlsStringArray::IgnoreCase);
      if(nItem >= 0)
      {
         m_pusStartColumn[curField] = nItem;
         // Track last used column in header
         // This logic assumes that last header doesn't have data that 
         // spans columns.
         if(nItem > m_nLastUsedColumn)
            m_nLastUsedColumn = nItem;
      }
		else if(!GetRequired(curField))
		{
			m_pusStartColumn[curField] = -1;
		}
      else
		{
			if(pLogFile && pLogFile->m_hFile)
			{
				CString  strError = "Unable to Locate Header: ";
				strError += GetName(curField);
				strError += "\n";
				pLogFile->WriteString(strError);
			}
			bReportedError = TRUE;
		}
   }
   if(bReportedError)
      return ERRORCODE_Fail;
   return ERRORCODE_None;
}

LPCSTR CCollHeaderArray::GetName(int nFieldType)
{
  return CollHeaderInfo[nFieldType].m_pszName;
}

BOOL CCollHeaderArray::GetRequired(int nFieldType)
{
  return CollHeaderInfo[nFieldType].m_bRequired;
}



//
//CPCompDBHeaderArray class
//////////////////////////////////////////////
CPCompDBHeaderArray::CPCompDBHeaderArray():
	CSpreadSheetHeaderArray(CPCompDBHeaderArray::ListEnd)
{
	
	
}

CPCompDBHeaderArray::~CPCompDBHeaderArray()
{
}

int CPCompDBHeaderArray::ValidateFields(CStdioFile *pLogFile)
{
   BOOL  bReportedError = FALSE;

   DetectRanges();

   if(m_pusStartColumn==NULL)
      return ERRORCODE_Fail;
   for(FieldTypes curField=TYPE;curField < ListEnd;curField++)
   {
      int nItem = CMlsStringArray::FindName(GetName(curField), CMlsStringArray::IgnoreCase);
      if(nItem >= 0)
         {
            m_pusStartColumn[curField] = nItem;
            // Track last used column in header
            // This logic assumes that last header doesn't have data that 
            // spans columns.
            if(nItem > m_nLastUsedColumn)
               m_nLastUsedColumn = nItem;
         }
		else if(!GetRequired(curField))
			{
				m_pusStartColumn[curField] = -1;
			}
      else
			{
				if(pLogFile && pLogFile->m_hFile)
					{
						CString  strError = "Unable to Locate Header: ";
						strError += GetName(curField);
						strError += "\n";
						pLogFile->WriteString(strError);
					}
				bReportedError = TRUE;
			}
   }
   if(bReportedError)
      return ERRORCODE_Fail;
   return ERRORCODE_None;

   return ERRORCODE_None;
}

LPCSTR CPCompDBHeaderArray::GetName(int nFieldType)
{
  return CompDBHeaderInfo[nFieldType].m_pszName;
}

BOOL CPCompDBHeaderArray::GetRequired(int nFieldType)
{
  return CompDBHeaderInfo[nFieldType].m_bRequired;
}
 
//
//CPCompDBHeaderArray class
//////////////////////////////////////////////
CPCompPageHeaderArray::CPCompPageHeaderArray():
	CSpreadSheetHeaderArray(CPCompPageHeaderArray::ListEnd)
{
	
	
}

CPCompPageHeaderArray::~CPCompPageHeaderArray()
{
}

int CPCompPageHeaderArray::ValidateFields(CStdioFile *pLogFile)
{
   BOOL  bReportedError = FALSE;

   DetectRanges();

   if(m_pusStartColumn==NULL)
      return ERRORCODE_Fail;
   for(FieldTypes curField=PAGENAME;curField < ListEnd;curField++)
   {
      int nItem = CMlsStringArray::FindName(GetName(curField), CMlsStringArray::IgnoreCase);
      if(nItem >= 0)
         {
            m_pusStartColumn[curField] = nItem;
            // Track last used column in header
            // This logic assumes that last header doesn't have data that 
            // spans columns.
            if(nItem > m_nLastUsedColumn)
               m_nLastUsedColumn = nItem;
         }
		else if(!GetRequired(curField))
			{
				m_pusStartColumn[curField] = -1;
			}
      else
			{
				if(pLogFile && pLogFile->m_hFile)
					{
						CString  strError = "Unable to Locate Header: ";
						strError += GetName(curField);
						strError += "\n";
						pLogFile->WriteString(strError);
					}
				bReportedError = TRUE;
			}
   }
   if(bReportedError)
      return ERRORCODE_Fail;
   return ERRORCODE_None;

   return ERRORCODE_None;
}

LPCSTR CPCompPageHeaderArray::GetName(int nFieldType)
{
  return CompPageHeaderInfo[nFieldType].m_pszName;
}

BOOL CPCompPageHeaderArray::GetRequired(int nFieldType)
{
  return CompPageHeaderInfo[nFieldType].m_bRequired;
}


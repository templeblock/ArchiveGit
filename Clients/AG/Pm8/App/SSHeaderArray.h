//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/SSHeaderArray.h 1     3/03/99 6:11p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/SSHeaderArray.h $
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
//////////////////////////////////////////////////////////////////////////////

#ifndef __SSHEADER_H__
#define __SSHEADER_H__

#include "sortarry.h"

class CSpreadSheetHeaderArray : public CMlsStringArray
{
public:
   CSpreadSheetHeaderArray(int nSize);
   virtual ~CSpreadSheetHeaderArray();
       // Locate header field positions by name, report errors to log file
   virtual int   ValidateFields(CStdioFile *pLogFile=NULL) = 0;
   int           GetStartColumn(int nFieldType);
   int           GetEndColumn(int nFieldType);
   int			  GetLastUsedColumn();
   void          SetLastUsedColumn(int nColumn);
   int           GetFieldCount()
      { return m_nFieldCount; }
   virtual LPCSTR GetName(int nFieldType) = 0;
   virtual BOOL   GetRequired(int nFieldType) = 0;
// Helpers
protected:
   FindFieldTypes();
   void     DetectRanges();      // Detects start and end ranges for headers
// Data Members
protected:
   unsigned short *  m_pusEndColumn;
   unsigned short *  m_pusStartColumn;
   int               m_nFieldCount;
   int               m_nLastUsedColumn;
};

inline int CSpreadSheetHeaderArray::GetLastUsedColumn()
{
   return m_nLastUsedColumn;
}

inline void CSpreadSheetHeaderArray::SetLastUsedColumn(int nColumn)
{
   m_nLastUsedColumn = nColumn;
}


class CCollHeaderArray : public CSpreadSheetHeaderArray
{
public:
	enum FieldTypes
	{
		SuperCategory = 0, 
		Category,
		SubCategory,
		Directory,
		FileName,
		FriendlyName,
		Tone,
		Keywords,
		ProductFlags,
		ProductCode,
		Front,
		Inside,
		Thumbnail,
		FreeSample,
		HideItem,
		CropCopyright,
		ExternalFile,			// Data will be stored in an external file (not stored in CDT)
		GraphicType,
		ListEnd
	};

   CCollHeaderArray();
   virtual ~CCollHeaderArray();

   virtual int ValidateFields(CStdioFile *pLogFile=NULL);
   virtual LPCSTR GetName(int nFieldType);
   virtual BOOL   GetRequired(int nFieldType);
};

inline CCollHeaderArray::FieldTypes operator++(CCollHeaderArray::FieldTypes &refType,int)
{
   return refType = (CCollHeaderArray::FieldTypes)(refType+1);
}


class CPCompDBHeaderArray : public CSpreadSheetHeaderArray
{
public: 
	enum FieldTypes
	{
		TYPE = 0, 
		DIRECTORY,
		FILENAME,
		RESOLUTION,
		ListEnd
	};

   CPCompDBHeaderArray();
   virtual ~CPCompDBHeaderArray();
   
	virtual int ValidateFields(CStdioFile *pLogFile=NULL);
   virtual LPCSTR GetName(int nFieldType);
   virtual BOOL   GetRequired(int nFieldType);
};

inline CPCompDBHeaderArray::FieldTypes operator++(CPCompDBHeaderArray::FieldTypes &refType,int)
{
   return refType = (CPCompDBHeaderArray::FieldTypes)(refType+1);
}


class CPCompPageHeaderArray : public CSpreadSheetHeaderArray
{
public:
	enum FieldTypes
	{
		PAGENAME = 0, 
		CONTROLNAME, 
		CONTROL,
		BTNTYPE,
		POSITION,
		SIZE,
		IMAGEFILE,
		TILED,
		CAPTION,
		TOOLTIP,
		FONTNAME,
		FONTSIZE,
		ListEnd
	};

   CPCompPageHeaderArray();
   virtual ~CPCompPageHeaderArray();
   
	virtual int ValidateFields(CStdioFile *pLogFile=NULL);
   virtual LPCSTR GetName(int nFieldType);
   virtual BOOL   GetRequired(int nFieldType);
};

inline CPCompPageHeaderArray::FieldTypes operator++(CPCompPageHeaderArray::FieldTypes &refType,int)
{
   return refType = (CPCompPageHeaderArray::FieldTypes)(refType+1);
}


#endif

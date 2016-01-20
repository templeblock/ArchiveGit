/*
// $Workfile: LABELS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LABELS.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 6     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 5     7/17/98 10:39a Jayn
// 
// 4     7/14/98 6:34p Jayn
// 
// 3     6/03/98 9:37p Hforman
// add FindLabel()
// 
// 2     3/06/98 4:48p Fredf
// Now supports creating tab-delimited spreadsheet file from .DAT file.
// 
//    Rev 1.0   14 Aug 1997 15:22:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:24   Fred
// Initial revision.
// 
//    Rev 1.2   08 Sep 1996 13:37:14   Jay
// New labels
// 
//    Rev 1.1   07 May 1996 12:44:04   Jay
// From PMW2
// 
//    Rev 1.4   03 May 1996 15:20:08   JAY
// New label reading code
// 
//    Rev 1.3   27 Sep 1995 14:41:18   FRED
// BDE Thunking
// 
//    Rev 1.2   14 Aug 1995 09:14:26   FRED
// Envelope size under preview
// 
//    Rev 1.1   26 Jul 1995 07:41:22   JAY
// Envelope support
// 
//    Rev 1.0   20 Jul 1995 12:53:08   JAY
// Initial revision.
*/

#ifndef __LABELS_H__
#define __LABELS_H__

#include "paper.h"

// Define READ_FROM_SPREADSHEET if you want the code to
// always read from the tab delimited version of the
// spreadsheet and create the .DAT file.

//#define READ_FROM_SPREADSHEET

// Define WRITE_TO_SPREADSHEET if you want the code to
// always read the .DAT file and write out a tab
// delimited version of the spreadsheet file.

//#define WRITE_TO_SPREADSHEET

// It is probably not a good idea to define both at once...

#ifdef READ_FROM_SPREADSHEET
class CMlsStringArray;
#endif

#ifdef WRITE_TO_SPREADSHEET
class CMlsStringArray;
#endif

class CLabelData : public CPaperInfo
{
public:
	ERRORCODE ReadData(StorageDevice* pDevice);
	ERRORCODE WriteData(StorageDevice* pDevice);

#ifdef READ_FROM_SPREADSHEET
	static CString GetString(CMlsStringArray& csaFields, int nField);
	static int GetInt(CMlsStringArray& csaFields, int nField);
	static CFixed GetDouble(CMlsStringArray& csaFields, int nField, CFixed lUnits);
	BOOL ReadFromFields(CMlsStringArray& pFields);
#endif
#ifdef WRITE_TO_SPREADSHEET
	static BOOL WriteFieldHeaderToFile(CStdioFile& File);
	BOOL WriteFieldsToFile(CStdioFile& File);
#endif
protected:
#ifdef READ_FROM_SPREADSHEET
	enum FieldIndex
	{
		FIELD_Name					= 0,
		FIELD_PageWidth			= 1,
		FIELD_PageHeight			= 2,
		FIELD_Columns				= 3,
		FIELD_Rows					= 4,
		FIELD_LeftMargin			= 5,
		FIELD_TopMargin			= 6,
		FIELD_Width					= 7,
		FIELD_Height				= 8,
		FIELD_WidthPlusGap		= 9,
		FIELD_HeightPlusGap		= 10,
		FIELD_CornerType			= 11,
		FIELD_RadiusX				= 12,
		FIELD_PaperType			= 13,
		FIELD_Orientation			= 14,
		FIELD_HalfPagePerfSkip	= 15,
		FIELD_HalfPageRowCount	= 16,
		FIELD_Description			= 17,
		FIELD_Units					= 18,
		NUMBER_OF_FIELDS
	};
#endif
#ifdef WRITE_TO_SPREADSHEET
	enum FieldIndex
	{
		FIELD_Name					= 0,
		FIELD_PageWidth			= 1,
		FIELD_PageHeight			= 2,
		FIELD_Columns				= 3,
		FIELD_Rows					= 4,
		FIELD_LeftMargin			= 5,
		FIELD_TopMargin			= 6,
		FIELD_Width					= 7,
		FIELD_Height				= 8,
		FIELD_WidthPlusGap		= 9,
		FIELD_HeightPlusGap		= 10,
		FIELD_CornerType			= 11,
		FIELD_RadiusX				= 12,
		FIELD_PaperType			= 13,
		FIELD_Orientation			= 14,
		FIELD_HalfPagePerfSkip	= 15,
		FIELD_HalfPageRowCount	= 16,
		FIELD_Description			= 17,
		FIELD_Units					= 18,
		NUMBER_OF_FIELDS
	};
#endif
	
	CString m_csDescription;
	
public:

	/*
	// Get the label's description.
	*/

	const CString& GetDescription(void) const
		{ return m_csDescription; }

	/*
	// Set the label's description.
	*/

	void SetDescription(LPCSTR pDescription)
		{ m_csDescription = pDescription; }
};

class CLabelList
{
public:

	virtual ~CLabelList();
	virtual BOOL ReadList(LPCSTR FileName);

	int Labels(void) const
		{ return m_cpaLabels.GetSize(); }
	CLabelData* Label(int nLabel) const
		{ return (CLabelData*)m_cpaLabels.GetAt(nLabel); }

	CLabelData* FindLabel(const CString& strLabel);

	struct CListHeader
	{
		WORD m_wVersion;
		WORD m_wCount;
	};

	enum
	{
		Version		=	1
	};

	static const DWORD m_dwStandardId;
	static const DWORD m_dwSwappedId;

protected:
	// array of CLabelData items
	CPtrArray m_cpaLabels;
};

#endif

/*
// $Workfile: LABELS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LABELS.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 6     2/01/99 1:00a Psasse
// support for PhotoProjects
// 
// 5     11/30/98 11:51a Johno
// In CLabelList::ReadList(), call to GetGlobalPathManager()->LocatePath()
// returns a CString, was being assigned to a LPCSTR. This caused it to
// get garbage, fail, and probably prevented labels from appearing in the"
// With Help" path.
// 
// 4     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 3     6/03/98 9:37p Hforman
// add FindLabel()
// 
// 2     3/06/98 4:47p Fredf
// Now supports creating tab-delimited spreadsheet file from .DAT file.
// 
//    Rev 1.1   30 Sep 1997 11:08:32   johno
// ReadList() now supports PAPER_OrientationInvalid.
// In spreadsheet:
// 
// 1          = PAPER_Portrait
// 2          = PAPER_Landscape
// All others = PAPER_OrientationInvalid
// 
//    Rev 1.0   14 Aug 1997 15:22:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:58   Fred
// Initial revision.
// 
//    Rev 1.5   14 May 1997 10:16:44   Jay
// Changes for 16-bit file handle problem.
// 
//    Rev 1.4   28 Oct 1996 10:16:22   johno
// Moved strings to resource file
// 
//    Rev 1.3   08 Sep 1996 13:37:14   Jay
// New labels
// 
//    Rev 1.2   16 Jul 1996 08:51:24   Fred
// Hooked up address/sender book
// 
//    Rev 1.1   07 May 1996 12:44:02   Jay
// From PMW2
// 
//    Rev 1.6   03 May 1996 15:20:06   JAY
// New label reading code
// 
//    Rev 1.5   27 Sep 1995 14:41:10   FRED
// BDE Thunking
// 
//    Rev 1.4   14 Aug 1995 09:14:28   FRED
// Envelope size under preview
// 
//    Rev 1.3   08 Aug 1995 12:10:32   FRED
// Open databases read-only
// 
//    Rev 1.2   26 Jul 1995 07:41:24   JAY
// Envelope support
// 
//    Rev 1.1   20 Jul 1995 16:27:22   JAY
//  
// 
//    Rev 1.0   20 Jul 1995 12:53:08   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "labels.h"
#include "file.h"
#include "util.h"
#include "sortarry.h"

/////////////////////////////////////////////////////////////////////////////
// CLabelData

ERRORCODE CLabelData::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = CPaperInfo::ReadData(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->read_cstring(m_csDescription);
	}
	return error;
}

ERRORCODE CLabelData::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = CPaperInfo::WriteData(pDevice)) == ERRORCODE_None)
	{
		error = pDevice->write_cstring(m_csDescription);
	}
	return error;
}

#ifdef READ_FROM_SPREADSHEET
CString CLabelData::GetString(CMlsStringArray& csaFields, int nField)
{
	CString csString = csaFields.GetAt(nField);
	int nLength = csString.GetLength();
	if ((nLength >= 2)
	 && (csString[0] == '"')
	 && (csString[nLength-1] == '"'))
	{
		CString csOutput;
		int nString = 0;
		int nOutput = 0;

		while (nString < nLength)
		{
			if (csString[nString] == '"')
			{
				nString++;
			}

			if (nString < nLength)
			{
				csOutput += csString[nString];
				nOutput++;
			}

			nString++;
		}

		csString = csOutput;
	}

	return csString;
}

int CLabelData::GetInt(CMlsStringArray& csaFields, int nField)
{
	return atoi(csaFields.GetAt(nField));
}

CFixed CLabelData::GetDouble(CMlsStringArray& csaFields, int nField, CFixed lUnits)
{
	CFixed lValue = MakeFixed(atof(csaFields.GetAt(nField)));
	if (lUnits != MakeFixed(1))
	{
		lValue = DivFixed(lValue, lUnits);
	}
	return lValue;
}

BOOL CLabelData::ReadFromFields(CMlsStringArray& csaFields)
{
	if (csaFields.GetSize() != NUMBER_OF_FIELDS)
	{
		// Incorrect number of fields.
		ASSERT(FALSE);
		return FALSE;
	}

	CFixed lUnits = MakeFixed(GetInt(csaFields, FIELD_Units));

	m_csName = csaFields.GetAt(FIELD_Name);

	m_Record.m_nClass = CLASS_Labels;
	m_Record.m_nMarginStyle = MARGINS_Minimal;		// For now...

	m_Record.m_nType = GetInt(csaFields, FIELD_PaperType);

/*
	m_Record.m_nOrientation = (GetInt(csaFields, FIELD_Orientation) == 0)
											? PAPER_Landscape
											: PAPER_Portrait;
*/
   switch (GetInt(csaFields, FIELD_Orientation))
   {
      case 1:
      m_Record.m_nOrientation = PAPER_Portrait;
      break;
      
      case 2:
      m_Record.m_nOrientation = PAPER_Landscape;
      break;
      
      case 3:
      m_Record.m_nOrientation = 3;
      break;
      
      default:
      m_Record.m_nOrientation = PAPER_OrientationInvalid; 
      break;
   }

	m_Record.m_PaperDims.x = GetDouble(csaFields, FIELD_PageWidth, lUnits);
	m_Record.m_PaperDims.y = GetDouble(csaFields, FIELD_PageHeight, lUnits);

	m_Record.m_nSlotClass = GetInt(csaFields, FIELD_CornerType);

	m_Record.m_nSlotsAcross = GetInt(csaFields, FIELD_Columns);
	m_Record.m_nSlotsDown = GetInt(csaFields, FIELD_Rows);

	m_Record.m_SlotOrigin.x = GetDouble(csaFields, FIELD_LeftMargin, lUnits);
	m_Record.m_SlotOrigin.y = GetDouble(csaFields, FIELD_TopMargin, lUnits);

	m_Record.m_SlotDims.x = GetDouble(csaFields, FIELD_Width, lUnits);
	m_Record.m_SlotDims.y = GetDouble(csaFields, FIELD_Height, lUnits);

	m_Record.m_SlotStep.x = GetDouble(csaFields, FIELD_WidthPlusGap, lUnits);
	m_Record.m_SlotStep.y = GetDouble(csaFields, FIELD_HeightPlusGap, lUnits);

	m_Record.m_Radius = GetDouble(csaFields, FIELD_RadiusX, lUnits);

	m_Record.m_HalfPagePerfSkip = GetDouble(csaFields, FIELD_HalfPagePerfSkip, lUnits);

	m_Record.m_nHalfPageRows = GetInt(csaFields, FIELD_HalfPageRowCount);

	m_csDescription = csaFields.GetAt(FIELD_Description);

	/*
	// Set the desired margins to something.
	*/

	CFixed lSqrt2 = MakeFixed(1.5);		// sqrt2 - slight spacing

	switch (m_Record.m_nSlotClass)
	{
		case SLOT_Rounded:
		{
		/*
		// For now, try the desired margins at the size of the rounded corners.
		*/
			CFixed r = m_Record.m_Radius;

			m_Record.m_MinimumMargins.Left = 
				m_Record.m_MinimumMargins.Top =
				m_Record.m_MinimumMargins.Right = 
				m_Record.m_MinimumMargins.Bottom = r - DivFixed(r, lSqrt2);
			break;
		}
		case SLOT_Circular:
		{
			CFixed x = m_Record.m_SlotDims.x/2;
			CFixed y = m_Record.m_SlotDims.y/2;

			m_Record.m_MinimumMargins.Left = 
				m_Record.m_MinimumMargins.Right = x - DivFixed(x, lSqrt2);
			m_Record.m_MinimumMargins.Top =
				m_Record.m_MinimumMargins.Bottom = y - DivFixed(y, lSqrt2);
			break;
		}
		case SLOT_Avery5385:
		case SLOT_Avery5386:
		{
			m_Record.m_MinimumMargins.Left = 
				m_Record.m_MinimumMargins.Right =
				m_Record.m_MinimumMargins.Top =
				m_Record.m_MinimumMargins.Bottom = MakeFixed(1)/8;
		// Leave room for the slits on the bottom.
			m_Record.m_MinimumMargins.Bottom += MakeFixed(1)/2;
			break;
		}
//		case SLOT_Rectangular:
		default:
		{
			m_Record.m_MinimumMargins.Left = 
				m_Record.m_MinimumMargins.Right =
				m_Record.m_MinimumMargins.Top =
				m_Record.m_MinimumMargins.Bottom = MakeFixed(1)/8;
			break;
		}
	}

/* Now go through and compute the margins to use. */

	ComputeMargins();

	return TRUE;
}
#endif

#ifdef WRITE_TO_SPREADSHEET
BOOL CLabelData::WriteFieldHeaderToFile(CStdioFile& File)
{
	BOOL fResult = FALSE;
	CString csFieldHeader;
	TRY
	{
		csFieldHeader.Format(
			"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
			"Name",						// Name
			"Page Width",				// Page Width
			"Page Height",				// Page Height
			"Columns",					// Columns
			"Rows",						// Rows
			"Left Margin",				// Left Margin
			"Right Margin",			// Top Margin
			"Width",						// Width
			"Height",					// Height
			"Width plus gap",			// Width plus gap
			"Height plus gap",		// Height plus gap
			"Corner type",				// Corner type
			"Radius X",					// Radius X
			"Paper type",				// Paper type
			"Orientation",				// Orientation
			"Half page perf skip",	// Half page perf skip
			"Half page row count",	// Half page row count
			"Description",				// Description
			"Units");					// Units
		File.WriteString(csFieldHeader);
		fResult = TRUE;
	}
	END_TRY
	return fResult;
}

BOOL CLabelData::WriteFieldsToFile(CStdioFile& File)
{
	BOOL fResult = FALSE;
	CString csFields;
	TRY
	{
		csFields.Format(
			"%s\t%f\t%f\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%d\t%f\t%d\t%d\t%f\t%d\t%s\t%d\n",
			(LPCSTR)m_csName,													// Name
			MakeDouble(m_Record.m_PaperDims.x),							// Page Width
			MakeDouble(m_Record.m_PaperDims.y),							// Page Height
			m_Record.m_nSlotsAcross,										// Columns
			m_Record.m_nSlotsDown,											// Rows
			MakeDouble(m_Record.m_SlotOrigin.x),						// Left Margin
			MakeDouble(m_Record.m_SlotOrigin.y),						// Top Margin
			MakeDouble(m_Record.m_SlotDims.x),							// Width
			MakeDouble(m_Record.m_SlotDims.y),							// Height
			MakeDouble(m_Record.m_SlotStep.x),							// Width plus gap
			MakeDouble(m_Record.m_SlotStep.y),							// Height plus gap
			m_Record.m_nSlotClass,											// Corner type
			MakeDouble(m_Record.m_Radius),								// Radius X
			m_Record.m_nType,													// Paper type
			(m_Record.m_nOrientation == PAPER_Landscape) ? 0 : 1,	// Orientation
			MakeDouble(m_Record.m_HalfPagePerfSkip),					// Half page perf skip
			m_Record.m_nHalfPageRows,										// Half page row count
			(LPCSTR)m_csDescription,										// Description
			1);																	// Units (always 1)
		File.WriteString(csFields);
		fResult = TRUE;
	}
	END_TRY
	return fResult;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CLabelList

const DWORD CLabelList::m_dwStandardId =		 (DWORD)'L'
													| (((DWORD)'B')<<8)
													| (((DWORD)'L')<<16)
													| (((DWORD)'S')<<24);
const DWORD CLabelList::m_dwSwappedId =			 (DWORD)'S'
													| (((DWORD)'L')<<8)
													| (((DWORD)'B')<<16)
													| (((DWORD)'L')<<24);

CLabelList::~CLabelList()
{
	int nLabels = Labels();

	for (int nLabel = 0; nLabel < nLabels; nLabel++)
	{
		delete Label(nLabel);
	}
}

/*
// Swap a word of data.
*/

static void SwapWord(WORD* w)
{
#ifdef _WIN32
	BYTE tmp = ((BYTE*)w)[0];
	((BYTE*)w)[0] = ((BYTE*)w)[1];
	((BYTE*)w)[1] = tmp;
#else
	_asm
	{
		les	bx, dword ptr w
		mov	ax, es:[bx]
		xchg	ah, al
		mov	es:[bx], ax
	}
#endif
}

BOOL CLabelList::ReadList(LPCSTR pFileName)
{
	BOOL fSuccess = FALSE;
#ifdef READ_FROM_SPREADSHEET
#ifndef WIN32
	// This should not compile.
	>>> "CStdioFile has problems in 16-bit!") <<<
#endif
	pFileName = GetGlobalPathManager()->LocatePath(pFileName);

	CString csPath;
	CString csDrive, csDirectory, csFile;
	CString csLine;
	CStdioFile File;
	CMlsStringArray csaFields;
	CLabelData* pLabelData = NULL;

	TRY
	{
		Util::SplitPath(pFileName, &csDrive, &csDirectory, &csFile, NULL);
		Util::ConstructPath(csPath, csDrive, csDirectory, csFile, ".TXT");

		TRACE("Open spreadsheet %s for label names.\n", pFileName);

		// Open file.
		if (File.Open(csPath, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
		{
			// Read the format line.
			if (File.ReadString(csLine))
			{
				// Now read the rest of the lines.
				while (File.ReadString(csLine))
				{
					csaFields.SetNames(csLine, '\t');
					pLabelData = new CLabelData;

					if (pLabelData->ReadFromFields(csaFields))
					{
						//if (pLabelData->Orientation() != CPaperInfo::PAPER_Landscape)
                  if (pLabelData->Orientation() != CPaperInfo::PAPER_OrientationInvalid)
						{
							m_cpaLabels.Add(pLabelData);
							pLabelData = NULL;
						}
					}
					// If we have a label now, delete it!
					delete pLabelData;
					pLabelData = NULL;
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	delete pLabelData;

	TRACE("Got %d labels\n", Labels());

	StorageFile OutFile(pFileName);

	DWORD dwID = m_dwStandardId;

	CListHeader Header;

	memset(&Header, 0, sizeof(Header));
	Header.m_wVersion = Version;
	Header.m_wCount = Labels();

	if (OutFile.initialize() == ERRORCODE_None
		 && OutFile.write(&dwID, sizeof(dwID)) == ERRORCODE_None
		 && OutFile.write_record(&Header, sizeof(Header)) == ERRORCODE_None)
	{
		/* Write out each entry. */
		WORD w;
		for (w = 0; w < Header.m_wCount; w++)
		{
			CLabelData* pLabel = Label(w);
			if (pLabel == NULL
				 || pLabel->WriteData(&OutFile) != ERRORCODE_None)
			{
				break;
			}
		}

		if (w == Header.m_wCount)
		{
			fSuccess = TRUE;
		}
	}
	if (!fSuccess)
	{
	/* Get rid of the file. */
		OutFile.flush();
		OutFile.zap();
		//AfxMessageBox(IDS_NO_WRITE_LABEL_FILE);
      AfxMessageBox("Insert IDS_NO_WRITE_LABEL_FILE here");
	}

#else
/*
// Read the field names into our cache.
*/
	CString FileName = GetGlobalPathManager()->LocatePath(pFileName);

	TRACE("Open file %s for label names.\n", FileName);
	ReadOnlyFile File(FileName);

	DWORD dwID;
	if (File.read(&dwID, sizeof(dwID)) == ERRORCODE_None)
	{
		if (dwID == m_dwStandardId || dwID == m_dwSwappedId)
		{
		/* We have a potentially valid file. */
			CListHeader Header;

			if (File.read_record(&Header, sizeof(Header)) == ERRORCODE_None)
			{
				if (dwID == m_dwSwappedId)
				{
				/*
				// We are swapped. Swap the header.
				// Is is assumed that the objects handle their own swapping
				// to native format.
				*/
					SwapWord(&Header.m_wVersion);
					SwapWord(&Header.m_wCount);
				}

				// Read all the labels.

				WORD w;
				for (w = 0; w < Header.m_wCount; w++)
				{
					CLabelData* pLabelData = new CLabelData;

					if (pLabelData->ReadData(&File) != ERRORCODE_None)
					{
						delete pLabelData;
						break;
					}

					//ASSERT(pLabelData->Orientation() != CPaperInfo::PAPER_Landscape);
               ASSERT(pLabelData->Orientation() != CPaperInfo::PAPER_OrientationInvalid);
					m_cpaLabels.Add(pLabelData);
				}
				if (w == Header.m_wCount)
				{
					ASSERT(Labels() == (int)Header.m_wCount);
					TRACE("Got %d labels\n", Labels());

					fSuccess = TRUE;
				}
			}
		}
	}

#ifdef WRITE_TO_SPREADSHEET
	CString csPath;
	CString csDrive, csDirectory, csFile;
	CString csLine;
	CStdioFile TextFile;
	CMlsStringArray csaFields;
	CLabelData* pLabelData = NULL;

	TRY
	{
		Util::SplitPath(FileName, &csDrive, &csDirectory, &csFile, NULL);
		Util::ConstructPath(csPath, csDrive, csDirectory, csFile, ".TXT");

		TRACE("Open spreadsheet %s for label names.\n", (LPCSTR)csPath);

		// Open file.
		if (TextFile.Open(csPath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone | CFile::typeText))
		{
			// Write out the header.
			if (CLabelData::WriteFieldHeaderToFile(TextFile))
			{
				// Write out each label.
				for (int nLabel = 0; nLabel < Labels(); nLabel++)
				{
					CLabelData* pLabel = Label(nLabel);
					if (pLabel != NULL)
					{
						pLabel->WriteFieldsToFile(TextFile);
					}
				}
			}

			TextFile.Close();
		}
	}
	END_TRY
#endif

#endif

	return fSuccess;
}

CLabelData* CLabelList::FindLabel(const CString& strLabel)
{
	for (int nLabel = 0; nLabel < m_cpaLabels.GetSize(); nLabel++)
	{
		CLabelData* pLabel = (CLabelData*)m_cpaLabels[nLabel];
		if (pLabel->GetName() == strLabel)
		{
			return pLabel;
		}
	}

	return NULL;
}

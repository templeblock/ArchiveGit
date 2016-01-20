#include "stdafx.h"
#include "TrueType.h"
#include <atlfile.h>
#include "RegKeys.h"
#include <shlobj.h> // for CSIDL_*

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CString CTrueType::GetPathFromFileName(LPCTSTR pszFileName)
{
	// Compute the font file location
#define CString ATL::CAtlString
	CString strPath;
	GetSpecialFolderLocation(strPath, CSIDL_FONTS);
#undef CString
	strPath += '\\';
	strPath += pszFileName;
	return strPath;
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetFaceNameFromFileName(LPCTSTR pszFileName, CString& strFaceName)
{
	strFaceName.Empty();
	CString strPath = GetPathFromFileName(pszFileName);
	TTF_PROPERTIES FontProps;
	if (!CTrueType::GetProperties(strPath, &FontProps))
	{
		strFaceName.Empty();
		return false;
	}
	
	strFaceName = FontProps.strFamily;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetFaceNameFromDisplayName(LPCTSTR pszDisplayName, CString& strFaceName)
{
	return RegistryLookup(pszDisplayName, TTF_DisplayName, strFaceName, TTF_FaceName);
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetFileNameFromDisplayName(LPCTSTR pszDisplayName, CString& strFileName)
{
	return RegistryLookup(pszDisplayName, TTF_DisplayName, strFileName, TTF_FileName);
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetFileNameFromFaceName(LPCTSTR pszFaceName, CString& strFileName)
{
	return RegistryLookup(pszFaceName, TTF_FaceName, strFileName, TTF_FileName);
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetDisplayNameFromFileName(LPCTSTR pszFileName, CString& strDisplayName)
{
	return RegistryLookup(pszFileName, TTF_FileName, strDisplayName, TTF_DisplayName);
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetDisplayNameFromFaceName(LPCTSTR pszFaceName, CString& strDisplayName)
{
	return RegistryLookup(pszFaceName, TTF_FaceName, strDisplayName, TTF_DisplayName);
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::RegistryLookup(LPCTSTR pszTarget, TTF_DATA_TYPE TargetType, CString& strResult, TTF_DATA_TYPE ResultType)
{
	strResult.Empty();

	CString strTarget = pszTarget;
	if (strTarget.IsEmpty())
		return false;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	bool bNT = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);

	CRegKey regkey;
	if (regkey.Open(HKEY_LOCAL_MACHINE, (bNT ? REGKEY_NTFONT : REGKEY_FONT)) != ERROR_SUCCESS)
		return false;

	char strDisplayName[MAX_PATH];
	DWORD dwDisplayNameLength = sizeof(strDisplayName)-1;
	char strFileName[MAX_PATH];
	DWORD dwFileNameLength = sizeof(strFileName)-1;

	bool bFound = false;

	if (TargetType == TTF_DisplayName)
	{
		CString strTrueType = " (TrueType)";
		if (strTarget.Find(strTrueType) < 0)
			strTarget += strTrueType;

		strcpy(strDisplayName, strTarget);
		bFound = (regkey.QueryStringValue(strDisplayName, strFileName, &dwFileNameLength) == ERROR_SUCCESS);
	}
	else
	{
		DWORD dwType = 0;
		DWORD dwIndex = 0;
		while (::RegEnumValue(regkey, dwIndex, strDisplayName, &dwDisplayNameLength, NULL, &dwType, (BYTE*)strFileName, &dwFileNameLength) == ERROR_SUCCESS)
		{
			dwIndex++;
			dwDisplayNameLength = sizeof(strDisplayName)-1;
			dwFileNameLength = sizeof(strFileName)-1;
			if (dwType != REG_SZ)
				continue;

			CString strValue;
			if (TargetType == TTF_FileName)
				strValue = strFileName;
			else
			if (TargetType == TTF_FaceName)
			{
			 // Because GetFaceNameFromFileName() can be slow, assume that this is not a match 
			 // unless the first 2 characters of the target FaceName match the DisplayName
				if (strTarget[0] != strDisplayName[0] || strTarget[1] != strDisplayName[1])
					continue;

				GetFaceNameFromFileName(strFileName, strValue);
			}

			bFound = (strTarget.CompareNoCase(strValue) == 0);
			if (bFound)
				break;
		}
	}

	if (!bFound)
		return false;

	if (ResultType == TTF_DisplayName)
		strResult = strDisplayName;
	else
	if (ResultType == TTF_FileName)
		strResult = strFileName;
	else
	if (ResultType == TTF_FaceName)
		GetFaceNameFromFileName(strFileName, strResult);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::AddToRegistry(LPCTSTR pszDisplayName, LPCTSTR pszFileName)
{
	if (!pszFileName)
		return false;

	CString strDisplayName = pszDisplayName;
	if (strDisplayName.IsEmpty())
		return false;

	CString strTrueType = " (TrueType)";
	if (strDisplayName.Find(strTrueType) < 0)
		strDisplayName += strTrueType;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	bool bNT = (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);

	CRegKey regkey;
	if (regkey.Open(HKEY_LOCAL_MACHINE, (bNT ? REGKEY_NTFONT : REGKEY_FONT)) != ERROR_SUCCESS)
		return false;

	return (regkey.SetStringValue(strDisplayName, pszFileName) == ERROR_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
typedef struct
{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
} TTF_OFFSET_TABLE;

typedef struct
{
	char	szTag[4];			// Table name
	ULONG	uCheckSum;			// Check sum
	ULONG	uOffset;			// Offset from beginning of file
	ULONG	uLength;			// Length of the table in bytes
} TTF_TABLE_DIRECTORY;

typedef struct
{
	USHORT	uFSelector;			// Format selector; Always 0
	USHORT	uNRCount;			// Name Records count
	USHORT	uStorageOffset;		// Offset for strings storage, from start of the table
} TTF_NAME_TABLE_HEADER;

typedef struct
{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;		// Offset from the start of storage area
} TTF_NAME_RECORD;

typedef struct
{
	USHORT	uVersion;
	short	uAverageCharWidth;
	USHORT	uWeightClass;
	USHORT	uWidthClass;
	USHORT	uFsType;
	short	ySubscriptXSize;
	short	ySubscriptYSize;
	short	ySubscriptXOffset;
	short	ySubscriptYOffset;
	short	ySuperscriptXSize;
	short	ySuperscriptYSize;
	short	ySuperscriptXOffset;
	short	ySuperscriptYOffset;
	short	yStrikeoutSize;
	short	yStrikeoutPosition;
	USHORT	sFamilyClass;
	BYTE	panose[10];
	ULONG	ulUnicodeRange[4];
	char	achVendID[4];
	USHORT	fsSelection;
	USHORT	usFirstCharIndex;
	USHORT	usLastCharIndex;
	short	sTypoAscender;
	short	sTypoDescender;
	short	sTypoLineGap;
	USHORT	usWinAscent;
	USHORT	usWinDescent;
	//ULONG	ulCodePageRange[2]; // only in version 1 tables
} TTF_OS2_TABLE;

#define SWAPWORD(x)	MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)	MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

/////////////////////////////////////////////////////////////////////////////
bool CTrueType::GetProperties(LPCTSTR pszFilePath, TTF_PROPERTIES* pProperties)
{
	CAtlFile FontFile;
	HRESULT hr = FontFile.Create(pszFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (FAILED(hr))
		return false;

	TTF_OFFSET_TABLE ttOffsetTable;
	FontFile.Read(&ttOffsetTable, sizeof(TTF_OFFSET_TABLE));
	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	// See if this is a true type font and the version is 1.0
	if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		return false;
	
	TTF_TABLE_DIRECTORY tblDir;
	TTF_TABLE_DIRECTORY tblName;
	TTF_TABLE_DIRECTORY tblOS2;
	bool bFoundNameTable = false;
	bool bFoundOS2Table = false;
	for (int i=0; i< ttOffsetTable.uNumOfTables; i++)
	{
		FontFile.Read(&tblDir, sizeof(TTF_TABLE_DIRECTORY));
		CString strName = CString(tblDir.szTag, 4);
		if (strName.IsEmpty())
			break;

		if (!strName.CompareNoCase("name"))
		{
			bFoundNameTable = true;
			tblName = tblDir;
			tblName.uLength = SWAPLONG(tblName.uLength);
			tblName.uOffset = SWAPLONG(tblName.uOffset);
		}
		else
		if (!strName.CompareNoCase("OS/2"))
		{
			bFoundOS2Table = true;
			tblOS2 = tblDir;
			tblOS2.uLength = SWAPLONG(tblOS2.uLength);
			tblOS2.uOffset = SWAPLONG(tblOS2.uOffset);
		}

		if (bFoundNameTable && bFoundOS2Table)
			break;
	}
	
	if (bFoundNameTable)
	{
		FontFile.Seek(tblName.uOffset, FILE_BEGIN);

		TTF_NAME_TABLE_HEADER ttNTHeader;
		FontFile.Read(&ttNTHeader, sizeof(TTF_NAME_TABLE_HEADER));
		ttNTHeader.uFSelector      = SWAPWORD(ttNTHeader.uFSelector);
		ttNTHeader.uNRCount        = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset  = SWAPWORD(ttNTHeader.uStorageOffset);

		for (int i=0; i<ttNTHeader.uNRCount; i++)
		{
			TTF_NAME_RECORD ttRecord;
			FontFile.Read(&ttRecord, sizeof(TTF_NAME_RECORD));
			ttRecord.uPlatformID   = SWAPWORD(ttRecord.uPlatformID);
			ttRecord.uEncodingID   = SWAPWORD(ttRecord.uEncodingID);
			ttRecord.uLanguageID   = SWAPWORD(ttRecord.uLanguageID);
			ttRecord.uNameID       = SWAPWORD(ttRecord.uNameID);
			ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
			ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

			if (ttRecord.uPlatformID != 3) // Microsoft
				continue;

			ULONGLONG nPos = 0;
			FontFile.GetPosition(nPos);
			FontFile.Seek(tblName.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, FILE_BEGIN);

			int iUnicodeChars = (ttRecord.uStringLength + 1) / 2;
			WCHAR* pUnicodeBuffer = new WCHAR[iUnicodeChars];
			FontFile.Read(pUnicodeBuffer, ttRecord.uStringLength);
			for (int i = 0; i < iUnicodeChars; i++)
				pUnicodeBuffer[i] = SWAPWORD(pUnicodeBuffer[i]);

			CString strName = CString(pUnicodeBuffer, iUnicodeChars);
			delete [] pUnicodeBuffer;
			FontFile.Seek(nPos, FILE_BEGIN);
			
			if (strName.IsEmpty())
				continue;

			switch (ttRecord.uNameID)
			{
				case 1: // Font family
					pProperties->strFamily.IsEmpty() ? pProperties->strFamily = strName : void();
					break;
				case 0: // Copyright notice
					pProperties->strCopyright.IsEmpty() ? pProperties->strCopyright = strName : void();
					break;
				case 7: // Trademark notice
					pProperties->strTrademark.IsEmpty() ? pProperties->strTrademark = strName : void();
					break;
				case 4: // Full Name of the Font
					pProperties->strName.IsEmpty() ? pProperties->strName = strName : void();
					break;
				case 2: // Font sub family
				case 3: // Unique Family Identifier
				case 5: // Version of the name table
				case 6: // PostScript name of the font
				default:
				{
					break;
				}
			}
		}			
	}

	if (bFoundOS2Table)
	{
		FontFile.Seek(tblOS2.uOffset, FILE_BEGIN);

		TTF_OS2_TABLE ttNTHeader;
		FontFile.Read(&ttNTHeader, sizeof(TTF_OS2_TABLE));
		ttNTHeader.uVersion          = SWAPWORD(ttNTHeader.uVersion);
		ttNTHeader.uAverageCharWidth = SWAPWORD(ttNTHeader.uAverageCharWidth);
		ttNTHeader.uWeightClass      = SWAPWORD(ttNTHeader.uWeightClass);
		ttNTHeader.uWidthClass       = SWAPWORD(ttNTHeader.uWidthClass);
		ttNTHeader.uFsType           = SWAPWORD(ttNTHeader.uFsType);
	
		if (ttNTHeader.uFsType & 0x0001)	// 1: reserved - must be zero; if not, turn it off
			ttNTHeader.uFsType &= ~0x0001;

		if (ttNTHeader.uFsType == 0x0000)	// 0: embedding and permanent installation allowed
		{
			pProperties->strEmbed = "Installable";
			pProperties->enumEmbed = TTF_Embed_Installable;
		}
		else
		if (ttNTHeader.uFsType & 0x0008)	// 8: editable embedding allowed
		{
			pProperties->strEmbed = "Editable";
			pProperties->enumEmbed = TTF_Embed_Editable;
		}
		else
		if (ttNTHeader.uFsType & 0x0004)	// 4: preview and print embedding allowed
		{
			pProperties->strEmbed = "Printable";
			pProperties->enumEmbed = TTF_Embed_Printable;
		}
		else
		if (ttNTHeader.uFsType & 0x0002)
		{
			pProperties->strEmbed = "None"; // 2: no embedding allowed
			pProperties->enumEmbed = TTF_Embed_None;
		}
		else
		{
			pProperties->strEmbed.Format("%d", ttNTHeader.uFsType); // unknown
			pProperties->enumEmbed = TTF_Embed_Unknown;
		}
	}

	FontFile.Close();

	return !pProperties->strName.IsEmpty();
}

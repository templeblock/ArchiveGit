#pragma once

typedef enum 
{
	TTF_Embed_Unknown     = -1,
	TTF_Embed_Installable = 0,	// permanent installation embedding allowed
	TTF_Embed_Editable    = 8,	// editable embedding allowed
	TTF_Embed_Printable   = 4,	// preview and print embedding allowed
	TTF_Embed_None        = 2,	// no embedding allowed
} TTF_EMBEDDING;

typedef struct
{
	CString strName;
	CString strCopyright;
	CString strTrademark;
	CString strFamily;
	CString strEmbed;
	TTF_EMBEDDING enumEmbed;
} TTF_PROPERTIES;

typedef enum 
{
	TTF_FaceName = 0,
	TTF_FileName,
	TTF_DisplayName,
} TTF_DATA_TYPE;

/////////////////////////////////////////////////////////////////////////////
class CTrueType
{
public:
	CTrueType() {}
	~CTrueType() {}
	static CString GetPathFromFileName(LPCTSTR pszFileName);
	static bool GetFaceNameFromFileName(LPCTSTR pszFileName, CString& strFaceName);
	static bool GetFaceNameFromDisplayName(LPCTSTR pszDisplayName, CString& strFaceName);
	static bool GetFileNameFromDisplayName(LPCTSTR pszDisplayName, CString& strFileName);
	static bool GetFileNameFromFaceName(LPCTSTR pszFaceName, CString& strFileName);
	static bool GetDisplayNameFromFileName(LPCTSTR pszFileName, CString& strDisplayName);
	static bool GetDisplayNameFromFaceName(LPCTSTR pszFaceName, CString& strDisplayName);
	static bool AddToRegistry(LPCTSTR pszDisplayName, LPCTSTR pszFileName);
	static bool GetProperties(LPCTSTR pszFilePath, TTF_PROPERTIES* pProperties);
private:
	static bool RegistryLookup(LPCTSTR pszTarget, TTF_DATA_TYPE TargetType, CString& strResult, TTF_DATA_TYPE ResultType);
};

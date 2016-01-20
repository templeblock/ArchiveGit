#ifndef _YEARDLL_H_
#define _YEARDLL_H_

//  Error codes definitions
#define	ERR_STATUSOK				(0)			// All is going OK!!!
#define	ERR_DATANOTFOUND			(-1)		// Database contains no data to provide on request
#define	ERR_RETRSTRING				(-2)		// Error while retriving string data
#define	ERR_RETRMETAFILE			(-3)		// Error while retrieving metafile data
#define	ERR_RETRMEMO				(-4)		// Error while retrieving memo data
#define	ERR_METAFILESAVE			(-5)		// Error saving mefafile to temporary file
#define	ERR_INITIALIZING			(-6)		// Error initializing DLL
#define	ERR_BADDATEPARAM			(-7)		// Invalid date parameter passed
#define	ERR_TABLENOTFOUND			(-8)		// Database component table not found
#define	ERR_CURRUPTTABLE			(-9)		// Bad table file
#define	ERR_UNKNOWNERROR			(-10)		// Unknown error
#define	ERR_NOTHINGTODO				(-11)		// Nothing to do
#define	ERR_GETCATEGORIES			(-12)		// Error while GetCategories call
#define	ERR_INVALIDBUFFER			(-13)		// Invalid buffer pointer passed
#define	ERR_NOTSUPPORTED			(-14)		// Additional category code not supported
#define	ERR_INVALIDERRORCODE		(-15)		// Invalid error code detected
#define	ERR_DATEANDNAMENOTSET		(-16)		// Valid date and name have not been set

#define	ERR_DLLNOTFOUND				(-100)	// DLL not found
#define	ERR_ENTRYPOINTNOTFOUND		(-101)	// DLL entry point not found
#define	ERR_INVALIDINTERFACE		(-102)	// Interface object is not valid

//	Standard categories codes
#define	catTimeArticle 				(1)
#define	catNewsArticle 				(2)
#define	catFamousArticle 			(3)
#define	catZodiacArticle			(4)
#define	catChineseYearArticle 		(5)
#define	catEntertainmentArticle		(6)
#define	catSportArticle 			(7)
#define	catYearTitle 				(8)
#define	catIntroductionTitle 		(9)
#define	catNewsTitle 				(10)
#define	catFamousTitle 				(11)
#define	catZodiacTitle 				(12)
#define	catChineseYearTitle   		(13)
#define	catBirthstoneTitle 			(14)
#define	catEntertainmentTitle 		(15)
#define	catSportTitle 				(16)
#define	catZodiacGraphic 			(17)
#define	catChineseYearGraphic 		(18)
#define	catBorderGraphic 			(19)

#define catFirstExternal			(20)

//	Category data structure
struct CategoryData
{
	DWORD	dwCategoryCode;
	char	szCategoryName[256];
	DWORD	dwCategoryType;
};

// Type definitions for entry points.
typedef void (WINAPI *FN_StartUp)(LPCSTR pszStandardPath, LPCSTR pszAdditionalPath);
typedef void (WINAPI *FN_ShutDown)(void);
typedef void (WINAPI *FN_SetDateAndName)(SYSTEMTIME Birthdate, LPCSTR pszName);
typedef int (WINAPI *FN_GetData)(DWORD dwCategory, LPSTR pszBuffer);
typedef int (WINAPI *FN_GetCategories)(CategoryData* pCategories);
typedef int (WINAPI *FN_GetErrorCode)(void);
typedef int (WINAPI *FN_GetErrorString)(LPSTR pszBuffer);

class CYearDllInterface
{
public:
	virtual int Release(void) = 0;
	virtual int SetDateAndName(SYSTEMTIME Birthdate, LPCSTR pszName) = 0;
	virtual int GetData(DWORD dwCategory, CString& csData) = 0;
	virtual int GetCategories(CategoryData* pCategories) = 0;
	virtual int GetErrorCode(void) = 0;
	virtual int GetErrorString(CString& csError) = 0;
};

class CYearDll : public CYearDllInterface
{
public:
	CYearDll();
	virtual ~CYearDll();

public:
	int Initialize(LPCSTR pszDllPath, LPCSTR pszStandardPath, LPCSTR pszAdditionalPath, CYearDllInterface*& pInterface);

protected:
	void FreeDll(void);

protected:
	HINSTANCE m_hLibrary;
	BOOL m_fInitialized;
	FN_StartUp m_pStartUp;
	FN_ShutDown m_pShutDown;
	FN_SetDateAndName m_pSetDateAndName;
	FN_GetData m_pGetData;
	FN_GetCategories m_pGetCategories;
	FN_GetErrorCode m_pGetErrorCode;
	FN_GetErrorString m_pGetErrorString;

public:
	virtual int Release(void);
	virtual int SetDateAndName(SYSTEMTIME Birthdate, LPCSTR pszName);
	virtual int GetData(DWORD dwCategory, CString& csData);
	virtual int GetCategories(CategoryData* pCategories);
	virtual int GetErrorCode(void);
	virtual int GetErrorString(CString& csError);
};

#endif

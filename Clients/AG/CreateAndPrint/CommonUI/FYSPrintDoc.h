#pragma once

#include "resource.h"
#include "AGDC.h"
#include "AGDoc.h"
#include "AGSym.h"

template <class T> class FileStructEqualHelper  : public CSimpleArrayEqualHelper<class T>
{
public:
	static bool IsEqual(const T& t1, const T& t2)
	{
		return (*t1 == *t2);
	}
};

typedef CSimpleArray<FileStruct*, FileStructEqualHelper<FileStruct*> > FileSpecsArray;

class CFYSPrintDoc
{
public:
	CFYSPrintDoc(CAGDoc* pAGDoc);
	~CFYSPrintDoc();

public:
	bool Error() { return m_bError; }
	bool CreateXmlDoc(CString& strOrderId, CString& strCorrId, CString& strFYSInfo, bool bUnpackFiles = false);
	bool SendXmlDoc(CString strURL);
	bool FindFileSpec(FileStruct* pfstruct);
	bool AddFileSpec(FileStruct* pfstruct);

	void SetError(LPCSTR szErrorMsg);
	CString GetErrorMsg();

	void SetXmlPath(LPCTSTR pszTempPath);
	CString GetXmlPath();

private:
	void Initialize();
	void CleanUp();
	void CleanUpEx(LPCSTR szFile);
	void UnpackFiles();
	bool CreateSendFile();
	bool WriteXmlStreamHeader(CString& strXmlPath);
	bool WriteCreatePrintXml(LPCSTR strDocFileName, CString& strXmlPath);
	bool GetResourcesElement();
	bool GetConfigurationElement(CString& strFYSInfo);
	bool PageHasSymbols(CAGPage* pPage);
	bool CopyFiles(LPCSTR strSrcFile, FILE* output);
	bool CreateFontFile(CString strFontFile, CString strFontPath);
	bool DumpImageFile(CAGSymImage* pSymImage, CString& strImagePath);
	bool DumpFontFiles(CString& strFontsPath);
	bool WriteDIB(BYTE* pMemory, FILE* imgout);
	bool CreateDirAndSubDirs();

	FileStruct* GetMRAImageFileSpec();
	FileStruct* GetFontFileSpec(int& Ndx);

private:
	VOID* m_pConfig;
	VOID* m_pRes;

	FileSpecsArray m_arFileSpecs;

	CAGDoc* m_pAGDoc;
	CString m_strFYSTempPath;
	CString m_strErrorMsg;

	bool m_bError;
};
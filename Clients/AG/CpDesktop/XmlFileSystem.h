#pragma once

#include <comdef.h>
#include <comdefsp.h>

class CXmlFileSystem
{
public:
	CXmlFileSystem(void)
	{
		hr = m_pDoc.CreateInstance(CLSID_DOMDocument);
		m_strFileSpec = _T("*.*");
	}
	
	~CXmlFileSystem(void)
	{
	}

public:

	/////////////////////////////////////////////////////////////////////////////
	void SetFileSpec(LPCSTR pFileSpec)
	{
		m_strFileSpec = pFileSpec;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	bool CreateXml()
	{
		BROWSEINFO binfo;
		binfo.hwndOwner = NULL;
		binfo.pidlRoot = NULL;
		binfo.pszDisplayName = NULL;
		binfo.lpszTitle = (LPCTSTR)"Select the Root Content Directory";
		binfo.ulFlags = BIF_RETURNONLYFSDIRS;
		binfo.lpfn = NULL;
		binfo.lParam = 0;
		binfo.iImage = 0;

		LPITEMIDLIST pItemID = SHBrowseForFolder(&binfo);	
		if (!pItemID)
			return false;

		TCHAR szPath[MAX_PATH];
		*szPath = _T('\0');
		SHGetPathFromIDList(pItemID, szPath);
		CString strPath = szPath;

		return CreateXml(strPath);
	}

	/////////////////////////////////////////////////////////////////////////////
	bool CreateXml(LPCSTR strPath)
	{
		// Create the root node
		bool bOK = true;
		m_pRootNode = CreateRootNode("linelist", "linelist.xsl", "Copyright © 2004 AmericanGreetings.com");
		hr = SetAttribute(m_pRootNode, "title", "Content linelist");
		bOK &= SUCCEEDED(hr);

		IXMLDOMNodePtr pNode = CreateNode(m_pRootNode, "variables");
		IXMLDOMElementPtr pElement = CreateElement(pNode, "var");
		hr = SetAttribute(pElement, "name", "drawicons");
		bOK &= SUCCEEDED(hr);
		hr = SetAttribute(pElement, "value", "0");
		bOK &= SUCCEEDED(hr);

		DWORD dwFileCount = Build(m_pRootNode, strPath, 0, false/*bJustCount*/);
		return bOK;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	bool Save(LPCSTR strOutputPath)
	{
		bool bOK = true;
		hr = m_pDoc->save(CComVariant(strOutputPath));
		bOK &= SUCCEEDED(hr);

		VARIANT_BOOL vbIsSuccessful = false;
		hr = m_pDoc->load(CComVariant(strOutputPath), &vbIsSuccessful);
		bOK &= SUCCEEDED(hr);

		long ready = READYSTATE_UNINITIALIZED;
		hr = m_pDoc->get_readyState(&ready);
		bOK &= SUCCEEDED(hr);

		return bOK;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	DWORD FileSystemCount(LPCSTR strPath)
	{
		DWORD dwFileCount = Build(m_pRootNode, strPath, 0, true/*bJustCount*/);
		return dwFileCount;
	}

private:
	/////////////////////////////////////////////////////////////////////////////
	DWORD Build(IXMLDOMNodePtr pNode, LPCSTR strFolderPath, int iDepth, bool bJustCount)
	{
		CString strPath = strFolderPath + CString(_T("\\")) + m_strFileSpec;
		CString strDepth;
		strDepth.Format("%d", iDepth);

		DWORD dwFileCount = 0;
		CFileFind FileFind;
		BOOL bFound = FileFind.FindFile(strPath);
		while (bFound)
		{
			bFound = FileFind.FindNextFile();
			if (FileFind.IsDots())
				continue;

			CString strFilePath = FileFind.GetFilePath();
			CString strFileName = FileFind.GetFileName();
			CTime CreationTime;
			FileFind.GetCreationTime(CreationTime);
			CString strFileCreated;
			strFileCreated.Format("%4d/%02d/%02d %02d:%02d:%02d",
				CreationTime.GetYear(),
				CreationTime.GetMonth(),
				CreationTime.GetDay(),
				CreationTime.GetHour(),
				CreationTime.GetMinute(),
				CreationTime.GetSecond());

			if (FileFind.IsDirectory())
			{
				IXMLDOMElementPtr pElement = pNode;
				if (!bJustCount)
					pElement = OutputElement(pNode, "folder", strFileName, strDepth, strFileCreated);
				dwFileCount++;
				DWORD dwFiles = Build(pElement, strFilePath, iDepth+1, bJustCount);
				dwFileCount += dwFiles;
				if (!bJustCount && dwFiles <= 1)
					SetAttribute(pElement, "type", "folderx");

				continue;
			}

			if (!bJustCount)
				OutputElement(pNode, "file", strFileName, strDepth, strFileCreated);
			dwFileCount++;
		}
		
		FileFind.Close();
		return dwFileCount;
	}

	/////////////////////////////////////////////////////////////////////////////
	IXMLDOMElementPtr OutputElement(IXMLDOMNodePtr pNode, LPCSTR strElementName, LPCSTR strFileName, LPCSTR strDepth, LPCSTR strFileCreated)
	{
		if (pNode == NULL)
			return E_FAIL;


		CString strExtension = strFileName;
		int i = strExtension.ReverseFind('.');
		strExtension = (i < 0 ? "" : strExtension.Mid(i+1));
		strExtension.MakeLower();

		IXMLDOMElementPtr pElement = CreateElement(pNode, strElementName);
		hr = SetAttribute(pElement, "type", strElementName);
		hr = SetAttribute(pElement, "name", strFileName);
#ifdef NOTUSED //j
		hr = SetAttribute(pElement, "cdate", strFileCreated);
#endif NOTUSED //j
		return pElement;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	CString FilterXmlString(LPCSTR pString)
	{
		CString strString = pString;
#ifdef NOTUSED //j
		strString.Replace("&", "&amp;");
		strString.Replace("\"", "&apos;");
		strString.Replace("'", "&apos;");
#endif NOTUSED //j
		return strString;
	}

	/////////////////////////////////////////////////////////////////////////////
	IXMLDOMNodePtr CreateRootNode(LPCSTR strElementName, LPCSTR strStyleSheet, LPCSTR strComment)
	{
		IXMLDOMProcessingInstructionPtr pInstruction;
		hr = m_pDoc->createProcessingInstruction(CComBSTR("xml"), CComBSTR(" version='1.0' encoding='iso-8859-1'"), &pInstruction);
		hr = m_pDoc->appendChild(pInstruction, NULL);
		if (strStyleSheet)
		{
			CString strXSL;
			strXSL.Format("type='text/xsl' href='%s'", strStyleSheet);
			hr = m_pDoc->createProcessingInstruction(CComBSTR("xml-stylesheet"), CComBSTR(strXSL), &pInstruction);
			hr = m_pDoc->appendChild(pInstruction, NULL);
		}
		if (strComment)
		{
			IXMLDOMCommentPtr pComment;
			hr = m_pDoc->createComment(CComBSTR(strComment), &pComment);
			hr = m_pDoc->appendChild(pComment, NULL);
		}

		return CreateNode(NULL, strElementName);
//		IXMLDOMNodePtr pNewNode;
//		hr = m_pDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(strElementName), NULL, &pNewNode);
//		hr = m_pDoc->appendChild(pNewNode, NULL);
//		return pNewNode;
	}

	/////////////////////////////////////////////////////////////////////////////
	IXMLDOMNodePtr CreateNode(IXMLDOMNodePtr pParentNode, LPCSTR strElementName)
	{
		IXMLDOMNodePtr pNewNode;
		hr = m_pDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(strElementName), NULL, &pNewNode);
		if (pParentNode)
			hr = pParentNode->appendChild(pNewNode, NULL);
		else
			hr = m_pDoc->appendChild(pNewNode, NULL);
		return pNewNode;
	}

	/////////////////////////////////////////////////////////////////////////////
	IXMLDOMElementPtr CreateElement(IXMLDOMNodePtr pNode, LPCSTR strElementName)
	{
		IXMLDOMElementPtr pElement;
		hr = m_pDoc->createElement(CComBSTR(strElementName), &pElement);
		if (pNode)
			hr = pNode->appendChild(pElement, NULL);
		return pElement;
	}

	/////////////////////////////////////////////////////////////////////////////
	HRESULT SetAttribute(IXMLDOMElementPtr pElement, LPCSTR strName, LPCSTR strValue)
	{
		if (pElement == NULL)
			return E_FAIL;

		CComVariant strVarTemp;
		pElement->getAttribute(CComBSTR(strName), &strVarTemp);
		return pElement->setAttribute(CComBSTR(strName), CComVariant(CComBSTR(FilterXmlString(strValue))));
	}

private:
	HRESULT hr;
	IXMLDOMDocumentPtr m_pDoc;
	IXMLDOMNodePtr m_pRootNode;
	CString m_strFileSpec;
};

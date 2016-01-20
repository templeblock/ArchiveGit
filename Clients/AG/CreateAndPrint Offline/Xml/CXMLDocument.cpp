// CXMLDocument.cpp: implementation of the CXMLDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CXMLDocument.h"

////////////////////////////////////////////////////////////////////////////
CXMLDocument::CXMLDocument()
{
	m_pCallbackFunction = NULL;
	m_pCallbackParam = NULL;
	m_pDoc = NULL;
	HRESULT hr = m_pDoc.CreateInstance(MSXML::CLSID_DOMDocument); //j (CLSID_FreeThreadedDOMDocument);
	hr = CheckHR(hr, "in the constructor");
}

////////////////////////////////////////////////////////////////////////////
CXMLDocument::~CXMLDocument()
{
}

////////////////////////////////////////////////////////////////////////////
// Function: 
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::RegisterCallback(XMLCALLBACKFUNC pCallbackFunction, LPARAM lParam)
{
	m_pCallbackFunction = pCallbackFunction;
	m_pCallbackParam = lParam;
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function: Load an XML Document from the specified file or URL synchronously.
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::Load(CString strURLFileName, bool bAsync)
{
	// set asynchronous loading flag
	HRESULT hr = m_pDoc->put_async(bAsync ? VARIANT_TRUE : VARIANT_FALSE);
	hr = CheckHR(hr, "in load: put_async");

	// Load xml document from the given URL or file path
	VARIANT_BOOL vbIsSuccessful = false;
	hr = m_pDoc->load(CComVariant(strURLFileName), &vbIsSuccessful);
	hr = CheckHR(hr, "in load");

	// Now wait for download to complete!
	if (bAsync)
		WaitForCompletion();

	hr = CheckLoad();
					
	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Function: Load an XML document from a given stream object
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::LoadStream(CString strFileName)
{
	HRESULT hr = S_OK;

	// Create an IStream object for reading the specified URL.
	char szURL[MAX_PATH];
	if (!strncmp(strFileName, "http:", 5))
		strcpy(szURL, strFileName);
	else
		::GetFullPathNameA(strFileName, MAX_PATH, szURL, NULL);

	IStreamPtr pStream = NULL;
	hr = ::URLOpenBlockingStreamA(0, szURL, &pStream, 0, 0);
	hr = CheckHR(hr, "in load stream: URLOpenBlockingStreamA");

	IPersistStreamInitPtr pPSI = m_pDoc;
	if (pPSI == NULL)
		return E_FAIL;

	hr = pPSI->Load(pStream);
	hr = CheckHR(hr, "in load stream");

	// Since we don't know whether this was a URLStream or not.
	WaitForCompletion();

	hr = CheckLoad();

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Helper function: Wait for async download to complete
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::WaitForCompletion()
{
	long ready = READYSTATE_UNINITIALIZED;
	HRESULT hr = m_pDoc->get_readyState(&ready);
	hr = CheckHR(hr, "in wait for completion");

	MSG msg;
	while (ready != READYSTATE_COMPLETE)
	{
		GetMessage(&msg, NULL, 0, 0);
		DispatchMessage(&msg);
		hr = m_pDoc->get_readyState(&ready);
		hr = CheckHR(hr, "in wait for completion");
	}

	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		DispatchMessage(&msg);
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function: Walk all the Elements in a loaded XML document.
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::WalkTree(CString& strSearchPattern)
{
	HRESULT hr = S_OK;

	if (strSearchPattern.IsEmpty())
	{
		MSXML::IXMLDOMNodePtr pNode = m_pDoc;
		if (pNode == NULL)
			return E_FAIL;

		hr = WalkTree(0/*iLevel*/, pNode);
	}
	else
	{
		//j	("//video");
		//j	("//video[@id='crap1']");
		//j	("//video[@id='crap1']");

		// filter the nodes using the search pattern
		MSXML::IXMLDOMNodeListPtr pNodeList = NULL;
		hr = m_pDoc->selectNodes(CComBSTR(strSearchPattern), &pNodeList);
		hr = CheckHR(hr, "in the search pattern");

		// get the length of the resulting node list
		if (pNodeList)
		{
			long lLength;
			hr = pNodeList->get_length(&lLength);
			hr = CheckHR(hr, "retrieving node list length");

			MSXML::IXMLDOMNodePtr pNode = NULL;
			for (long i = 0; i < lLength; i++)
			{
				hr = pNodeList->get_item(i, &pNode);
				hr = WalkTree(0/*iLevel*/, pNode);
			}
		}
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Helper function: Walk all the Elements at a particular node
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::WalkTree(int iLevel, MSXML::IXMLDOMNode* pNode)
{
	HRESULT hr = S_OK;

	CComBSTR bstrNodeName;
	pNode->get_nodeName(&bstrNodeName);

	CComVariant vNodeValue;
	pNode->get_nodeValue(&vNodeValue);
	vNodeValue.ChangeType(VT_BSTR);
	CString strValueString = V_BSTR(&vNodeValue);

	if (m_pCallbackFunction)
		m_pCallbackFunction(iLevel, CString(bstrNodeName), strValueString, m_pCallbackParam);

	MSXML::IXMLDOMNamedNodeMapPtr pAttributes = NULL;
	if (SUCCEEDED(pNode->get_attributes(&pAttributes)) && (pAttributes != NULL))
	{
		MSXML::IXMLDOMNodePtr pAttribute = NULL;
		pAttributes->nextNode(&pAttribute);
		while (pAttribute)
		{
			CComBSTR bstrNodeName;
			pAttribute->get_nodeName(&bstrNodeName);

			CComVariant vNodeValue;
			pAttribute->get_nodeValue(&vNodeValue);
			vNodeValue.ChangeType(VT_BSTR);
			CString strValueString = V_BSTR(&vNodeValue);

			if (m_pCallbackFunction)
				m_pCallbackFunction(iLevel+1, CString(bstrNodeName), strValueString, m_pCallbackParam);

			pAttributes->nextNode(&pAttribute);
		}
	}

	MSXML::IXMLDOMNodePtr pChild = NULL;
	pNode->get_firstChild(&pChild);
	while (pChild)
	{
		WalkTree(iLevel+1, pChild);

		MSXML::IXMLDOMNode* pNext = NULL;
		pChild->get_nextSibling(&pNext);
		pChild = pNext;
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// Function: Save document out to specified local file.
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::Save(CString strFileName)
{
	HRESULT hr = m_pDoc->save(CComVariant(strFileName));
	hr = CheckHR(hr, "saving");
	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Function: Return an in-memory encoded version of the XML data which
// we can squirrel away in memory for use later.  This is better 
// and faster than storing the document in a huge BSTR via get_xml.
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::PersistToMemory(PBYTE* ppBuffer, ULONG* pLen)
{
	// Create stream in global memory and save the xml document to it
	IStreamPtr pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, true/*fDeleteOnRelease*/, &pStream);
	hr = CheckHR(hr, "in persist to memory");

	IPersistStreamInitPtr pPSI = m_pDoc;
	if (pPSI == NULL)
		return E_FAIL;

	hr = pPSI->Save(pStream, true);
	hr = CheckHR(hr, "in persist to memory save");

	// now to get the real size of the stream, we have to use the
	// seek function.  You can't depend on the GlobalSize because
	// the hGlobal might have been rounded up to some paragraph boundary.
	LARGE_INTEGER li = {0, 0};
	ULARGE_INTEGER uli;
	hr = pStream->Seek(li, STREAM_SEEK_CUR, &uli);
	hr = CheckHR(hr, "in persist to memory seek");

	ULONG size = (int)uli.QuadPart;
	*pLen = size;

	BYTE* pbData = new BYTE[size];
	if (pbData == NULL)
		hr = E_OUTOFMEMORY;
	else
	{
		*ppBuffer = pbData;

		HGLOBAL hGlobal;
		hr = ::GetHGlobalFromStream(pStream, &hGlobal);
		hr = CheckHR(hr, "in persist to memory");

		::memcpy(pbData, (PBYTE)GlobalLock(hGlobal), size);
		GlobalUnlock(hGlobal);
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Function: Load the document from memory encoded bytes.
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::LoadFromMemory(PBYTE pData, ULONG ulLen)
{
	// Create a stream and write the bytes to it
	IStreamPtr pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(NULL, true/*fDeleteOnRelease*/, &pStream);
	hr = CheckHR(hr, "load from memory");

	ULONG ulWritten;
	pStream->Write(pData, ulLen, &ulWritten);

	// Reset the stream back to the beginning
	LARGE_INTEGER li = {0, 0};
	hr = pStream->Seek(li, STREAM_SEEK_SET, NULL);
	hr = CheckHR(hr, "in load from memory seek");

	// Now, load the document from the stream
	IPersistStreamInitPtr pPSI = m_pDoc;
	if (pPSI == NULL)
		return E_FAIL;

	hr = pPSI->Load(pStream);
	hr = CheckHR(hr, "load from memory load");

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Helper function: Check load results
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::CheckLoad()
{
	// And since we don't have the VARIANT_BOOL from load we
	// need to check the parse Error errorCode property to see
	// if everything went ok.
	MSXML::IXMLDOMParseErrorPtr pXMLError = NULL;
	HRESULT hr = m_pDoc->get_parseError(&pXMLError);
	hr = CheckHR(hr, "getting parse error");

	long lErrorCode = NULL;
	if (pXMLError)
	{
		hr = pXMLError->get_errorCode(&lErrorCode);
		hr = CheckHR(hr, "getting error code");
		if (lErrorCode)
			hr = ReportError(pXMLError);
	}

	return lErrorCode;
}

////////////////////////////////////////////////////////////////////////////
// Function: Create an XML DOM Document from Scratch in memory
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::Build()
{
	// Create a root node
	MSXML::IXMLDOMNodePtr pRoot;
	m_pDoc->createNode(CComVariant(MSXML::NODE_ELEMENT), CComBSTR("Root"), NULL, &pRoot);

	// add child nodes
	HRESULT hr = S_OK;
	for (int i = 0; i < 10; i++)
	{
		MSXML::IXMLDOMNodePtr pNode;
		m_pDoc->createNode(CComVariant(MSXML::NODE_ELEMENT), CComBSTR("Order"), NULL, &pNode);

		MSXML::IXMLDOMElementPtr pElement = pNode;
		if (pElement != NULL)
			pElement->setAttribute(CComBSTR("id"), CComVariant(i));

		MSXML::IXMLDOMNodePtr p0 = NULL;
		CComVariant after;
		hr = pRoot->insertBefore(pNode, after, &p0);
		hr = CheckHR(hr, "inserting node");

		// The XML Document should now own the node.
		for (int j = 0; j < 10; j++)
		{
			MSXML::IXMLDOMNodePtr pNode2;
			m_pDoc->createNode(CComVariant(MSXML::NODE_ELEMENT), CComBSTR("Item"), NULL, &pNode2);

			MSXML::IXMLDOMElementPtr pElement2 = pNode2;
			if (pElement2 != NULL)
				pElement2->setAttribute(CComBSTR("id"), CComVariant((i*10) + j));
	
			MSXML::IXMLDOMNodePtr p1 = NULL;
			CComVariant after;
			hr = p0->insertBefore(pNode2, after, &p1);
			hr = CheckHR(hr, "inserting node");
			// The XML Document should now own the node.
		}
	}

	// Now attach this new subtree to the document.
	m_pDoc->appendChild(pRoot, NULL);

	return hr;
}

////////////////////////////////////////////////////////////////////////////
// Helper function:
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::ReportError(MSXML::IXMLDOMParseError* pXMLError)
{
	CComBSTR bstrReason;
	HRESULT hr = pXMLError->get_reason(&bstrReason);
	hr = CheckHR(hr, "getting reason for the error");

	CString strError = "Error: " + CString(bstrReason);

	long line = -1;
	hr = pXMLError->get_line(&line);
	hr = CheckHR(hr, "getting line");

	if (line > 0)
	{
		long linePos = -1;
		hr = pXMLError->get_linepos(&linePos);
		hr = CheckHR(hr, "getting line position");

		long lErrorCode = -1;
		hr = pXMLError->get_errorCode(&lErrorCode);
		hr = CheckHR(hr, "getting error code");

		CComBSTR bstrURL;
		hr = pXMLError->get_url(&bstrURL);
		hr = CheckHR(hr, "getting URL");

		CString strError1;
		strError1.Format(" @ line %d, position %d in \"%S\".", line, linePos, bstrURL);
		strError += strError1;
	}

	if (m_pCallbackFunction)
		m_pCallbackFunction(0/*iLevel*/, strError, (LPCSTR)NULL, m_pCallbackParam);

	return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////
// Helper function:
////////////////////////////////////////////////////////////////////////////
HRESULT CXMLDocument::CheckHR(HRESULT hr, LPCSTR pstrErrorText)
{
	if (FAILED(hr))
	{
		CString strErrorText;
		if (pstrErrorText)
		{
			strErrorText = "Error ";
			strErrorText += pstrErrorText;
		}
		else
		{
			strErrorText.Format("Error result %lx", hr);
		}

		if (m_pCallbackFunction)
			m_pCallbackFunction(0/*iLevel*/, strErrorText, NULL, m_pCallbackParam);
	}

	return hr;
}

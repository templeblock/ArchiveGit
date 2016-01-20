//	  USAGE RECORD STRUCTURE - SERVER PERSEPECTIVE
//
//	  A usage record consists of a header and a payload.
//
//    The header is a 12-byte structure that contains the crc32 checksum of the
//    payload, the timestamp of the send, and the length of the payload.
//
//    The format of the header is as follows:
//
//    [ 0   3 ][ 4       7 ][ 8   11 ]
//      crc32    timestamp    length
//
//    In other words, for each record, the header consists of:
//
//	  The CRC32 checksum at byte offset 0 for a length of 4 bytes
//    The timestamp (seconds since UNIX epoch) at byte offset 4 for a length of 4 bytes
//    The record length (sans header) at byte offset 8 for a length of  4 bytes
//   
//    
//
//    The payload is an arbitrary sequence of eight-byte structures that each
//    encapsulate the insert count, product number, asset type code, pathnumber,
//    price, and send count of a single product included in a Creata Mail message.
//    As a result, the length of the payload will always be a multiple of eight.
//
//    Each eight-byte structure is best understood as a pair of 32-bit integers in
//    which the aforementioned values are &quot;encoded.&quot;
//
//    The bitfield format of the first four bytes is as follows:
//
//    
//    [ 31 ][ 30        25 ][ 24           2 ][ 1        0 ]
//            insert count    product number    asset type
//   
//
//    In other words, for bits 1 - 32 (going left to right):
//    
//    The high-order bit is unused>
//    The next six bits contain the insert count
//    The next 23 bits contain the product number
//    The two low-order bits contain the asset type code
//    >
//
//    The bitfield format of the second four bytes is as follows:
//
//    [ 31 ][ 30       10 ][ 9   6 ][ 5        0 ]
//            path number    price    send count
//
//    In other words, for bits 33 - 64 (going left to right):
//    
//    The high-order bit is unused
//    The next 21 bits contain the path number
//    The next four bits contain the price
//    the six low-order bits contain the send count
//  
//
//    There are several limitations imposed by this format:
//    
//    The <em>insert count</em> cannot exceed 63
//    The <em>product number</em> cannot exceed 8388607
//    The <em>asset type</em> is one of (0, 1, 2)
//    The <em>path number</em> cannot exceed 2097151
//    The <em>price</em> value cannot exceed 15
//    The <em>send count</em> cannot exceed 63
//   
//
//    Notes:
//    
//			All integers are represented in big-endian byte order.
//			The high order bit of a 32-byte integer is unused to accommodate
//			Python's &quot;One's Complement&quot; binary representation for signed
//			integer values
//    
//
//    """
//

#include "stdafx.h"
#include <atlenc.h>
#include "UsageManager.h"
#include "HelperFunctions.h"
#include "crc32.h"
#include "HTTPClient.h"
#include "Internet.h"
#include "Regkeys.h"
#include "Authenticate.h"
#include "URLEncode.h"
#include "MsAddInBase.h"
#include "ModuleVersion.h"

// Uncomment the following line to use the inline version of htonl
// This saves us from depending on Ws2_32.dll
// If you are comfortable with the inline version, you can remove the dependency on Winsock and Ws2_32.lib
//#define HTONL_INLINE

#ifndef HTONL_INLINE
	#include <Winsock2.h>
	#pragma comment(lib, "Ws2_32.lib") // for htonl
	#define HTONL htonl
#else
	inline DWORD HTONL(DWORD dwHost) // derived from INLINE_HTONL() in WinDNS.h
    {
		return	((dwHost << 8) & 0x00ff0000) |
				 (dwHost << 24)              |
				((dwHost >> 8) & 0x0000ff00) |
				 (dwHost >> 24);
    }
#endif HTONL_INLINE

static const LONG USAGE_SEND_INTERVAL = 4; // send at 4 hour intervals
static const WORD MAX_COUNT = 63; // Max count per product id.

///////////////////////////////////////////////////////////////////////////////
CUsageManager::CUsageManager()
{
	StartLogging("CreataMail.log");
	m_dwRecordSize = 0;
	m_dwMethod = 0;
	//CreateFile();
}

///////////////////////////////////////////////////////////////////////////////
CUsageManager::~CUsageManager(void)
{
	Flush();
	m_ResidualMetaData.clear();
	EndLogging();
}

///////////////////////////////////////////////////////////////////////////////
void CUsageManager::Initialize()
{
	EraseAll();
	return;
}

///////////////////////////////////////////////////////////////////////////////
void CUsageManager::SetMethod(DWORD dwMeth)
{
	m_dwMethod = dwMeth;
}

///////////////////////////////////////////////////////////////////////////////
void CUsageManager::Flush()
{
	CFuncLog log(g_pLog, "CUsageManager::Flush()");
	SaveToFile();
	//CloseFile();
}

/////////////////////////////////////////////////////////////////////////////
bool CUsageManager::SendIfReady(bool bForce)
{
	CFuncLog log(g_pLog, "CUsageManager::SendIfReady()");

	if (!IsTimeToSend() && !bForce)
		return true;

	CString szResult = Send();
	
	if (szResult.CompareNoCase(USAGE_SUCCESS_SEND) != 0)
	{
		log.LogString(LOG_ERROR, szResult.GetBuffer());
		return false;
	}

	log.LogString(LOG_INFO, szResult.GetBuffer());
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CUsageManager::IsTimeToSend()
{
	CFuncLog log(g_pLog, "CUsageManager::IsTimeToSend()");

	if (!m_Authenticate.IsUsageSendRequestOn())
		return false;

	CTime tTimeStamp;
	if (!ReadTimeStamp(tTimeStamp))
		return true;

	CTime tCurrentTime = CTime::GetCurrentTime();
	CTimeSpan tElapsedTime  = tCurrentTime -  tTimeStamp;
	LONGLONG llTotalHours = tElapsedTime.GetTotalHours();
	if (llTotalHours >= USAGE_SEND_INTERVAL)
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CUsageManager::ReadTimeStamp(CTime& tTime) 
{
	CFuncLog log(g_pLog, "CUsageManager::ReadTimeStamp()");

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	ULONGLONG TimeStamp;
	if (regkey.QueryQWORDValue(REGVAL_USAGETIMESTAMP, TimeStamp) != ERROR_SUCCESS)
		return false;

	CTime t(TimeStamp);

	tTime = t;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CUsageManager::WriteTimeStamp()
{
	CFuncLog log(g_pLog, "CUsageManager::WriteTimeStamp()");
	
	// Get current time and store in registry.
	CTime tTime = CTime::GetCurrentTime();
	ULONGLONG  ullTimeStamp = tTime.GetTime();
		

	// Write data to registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	if (regkey.SetQWORDValue(REGVAL_USAGETIMESTAMP, ullTimeStamp) != ERROR_SUCCESS)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::Add(DownloadParms& Parms, DWORD dwCount)
{
	CFuncLog log(g_pLog, "CUsageManager::Add()");

	// If product id is already in list then
	// update count. Otherwise, add product id 
	// and count to list.
	DWORD dwTotalCount;
	DWORD dwProdId = Parms.iProductId;
	ProductMapType::iterator itr;
	itr = m_ProductList.find(dwProdId);
	if (itr != m_ProductList.end())
	{
		// Ensure count does not exceed MAX_COUNT
		dwTotalCount = min(((itr->second.Count) + dwCount), MAX_COUNT);
		
		itr->second.Count		= dwTotalCount;
		itr->second.PathPrice	= Parms.iPathPrice;
		itr->second.Category	= Parms.iCategory;
		itr->second.AssetType	= Parms.iGroup;
	}
	else
	{
		// Ensure count does not exceed MAX_COUNT
		dwTotalCount = min(dwCount, MAX_COUNT); 

		m_ProductList[dwProdId].Count		= dwTotalCount;
		m_ProductList[dwProdId].PathPrice	= Parms.iPathPrice;
		m_ProductList[dwProdId].Category	= Parms.iCategory;
		m_ProductList[dwProdId].AssetType	= Parms.iGroup;
		m_ProductList[dwProdId].SendCount	= 0;
	}	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::AddSendCount(DownloadParms& Parms, DWORD dwCount)
{
	CFuncLog log(g_pLog, "CUsageManager::AddSendCount()");

	// If product id is already in list then
	// update count. Otherwise, add product id 
	// and count to list.
	DWORD dwTotalCount;
	DWORD dwProdId = Parms.iProductId;
	ProductMapType::iterator itr;
	itr = m_ProductList.find(dwProdId);
	if (itr != m_ProductList.end())
	{
		// Ensure count does not exceed MAX_COUNT
		dwTotalCount = min(((itr->second.SendCount) + dwCount), MAX_COUNT);
		
		itr->second.SendCount	= dwTotalCount;
		itr->second.PathPrice	= Parms.iPathPrice;
		itr->second.Category	= Parms.iCategory;
		itr->second.AssetType	= Parms.iGroup;
	}
	else
	{
		// Ensure count does not exceed MAX_COUNT
		dwTotalCount = min(dwCount, MAX_COUNT); 

		m_ProductList[dwProdId].SendCount	= dwTotalCount;
		m_ProductList[dwProdId].PathPrice	= Parms.iPathPrice;
		m_ProductList[dwProdId].Category	= Parms.iCategory;
		m_ProductList[dwProdId].AssetType	= Parms.iGroup;
	}	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::ClearSendCount()
{
	CFuncLog log(g_pLog, "CUsageManager::ClearSendCount()");

	if (m_ProductList.empty())
		return true;

	ProductMapType::iterator itr;
	itr = m_ProductList.begin();
	while (itr != m_ProductList.end())
	{
		itr->second.SendCount	= 0;
		itr++;
	}
		
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::AddAsSendCandidate(IHTMLDocument2 *pDoc, DownloadParms& Parms)
{
	CFuncLog log(g_pLog, "CUsageManager::AddAsSendCandidate()");
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	// Do not add if Product has already been added.
	CString szProductID;
	szProductID.Format("%d",Parms.iProductId);
	if (IsJMUProductIDPresent(spDoc, szProductID))
		return false;

	// Construct product record as Meta tag: 
	// name="jmu" content="category,Product Id,PathPrice, Asset type"
	CString szTag;
	szTag.Format("<META name=\"%s\" content=\"%d,%d,%d,%d\">", JMU_ATTRIBUTE, Parms.iCategory, Parms.iProductId, Parms.iPathPrice, Parms.iGroup);
	
		
	if (!CreateMetaElement(spDoc, szTag))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::CreateMetaElement(IHTMLDocument2 *pDoc, LPCTSTR lpszTag)
{
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CString szTag(lpszTag);
	if (szTag.IsEmpty())
		return false;

	// Find Head element.
	CComPtr<IHTMLElement> spHeadElem;
	GetHeadElement(spDoc, spHeadElem);
	if (!spHeadElem)
		return false;

	// Create the Meta element containing our product record.
	// Then add as child to Head node.
	CComPtr<IHTMLElement> spElem;
	HRESULT hr = spDoc->createElement(CComBSTR(szTag), &spElem);
	if (FAILED(hr))
		return false;

	// Get IHTMLDOMNode from newly created element that we are going to insert.
	CComQIPtr<IHTMLDOMNode>spNewNode(spElem);
	if (!spNewNode)
		return false;

	// Get IHTMLDOMNode from Head element, parent of our new node.
	CComQIPtr<IHTMLDOMNode> spDOM(spHeadElem);
	if (!spDOM)
		return false;

	// Append new node to end.
	CComPtr<IHTMLDOMNode> spOutNode;
	hr = spDOM->appendChild(spNewNode, &spOutNode);
	if (FAILED(hr))
		return false;

	return true;
}
//=============================================================================
//	GetMetaDataFromString
//=============================================================================
bool CUsageManager::GetMetaDataFromString(LPCTSTR lpszHtml, LPCTSTR lpszName, METATAGLIST &List)
{
	CString szHtml(lpszHtml);
	if (szHtml.IsEmpty())
		return false;

	CString szName(lpszName);
	szHtml.MakeLower();
	szName.MakeLower();


	CString szTag;
	int iStart=0;
	int iEnd=0;
	int iNext=0;
	int iNameIdx=0;
	int iLen = szHtml.GetLength();
	bool bDone = false;
	while(!bDone)
	{
		// Look for meta tag.
		iStart = szHtml.Find("<meta", iNext);
		if (iStart < 0)
		{
			bDone = true;
			continue;
		}
		
		// find '>' tag.
		iEnd = szHtml.Find(">", iStart);
		if (iEnd <= iStart)
			return false;

		// extract tag
		int iCount = iEnd - iStart + 1;
		szTag = szHtml.Mid(iStart, iCount);

		// update index to find next occurrence.
		iNext = iEnd + 1;
		
		// Add tag to collection. If tag name
		// specified then only add if name matches.
		if (!szName.IsEmpty())
		{
			iNameIdx = szTag.Find(szName);
			if (iNameIdx < 0)
				continue;
		}

		List.push_back(szTag);
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::IsJMUProductIDPresent(IHTMLDocument2 *pDoc, LPCTSTR szProductID)
{
	CFuncLog log(g_pLog, "CUsageManager::IsJMUProductIDPresent()");
	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByName(CComBSTR(JMU_ATTRIBUTE), &spAll);
	if (!spAll)
		return false;

	long nCnt=0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return false;

	bool bFound= false;
	CComQIPtr<IHTMLElement> spElem;
	CString szProdID;
	for (int i=0; !bFound && i< nCnt; i++)
	{
		CComVariant varIdx;
		varIdx.vt = VT_I4;
		varIdx.intVal = i;
		
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(varIdx, varIdx, &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			return false;
		
		spElem = spTagDisp;
		if (!spElem)
			return false;

		VARIANT varValue; 
		varValue.vt = VT_NULL;
		HRESULT hr = spElem->getAttribute(CComBSTR("content"), 0, &varValue);
		if (FAILED(hr) || (varValue.vt == VT_NULL))
			continue;
		
		// Compare szProductID to Product id saved as part of "content"
		// attribute.
		CString szValue(varValue.bstrVal);
		DownloadParms Parms;
		GetProductInfoFromContent(szValue, szProdID, Parms);
		if (!szProdID.CompareNoCase(szProductID))
			bFound = true;
	}

	return bFound;
}


///////////////////////////////////////////////////////////////////////////////
void CUsageManager::CountProductId(LPCTSTR szBody, LPCTSTR szProductID, DWORD &dwCount)
{
	CString szHtml(szBody);
	bool bDone = false;
	int curPos = 0;
	DWORD dwCnt = 0;
	szHtml.MakeLower();
	while (!bDone)
	{
		curPos = szHtml.Find(szProductID, curPos);
		if (curPos < 0)
			bDone = true;
		else
		{
			curPos++;
			dwCnt++;
		}
	}

	dwCount = dwCnt;
	return;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetSendCount(IHTMLDocument2 *pDoc, DWORD &dwTotal)
{
	CFuncLog log(g_pLog, "CUsageManager::GetSendCount()");
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComPtr<IHTMLElement> spBody;
	HRESULT hr = spDoc->get_body(&spBody);
	if (!spBody)
		return false;

	CComBSTR bstrHtml;
	hr = spBody->get_innerHTML(&bstrHtml);
	CString szBody(bstrHtml);

	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	// Get a collection of element with JMU attribute, i.e. meta product info.
	CComPtr<IHTMLElementCollection> spAll;
	hr = spDoc3->getElementsByName(CComBSTR(JMU_ATTRIBUTE), &spAll);
	if (!spAll)
		return false;

	long nCnt=0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return false;

	// Find the count for each product id found in the meta data.
	CComQIPtr<IHTMLElement> spElem;
	CString szProdID;
	DownloadParms Parms;
	for(int i=0; i < nCnt; i++)
	{
		CComVariant varIdx;
		varIdx.vt = VT_I4;
		varIdx.intVal = i;
		
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(varIdx, varIdx, &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			return false;
		
		spElem = spTagDisp;
		if (!spElem)
			return false;

		VARIANT varValue; 
		varValue.vt = VT_NULL;
		HRESULT hr = spElem->getAttribute(CComBSTR("content"), 0, &varValue);
		if (FAILED(hr) || (varValue.vt == VT_NULL))
			continue;
		
		// Get the count of times the product id occurs in body.
		CString szValue(varValue.bstrVal);
		GetProductInfoFromContent(szValue, szProdID, Parms);
		DWORD dwCount=0; 
		CountProductId(szBody, szProdID, dwCount);

		// Ignore prducts that have zero count.
		if (dwCount <= 0)
			continue;

		// For non-embedded types limit count to one. 
		// i.e. Stationery, Ecards, Invites, Anncouncement, 
		// Add a Photo.
		if (Parms.iCategory <= TYPE_ADDAPHOTO)
			dwCount=1;

		// If Sound, divide count by two. Sounds include
		// a product id reference in Source and Movie Param.
		if (Parms.iCategory == TYPE_SOUNDS)
			dwCount = dwCount/2;
		
		// If send count more than zero, add to product list.
		// Products with count of zero were probably deleted 
		// from message.
		AddSendCount(Parms, dwCount);
		dwTotal += dwCount;
	}

	// No longer need meta data, so remove. This minimizes risk that
	// send count may be skewed when a Creata Mail user responds or forwards
	// a Creata Mail message.
	RemoveMetaData(spDoc);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::RemoveMetaData(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CUsageManager::RemoveMetaData()");
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	// Get the collection of elements with JMU attribute, i.e. meta product info.
	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByName(CComBSTR(JMU_ATTRIBUTE), &spAll);
	if (!spAll)
		return false;

	// Get enumerator to collection.
	CComPtr<IUnknown> pUnk;
	hr = spAll->get__newEnum(&pUnk);
	if (!pUnk)
		return false;

	CComQIPtr<IEnumVARIANT> spItr(pUnk);
	if (!spItr)
		return false;

	// Iterate thru each Meta element with JMU attribute
	// and remove.
	VARIANT varData;
	ULONG uCount=0;
	bool bDone = false;
	while (!bDone)
	{
		hr = spItr->Next(1, &varData, &uCount);
		if (FAILED(hr) || uCount<=0)
		{
			bDone = true;
			continue;
		}

		CComQIPtr<IHTMLElement> spElem(varData.pdispVal);
		if (!spElem)
			return false;

		CComQIPtr<IHTMLDOMNode> spDOM(spElem);
		if (!spDOM)
			return false;

		CComPtr<IHTMLDOMNode> spRemovedNode;
		hr = spDOM->removeNode(VARIANT_FALSE, &spRemovedNode);
		if (FAILED(hr))
			return false;

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in RemoveMetaData()", "Creata Mail", MB_OK);
#endif _DEBUG
	
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::RestoreResidualMetaData(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CUsageManager::RestoreResidualMetaData()");

	if (m_ResidualMetaData.empty())
		return false;

	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	// Iterate thru list of saved Meta data and add to document
	// as potential send meta data.
	vector<DownloadParms>::iterator itr;
	itr = m_ResidualMetaData.begin();
	while(itr != m_ResidualMetaData.end())
	{
		AddAsSendCandidate(spDoc, (*itr));
		itr++;
	}	

	// Erase all data.
	m_ResidualMetaData.clear();

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetMetaData(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CUsageManager::GetMetaData()");
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	// Get the collection of elements with JMU attribute, i.e. meta product info.
	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByName(CComBSTR(JMU_ATTRIBUTE), &spAll);
	if (!spAll)
		return false;

	// Get enumerator to collection.
	CComPtr<IUnknown> pUnk;
	hr = spAll->get__newEnum(&pUnk);
	if (!pUnk)
		return false;

	CComQIPtr<IEnumVARIANT> spItr(pUnk);
	if (!spItr)
		return false;

	// Iterate thru each Meta element with JMU attribute
	// and remove.
	VARIANT varData;
	ULONG uCount=0;
	bool bDone = false;
	while (!bDone)
	{
		hr = spItr->Next(1, &varData, &uCount);
		if (FAILED(hr) || uCount<=0)
		{
			bDone = true;
			continue;
		}

		CComQIPtr<IHTMLElement> spElem(varData.pdispVal);
		if (!spElem)
			return false;


		VARIANT varValue; 
		varValue.vt = VT_NULL;
		HRESULT hr = spElem->getAttribute(CComBSTR("content"), 0, &varValue);
		if (FAILED(hr) || (varValue.vt == VT_NULL))
			continue;
		
		// Save meta data so that it can be restored later.
		CString szValue(varValue.bstrVal);
		CString szProdID;
		DownloadParms Parms;
		GetProductInfoFromContent(szValue, szProdID, Parms);
		m_ResidualMetaData.push_back(Parms);
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::RefreshMetaData(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CUsageManager::RefreshMetaData()");
	CComQIPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	// Add any residual meta data that may have been preserved from 
	// a previous content.
	RestoreResidualMetaData(spDoc);

	// Iterate thru product list to get product info
	// add as Meta data.
	ProductMapType::iterator itr;
	itr = m_ProductList.begin();
	DownloadParms Parms;
	while (itr != m_ProductList.end())
	{
		Parms.iProductId	= itr->first;
		Parms.iCategory		= itr->second.Category;
		Parms.iPathPrice	= itr->second.PathPrice;
		Parms.iGroup		= itr->second.AssetType;
		AddAsSendCandidate(spDoc, Parms);
		itr++;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::SaveToFile()
{
	CFuncLog log(g_pLog, "CUsageManager::SaveToFile()");

	HANDLE hFile=NULL;
	if (!CreateFile(hFile)) 
		return false;

	// Get product data
	BYTE *pProductData = NULL;
	DWORD dwProductDataLength = GetProductData(&pProductData);
	if (!dwProductDataLength)
	{
		CloseFile(hFile);
		if (pProductData)
			free(pProductData);
		return false;
	}

	// Get time stamp
	DWORD dwTimeStamp = GetTimeStamp();	

	/////////////////////////////////////////////////////////////////////////////
	// Construct record : CRC|Time Stamp|Length of Product data|Product data...
	/////////////////////////////////////////////////////////////////////////////
	
	// Allocate memory buffers.
	DWORD* pDWORDRecords = (DWORD*)malloc(dwProductDataLength);
	BYTE* pBigEProductRecords = (BYTE*)malloc(dwProductDataLength);
	if (!pDWORDRecords || !pBigEProductRecords)
	{
		if (pBigEProductRecords)
			free(pBigEProductRecords);
		if (pDWORDRecords)
			free(pDWORDRecords);
		if (pProductData)
			free(pProductData);

		CloseFile(hFile);
		return false;
	}

	// Convert product data to Big Endian (TCP/IP network byte order),
	// then copy to buffer.
	::memcpy(pDWORDRecords, pProductData, dwProductDataLength);
	DWORD dwLen = dwProductDataLength/4;
	DWORD dwByteCount = 0;
	DWORD dwBigEndian = 0;
	for(DWORD i=0; i<dwLen; i++)
	{
		dwBigEndian = HTONL(pDWORDRecords[i]);
		//dwBigEndian = pDWORDRecords[i];
		::memcpy(pBigEProductRecords + dwByteCount, &dwBigEndian, 4);
		dwByteCount+=4;
	}

	// Free unneeded memory
	if (pDWORDRecords)
		free(pDWORDRecords);
	if (pProductData)
		free(pProductData);

	// Get checksum of product data
	CCRC32 crc;
	DWORD dwCRC = crc.crc32(0, (BYTE *)pBigEProductRecords, dwProductDataLength);

	// Allocate memory for record.
	DWORD dwCRCSize = sizeof(dwCRC);
	DWORD dwTimeStampSize = sizeof(dwTimeStamp);
	DWORD dwProductLengthSize = sizeof(dwProductDataLength);
	BYTE *pRecord = NULL;
	bool bSuccess = false;
	pRecord = (BYTE*)malloc(dwCRCSize + dwTimeStampSize + dwProductLengthSize + dwProductDataLength);
	if (pRecord)
	{
		// Add Crc to record
		DWORD dwRecordSize = 0;
		dwBigEndian = HTONL(dwCRC);
		::memcpy(pRecord, &dwBigEndian, dwCRCSize);

		// add Time Stamp
		dwRecordSize += dwCRCSize;
		dwBigEndian = HTONL(dwTimeStamp);
		::memcpy(pRecord + dwRecordSize, &dwBigEndian, dwTimeStampSize);

		// add Product Data size
		dwRecordSize += dwTimeStampSize;
		dwBigEndian = HTONL(dwProductDataLength);
		::memcpy(pRecord + dwRecordSize, &dwBigEndian, dwProductLengthSize);

		// add Product Data 
		dwRecordSize += dwProductLengthSize;
		::memcpy(pRecord + dwRecordSize, pBigEProductRecords, dwProductDataLength);
		dwRecordSize += dwProductDataLength;

		// Write record to file.
		bSuccess = WriteToFile(hFile, pRecord, dwRecordSize);
		
		// Free memory 
		if (pRecord)
			free(pRecord);
	}

	// Free memory
	if (pBigEProductRecords)
		free(pBigEProductRecords);

	// Close usage file
	CloseFile(hFile);

	// Clear all data from memory
	EraseAll();

	return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
DWORD CUsageManager::GetProductData(BYTE **pData)
{
	CFuncLog log(g_pLog, "CUsageManager::GetProductData()");

	if (m_ProductList.empty())
		return 0;

	DWORD dwSize = 0;
	DWORD dwRecordLen = 0;
	DWORD dwProductId;
	DWORD dwPathPrice;
	DWORD dwAssetType;
	DWORD dwInsertCount;
	DWORD dwSendCount;
	DWORD dwInsertCountData;
	DWORD dwSendCountData;

	// Iterate thru product list to get product data
	// and copy to buffer.
	BYTE *pBuffer = NULL;
	ProductMapType::iterator itr;
	itr = m_ProductList.begin();
	while (itr != m_ProductList.end())
	{
		// Process product data for each product in list.
		dwProductId		= itr->first;
		dwPathPrice		= itr->second.PathPrice;
		dwInsertCount	= itr->second.Count;
		dwSendCount		= itr->second.SendCount;
		dwAssetType		= itr->second.AssetType;

		// logging only /////////////////
		DWORD dwPrice = (dwPathPrice & 0x3C0) >> 6;
		DWORD dwPath = (dwPathPrice & 0x7FFFFC00)  >> 10;
		log.LogString(LOG_INFO, log.FormatString("  ProductId=[%d], PathPrice=[%X], Path=[%d], Price=[%d], InsertCount=[%d] SendCount=[%d], AssetTypet=[%d]", dwProductId, dwPathPrice, dwPath, dwPrice, dwInsertCount, dwSendCount, dwAssetType)); 
		//////////////////////////

		////////////////////////////////////////////////////////////
		// CONSTRUCT INSERT COUNT DATA
		//
		// insert count data format:
		//
		//	[ 31 ][ 30        25 ][ 24           2 ][ 1        0 ]
		//			insert count    product number    asset type

		//	In other words, for bits 1 - 32 (going left to right):
		//
		//	The high-order bit is unused.
		//	The next six bits contain the insert count.
		//	The next 23 bits contain the product number.
		//	The two low-order bits contain the asset type code.
		///////////////////////////////////////////////////////////
		DWORD dwTemp;
		dwInsertCountData	= (dwInsertCount << 25);
		dwTemp				= (dwProductId << 2);
		dwProductId			= (dwTemp | dwAssetType);
		dwInsertCountData	= (dwInsertCountData | dwProductId);



		//DWORD dwInsertCountDataBigE =  HTONL(dwInsertCountData);

		////////////////////////////////////////////////////////////
		// CONSTRUCT SEND COUNT DATA
		//
		//  send count data format:
		//
		//	[ 31 ][ 30       10 ][ 9   6 ][ 5        0 ]
		//			path number    price    send count
		//
		//	In other words, for bits 33 - 64 (going left to right):
		//
		//	The high-order bit is unused
		//	The next 21 bits contain the path number
		//	The next four bits contain the price
		//	the six low-order bits contain the send count
		// 
		// NOTE: PathPrice from download is already shifted left. 
		// So we just need to bitwise OR.
		///////////////////////////////////////////////////////////
		ATLASSERT(!(dwPathPrice & 0x3f));
		dwPathPrice = (dwPathPrice & ~0x3f); // ensure send count bits are cleared.
		log.LogString(LOG_INFO, log.FormatString("  dwPathPrice=[%X]", dwPathPrice)); 
		dwSendCountData	= (dwPathPrice | dwSendCount);
	

		log.LogString(LOG_INFO, log.FormatString("  dwInsertCountData=[%X], dwSendCountData=[%X]", dwInsertCountData, dwSendCountData)); 
		
		// Allocate memory for buffer.
		dwRecordLen = sizeof(dwInsertCountData) + sizeof(dwSendCountData);
		if (!pBuffer)
			pBuffer = (BYTE*)malloc(dwRecordLen);
		else
			pBuffer = (BYTE*)realloc(pBuffer, dwSize + dwRecordLen);

		if (!pBuffer)
			return 0;

		// Copy Product ID with Count and Path with Price to buffer.
		::CopyMemory(pBuffer + dwSize, &dwInsertCountData, 4);
		dwSize += 4;
		::CopyMemory(pBuffer + dwSize, &dwSendCountData, 4);
		dwSize += 4;

		itr++;
	}
	
	*pData = pBuffer; // Caller must free pData.

	return dwSize;
}

///////////////////////////////////////////////////////////////////////////////
LPCTSTR CUsageManager::Send()
{
	CFuncLog log(g_pLog, "CUsageManager::Send()");
	LPCTSTR szResult = USAGE_SUCCESS_SEND;

	// Abort if not online;
	CInternet Internet;
	if (!Internet.GetIsOnline())
		return USAGE_ERROR_CONNECT;

	// Save current product info to file.
	SaveToFile();
	
	CComPtr<IWebBrowser2> pBrowser;
	HRESULT hr = pBrowser.CoCreateInstance(CComBSTR("InternetExplorer.Application"));
	if (FAILED(hr))
		return USAGE_ERROR_COCREATE;


	// Get customer #, member status and user id (email address).
	CString szUserID;
	DWORD dwCustNum;
	if (!m_Authenticate.ReadUserID(szUserID))
		return USAGE_ERROR_USERINFO;

	// Get member status - free = 0, pay = 0xacce55;
	DWORD dwMemStatus=0;
	if (!m_Authenticate.ReadMemberStatus(dwMemStatus))
		return USAGE_ERROR_USERINFO;

	// If member status is Pay then set to '1' so that
	// server can interpret.
	if (dwMemStatus == USER_STATUS_PAY)
		dwMemStatus = 1; 


	if (!m_Authenticate.ReadMemberNum(dwCustNum))
		return USAGE_ERROR_USERINFO;

	// Get version info
	CString szOSVer, szMailVer, szIEVer, szPlugVer;
	GetJMVersion(szPlugVer);
	GetOSVersion(szOSVer);
	GetMailClientVersion(szMailVer);
	GetIEVersion(szIEVer);

	// Get url for usage
	CString szUrl;
	CString szUsage;
	m_Authenticate.ReadHost(szUrl);
	m_Authenticate.ReadUsagePage(szUsage);
	if (szUsage.Find("http") >= 0)
		szUrl = szUsage;
	else
		szUrl += szUsage;

	// Get usage data from file
	CString szBuffer;
	if (!ReadFromFile(szBuffer) || szBuffer.IsEmpty())
		return USAGE_ERROR_FILEREAD;

	// Url encode
	CURLEncode UrlEncode;
	CString szEncode = UrlEncode.URLEncode(szBuffer);

	// Check if logging is requested for server.
	DWORD dwLoggingOpt=0;
	m_Authenticate.ReadServerLoggingOption(dwLoggingOpt);
	
	// Post authentication request.
	CHTTPClient Http(pBrowser);
	Http.InitilizePostArguments();
	Http.AddPostArguments(TAG_USAGE, szEncode);
	Http.AddPostArguments(TAG_EMAIL, szUserID);
	Http.AddPostArguments(TAG_MEMNUM, dwCustNum);
	Http.AddPostArguments(TAG_MEMTYPE, dwMemStatus);
	Http.AddPostArguments(TAG_VER_EMAIL, szMailVer);
	Http.AddPostArguments(TAG_VER_IE, szIEVer);
	Http.AddPostArguments(TAG_VER_OS, szOSVer);
	Http.AddPostArguments(TAG_VER_JM, szPlugVer);
	Http.AddPostArguments(TAG_DELIVMETH, m_dwMethod);
	Http.AddPostArguments(TAG_USAGE_DEBUG, dwLoggingOpt);
	if (!Http.Request(szUrl, CHTTPClient::RequestPostMethod))
		szResult = USAGE_ERROR_POST;

	// If logging turned on then log usage results - web page text.
	if (g_pLog)
	{
		log.LogString(LOG_INFO, log.FormatString("  szUserID=[%s], dwCustNum=[%d],  dwMemStatus=[%d]", szUserID.GetBuffer(), dwCustNum, dwMemStatus));
		log.LogString(LOG_INFO, log.FormatString("  szMailVer=[%s], szIEVer=[%s], szOSVer=[%s], szPlugVer=[%s]",
			szMailVer.GetBuffer(), szIEVer.GetBuffer(), szOSVer.GetBuffer(), szPlugVer.GetBuffer()));
		
		log.LogString(LOG_INFO, log.FormatString("  szEncode=[%s]", szEncode.GetBuffer()));
		log.LogString(LOG_INFO, log.FormatString("  szBuffer=[%s]", szBuffer.GetBuffer()));
		
		CComPtr<IDispatch> spDisp;
		HRESULT hr = pBrowser->get_Document(&spDisp);
		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (spDoc)
		{
			CComPtr<IHTMLElement> spElement;
			HRESULT hr = spDoc->get_body(&spElement);
			if (spElement)
			{
				CString szText = " USAGE RESULTS: \n\n";
				CComBSTR bstrText;
				hr = spElement->get_innerText(&bstrText);
				szText += CString(bstrText);
				szText += "\n\n";
				log.LogString(LOG_INFO, szText.GetBuffer());
			}
		}
	}
	// see the results
	bool bShowBrowser = (g_pLog ? true : false);
#if defined _DEBUG
	bShowBrowser = true;
#endif

	if (bShowBrowser)
		pBrowser->put_Visible(true);
	else
		pBrowser->put_Visible(false);

	// Save time stamp. 
	WriteTimeStamp();

	// Delete usage file.
	if (szResult == USAGE_SUCCESS_SEND)
	{
		if (!RemoveFile())
			return USAGE_ERROR_FILEDELETE;
	}


	return szResult;
}

///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::CreateFile(HANDLE &hFile)
{
	CFuncLog log(g_pLog, "CUsageManager::CreateFile()");

	if (!GetInstallPath(m_szUsageFile))
		return false;

	m_szUsageFile += "JMut.txt";
	hFile = ::CreateFile(m_szUsageFile, 
						GENERIC_READ | GENERIC_WRITE, 
						FILE_SHARE_READ | FILE_SHARE_WRITE, 
						NULL, 
						OPEN_ALWAYS, 
						FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		CString strMsg; 	
		strMsg.Format(_T("CUsageManager::CreateFile() failed to create file. File handle = [%X] "), hFile); 	
		GetError(strMsg);
		log.LogString(LOG_ERROR, strMsg.GetBuffer());
		return false;
	}

	
	// Move to end of file
	SetFilePointer(hFile, 0, NULL, FILE_END); 
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Caller must free pBuffer
bool CUsageManager::ReadFromFile(CString &szBuffer)
{
	CFuncLog log(g_pLog, "CUsageManager::ReadFromFile()");

	BYTE * pBuffer;
	DWORD	dwNumOfBytesRead=0;
	DWORD	dwTotalBytes=0;

	// Open usage file
	HANDLE hFile=::CreateFile(m_szUsageFile, 
							GENERIC_READ,		// desired access
							FILE_SHARE_READ,	// share mode
							NULL,				// security attribute
							OPEN_EXISTING,		// create disposition
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
							NULL);				// template file handle

	if (hFile == INVALID_HANDLE_VALUE)
	{
		CString szMsg;
		szMsg.Format(_T("CUsageManager::ReadFromFile(%s)"), m_szUsageFile); 
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		::CloseHandle(hFile);
		return false;
	}

	// Get file size
	DWORD	dwSize=::GetFileSize(hFile, NULL);
	if (dwSize == INVALID_FILE_SIZE)
	{
		CString szMsg;
		szMsg.Format(_T("CUsageManager::ReadFromFile(%s)/GetFileSize()"), m_szUsageFile);  
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		::CloseHandle(hFile);
		return false;
	}
	
	// get file data
	pBuffer=(BYTE*)malloc(dwSize+1);
	BYTE pBytes[__SIZE_BUFFER+1]="";

	BOOL bReadFileSuccess = ::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesRead, NULL);
	while(bReadFileSuccess && dwNumOfBytesRead>0 && dwTotalBytes<=dwSize)
	{
		::memcpy((pBuffer+dwTotalBytes), pBytes, dwNumOfBytesRead);
		::ZeroMemory(pBytes, __SIZE_BUFFER+1);
		dwTotalBytes+=dwNumOfBytesRead;	
		bReadFileSuccess = ::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesRead, NULL);
	}

	if (!bReadFileSuccess)
	{
		CString szMsg;
		szMsg.Format(_T("CUsageManager::ReadFromFile(%s)/::ReadFile()"), m_szUsageFile);
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		::CloseHandle(hFile);
		return false;
	}
	
	// Base64 encode data
	BYTE *pRecord64 = NULL;
	int iRecord64Size = Base64EncodeGetRequiredLength(dwTotalBytes, ATL_BASE64_FLAG_NONE);
	pRecord64 = (BYTE*)malloc(iRecord64Size+1);
	if (pRecord64)
		Base64Encode(pBuffer, dwTotalBytes, (LPSTR)pRecord64, &iRecord64Size, ATL_BASE64_FLAG_NONE );

	// Close file
	::CloseHandle(hFile);

	// Copy data to cstring
	pRecord64[iRecord64Size]='\0';
	szBuffer = pRecord64;

	// Free memory
	if (pBuffer)
		free(pBuffer);

	if (pRecord64)
		free(pRecord64);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CUsageManager::RemoveFile()
{
	CFuncLog log(g_pLog, "CUsageManager::RemoveFile()");
	/*if (!CloseFile())
		return FALSE;*/

	if (!::DeleteFile(m_szUsageFile))
	{
		CString szMsg; 	
		szMsg.Format(_T("RemoveFile attempt to delete [%s] "), m_szUsageFile); 	
		GetError(szMsg);
		log.LogString(LOG_ERROR, szMsg.GetBuffer());
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::CloseFile(HANDLE hFile)
{
	CFuncLog log(g_pLog, "CUsageManager::CloseFile()");
	if (hFile != NULL)
	{
		if (!::CloseHandle(hFile))
		{
			CString szMsg; 	
			szMsg.Format(_T("CloseFile() failed to close handle [%X] "), hFile); 	
			GetError(szMsg);
			log.LogString(LOG_ERROR, szMsg.GetBuffer());
			return false;
		}
	}
	
	hFile = NULL;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::WriteToFile(HANDLE hFile, BYTE *pData, DWORD dwSize)
{
	CFuncLog log(g_pLog, "CUsageManager::WriteToFile()");
	ATLASSERT(hFile);

	if (hFile == NULL)
		return false;
	
	if (!pData || !dwSize)
		return false;
	
	DWORD dwBytesWritten = 0;
	if (!::WriteFile(hFile, (LPCVOID)pData, dwSize, &dwBytesWritten, NULL))
	{
		GetError("WriteFile()");
		log.LogString(LOG_ERROR, "CUsageManager::WriteFile()");
		return false;
	}

	if (!::FlushFileBuffers(hFile))
	{
		GetError("FlushFileBuffers()");
		log.LogString(LOG_ERROR, "CUsageManager::FlushFileBuffers()");
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetMailClientVersion(CString& szVersion)
{
	CString szAppPath;
	CString szName;
	
	if (m_dwMethod == METH_OUTLOOK)
	{
		szAppPath = "outlook.exe";
		szName = "Outlook";
	}
	else if (m_dwMethod == METH_EXPRESS)
	{
		szAppPath = "msimn.exe";
		szName = "Express";
	}
	else if (m_dwMethod == METH_HOTMAIL)
	{
		szAppPath = "iexplore.exe";
		szName = "Hotmail";
	}
	else if (m_dwMethod == METH_YAHOO)
	{
		szAppPath = "iexplore.exe";
		szName = "Yahoo";
	}
	else if (m_dwMethod == METH_AOL)
	{
		szAppPath = "waol.exe";
		szName = "AOL Client";
	}

	if (!GetAppPath(szAppPath))
		return false;


	CModuleVersion ver;
	if (!ver.GetFileVersionInfo(szAppPath))
		return false;

	CString szVer;
	szVer = ver.GetValue(_T("ProductVersion"));
	szVersion.Format("%s: %s", szName.GetBuffer(), szVer.GetBuffer());

	return true;

}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetIEVersion(CString& szVersion)
{
	CString szAppPath = "iexplore.exe";

	if (!GetAppPath(szAppPath))
		return false;

	CModuleVersion ver;
	if (!ver.GetFileVersionInfo(szAppPath))
		return false;

	CString szVer;
	szVer = ver.GetValue(_T("ProductVersion"));
	szVersion.Format("IE: %s", szVer.GetBuffer());

	return true;

}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetJMVersion(CString& szVersion)
{
	m_Authenticate.ReadVersion(szVersion);
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CUsageManager::GetOSVersion(CString& szVersion)
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);

	CString szVer;
	CString szPlatform = "UNKNOWN";
	DWORD dwPlatformID = osvi.dwPlatformId;
	switch (osvi.dwMajorVersion)
    {
		case 5:
			switch (osvi.dwMinorVersion)
			{
				case 0:
					szPlatform = "Windows 2000";
					break;
				case 1:
					szPlatform = "Windows XP";
					break;
				case 2:
					szPlatform = "Windows Server 2003";
					break;
				default:
					break;

			}
			break;
		case 4:
		switch (osvi.dwMinorVersion)
			{
				case 0:
					if (dwPlatformID == VER_PLATFORM_WIN32_WINDOWS)
						szPlatform = "Windows 95";
					else
						szPlatform = "Windows NT 4";
					break;
				case 10:
					szPlatform = "Windows 98";
					break;
				case 90:
					szPlatform = "Windows Me";
					break;
				default:
					break;

			}
			break;
		
		case 3:
		switch (osvi.dwMinorVersion)
			{
				case 51:
					szPlatform = "Windows NT 3.51";
					break;
				
			}
			break;
		default:
			break;		
	}

	if (szPlatform.IsEmpty())
	{
		if (dwPlatformID == VER_PLATFORM_WIN32_WINDOWS)
			szPlatform ="Windows Family";
		else if (dwPlatformID == VER_PLATFORM_WIN32_NT)
			szPlatform ="Windows NT Family";
		else if (dwPlatformID == VER_PLATFORM_WIN32s)
			szPlatform ="Win32s on Windows 3.1";
	}


	szVer.Format("%s: %d.%d.%d (%.40s)", szPlatform, osvi.dwMajorVersion, 
		  osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.szCSDVersion);
	szVersion = szVer;
	return true;
}
///////////////////////////////////////////////////////////////////////////////
DWORD CUsageManager::GetTimeStamp()
{
	// Get current time
	CTime tTime = CTime::GetCurrentTime();
	DWORD  dwTimeStamp = (DWORD)tTime.GetTime();
	return dwTimeStamp;
}

///////////////////////////////////////////////////////////////////////////////
void CUsageManager::EraseAll()
{
	m_ProductList.clear();
}

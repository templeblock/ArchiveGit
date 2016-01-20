#include "stdafx.h"
#include "WebPage.h"
#include "Utility.h"

#define CHECK_POINTER(p)\
	ATLASSERT(p != NULL);\
	if(p == NULL)\
	{\
		ShowError("NULL pointer");\
		return false;\
	}


CString GetNextToken(CString& strSrc, const CString strDelim, bool bTrim = false, bool bFindOneOf = true);

/////////////////////////////////////////////////////////////////////////////
CString GetNextToken(CString& strSrc, const CString strDelim, bool bTrim, bool bFindOneOf)
{
	CString strToken;
	int idx = bFindOneOf? strSrc.FindOneOf(strDelim) : strSrc.Find(strDelim);
	if(idx != -1)
	{
		strToken  = strSrc.Left(idx);
		strSrc = strSrc.Right(strSrc.GetLength() - (idx + 1) );
	}
	else
	{
		strToken = strSrc;
		strSrc.Empty();
	}
	if(bTrim)
	{
		strToken.TrimLeft();
		strToken.TrimRight();
	}
	return strToken;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWebPage::CWebPage()
{
}

/////////////////////////////////////////////////////////////////////////////
CWebPage::CWebPage(IDispatch* pDisp)
{
	if (pDisp)
		SetDocument(pDisp);
}

/////////////////////////////////////////////////////////////////////////////
CWebPage::~CWebPage()
{
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::SetDocument(IDispatch* pDisp)
{
	CHECK_POINTER(pDisp);

	m_spDoc = NULL;

	CComPtr<IDispatch> spDisp = pDisp;
	CHECK_POINTER(spDisp);

	HRESULT hr = spDisp->QueryInterface(IID_IHTMLDocument2,(void**)&m_spDoc);
	if(FAILED(hr))
	{
		ShowError("Failed to get HTML document COM object");
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::GetJScript(CComPtr<IDispatch>& spDisp)
{
	CHECK_POINTER(m_spDoc);
	HRESULT hr = m_spDoc->get_Script(&spDisp);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::GetJScripts(CComPtr<IHTMLElementCollection>& spColl)
{
	CHECK_POINTER(m_spDoc);
	HRESULT hr = m_spDoc->get_scripts(&spColl);
	ATLASSERT(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::CallJScript(const CString strFunc, CComVariant* pVarResult)
{
	CSimpleArray<CString> paramArray;
	return CallJScript(strFunc,paramArray,pVarResult);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::CallJScript(const CString strFunc, const CString strArg1, CComVariant* pVarResult)
{
	CSimpleArray<CString> paramArray;
	paramArray.Add(strArg1);
	return CallJScript(strFunc,paramArray,pVarResult);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::CallJScript(const CString strFunc, const CString strArg1, const CString strArg2, CComVariant* pVarResult)
{
	CSimpleArray<CString> paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	return CallJScript(strFunc,paramArray,pVarResult);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::CallJScript(const CString strFunc, const CString strArg1, const CString strArg2, const CString strArg3, CComVariant* pVarResult)
{
	CSimpleArray<CString> paramArray;
	paramArray.Add(strArg1);
	paramArray.Add(strArg2);
	paramArray.Add(strArg3);
	return CallJScript(strFunc,paramArray,pVarResult);
}

//////////////////////////////////////////////////////////////////////
bool CWebPage::CallJScript(const CString strFunc, const CSimpleArray<CString>& paramArray, CComVariant* pVarResult)
{
try
{
	CComPtr<IDispatch> spScript;
	if(!GetJScript(spScript))
	{
		ShowError("Cannot GetScript");
		return false;
	}
	CComBSTR bstrMember(strFunc);
	DISPID dispid = NULL;
	HRESULT hr = spScript->GetIDsOfNames(IID_NULL, &bstrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
	if(FAILED(hr))
	{
		ShowError(HRString(hr));
		return false;
	}

	const int arraySize = paramArray.GetSize();

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(dispparams));
	dispparams.cNamedArgs = 0;
	dispparams.cArgs = arraySize;
	if (arraySize)
	{
		dispparams.rgvarg = new VARIANT[arraySize];
		for( int i = 0; i < arraySize; i++)
		{
			CComBSTR bstr = paramArray[arraySize - 1 - i]; // back reading
			bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
			dispparams.rgvarg[i].vt = VT_BSTR;
		}
	}

	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof(excepInfo));
   	CComVariant vaResult;
	UINT nArgErr = (UINT)-1;  // initialize to invalid arg
         
	hr = spScript->Invoke(dispid,IID_NULL,0,
							DISPATCH_METHOD,&dispparams,&vaResult,&excepInfo,&nArgErr);

	/////////////////// bug fix memory leak code start ///////////////
	for( int j = 0; j < arraySize; j++)
		::SysFreeString(dispparams.rgvarg[j].bstrVal);

	/////////////////// bug fix memory leak code end ///////////////

	delete [] dispparams.rgvarg;
	if(FAILED(hr))
	{
		ShowError(HRString(hr));
		return false;
	}
	
	if(pVarResult)
	{
		*pVarResult = vaResult;
	}

	return true;
}
catch (...)
{
}
	return false;
}

//////////////////////////////////////////////////////////////////////
// returned java script function name, input string is truncating
CString CWebPage::ScanJScript(CString& strAText, CSimpleArray<CString>& args)
{
	args.RemoveAll();
	CString strDelim(" \n\r\t"),strSrc(strAText);
	bool bFound = false;
	while(!strSrc.IsEmpty())
	{
		CString strStart = GetNextToken(strSrc, strDelim);
		if(strStart == "function")
		{
			bFound = true;
			break;
		}
		if(strStart == "/*")
		{
			// Skip comments
			while(!strSrc.IsEmpty())
			{
				CString strStop = GetNextToken(strSrc,strDelim);
				if(strStop == "*/")
				{
					break;
				}
			}
		}
	}

	if(!bFound)
		return "";
	
	CString strFunc = GetNextToken(strSrc,"(",true);
	CString strArgs = GetNextToken(strSrc,")",true);

	// Parse arguments
	CString strArg;
	while(!(strArg = GetNextToken(strArgs,",")).IsEmpty())
		args.Add(strArg);

	strAText= strSrc;
	return strFunc;
}

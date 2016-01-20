#include "stdafx.h"
#include "PhotoManager.h"
#include "FunctionObject.h"
#include "HelperFunctions.h"
#include "shlobj.h" // for CSIDL_*
#include "Image.h"
#include "UploadImages.h"

//#define DISPID_MYCLICK DISPID_DOCLICK
//#define DISPID_MYCLICK DISPID_MOUSEDOWN
#define DISPID_MYCLICK DISPID_DBLCLICK //j DISPID_CLICK

#define ATTR_ORIG_WIDTH "OrigWidth"
#define ATTR_ORIG_HEIGHT "OrigHeight"

//////////////////////////////////////////////////////////////////////////////
CPhotoManager::CPhotoManager()
{
	m_hWndOpenDialog = NULL;
}

//////////////////////////////////////////////////////////////////////////////
CPhotoManager::~CPhotoManager()
{
	m_ElementList.clear();
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::Activate(IHTMLDocument2Ptr spDoc, bool bOn, CString& strHost)
{
	m_strHost = strHost;

	// spDoc could be NULL if the document is being closed
	if (spDoc == NULL && bOn)
		return false;
				
	if ((spDoc != NULL) && !IsDocInEditMode(spDoc))
		return false;

	if (bOn)
	{
		GetElements(spDoc);
		SetAllAttributes(true);
	}
	else
	{
		SetAllAttributes(false);
		m_ElementList.clear();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::HasOpenDialog()
{
	if (!m_hWndOpenDialog)
		return false;

	::MessageBox(NULL, _T("Please close the Add a Photo dialog first"), g_szAppName, MB_OK);
	::SetActiveWindow(m_hWndOpenDialog);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetAllAttributes(bool bOn)
{
	// This function sets the attributes for all the elements in the list
	PMELEMENTLIST::iterator itr;
	itr = m_ElementList.begin();
	while (itr != m_ElementList.end())
	{
		SetAttributes(*itr, bOn);	
		itr++;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetAttributes(PMElementInfo& ElemInfo, bool bOn)
{
	SetContentEditable(ElemInfo, false);
	SetMouseDownCallback(ElemInfo, bOn);
	SetMouseClickCallback(ElemInfo, bOn);
	//SetMouseOverCallback(ElemInfo, bOn);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetContentEditable(PMElementInfo& ElemInfo, bool bOn)
{
	CComQIPtr<IDispatch> spDisp(ElemInfo.pTagDisp);
	if (!spDisp)
		return false; 

	return SetAttribute(spDisp, CComBSTR(HTML_ATTRIBUTE_CONTENTEDITABLE), CComVariant(bOn ? "true" : "false"));
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetMouseClickCallback(PMElementInfo& ElemInfo, bool bOn)
{
	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return false;
		
	VARIANT vFunction;
	if (!bOn)
	{
		vFunction.vt = VT_NULL;
	}
	else
	{
		vFunction.vt = VT_DISPATCH;
		vFunction.pdispVal = CFunctionObject<CPhotoManager>::CreateEventFunctionObject(this, &CPhotoManager::Callback, ElemInfo.pTagDisp, NULL, DISPID_MYCLICK);
	}
	
	HRESULT hr = spElem->put_onclick(vFunction);
	if (FAILED(hr))
		return false;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetMouseDownCallback(PMElementInfo& ElemInfo, bool bOn)
{
	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return false;
		
	VARIANT vFunction;
	if (!bOn)
	{
		vFunction.vt = VT_NULL;
	}
	else
	{
		vFunction.vt = VT_DISPATCH;
		vFunction.pdispVal = CFunctionObject<CPhotoManager>::CreateEventFunctionObject(this, &CPhotoManager::Callback, ElemInfo.pTagDisp, NULL, DISPID_MOUSEDOWN);
	}
	
	HRESULT hr = spElem->put_onmousedown(vFunction);
	if (FAILED(hr))
		return false;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::SetMouseOverCallback(PMElementInfo& ElemInfo, bool bOn)
{
	CComQIPtr<IHTMLElement> spElem(ElemInfo.pTagDisp);
	if (!spElem)
		return false;
		
	VARIANT vFunction;
	if (!bOn)
	{
		vFunction.vt = VT_NULL;
	}
	else
	{
		vFunction.vt = VT_DISPATCH;
		vFunction.pdispVal = CFunctionObject<CPhotoManager>::CreateEventFunctionObject(this, &CPhotoManager::Callback, ElemInfo.pTagDisp, NULL, DISPID_MOUSEOVER);
	}
	
	HRESULT hr = spElem->put_onmouseover(vFunction);
	if (FAILED(hr))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::GetElemSize(CComPtr<IHTMLElement> spElem, long& lElemWidth, long& lElemHeight)
{
	if (spElem == NULL)
		return;

	HRESULT hr = S_OK;
	VARIANT varValue;
	varValue.vt = VT_NULL;

	hr = spElem->getAttribute(CComBSTR(ATTR_ORIG_WIDTH), 1, &varValue);
	if (SUCCEEDED(hr) && varValue.vt != VT_NULL)
		lElemWidth = varValue.intVal;

	hr = spElem->getAttribute(CComBSTR(ATTR_ORIG_HEIGHT), 1, &varValue);
	if (SUCCEEDED(hr) && varValue.vt != VT_NULL)
		lElemHeight = varValue.intVal;
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::SetElemSize(CComPtr<IHTMLElement> spElem, long lElemWidth, long lElemHeight)
{
	if (spElem == NULL)
		return;

	HRESULT hr = S_OK;
	hr = spElem->setAttribute(CComBSTR(ATTR_ORIG_WIDTH), CComVariant((int)lElemWidth, VT_INT));
	hr = spElem->setAttribute(CComBSTR(ATTR_ORIG_HEIGHT), CComVariant((int)lElemHeight, VT_INT));
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::SetElemStyle(CComPtr<IHTMLElement> spElem, long lElemWidth, long lElemHeight)
{
	if (spElem == NULL)
		return;

	HRESULT hr = S_OK;
	CComPtr<IHTMLStyle> spStyle;
	hr = spElem->get_style(&spStyle);
	if (spStyle == NULL)
		return;
		
//j Doesn't work yet	hr = spStyle->put_textAlign(CComBSTR("center"));
//j Doesn't work yet	hr = spStyle->put_verticalAlign(CComVariant(CComBSTR("center")));
	hr = spStyle->put_width(CComVariant(lElemWidth));
	hr = spStyle->put_height(CComVariant(lElemHeight));
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::GetImgElemSize(CComPtr<IHTMLImgElement> spImgElem, long& lImageWidth, long& lImageHeight)
{
	if (spImgElem == NULL)
		return;

	HRESULT hr = S_OK;
	hr = spImgElem->get_width(&lImageWidth);
	hr = spImgElem->get_height(&lImageHeight);
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::SetImgElemSize(CComPtr<IHTMLImgElement> spImgElem, long lImageWidth, long lImageHeight)
{
	if (spImgElem == NULL)
		return;

	HRESULT hr = S_OK;
	hr = spImgElem->put_width(lImageWidth);
	hr = spImgElem->put_height(lImageHeight);
}

//////////////////////////////////////////////////////////////////////////////
double CPhotoManager::ScaleToFit(long* DestWidth, long* DestHeight, long SrcWidth, long SrcHeight, bool bUseSmallerFactor)
{
	// Scale the source window to fit the destination window...
	double scale1 = (double)*DestWidth / SrcWidth;
	double scale2 = (double)*DestHeight / SrcHeight;

	// Adjust the desination size, and return the scale factor
	double rate = (bUseSmallerFactor ? min(scale1, scale2) : max(scale1, scale2));
	*DestHeight = (long)((double)SrcHeight * rate);
	*DestWidth	= (long)((double)SrcWidth * rate);
	return rate;
}

//////////////////////////////////////////////////////////////////////////////
void CPhotoManager::Callback(IDispatch* pDisp1, IDispatch* pDisp2, DISPID id, VARIANT* pVarResult)
{
	if (!pDisp1)
		return;

	HRESULT hr = S_OK;
	if (id == DISPID_MOUSEOVER)
	{
		CComQIPtr<IHTMLElement2> spElem2(pDisp1);
		if (!spElem2)
			return;

		hr = spElem2->focus();
		
	}
	else
	if (id == DISPID_MOUSEDOWN)
	{
		CComQIPtr<IHTMLElement> spElem(pDisp1);
		if (!spElem)
			return;
		
		CComPtr<IDispatch> spDisp;
		spElem->get_document(&spDisp);
		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (FAILED(hr) || !spDoc)
			return;
		
		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;
		
		long lVal;
		spEventObj->get_button(&lVal);
		if ( lVal == 1) // 1 = left button is pressed 		
			hr = spElem->click();		
	}
	else
	if (id == DISPID_MYCLICK)
	{
		CComQIPtr<IHTMLElement> spElem(pDisp1);
		if (!spElem)
			return;

		CComQIPtr<IHTMLImgElement> spImgElem(pDisp1);
		if (!spImgElem)
			return;

		CComPtr<IDispatch> spDisp;
		hr = spElem->get_document(&spDisp);
		if (FAILED(hr) || !spDisp)
			return;

		CComQIPtr<IHTMLDocument2> spDoc(spDisp);
		if (!spDoc)
			return;

		CString strFileName;
		bool bOK = AddAPhoto((IHTMLDocument2*)spDoc, strFileName);
		if (!bOK)
			return;
		
		CComQIPtr<IMarkupServices2> spMarkup2 = spDoc;
		if (spMarkup2)
			hr = spMarkup2->BeginUndoUnit(L"Add a Photo");

		long lElemWidth = 0;
		long lElemHeight = 0;
		GetElemSize(spElem, lElemWidth, lElemHeight);
		if (!lElemWidth && !lElemHeight)
		{
			GetImgElemSize(spImgElem, lElemWidth, lElemHeight);
			SetElemSize(spElem, lElemWidth, lElemHeight);
		}

		long lImageWidth = 0;
		long lImageHeight = 0;
		CImage Image(strFileName);
		Image.GetNativeImageSize(lImageWidth, lImageHeight);

		#define SHIFT (GetAsyncKeyState(VK_SHIFT) < 0)
		if (SHIFT)
			::MessageBox(NULL, "The photo will be embedded in the email message.", g_szAppName, MB_OK);
		else
		{
			CString strURL = m_strHost + "/cgi-bin/flashalbum/cmaddaphoto.pl";

			// strFormDataPairs is a string of name/value pairs as follows "name1:value1|\nname2:value2|\n"
			CString strFormDataPairs;
			strFormDataPairs += "inputfile:~" + strFileName + "|\n";

			CUploadImages Upload(ULI_NOUPSIZE /*| ULI_LEAVETEMP | ULI_DEBUG*/);
			CString strResult;
			Upload.UploadImages(strURL, strFormDataPairs, lElemWidth, lElemHeight, 70/*nQuality*/, strResult);
			if (strResult.Find("error") < 0 && strResult.Find("http") >= 0)
			{
				GetImageHost(strResult);
				strFileName = strResult;
				//::MessageBox(NULL, String("The photo was uploaded first to %s.", strFileName), g_szAppName, MB_OK);
			}
			else
			{
				CString szMsg;
				szMsg.LoadString(IDS_ADDAPHOTO_ERROR);
				::MessageBox(NULL, szMsg, g_szAppName, MB_OK);
				if (spMarkup2)
					hr = spMarkup2->EndUndoUnit();
				return;
			}
		}

		if (lImageWidth && lImageHeight)
		{
			ScaleToFit(&lElemWidth, &lElemHeight, lImageWidth, lImageHeight, true/*bUseSmallerFactor*/);
			SetImgElemSize(spImgElem, lElemWidth, lElemHeight);
			SetElemStyle(spElem, lElemWidth, lElemHeight);
		}

		hr = spImgElem->put_src(CComBSTR(strFileName));

		if (spMarkup2)
		{
			hr = spMarkup2->EndUndoUnit();
			CComQIPtr<IMarkupContainer2> spMarkupCont2 = spMarkup2;
			if (spMarkupCont2)
			{
				long l = spMarkupCont2->GetVersionNumber();
				l += 0;
			}
		}

		if (FAILED(hr))
			return;
	}
}

//////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::GetElements(IHTMLDocument2Ptr spDoc)
{
	m_ElementList.clear();

	if (spDoc == NULL)
		return false;

	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc->get_all(&spAll);
	if (FAILED(hr) || !spAll)
		return false;
		
	// Find all the IMG tags in the document
    CComPtr<IDispatch> spTagsDisp;
	hr = spAll->tags(CComVariant(CComBSTR("img")), &spTagsDisp);
	if (FAILED(hr) || !spTagsDisp)
		return false;

	CComQIPtr<IHTMLElementCollection> spTags(spTagsDisp);
	if (!spTags)
		return false;

	long nTags = 0;
	hr = spTags->get_length(&nTags);
	if (FAILED(hr))
		return false;

	bool bSuccess = false;
	for (long i=0; i < nTags; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spTags->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			continue;
		
		CComQIPtr<IHTMLImgElement> spObject(spTagDisp);
		if (!spObject)
			continue;

		CComBSTR bstrFileName;
		hr = spObject->get_src(&bstrFileName);
		if (FAILED(hr))
			continue;

		CString strFileName = bstrFileName;
		if (strFileName.IsEmpty())
			continue;
			
		strFileName.MakeLower();
		bool bPhoto = (strFileName.Find("photo.gif") >= 0);
		bool bCid = (strFileName.Find("cid:") >= 0);
		if (!bPhoto && !bCid)
			continue;

		PMElementInfo ElemInfo;
		ElemInfo.pTagDisp= spTagDisp;
		m_ElementList.push_back(ElemInfo);
		bSuccess = true;
	}
	
	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::AddAPhoto(IHTMLDocument2Ptr spDoc, CString& strFileName)
{
	CString strPath;
	bool bOK = GetSpecialFolderLocation(strPath, CSIDL_MYPICTURES);
	if (!bOK)
		 bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_PICTURES);

	HWND hWndOwner = ::GetActiveWindow();
	if (spDoc)
	{
		CComQIPtr<IOleWindow> spOleWin = spDoc;
		if (spOleWin)
		{
			HWND hWnd = NULL;
			if (SUCCEEDED(spOleWin->GetWindow(&hWnd)))
				hWndOwner = hWnd;
		}
	}

	char szFileName[MAX_PATH];
	*szFileName = 0;

	struct OPENFILENAMEX
	{
		OPENFILENAME of;
		void* pvReserved;
		DWORD dwReserved;
		DWORD FlagsEx;
	};

	OPENFILENAMEX ofx;
	memset(&ofx, 0, sizeof(ofx));
	ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx.of));
	ofx.of.hwndOwner = hWndOwner;
	ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
	ofx.of.lpstrFilter = "All Supported Files (*.bmp, *.gif, *.jpg, *.png, *.wmf, *.ico, *.emf)\0*.bmp;*.gif;*.jpg;*.png;*.wmf;*.ico;*.emf\0Bitmap Files (*.bmp)\0*.bmp\0GIF Files (*.gif)\0*.gif\0JPG Files (*.jpg)\0*.jpg\0PNG Files (*.png)\0*.png\0Windows Metafiles Files (*.wmf)\0*.wmf\0Windows Enhanced Metafiles Files (*.emf)\0*.emf\0Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0\0";
	ofx.of.lpstrTitle = "Add a Photo® or an Art File";
	ofx.of.lpstrDefExt = "bmp";
	ofx.of.lpstrInitialDir = strPath;
	ofx.of.lpstrFile = szFileName;
	ofx.of.lpfnHook = NULL;
	ofx.of.nMaxFile = MAX_PATH;

	m_hWndOpenDialog = hWndOwner;
	bOK = !!::GetOpenFileName(&ofx.of);
	m_hWndOpenDialog = NULL;

	if (bOK)
		strFileName = szFileName;
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
bool CPhotoManager::GetImageHost(CString& szImageHost) 
{
	CString szTemp = m_strHost;
	szTemp.MakeLower();

	if (szTemp.Find(_T("//dev")) >= 0)
		szImageHost.Replace(_T("//www"), _T("//dev"));
	else if (szTemp.Find(_T("//work")) >= 0)
		szImageHost.Replace(_T("//www"), _T("//work"));
	else if (szTemp.Find(_T("//stage")) >= 0)
		szImageHost.Replace(_T("//www"), _T("//stage"));

	return true;
}
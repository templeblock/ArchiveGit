#pragma once
#include <vector>
using namespace std;

struct PMElementInfo
{
	CComPtr<IDispatch> pTagDisp;
};
typedef vector<PMElementInfo> PMELEMENTLIST;

class CPhotoManager
{
public:
	CPhotoManager();
	~CPhotoManager();

public:
	bool Activate(IHTMLDocument2Ptr pDoc, bool bOn, CString& strHost);
	bool HasOpenDialog();

protected:	
	bool SetAllAttributes(bool bContentEditable);
	bool SetAttributes(PMElementInfo& ElemInfo, bool bOn);
	bool SetContentEditable(PMElementInfo& ElemInfo, bool bOn);
	bool SetMouseDownCallback(PMElementInfo& ElemInfo, bool bOn);
	bool SetMouseClickCallback(PMElementInfo& ElemInfo, bool bOn);
	bool SetMouseOverCallback(PMElementInfo& ElemInfo, bool bOn);
	void GetElemSize(CComPtr<IHTMLElement> spElem, long& lElemWidth, long& lElemHeight);
	void SetElemSize(CComPtr<IHTMLElement> spElem, long lElemWidth, long lElemHeight);
	void SetElemStyle(CComPtr<IHTMLElement> spElem, long lElemWidth, long lElemHeight);
	void GetImgElemSize(CComPtr<IHTMLImgElement> spImgElem, long& lImageWidth, long& lImageHeight);
	void SetImgElemSize(CComPtr<IHTMLImgElement> spImgElem, long lImageWidth, long lImageHeight);
	double ScaleToFit(long* DestWidth, long* DestHeight, long SrcWidth, long SrcHeight, bool bUseSmallerFactor);
	void Callback(IDispatch* pDisp1, IDispatch* pDisp2, DISPID id, VARIANT* pVarResult);
	bool GetElements(IHTMLDocument2Ptr spDoc);
	bool AddAPhoto(IHTMLDocument2Ptr spDoc, CString& strFileName);
	bool GetImageHost(CString& szImageHost);
	
protected:
	PMELEMENTLIST m_ElementList;
	HWND m_hWndOpenDialog;
	CString m_strHost;
};

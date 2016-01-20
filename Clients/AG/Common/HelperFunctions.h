#pragma once
#include "resource.h"
#include "CLog.h"
#include "CFunclog.h"
#include "CWinlog.h"
#include "CFilelog.h"
#include "CM_Constants.h"

class CLog;
extern CLog* g_pLog;

BOOL CreateDir(LPCSTR lpszPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
BOOL GetInstallPath(CString &szPath);
bool ShowPage(IWebBrowser2 * pWB, int nPage);
//bool Wait(DWORD dwExpire);
bool IsDocInEditMode(IHTMLDocument2* spDoc);
bool StartLogging(LPCTSTR szLogFile);
bool SetAttribute(IDispatch* pDisp, CComBSTR bsAttribute, VARIANT varAttributeValue, LONG lFlag=0);
bool GetAttribute(IDispatch* pDisp, CComBSTR bsAttribute, LONG lFlag, VARIANT * varAttributeValue);
bool IsAttributeValuePresent(IDispatch* pDisp, LPCTSTR lpszAttribute, LPCTSTR lpszExpectedValue); 
bool IsElementContentEditable(IDispatch* pDisp);
bool IsDocLoaded(IHTMLDocument2* pDoc, bool bInteractive=true);
bool IsDocLoadedWait(IHTMLDocument2* pDoc, bool bInteractive=true, DWORD dwLimit=10000);
bool GetVersion(LPCTSTR szAppPath, VERSIONINFOSTRUCT &Version);
bool GetIEMajorVersion(DWORD &dwVersion);
bool GetSetupVersion(CString &szVer);
bool GetInstallStatus(DWORD &dwStatus);
bool SetInstallStatus(DWORD dwStatus);
bool GetHeadElement(IHTMLDocument2 *pDoc, CComPtr<IHTMLElement> &spElem);
bool GetElementByTagAndAttribute(IHTMLDocument2 *pDoc, LPCTSTR szTag, LPCTSTR szAttribute, LPCTSTR szAttrValue,  CComPtr<IHTMLElement> &spElem);
bool GetBody(LPCTSTR szDoc, CString &szBody);
bool GetPagePath(int nPage, CString &szPath);
bool EvaluateConnection(IWebBrowser2 * pWB, DWORD dwTimeOut=60000);
bool RemoveDefaultStationery(DWORD dwClientType);
bool RemoveFilesFromFolder(LPCTSTR szFolder, LPCTSTR szType);
bool IsFilePresent(LPCTSTR szFilePath);
bool GetFrameInfo(IHTMLElement *pElem, CString &szName, CString &szID, CString &szSrc, CString &szParentID, CString szBody, CComPtr<IHTMLWindow2> &spContainingWindow, CComPtr<IHTMLWindow2> &spParentWindow);
BOOL RestoreWindowProc(HWND hwnd, WNDPROC pfProc);
WNDPROC SetWindowProc(HWND hwnd, WNDPROC pfProc);
BOOL EnableClient(DWORD dwClientType);
BOOL DisableClient(DWORD dwClientType);
int FindMenuItem(CMenu* pMenu, UINT MenuId);
int FindMenuItem(CMenu* pMenu, CString& str);
void GetProductInfoFromContent(LPCTSTR szProductRecord, CString& szProdID, DownloadParms& Parms);
void EndLogging();
CString GetOptionStringValue(LPCTSTR szOptionString, LPCTSTR szNameTarget);

/************************************************************************

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

************************************************************************/

#ifndef __dlgcms_h__
#define __dlgcms_h__

/************************************************************************/
#include "ppmodal.h"
#include "cmsutil.h"

__declspec( dllimport ) LPKODAKATTRIB	KCMSGetAttribute;

#define GetRegStr(K,V,STR) GetPrivateProfileString(_T("-"),K,STR,V,256,szCms)
#define GetRegInt(K,V,INT) (V=GetPrivateProfileInt(_T("-"),K,INT,szCms))
#define GetRegFix(K,V,INT) { TCHAR t[40]; GetPrivateProfileString(_T("-"),K,itoa(INT,t,10),t,sizeof(t),szFileName); V=AsciiFixed(t); }
#define PutRegStr(K,V) WritePrivateProfileString(_T("-"),K,V,szCms)
#define PutRegInt(K,V) { TCHAR t[40]; WritePrivateProfileString(_T("-"),K,itoa(V,t,10),szCms); }
#define PutRegFix(K,V) { TCHAR t[40]; WritePrivateProfileString(_T("-"),K,FixedAscii(V,t,-1),szFileName); }

class CCmsDlg : public CPPModalDlg
{
public:
        
	CCmsDlg( int nDialogId, CWnd* pParentWnd = NULL );

protected:

	void	ControlGetText( UINT nId );
	void	ControlGetText( HWND hControl );
	void	ComboBoxAddString( UINT	nId );
	void	ComboBoxAddString( UINT	nId, LPCTSTR string );
	void	ComboBoxSelectString( UINT nId, LPCTSTR string );
	void	ListBoxAddString( UINT nId );
	int 	ListBoxFindStringExact( int nId );
	void	ListBoxSetCurSel( UINT nId, int index );
	void	ListBoxResetContent( UINT nId );
	int	ComboBoxFindStringExact( int nId );
	void	ComboBoxSetCurSel( UINT nId, int index );
	void	CCmsDlg::ControlGetParentText( int nId );
	void	StaticSetText( UINT nId );
	void	ListBoxGetSelectedString( UINT nId );
	void	CCmsDlg::PCDSaveSettings();

public:

	inline BOOL GetReturn() { return( bReturn ); };

protected:

	LPCMSINFO			pCmsInfo;
	CString				string;
	CString				string1;
	POSITION				pos;
	POSITION				pos1;
	DWORD					dwId;
	int					nCrc;
	int					iSel;
	int					bReturn;
	CMapStringToPtr	*pPTs;

public:

};

class CCmsInfoDlg : public CCmsDlg
{
public:
        
	CCmsInfoDlg( BOOL bSrc, BOOL bDst, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();

protected:

	BOOL	bDoSrc;
	BOOL	bDoDst;

private:

};

class CCmsFileOpenDlg : public CCmsDlg
{
public:
        
	CCmsFileOpenDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsConvertToDlg : public CCmsDlg
{
public:
        
	CCmsConvertToDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsPasteDlg : public CCmsDlg
{
public:
        
	CCmsPasteDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsPCDPasteDlg : public CCmsDlg
{
public:
        
	CCmsPCDPasteDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsFileNewDlg : public CCmsDlg
{
public:
        
	CCmsFileNewDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsConvertDlg : public CCmsDlg
{
public:
        
	CCmsConvertDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsPhotoCDOpenDlg : public CCmsDlg
{
public:
        
	CCmsPhotoCDOpenDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsPrintPreviewDlg : public CCmsDlg
{
public:
        
	CCmsPrintPreviewDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

class CCmsScanSetupDlg : public CCmsDlg
{
public:
        
	CCmsScanSetupDlg( LPCMSINFO pCmsInfo, CWnd* pParentWnd = NULL );

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );

protected:

	CCmsInfoDlg*	pInfo;

private:

};

#endif // __dlgcms_h__



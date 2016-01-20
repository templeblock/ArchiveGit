// ProfileDoc.h : interface of the CProfileDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef CPROFILE_DOC_H
#define CPROFILE_DOC_H

#pragma once

#include "mcostat.h"
#include "mcotypes.h"
#include "mpfileio.h"
#include "curve.h"
#include "tweak_patch.h"
#include "profiledocfiles.h"
#include "ProfileSheet.h"
#include "DeviceSheet.h"
#include "CommDevice.h"
#include "MyMessageBox.h"

class CProfileDoc : public CDocument
{
protected: // create from serialization only
	CProfileDoc();
	DECLARE_DYNCREATE(CProfileDoc)

	void	InitLinear(char *table, long step, double *val, int which);
	void	InitCmyk(char *table, long step, double *cmy, double b);
	void	FormatToRgbs(char** table, PatchFormat *patchdata);
	int		SaveMessageBox(CString prompt, BOOL save);

// Attributes
public:
	ProfileDocFiles	_profiledocfiles;

	char	m_fname[256];
	int		*m_monacoDongle;	// flag indicates if a dongle exists;
	int		*m_changedProfile;	// flag indicates if a profile should be created
	int		*m_monacoProfile;	// flag indicates if profile was edited or created by monaco
	int		*m_monacoEdited;	// flag indicating that profile was edited by monaco
	int		*m_createdProfile; // flag indicating that profile was created
	int		*m_changedLinearTone; // flag indicating that linear or tone data has changed
	BOOLEAN	*m_needSave;		// flag indicating description tag needs to be saved
	PatchType	*m_sourceexist;		//source data exists
	PatchType	*m_dryjetexist;		//dryjet data exists

	DeviceSetup	m_deviceSetup;	//device setup
	BOOL		m_measureLab;	//type of measurement, TRUE LAB, FALSE DENSITY

	char	*m_srcrgbs;	//rgbs for source
	char	*m_dryrgbs;	//rgbs for dryjet
	char	*m_drgbs;	//default rgbs for measure
	int		m_patches[5];	//lab and density patches

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument( LPCTSTR lpszPathName );
	virtual BOOL OnSaveDocument( LPCTSTR lpszPathName );
	virtual void OnCloseDocument();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CProfileDoc();

	char* GetRgbs(int which);
	BOOL IsProfileSavable(void);	//all the necesssary components for processing

//	BOOL CreateProfile( LPCTSTR fname );
	void CleanupData(void);
	BOOL InitData(void);
	void SetProfileModified(BOOL bModified);
	void UpdateTabStates(void);

	BOOL GetCalibrateDescription( CString& szDescription );
	BOOL SetCalibrateDescription( CString szDescription );
	int GetCalibrateData( CMYK iCurve, double* x, double* y, int nMaxPoints );
	BOOL SetCalibrateData( CMYK iCurve, double* x, double* y, int nPoints );
	BOOL GetCalibrateDate( long* date);
	BOOL SetCalibrateDate( long date);
	BOOL GetToneAdjustDescription( CString& szDescription );
	BOOL SetToneAdjustDescription( CString szDescription );
	int GetToneAdjustData( CMYK iCurve, double* x, double* y, int nMaxPoints );
	BOOL SetToneAdjustData( CMYK iCurve, double* x, double* y, int nPoints );
	BOOL GetColorSourceDescription( CString& szDescription );
	BOOL SetColorSourceDescription( CString szDescription );
	BOOL GetColorDryjetDescription( CString& szDescription );
	BOOL SetColorDryjetDescription( CString szDescription );
	BOOL GetDataDate( long* date, long which );
	BOOL SetDataDate( long date, long which );
	int GetColorData( double** pfIndex, double** pfL, double** pfa, double** pfb, int* nL, BOOL bSource );
	int GetColorSourceData( double** pfIndex, double** pfL, double** pfa, double** pfb, int* nLinear );
	int GetColorDryjetData( double** pfIndex, double** pfL, double** pfa, double** pfb, int* nLinear );
	BOOL GetSettingAutomatic( void );
	void SetSettingAutomatic( BOOL bValue );
	BOOL GetSettingTextEnhance( void );
	void SetSettingTextEnhance( BOOL bValue );
	int GetSettingColorBalance( void );
	void SetSettingColorBalance( int iValue );
	int GetSettingInkNeutralize( void );
	void SetSettingInkNeutralize( int iValue );
	int GetSettingColorBalanceCutoff( void );
	void SetSettingColorBalanceCufoff( int iValue );
	int GetSettingContrast( void );
	void SetSettingContrast( int iValue );
	int GetSettingSaturation( void );
	void SetSettingSaturation( int iValue );
	int GetSettingSmoothData( void );
	void SetSettingSmoothData( int iValue );
	BOOL LoadPatch(char* patchname, long which);
	BOOL SavePatch(char* patchname, long which);
	BOOL LoadLinear(char* patchname);
	BOOL LoadLinearFromPatch(void);
	BOOL SaveLinear(char* patchname);
	BOOL LoadTone(char* patchname);
	BOOL SaveTone(char* patchname);
	int GetTweaks(Tweak_Element* pTweaksArray[], int nTweaks);
	void PutTweaks(Tweak_Element* pTweaksArray[], int nTweaks);

	void EditPreferences(BOOL labdata);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CProfileDoc)
	afx_msg BOOL OnFileRevert();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRevert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPreferences(CCmdUI* pCmdUI);
	afx_msg void OnEditPreferences();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

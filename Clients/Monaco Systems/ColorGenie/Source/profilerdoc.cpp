// ProfileDoc.cpp : implementation of the CProfileDoc class
//

#include "stdafx.h"
#include "ColorGenie.h"
#include "Message.h"
#include "ProfilerDoc.h"
#include "ProfileView.h"
#include "BuildProgress.h"
#include "Winmisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CProfileDoc

IMPLEMENT_DYNCREATE(CProfileDoc, CDocument)

BEGIN_MESSAGE_MAP(CProfileDoc, CDocument)
	//{{AFX_MSG_MAP(CProfileDoc)
	ON_COMMAND(ID_FILE_REVERT, OnFileRevert)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_REVERT, OnUpdateFileRevert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PREFERENCES, OnUpdateEditPreferences)
	ON_COMMAND(ID_EDIT_PREFERENCES, OnEditPreferences)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProfileDoc construction/destruction

/////////////////////////////////////////////////////////////////////////////
CProfileDoc::CProfileDoc()
{
	memset(m_fname, 0x00, 256);

	m_monacoDongle = &_profiledocfiles._monacoprint;
	m_changedProfile = &_profiledocfiles.changedProfile;	
	m_monacoProfile = &_profiledocfiles.monacoProfile;	
	m_monacoEdited = &_profiledocfiles.monacoEdited;	
	m_createdProfile = &_profiledocfiles.createdProfile;
	m_changedLinearTone = &_profiledocfiles.changedLinearTone;
	m_needSave = &_profiledocfiles.needsSave;
	m_sourceexist = &_profiledocfiles.patchD->type;
	m_dryjetexist = &_profiledocfiles.simupatchD->type;

	CApp* app = (CApp*)AfxGetApp( );
	if(app) *m_monacoDongle = app->mhasDongle; 

	//default device setup
	m_deviceSetup.numPorts = CCommDevice::GetComPorts(m_deviceSetup.portNames);
	m_deviceSetup.deviceType = GRETAG_SCAN;
	m_deviceSetup.port	= 0;
	m_deviceSetup.baudRate = 9600;
	m_deviceSetup.calFrequency = 50;
	m_measureLab = FALSE;

	//default rgb table
	PatchFormat	patchdata;

	long	steps[] = {9, 6, 5, 5, 4, 4, 4};	
	double	blacks[] = {0.0, 20.0, 40.0, 60.0, 80.0, 90.0, 100.0} ;					
	double	cmy[] = 
	{
	0.0, 10.0, 22.0, 34.0, 47.0, 60.0, 73.0, 86.0, 100.0, 
	0.0, 20.0, 40.0, 60.0, 80.0, 100.0, 
	0.0, 25.0, 50.0, 75.0, 100.0, 
	0.0, 25.0, 50.0, 75.0, 100.0, 
	0.0, 33.0, 66.0, 100.0, 
	0.0, 33.0, 66.0, 100.0, 
	0.0, 33.0, 66.0, 100.0 
	};						
	double	linear[] = 
	{					
	0.0, 4.0, 8.0, 12.0, 16.0, 
	20.0, 24.0, 28.0, 32.0, 36.0, 
	40.0, 44.0, 48.0, 52.0, 56.0, 
	60.0, 64.0, 68.0, 72.0, 76.0, 
	80.0, 84.0, 88.0, 92.0, 96.0, 100.0 
	};

	patchdata.patchType = 1;
	patchdata.numComp = 4;
	patchdata.numCube = 7;
	patchdata.numLinear = 26;
	patchdata.steps = steps;
	patchdata.blacks = blacks;
	patchdata.cmy = cmy;
	patchdata.linear = linear;

	m_drgbs = NULL;
	m_srcrgbs = NULL;
	m_dryrgbs = NULL;
	FormatToRgbs(&m_drgbs, &patchdata);

	m_patches[0] = 1387;
	m_patches[1] =  25;	
	m_patches[2] =  25;	
	m_patches[3] =  25;	
	m_patches[4] =  25;	

	SetProfileModified(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
CProfileDoc::~CProfileDoc()
{																							   
	CleanupData();

	//this is only delete once
	if(m_drgbs)
	{
		delete m_drgbs;
		m_drgbs = NULL;
	}
}

void CProfileDoc::FormatToRgbs(char** intable, PatchFormat *patchdata)
{
	long i, j, k;
	long cmykpatches, labpatches, linearpatches;
	long step;
	char *cmyk;
	char *table;						

	CApp* app = (CApp*)AfxGetApp( );
	if(!app || !app->mhasCmm)	return; 

	if(patchdata->patchType != 1)	return;

	if(*intable)
	{
		delete *intable;
		*intable = NULL;
	}

 	labpatches = 0;
	for(i = 0; i < (long)patchdata->numCube; i++)
		labpatches += patchdata->steps[i]*patchdata->steps[i]*patchdata->steps[i];

	linearpatches = (patchdata->numLinear-1)*patchdata->numComp;
	if(linearpatches <	0)	linearpatches = 0;

	table = new char[ (labpatches+linearpatches)*patchdata->numComp];
	if(!table)	return;

	//init the lab patches for cmyk values
	cmyk = table;
	long totalstep = 0;
	for(i = 0; i < patchdata->numCube; i++)
	{
		step = patchdata->steps[i];
		InitCmyk(cmyk, step, &patchdata->cmy[totalstep], patchdata->blacks[i]); 
		totalstep += step;
		cmyk += step*step*step*patchdata->numComp;
	}		 

	//init the linear patches for cmyk values
	if(linearpatches)
	{
		for(i = 0; i < patchdata->numComp; i++)
		{
			InitLinear(cmyk, patchdata->numLinear - 1, &patchdata->linear[1], i);
			cmyk += (patchdata->numLinear-1)*patchdata->numComp;
		}
	}

	//convert to rgb values
	app->mcmmConvert.ApplyXform(table, table, 8, labpatches+linearpatches);
	*intable = table; 
}

void	CProfileDoc::InitLinear(char *table, long step, double *val, int which)
{
	int j;

	for(int i = 0; i < step; i++)
	{
		j = i*4;
		table[j] = 0;
		table[j+1] = 0;
		table[j+2] = 0;
		table[j+3] = 0;
		table[j+which] = (char)(val[i]*2.55 + 0.5);
	}
}

void	CProfileDoc::InitCmyk(char *table, long step, double *cmy, double b)
{
	long 	i, j, k;
	char  *cmyk;
	char c, m, y;

	cmyk = table;

	for(i = 0; i < step; i++)
	{
		y = (char)(cmy[i]*2.55+0.5);
		for(j = 0; j < step; j++)
		{				
			c = (char)(cmy[j]*2.55+0.5);

			for(k = 0; k < step; k++)
			{
				m = (char)(cmy[k]*2.55+0.5);

				cmyk[0] = c;
				cmyk[1] = m;
				cmyk[2] = y;
				cmyk[3] = (char)(b*2.55+0.5);
				cmyk += 4;
			}
		}
	}
}

char* CProfileDoc::GetRgbs(int which)
{
	switch(which)
	{
		case(1):
		return m_srcrgbs;

		case(2):
		return m_dryrgbs;

		case(3):
		return m_drgbs;

		default:
		return m_drgbs;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::CleanupData(void)
{
	if(m_srcrgbs)
	{
		delete m_srcrgbs;
		m_srcrgbs = NULL;
	}
	if(m_dryrgbs)
	{
		delete m_dryrgbs;
		m_dryrgbs = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::InitData(void)
{
	if(_profiledocfiles.initPrintData() == MCO_SUCCESS)	
		return TRUE;

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetProfileModified(BOOL bModified)
{
	SetModifiedFlag(bModified);
	if (bModified)
		UpdateTabStates();
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::UpdateTabStates(void)
{
	POSITION p = GetFirstViewPosition();
	CProfileView* pProfileView = (CProfileView*)GetNextView(p);
	if (!pProfileView) return;
	CProfileSheet* pProfileSheet = pProfileView->GetPropertySheet();
	if (!pProfileSheet) return;

	// Get the current enabled state of the tabs
	BOOL bEnabledOrig[5];
	BOOL bEnabled[5];
	for (int i=0; i<5; i++)
		bEnabledOrig[i] = bEnabled[i] = pProfileSheet->TabIsEnabled(i);

	// Set the new state of the tabs
	BOOL bHasData = FALSE;
	if ( _profiledocfiles.printData->srcpatch.name[0] != '\0' &&
		 _profiledocfiles.printData->destpatch.name[0] != '\0' )
		bHasData = TRUE;

	bEnabled[0] = bHasData;		// The Calibrate Tab
	bEnabled[1] = bHasData;		// The Ton Adjust Tab
	bEnabled[2] = TRUE;			// The Color Data tab is always enabled
	bEnabled[3] = bHasData;		// The Settings Tab
	bEnabled[4] = bHasData;		// The Tuning Tab


	if(*m_monacoDongle && *m_monacoProfile && !*m_monacoEdited)
		bEnabled[2] = TRUE;
	else
		bEnabled[2] = FALSE;

	if(*m_sourceexist == No_Patches || *m_dryjetexist == No_Patches)
	{
		bEnabled[3] = FALSE;
		bEnabled[4] = FALSE;

		if(*m_createdProfile)
		{
			bEnabled[0] = FALSE;
			bEnabled[1] = FALSE;
		}
		else
		{
			bEnabled[0] = TRUE;
			bEnabled[1] = TRUE;
		}
	}
	else
	{
		bEnabled[0] = TRUE;
		bEnabled[1] = TRUE;
		if(*m_monacoDongle && _profiledocfiles.validCal()){
			bEnabled[3] = TRUE;
			bEnabled[4] = TRUE;
		}
		else{
			bEnabled[3] = FALSE;
			bEnabled[4] = FALSE;
		}
	}

	// Copy the enabled states back to the tabs, if they changed
	for (i=0; i<5; i++)
	{
		if (bEnabledOrig[i] != bEnabled[i])
			pProfileSheet->EnableTab(i, bEnabled[i]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProfileDoc commands

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::OnNewDocument()
{
	CleanupData();
	InitData();

	if (!CDocument::OnNewDocument())
		return FALSE;

	UpdateTabStates();
	SetProfileModified(FALSE);
	UpdateAllViews(NULL, 1/*lHint*/);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::OnOpenDocument( LPCTSTR fname )
{
	CleanupData();
	InitData();

	Mpfileio mpfileio(_profiledocfiles.printData);

	McoStatus status = mpfileio.openProfile((char*)fname);
	if (status != MCO_SUCCESS)
	{
		Message("Error opening '%s'.\nIt is not a valid ICC linked profile.", fname);
		return FALSE;
	}

	status = _profiledocfiles.copyOutOfPrintData();
	if (status != MCO_SUCCESS)
	{
		Message("Error opening '%s'.\nIt is not a valid ICC linked profile.", fname);
		return FALSE;
	}

	PrintData *printData = _profiledocfiles.printData;

	if ((printData->icctype == NOT_MONACO_ICC) || (printData->icctype == MONACOP_CREATED_ICC)) 
		*m_monacoProfile = 0;
	else 
		*m_monacoProfile = 1;
	
	if ((printData->icctype == MONACO_EDITED_ICC) || (printData->icctype == MONACOP_EDITED_ICC)) 
		*m_monacoEdited = 1;
	else 
		*m_monacoEdited= 0;
	
	*m_changedProfile = 0;
	*m_createdProfile = 0;
	*m_changedLinearTone = 0;

	UpdateTabStates();
	SetProfileModified(FALSE);
	UpdateAllViews(NULL, 1/*lHint*/); 

	*m_needSave = 0;

	strcpy(m_fname, fname);
	return TRUE;
}

/*
/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::CreateProfile( LPCTSTR fname )
{
	McoStatus status;

	status = _profiledocfiles.copyOutOfPrintData();
	if(status != MCO_SUCCESS)
	{
		Message("Error creating ICC linked profile '%s'\n.", fname);
		return FALSE;
	}

	CBuildProgress	buildprogress( (long)&_profiledocfiles, 0, Process_BuildingTable);

	if(buildprogress.DoModal() == IDCANCEL)
	{
		Message("Error creating ICC linked profile '%s'\n.", fname);
		return FALSE;
	}
		
	char *linkH;
	linkH = new char[_profiledocfiles._rev_table_size*4];
	if(!linkH)
	{
		Message("Memory error.");
		return FALSE;
	}

	status = _profiledocfiles._createLinktable((McoHandle)linkH);
	if(status != MCO_SUCCESS)	
	{
		Message("Error creating ICC linked profile '%s'\n.", fname);
		return FALSE;
	}

	_profiledocfiles.printData->tableH = linkH;

	return TRUE;
}
*/

/*
	else{
		CopyFile(m_fname, fname, FALSE);
	}

//duplicate the original file if it exists
	char temp_name[256];

	strcpy(temp_name, fname);
	strcat(temp_name, "_0000temp" );
	CopyFile(fname, temp_name, FALSE); 

	Mpfileio mpfileio(_profiledocfiles.printData);

	status = mpfileio.saveProfile((char*)fname);
	if (status != MCO_SUCCESS)
	{
		if(strcmp(m_fname, fname) == 0)
		{
			DeleteFile(fname);
			CopyFile(temp_name, fname, FALSE);
		}
		else
		{
			DeleteFile(fname);
			DeleteFile(temp_name);
		}
		Message("Error saving '%s'\nas an ICC linked profile.", fname);
		return FALSE;
	}

	//delete the original file if it exists
	BOOL bDeleted = DeleteFile(temp_name);
*/

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::OnSaveDocument( LPCTSTR fname )
{
	McoStatus status;
	int	changed = *m_changedProfile;

	status = _profiledocfiles.copyOutOfPrintData();
	if(status != MCO_SUCCESS)
	{
		Message("Error creating ICC linked profile '%s'\n.", fname);
		return FALSE;
	}

	const char*	name = strrchr( fname, '\\' );
	if( name != NULL ) name++;

	CBuildProgress	buildprogress( (long)&_profiledocfiles, 0, Process_BuildingTable, name);

	if(buildprogress.DoModal() == IDCANCEL)
	{
		goto bail;
	}

	char *linkH;

	if(changed){		
		linkH = new char[_profiledocfiles._rev_table_size*4];
		if(!linkH)
		{
			goto bail;
		}

		status = _profiledocfiles._createLinktable((McoHandle)linkH);
		if(status != MCO_SUCCESS)	
		{
			goto bail;
		}

		_profiledocfiles.printData->tableH = linkH;
	}

	if( strcmp(m_fname, fname) == 0)	//same name
	{
		//duplicate the original file if it exists
		char temp_name[256];

		strcpy(temp_name, fname);
		strcat(temp_name, "_0000temp" );
		CopyFile(fname, temp_name, FALSE); 

		Mpfileio mpfileio(_profiledocfiles.printData);

		status = mpfileio.saveProfile((char*)fname);
		if (status != MCO_SUCCESS)
		{
			DeleteFile(fname);
			CopyFile(temp_name, fname, FALSE);
			goto bail;
		}

		DeleteFile(temp_name);
	}
	else //different name
	{
		Mpfileio mpfileio(_profiledocfiles.printData);

		status = mpfileio.saveProfile((char*)fname);
		if (status != MCO_SUCCESS)
			goto bail;
	}

	*m_changedProfile = 0;
	*m_changedLinearTone = 0;
	*m_needSave = 0;
	strcpy(m_fname, fname);
	SetProfileModified(FALSE);
	return TRUE;

bail:
	*m_changedProfile = changed;
	Message("Error saving '%s'\nas an ICC linked profile.", fname);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::OnCloseDocument() 
{
	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::GetCalibrateDescription( CString& szDescription )
{
	int iLastByte = sizeof(_profiledocfiles.printData->setting.linearDesc) - 1;
	// be sure it's null terminated
	_profiledocfiles.printData->setting.linearDesc[iLastByte] = '\0';
	szDescription = _profiledocfiles.printData->setting.linearDesc;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetCalibrateDescription( CString szDescription )
{
	strcpy( _profiledocfiles.printData->setting.linearDesc, szDescription );
	*m_needSave = 1;
	SetProfileModified(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetCalibrateData( CMYK iCurve, double* x, double* y, int nMaxPoints )
{
	int nPoints = _profiledocfiles.printData->setting.numLinearHand[iCurve];

	if (nPoints > nMaxPoints)
		nPoints = nMaxPoints;

	if (nPoints > MAX_LINEAR_HAND)
		nPoints = MAX_LINEAR_HAND;

	for ( int i = 0; i < nPoints; i++ )
	{
		*x++ = _profiledocfiles.printData->setting.linearX[(iCurve*MAX_LINEAR_HAND) + i]/100;
		*y++ = _profiledocfiles.printData->setting.linearY[(iCurve*MAX_LINEAR_HAND) + i]/2;
	}

	return nPoints;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetCalibrateData( CMYK iCurve, double* x, double* y, int nPoints )
{
	if (nPoints > MAX_LINEAR_HAND)
		nPoints = MAX_LINEAR_HAND;

	for ( int i = 0; i < nPoints; i++ )
	{
		_profiledocfiles.printData->setting.linearX[(iCurve*MAX_LINEAR_HAND) + i] = *x++*100;
		_profiledocfiles.printData->setting.linearY[(iCurve*MAX_LINEAR_HAND) + i] = *y++*2;
	}

	_profiledocfiles.printData->setting.numLinearHand[iCurve] = nPoints;
	*m_changedLinearTone = 1;
	SetProfileModified(TRUE);
	return TRUE;
}

BOOL CProfileDoc::GetCalibrateDate( long* date)
{
	*date = _profiledocfiles.printData->setting.linearDate;
	return TRUE;
}

BOOL CProfileDoc::SetCalibrateDate( long date)
{
	_profiledocfiles.printData->setting.linearDate = date;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::GetToneAdjustDescription( CString& szDescription )
{
	int iLastByte = sizeof(_profiledocfiles.printData->setting.toneDesc) - 1;
	// be sure it's null terminated
	_profiledocfiles.printData->setting.toneDesc[iLastByte] = '\0';
	szDescription = _profiledocfiles.printData->setting.toneDesc;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetToneAdjustDescription( CString szDescription )
{
	strcpy( _profiledocfiles.printData->setting.toneDesc, szDescription );
	*m_needSave = 1;	
	SetProfileModified(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetToneAdjustData( CMYK iCurve, double* x, double* y, int nMaxPoints )
{
	int nPoints = _profiledocfiles.printData->setting.numToneHand[iCurve];

	if (nPoints > nMaxPoints)
		nPoints = nMaxPoints;

	if (nPoints > MAX_LINEAR_HAND)
		nPoints = MAX_LINEAR_HAND;

	for ( int i = 0; i < nPoints; i++ )
	{
		*x++ = _profiledocfiles.printData->setting.toneX[(iCurve*MAX_TONE_HAND) + i]/100;
		*y++ = _profiledocfiles.printData->setting.toneY[(iCurve*MAX_TONE_HAND) + i]/100;
	}

	return nPoints;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetToneAdjustData( CMYK iCurve, double* x, double* y, int nPoints )
{
	if (nPoints > MAX_TONE_HAND)
		nPoints = MAX_TONE_HAND;

	for ( int i = 0; i < nPoints; i++ )
	{
		_profiledocfiles.printData->setting.toneX[(iCurve*MAX_TONE_HAND) + i] = *x++*100;
		_profiledocfiles.printData->setting.toneY[(iCurve*MAX_TONE_HAND) + i] = *y++*100;
	}

	_profiledocfiles.printData->setting.numToneHand[iCurve] = nPoints;
	*m_changedLinearTone = 1;
	SetProfileModified(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::GetColorSourceDescription( CString& szDescription )
{
	int iLastByte = sizeof(_profiledocfiles.printData->srcpatch.name) - 1;
	// be sure it's null terminated
	_profiledocfiles.printData->srcpatch.name[iLastByte] = '\0';
	szDescription = _profiledocfiles.printData->srcpatch.name;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetColorSourceDescription( CString szDescription )
{
	strcpy( _profiledocfiles.printData->srcpatch.name, szDescription );
	*m_needSave = 1;
	SetProfileModified(TRUE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::GetColorDryjetDescription( CString& szDescription )
{
	int iLastByte = sizeof(_profiledocfiles.printData->destpatch.name) - 1;
	// be sure it's null terminated
	_profiledocfiles.printData->destpatch.name[iLastByte] = '\0';
	szDescription = _profiledocfiles.printData->destpatch.name;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SetColorDryjetDescription( CString szDescription )
{
	strcpy( _profiledocfiles.printData->destpatch.name, szDescription );
	*m_needSave = 1;
	SetProfileModified(TRUE);
	return TRUE;
}

BOOL CProfileDoc::GetDataDate( long* date, long which )
{
	if(which)	//source
		*date = _profiledocfiles.printData->setting.srcDate;
	else
		*date = _profiledocfiles.printData->setting.dstDate;
	return TRUE;
}

BOOL CProfileDoc::SetDataDate( long date, long which )
{
	if(which)	//source
		_profiledocfiles.printData->setting.srcDate = date;
	else
		_profiledocfiles.printData->setting.dstDate = date;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetColorData( double** ppfIndex, double** ppfL, double** ppfa, double** ppfb, int *nL, BOOL bSource )
{
	PatchFormat* patchdata;

	*nL = 0;
	if (bSource)
		patchdata = &(_profiledocfiles.printData->srcpatch);
	else
		patchdata = &(_profiledocfiles.printData->destpatch);

	if (!patchdata->data)
		return 0;

	int nValues = 0;
	int nLab = 0;
	for (int i = 0; i < (int)patchdata->numCube; i++)
		nLab += patchdata->steps[i] * patchdata->steps[i] * patchdata->steps[i];

	//density curve
	int nLinear = patchdata->numLinear;
	if(nLinear)	nLinear = (nLinear - 1)*patchdata->numComp;
	nValues = nLab + nLinear;
		
	if (nValues <= 0 || nValues > 4000) // a safety valve
		return 0;

	// If caller is just getting the patch count, get out...
	if (!ppfIndex || !ppfL || !ppfa || !ppfb )
		return nValues;

	double* pfIndex = new double[nValues];
	double* pfL = new double[nValues];
	double* pfa = new double[nValues];
	double* pfb = new double[nValues];
	if (!pfIndex || !pfL || !pfa || !pfb)
	{
		if (pfIndex) delete [] pfIndex;
		if (pfL) delete [] pfL;
		if (pfa) delete [] pfa;
		if (pfb) delete [] pfb;
		return 0;
	}

	// Pass the data back to the caller
	*ppfIndex = pfIndex;
	*ppfL = pfL;
	*ppfa = pfa;
	*ppfb = pfb;

	double fRow = 0.0;
/*
	for (i = 0; i < nValues; i++)
	{
		*pfIndex++ = fRow / (nValues-1); fRow++;
		int j = i * 3;
		*pfL++ = patchdata->data[j + 0] / 100;
		*pfa++ = patchdata->data[j + 1] / 100;
		*pfb++ = patchdata->data[j + 2] / 100;
	}
*/
	for (i = 0; i < nLab; i++)
	{
		*pfIndex++ = fRow / (nValues-1); fRow++;
		int j = i * 3;
		*pfL++ = patchdata->data[j + 0] / 100;
		*pfa++ = patchdata->data[j + 1] / 100;
		*pfb++ = patchdata->data[j + 2] / 100;
	}

	for ( i = 0; i < nLinear; i++)
	{
		*pfIndex++ = fRow / (nValues-1); fRow++;
		int j = i * 3;
		*pfL++ = patchdata->ldata[j + 0] / 100;
		*pfa++ = patchdata->ldata[j + 1] / 100;
		*pfb++ = patchdata->ldata[j + 2] / 100;
	}

	//create rgb table
	if(bSource)
		FormatToRgbs(&m_srcrgbs, patchdata);
	else
		FormatToRgbs(&m_dryrgbs, patchdata);
	
	*nL = nLinear;	
	return nValues;
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetColorSourceData( double** ppfIndex, double** ppfL, double** ppfa, double** ppfb, int *nLinear )
{
	return GetColorData( ppfIndex, ppfL, ppfa, ppfb, nLinear, TRUE/*bSource*/ );
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetColorDryjetData( double** ppfIndex, double** ppfL, double** ppfa, double** ppfb, int *nLinear )
{
	return GetColorData( ppfIndex, ppfL, ppfa, ppfb, nLinear, FALSE/*bSource*/ );
}

/////////////////////////////////////////////////////////////////////////////
//P
BOOL CProfileDoc::GetSettingAutomatic( void )
{
	if(_profiledocfiles.printData->setting.flag & 2)
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//P
void CProfileDoc::SetSettingAutomatic( BOOL bValue )
{
	if(bValue){
		if( _profiledocfiles.printData->setting.flag & 2)
			_profiledocfiles.printData->setting.flag -= 2;
	}			
	else
		_profiledocfiles.printData->setting.flag |= 2; 
				
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::GetSettingTextEnhance( void )
{
	return _profiledocfiles.printData->setting.kOnly;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingTextEnhance( BOOL bValue )
{
	_profiledocfiles.printData->setting.kOnly = bValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetSettingColorBalance( void )
{
	return _profiledocfiles.printData->setting.colorBalance;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingColorBalance( int iValue )
{
	_profiledocfiles.printData->setting.colorBalance = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetSettingInkNeutralize( void )
{
	return _profiledocfiles.printData->setting.inkNeutralize;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingInkNeutralize( int iValue )
{
	_profiledocfiles.printData->setting.inkNeutralize = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetSettingColorBalanceCutoff( void )
{
	return _profiledocfiles.printData->setting.colorBalanceCut;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingColorBalanceCufoff( int iValue )
{
	_profiledocfiles.printData->setting.colorBalanceCut = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetSettingContrast( void )
{
	return _profiledocfiles.printData->setting.linear;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingContrast( int iValue )
{
	_profiledocfiles.printData->setting.linear = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetSettingSaturation( void )
{
	return _profiledocfiles.printData->setting.saturation;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::SetSettingSaturation( int iValue )
{
	_profiledocfiles.printData->setting.saturation = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
//P
int CProfileDoc::GetSettingSmoothData( void )
{
	return _profiledocfiles.printData->setting.smoothSimu;
}

/////////////////////////////////////////////////////////////////////////////
//P
void CProfileDoc::SetSettingSmoothData( int iValue )
{
	_profiledocfiles.printData->setting.smoothSimu = iValue;
	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::LoadPatch(char* patchname, long which)
{
	Mpfileio mpfileio(_profiledocfiles.printData);

	McoStatus status = mpfileio.loadPatchdata(patchname, which);
	if(status == MCO_SUCCESS){
		status = _profiledocfiles.copyOutOfPrintData();
		if(status == MCO_SUCCESS){
			*m_changedProfile = 1;
			SetProfileModified(TRUE);
		}
	}
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SavePatch(char* patchname, long which)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.savePatchdata(patchname, which);
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::LoadLinear(char* patchname)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.loadLineardata(patchname);
	if(status == MCO_SUCCESS){
		*m_changedLinearTone = 1;	
		SetProfileModified(TRUE);
	}
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::LoadLinearFromPatch(void)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.loadLineardata();
	if(status == MCO_SUCCESS){
		*m_changedLinearTone = 1;	
		SetProfileModified(TRUE);
	}
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SaveLinear(char* patchname)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.saveLineardata(patchname);
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::LoadTone(char* patchname)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.loadTonedata(patchname);
	if(status == MCO_SUCCESS){
		*m_changedLinearTone = 1;	
		SetProfileModified(TRUE);
	}
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProfileDoc::SaveTone(char* patchname)
{
	Mpfileio mpfileio(_profiledocfiles.printData);
	McoStatus status = mpfileio.saveTonedata(patchname);
	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
int CProfileDoc::GetTweaks(Tweak_Element* pTweaksArray[], int nTweaks)
{
	if (!pTweaksArray)
		return 0;

	// Delete any existing tweaks in the array
	for (int i = 0; i< nTweaks; i++)
	{
		delete pTweaksArray[i];
		pTweaksArray[i] = NULL;
	}
  
	nTweaks = 0;

	// Copy the tweaks from the document
	for (i = 0; i < _profiledocfiles.printData->numTweaks; i++)
	{
		if (i >= MAX_NUM_TWEAKS)
			break; 

		TweakElement* pDocTweak = &_profiledocfiles.printData->tweakele[i];
		if (!pDocTweak)
			break;

		Tweak_Element* pTweak = new Tweak_Element(pDocTweak->name);
		if (!pTweak)
			break;

		pTweak->lab_d[0] = pDocTweak->lab_d[0];
		pTweak->lab_d[1] = pDocTweak->lab_d[1];
		pTweak->lab_d[2] = pDocTweak->lab_d[2];
		
		pTweak->lab_p[0] = pDocTweak->lab_p[0];
		pTweak->lab_p[1] = pDocTweak->lab_p[1];
		pTweak->lab_p[2] = pDocTweak->lab_p[2];
		
		pTweak->lab_g[0] = pDocTweak->lab_g[0];
		pTweak->lab_g[1] = pDocTweak->lab_g[1];
		pTweak->lab_g[2] = pDocTweak->lab_g[2];

		pTweak->Lr = pDocTweak->Lr;
		pTweak->Cr = pDocTweak->Cr;
		pTweak->type = (Tweak_Types)pDocTweak->type;
		memcpy(pTweak->name, pDocTweak->name, sizeof(pTweak->name));

		pTweaksArray[i] = pTweak;
		nTweaks++;
	}

	return nTweaks;
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::PutTweaks(Tweak_Element* pTweaksArray[], int nTweaks)
{
	if (!pTweaksArray)
		return;

	// Delete any existing document tweaks
	if (_profiledocfiles.printData->tweakele)
		delete _profiledocfiles.printData->tweakele;
	_profiledocfiles.printData->tweakele = NULL;
	_profiledocfiles.printData->numTweaks = 0;

	if (!nTweaks)
		return;

	// Allocate space for the new doc tweaks
	_profiledocfiles.printData->tweakele = new TweakElement[nTweaks];

	// Copy the local tweaks into the document
	for (int i = 0; i < nTweaks; i++)
	{
		TweakElement* pDocTweak = &_profiledocfiles.printData->tweakele[i];
		if (!pDocTweak)
			continue;

		Tweak_Element* pTweak = pTweaksArray[i];
		if (!pTweak)
			continue;

		pDocTweak->lab_d[0] = pTweak->lab_d[0];
		pDocTweak->lab_d[1] = pTweak->lab_d[1];
		pDocTweak->lab_d[2] = pTweak->lab_d[2];
		    
		pDocTweak->lab_p[0] = pTweak->lab_p[0];
		pDocTweak->lab_p[1] = pTweak->lab_p[1];
		pDocTweak->lab_p[2] = pTweak->lab_p[2];
		    
		pDocTweak->lab_g[0] = pTweak->lab_g[0];
		pDocTweak->lab_g[1] = pTweak->lab_g[1];
		pDocTweak->lab_g[2] = pTweak->lab_g[2];

		pDocTweak->Lr = pTweak->Lr;
		pDocTweak->Cr = pTweak->Cr;
		pDocTweak->type = pTweak->type;
		memcpy(pDocTweak->name, pTweak->name, sizeof(pDocTweak->name));

		_profiledocfiles.printData->numTweaks++;
	}

	*m_changedProfile = 1;
	SetProfileModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CProfileDoc diagnostics

#ifdef _DEBUG
void CProfileDoc::AssertValid() const
{
	CDocument::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CProfileDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CProfileDoc::OnFileRevert() 
{
	// TODO: Add your command handler code here
	if( (*m_changedProfile || *m_needSave || *m_changedLinearTone) &&
		(*m_monacoProfile || *m_monacoEdited) ){

		return	OnOpenDocument(m_fname);
	}
	
	return FALSE;
}

void CProfileDoc::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( FALSE );	
}

void CProfileDoc::OnUpdateFileOpen(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( FALSE );	
}

BOOL CProfileDoc::IsProfileSavable(void)
{
	if(_profiledocfiles.dataPresent() && (*m_changedProfile || *m_needSave || *m_changedLinearTone) )
		return TRUE;
	else
		return FALSE;
}
						
void CProfileDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( IsProfileSavable() )
		pCmdUI->Enable( TRUE );	
	else
		pCmdUI->Enable( FALSE );			
}

void CProfileDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( IsProfileSavable() )
		pCmdUI->Enable( TRUE );	
	else
		pCmdUI->Enable( FALSE );			
}

void CProfileDoc::OnUpdateFileRevert(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( (*m_changedProfile || *m_needSave || *m_changedLinearTone) && 
		( (*m_monacoProfile && !*m_createdProfile) || *m_monacoEdited))
		pCmdUI->Enable( TRUE );	
	else
		pCmdUI->Enable( FALSE );			
}

void CProfileDoc::OnUpdateEditPreferences(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable( TRUE );	
}

void CProfileDoc::EditPreferences(BOOL labdata)
{
	m_measureLab = labdata;
	OnEditPreferences();
}

void CProfileDoc::OnEditPreferences() 
{
	// TODO: Add your command handler code here
	int	result;
	int	baudrate[] = { 4800, 9600 };
	int	frequency[] = { 50, 100 };
	int i;

	// TODO: Add your control notification handler code here
	CDeviceSheet	devicesheet(IDS_DEVICE);

	for(i = 0; i < MAX_NUM_DEVICES; i++) devicesheet.m_deviceSetting.m_deviceList[i] = FALSE;
	if(m_measureLab)
	{
		devicesheet.m_deviceSetting.m_deviceList[0]	= TRUE;
		if(m_deviceSetup.deviceType == XRITE_408)
			m_deviceSetup.deviceType = GRETAG_SCAN;
	}
	else
	{
		devicesheet.m_deviceSetting.m_deviceList[0]	= TRUE;
		devicesheet.m_deviceSetting.m_deviceList[2]	= TRUE;
	}

	devicesheet.m_deviceSetting.m_port  = m_deviceSetup.port;
	for(i = 0; i < 2; i++){
		if(baudrate[i] == m_deviceSetup.baudRate)
			break;
	}
	devicesheet.m_deviceSetting.m_baudRate = i;
	devicesheet.m_deviceSetting.m_deviceType = m_deviceSetup.deviceType;
	for(i = 0; i < 2; i++){
		if(frequency[i] == m_deviceSetup.calFrequency)
			break;
	}
	devicesheet.m_patchFormat.m_calFrequency = i;

	if( devicesheet.DoModal() == IDOK){
		m_deviceSetup.port = devicesheet.m_deviceSetting.m_port;
		m_deviceSetup.baudRate = baudrate[devicesheet.m_deviceSetting.m_baudRate];
		m_deviceSetup.deviceType = devicesheet.m_deviceSetting.m_deviceType;
		m_deviceSetup.calFrequency = frequency[devicesheet.m_patchFormat.m_calFrequency];
	}

}

void CProfileDoc::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	CString dirname((LPCSTR)IDS_PROFILE_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);
	
	CDocument::OnFileSaveAs();	
}

void CProfileDoc::OnFileSave() 
{
	// TODO: Add your command handler code here
	CString filename = GetPathName( );
	if( filename.IsEmpty() )
	{
		CString dirname((LPCSTR)IDS_PROFILE_DIRNAME);
		BOOL exist = SetDefaultDir((LPCSTR)dirname);
	}

	CDocument::OnFileSave();	
}

BOOL CProfileDoc::SaveModified() 
{
	if (!IsModified())
		return TRUE;        // ok to continue

	BOOL	saveflag = IsProfileSavable();

	CString name = GetTitle();
	if (name.IsEmpty())
	{
		// get name based on caption
		name = m_strTitle;
		if (name.IsEmpty())
			VERIFY(name.LoadString(AFX_IDS_UNTITLED));
	}

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);

	CString dirname((LPCSTR)IDS_PROFILE_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);

	switch (SaveMessageBox(prompt, saveflag))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

int CProfileDoc::SaveMessageBox(CString prompt, BOOL save)
{
	MyMessageBox	box(prompt, !save);
	
	int which = box.DoModal();		 		
	return which;
}
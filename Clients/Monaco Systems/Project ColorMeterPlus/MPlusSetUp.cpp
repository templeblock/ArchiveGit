// MPlusSetUp.cpp : implementation file
//

#include "stdafx.h"
#include "Color Meter Plus.h"
#include "MPlusSetUp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SetUpProfile gSetUpProfile;
extern ReferenceFile MonitorData;
double gColorTemp;
FILE *gFilePtr;
BOOL WriteToFile;

/////////////////////////////////////////////////////////////////////////////
// CMPlusSetUp dialog


CMPlusSetUp::CMPlusSetUp(CWnd* pParent /*=NULL*/)
	: CDialog(CMPlusSetUp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMPlusSetUp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMPlusSetUp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMPlusSetUp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMPlusSetUp, CDialog)
	//{{AFX_MSG_MAP(CMPlusSetUp)
	ON_BN_CLICKED(IDC_RADIO_FAST, OnRadioFast)
	ON_BN_CLICKED(IDC_RADIO_MEDIUM, OnRadioMedium)
	ON_BN_CLICKED(IDC_RADIO_SLOW, OnRadioSlow)
	ON_BN_CLICKED(IDC_RADIO_LUV, OnRadioLuv)
	ON_BN_CLICKED(IDC_RADIO_YXY, OnRadioYxy)
	ON_BN_CLICKED(IDC_RADIO_RGB_MATCH, OnRadioRgbMatch)
	ON_BN_CLICKED(IDC_RADIO_FTLAMBERTS, OnRadioFtlamberts)
	ON_BN_CLICKED(IDC_RADIO_CANDELAS, OnRadioCandelas)
	ON_BN_CLICKED(IDC_BUTTON_MEAS_FREQ, OnButtonMeasFreq)
	ON_BN_CLICKED(IDC_SAVEREFERENCE, OnSavereference)
	ON_NOTIFY(NM_CLICK, IDC_SPIN1, OnClickSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMPlusSetUp message handlers

void CMPlusSetUp::OnRadioFast() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_FAST,1);
	CheckDlgButton(IDC_RADIO_MEDIUM,0);
	CheckDlgButton(IDC_RADIO_SLOW,0);
	gSetUpProfile.whichSpeed = MFAST;
}

void CMPlusSetUp::OnRadioMedium() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_FAST,0);
	CheckDlgButton(IDC_RADIO_MEDIUM,1);
	CheckDlgButton(IDC_RADIO_SLOW,0);
	gSetUpProfile.whichSpeed = MMED;

}

void CMPlusSetUp::OnRadioSlow() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_FAST,0);
	CheckDlgButton(IDC_RADIO_MEDIUM,0);
	CheckDlgButton(IDC_RADIO_SLOW,1);
	gSetUpProfile.whichSpeed = MSLOW;

}

void CMPlusSetUp::OnRadioLuv() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_YXY,0);
	CheckDlgButton(IDC_RADIO_LUV,1);
	CheckDlgButton(IDC_RADIO_RGB_MATCH,0);
	gSetUpProfile.whichDisplay = MUVL;


}

void CMPlusSetUp::OnRadioYxy() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_YXY,1);
	CheckDlgButton(IDC_RADIO_LUV,0);
	CheckDlgButton(IDC_RADIO_RGB_MATCH,0);
	gSetUpProfile.whichDisplay = MXYL;

}

void CMPlusSetUp::OnRadioRgbMatch() 
{
	// TODO: Add your control notification handler code here

		OPENFILENAME fStuf;
	DWORD dw_error;
	
	char FileName[256];
	char SysDir[256];
	
	short offset;
	char *Filter = "Reference Files\0*.brf\0All\0*.*\0";
	


	GetSystemDirectory( SysDir,256);
  	strcat (SysDir,"\\Sequel\\");

	

	fStuf.lStructSize = sizeof(OPENFILENAME);
	fStuf.hwndOwner = m_hWnd;
	fStuf.hInstance = AfxGetInstanceHandle( );
	fStuf.lpstrFilter = Filter;
	fStuf.lpstrCustomFilter = NULL;
	fStuf.nMaxCustFilter = NULL;
	fStuf.nFilterIndex = 1;
	fStuf.lpstrFile = NULL;
	fStuf.nMaxFile = 1024;
	fStuf.lpstrFileTitle = FileName;
	fStuf.nMaxFileTitle = 256;
	fStuf.lpstrInitialDir = SysDir;
	fStuf.lpstrTitle = "Input a Reference File..";
	fStuf.Flags = OFN_EXPLORER|OFN_LONGNAMES|OFN_HIDEREADONLY;
	
	if(!GetOpenFileName(&fStuf))
		return;
	dw_error = CommDlgExtendedError();
		
	strcat(SysDir,FileName);

	FILE * fout = fopen(SysDir,"r+b");
	if(fout == NULL)
		return;
	fread(&MonitorData,sizeof(ReferenceFile),1,fout);
	strcpy(gSetUpProfile.whichFileName,FileName);
	CheckDlgButton(IDC_RADIO_YXY,0);
	CheckDlgButton(IDC_RADIO_LUV,0);
	CheckDlgButton(IDC_RADIO_RGB_MATCH,1);
	gSetUpProfile.whichDisplay = MRGB;
	
	
}

void CMPlusSetUp::OnRadioFtlamberts() 
{
	// TODO: Add your control notification handler code here
	CheckDlgButton(IDC_RADIO_FTLAMBERTS,1);
	CheckDlgButton(IDC_RADIO_CANDELAS,0);
	
	gSetUpProfile.whichUnits = MENGLISH;
	
}

void CMPlusSetUp::OnRadioCandelas() 
{
	// TODO: Add your control notification handler code here
	 CheckDlgButton(IDC_RADIO_FTLAMBERTS,0);
	CheckDlgButton(IDC_RADIO_CANDELAS,1);
	
	gSetUpProfile.whichUnits = MMETRIC;

}

BOOL CMPlusSetUp::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char freqString[256];
	// TODO: Add extra initialization here
	switch (gSetUpProfile.whichUnits)
	{
		case MMETRIC:
			CheckDlgButton(IDC_RADIO_FTLAMBERTS,0);
			CheckDlgButton(IDC_RADIO_CANDELAS,1);
			break;

		case MENGLISH:
			CheckDlgButton(IDC_RADIO_FTLAMBERTS,1);
			CheckDlgButton(IDC_RADIO_CANDELAS,0);
			break;
			
	}

	switch (gSetUpProfile.whichSpeed)
	{
		case MFAST:

			CheckDlgButton(IDC_RADIO_FAST,1);
			CheckDlgButton(IDC_RADIO_MEDIUM,0);
			CheckDlgButton(IDC_RADIO_SLOW,0);

		break;

		case MMED:

			CheckDlgButton(IDC_RADIO_FAST,0);
			CheckDlgButton(IDC_RADIO_MEDIUM,1);
			CheckDlgButton(IDC_RADIO_SLOW,0);

		break;

		case MSLOW:

			CheckDlgButton(IDC_RADIO_FAST,0);
			CheckDlgButton(IDC_RADIO_MEDIUM,0);
			CheckDlgButton(IDC_RADIO_SLOW,1);

		break;



	}
	switch (gSetUpProfile.whichDisplay)
	{
		case MUVL:

			CheckDlgButton(IDC_RADIO_YXY,0);
			CheckDlgButton(IDC_RADIO_LUV,1);
			CheckDlgButton(IDC_RADIO_RGB_MATCH,0);
		break;

		case MXYL:

			CheckDlgButton(IDC_RADIO_YXY,1);
			CheckDlgButton(IDC_RADIO_LUV,0);
			CheckDlgButton(IDC_RADIO_RGB_MATCH,0);
		break;

		case MRGB:

			CheckDlgButton(IDC_RADIO_YXY,0);
			CheckDlgButton(IDC_RADIO_LUV,0);
			CheckDlgButton(IDC_RADIO_RGB_MATCH,1);
		break;



	}
	 sprintf(freqString,"%6.3f",gSetUpProfile.whichFrequency);
   SetDlgItemText(IDC_FREQUENCY,freqString);
   gColorTemp = 6500.;
   sprintf(freqString ,"%6.3f",gColorTemp);
    SetDlgItemText(IDC_COLORTEMP_TEXT,freqString);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMPlusSetUp::OnButtonMeasFreq() 
{
	// TODO: Add your control notification handler code here
	double MonitorFrequency ;
	double dIntTime;

	sipGetIntegrationTime(&dIntTime);
		sipMeasMonFreqAndSetIntTime(50,&MonitorFrequency);
		sipSetIntegrationTime(dIntTime);
	char freqString[256];
	if(MonitorFrequency != -1)
	{
		gSetUpProfile.whichFrequency = MonitorFrequency;
		sprintf(freqString,"%6.3f",gSetUpProfile.whichFrequency);
	}
	else
		sprintf(freqString,"Error!!") ;
   
	SetDlgItemText(IDC_FREQUENCY,freqString);
   

}

void CMPlusSetUp::OnOK() 
{
	// TODO: Add extra validation here
	switch (gSetUpProfile.whichSpeed)
	{
		case MFAST:
			gSetUpProfile.whichIntegrationTime =  7/gSetUpProfile.whichFrequency;
		break; 

		case MMED:
			gSetUpProfile.whichIntegrationTime =  25/gSetUpProfile.whichFrequency;
		break;

		case MSLOW:
			gSetUpProfile.whichIntegrationTime =  100/gSetUpProfile.whichFrequency;
		break;

   	}

   
   sipSetIntegrationTime(gSetUpProfile.whichIntegrationTime);

	CDialog::OnOK();
}

void CMPlusSetUp::OnSavereference() 
{
	// TODO: Add your control notification handler code here
	
	OPENFILENAME fStuf;
	DWORD dw_error;
	
	char FileName[256];
	char SysDir[256];
	
	short offset;
	char *Filter = "Reference Files\0*.brf\0All\0*.*\0";
	MonitorData.CancelOp ==FALSE; // Assume all is well with the measurements;
	
	MonitorData.ReferenceCT = gColorTemp;

	MeasureThisMonitor.DoModal();
	
	 if (MonitorData.CancelOp == FALSE)
	 {
		GetSystemDirectory( SysDir,256);
  		strcat (SysDir,"\\Sequel\\");

		

		fStuf.lStructSize = sizeof(OPENFILENAME);
		fStuf.hwndOwner = m_hWnd;
		fStuf.hInstance = AfxGetInstanceHandle( );
		fStuf.lpstrFilter = Filter;
		fStuf.lpstrCustomFilter = NULL;
		fStuf.nMaxCustFilter = NULL;
		fStuf.nFilterIndex = 1;
		fStuf.lpstrFile = NULL;
		fStuf.nMaxFile = 1024;
		fStuf.lpstrFileTitle = FileName;
		fStuf.nMaxFileTitle = 256;
		fStuf.lpstrInitialDir = SysDir;
		fStuf.lpstrTitle = "Input a Reference File..";
		fStuf.Flags = OFN_EXPLORER|OFN_LONGNAMES|OFN_HIDEREADONLY;
		
		GetSaveFileName(&fStuf);
		dw_error = CommDlgExtendedError();
			
		strcat(SysDir,FileName);

		FILE * fout = fopen(SysDir,"w+b");

		fwrite(&MonitorData,sizeof(ReferenceFile),1,fout);
	 }
		


}

void CMPlusSetUp::OnClickSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CMPlusSetUp::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	char freqString[256];
    gColorTemp = gColorTemp - (pNMUpDown->iDelta*25);
	if(gColorTemp < 4500) gColorTemp = 4500;
	if(gColorTemp > 10000)gColorTemp = 10000;
   sprintf(freqString ,"%6.0f",gColorTemp);
    SetDlgItemText(IDC_COLORTEMP_TEXT,freqString);	
	
	*pResult = 0;
}

void CMPlusSetUp::OnCheck1() 
{
	// TODO: Add your control notification handler code here
		OPENFILENAME fStuf;
	DWORD dw_error;
	
	char FileName[256];
	char SysDir[256];
	
	short offset;
	char *Filter = "Measurement File\0*.prn\0All\0*.*\0";
	

	if(IsDlgButtonChecked( IDC_CHECK1)&&!WriteToFile)
	{
		GetSystemDirectory( SysDir,256);
  		strcat (SysDir,"\\Sequel\\");

		

		fStuf.lStructSize = sizeof(OPENFILENAME);
		fStuf.hwndOwner = m_hWnd;
		fStuf.hInstance = AfxGetInstanceHandle( );
		fStuf.lpstrFilter = Filter;
		fStuf.lpstrCustomFilter = NULL;
		fStuf.nMaxCustFilter = NULL;
		fStuf.nFilterIndex = 1;
		fStuf.lpstrFile = NULL;
		fStuf.nMaxFile = 1024;
		fStuf.lpstrFileTitle = FileName;
		fStuf.nMaxFileTitle = 256;
		fStuf.lpstrInitialDir = SysDir;
		fStuf.lpstrTitle = "Input a Reference File..";
		fStuf.Flags = OFN_EXPLORER|OFN_LONGNAMES|OFN_HIDEREADONLY;
		
		GetSaveFileName(&fStuf);
		dw_error = CommDlgExtendedError();
			
		strcat(SysDir,FileName);

		gFilePtr = fopen(SysDir,"w+t");
		WriteToFile = TRUE;
		//fwrite(&MonitorData,sizeof(ReferenceFile),1,fout);
	   }
		
		else
		{
		 if(WriteToFile) 
			fclose(gFilePtr);

		   WriteToFile = FALSE;
		}


}

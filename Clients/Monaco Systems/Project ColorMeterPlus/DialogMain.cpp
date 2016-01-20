// DialogMain.cpp : implementation file
//

#include "stdafx.h"
#include "Color Meter Plus.h"
#include "DialogMain.h"
#include "calcMatrix.h"
#include "PlankianColorTemp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global display definition 
#define LedLineLength 20
#define LedNumLines   5

static LEDdisplay gLEDdisplay,*gPtrLEDdisplay;
static char *ptrStrTitles[5]; // an array of 5 ptrs to space for string buffers 
static char FastToggle;
static char gstrBuf[LedLineLength * LedNumLines];
 static char gcVersion;
static RECT myMeterRect;
static	CWnd *myCMeterWindow; 
static	HWND myHMeterWindow;
static short CalPort;

static BOOL gMeasureLock;
static BOOL gDisplayCT;
// Global calibrator definition 

// CalInfo *myPtr2Calibrator;
 //static CalInfo myCalibrator;
 static dYxy myMeasurement;
extern SetUpProfile gSetUpProfile;
extern ReferenceFile MonitorData;
extern FILE *gFilePtr;
extern BOOL WriteToFile;
/////////////////////////////////////////////////////////////////////////////
// CDialogMain dialog


CDialogMain::CDialogMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMain::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMain)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMain)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogMain, CDialog)
	//{{AFX_MSG_MAP(CDialogMain)
	ON_BN_CLICKED(IDC_BUTTON_SETUP, OnButtonSetup)
	ON_BN_CLICKED(IDC_BUTTON_SAVEVALUE, OnButtonSavevalue)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_MEASURE, OnButtonMeasure)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_DISPLAYCT, OnDisplayct)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMain message handlers

void CDialogMain::OnButtonSetup() 
{
	// TODO: Add your control notification handler code here
	
	SetUpThisMeter.DoModal();
	if(gSetUpProfile.whichDisplay == MXYL)
	 {
		 strcpy (ptrStrTitles[0] , " Measure Yxy");
		 strcpy (ptrStrTitles[1] , "  Y = ????");
		 strcpy (ptrStrTitles[2] , "  x = .???");
		 strcpy (ptrStrTitles[3] , "  y = .???");
		 strcpy (ptrStrTitles[4] , " ");
	 }
	 if(gSetUpProfile.whichDisplay == MUVL)
	 {
		 strcpy (ptrStrTitles[0] , "Measure Lu'v'");
		 strcpy (ptrStrTitles[1] , " L = ????");
		 strcpy (ptrStrTitles[2] , " u' = .???");
		 strcpy (ptrStrTitles[3] , " v' = .???");
		 strcpy (ptrStrTitles[4] , " ");
	 }
	if(gSetUpProfile.whichDisplay == MRGB)
	 {
		 strcpy (ptrStrTitles[0] , " Set RGB");
		 strcpy (ptrStrTitles[1] , "");
		 strcpy (ptrStrTitles[2] , "");
		 strcpy (ptrStrTitles[3] , "");
		 strcpy (ptrStrTitles[4] , "");
	 }

	 SetDlgItemText(IDC_REFERENCE_TEXT, gSetUpProfile.whichFileName);

		 		PAINTSTRUCT ps; 
	BeginPaint(&ps);

	RefreshLedDisplay(myHMeterWindow, gPtrLEDdisplay);
	if(gSetUpProfile.whichDisplay == MRGB)
		DrawRGB_BarGraph(myHMeterWindow, gPtrLEDdisplay, 1.0,1.5,.8,BELinear);

	EndPaint(&ps);

}

void CDialogMain::OnButtonSavevalue() 
{
	// TODO: Add your control notification handler code here

	

	OPENFILENAME fStuf;
	DWORD dw_error;
	
	char FileName[256];
	char SysDir[256];
	
	short offset;
	char *Filter = "Reference Files\0*.brf\0All\0*.*\0";
	MonitorData.CancelOp ==FALSE; // Assume all is well with the measurements;

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

void CDialogMain::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
		/* initialize and define the graphical meter */

	 		PAINTSTRUCT ps; 
	BeginPaint(&ps);

	DisplayMeasurement(	&myMeasurement,gPtrLEDdisplay,&gSetUpProfile);

	EndPaint(&ps);



	
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CDialogMain::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
		short i;

	WriteToFile = FALSE;
	myCMeterWindow = GetDlgItem(IDC_METERWINDOW);
	myHMeterWindow = myCMeterWindow->m_hWnd;
	myCMeterWindow->GetClientRect(&myMeterRect);

	 FastToggle = 0;	
	 for(i=0; i< LedNumLines; i++)
	 	ptrStrTitles[i] = &gstrBuf[LedLineLength * i];
	 if(gSetUpProfile.whichDisplay == MXYL)
	 {
		 strcpy (ptrStrTitles[0] , " Measure Yxy");
		 strcpy (ptrStrTitles[1] , "  Y = ????");
		 strcpy (ptrStrTitles[2] , "  x = .???");
		 strcpy (ptrStrTitles[3] , "  y = .???");
		 strcpy (ptrStrTitles[4] , " ");
	 }
	 if(gSetUpProfile.whichDisplay == MUVL)
	 {
		 strcpy (ptrStrTitles[0] , "Measure Lu'v'");
		 strcpy (ptrStrTitles[1] , " L = ????");
		 strcpy (ptrStrTitles[2] , " u' = .???");
		 strcpy (ptrStrTitles[3] , " v' = .???");
		 strcpy (ptrStrTitles[4] , " ");
	 }
	if(gSetUpProfile.whichDisplay == MRGB)
	 {
		 strcpy (ptrStrTitles[0] , " Set RGB");
		 strcpy (ptrStrTitles[1] , "");
		 strcpy (ptrStrTitles[2] , "");
		 strcpy (ptrStrTitles[3] , "");
		 strcpy (ptrStrTitles[4] , "");
		 SetDlgItemText(IDC_REFERENCE_TEXT,	gSetUpProfile.whichFileName);
	 }

	
	 
	gLEDdisplay.nLines = 5;
	gLEDdisplay.nCharsPerLine=15; 
	gLEDdisplay.AspectRatio= 1;
	gLEDdisplay.DisplayWidth = (short)(myMeterRect.right - myMeterRect.left);
	gLEDdisplay.DisplayHeight = (gLEDdisplay.DisplayWidth*9)/7;
	gLEDdisplay.Red = 255;
	gLEDdisplay.Green = 128;
	gLEDdisplay.Blue = 0;
	gLEDdisplay.LineStrings = ptrStrTitles;

	gPtrLEDdisplay = &gLEDdisplay;
	
	//short CenterHeight = ((myMeterRect.top - myMeterRect.bottom)-gLEDdisplay.DisplayHeight)/2;
	DefineLedDisplay(myHMeterWindow	, 0, 10, gPtrLEDdisplay);
	RefreshLedDisplay(myHMeterWindow,gPtrLEDdisplay);
	 if(gSetUpProfile.whichDisplay == MRGB)
		DrawRGB_BarGraph(myHMeterWindow, gPtrLEDdisplay, 0,0,0,BELinear);

	// now initialize and find the calibrator;

		// Now initialize the calibrator here:
	

long lerr =	sipOpenCalibrator(1,&gcVersion,NULL); // This does all the initialization stuff!!
	if(lerr != 0)
	{
	

		MessageBox("Calibrator Information not found!","Ouch!!!",MB_ICONINFORMATION|MB_OK);

		


	}

	sipSetIntegrationTime(gSetUpProfile.whichIntegrationTime);

	gMeasureLock = FALSE;
	gDisplayCT = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogMain::OnButtonMeasure() 
{
	// TODO: Add your control notification handler code here
	MSG message;
	while(gMeasureLock)
	{
		 sipMeasureYxy(FOOT_LAMBERTS,&myMeasurement);
		//myMeasurement = (dYxy *)sipCalMeasureRGB();
		DisplayMeasurement(&myMeasurement,gPtrLEDdisplay,&gSetUpProfile);
		if(WriteToFile == TRUE)
		{

		  fprintf(gFilePtr,"%8.4f,%6.4f,%6.4f\n",myMeasurement.Y,myMeasurement.x,myMeasurement.y);

		}

		
		if(PeekMessage(&message,GetDlgItem(IDC_RADIO1)->m_hWnd,BN_CLICKED,BN_CLICKED,PM_NOREMOVE))
		{	
			if(message.message == BN_CLICKED)
			{
				gMeasureLock == FALSE;
				CheckDlgButton(IDC_RADIO1,0); //uncheck it;
				break; 
			}
			
		}


	}

	  sipMeasureYxy(FOOT_LAMBERTS,&myMeasurement);
		DisplayMeasurement(&myMeasurement,gPtrLEDdisplay,&gSetUpProfile);
	if(WriteToFile == TRUE)
		{

		  fprintf(gFilePtr,"%8.4f,%6.4f,%6.4f\n",myMeasurement.Y,myMeasurement.x,myMeasurement.y);

		}

	
}

void CDialogMain::DisplayMeasurement(dYxy * meas, LEDdisplay *LEDptr, SetUpProfile *profile)
{
	char tempString[256];	 
	double tempMat[3],outMat[3], ColorTemp;
	if(profile->whichUnits == MMETRIC){
		if(meas != NULL){
			meas->Y /= 3.14159;
			meas->Y *= 10.764;
		}
	}


	switch(profile->whichDisplay)
	 {

	 case MXYL:

	    strcpy (ptrStrTitles[0] , " Measure Yxy");
		 strcpy (ptrStrTitles[1] , "  Y =");
		 strcpy (ptrStrTitles[2] , "  x =");
		 strcpy (ptrStrTitles[3] , "  y =");
		 strcpy (ptrStrTitles[4] , " ");
		 if(meas != NULL)
		 {
			 sprintf(tempString,"%6.2f",meas->Y);
			 strcat(ptrStrTitles[1],tempString);

			 sprintf(tempString,"%6.4f",meas->x);
			 strcat(ptrStrTitles[2],tempString);

			 sprintf(tempString,"%6.4f",meas->y);
			 strcat(ptrStrTitles[3],tempString);
		 }
		 if(gDisplayCT)
		 {
			  Pxy2Kelvin(meas->x,meas->y,&ColorTemp);
			  sprintf(tempString,"%6.0f",ColorTemp);
			  strcat(tempString," K");
			 	strcat(ptrStrTitles[4],tempString);

		 }
		 
		 break;

	case MUVL:

	    
		 strcpy (ptrStrTitles[1] , "  L =");
		 strcpy (ptrStrTitles[2] , "  u' =");
		 strcpy (ptrStrTitles[3] , "  v' =");
		 strcpy (ptrStrTitles[4] , " ");
		 if(meas != NULL)
		 {
			 sprintf(tempString,"%6.2f",meas->Y);
			 strcat(ptrStrTitles[1],tempString);

			 sprintf(tempString,"%6.4f",(4*meas->x)/(-2*meas->x + 12* meas->y + 3));
			 strcat(ptrStrTitles[2],tempString);

			 sprintf(tempString,"%6.4f",(9*meas->y)/(-2*meas->x + 12* meas->y + 3));
			 strcat(ptrStrTitles[3],tempString);
		 }

		 
		 break;


	case MRGB:
		if(meas !=0)
		{
			tempMat[0] = meas->x * meas->Y/meas->y;
			tempMat[1] = meas->Y;
			tempMat[2] = meas->z * meas->Y/meas->y;
		}
		else
		{
		    tempMat[0] = 0;
			tempMat[1] = 0;
			tempMat[2] = 0;

		}
		MatMult(MonitorData.XYZ2RGB,3,3,tempMat,3,1,outMat);
	  break;
	}

	
		

		if(gSetUpProfile.whichDisplay == MRGB)
			DrawRGB_BarGraph(myHMeterWindow, gPtrLEDdisplay, outMat[0]/MonitorData.Rref,outMat[1]/MonitorData.Gref,outMat[2]/MonitorData.Bref,BELinear);
		else
		RefreshLedDisplay(myHMeterWindow, gPtrLEDdisplay);
		

		

}

void CDialogMain::OnOK() 
{
	// TODO: Add extra validation here
	if(WriteToFile) fclose(gFilePtr);
	CDialog::OnOK();
}

void CDialogMain::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	if(gMeasureLock)
	{
		
			gMeasureLock = FALSE;
				CheckDlgButton(IDC_RADIO1,0); //uncheck it;

	}

	else
	{

		CheckDlgButton(IDC_RADIO1,1); // check it and...
		gMeasureLock = TRUE;
	}

}

void CDialogMain::OnDisplayct() 
{
	// TODO: Add your control notification handler code here
	if(gDisplayCT)
	{
		 gDisplayCT = FALSE;
		 CheckDlgButton(IDC_DISPLAYCT,0); //uncheck it;
	}
		else
	{

		CheckDlgButton(IDC_DISPLAYCT,1); // check it and...
		gDisplayCT = TRUE;
	}

}

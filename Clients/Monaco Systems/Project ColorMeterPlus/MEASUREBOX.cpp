// MEASUREBOX.cpp : implementation file
//

#include "stdafx.h"
#include "Color Meter Plus.h"
#include "MEASUREBOX.h"
#include "calcMatrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ReferenceFile MonitorData;
static dYxy theData;

/////////////////////////////////////////////////////////////////////////////
// CMEASUREBOX dialog


CMEASUREBOX::CMEASUREBOX(CWnd* pParent /*=NULL*/)
	: CDialog(CMEASUREBOX::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEASUREBOX)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMEASUREBOX::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEASUREBOX)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMEASUREBOX, CDialog)
	//{{AFX_MSG_MAP(CMEASUREBOX)
	ON_BN_CLICKED(IDC_BEGIN, OnBegin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMEASUREBOX message handlers

void CMEASUREBOX::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();

	


}

BOOL CMEASUREBOX::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMEASUREBOX::OnBegin() 
{
	// TODO: Add your control notification handler code here
		CWnd* myMeasureWindow = GetDlgItem(IDC_MEASURE_BOX); 
		RECT MeasurementRect;
		myMeasureWindow->GetWindowRect(&MeasurementRect);
		ScreenToClient(&MeasurementRect);
 		HBRUSH hBrush;
		HPEN hPen;
		CDC * mainHDC ;
		CDC* pMemDC = new CDC;	 
		short err;

		mainHDC = myMeasureWindow->GetDC();
  		

 		theImage = new CBitmap;
		
		err = theImage->LoadBitmap(IDB_BITMAP1);

		err = pMemDC->CreateCompatibleDC(mainHDC); 

		pMemDC->SelectObject((theImage));

	  hBrush = CreateSolidBrush (RGB(255,0,0));
	  hBrush = (HBRUSH)mainHDC->SelectObject(hBrush);
      
	  hPen =CreatePen(PS_SOLID,1,RGB(0,0,0));
	  hPen = (HPEN)mainHDC->SelectObject(hPen);
	  mainHDC->Rectangle(&MeasurementRect);
    
	 
	  DeleteObject(mainHDC->SelectObject(hBrush)); 
	  DeleteObject(mainHDC->SelectObject(hPen));
 
	  sipMeasureYxy(FOOT_LAMBERTS,&theData);
	  MonitorData.Red.Y = theData.Y;
	  MonitorData.Red.x = theData.x;
	  MonitorData.Red.y = theData.y;
	  MonitorData.Red.z = theData.z;
	  //green
	  hBrush = CreateSolidBrush (RGB(0,255,0));
	  hBrush = (HBRUSH)mainHDC->SelectObject(hBrush);
      
	  hPen =CreatePen(PS_SOLID,1,RGB(0,0,0));
	  hPen = (HPEN)mainHDC->SelectObject(hPen);
	  mainHDC->Rectangle(&MeasurementRect);
    
	 
	  DeleteObject(mainHDC->SelectObject(hBrush)); 
	  DeleteObject(mainHDC->SelectObject(hPen));
 
	  sipMeasureYxy(FOOT_LAMBERTS,&theData);
	  MonitorData.Green.Y = theData.Y;
	  MonitorData.Green.x = theData.x;
	  MonitorData.Green.y = theData.y;
	  MonitorData.Green.z = theData.z;
	  //Blue
	  hBrush = CreateSolidBrush (RGB(0,0,255));
	  hBrush = (HBRUSH)mainHDC->SelectObject(hBrush);
      
	  hPen =CreatePen(PS_SOLID,1,RGB(0,0,0));
	  hPen = (HPEN)mainHDC->SelectObject(hPen);
	  mainHDC->Rectangle(&MeasurementRect);
    
	 
	  DeleteObject(mainHDC->SelectObject(hBrush)); 
	  DeleteObject(mainHDC->SelectObject(hPen));
 
	  sipMeasureYxy(FOOT_LAMBERTS,&theData);
	  MonitorData.Blue.Y = theData.Y;
	  MonitorData.Blue.x = theData.x;
	  MonitorData.Blue.y = theData.y;
	  MonitorData.Blue.z = theData.z;

	  
	    //Blue
	  hBrush = CreateSolidBrush (RGB(255,255,255));
	  hBrush = (HBRUSH)mainHDC->SelectObject(hBrush);
      
	  hPen =CreatePen(PS_SOLID,1,RGB(0,0,0));
	  hPen = (HPEN)mainHDC->SelectObject(hPen);
	  mainHDC->Rectangle(&MeasurementRect);
    
	 
	  DeleteObject(mainHDC->SelectObject(hBrush)); 
	  DeleteObject(mainHDC->SelectObject(hPen));
 
	  sipMeasureYxy(FOOT_LAMBERTS,&theData);
	  MonitorData.White.Y = theData.Y;
	  MonitorData.White.x = theData.x;
	  MonitorData.White.y = theData.y;
	  MonitorData.White.z = theData.z;


	hBrush = CreateSolidBrush (RGB(192,192,192));
	  hBrush = (HBRUSH)mainHDC->SelectObject(hBrush);
      
	  hPen =CreatePen(PS_SOLID,1,RGB(192,192,192));
	  hPen = (HPEN)mainHDC->SelectObject(hPen);
	  mainHDC->Rectangle(&MeasurementRect);
    
	 
	  DeleteObject(mainHDC->SelectObject(hBrush)); 
	  DeleteObject(mainHDC->SelectObject(hPen));


	mainHDC->BitBlt(0,0,
		MeasurementRect.right-MeasurementRect.left,
		MeasurementRect.bottom-MeasurementRect.top,
		pMemDC,0,0,SRCCOPY);
	
	delete pMemDC;
			
	ReleaseDC(mainHDC);

	calcMatrix(&MonitorData);
	calcScalars(&MonitorData);

	GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
;

	
}

void CMEASUREBOX::OnCancel() 
{
	MonitorData.CancelOp = TRUE;
	
	CDialog::OnCancel();
}

// Color Meter Plus.h : main header file for the COLOR METER PLUS application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols



#include "sipCal.h"

typedef enum Mspeed 
{
	MFAST = 0,
	MMED,
	MSLOW
}Mspeed;

typedef enum Mdisplay 
{
	MXYL = 0,
	MUVL,
	MRGB
}Mdisplay;

typedef enum Munits
{

	MENGLISH = 0,
	MMETRIC
}Munits;

typedef struct SetUpProfile
{
	Mspeed whichSpeed;
	Mdisplay whichDisplay;
	Munits whichUnits;
	double whichFrequency;
	double whichIntegrationTime;
	char  whichFileName[1024]; // name of last saved reference value if it exists ;

}SetUpProfile; 

typedef struct ReferenceFile
{

   dYxy Red;
   dYxy Green;
   dYxy Blue;
   dYxy White;
   double XYZ2RGB[9];
   double ReferenceCT;
   double Rref;
   double Gref;
   double Bref;
   BOOL CancelOp;

}ReferenceFile;



/////////////////////////////////////////////////////////////////////////////
// CColorMeterPlusApp:
// See Color Meter Plus.cpp for the implementation of this class
//

class CColorMeterPlusApp : public CWinApp
{
public:
	CColorMeterPlusApp();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorMeterPlusApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CColorMeterPlusApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

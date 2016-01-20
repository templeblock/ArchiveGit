//////////////////////////////////////////////////////////////////////////////
//  datatab.cpp																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "datatab.h"
#include "think_bugs.h"
#include "errordial.h"

#define ST_DITL_ID	6003

#define Open_Source		2
#define Export_Source	4
#define Measure_Source	3
#define Display_Source	6
#define Gamut_Source	5
#define Text_Source		1

#define Open_Dest		8
#define Export_Dest		10
#define Measure_Dest	7
#define Display_Dest	12
#define Gamut_Dest		9
#define Text_Dest		11

#define SourceText		14
#define SourceCheck		18

#define DestText		13
#define DestCheck		19

#define Divider			17

#define DividerPict		156
#define SourceTextPict	158
#define DestTextPict	157
#define CheckOnPict		159
#define CheckOffPict	160

DataTab::DataTab(ProfileDoc *dc,WindowPtr d):WindowTab(d,ST_DITL_ID)
{

	doc = dc;

// set up the slider

	updateWindowData(0);
// set up the parameters

	CopyParameters();
	
}


DataTab::~DataTab(void)
{
}

McoStatus DataTab::CopyParameters(void)
{
return MCO_SUCCESS;
}


McoStatus DataTab::updateWindowData(int changed)
{
Rect r1;
short	iType;
Handle	iHandle;
Str255	text;
char	desc[100];
int 	result;

if (!doc) return MCO_OBJECT_NOT_INITIALIZED;

if (doc->simupatchD->type != No_Patches)
	{
	Enable(dp,Export_Source+startNum);
	Enable(dp,Display_Source+startNum);
	Enable(dp,Gamut_Source+startNum);
	}
else
	{
	Disable(dp,Export_Source+startNum);
	Disable(dp,Display_Source+startNum);
	Disable(dp,Gamut_Source+startNum);
	}
if (doc->patchD->type != No_Patches)
	{
	Enable(dp,Export_Dest+startNum);
	Enable(dp,Display_Dest+startNum);
	Enable(dp,Gamut_Dest+startNum);
	}
else
	{
	Disable(dp,Export_Dest+startNum);
	Disable(dp,Display_Dest+startNum);
	Disable(dp,Gamut_Dest+startNum);
	}
	
GetDItem (dp, Text_Source+startNum, &iType, (Handle*)&iHandle, &r1);
if (!iHandle) 
	return MCO_FAILURE;
GetIText(iHandle,text);
ptocstr(text);
doc->simupatchD->GetDesc(desc);
if (strcmp((char*)text,desc))
	{
	ctopstr(desc);
	SetIText(iHandle,(unsigned char*)desc);
	}

GetDItem (dp, Text_Dest+startNum, &iType, (Handle*)&iHandle, &r1);
if (!iHandle) 
	return MCO_FAILURE;
GetIText(iHandle,text);
ptocstr(text);
doc->patchD->GetDesc(desc);
if (strcmp((char*)text,desc))
	{
	ctopstr(desc);
	SetIText(iHandle,(unsigned char*)desc);
	}

return MCO_SUCCESS;
}

McoStatus DataTab::UpdateWindow(void)	
{	

PicHandle	pict;
Rect 	r1;
short	iType;
Handle	iHandle;

GetDItem (dp, Divider+startNum, &iType, (Handle*)&iHandle, &r1);


pict = GetPicture(DividerPict);
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}	
	
GetDItem (dp, SourceText+startNum, &iType, (Handle*)&iHandle, &r1);	
	
pict = GetPicture(SourceTextPict);
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}	

GetDItem (dp, DestText+startNum, &iType, (Handle*)&iHandle, &r1);
	
pict = GetPicture(DestTextPict);
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}		

GetDItem (dp, SourceCheck+startNum, &iType, (Handle*)&iHandle, &r1);			
if (doc->simupatchD->type != No_Patches)
	pict = GetPicture(CheckOnPict);
else
	pict = GetPicture(CheckOffPict);
	
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}

GetDItem (dp, DestCheck+startNum, &iType, (Handle*)&iHandle, &r1);			
if (doc->patchD->type != No_Patches)
	pict = GetPicture(CheckOnPict);
else
	pict = GetPicture(CheckOffPict);
	
if (pict != 0L) 
	{
	DrawPicture(pict,&r1);
	ReleaseResource((Handle)pict);
	}
return MCO_SUCCESS;
}


// The event handler for the main menu
McoStatus	DataTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
McoStatus state = MCO_SUCCESS;
Str255	text;
Rect r1;
short	iType;
Handle	iHandle;

short item2 = item - startNum;
	// no codes passed back
	*numwc = 0;
	
	if (item2 == Open_Source)
		{
		if (doc == NULL) return MCO_FAILURE;
		state = doc->_handle_read_sim_datafile();
		if (state) McoErrorAlert(state);
		if (!changed) changed = 1;
		}
	else if (item2 == Export_Source)
		{
		if (doc == NULL) return MCO_FAILURE;
		state = doc->handle_save_datafile(doc->simupatchD);		
		}
	else if (item2 == Measure_Source)
		{
		*numwc = 1;
		wc[0].code = WE_Doc_Read;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item2 == Display_Source)
		{		
		*numwc = 1;
		wc[0].code = WE_Open_Window;
		wc[0].wintype = DisplayWindow;
		wc[0].winnum = 1;
		wc[0].doc = doc;
		*data = doc->simupatchD;
		}
	else if (item2 == Gamut_Source)
		{		
		*numwc = 1;
		wc[0].code = WE_PrSource_Gammut;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item2 == Open_Dest)
		{
		if (doc == NULL) return MCO_FAILURE;
		state = doc->_handle_read_datafile();
		if (state) McoErrorAlert(state);
		if (!changed) changed = 1;
		}
	else if (item2 == Export_Dest)
		{
		if (doc == NULL) return MCO_FAILURE;
		state = doc->handle_save_datafile(doc->patchD);		
		}
	else if (item2 == Measure_Dest)
		{
		*numwc = 1;
		wc[0].code = WE_Doc_Read;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item2 == Display_Dest)
		{		
		*numwc = 1;
		wc[0].code = WE_Open_Window;
		wc[0].wintype = DisplayWindow;
		wc[0].winnum = 2;
		wc[0].doc = doc;
		*data = doc->patchD;
		}
	else if (item2 == Gamut_Dest)
		{		
		*numwc = 1;
		wc[0].code = WE_PrDoc_Gammut;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else
		{
		GetDItem (dp, Text_Source+startNum, &iType, (Handle*)&iHandle, &r1);
		GetIText(iHandle,text);
		if (text[0] > 100) text[0] = 100;
		ptocstr(text);
		doc->simupatchD->SetDesc((char*)text);

		GetDItem (dp, Text_Dest+startNum, &iType, (Handle*)&iHandle, &r1);
		GetIText(iHandle,text);
		if (text[0] > 100) text[0] = 100;
		ptocstr(text);
		doc->patchD->SetDesc((char*)text);
		}	
	return MCO_SUCCESS;
}
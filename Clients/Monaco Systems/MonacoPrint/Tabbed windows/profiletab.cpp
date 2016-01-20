//////////////////////////////////////////////////////////////////////////////
// profiletab.cpp																//
//	The tab that supports the source patches								//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "profiletab.h"

#define ST_DITL_ID			6004
#define Sat_slider			1
#define ColBal_silder		2
#define L_Linear_slider		3
#define Smooth_slider		23
#define Sat_Text			13
#define Col_Bal_Text		14
#define L_Linear_Text		15
#define Smooth_Text			19
#define ProcessMode			25

#define K_Only_Button_on	17
#define	K_Only_Button_off	18

/* The PICT ids for the medium slider */
/* disable back, enable unpress back, enable press, disable slide, enable unpress slide, enable press slide */

#define LARGE_PICT_IDS {18013,18012,18012,-32000,18009,18009}

/* The offsets etc. used by the medium sliders */
/* horz offset, horz inset, vert offset, horz text, vert text, horz text, vert text, horz text, vert text, Rect left, rect right, text id */

#define LARGE_OFFSETS {-1,20,0,-32000,-32000,-32000,-32000,-32000,-32000, \
						 1,2,12,13,   86,2,97,13, 1,1, -1}



ProfileTab::ProfileTab(ProfileDoc *dc,WindowPtr d,int twn):WindowTab(d,ST_DITL_ID)
{

	doc = dc;
	tabwinnum = twn;
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	int		ch;
	
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;


// set up the slider
	refCon1 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = startNum+Sat_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+Sat_slider,refCon1);	
	
	refCon2 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = startNum+Col_Bal_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+ColBal_silder,refCon2);
	
	refCon3 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = startNum+L_Linear_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+L_Linear_slider,refCon3);
	
	refCon4 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = startNum+Smooth_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+Smooth_slider,refCon4);

// set up the parameters

	GetDItem(dp,ProcessMode+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle,1); 
	lmode = 1;
	
	params[0] = doc->_condata.saturation;
	params[1] = doc->_condata.col_balance;
	params[2] = doc->_condata.L_linear;
	params[3] = doc->_condata.smooth_sim;
	params[4] = doc->_condata.K_only;
	
	CopyParameters();
	SetParameters(&ch);
	
}


ProfileTab::~ProfileTab(void)
{
if (refCon1) delete refCon1;
if (refCon2) delete refCon2;
if (refCon3) delete refCon3;
if (refCon4) delete refCon4;
}

McoStatus ProfileTab::CopyParameters(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,num;
	long 	temp;
	Str255	numstr,s;
	int mode;


	GetDItem(dp,Sat_slider+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.saturation); 
	GetDItem(dp,Sat_Text+startNum,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.saturation, s);
	SetIText (iHandle, s); 
	
	GetDItem(dp, ColBal_silder+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.col_balance); 
	GetDItem(dp,Col_Bal_Text+startNum,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.col_balance, s);
	SetIText (iHandle, s); 
	
	GetDItem(dp, L_Linear_slider+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.L_linear); 
	GetDItem(dp,L_Linear_Text+startNum,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.L_linear, s);
	SetIText (iHandle, s); 

	GetDItem(dp, Smooth_slider+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.smooth_sim); 
	GetDItem(dp,Smooth_Text+startNum,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.smooth_sim, s);
	SetIText (iHandle, s); 

	// text enhancement
	GetDItem(dp, K_Only_Button_on+startNum, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.K_only); 

	GetDItem(dp, K_Only_Button_off+startNum, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, 1-doc->_condata.K_only); 
	
	
	GetDItem(dp,ProcessMode+startNum,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle,lmode); 
	if (lmode == 1)
		{		
		Disable(dp,Sat_slider+startNum);
		Disable(dp,ColBal_silder+startNum);
		Disable(dp,L_Linear_slider+startNum);
		Disable(dp,Smooth_slider+startNum);
		Disable(dp,K_Only_Button_on+startNum);
		Disable(dp,K_Only_Button_off+startNum);	
		}
	else
		{
		Enable(dp,Sat_slider+startNum);
		Enable(dp,ColBal_silder+startNum);
		Enable(dp,L_Linear_slider+startNum);
		Enable(dp,Smooth_slider+startNum);
		Enable(dp,K_Only_Button_on+startNum);
		Enable(dp,K_Only_Button_off+startNum);
		}

return MCO_SUCCESS;
}


McoStatus ProfileTab::SetParameters(int *ch)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,num;
	long 	temp;
	Str255	numstr,s;
	int changed = 0;
	int		mode;
	
	GetDItem(dp,ProcessMode+startNum,&iType,&iHandle,&iRect);
	mode = GetCtlValue((ControlHandle)iHandle); 
	if (mode == 1)
		{
		if (lmode != mode)
			{
			params[0] = doc->_condata.saturation;
			params[1] = doc->_condata.col_balance;
			params[2] = doc->_condata.L_linear;
			params[3] = doc->_condata.smooth_sim;
			params[4] = doc->_condata.K_only;
			
			
			}
			
		Disable(dp,Sat_slider+startNum);
		Disable(dp,ColBal_silder+startNum);
		Disable(dp,L_Linear_slider+startNum);
		Disable(dp,Smooth_slider+startNum);
		Disable(dp,K_Only_Button_on+startNum);
		Disable(dp,K_Only_Button_off+startNum);	
			
		lmode = mode;
		temp = doc->_condata.saturation;
		doc->_condata.saturation = 80;
		if (temp != doc->_condata.saturation) changed = 1;
		
		temp = doc->_condata.col_balance;
		doc->_condata.col_balance = 100;
		if (temp != doc->_condata.col_balance) changed = 1;
		
		temp = doc->_condata.L_linear;
		doc->_condata.L_linear = 80;
		if (temp != doc->_condata.L_linear) changed = 1;
		
		temp = doc->_condata.smooth_sim;
		doc->_condata.smooth_sim = 0;
		if (temp != doc->_condata.smooth_sim) changed = 1;
		
		temp = doc->_condata.K_only;
		doc->_condata.K_only = 1;
		if (temp != doc->_condata.K_only) changed = 1;
		
		
		CopyParameters();
		
		}
	else
		{
		if (lmode != mode)
			{
			doc->_condata.saturation = params[0];
			doc->_condata.col_balance = params[1];
			doc->_condata.L_linear = params[2];
			doc->_condata.smooth_sim = params[3];
			doc->_condata.K_only = params[4];
			
			Enable(dp,Sat_slider+startNum);
			Enable(dp,ColBal_silder+startNum);
			Enable(dp,L_Linear_slider+startNum);
			Enable(dp,Smooth_slider+startNum);
			Enable(dp,K_Only_Button_on+startNum);
			Enable(dp,K_Only_Button_off+startNum);
			lmode = mode;
			CopyParameters();
			}
		else 
			{
		


			GetDItem(dp,Sat_slider+startNum,&iType,&iHandle,&iRect);
			temp = doc->_condata.saturation;
			doc->_condata.saturation = GetCtlValue((ControlHandle)iHandle); 
			if (temp != doc->_condata.saturation) changed = 1;
			
			GetDItem(dp, ColBal_silder+startNum,&iType,&iHandle,&iRect);
			temp = doc->_condata.col_balance;
			doc->_condata.col_balance = GetCtlValue((ControlHandle)iHandle ); 
			if (temp != doc->_condata.col_balance) changed = 1;
			
			GetDItem(dp, L_Linear_slider+startNum,&iType,&iHandle,&iRect);
			temp = doc->_condata.L_linear;
			doc->_condata.L_linear = GetCtlValue((ControlHandle)iHandle); 
			if (temp != doc->_condata.L_linear) changed = 1;


			GetDItem(dp, Smooth_slider+startNum,&iType,&iHandle,&iRect);
			temp = doc->_condata.smooth_sim;
			doc->_condata.smooth_sim = GetCtlValue((ControlHandle)iHandle); 
			if (temp != doc->_condata.smooth_sim) changed = 1;
			}
		lmode = mode;
		}

	if (changed) doc->changedProfile = 1;
	
	*ch = changed;

return MCO_SUCCESS;
}


McoStatus ProfileTab::SetUpDial(void)
{
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;


// set up the slider
	large_slid_off[OFFSET_TEXT_ID] = startNum+Sat_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+Sat_slider,refCon1);	
	
	large_slid_off[OFFSET_TEXT_ID] = startNum+Col_Bal_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+ColBal_silder,refCon2);
	
	large_slid_off[OFFSET_TEXT_ID] = startNum+L_Linear_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+L_Linear_slider,refCon3);
	
	large_slid_off[OFFSET_TEXT_ID] = startNum+Smooth_Text;
	setSlider(large_slid_picts,large_slid_off,startNum+Smooth_slider,refCon4);


	return MCO_SUCCESS;
}


McoStatus ProfileTab::updateWindowData(int changed)
{

if (!doc) return MCO_OBJECT_NOT_INITIALIZED;


return MCO_SUCCESS;
}

// The event handler for the main menu
McoStatus	ProfileTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,num;
	long 	temp;
	Str255	numstr,s;
	int		Changed = 0;
	
	short item2 = item - startNum;

	// no codes passed back
	*numwc = 0;
	
	SetParameters(&Changed);
	if ((item2 == K_Only_Button_on) || (item2 == K_Only_Button_off))
		{
		doc->_condata.K_only = 1-doc->_condata.K_only;
		Changed = 1;
		GetDItem(dp, K_Only_Button_on+startNum, &iType, &iHandle,&iRect);
		SetCtlValue ((ControlHandle)iHandle, doc->_condata.K_only); 

		GetDItem(dp, K_Only_Button_off+startNum, &iType, &iHandle,&iRect);
		SetCtlValue ((ControlHandle)iHandle, 1-doc->_condata.K_only); 
		}
	
	
	if (Changed) doc->changedProfile = 1;
	changed = Changed;
	return MCO_SUCCESS;
}
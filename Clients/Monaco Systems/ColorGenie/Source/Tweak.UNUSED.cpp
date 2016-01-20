#include "Tweak.h"
#include "mathutilities.h"
#include "fileformat.h"
//j #include "errordial.h"
//j #include "pascal_string.h"
//j #include "think_bugs.h"
#include "colortran.h"

#define Light_slider	1
#define Color_slider	2

#define LightViewSlider	21

#define View_Box		20

#define Desired_Box		19
#define Gamut_Box		22
#define Printed_Box		31

#define Desired_View_Box	32
#define Tweak_View_Box	33

#define Tweak_Menu		35
#define Tweak_Name		36
#define New_Tweak		37
#define Delete_Tweak	38

#define Gamut_Tweak_Radio		39
#define Printing_Tweak_Radio	40

#define Zoom_Button		41
#define Move_Button		42

#define L_text			44

#define Desired_Start	8
#define Gamut_Start		45
#define Gamut_Start_Ne		23
#define Print_Start		11
#define Print_Lab_St	14

#define Desired_View_Text_St 51
#define Tweak_View_Text_St 57
#define View_Text_Type	61

#define Print_Box_Label 4

#define Tweak_Menu_ID	9001

#define Assorted_Names 2007



/* The PICT ids for the medium slider */
/* disable back, enable unpress back, enable press, disable slide, enable unpress slide, enable press slide */

#define LARGE_PICT_IDS {18013,18012,18012,-32000,18009,18009}

/* The offsets etc. used by the medium sliders */
/* horz offset, horz inset, vert offset, horz text, vert text, horz text, vert text, horz text, vert text, Rect left, rect right, text id */

#define LARGE_OFFSETS {-1,20,0,-32000,-32000,-32000,-32000,-32000,-32000, \
						 1,2,12,13,   86,2,97,13, 1,1, -1}

#define QD_IDS	{-1,20,41,-1,42,-1}

#define NUM_TT_IDS	6

/////////////////////////////////////////////////////////////////////////////
TweakTab::TweakTab()
{
	int i;
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;
	Str255			theString,numstr;
	McoColorSync *mon,*lab,*list[2];
	short			tids[] = QD_IDS;
	McoStatus		state = MCO_SUCCESS;
	short itemType;
	Handle	h;
	
	idDITL = ST_DITL_ID;
	doc = (ProfileDocInf*)dc;
	refCon1 = 0L;
	refCon2 = 0L;
	lab_rgb_xform = 0L;
	doneGamut = 0;
	startGamut = 0;
	refCon1 = new RefConType;
	refCon2 = new RefConType;
	
	setSlider(large_slid_picts,large_slid_off,startNum+Light_slider,refCon1);	
	
	setSlider(large_slid_picts,large_slid_off,startNum+Color_slider,refCon2);

	current_tweak = 0L;

	if (doc->didGammutCompression) 
		doneGamut = 1;
	
	the_window = dp;
	
	for (i=0; i<NUM_TT_IDS; i++) ids[i] = tids[i];

// initialize comminications with the device
	state = doc->openInputDevice(0,1);
	if (state != MCO_SUCCESS) McoErrorAlert(state);
// calibrate the device
	if (doc->tcom) doc->tcom->Calibrate();
	
	_last_tick_time2 = 0;
	Current_Text = 0;

// set up the parameters

	
	if (doc->printerData->num_tweaks >0)
		{
		current_tweak = doc->printerData->tweaks[0];
		tweak_num = 0;
		}
	else 
		{
		current_tweak = 0L;
		tweak_num = -1;
		}
		
	//CopyParameters();
	
	// set up color conversion
	mon = new McoColorSync;
	mon->setType(Mco_ICC_System);

	lab = new McoColorSync;
	lab->GetProfile(LAB_PROFILE_NAME);
		
	lab_rgb_xform = new McoXformIcc;
	list[0] = lab;
	list[1] = mon;
	state = lab_rgb_xform->buildCW(list,2);	
	if (state != MCO_SUCCESS) { delete lab_rgb_xform; lab_rgb_xform = 0L;}
		
	delete mon;
	delete lab;
	
	// initialise our document structure
	
	GetDItem (dp, ids[QD3D_Box]+startNum, &itemType, &h, &box_rect);
	
	ambient = 1.0;
	point = 0;
	fill = 0;

	Document.fCameraFrom.x = 0.0; 
	Document.fCameraFrom.y = 0.0;
	Document.fCameraFrom.z = 30.0;

	Document.fCameraTo.x = 0.0; 
	Document.fCameraTo.y = 0.0;
	Document.fCameraTo.z = 0.0;


	SliderL = 50;
	// get the correct 
	if (doc->patchD->type == CMYKPatches)
		gammutSurfaceH = doc->_gammutSurfaceBlackH;
	else 
		gammutSurfaceH = doc->_gammutSurfaceH;
		
	InitDocumentData( &Document ) ;
		
			
	state = ChangeGeometry(MP_TweakAx,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_Circle,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_Arrow,0);
	if (state != MCO_SUCCESS) goto fail;
	
	state = ChangeGeometry(MP_GamutSlice,0);
	if (state != MCO_SUCCESS) goto fail;
	
	state = ChangeGeometry(MP_BlackBox,0);
	if (state != MCO_SUCCESS) goto fail;

	state = ChangeGeometry(MP_TweakMark1,0);
	if (state != MCO_SUCCESS) goto fail;


	AdjustCamera(	&Document,
					(box_rect.right - box_rect.left),
					(box_rect.bottom - box_rect.top) ) ;	

	AdjustCamera2(&Document);
	
	desiredRGB[0] = 0;
	desiredRGB[1] = 0;
	desiredRGB[2] = 0;
	
	printedRGB[0] = 0;
	printedRGB[1] = 0;
	printedRGB[2] = 0;
	
	gammutRGB[0] = 0;
	gammutRGB[1] = 0;
	gammutRGB[2] = 0;
	
	rgb_d[0] = 0;	
	rgb_d[1] = 0;
	rgb_d[2] = 0;

	rgb_p[0] = 0;	
	rgb_p[1] = 0;
	rgb_p[2] = 0;

	rgb_g[0] = 0;	
	rgb_g[1] = 0;
	rgb_g[2] = 0;
	
	Current_Button = -1;

	if (current_tweak) SliderL = current_tweak->lab_d[0];
	
	updateWindowData();
	_updatSliderL();
	_changeTweakData(1);
	_setUpTweakMenu();
	
	validWin = 1;
	return;

fail:
	validWin = 0;
	return;
}




/////////////////////////////////////////////////////////////////////////////
TweakTab::~TweakTab(void)
{
	if (lab_rgb_xform) delete lab_rgb_xform;
	if (doc != 0L) doc->closeInputDevice();
	if (refCon1) delete refCon1;
	if (refCon2) delete refCon2;
}


/////////////////////////////////////////////////////////////////////////////
void TweakTab::_changeToCalib(void)
{
	Str255	st;

	ShowDItem(dp,Printed_Box+startNum);
	ShowDItem(dp,Print_Box_Label+startNum);
	ShowDItem(dp,Print_Start+startNum);
	ShowDItem(dp,Print_Start+startNum+1);
	ShowDItem(dp,Print_Start+startNum+2);

	ShowDItem(dp,Gamut_Start_Ne+startNum);
	ShowDItem(dp,Gamut_Start_Ne+startNum+1);
	ShowDItem(dp,Gamut_Start_Ne+startNum+2);

	ShowDItem(dp,Print_Lab_St+startNum);
	ShowDItem(dp,Print_Lab_St+startNum+1);
	ShowDItem(dp,Print_Lab_St+startNum+2);

	HideDItem(dp,Gamut_Start+startNum);
	HideDItem(dp,Gamut_Start+startNum+1);
	HideDItem(dp,Gamut_Start+startNum+2);

	SetItemText(Tweak_View_Text_St+startNum,"\p   ");
	SetItemText(Tweak_View_Text_St+startNum+1,"\p   ");
	SetItemText(Tweak_View_Text_St+startNum+2,"\p   ");

	GetIndString(st,Assorted_Names,3);

	SetItemText(View_Text_Type+startNum,st);
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_changeToGamut(void)
{
	Str255	st;

	HideDItem(dp,Printed_Box+startNum);
	HideDItem(dp,Print_Box_Label+startNum);
	HideDItem(dp,Print_Start+startNum);
	HideDItem(dp,Print_Start+startNum+1);
	HideDItem(dp,Print_Start+startNum+2);

	HideDItem(dp,Gamut_Start_Ne+startNum);
	HideDItem(dp,Gamut_Start_Ne+startNum+1);
	HideDItem(dp,Gamut_Start_Ne+startNum+2);

	HideDItem(dp,Print_Lab_St+startNum);
	HideDItem(dp,Print_Lab_St+startNum+1);
	HideDItem(dp,Print_Lab_St+startNum+2);

	ShowDItem(dp,Gamut_Start+startNum);
	ShowDItem(dp,Gamut_Start+startNum+1);
	ShowDItem(dp,Gamut_Start+startNum+2);

	SetItemText(Tweak_View_Text_St+startNum,"\p   ");
	SetItemText(Tweak_View_Text_St+startNum+1,"\p   ");
	SetItemText(Tweak_View_Text_St+startNum+2,"\p   ");

	GetIndString(st,Assorted_Names,2);

	SetItemText(View_Text_Type+startNum,st);
}


/////////////////////////////////////////////////////////////////////////////
void TweakTab::_setUpTweak(int which)
{
	Rect r;
	ControlHandle h;
	short itemType;

	if (!current_tweak) return;

	ConvertToRGB(current_tweak->lab_d,desiredRGB);
	if ((current_tweak->type == Calibrate_Tweak) || (which == 0)) 
		{
		ConvertToGammutC(current_tweak->lab_d,current_tweak->lab_g);
		}
	if (which == 0)
		{
		current_tweak->lab_p[0] = current_tweak->lab_g[0];
		current_tweak->lab_p[1] = current_tweak->lab_g[1];
		current_tweak->lab_p[2] = current_tweak->lab_g[2];
		}
	ConvertToRGB(current_tweak->lab_g,gammutRGB);
	ConvertToRGB(current_tweak->lab_p,printedRGB);

	_copyTextToDial();

	GetDItem ( dp, Light_slider+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue (h,(int32)current_tweak->Lr);

	GetDItem ( dp, Color_slider+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue ( h,(int32)current_tweak->Cr);

	GetDItem ( dp, LightViewSlider+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue ( h,(int32)SliderL);

	SetItemTextToNum( L_text+startNum, (int32)SliderL );

			
	GetDItem ( dp, Gamut_Tweak_Radio+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue (h, current_tweak->type);
	GetDItem ( dp, Printing_Tweak_Radio+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue (h, 1-current_tweak->type);

	if (current_tweak->type == Calibrate_Tweak) _changeToCalib();
	if (current_tweak->type == GamutComp_Tweak) _changeToGamut();

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::_setUpTweaks(void)
{
	doc->tweak->Init_Table();
	doc->tweak->Modify_Table(doc->printerData->num_tweaks,doc->printerData->tweaks);
	// tweak the patches using the tweaking table	

	doc->gtweak->Init_Table();
	// the gamut compresion tweak
	doc->gtweak->Modify_Table(doc->printerData->num_tweaks,doc->printerData->tweaks);
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_changeTweakData(int which)
{
	if (!current_tweak) return;

	_setUpTweak(which);
	if (current_tweak->type == Calibrate_Tweak)
		{
		ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
		ModifyArrow(current_tweak->lab_g,current_tweak->lab_p);
		}
	else if (current_tweak->type == GamutComp_Tweak)
		{
		ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
		ModifyArrow(current_tweak->lab_d,current_tweak->lab_g);
		}
		
	ModifyCross(current_tweak->lab_d);

	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_updatSliderL(void)
{
	Rect r;
	ControlHandle h;
	short itemType;

	if (!current_tweak) return;

	GetDItem ( dp, LightViewSlider+startNum, &itemType, (Handle*)&h, &r );
	SetCtlValue ( h,(int32)SliderL);

	if (current_tweak->type == Calibrate_Tweak)
		ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
	else if (current_tweak->type == GamutComp_Tweak)
		ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
	ModifyGammutObject(gammutSurfaceH);
	SetItemTextToNum( L_text+startNum, (int32)SliderL );
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_setUpTweakMenu(void)
{
	int i;
	MenuHandle menuH;
	short menucount;
	char	str[255];
	

	menuH = GetMenu(Tweak_Menu_ID);
	menucount = CountMItems(menuH);

//delete anything above 1
	for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);	
			
	for (i=0; i<doc->printerData->num_tweaks; i++)
		{
		strcpy(str,doc->printerData->tweaks[i]->name);
		ctopstr(str);
		InsMenuItem(menuH, (unsigned char*)str, i+1);
		}
		
	DrawControls(dp);
}


/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::CopyParameters(void)
{
	if (!doneGamut) return MCO_SUCCESS;
	_setUpTweak(0);
	_setUpTweakMenu();
	return MCO_SUCCESS;
}


#define bigger(a, b) ((a >= b) ? a :b)
#define smaller(a, b) ((a < b) ? a :b)

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_checkMaxVals(double *lab)
{
	double m;

	if (lab[0] > 100) lab[0] = 100;
	if (lab[0] < 0) lab[0] = 0;

	if (lab[1] < -128) 
		{
		m = -128/lab[1];
		lab[1] = -128;
		lab[2] = lab[2]*m;
		}
		
	if (lab[1] > 127) 
		{
		m = 127/lab[1];
		lab[1] = 127;
		lab[2] = lab[2]*m;
		}	

	if (lab[2] < -128) 
		{
		m = -128/lab[2];
		lab[2] = -128;
		lab[1] = lab[1]*m;
		}
		
	if (lab[2] > 127) 
		{
		m = 127/lab[2];
		lab[2] = 127;
		lab[1] = lab[1]*m;
		}	
}


/////////////////////////////////////////////////////////////////////////////
void TweakTab::ConvertToRGB(double *lab,double *rgb)
{
	unsigned char temp[3];
	double	tlab[3];

	tlab[0] = lab[0];
	tlab[1] = lab[1];
	tlab[2] = lab[2];

	_checkMaxVals(tlab);

	temp[0] = 2.55*tlab[0];
	temp[1] = tlab[1]+128;
	temp[2] = tlab[2]+128;

		
	if (lab_rgb_xform != NULL) lab_rgb_xform->evalColors(temp,temp,1);
		
	rgb[0] = temp[0];
	rgb[1] = temp[1];
	rgb[2] = temp[2];
}


/////////////////////////////////////////////////////////////////////////////
void TweakTab::ConvertToGammutC(double *lab,double *labc)
{
	double	*labTable;

	labTable = (double*)McoLockHandle(doc->_labtableH);

	interpolate_33(lab,labc,labTable);

	McoUnlockHandle(doc->_labtableH);
}


/////////////////////////////////////////////////////////////////////////////
int TweakTab::isColorInGamut(double *lab)
{
	unsigned char		*out_gamut;
	double				lch[3];
	int					j,k,l;
	int					gamut;

	out_gamut = (unsigned char*)McoLockHandle(doc->_out_of_gamutH);

	if (lab[1] < -128) lab[1] = -128;
	if (lab[1] > 127) lab[1] = 127;

	if (lab[2] < -128) lab[2] = -128;
	if (lab[2] > 127) lab[2] = 127;

	j = 33*lab[0]/100;
	k = 33*(lab[1]+128)/256;
	l = 33*(lab[2]+128)/256;

	if (out_gamut[l+k*33+j*33*33]) gamut = 0;
	else gamut = 1;

	McoUnlockHandle(doc->_out_of_gamutH);
	return gamut;
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_converttoTwPrint(double *lab,double *labo)
{
	doc->tweak->eval(lab,labo,1);
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::_converttoTwGamut(double *lab,double *labo)
{
	ConvertToGammutC(lab,labo);
	doc->gtweak->eval(lab,labo,labo,1);
}


/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::SetUpDial(void)
{
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;

	if (!doneGamut) return MCO_SUCCESS;

	// set up the slider
	setSlider(large_slid_picts,large_slid_off,startNum+Light_slider,refCon1);	
	
	setSlider(large_slid_picts,large_slid_off,startNum+Color_slider,refCon2);
	

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
void TweakTab::Clear3DWin(void)
{
	RGBColor	c,old_fore;

	GetForeColor(&old_fore);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);
	PaintRect(&box_rect);
	RGBForeColor(&old_fore);
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::UpdateWindow(void)	
{			
	PicHandle	pict;
	Rect r1;
	short	iType;
	Handle	iHandle;
	McoStatus	state;

	if (doneGamut)
		{
		if (Current_Text == 1) 
			{
			unbox_item(dp,Desired_Box+startNum);
			if (current_tweak->type) 
				{
				box_item(dp,Gamut_Box+startNum);
				unbox_item(dp,Printed_Box+startNum);
				}
			else 
				{
				unbox_item(dp,Gamut_Box+startNum);
				box_item(dp,Printed_Box+startNum);
				}
			}
		else if (Current_Text == 0)
			{
			box_item(dp,Desired_Box+startNum);
			unbox_item(dp,Gamut_Box+startNum);
			unbox_item(dp,Printed_Box+startNum);
			}
		else
			{
			unbox_item(dp,Desired_Box+startNum);
			unbox_item(dp,Gamut_Box+startNum);
			unbox_item(dp,Printed_Box+startNum);
			}
		state = DrawColorBox(Desired_Box+startNum,desiredRGB);
		if (state != MCO_SUCCESS) return state;	
			
		if (current_tweak->type == Calibrate_Tweak)
			{
			state = DrawColorBox(Printed_Box+startNum,printedRGB);
			if (state != MCO_SUCCESS) return state;	
			}	

		state = DrawColorBox(Gamut_Box+startNum,gammutRGB);
		if (state != MCO_SUCCESS) return state;	

		state = DrawColorBox(Desired_View_Box+startNum,rgb_d);
		if (current_tweak)
			{
			if (current_tweak->type == Calibrate_Tweak)
				state = DrawColorBox(Tweak_View_Box+startNum,rgb_p);
			else if (current_tweak->type == GamutComp_Tweak)
				state = DrawColorBox(Tweak_View_Box+startNum,rgb_g);	
			}		
			
		if (current_tweak) QD3DWin::DrawWindow();
		else Clear3DWin();

		}
		
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::_copyTextFromDial(void)
{

	Boolean		changed = 0,changed2;
	Str255		inname;
	Rect r1;
	short	iType;
	Handle	iHandle;	
	
	if (!current_tweak) return MCO_SUCCESS;
	if (!doneGamut) return MCO_SUCCESS;

	changed = changed | GetItemTextFromNum(Desired_Start+startNum,&current_tweak->lab_d[0],0,100,0);
	changed = changed | GetItemTextFromNum(Desired_Start+startNum+1,&current_tweak->lab_d[1],-128,128,0);
	changed = changed | GetItemTextFromNum(Desired_Start+startNum+2,&current_tweak->lab_d[2],-128,128,0);
	changed2 = changed;

	changed = changed | GetItemTextFromNum(Gamut_Start+startNum,&current_tweak->lab_g[0],0,100,0);
	changed = changed | GetItemTextFromNum(Gamut_Start+startNum+1,&current_tweak->lab_g[1],-128,128,0);
	changed = changed | GetItemTextFromNum(Gamut_Start+startNum+2,&current_tweak->lab_g[2],-128,128,0);

	if (current_tweak->type == Calibrate_Tweak)
		{
		changed = changed | GetItemTextFromNum(Print_Start+startNum,&current_tweak->lab_p[0],0,100,0);
		changed = changed | GetItemTextFromNum(Print_Start+startNum+1,&current_tweak->lab_p[1],-128,128,0);
		changed = changed | GetItemTextFromNum(Print_Start+startNum+2,&current_tweak->lab_p[2],-128,128,0);
		}

	if (changed2)
		{
		if (isColorInGamut(current_tweak->lab_d)) current_tweak->type = Calibrate_Tweak;
		else current_tweak->type = GamutComp_Tweak;
		SliderL = current_tweak->lab_d[0];
		ModifyGammutObject(gammutSurfaceH);
		}
	else if ((changed) && (current_tweak->type == Calibrate_Tweak))
		{
		SliderL = current_tweak->lab_p[0];
		ModifyGammutObject(gammutSurfaceH);
		}
	else if (changed)
		{
		SliderL = current_tweak->lab_p[0];
		ModifyGammutObject(gammutSurfaceH);
		}

	if (changed) 
		{
		doc->changedProfile = 1;
		_changeTweakData(1-changed2);
		QD3DWin::DrawWindow();	

		}

		

	GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
	GetIText(iHandle,inname);
	if (inname[0] > 40) inname[0] = 40;
	ptocstr(inname);
	if (strcmp(current_tweak->name,(char*)inname)) 
		{
		strcpy(current_tweak->name,(char*)inname);
		_setUpTweakMenu();
		}
	

	return MCO_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::_copyTextToDial(void)
{

	if (!doneGamut) return MCO_SUCCESS;

	if (!current_tweak)
		{
		SetItemText(Desired_Start+startNum,"\p   ");
		SetItemText(Desired_Start+startNum+1,"\p   ");
		SetItemText(Desired_Start+startNum+2,"\p   ");

		SetItemText(Gamut_Start+startNum,"\p   ");
		SetItemText(Gamut_Start+startNum+1,"\p   ");
		SetItemText(Gamut_Start+startNum+2,"\p   ");

		SetItemText(Gamut_Start_Ne+startNum,"\p   ");
		SetItemText(Gamut_Start_Ne+startNum+1,"\p   ");
		SetItemText(Gamut_Start_Ne+startNum+2,"\p   ");
	
		SetItemText(Print_Start+startNum,"\p   ");
		SetItemText(Print_Start+startNum+1,"\p   ");
		SetItemText(Print_Start+startNum+2,"\p   ");

		}
	else
		{

		SetItemTextToNum(Desired_Start+startNum,current_tweak->lab_d[0],0);
		SetItemTextToNum(Desired_Start+startNum+1,current_tweak->lab_d[1],0);
		SetItemTextToNum(Desired_Start+startNum+2,current_tweak->lab_d[2],0);

		SetItemTextToNum(Gamut_Start+startNum,current_tweak->lab_g[0],0);
		SetItemTextToNum(Gamut_Start+startNum+1,current_tweak->lab_g[1],0);
		SetItemTextToNum(Gamut_Start+startNum+2,current_tweak->lab_g[2],0);
		
		SetItemTextToNum(Gamut_Start_Ne+startNum,current_tweak->lab_g[0],0);
		SetItemTextToNum(Gamut_Start_Ne+startNum+1,current_tweak->lab_g[1],0);
		SetItemTextToNum(Gamut_Start_Ne+startNum+2,current_tweak->lab_g[2],0);

		if (current_tweak->type == Calibrate_Tweak)
			{
			SetItemTextToNum(Print_Start+startNum,current_tweak->lab_p[0],0);
			SetItemTextToNum(Print_Start+startNum+1,current_tweak->lab_p[1],0);
			SetItemTextToNum(Print_Start+startNum+2,current_tweak->lab_p[2],0);
			}
		}

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::updateWindowData(void)
{
	Rect r1;
	short	iType;
	Handle	iHandle;
	ControlHandle ControlH;
	Str255	text;
	char	desc[100];


	if ((doc->didGammutCompression) && (!doneGamut))
		{
		doneGamut = 1;
		showWindow();
		}
	if (doneGamut)
		{
		if (!current_tweak)
			{
			if (doc->printerData->num_tweaks > 0)
				{ 
				current_tweak = doc->printerData->tweaks[0];
				tweak_num = 0;
				GetDItem ( dp, Tweak_Menu+startNum, &iType, (Handle*)&ControlH, &r );
				SetCtlValue ( ControlH,tweak_num);
				_setUpTweak(1);
				_setUpTweakMenu();
				}
			}
		else
			{
			if (doc->printerData->tweaks[tweak_num] != current_tweak)
				{
				if (doc->printerData->num_tweaks > 0)
					{ 
					current_tweak = doc->printerData->tweaks[0];
					tweak_num = 0;
					GetDItem ( dp, Tweak_Menu+startNum, &iType, (Handle*)&ControlH, &r );
					SetCtlValue ( ControlH,tweak_num);
					_setUpTweak(1);
					_setUpTweakMenu();
					}
				else 
					{
					current_tweak = 0L;
					tweak_num = -1;
					_setUpTweak(1);
					_setUpTweakMenu();
					}
				}
			}
		if (!current_tweak)
			{
			Disable(dp,Light_slider+startNum);
			Disable(dp,Color_slider+startNum);
			Disable(dp,Delete_Tweak+startNum);
			Disable(dp,Gamut_Tweak_Radio+startNum);
			Disable(dp,Printing_Tweak_Radio+startNum);
			GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
			SetIText(iHandle,"\p");
			}
		else
			{
			Enable(dp,Light_slider+startNum);
			Enable(dp,Color_slider+startNum);
			Enable(dp,Delete_Tweak+startNum);
			Enable(dp,Gamut_Tweak_Radio+startNum);
			Enable(dp,Printing_Tweak_Radio+startNum);
			GetDItem (dp, Tweak_Name+startNum, &iType, (Handle*)&iHandle, &r1);
			GetIText(iHandle,text);
			ptocstr(text);
			strcpy(desc,current_tweak->name);
			if (strcmp((char*)text,desc))
				{
				ctopstr(desc);
				SetIText(iHandle,(unsigned char*)desc);
				}
			}
			
		_copyTextToDial();
		}	
	return MCO_SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////
McoStatus 	TweakTab::NewTweak(void)
{
	Str255	theString,numstr;
	Rect r;
	Handle h;
	short itemType;
	ControlHandle	sliderControl;

	if (doc->printerData->num_tweaks >= MAX_NUM_TWEAKS) return MCO_FAILURE; 

	GetIndString(theString,Assorted_Names,1);

	NumToString(doc->printerData->num_tweaks+1, numstr);
	add_string(theString,numstr);
	ptocstr(theString);
	doc->printerData->tweaks[doc->printerData->num_tweaks] = new Tweak_Element((char*)theString);
	current_tweak = doc->printerData->tweaks[doc->printerData->num_tweaks];
	doc->printerData->num_tweaks++;
	
	_setUpTweakMenu();
	
	GetDItem ( dp, Tweak_Menu+startNum, &itemType, (Handle*)&sliderControl, &r );
	SetCtlValue ( sliderControl,doc->printerData->num_tweaks);
	tweak_num = doc->printerData->num_tweaks-1;
	
	_copyTextToDial();
	_updatSliderL();
	_changeTweakData(1);
	updateWindowData();
	
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
// delete the currently active tweak
McoStatus TweakTab::DeleteTweak(void)
{
	int i,j;
	Rect r;
	short itemType;
	ControlHandle	sliderControl;

	for (i=0; i<doc->printerData->num_tweaks; i++) if (doc->printerData->tweaks[i] == current_tweak)
		{
		delete current_tweak;
		for (j=i+1; j<doc->printerData->num_tweaks; j++)
			{
			doc->printerData->tweaks[j-1] = doc->printerData->tweaks[j];
			}
		doc->printerData->num_tweaks--;
	 	if (doc->printerData->num_tweaks > 0) 
	 		{
	 		current_tweak = doc->printerData->tweaks[0];
	 		tweak_num = 0;
	 		GetDItem ( dp, Tweak_Menu+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue ( sliderControl,tweak_num);
	 		}
	 	else  
	 		{
	 		current_tweak = 0L;
	 		tweak_num = -1;
	 		}
		break;
		}
	_changeTweakData(1);
	_setUpTweakMenu();
	
	updateWindowData();
	
	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::SaveTweakedPatches(void)
{
	McoStatus state = MCO_SUCCESS;
	RawData	  *tempPatchesD;

	tempPatchesD = new RawData;
	if (tempPatchesD == NULL) return MCO_MEM_ALLOC_ERROR;
	state = tempPatchesD->copyRawData(doc->patchD);
	if (state != MCO_SUCCESS) return state;


	state = doc->tweak->eval(doc->patchD,tempPatchesD,doc->patchD->format.total);
	if (state != MCO_SUCCESS) goto bail;
	state = doc->handle_save_datafile(tempPatchesD);

	bail:
	delete tempPatchesD;
	return state; 
}


/////////////////////////////////////////////////////////////////////////////
Boolean TweakTab::isMyObject(Point where,short *item) 
{
	WindowPtr oldP;
	short  titem;
	ControlHandle	sliderControl;

	GetPort(&oldP);
	SetPort(dp);
	GlobalToLocal(&where);
	titem = FindDItem(dp,where);
	titem ++;
	if (titem == LightViewSlider+startNum)
		{
		if (FindControl(where,dp,(ControlHandle*)&sliderControl) != inThumb)
			{
			*item = titem;
			SetPort(oldP);
			return TRUE;
			}
		}
	SetPort(oldP);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
Boolean TweakTab::TimerEventPresent(void)
{
	if( TickCount() - _last_tick_time2 > 4 ) 
		{
		return TRUE;
		}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Get the active region
McoStatus TweakTab::makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn)
{
	Rect r;
	Handle h;
	short itemType;

	GetDItem (dp,View_Box+startNum, &itemType, &h, &r);

	RectRgn(previewRgn,&r);
	XorRgn(mouseRgn,previewRgn,mouseRgn);

	return MCO_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
McoStatus TweakTab::doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype)
{
	McoStatus state = MCO_SUCCESS;
	unsigned char 	*previewOutBuf;
	unsigned char 	*previewInBuf;
	GrafPtr			oldGraf;
	Rect r1;
	Handle h;
	short itemType;
	double  labc[3];
	double	lab[3];
	double	labp[3];
	double  rgb[3];
	Point	tp;
	int		ingamut = 0;

	WindowPtr	oldP;
		
	GetPort(&oldP);
	SetPort( dp ) ;

	tp = where;

	GlobalToLocal(&tp);
	if ( (PtInRect(tp,&box_rect)) && (current_tweak))
		{
		if (current_tweak->type == GamutComp_Tweak)
			{
			get3Dinfo(where,lab,0,&ingamut);
			ingamut = 1-ingamut;
			}
		else if (current_tweak->type == Calibrate_Tweak)
			get3Dinfo(where,lab,1,&ingamut);

		if (ingamut)
			{
			ConvertToRGB(lab,rgb_d);
			_converttoTwGamut(lab,labc);
			ConvertToRGB(labc,rgb_g);
			ConvertToGammutC(lab,labp);
			_converttoTwPrint(labp,labp);
			ConvertToRGB(labp,rgb_p);

			state = DrawColorBox(Desired_View_Box+startNum,rgb_d);

			SetItemTextToNum(Desired_View_Text_St+startNum,lab[0],0);
			SetItemTextToNum(Desired_View_Text_St+startNum+1,lab[1],0);
			SetItemTextToNum(Desired_View_Text_St+startNum+2,lab[2],0);
			
			if (current_tweak->type == GamutComp_Tweak)
				{
				state = DrawColorBox(Tweak_View_Box+startNum,rgb_g);
				SetItemTextToNum(Tweak_View_Text_St+startNum,labc[0],0);
				SetItemTextToNum(Tweak_View_Text_St+startNum+1,labc[1],0);
				SetItemTextToNum(Tweak_View_Text_St+startNum+2,labc[2],0);
				}
			else if (current_tweak->type == Calibrate_Tweak)
				{
				state = DrawColorBox(Tweak_View_Box+startNum,rgb_p);
				SetItemTextToNum(Tweak_View_Text_St+startNum,labp[0],0);
				SetItemTextToNum(Tweak_View_Text_St+startNum+1,labp[1],0);
				SetItemTextToNum(Tweak_View_Text_St+startNum+2,labp[2],0);
				}
			}
		}
	SetPort(oldP);
	return state;
} 


/////////////////////////////////////////////////////////////////////////////
// The event handler for the main menu
McoStatus	TweakTab::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
	StandardFileReply		theReply;
	SFTypeList typeList;
	FileFormat *file;
	McoStatus state = MCO_SUCCESS;
	Rect r;
	Handle h;
	short itemType;
	ControlHandle	sliderControl;
	long num;
	int	Changed = 0;
	double temp;
	Point		mpoint;
	Window_Events 	code2;
	int		ingamut;

	short item2 = item - startNum;

	// no codes passed back
	*numwc = 0;
	
	
	if (item2 == Tweak_Menu)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		num = GetCtlValue ( sliderControl)-1;
		if (doc->printerData->tweaks[num]) current_tweak = doc->printerData->tweaks[num];
		tweak_num = num;
		_changeTweakData(1);
		updateWindowData();
		}
	else if (item2 == Desired_Box)
		{
		Enable(dp,Current_Button);
		Current_Text = 0;
		SliderL = current_tweak->lab_d[0];
		_updatSliderL();
		UpdateWindow();
		}
	else if (item2 == Gamut_Box)
		{
		if (current_tweak)
			if (current_tweak->type) 
				{
				Enable(dp,Current_Button);
				Current_Text = 1;
				SliderL = current_tweak->lab_g[0];
				_updatSliderL();
				UpdateWindow();
				}
		}
	else if (item2 == Printed_Box)
		{
		if (current_tweak)
			if (!current_tweak->type) 
				{
				Enable(dp,Current_Button);
				Current_Text = 1;
				SliderL = current_tweak->lab_p[0];
				_updatSliderL();
				UpdateWindow();
				}
		}
	else if (item2 == Gamut_Tweak_Radio)
		{
		if (current_tweak)
			{
			GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 1);
			GetDItem ( dp, Printing_Tweak_Radio+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 0);
			current_tweak->type = GamutComp_Tweak;
			_changeTweakData(1);
			Changed = TRUE;
			}
		}
	else if (item2 == Printing_Tweak_Radio)
		{
		if (current_tweak)
			{
			GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 1);
			GetDItem ( dp, Gamut_Tweak_Radio+startNum, &itemType, (Handle*)&sliderControl, &r );
			SetCtlValue (sliderControl, 0);
			current_tweak->type = Calibrate_Tweak;
			_changeTweakData(1);
			Changed = TRUE;
			}
		}		
	else if (item2 == Light_slider)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		temp = current_tweak->Lr;
		if (current_tweak) current_tweak->Lr = GetCtlValue ( sliderControl);
		if (temp != current_tweak->Lr) changed = 1;
		if (current_tweak->type == Calibrate_Tweak)
			ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
		else if (current_tweak->type == GamutComp_Tweak)
			ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
		QD3DWin::DrawWindow();
		Changed = TRUE;	
		}
	else if (item2 == Color_slider)
		{
		GetDItem ( dp, item, &itemType, (Handle*)&sliderControl, &r );
		temp = current_tweak->Cr;
		if (current_tweak) current_tweak->Cr = GetCtlValue ( sliderControl);
		if (temp != current_tweak->Cr) changed = 1;
		if (current_tweak->type == Calibrate_Tweak)
			ModifyCircle(current_tweak->lab_g,current_tweak->Lr,current_tweak->Cr,SliderL);
		else if (current_tweak->type == GamutComp_Tweak)
			ModifyCircle(current_tweak->lab_d,current_tweak->Lr,current_tweak->Cr,SliderL);
		QD3DWin::DrawWindow();
		Changed = TRUE;	
		}	
	else if (item2 == New_Tweak)
		{
		NewTweak();
		Changed = 1;
		}
	else if (item2 == Delete_Tweak)
		{
		DeleteTweak();
		Changed = 1;
		}
	else if (item2 == LightViewSlider)
		{
		GetMouse(&mpoint);
		do {
			SliderL = GetSlider(mpoint,item);
			_updatSliderL();
			QD3DWin::DrawWindow();
			GetMouse(&mpoint);
			} while ( StillDown ( ) );
		}
	if (item2 == Zoom_Button)
		{
		Enable(dp,Current_Button);
		Disable(dp,Zoom_Button+startNum);
		Current_Button = item;
		Current_Text = -1;
		UpdateWindow();
		}
	else if (item2 == Move_Button)
		{
		Enable(dp,Current_Button);
		Disable(dp,Move_Button+startNum);
		Current_Button = item;
		Current_Text = -1;
		UpdateWindow();
		}
	else if ((item2 == View_Box) && (current_tweak))
		{
		if (Current_Text == -1)
			{
			if (Current_Button == Move_Button+startNum)
				{
				code2 = WE_Translate;
				}
			else
				{
				code2 = WE_Scale;
				}
			ProcessMouseDown(clickPoint,code2);
			ModifyCross(current_tweak->lab_d);
			QD3DWin::DrawWindow();
			}
		else
			{
			if (Current_Text == 0)
				{
				state = get3Dinfo(clickPoint,current_tweak->lab_d,0,&ingamut);
				if (ingamut) current_tweak->type = Calibrate_Tweak;
				else current_tweak->type = GamutComp_Tweak;
				}
			else if (Current_Text == 1)
				{
				if (current_tweak->type == Calibrate_Tweak)
					state = get3Dinfo(clickPoint,current_tweak->lab_p,1,&ingamut);
				else 
					state = get3Dinfo(clickPoint,current_tweak->lab_g,1,&ingamut);
				}
			Changed = TRUE;
			_changeTweakData(Current_Text);
			UpdateWindow();
			}
		}
	else if (item == TIMER_ITEM ) 
		{   // a timer event has occured
		_last_tick_time2 = TickCount();
		if (doc->tcom) if (!doc->tcom->IsBufferEmpty())	
			{
			if (doc->tcom->IsDataPresent())
				{
				if (Current_Text == 0)
					{
					state = doc->tcom->getNextPatch(current_tweak->lab_d);
					SliderL = current_tweak->lab_d[0];
					if (isColorInGamut(current_tweak->lab_d)) current_tweak->type = Calibrate_Tweak;
					else current_tweak->type = GamutComp_Tweak;
					_updatSliderL();
					_changeTweakData(0);

					}
				else if (Current_Text == 1)
					{
					if (current_tweak->type == Calibrate_Tweak)
						{
						state = doc->tcom->getNextPatch(current_tweak->lab_p);
						SliderL = current_tweak->lab_p[0];
						}
					else
						{
						state = doc->tcom->getNextPatch(current_tweak->lab_g);
						SliderL = current_tweak->lab_g[0];
						}
					_updatSliderL();
					_changeTweakData(1);

					}
				Changed = TRUE;
				if (state != MCO_SUCCESS) McoErrorAlert(state);
				state = MCO_SUCCESS;
				}
			}
		}
	else state = _copyTextFromDial();
	
	if (Changed) 
		{
		doc->changedProfile = 1;
		_setUpTweaks();
		}
		
	return state;
}
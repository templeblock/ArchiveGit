////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1994 Monaco Systems Inc.
//
//	blackdialog.c
//
//
//	Create Date:	??? by Peter, modified by James
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Added by James
#include <QDOffscreen.h>
#include "think_bugs.h"
#include "basedialog.h"
#include "blackdialog.h"
// Addded by James
#include "Monaco Slider.h"
#include "pascal_string.h"
#include "fullname.h"
#include "monacoprint.h"
#include "errordial.h"

#define WIN_MARGIN_DIFF	(2)
#define WIN_LENGTH_DIFF (WIN_MARGIN_DIFF*2)
#define HAND_RANGE	(10.0/3.0)

#define TOTALINKLOW (200)
#define TOTALINKHIGH (400)
#define BLACKINKLOW (0)
#define BLACKINKHIGH (100)

// A nonzero maximum K value that is used for scalling
#define SMALL_K_MAX 0.00000000000001

//  Define Open_Warn_Dialog item IDs                                               

#define OK	1
#define Cancel 3

#define Black_Dialog		2002

//  Define Open_Warn_Dialog_Ditl item IDs
#define Ok_Box			2                                             
#define CurveWindow		4
#define BlackMenu		5
#define Total_ink_limit	9
#define Black_ink_limit	7
#define Table_type_menu	10
#define Simulate_type	14
#define Black_menu		2000
#define Simulate_menu 2003
#define Table_menu		2002
#define PrinterMenu		2005

#define Gamut_comp_menu	16

#define Gcr_radio		17
#define Ucr_radio		18	

#define Tweak			25
#define Options			20

#define PrinterTypeID				24
#define SaveSettings 			19
#define	LoadSettings			26
 
#define GridPict		135
 
#define Black_type_string (999)
#define Black_ink_string (1000)
#define Total_ink_string (1001)

#define Table_type_string (1002)
#define Gamut_type_string (1003)
#define Ink_type_string (1004)
#define Min_cyan_string (1005)
#define Max_cyan_string (1006)
#define Seperation_string (1019)

#define Black_type_ucr_menu_string	(2000)
#define Black_type_gcr_menu_string	(2001)
#define Ink_simulation_menu_string	(2002)

#define CMYK_TO_CMYK (5)

BlackDialog::BlackDialog(Document *dc)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i;
	long 	temp;
	
	priority = 1;
	
	doc = (ProfileDocInf*)dc;
	WinType = CalibDialog;
	WinNum = 0;
	disabled = 0;
	
	printers = new PrinterType*[MAX_PRINTERTYPES];
	for (i=0; i<MAX_PRINTERTYPES; i++) printers[i] = new PrinterType;

	
	setDialog(Black_Dialog);
	frame_button(Ok_Box);
	_blackH = doc->_blacktableH;
	

	_condataptr = &doc->_condata;	
	
//get size of the curve window
	GetDItem( dp, CurveWindow, &iType, &iHandle, &iRect);
	_left = iRect.left + WIN_MARGIN_DIFF;
	_bottom = iRect.bottom - WIN_MARGIN_DIFF; 
	_hor = iRect.right - iRect.left - WIN_LENGTH_DIFF; 
	_ver = iRect.top - iRect.bottom + WIN_LENGTH_DIFF;
	_clearrect = iRect;


//change the points in curve window
	for( i = 0; i <= 50; i++)
		_xc_points[i] = _left+2*i*_hor/100; 
		
// load in the printer types	

	
	current_printer = doc->defaults->printer_type;
	copyprinterType(printers[current_printer],doc->printerData);
	openPrinterTypes();
	setParameters();				



//init total and black ink limit



//added on 4/11 for ink simulation
	
	window_size = 0;
	
	// 3D the curve window
	
	threeD_box(CurveWindow);
	
	
}	

// added by James to remove slider stuff

BlackDialog::~BlackDialog(void)
{
int i;
if (printers) for (i=0; i<MAX_PRINTERTYPES; i++) if (printers[i]) 
	 delete printers[i];
if (printers) delete printers;
}



McoStatus BlackDialog::setParameters(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,m;
	long 	temp;
	Str255	numstr;
	
	
	doc->_condata = printers[current_printer]->data;
	
	_total_ink_limit = printers[current_printer]->data.total_ink_limit;

	
	if (doc->patchtype != RGBPatches)
		{
		GetDItem (dp, Total_ink_limit , &iType, &iHandle, &iRect);
		NumToString(_total_ink_limit, numstr);
		SetIText(iHandle, numstr);
		}
		
	_black_ink_limit = printers[current_printer]->data.black_ink_limit;


	if (doc->patchtype != RGBPatches)
		{
		GetDItem (dp, Black_ink_limit , &iType, &iHandle, &iRect);
		NumToString(_black_ink_limit, numstr);
		SetIText(iHandle, numstr);
		}
		
	_total_limit_ptr = &doc->_total_ink_limit;
	_black_limit_ptr = &doc->_black_ink_limit;
	
	
	_condataptr->separation_type = printers[current_printer]->data.separation_type;

	if (_condataptr->separation_type)
		{
		_Setradiobutton(Ucr_radio, Gcr_radio);
		_separation_type = Ucr_radio;
		Disable(dp,BlackMenu);
		}
	else
		{
		_Setradiobutton(Gcr_radio, Ucr_radio);
		_separation_type = Gcr_radio;
		Enable(dp,BlackMenu);
		}
	
	_black_type = printers[current_printer]->data.black_type;
	_num_hands = printers[current_printer]->num_hands;
	if (_num_hands > 0) 
		{
		for (i=0; i<_num_hands; i++)
			{
			_x_hands[i+1] = printers[current_printer]->x_hands[i];
			_y_hands[i+1] = printers[current_printer]->y_hands[i];
			}
		//_scale_spline_points((double)_black_ink_limit/100.0);
		_spline_points();
		DrawWindow();
		}
	else _init_spline_points();
	
// set the menu

	GetDItem(dp, BlackMenu, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle, (short)_black_type);
	
	//added on 1/8 for separation type
	


//init hand position
	_phand.h = _left;
	_phand.v = _bottom;	

	//Added on 9/28, what type of table, photoshop, ICC, or rgb to rgb
	//default is photoshop table

	MenuHandle menuH;
	short menucount;
	
	
	_table_type = printers[current_printer]->data.tabletype;
	doc->_condata.tabletype = _table_type;
	// if patches are rgb then set to rgb
	if (doc->patchtype != RGBPatches)
		{
		if (_table_type == 4)
			{
			_table_type = 2;
			doc->_condata.tabletype = _table_type;
			}
		GetDItem(dp, Table_type_menu, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle, (short)_table_type);
		_last_table_type = _table_type;
		menuH = GetMenu(Table_menu);
		DisableItem(menuH,4);

		}
	else 
		{
		temp = 4;
		GetDItem(dp, Table_type_menu, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle, (short)temp);
		_table_type = (short)temp;
		doc->_condata.tabletype = _table_type;
		_last_table_type = _table_type;
		menuH = GetMenu(Table_menu);
		for (i=1; i<=CountMItems(menuH);  i++) if (i != 4) DisableItem(menuH,i);
		Disable(dp,BlackMenu);
		Disable(dp,Gcr_radio);
		Disable(dp,Ucr_radio);
		HideDItem(dp,Total_ink_limit);
		HideDItem(dp,Black_ink_limit);
		}
		
// set the ink simulation menu
	openInkSimulation();	
	
	
	menuH = GetMenu(PrinterMenu);
	menucount = CountMItems(menuH);

	for( i = menucount; i > 2; i-- )
		DelMenuItem(menuH,i);
	
	m = 3;
	for (i=0; i<num_printers; i++)
		{
		InsMenuItem(menuH, printernames[i+3], m++);
		}
		
		
	GetDItem(dp, Simulate_type, &iType, &iHandle, &iRect);
	if (printers[current_printer]->sim_num > num_sims+1) printers[current_printer]->sim_num = 1;
	SetCtlValue((ControlHandle)iHandle, (short)printers[current_printer]->sim_num);

//	DrawControls(dp);
	
	return MCO_SUCCESS;	
}

// copy data into the settings
McoStatus BlackDialog::getParameters(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;	
	int i;	
	
	printers[current_printer]->data = doc->_condata;
	
	printers[current_printer]->data.total_ink_limit = _total_ink_limit;
	printers[current_printer]->data.black_ink_limit = _black_ink_limit;

	printers[current_printer]->data.black_type = _black_type;
	printers[current_printer]->num_hands = _num_hands;
	if (_num_hands > 0) 
		{
		for (i=0; i<_num_hands; i++)
			{
			 printers[current_printer]->x_hands[i] = _x_hands[i+1];
			 printers[current_printer]->y_hands[i] = _y_hands[i+1];
			}
		}
		
	printers[current_printer]->data.tabletype = _table_type;
	
	GetDItem(dp, Simulate_type, &iType, &iHandle, &iRect);
	printers[current_printer]->sim_num = GetCtlValue((ControlHandle)iHandle);
		
return MCO_SUCCESS;	
}

//open and get the all file names in the preference folder

McoStatus BlackDialog::openInkSimulation(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;

	int i;
	short num_files;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
	Str32	filename;
	CInfoPBRec	cipbr;
	DirInfo		*dpb = (DirInfo *)&cipbr;
	int16		idx = 1, filenum = 2;
	Str255		theString;	

	HFileInfo	myCPB;
	short	myVRef;	
	long	myDirID;
	char tempname[60];
	
	MenuHandle menuH;
	short menucount;
	
	menuH = GetMenu(Simulate_menu);
	menucount = CountMItems(menuH);

//delete anything above 1
	for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);	
	

 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);

//find dir 		
  	myCPB.ioFDirIndex = 0;
 	myCPB.ioDirID = myDirID;
 	myCPB.ioNamePtr = MONACO_FOLDER;
 	myCPB.ioVRefNum = myVRef;
 	
 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
 
 	sysErr = MCO_FILE_OPEN_ERROR;
 	if (myErr == noErr) {
 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
 	}

//find subdir
	if(sysErr == 0){
	  	myCPB.ioFDirIndex = 0;
	 	myCPB.ioNamePtr = MONACO_INK_SIM;
	 	myCPB.ioVRefNum = myVRef;
	 	
	 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
	 
	 	sysErr = MCO_FILE_OPEN_ERROR;
	 	if (myErr == noErr) {
	 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
	 	}
	}
	
	
	if(_table_type < CMYK_TO_CMYK){
		GetIndString(theString,Ink_simulation_menu_string,1);
		InsMenuItem(menuH,theString,0);		
	} 
	
	//add anything in the reference   	
	//make a copy of all the files that need to be processed  
 	//check whether folder is empty
 	if(sysErr == 0){
		while(1){
		    dpb->ioNamePtr = filename;
		    dpb->ioVRefNum = myVRef;
		    dpb->ioDrDirID = myCPB.ioDirID;
		    dpb->ioFDirIndex = idx++;
		   	sysErr = PBGetCatInfo(&cipbr, FALSE);
		    if( sysErr == fnfErr )
		    	break; //go out of the loop	
		    	
		    memcpy(_simunames[filenum-2], filename, 33);  
		    	    	
		    if(!(dpb->ioFlAttrib & 16)) 
		      	InsMenuItem(menuH, filename, filenum++);    
		}
		
		_simuFile.parID = myCPB.ioDirID;
		_simuFile.vRefNum = myVRef;
	}
	
// disable the link	
num_sims = filenum -2;
if (filenum == 2)
	{
	menuH = GetMenu(Table_menu);
		for (i=4; i<=CountMItems(menuH);  i++) DisableItem(menuH,i);
	if (doc->_condata.tabletype > 4)  doc->_condata.tabletype = 2;
	_table_type = doc->_condata.tabletype;
	GetDItem(dp, Table_type_menu, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle, (short)_table_type);
	_Settabletype();
	}
	
return MCO_SUCCESS;	
}


McoStatus BlackDialog::loadLastPrinter(void)
{
FileFormat filef;
FSSpec fspec;
McoStatus  state;
Str255  def = MONACO_DEFAULT_SETTINGS;
Str255  fold = MONACO_FOLDER;

ptocstr(def);
ptocstr(fold);

state = filef.findFilefromPref(&fspec,(char*)def,(char*)fold);
if (state != MCO_SUCCESS) return state;
return printers[1]->Load(&fspec);
}


McoStatus BlackDialog::saveLastPrinter(void)
{
FileFormat filef;
FSSpec fspec;
McoStatus  state;
Str255  def = MONACO_DEFAULT_SETTINGS;
Str255  fold = MONACO_FOLDER;

ptocstr(def);
ptocstr(fold);


state = filef.createFileinPref(&fspec,(char*)def,(char*)fold);
if (state != MCO_SUCCESS) return state;
return printers[1]->Save(&fspec);
}


McoStatus BlackDialog::saveprinterfile(void)
{

		
	
	return printers[1]->Save();
}	
	
McoStatus BlackDialog::loadprinterfile(void)
{
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	
	filelist[0] = 'MCO6';
	StandardGetFile(nil, 1, filelist, &sinReply);
	if(sinReply.sfGood == 0)
		return MCO_CANCEL;
		
		
	current_printer = 1;
	copyprinterType(printers[current_printer],doc->printerData);
	return printers[current_printer]->Load(&sinReply.sfFile);
}	



McoStatus BlackDialog::loadprinterfile(int32 filenum,long ioDirID,short myVRef,unsigned char *filename)
{

FSSpec fspec;

fspec.vRefNum = myVRef;
fspec.parID = ioDirID;
copy_str(fspec.name,filename);

if (filenum >= MAX_PRINTERTYPES) return MCO_FAILURE;

return printers[filenum]->Load(&fspec);	
		
}

 
void BlackDialog::openPrinterTypes(void)
{
	short num_files;
 	OSErr sysErr = noErr;
 	OSErr	myErr;
	Str32	filename;
	CInfoPBRec	cipbr;
	DirInfo		*dpb = (DirInfo *)&cipbr;
	int16		idx = 1;
	int			filenum;
	
	HFileInfo	myCPB;
	short	myVRef;	
	long	myDirID;
	char tempname[60];
	
	loadLastPrinter();

 	sysErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kDontCreateFolder, 
 			&myVRef, &myDirID);

//find dir 		
  	myCPB.ioFDirIndex = 0;
 	myCPB.ioDirID = myDirID;
 	myCPB.ioNamePtr = MONACO_FOLDER;
 	myCPB.ioVRefNum = myVRef;
 	
 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
 
 	sysErr = MCO_FILE_OPEN_ERROR;
 	if (myErr == noErr) {
 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
 	}


	filenum = 3;
//find subdir
	if(sysErr == 0){
	  	myCPB.ioFDirIndex = 0;
	 	myCPB.ioNamePtr = MONACO_PRINTERS;
	 	myCPB.ioVRefNum = myVRef;
	 	
	 	myErr = PBGetCatInfo((CInfoPBPtr)&myCPB, false);
	 
	 	sysErr = MCO_FILE_OPEN_ERROR;
	 	if (myErr == noErr) {
	 		if(((myCPB.ioFlAttrib >> 4) & 0x01) == 1) sysErr = 0;
	 	}
	}
	
	//add anything in the reference   	
	//make a copy of all the files that need to be processed  
 	//check whether folder is empty
 	if(sysErr == 0){
		while(1){
		    dpb->ioNamePtr = filename;
		    dpb->ioVRefNum = myVRef;
		    dpb->ioDrDirID = myCPB.ioDirID;
		    dpb->ioFDirIndex = idx++;
		   	sysErr = PBGetCatInfo(&cipbr, FALSE);
		    if( sysErr == fnfErr )
		    	break; //go out of the loop	
		    	
		    memcpy(printernames[filenum], filename, 33);
		    
		     if(!(dpb->ioFlAttrib & 16)) 
		     	loadprinterfile(filenum,myCPB.ioDirID,myVRef,filename);  
		    
		    filenum++;
		    if (filenum == MAX_PRINTERTYPES) break;
		    	    	
		}
		
	}
	
num_printers = filenum -3;
	
		
}




//create initial spline points, called by choose_black_type	
void BlackDialog::_init_spline_points(void)
{
	short i;

	
	//init depending on the black type
	if (_separation_type == Gcr_radio) switch(_black_type){
		case(1): //None	
			_num_hands = 2;
			_x_hands[1] = 0;
			_y_hands[1] = 0;
			_x_hands[2] = 100;
			_y_hands[2] = 0;
			break;
		
		case(2): //Light	
			_num_hands = 4;
			_x_hands[1] = 0;
			_y_hands[1] = 0;
			_x_hands[2] = 40;
			_y_hands[2] = 0;
			_x_hands[3] = 85;
			_y_hands[3] = 50;
			_x_hands[4] = 100;
			_y_hands[4] = 100;
			break;

		case(3): //Medium	
			_num_hands = 4;
			_x_hands[1] = 0;
			_y_hands[1] = 0;
			_x_hands[2] = 20;
			_y_hands[2] = 0;
			_x_hands[3] = 82;
			_y_hands[3] = 55;
			_x_hands[4] = 100;
			_y_hands[4] = 100;
		break;

		case(4): //Heavy	
			_num_hands = 4;
			_x_hands[1] = 0;
			_y_hands[1] = 0;
			_x_hands[2] = 10;
			_y_hands[2] = 0;
			_x_hands[3] = 60;
			_y_hands[3] = 40;
			_x_hands[4] = 100;
			_y_hands[4] = 100;
		break;

		case(5): //Maximum	
			_num_hands = 2;
			_x_hands[1] = 0;
			_y_hands[1] = 0;
			_x_hands[2] = 100;
			_y_hands[2] = 100;
		break;
	}
	else {  // ucr (same as Heavy)
		_num_hands = 4;
		_x_hands[1] = 0;
		_y_hands[1] = 0;
		_x_hands[2] = 40;
		_y_hands[2] = 0;
		_x_hands[3] = 85;
		_y_hands[3] = 50;
		_x_hands[4] = 100;
		_y_hands[4] = 100;
		}	
	
	_scale_spline_points((double)_black_ink_limit/100.0);
	_spline_points();
	DrawWindow();
}

//choose black type	(none, light, etc.)
void BlackDialog::_choose_black_type(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	black_type;

	GetDItem(dp, BlackMenu, &iType, &iHandle, &iRect);
	black_type = GetCtlValue((ControlHandle)iHandle);
	
	if( _black_type == black_type)
		return;
		
	_black_type = black_type;
	
//reinitialize the new black type
	_init_spline_points();
}



void BlackDialog::_scale_spline_points(double scale)
{
	long i;
	
	if (scale == 0) scale = SMALL_K_MAX;
	
	for(i = 1; i <= _num_hands; i++)
		_y_hands[i] *= scale;
}

// create the points 
void BlackDialog::_spline_points(void)
{
	short i;
	double yp1, ypn;

	yp1=1.0e30;
	//ypn = 10;
	ypn=(_y_hands[_num_hands]-_y_hands[_num_hands-1])/(_x_hands[_num_hands]-_x_hands[_num_hands-1]);
	spline(_x_hands,_y_hands,_num_hands,yp1,ypn,_y2_hands);

//change the lookup table	
	for( i = 0; i <= 100; i++)
		splint(_x_hands,_y_hands,_y2_hands,_num_hands,i,&_yd_points[i]);
		
//change the hand points in curve window
	for( i = 0; i < _num_hands; i++){
		_xc_hands[i] = _left + (short)(_x_hands[i+1]*_hor/100);
		_yc_hands[i] = _bottom + (short)(_y_hands[i+1]*_ver/100); 
	}	
	
//clipping the _yd_points
	for( i = 0; i <= 100; i++){
		if(_yd_points[i] < 0)
			_yd_points[i] = 0.0;
		else if(_yd_points[i] > 100)
			_yd_points[i] = 100.0;
	}

//fix on 8/23
//clipping the _yd_points which on the left of letfmost hand
//to the value of the leftmost hand
//on the right of rightmost hand to the value of the rightmost hand
	for( i = 0; i <= 100; i++){
		if( i <= _x_hands[1] ) //left of the leftmost hand
			_yd_points[i] = _y_hands[1];
		if( i >= _x_hands[_num_hands] ) //left of the leftmost hand
			_yd_points[i] = _y_hands[_num_hands];
	}

//change the points in curve window
	for( i = 0; i <= 50; i++)
		_yc_points[i] = _bottom + _yd_points[2*i]*_ver/100; 
		
	_black_ink_limit = _yd_points[100];
					
}


// Draw the curve 
// Modified by James to use gworlds
void BlackDialog::_draw_curvewin(void)
{
	int i;
	short x,y;
	RGBColor	tc;
	Rect theRect;
	Rect r;
	Handle h;
	short itemType;
	GrafPtr		oldGraf;
		
	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	GWorldPtr	BigGW;
	PicHandle	pict;
	RGBColor	c,white;
	RGBColor	oldback,oldfore;

	white.red = 65535;
	white.green = 65535;
	white.blue = 65535;
	
	c.red = 61000;
	c.green = 61000;
	c.blue = 61000;
	
	GetBackColor(&oldback);
	RGBBackColor(&white);
	
	PixMapHandle myPixMapHandle;

	GetPort(&oldGraf);
	SetPort(dp);
			
	GetDItem (dp,CurveWindow, &itemType, &h, &r);
	
/*	if (_separation_type != Gcr_radio)
		{
		GetDItem (dp,CurveWindow, &itemType, &h, &r);
		EraseRect(&r);
		SetPort(oldGraf);
		return;
		} */
	
	theRect.left = _left;
	theRect.right = _left + _hor;
	theRect.top = _bottom + _ver;
	theRect.bottom = _bottom;

	error = NewGWorld( &BigGW, 32, &r, 0, 0, 0 );
	if (error == 0)
		{
		GetGWorld(&oldGW,&oldGD);
		SetGWorld(BigGW,nil);
		myPixMapHandle = GetGWorldPixMap( BigGW );	/* 7.0 only */
		LockPixels(myPixMapHandle);

	//new stuff
		RGBBackColor(&c); 
		EraseRect(&_clearrect);
		
		pict = GetPicture(GridPict);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}
		
		if (doc->patchtype != RGBPatches)
			{
			PenNormal();
			MoveTo(_xc_points[0], _yc_points[0]);
			for (i = 1; i <= 50; i++)
				LineTo(_xc_points[i],_yc_points[i]);
		
		//draw hands
			if (_separation_type != Ucr_radio)
				for( i = 0; i < _num_hands; i++)
					{
					theRect.left = theRect.right = _xc_hands[i];
					theRect.top = theRect.bottom = _yc_hands[i];
					PenSize(2,2);
					InsetRect(&theRect, -2, -2);
					FrameRect(&theRect);
					}
			}
		SetGWorld(oldGW,oldGD);	
		CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &r, &r, srcCopy, 0 );
		UnlockPixels(myPixMapHandle);
		DisposeGWorld( BigGW );
	}
	
	RGBBackColor(&oldback);
	SetPort(oldGraf);
}


//convert local position to data in x, y coordinate
void BlackDialog::_from_local_to_data(short h, short v, double *x, double *y)
{
	*x = (h - _left)*100.0/_hor;
	*y = (v - _bottom)*100.0/_ver;
}


// The mouse is down and a handle is being draged
void BlackDialog::_hand_curvewin(void)
{
	long 	i;
	Point	where;
	double 	x, y;
	double 	ds, dl, dr;
	short 	bracket, diff, exist;
	short 	low, mid, high;
	short	direct, ahand;	
	

	if (_separation_type == Ucr_radio) return;

	GetMouse ( &where );
	_from_local_to_data(where.h, where.v, &x, &y);

	//is the current hand inside range of the previous hands
	low = 1;
	high = _num_hands;
	mid = (low+_num_hands)/2;
	
	while(1){
		if( _x_hands[mid] <= x){ //must be at the upper half
			if( high - mid <= 1){
				bracket = mid;
				break;
			}	
			else{
				low = mid;
				mid = (low + high)/2;
			}
		}
		else{ //must be at the lower half
			if( mid - low <= 1){
				bracket = low;
				break;
			}	
			else{
				high = mid;
				mid = (low + high)/2;
			}
		}				
	}
			
	//is the current hand closer to the left or right hand
	//if it is close enough, replace the previous hand with it
	//else create a new hand
	dl = x - _x_hands[bracket];
	dr = _x_hands[bracket+1] - x;
	direct = 0;			//assume closer to left initially
	ds = dl;
	if( dr < ds){
		ds = dr;
		direct = 1;
	}
	if( ds < HAND_RANGE){	//replace hand
		if(direct){ //closer to right
			_x_hands[bracket+1] = x;
			_y_hands[bracket+1] = y;
			ahand = bracket+1;
		}
		else{ //closer to left
			_x_hands[bracket] = x;
			_y_hands[bracket] = y;
			ahand = bracket;
		}
	}	
	else{	//create a new hand
		for( i = _num_hands; i > bracket; i--){
			_x_hands[i+1] = _x_hands[i];
			_y_hands[i+1] = _y_hands[i];
		}	
		_x_hands[bracket+1] = x;
		_y_hands[bracket+1] = y;
		ahand = bracket+1;
		_num_hands++;
	}

	_spline_points();
	_draw_curvewin();
	SetItemTextToNum(Black_ink_limit,_black_ink_limit,0);
	_phand.h = where.h;
	_phand.v = where.v;
	exist = 1;	//current hand exists
	

	while ( StillDown( )){	// Hang arounnd until the button is released
		GetMouse ( &where );
		
		//clipping the hand which is out of the curve window
		if( where.h < _left)
			where.h = _left;
		if( where.h > _left + _hor)
			where.h = _left + _hor;
		if( where.v > _bottom)
			where.v = _bottom;
		if( where.v < _bottom + _ver)
			where.v = _bottom + _ver;
						
		if( where.h != _phand.h || where.v != _phand.v){
			_phand.h = where.h;
			_phand.v = where.v;
		
			_from_local_to_data(where.h, where.v, &x, &y);

			if(exist){
				//bug fix on 8/23
				if( ahand == 1 ){//leftmost 
					//special case, don't delete hand
					//but this hand can't move too far 
					if( x-_x_hands[ahand + 1] > -HAND_RANGE)
						x = _x_hands[ahand + 1] - HAND_RANGE;
					_x_hands[ahand] = x;
					_y_hands[ahand] = y;
				}											
				else if( ahand == _num_hands ){ //rightmost hand
					if( x-_x_hands[ahand - 1] < HAND_RANGE)
						x = _x_hands[ahand - 1] + HAND_RANGE;
					_x_hands[ahand] = x;
					_y_hands[ahand] = y;
				}	
			
				else { //other hands
					dl = x - _x_hands[ahand-1];
					dr = _x_hands[ahand+1] - x;
					direct = 0;			//assume closer to left initially
					ds = dl;
					if( dr < ds){
						ds = dr;
						direct = 1;
					}
							
					if(dl < 0 || dr < 0){ //left of ahand-1 or right of ahand+1
					//delete current hand
						for( i = ahand; i < _num_hands; i++){
							_x_hands[i] = _x_hands[i+1];
							_y_hands[i] = _y_hands[i+1];
						}	
						_num_hands--;
						exist = 0; //not exist
					}
					else if( ds < HAND_RANGE){	//replace hand
						for( i = ahand; i < _num_hands; i++){
							_x_hands[i] = _x_hands[i+1];
							_y_hands[i] = _y_hands[i+1];
						}	
						_num_hands--;
						exist = 0; //not exist
					}	
					else{	//update current hand position
						_x_hands[ahand] = x;
						_y_hands[ahand] = y;
					}
				}		
			}
			else{ //the current hand is deleted already
				dl = x - _x_hands[ahand-1];
				dr = _x_hands[ahand] - x;
				direct = 0;			//assume closer to left initially
				ds = dl;
				if( dr < ds){
					ds = dr;
					direct = 1;
				}
				if(dl > 0 && dr > 0 && ds > HAND_RANGE){ //mouse is in the center range of
					exist = 1;
					for( i = _num_hands; i >= ahand; i--){
						_x_hands[i+1] = _x_hands[i];
						_y_hands[i+1] = _y_hands[i];
					}	
					_x_hands[ahand] = x;
					_y_hands[ahand] = y;
					_num_hands++;
				}
			}	
			_spline_points();
			_draw_curvewin();
			SetItemTextToNum(Black_ink_limit,_black_ink_limit,0);
			
		}
	}
//remember the hand position
	_phand.h = where.h;
	_phand.v = where.v;

}

//save to input black table when window is closed
McoStatus	BlackDialog::_saveblack(void)
{
	short 	i;
	double *black;
	
	
	black = (double*)McoLockHandle(_blackH);
//L to Black table	
	for( i = 0; i <= 100; i++)
		splint(_x_hands,_y_hands,_y2_hands,_num_hands,i, black+i);

//fix on 8/23
//clipping the _yd_points which on the left of letfmost hand
//to the value of the leftmost hand
//on the right of rightmost hand to the value of the rightmost hand
	for( i = 0; i <= 100; i++){
		if( i <= _x_hands[1] ) //left of the leftmost hand
			black[i] = _y_hands[1];
		if( i >= _x_hands[_num_hands] ) //left of the leftmost hand
			black[i] = _y_hands[_num_hands];
	}

//clipping the table and normaliz to 1
	for( i = 0; i <= 100; i++){
		if(black[i] < 0)
			black[i] = 0;
		else if(black[i] > 100)
			black[i] = 1.0;
		else
			black[i] /= 100.0;
	}

				
	McoUnlockHandle(_blackH);
	
	*_total_limit_ptr = (double)_total_ink_limit/100.0;
	*_black_limit_ptr = (double)_black_ink_limit/100.0;
	
	return MCO_SUCCESS;
}	


McoStatus BlackDialog::DrawWindow(void)	
{			
	Rect updateRect;
	RGBColor	c,oldbackcolor;

	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	_draw_curvewin();
	return MCO_SUCCESS;
}	

McoStatus BlackDialog::UpdateWindow(void)	
{			
	Rect updateRect;
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	_draw_curvewin();
	DrawDialog( dp );
	EndUpdate( dp );
	
	return MCO_SUCCESS;
}			

McoStatus BlackDialog::updateWindowData(int changed)
{
short	iType;
Handle	iHandle;
Rect	iRect;
WindowPtr	oldp;
	
GetPort(&oldp);
SetPort(dp);	

if (doc->locked) 
	{
	Disable(dp,OK);
	Disable(dp,Cancel);
	}
else if (doc->inputStatus() == Building_Table)
	{
	disabled = 1;
	Disable(dp,OK);
	Disable(dp,Cancel);
	}	
else if ((doc->inputStatus() & Processing_Input2) || 
		 (doc->inputStatus() & Tweaking_Input))
	{
	disabled = 0;
	Disable(dp,OK);
	Disable(dp,Cancel);
	}
else 
	{
	disabled = 0;
	Enable(dp,OK);
	Enable(dp,Cancel);
	}
	

	
getParameters();
return MCO_SUCCESS;
}



McoStatus BlackDialog::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
{
	short 		itemType, lastitem = 1;
	GrafPtr		oldGraf;
	Handle			cn1;	// added by James
	ControlHandle	sliderControl;
	Rect			wr;		// added by James
	McoStatus		status = MCO_SUCCESS;

	GetPort(&oldGraf);
	SetPort(dp);
	
	Changed = FALSE;
	*numwc = 0;
	status = MCO_SUCCESS;

	if (item == OK)
		{ //Ok
		if (current_printer == 1) 
			{
			getParameters();
			saveLastPrinter();
			}
		
		_check_limit(item, lastitem);
		_save_resource();
		// build icc and photoshop tables if option key is pressed
		if (isPressed(0x3a)) _condataptr->tabletype = 8;
		_saveblack();				
		status = _loadsimufile();
		if (status != MCO_SUCCESS) 
			{
			McoErrorAlert(status);
			status = MCO_SUCCESS;
			*numwc = 1;
			wc[0].code = WE_Close_Window;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			}
		else
			{		
			*numwc = 2;
			wc[0].code = WE_PrDoc_BuildTable;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			wc[1].code = WE_Close_Window;
			wc[1].wintype = WinType;
			wc[1].winnum = WinNum;
			wc[1].doc = doc;
			}
		}

	else if (item == Cancel) //Cancel
		{
		*numwc = 1;
		wc[0].code = WE_Close_Window;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item == Tweak) // open a tweak window
		{
		*numwc = 1;
		wc[0].code = WE_PrDoc_Tweak;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item == Options) // open a options window
		{
		*numwc = 1;
		wc[0].code = WE_PrDoc_Options;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item == CurveWindow)
		{
		_check_limit(item, lastitem);
		_hand_curvewin();
		}
	else if (item == BlackMenu)
		{
		_check_limit(item, lastitem);
		_choose_black_type();
		}
		
	else if (item == Total_ink_limit)
		{
		_check_limit(item, lastitem);
		_check_limit_immed(item, lastitem);
		_choose_total_limit();
		}
		
	else if (item == Black_ink_limit)
		{
		_check_limit(item, lastitem);
		_check_limit_immed(item, lastitem);
		_choose_black_limit();
		}		
	// The following was added by James
	

	else if (item == Simulate_type)
		{
		_check_limit(item, lastitem);
		}			
	else if (item == Table_type_menu)
		{
		_check_limit(item, lastitem);
		_Settabletype();
		Changed = TRUE;
		}
	else if ((item == Gcr_radio) || (item == Ucr_radio))
		{
		_check_limit(item, lastitem);
		_Setradiobutton(item, _separation_type);
		_separation_type = item;
		_Setucr_ucr_gcr_type();
		_draw_curvewin();
		}
	else if (item == PrinterTypeID)
		{
		GetDItem(dp, PrinterTypeID, &itemType, (Handle*)&sliderControl,&wr);
		current_printer = (long)GetCtlValue((ControlHandle) sliderControl);
		setParameters();
		Changed = TRUE;
		copyprinterType(printers[current_printer],doc->printerData);
		}
	else if (item == LoadSettings)
		{
		loadprinterfile();
		setParameters();
		Changed = TRUE;
		}
	else if (item == SaveSettings)
		{
		current_printer = 1;
		getParameters();
		saveprinterfile();
		copyprinterType(printers[current_printer],doc->printerData);
		}	
	SetPort(oldGraf);
	
	changed = Changed;
	return status;
}


//check the total and black ink limit text boxes and update data
void BlackDialog::_check_limit(short curitem, short lastitem)
{
	short		iType, item;
	Handle		iHandle;
	Rect 		iRect;
	int32		i, num;	
	Str255		numstr;
	double 		scale;
	double		original,temp;

	if(curitem == lastitem)
		return;
	
	switch(lastitem){
		case(Total_ink_limit):	
			original = _total_ink_limit;
			GetDItem (dp, Total_ink_limit , &iType, &iHandle, &iRect);
			GetIText(iHandle, numstr);
			StringToNum(numstr, &_total_ink_limit);
		
			if(_total_ink_limit < TOTALINKLOW || _total_ink_limit > TOTALINKHIGH){
				//show_err("\pGamma must be a number between 0 and 400, closest number inserted.");
				if(_total_ink_limit < TOTALINKLOW)
					_total_ink_limit = TOTALINKLOW;
				else
					_total_ink_limit = TOTALINKHIGH;
				
				NumToString(_total_ink_limit, numstr);
				SetIText(iHandle, numstr);
			}
			if (original == 0) original = SMALL_K_MAX;
			temp = (double)_total_ink_limit;
			if (temp == 0) temp = SMALL_K_MAX;
			scale = temp/original;
			break;
			
		case(Black_ink_limit):
			original = _black_ink_limit;
			GetDItem (dp, Black_ink_limit , &iType, &iHandle, &iRect);
			GetIText(iHandle, numstr);
			StringToNum(numstr, &_black_ink_limit);
		
			if(_black_ink_limit < BLACKINKLOW || _black_ink_limit > BLACKINKHIGH){
				//show_err("\pGamma must be a number between 0 and 400, closest number inserted.");
				if(_black_ink_limit < BLACKINKLOW)
					_black_ink_limit = BLACKINKLOW;
				else
					_black_ink_limit = BLACKINKHIGH;

				NumToString(_black_ink_limit, numstr);
				SetIText(iHandle, numstr);
			}
			
			if (original == 0) original = SMALL_K_MAX;
			temp = (double)_black_ink_limit;
			if (temp == 0) temp = SMALL_K_MAX;
			scale = temp/original;
			_scale_spline_points(scale);					
			_spline_points();
			_draw_curvewin();
			break;

		default:
			break;
	}
}
	
//check the total and black ink limit
void BlackDialog::_check_limit_immed(short curitem, short lastitem)
{
	short		iType, item;
	Handle		iHandle;
	Rect 		iRect;
	int32		i, num;	
	Str255		numstr;
	double 		scale;
	double		original,temp;

	switch(curitem){
		case(Total_ink_limit):	
			original = _total_ink_limit;
			GetDItem (dp, Total_ink_limit , &iType, &iHandle, &iRect);
			GetIText(iHandle, numstr);
			StringToNum(numstr, &_total_ink_limit);
		
			if(_total_ink_limit < TOTALINKLOW || _total_ink_limit > TOTALINKHIGH){
				_total_ink_limit = original;
				return;
			}
			if (original == 0) original = SMALL_K_MAX;
			temp = (double)_total_ink_limit;
			if (temp == 0) temp = SMALL_K_MAX;
			scale = temp/original;
			break;
			
		case(Black_ink_limit):
			original = _black_ink_limit;
			GetDItem (dp, Black_ink_limit , &iType, &iHandle, &iRect);
			GetIText(iHandle, numstr);
			StringToNum(numstr, &_black_ink_limit);
		
			if(_black_ink_limit < BLACKINKLOW || _black_ink_limit > BLACKINKHIGH){
				_black_ink_limit = original; 
				return;
			}
			if (original == 0) original = SMALL_K_MAX;
			temp = (double)_black_ink_limit;
			if (temp == 0) temp = SMALL_K_MAX;
			scale = temp/original;
			_scale_spline_points(scale);					
			_spline_points();
			_draw_curvewin();
			break;

		default:
			break;
	}
}

			
void BlackDialog::_choose_total_limit(void)
{

}

void BlackDialog::_save_resource(void)
{
	short		iType, item;
	Handle		iHandle;
	Rect 		iRect;
	int32		i, num;	
	Str255		numstr;
	long		temp;

//black ink string

	_condataptr->black_ink_limit = _black_ink_limit;
//	printers[current_printer].black_ink_limit = _black_ink_limit;

//total ink string

	_condataptr->total_ink_limit = _total_ink_limit;
//	printers[current_printer].total_ink_limit = _total_ink_limit;
	
//table type string
	GetDItem(dp, Table_type_menu, &iType, &iHandle, &iRect);
	_condataptr->tabletype = (long)GetCtlValue((ControlHandle)iHandle);

	
//black type string
	GetDItem(dp, BlackMenu, &iType, &iHandle, &iRect);
	_condataptr->black_type = (long)GetCtlValue((ControlHandle)iHandle);
	
//added for black separation type (GCR or UCR)	
	if(_separation_type == Gcr_radio)
		_condataptr->separation_type = 0;
	else
		_condataptr->separation_type = 1;
		

}

//added for ink simulation on 4/11/96 by Peter
//load data file
McoStatus BlackDialog::_loadsimufile(void)
{
	short		iType, item;
	Handle		iHandle;
	Rect 		iRect;
	int32		i, num;	
	Str255		numstr;
	short		reterr;
	McoStatus status = MCO_SUCCESS;

	GetDItem(dp, Simulate_type, &iType, &iHandle, &iRect);
	item = GetCtlValue((ControlHandle)iHandle);

	if(item == 1 && _table_type != 5 &&_table_type != 6){	//no simulate
		_condataptr->simulate = 0;
		return MCO_SUCCESS;
	}	

	

	_condataptr->simulate = 1;
	
//	MenuHandle menuH;	
//	Str255 itemString;
	FILE 	*fs;
	Str255	inname;
	
//	menuH = GetMenu(Simulate_menu);	
//	GetItem(menuH,item,inname);

	memcpy(inname, _simunames[item-1],33);  
	
	
	PathNameFromDirID(_simuFile.parID, _simuFile.vRefNum, inname);
	ptocstr(inname);
	
	fs = fopen( (char*)inname, "r" );
	if(!fs)	return MCO_FAILURE;
	
	status = doc->_read_datafile(&doc->simupatchD,fs);
	
	fclose(fs);

	return status;
}

void	BlackDialog::_Setradiobutton(short curitem, short lastitem)
{

	if( lastitem != curitem ){
		_Setradiostate(lastitem, 0);		
	
		_Setradiostate(curitem, 1);	
	}
}


void	BlackDialog::_Setradiostate(short item, short value)
{
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	

	GetDItem (dp, item, &iType, (Handle*)&iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle,value);
}


void BlackDialog::_choose_black_limit(void)
{


}


//added on 5/8
void BlackDialog::_Settabletype(void)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i;
	long 	temp;
	MenuHandle menuH;
	short menucount;
	Str32	filename;
	Str255	theString;

	GetDItem(dp, Table_type_menu, &iType, &iHandle, &iRect);
	_table_type = GetCtlValue((ControlHandle)iHandle);
	doc->_condata.tabletype = _table_type;

	//need cmyk to cmyk conversion
	if(_table_type >= CMYK_TO_CMYK && _last_table_type < CMYK_TO_CMYK){
		menuH = GetMenu(Simulate_menu);
		menucount = CountMItems(menuH);
		DelMenuItem(menuH,1);
		DrawControls(dp);
	}
	else if(_table_type < CMYK_TO_CMYK	&& _last_table_type >= CMYK_TO_CMYK){
		menuH = GetMenu(Simulate_menu);
		menucount = CountMItems(menuH);
		GetIndString(theString,Ink_simulation_menu_string,1);
		InsMenuItem(menuH,theString,0);		
		DrawControls(dp);
	} 
	
	_last_table_type = _table_type;	
		
}


void BlackDialog::_Setucr_ucr_gcr_type(void)
{

	//need cmyk to cmyk conversion
	if(_separation_type == Gcr_radio){
		Enable(dp,BlackMenu);
		
	}
	else {
		Disable(dp,BlackMenu);

	} 
	
	//added for black separation type (GCR or UCR)	
	if(_separation_type == Gcr_radio)
		_condataptr->separation_type = 0;
	else
		_condataptr->separation_type = 1;
	
	_init_spline_points();
		
}
///////////////////////////////////////////////////////////////////////////////////
//	profileDocinf.cpp														  	 //
//																			  	 //
//	User interface parts of profiledoc											 //
//  																		     //
//	James Vogh																 	 //
// June 4, 1996																  	 //
///////////////////////////////////////////////////////////////////////////////////

#include "profiledocinf.h"
#include "inputdialog.h"
#include "rchoosedialog.h"
#include "printdialog.h"
#include "errordial.h"
#include "eve_stuff_2&3.h"
#include "gammut_comp.h"
#include "pascal_string.h"

#include "xritecom.h"
#include "gretag.h"
#include "spectrochart.h"
#include "techkon.h"
#include "tcr.h"
#include "xrite408.h"

ProfileDocInf::ProfileDocInf(void):ProfileDoc()
	{
	gdata.gH = NULL;
	copy_str(patchFName,"\puntitled");
	defaults = new Defaults;
	
	last_type = DT_None;
	last_port = 0;
	tcom = 0L;
	
	patchset = new PatchSet;
	if (defaults->patch_format >= patchset->filenum) defaults->patch_format = 0;
	
	}
	
ProfileDocInf::~ProfileDocInf(void)
{
if (defaults) delete defaults;
if (tcom) delete tcom;
if (patchset) delete patchset;
}

// check the dongle and decrement the counter
McoStatus ProfileDocInf::_checkEve(void)
{

	McoUse	use;
	int		count = 0;
	Str255	st;
	int		CompSize;

	use = get_usage_level(&count);
	//use = (McoUse)P_SuperUse;
	//count = 2500;
	
	if (use < P_AlmostNoUse) 
		{
		NumToString (count, st);
		McoMessAlert(MCO_NO_USE,st);
		return MCO_BADCOUNT;
		}
	if (use == P_AlmostNoUse) 
		{
		NumToString (count, st);
		McoMessAlert(MCO_ALMOST_USE,st);
		}
	
	// Decrement the counter.  This will not harm counter if >1499 
	
	if (count<NORMCOUNT) decrement_count_print();	
	IncDealerCnt(); 

return MCO_SUCCESS;
}

//read the file, now only read the data file
McoStatus ProfileDocInf::_read_datafile(RawData **patchd_2,FILE *fs)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	short				i,j,k,m;
	double *patch, *patchcopy;
	int32				total = 0, dimCount = 0;
	int32				dimp = 1;
	RawData 			*patchd;
		

	patchd = *patchd_2;	
	if (patchd != NULL) delete patchd;
	patchd = new RawData();
	if (patchd == NULL) return MCO_MEM_ALLOC_ERROR;
	*patchd_2 = patchd;
	
	status = patchd->loadData(fs);
	if (status) return status;
	
	patchtype = (PatchType)patchd->type;
	return MCO_SUCCESS;
}


// read in a data file

McoStatus ProfileDocInf::_handle_read(short *item,short *which,int density)
{
	RchooseDialog *rchoosedialog;
	
	
	rchoosedialog = new RchooseDialog(this,density);
	if(!rchoosedialog)
		return MCO_MEM_ALLOC_ERROR;
	
	*item = rchoosedialog->hit();
	*which = rchoosedialog->getchoice();
	delete rchoosedialog;
	if(*item == 1){
		if( *which == DataFile) //read from data file
			return _handle_read_datafile();
	}	
	
	return MCO_CANCEL;	
}

// save the file from the menu
// data may need to be copied from the input dialog
McoStatus  ProfileDocInf::save_datafile_menu(void)
{
InputDialog *iwin;
int i;


for (i=0; i<numWins; i++)
	{
	if ((wins[i]->isMyWindowType(StripDialog)) || (wins[i]->isMyWindowType(TcrWindow)) ||
		(wins[i]->isMyWindowType(TechkonWindow)) || (wins[i]->isMyWindowType(GretagSLWindow)) ||
		(wins[i]->isMyWindowType(GretagSCWindow)) )
		{
		iwin = (InputDialog*)wins[i];
		iwin->StorePatches();
		}
	}

return handle_save_datafile(patchDcopy);
}


// Remove a window from the list of windows
McoStatus ProfileDocInf::removeWindow(baseWin *win)
{
int i,j;
PrintDialog *pd;
MS_WindowTypes wt1,wt2;

wt1 = win->WinType;

for (i=0; i<numWins; i++) if (win == wins[i]) break;
if (i == numWins) return MCO_SUCCESS;
for (j=i; j<numWins; j++)
	{
	wins[j] = wins[j+1];
	}
numWins--;

if ((wt1 == Gammut_Dialog_Src) || (wt1 == Gammut_Dialog_Dst))
	{
	wt2 = PrintWindow;
	pd = (PrintDialog*)getWindow(&wt2,0);
	if (pd != NULL) pd->gammut_winNum = -1;
	}
return MCO_SUCCESS;
}

// determine what mode the document is currently in

long	ProfileDocInf::inputStatus()
{
int i;
long	state = 0;

// first see if an input level 2 window is open
for (i=0; i< numWins; i++) 
	{
	if (wins[i]->WinType == SelAdjWindow) state = state | Tweaking_Input;
	if (wins[i]->WinType == TweakWindow) state = state | Tweaking_Input;
	if (wins[i]->WinType == OptionWindow) state = state | Processing_Input2; 
	if (wins[i]->WinType == CalibDialog) state = state | Processing_Input1;
	if ((wins[i]->WinType == StripDialog) || 
	    (wins[i]->WinType == CropWindow)  ||
	    (wins[i]->WinType == TcrWindow)  ||
	    (wins[i]->WinType == TechkonWindow) ||
	    (wins[i]->WinType == GretagSLWindow) ||
	    (wins[i]->WinType == GretagSCWindow)) state = state | Getting_Input;  
	}

return state;
}

// restore the data in patchD with the copy stored in patchDcopy

McoStatus ProfileDocInf::copyWaiting(RawData *p)
{
McoStatus state = MCO_SUCCESS;
if (p == patchDcopy)
	{
	state = patchD->copyRawData(patchDcopy);
	if (state) return state;
	return loadLinearData();
	}
return MCO_SUCCESS;
}


// open an input device
McoStatus ProfileDocInf::openInputDevice(int xrite_num,int handheld,int density)
{
if (tcom) delete tcom;
long	istate;
McoStatus state;
short item,which;

istate = inputStatus();
handheld = handheld | (istate & Tweaking_Input);

if ((!density) && (defaults->input_type == DT_Xrite408))
	{	// must change device because lab reading is required
	state = _handle_read(&item,&which,0);
	}
	
if (tcom) delete tcom;
tcom = 0L;
switch (defaults->input_type) {
	case DT_DTP51:	// xrite dtp 51
		
		if (!handheld) tcom = new XriteCom(defaults->port-1,defaults->input_type,xrite_num);
		else { defaults->input_type = DT_None; return MCO_SUCCESS;}
		break;
	case DT_TechkonCP300:
	case DT_TechkonSP820:  // techkon handhelds
		tcom = new Techkon(defaults->port-1,defaults->input_type);
		break;
	case DT_TechkonTCRCP300:
	case DT_TechkonTCRSP820: // techkon table
		if (!handheld) tcom = new TCR(defaults->port-1,defaults->input_type);
		else { defaults->input_type = DT_None; return MCO_SUCCESS;}
		break;
	case DT_GretagHand: // gretag handheld
		tcom = new GretagCom(defaults->port-1,defaults->input_type);
		break;
	case DT_GretagTable: 
		if (!handheld) tcom = new SpectroChart(defaults->port-1,defaults->input_type);
		else { defaults->input_type = DT_None; return MCO_SUCCESS;}
		break;
	case DT_Xrite408:
		if (density) tcom = new XRite408(defaults->port-1);
		break;
	default: return MCO_SUCCESS;
	}

if (tcom == 0L) return MCO_MEM_ALLOC_ERROR;	
if (tcom->state != MCO_SUCCESS) return tcom->state;

return tcom->Initialize();
}

// change the current input device
// do nothing if the input dialog is open

McoStatus ProfileDocInf::updateInputDevice(int xrite_num)
{
McoStatus state;


if ((inputStatus() & Getting_Input) || (tcom == 0L)) 
	return MCO_SUCCESS;
	
if ((defaults->input_type == last_type) && (defaults->port == last_port))
	return MCO_SUCCESS;
	
// set handheld to zero because openInputDevices checks to see if a handheld 
// is required
// set density only flag to zero because lab may be required	
state = openInputDevice(xrite_num,0,0);
if (state != MCO_SUCCESS) return state;

tcom->Calibrate();

last_type = defaults->input_type;
last_port = defaults->port;
	
return MCO_SUCCESS;	
}

// close the input device if it is open

McoStatus ProfileDocInf::closeInputDevice(void)
{
if (tcom) delete tcom;
tcom = 0L;
}

// Do a command
McoStatus ProfileDocInf::doCommand(WindowCode *wc,WindowCode *wc2,void **data,Boolean *changed)
{
short item,which;
McoStatus state;
PrintDialog *pd;
MS_WindowTypes wt;
int32		wn;
int		saveCode;
baseWin *win;

wc2[0].code = WE_Do_Nothing;

switch (wc[0].code) {
	case WE_Doc_Print:	// print the document
		wc2[0].code = WE_Do_Nothing;
		return _handle_print();
	case WE_Set_Pref:
		state = _handle_read(&item,&which,1);
		updateInputDevice(0);
		break;
	case WE_Doc_Read: // do a read operation (from datafile, device, or tiff file)
	case WE_Source_Read:
		wc2[0].code = WE_Do_Nothing;
		if (wc[0].code == WE_Doc_Read) waitingPatches = patchDcopy;
		else waitingPatches = simupatchD;
	#ifdef IN_POLAROID
		if ((defaults->input_type == DT_None) || (defaults->input_type == DT_Xrite408))
			{
			state = _handle_read(&item,&which,0);
			if (item == 0) break;	
			}
	#else
		state = _handle_read(&item,&which,0);
		if (item == 0) break;	
	#endif
		which = defaults->input_type;	
		// set up the patch
		if ((defaults->patch_format < 0) || (defaults->patch_format > patchset->filenum)) goto bail;
		if (patchset->patches[defaults->patch_format] == NULL) goto bail;	
		if (waitingPatches->sameFormat(patchset->patches[defaults->patch_format]))
			waitingPatches->copyPatchData(patchset->patches[defaults->patch_format]);
		else waitingPatches->copyRawData(patchset->patches[defaults->patch_format]);	
			
		if (waitingPatches == NULL) break;
		if (which == DT_DTP51)  // Xrite DTP 51
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = StripDialog;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_TiffFile)   	//Tiff
			{
			state = _handle_read_tifffile(waitingPatches);
			McoErrorAlert(state);
			if (state == MCO_SUCCESS) 
				{
				wc2[0].code = WE_Open_Window;
				wc2[0].wintype = CropWindow;
				wc2[0].winnum = 0;
				wc2[0].doc = this;
				}
			}
		if (which == DT_TechkonCP300)	// Techkon CP300 handheld
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = TechkonWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_TechkonSP820)	// Techkon SP800 handheld
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = TechkonWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_TechkonTCRCP300) // Techkon CP300 on TCR
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = TcrWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_TechkonTCRSP820) // Techkon SP820 on TCR
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = TcrWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_GretagHand) // gretag spectrolino
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = GretagSLWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		if (which == DT_GretagTable) // gretag table
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = GretagSCWindow;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		*changed = TRUE;
		break;
	case WE_PrDoc_Gammut:	// open a gamut display window
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = Gammut_Dialog_Dst;
		wc2[0].winnum = -1;
		wc2[0].doc = this;
		wt = PrintWindow;
		wn = 0;
		pd = (PrintDialog*)getWindow(&wt,&wn);
		if (pd != 0L) wc2[0].winnum = pd->gammut_winNum;
		// first see if window is already open
		win = getWindow(&wc2[0].wintype,&wc2[0].winnum);
		if (win == 0L)
			{
			// check for therm window
			wc2[0].wintype = GammutSurfaceTherm_Dst;
			win = getWindow(&wc2[0].wintype,&wc2[0].winnum);
			if (win == 0L)
				{
				wc2[0].wintype = GammutSurfaceTherm_Dst;
				wc2[0].winnum = -1;
				wc2[0].doc = this;
				if (wc[0].code == WE_PrDoc_Gammut) *data = patchD;
				else *data = simupatchD;
				}
			}
		break;
	case WE_PrSource_Gammut:
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = Gammut_Dialog_Src;
		wc2[0].winnum = -1;
		wc2[0].doc = this;
		wt = PrintWindow;
		wn = 0;
		pd = (PrintDialog*)getWindow(&wt,&wn);
		if (pd != 0L) wc2[0].winnum = pd->gammut_winNum;
		// first see if window is already open
		win = getWindow(&wc2[0].wintype,&wc2[0].winnum);
		if (win == 0L)
			{
			wc2[0].wintype = GammutSurfaceTherm_Src;
			win = getWindow(&wc2[0].wintype,&wc2[0].winnum);
			if (win == 0L)
				{
				wc2[0].wintype = GammutSurfaceTherm_Src;
				wc2[0].winnum = -1;
				wc2[0].doc = this;
				if (wc[0].code == WE_PrDoc_Gammut) *data = patchD;
				else *data = simupatchD;
				}
			}
		break;
	case WE_PrDoc_Calibrate:	// open build profile window
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = CalibDialog;
		wc2[0].winnum = 0;
		wc2[0].doc = this;
		break;
	case WE_Doc_Open:			// open a datafile
		wc2[0].code = WE_Do_Nothing;
		*changed = TRUE;
		return _handle_read_datafile();
	case WE_PrDoc_Tweak:		// open the tweak window (first must build gamut compresion)
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = GammutCompTherm;
		wc2[0].winnum = 0;
		wc2[0].doc = this;
		break;
	case WE_PrDoc_Options:		// open options window
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = OptionWindow;
		wc2[0].winnum = 0;
		wc2[0].doc = this;
		break;
	case WE_PrDoc_BuildTable:	// build a table
		wc2[0].code = WE_Open_Window;
		wc2[0].wintype = ProcessTherm;
		wc2[0].winnum = 0;
		wc2[0].doc = this;
		break;
	case WE_Doc_Close_Save:
		saveCode = checkCloseDoc();		
		if (saveCode == 2)
			{
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = ProcessThermClose;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		else if (saveCode == 1)
			{
			wc2[0].code = WE_CloseDoc;
			wc2[0].wintype = No_Dialog;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}
		break;
	case WE_Doc_Save:			// save datafile		
	#ifndef IN_POLAROID
		wc2[0].code = WE_Do_Nothing;
		return save_datafile_menu();
	#else
		if (!GetFileName(0))
			{	// if no file has been selected then select a file
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = ProcessTherm;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}		
	#endif
		break;
	case WE_Doc_Save_As:			// save datafile		
	#ifdef IN_POLAROID
		if (!GetFileName(1))
			{	// select a file
			wc2[0].code = WE_Open_Window;
			wc2[0].wintype = ProcessTherm;
			wc2[0].winnum = 0;
			wc2[0].doc = this;
			}		
	#endif
		break;
	case WE_Revert:
		if (!McoMessAlert(MCO_REVERT_FILE,0L))
			{
		 	state = handle_reread_iccfile();
		 	if (state != MCO_SUCCESS) McoErrorAlert(state);
		 	*changed = 1;
		 	}
		break;
	case WE_UpdateInputDevice:	// update the status of the current input device
		state = updateInputDevice(-1);
		if (state != MCO_SUCCESS) McoErrorAlert(state);
		wc2[0].code = WE_Do_Nothing;
		break;		
	}
return MCO_SUCCESS;
bail:
return MCO_FAILURE;
}

// if doc is to be closed, check to se if data needs to be saved

Boolean ProfileDocInf::checkCloseDoc(void)
{
short item;

if (processing) return 0;

#ifndef IN_POLAROID
if (!needsSave) return 1;
#else
if ((!needsSave) && (!changedProfile) && (!changedLinearTone)) return 1;
#endif

item = McoCloseAlert(checkDocCanBeSave());
switch (item) {
	case 1:
		#ifndef IN_POLAROID
		save_datafile_menu();
		needsSave = FALSE;
		return 1;
		#else
		needsSave = FALSE;
		return 2;
		#endif		
	case 2: needsSave = FALSE;
		changedProfile = 0;
		changedLinearTone = 0;
		return 1;
	case 3: return 0;
	}
return 0;
}

// see if the document should be saved
// will return true if document has changed
int	ProfileDocInf::checkDocSave(void)
{

if (processing) return 0;
#ifndef IN_POLAROID
if (needsSave) return 1;
#else
if ((needsSave) || (changedProfile) || (changedLinearTone)) return 1;
#endif

//if (changedLinearTone) return TRUE;

//if ((dataPresent()) && (validCal())) return TRUE;
//if ((!monacoProfile)  && (validCal())) return TRUE;
//if ((monacoEdited) && (validCal())) return TRUE;

return 0;
}


// see if the document can be saved
// will return true only if document can actually be saved
int	ProfileDocInf::checkDocCanBeSave(void)
{
if (processing) return 0;
#ifndef IN_POLAROID
#else
if (((!monacoProfile) || (monacoEdited)) && (changedLinearTone)) return 1;
#endif


if ((dataPresent()) && (validCal()) &&  (changedProfile)) return TRUE;
if ((!createdProfile) && (monacoProfile) && (changedLinearTone)) return 1;
//if ((!monacoProfile)  && (validCal())) return TRUE;
//if ((monacoEdited) && (validCal())) return TRUE;

return 0;
}

int ProfileDocInf::isMeasuring(void)
{
if (tcom) return 1;
return 0;
}

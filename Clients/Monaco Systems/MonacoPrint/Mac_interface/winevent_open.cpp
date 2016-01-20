/******************************************************************/
/* winevent_open.cpp											  */
/* Copyright(c) 1995 Monaco Systems Inc.						  */
/* The main event handler										  */
/* open a window												  */
/* May 5, 1995												      */
/* James Vogh													  */
/******************************************************************/

#include "winevent.h"

#include "profiledocmac.h"
#include "pascal_string.h"
#include "think_bugs.h"
#include "fullname.h"

// windows and dialogs
#include "aboutdialog.h"
#include "qd3Dwin.h"
#include "gammutwin.h"
#include "specchartdial.h"
#include "techkondial.h"
#include "processtherm.h"
#include "gamutsurftherm.h"
#include "gamutcomptherm.h"
#include "displaydial.h"
#include "printdocwindow.h"
#include "tweakwin.h"
#include "logowin2.h"
#include "logodialog.h"
#include "aboutdialog.h"
#include "printdialog.h"
#include "selAdjWin.h"
#include "optiondialog.h"
#include "targetdialog5.h"
#include "tcrindialog.h"
#include "dtp51dialog.h"
#include "blackdialog.h"
#include "gretagspecl.h"
#include "errordial.h"


// Open a gammut surface dialog, use info passed in data if data is valid
McoStatus AllWins::OpenGammutDialog(void *data,MS_WindowTypes wintype,Document *doc)
{
	GamutWin *gamutwin;
	int32	maxwinnum  = 0;
	McoStatus	state;
	int i;
	Str255		patchFName;
	GammutWinData	*Gdata;


	if (!QD3D_Present) return MCO_NO_QD3D;
	if (xyztorgb == NULL) return MCO_FAILURE;
	if (data == 0L) return MCO_FAILURE;

	Gdata = (GammutWinData*)data;
	maxwinnum = 0;
	for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(wintype))
		{
		if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
		}
		
	gamutwin = new GamutWin(0,256,Gdata->gH,maxwinnum,wintype,xyztorgb,monitor_z ,Gdata->fn);

	// delete the gdata structure now
	delete Gdata;

	if(!gamutwin)
		return MCO_MEM_ALLOC_ERROR;

	if (gamutwin->object_state != MCO_SUCCESS) 
		{
		state = gamutwin->object_state;
		delete gamutwin;
		return state;
		}
		
	// a hack, set printdialog window number
	
	if ((wintype == Gammut_Dialog_Src) || (wintype == Gammut_Dialog_Dst))
		{
		PrintDialog *pd;
		
		
		for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(PrintWindow))
			{
			pd = (PrintDialog*)wins[i];
			pd->gammut_winNum = maxwinnum;
			}
		}

	addToShared(gamutwin,wintype);
	return AddWin(gamutwin,doc);
}


// Open a gammut surface dialog, use info passed in data if data is valid
McoStatus AllWins::openGammutSurface1(void)
{
	GamutSurfaceWin 	*gamutsurf;
	int32		maxwinnum  = 0;
	McoStatus	state = MCO_SUCCESS;
	int i;
	Str255		patchFName;
	RawData		*raw;
	

	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	FILE 				*fs;
	if (!QD3D_Present) return MCO_NO_QD3D;
	if (xyztorgb == NULL) return MCO_FAILURE;

	
	// raw is deleted by the window
	raw = new RawData();	
	
	filelist[0] = 'TEXT';
	StandardGetFile(nil, 1, filelist, &sinReply);
	if(sinReply.sfGood == 0)
		return MCO_CANCEL;
		
	copy_str(patchFName,sinReply.sfFile.name);

	memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);
	PathNameFromDirID(sinReply.sfFile.parID, sinReply.sfFile.vRefNum, inname);
	ptocstr(inname);	
	
	fs = fopen( (char*)inname, "r" );
	if(!fs)
		return MCO_FILE_OPEN_ERROR;
		
	raw->loadData(fs);
	
	fclose(fs);
	
	//for (i=0; i<tempdoc->_condata.smooth; i++) raw->smooth_patch();
	
	maxwinnum = 0;
	for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(GammutSurfaceTherm1))
		{
		if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
		}
		
	memcpy(inname, sinReply.sfFile.name, *(sinReply.sfFile.name)+1);	

	// the flag after raw indicates that the window should delete raw
	gamutsurf = new GamutSurfaceWin(0L,raw,1,GammutSurfaceTherm1,maxwinnum,inname);
	if (gamutsurf == 0L) return MCO_MEM_ALLOC_ERROR;
	if (gamutsurf->error == MCO_MEM_ALLOC_ERROR)
		{
		McoErrorAlert(gamutsurf->error);
		delete gamutsurf;
		return MCO_SUCCESS;
		}
	if (gamutsurf->error != MCO_SUCCESS) 
		{
		state = gamutsurf->error;
		delete gamutsurf;
		return state;
		}
	

	return AddWin(gamutsurf,0L);
}

// Open a gammut surface dialog, use info passed in data if data is valid
McoStatus AllWins::openGammutSurface2(Document *doc,void *data,MS_WindowTypes type)
{
	GamutSurfaceWin 	*gamutsurf;
	int32		maxwinnum  = 0;
	McoStatus	state = MCO_SUCCESS;
	int i;
	Str255		patchFName;
	RawData		*patch;
	char		fn[100];
	
	patch = (RawData*)data;


	if (!QD3D_Present) return MCO_NO_QD3D;
	if (xyztorgb == NULL) return MCO_FAILURE;

	if (num_documents <1) return MCO_FAILURE;

	
	for (i=0; i<((ProfileDoc*)doc)->_condata.smooth; i++) 
		((ProfileDoc*)doc)->patchD->smooth_patch();
	
	maxwinnum = 0;
	for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(type))
		{
		if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
		}
		
	strcpy(fn,patch->desc);
	ctopstr(fn);	
		
	memcpy(patchFName, fn, fn[0]+1);

	gamutsurf = new GamutSurfaceWin((ProfileDoc*)doc,patch,0,type,maxwinnum,patchFName);
	if (gamutsurf == 0L) return MCO_MEM_ALLOC_ERROR;
	if (gamutsurf->error == MCO_MEM_ALLOC_ERROR)
		{
		McoErrorAlert(gamutsurf->error);
		delete gamutsurf;
		return MCO_SUCCESS;
		}
	if (gamutsurf->error != MCO_SUCCESS) 
		{
		state = gamutsurf->error;
		delete gamutsurf;
		return state;
		}

	state = AddWin(gamutsurf,doc);
	return state;
}


McoStatus AllWins::openGammutCompTherm(Document *doc)
{
	int	i;
	int32		maxwinnum  = 0;
	GamutCompWin	*gamutcompwin;
	McoStatus	state = MCO_SUCCESS;

	if (num_documents <1) return MCO_FAILURE;

	maxwinnum = 0;
	for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(GammutCompTherm))
		{
		if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
		}
		
	gamutcompwin = new GamutCompWin((ProfileDoc*)doc,GammutCompTherm,maxwinnum);
	if (gamutcompwin == 0L) return MCO_MEM_ALLOC_ERROR;
	if (gamutcompwin->error != MCO_SUCCESS) 
		{
		state = gamutcompwin->error;
		delete gamutcompwin;
		return state;
		}
		
	state = AddWin(gamutcompwin,doc);
	return state;	
}

McoStatus AllWins::openProcessTherm(Document *doc,MS_WindowTypes type)
{
	int i;
	int32		maxwinnum  = 0;
	ProcessWin	*processwin;
	McoStatus	state = MCO_SUCCESS;

	if (num_documents <1) return MCO_FAILURE;

	maxwinnum = 0;
	for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(ProcessTherm))
		{
		if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
		}
		
	processwin = new ProcessWin((ProfileDoc*)doc,type,maxwinnum,((ProfileDoc*)doc)->patchFName);
	if (processwin == 0L) return MCO_MEM_ALLOC_ERROR;
	if (processwin->error == MCO_MEM_ALLOC_ERROR)
		{
		state = processwin->error;
		delete processwin;
		McoErrorAlert(MCO_MEM_ALLOC_ERROR);
		return MCO_SUCCESS;
		}	
	if (processwin->error != MCO_SUCCESS) 
		{
		state = processwin->error;
		delete processwin;
		return state;
		}
		
	return AddWin(processwin,doc);	
}

// open the logo dialog
McoStatus AllWins::openLogoWin(void)
{	
	LogoDialog	*logodialog;
	long		realwaittime;
	
#ifndef IN_POLAROID		
if ((QD3D_Present) && (xyztorgb != NULL))
	{
	LogoWin2 	*logodialog2;
	
	logodialog2 = new LogoWin2(xyztorgb,monitor_z);
	DidLogo = TRUE;
	return AddWin(logodialog2,0L);
	} 
else 
	{	
	LogoDialog *logodialog;
	
	logodialog = new LogoDialog;
	logodialog->hit();
	Delay(120, &realwaittime);
	delete logodialog;
	}

#else
	
	logodialog = new LogoDialog;
	logodialog->hit();
//	Delay(120, &realwaittime);
	delete logodialog;
#endif
DidLogo = TRUE;
return MCO_SUCCESS;
}

// open the about dialog
McoStatus AllWins::openAboutDialog(void)
{
AboutDialog	*adialog;
		
adialog = new AboutDialog;
adialog->hit();
delete adialog;
return MCO_SUCCESS;
}

// open the main dialog
McoStatus AllWins::openMainWindow(Document *doc)
{
PrintDialog *printdialog;
int	maxwinnum = 0;
int i;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

// find a unique window number for the window
for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(PrintWindow))
	{
	if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
	}
printdialog = new PrintDialog((ProfileDoc*)doc,maxwinnum);
if (printdialog == NULL) return MCO_MEM_ALLOC_ERROR;

return  AddWin(printdialog,doc);
}

// open the strip input dialog
McoStatus AllWins::openStripWindow(Document *doc)
{
DTP51Dialog *inputdialog;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

inputdialog = new DTP51Dialog((ProfileDoc*)doc,((ProfileDocInf*)doc)->waitingPatches);
if (inputdialog == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(inputdialog,doc);
}


McoStatus AllWins::openDisplayWindow(Document *doc,void *data,int num)
{
DisplayDialog *disdial;
RawData	*patch = (RawData*)data;

if (doc == 0L) return MCO_OBJECT_NOT_INITIALIZED;
if (patch == 0L) return MCO_OBJECT_NOT_INITIALIZED;

disdial = new DisplayDialog((ProfileDoc*)doc,patch,num);

return AddWin(disdial,doc);
}


// open the calibrate dialog
McoStatus AllWins::openCalibWindow(Document *doc)
{
BlackDialog *blackdialog;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;
blackdialog = new BlackDialog((ProfileDoc*)doc);

return AddWin(blackdialog,doc);
}

// open the tweak window
McoStatus AllWins::openTweakWin(Document *doc)
{	
/*CAdjWin *cadjwin;

if (xyztorgb == NULL) return MCO_FAILURE;

cadjwin = new CAdjWin((ProfileDocInf*)doc,xyztorgb,monitor_z);


return AddWin(cadjwin,doc); 
*/
#ifndef IN_POLAROID
TweakWin *tweakwin;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

tweakwin = new TweakWin((ProfileDocInf*)doc);
if (tweakwin == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(tweakwin,doc); 
#else
return MCO_SUCCESS;
#endif
}

// open the tweak window
McoStatus AllWins::openOptionWin(Document *doc)
{	
OptionDialog *optwin;

optwin = new OptionDialog((ProfileDoc*)doc);

return AddWin(optwin,doc);
}

McoStatus AllWins::openCropWin(Document *doc)
{	
TargetDialog *cropwin;

cropwin = new TargetDialog((ProfileDoc*)doc);

return AddWin(cropwin,doc);
}


// open the techkon handheld input dialog
McoStatus AllWins::openTechkonWindow(Document *doc)
{
TechkonDialog *tcrwindow;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

tcrwindow = new TechkonDialog((ProfileDoc*)doc,((ProfileDocInf*)doc)->waitingPatches);
if (tcrwindow == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(tcrwindow,doc);
}


// open the strip input dialog
McoStatus AllWins::openTcrWindow(Document *doc)
{
TCRInDialog *tcrwindow;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

tcrwindow = new TCRInDialog((ProfileDoc*)doc,((ProfileDocInf*)doc)->waitingPatches);
if (tcrwindow == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(tcrwindow,doc);
}

// open the gretag handheld window
McoStatus AllWins::openGTSLWindow(Document *doc)
{
GTSpecLinDialog *gtslwindow;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

gtslwindow = new GTSpecLinDialog((ProfileDoc*)doc,((ProfileDocInf*)doc)->waitingPatches);
if (gtslwindow == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(gtslwindow,doc);
}


// open the gretag handheld window
McoStatus AllWins::openSpChWindow(Document *doc)
{
SpChInDialog *gtslwindow;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

gtslwindow = new SpChInDialog((ProfileDoc*)doc,((ProfileDocInf*)doc)->waitingPatches);
if (gtslwindow == NULL) return MCO_MEM_ALLOC_ERROR;

return AddWin(gtslwindow,doc);

}

// open the main dialog
McoStatus AllWins::openProfDocWin(Document *doc)
{
#ifdef IN_POLAROID
PrintDocWindow *printdialog;
int	maxwinnum = 0;
int i;

if (doc == NULL) return MCO_OBJECT_NOT_INITIALIZED;

// find a unique window number for the window
for (i=0; i<numWins; i++) if (wins[i]->isMyWindowType(PrintWindow))
	{
	if (wins[i]->WinNum >= maxwinnum) maxwinnum = wins[i]->WinNum+1;
	}
printdialog = new PrintDocWindow((ProfileDoc*)doc,maxwinnum);
if (printdialog == NULL) return MCO_MEM_ALLOC_ERROR;

return  AddWin(printdialog,doc);
#endif
}

// Open one of the window types
McoStatus AllWins::openWindow(MS_WindowTypes type,int32 winNum,void *data,Document *doc)
{
McoStatus state;
MS_WindowTypes t;

switch (type) {
	case Gammut_Dialog1:
	case Gammut_Dialog_Src:
	case Gammut_Dialog_Dst:
		state = OpenGammutDialog(data,type,doc);
		break;
	case Startup_Dialog:
		state = openLogoWin();
		break;
	case PrintWindow:
		state = openMainWindow(doc);
		break;
	case AboutWindow:
		state = openAboutDialog();
		break;
	case StripDialog:
		state = openStripWindow(doc);
		break;
	case CalibDialog:
		state = openCalibWindow(doc);
		break;
	case SelAdjWindow:
		state = openTweakWin(doc);
		break;
	case OptionWindow:
		state = openOptionWin(doc);
		break;
	case CropWindow:
		state = openCropWin(doc);
		break;
	case TechkonWindow:
		state = openTechkonWindow(doc);
		break;
	case TcrWindow:
		state = openTcrWindow(doc);
		break;
	case GretagSLWindow:		
		state = openGTSLWindow(doc);
		break;
	case GretagSCWindow:		
		state = openSpChWindow(doc);
		break;
	case ProcessTherm:
	case ProcessThermClose:
		state = openProcessTherm(doc,type);
		break;
	case GammutSurfaceTherm1:
		state = openGammutSurface1();
		break;
	case GammutSurfaceTherm_Src:
	case GammutSurfaceTherm_Dst:
		state = openGammutSurface2(doc,data,type);
		break;
	case GammutCompTherm:
		state = openGammutCompTherm(doc);
		break;
	case DisplayWindow:
		state = openDisplayWindow(doc,data,winNum);
		break;	
// polaroid window
	case TabbedDocWindow:
		state = openProfDocWin(doc);
		break;
	default: return MCO_UNSUPPORTED_OPERATION;
	}
if (state != MCO_SUCCESS) return state;
// Make the window the front window
return MCO_SUCCESS;
}

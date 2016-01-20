/******************************************************************/
/* winevent.c													  */
/* Copyright(c) 1995 Monaco Systems Inc.						  */
/* The main event handler										  */
/* Get the event and then handle the events						  */
/* This code is from maincurvedial.c 							  */
/* May 5, 1995												      */
/* James Vogh													  */
/******************************************************************/



#include "winevent.h"
#include "QD3D.h"
#include "QD3DMath.h"
#include "QD3DDrawContext.h"
#include "QD3DShader.h"
#include "QD3DTransform.h"
#include "QD3DGroup.h"
#include "QD3DCamera.h"
#include "Thermometer.h"
#include "gammut_comp.h"
#include "monacoprint.h"
#include "think_bugs.h"
#include "fullname.h"
#include "pascal_string.h"
#include "errordial.h"
#include "profiledocmac.h"

#include "eve_stuff_2&3.h"

AllWins	*gwins;

McoStatus AllWins::Initialize3D(void)
{


QD3D_Present = (long)Q3Initialize;
	if (!QD3D_Present) return MCO_FAILURE;

Q3Initialize();

		
QD3D_Present = TRUE;
return MCO_SUCCESS;
}


McoStatus AllWins::LoadXyzToRgb(void)
{
	FileFormat 	*prefile = 0L;
	FSSpec		prefspec;
	PhotoMonitor	pmonitor;
	Monitor		monitor;
	McoStatus   status;
	
		// setup the lab to xyz transform

	prefile = new FileFormat;
	if(!prefile)
		return MCO_MEM_ALLOC_ERROR;

	status = prefile->openFilefromPref(&prefspec, (char* )"Monaco_Monitor", (char* )"MonacoCOLOR 1.51 Scanners");
	if (status!=MCO_SUCCESS) goto useDefault;
	
		
	status = prefile->relRead(sizeof(PhotoMonitor),(char*)&pmonitor);
	prefile->closeFile();
	delete prefile;
	prefile = 0L;
	if (status!=MCO_SUCCESS) goto useDefault;


	xyztorgb = new XyztoRgb;
	if(!xyztorgb)	return MCO_MEM_ALLOC_ERROR;

	ConvertMonitor(&pmonitor,&monitor);
	
	status = xyztorgb->xyz2rgb( &monitor);
	if(status != MCO_SUCCESS){
		delete xyztorgb;
		return status;
	}
	
	monitor_z = monitor.setup[0];
	return MCO_SUCCESS;
	
useDefault:
	pmonitor.version = 2;
	pmonitor.gamma = 180;
	pmonitor.whiteX = 3126;
	pmonitor.whiteY = 3289;
	pmonitor.RedX = 6584;
	pmonitor.RedY = 3377;
	pmonitor.GreenX = 2799;
	pmonitor.GreenY = 7200;	
	pmonitor.BlueX = 1342;
	pmonitor.BlueY = 720;
	
	
	xyztorgb = new XyztoRgb;
	if(!xyztorgb)	return MCO_MEM_ALLOC_ERROR;

	ConvertMonitor(&pmonitor,&monitor);
	
	status = xyztorgb->xyz2rgb( &monitor);
	if(status != MCO_SUCCESS){
		delete xyztorgb;
		return status;
	}
	
	monitor_z = monitor.setup[0];
	
	if (prefile) delete prefile;
	return MCO_SUCCESS;
}


// quit the application
pascal OSErr DoQuitApp(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon);
pascal OSErr DoQuitApp(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon)
{

if (gwins != NULL) gwins->DoQuit = TRUE;

return(0);
}

// open a document
pascal OSErr DoOpenDoc(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon);
pascal OSErr DoOpenDoc(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon)
{
	FSSpec	myFSS;
	AEDescList	docList;
	OSErr	myErr;
	long	i,index, itemsInList;
	Size	actualSize;
	AEKeyword	keywd;
	DescType	returnedType;
	McoStatus 	state;
	OSErr				iErr;
	FInfo				fndrInfo;
	
	// get the direct parameter--a descriptor list--and put
	// it into docList
	myErr = AEGetParamDesc(theAEvent, keyDirectObject,
							typeAEList, &docList);
	if (myErr) return myErr;


	// count the number of descriptor records in the list
	myErr = AECountItems (&docList,&itemsInList);

	// now get each descriptor record from the list, coerce
	// the returned data to an FSSpec record, and open the
	// associated file
	
	// will only open the first file
	if (itemsInList >0)
		{
		// get rid of all open documents
		state = MCO_SUCCESS;
		if (gwins->num_documents > 0) state = McoMessAlert(MCO_CLOSE_DOC,NULL);
		if (state == MCO_SUCCESS)
			{
			for (i=gwins->num_documents-1; i>=0; i--) if (gwins->docs[i] != NULL) gwins->removeDocument(gwins->docs[i]);
			
			index =1;
		
			myErr = AEGetNthPtr(&docList, index, typeFSS, &keywd,
							&returnedType, (Ptr)&myFSS,
							sizeof(myFSS), &actualSize);
			if (myErr) return myErr;
			
			if (!gwins->DidLogo)  gwins->openWindow(Startup_Dialog,0,0L,0L);
			
			iErr = FSpGetFInfo(&myFSS,&fndrInfo);
	
			if (fndrInfo.fdType == 'prof') gwins->openProfileDoc(&myFSS);

			if (myErr) return myErr;
			}
		}
	myErr = AEDisposeDesc(&docList);

	return	noErr;

}


// open a application
pascal OSErr DoOpenApp(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon);
pascal OSErr DoOpenApp(AppleEvent *theAEvent,AppleEvent *theAEreply,long handlerrefcon)
{
	if (!gwins->DidLogo) gwins->openWindow(Startup_Dialog,0,0L,0L);
	#ifndef IN_POLAROID		
	gwins->newProfileDoc();
	#else
	//if  (is_monaco_print()) gwins->newProfileDoc();
	#endif

	return	noErr;

}


AllWins::AllWins(Boolean Init3D)
{
int	i;
McoStatus	state;

max_priority = 0;
process_count = 0;

gwins = this;
DoQuit = FALSE;
DidLogo = FALSE;

AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,NewAEEventHandlerProc(DoQuitApp),0,false);	
AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,NewAEEventHandlerProc(DoOpenDoc),0,false);	
AEInstallEventHandler(kCoreEventClass, kAEOpenApplication,NewAEEventHandlerProc(DoOpenApp),0,false);	


QD3D_Present = FALSE;

Boolean Changed;

if (Init3D) 
	{
	state = Initialize3D();
	}

state = LoadXyzToRgb();
if ((state != MCO_SUCCESS) && (xyztorgb != NULL)) 
	{
	delete xyztorgb;
	xyztorgb = NULL;
	}

//OpenJustMemWindow();

numWins = 0;
wins = new baseWin*[MaxNumWins];
for (i=0; i<MaxNumWins; i++) wins[i] = NULL;
maxWins = MaxNumWins;
frontWin = NULL;	// A handy pointer that points the the front window
					// This window is also contained in wins			

cursors[WC_Nums_Eyedrop] = GetCursor(EyeDropCursor);
HLock((Handle)cursors[WC_Nums_Eyedrop]);
cursors[WC_Nums_Cross] = GetCursor(crossCursor);
HLock((Handle)cursors[WC_Nums_Cross]);
cursors[WC_Nums_Box] = GetCursor(BoxCursor);
HLock((Handle)cursors[WC_Nums_Box]);

cursors[WC_Nums_Crop] = GetCursor(CropCursor);
HLock((Handle)cursors[WC_Nums_Crop]);

cursors[WC_Nums_CropFin] = GetCursor(CropFinCursor);
HLock((Handle)cursors[WC_Nums_CropFin]);

cursors[WC_Nums_CropCancel] = GetCursor(CropCancelCursor);
HLock((Handle)cursors[WC_Nums_CropCancel]);

for (i=0; i<NumWatchCursor; i++)
	{
	cursors[WC_Nums_WatchStart+i] = GetCursor(WatchCursor+i);
	HLock((Handle)cursors[WC_Nums_WatchStart+i]);
	}
watch_num = 0;
currentCursor = WC_Arrow;

shared_data = new Shared_Data*[MaxNumWins];
num_shared_data = 0;

docs = new Document*[MaxNumDocs];
for (i=0; i<MaxNumDocs; i++) docs[i] = NULL;
num_documents = 0;
WaitingDoc = NULL;

_monacoprint = is_monaco_print();

}


AllWins::~AllWins(void)
{
int i;


removeWindows();
delete wins;

for (i=0; i<NumCursors; i++)
	{
	HUnlock((Handle)cursors[i]);
	ReleaseResource((Handle)cursors[i]);
	}
	
for (i=0; i<num_shared_data; i++) if (shared_data[i] != NULL) delete shared_data[i];	
	
delete shared_data;
if (QD3D_Present) Q3Exit();	

if (xyztorgb != NULL) delete xyztorgb;

for (i=0; i<num_documents; i++) if (docs[i] != NULL) delete docs[i];
delete docs;


FlushEvents(0xFFFFFFFF,0);
}

// Add a window and see if a documnet is waiting for a window to open
McoStatus AllWins::AddWin(baseWin *newwin,Document *doc)
{
if (numWins >= maxWins) return MCO_FAILURE;
wins[numWins] = newwin;
numWins++;

if (newwin->priority <1) newwin->priority = 1;

// A document is waiting for a window to open
if (doc != 0L) doc->addWindow(newwin);
frontWin = newwin;

findMaxPriority();

return MCO_SUCCESS;
}

// add a documnet to the list of documnets
McoStatus AllWins::addDocument(Document *doc)
{
if (num_documents == MaxNumDocs) return MCO_FAILURE;
docs[num_documents] = doc;
num_documents++;
SetMenu();
return MCO_SUCCESS;
}

// delete a documnet and remove all windows that it owns
McoStatus AllWins::removeDocument(Document *doc)
{
int i,j; 
baseWin *awin;

if (!doc->checkCloseDoc()) return MCO_SUCCESS;

for (i=0; i<num_documents; i++) if (docs[i] == doc)
	{
	// first close the document's windows
	awin = doc->findClosableWin();
	while (awin != NULL) 
		{
		removeWindow(awin);
		awin = doc->findClosableWin();
		}
	delete doc;
	break;
	}
for (j=i; j<num_documents; j++)
	{
	docs[j] = docs[j+1];
	}
docs[j] = NULL;
if (i<num_documents) num_documents--;
SetMenu();
return MCO_SUCCESS;
}

// given a window type and number, find the documnet that owns the window
Document* AllWins::FindDocFromWin(MS_WindowTypes type,int32 winNum)
{
baseWin *awin;
int i;

awin = get_window(type,winNum);
if (awin != NULL) for (i=0; i<num_documents; i++) if (docs[i]->isMyWindow(awin)) return docs[i];
return NULL;
}

// given a window, find the documnet that owns the window
Document* AllWins::FindDocFromWin(baseWin *awin)
{
int i;

if (awin != NULL) for (i=0; i<num_documents; i++) if (docs[i]->isMyWindow(awin)) return docs[i];
return NULL;
}

// Is the code for a documnet
Boolean AllWins::isCodeDocCode(Window_Events code)
{
switch (code) {
		case WE_Do_Nothing:
		case WE_OK:
		case WE_Cancel:
		case WE_Open_Window:
		case WE_Close_Window:
		case WE_Rotate:
		case WE_Scale:
		case WE_Translate:
		case WE_OpenProfileDoc:
		case WE_NewProfileDoc:
		case WE_CloseDoc:
			return FALSE;
		default:
			return TRUE;
	}
}

// is the window the main window for the document
Boolean AllWins::isDocumentMain(WindowPtr winp,Document **doc)
{
int i;
baseWin *win;

win = get_window(winp);

if (win == NULL) return FALSE;

for (i=0; i<num_documents; i++) 
	{
	if (docs[i]->isMyMainWindow(win)) 
		{
		*doc = docs[i];
		return TRUE;
		}
	}
*doc = NULL;
return FALSE;
}

#ifndef IN_POLAROID

// Create a profile and open a window
McoStatus AllWins::newProfileDoc(void)
{
ProfileDocMac *doc;

doc = new ProfileDocMac;
if (doc == NULL) return MCO_MEM_ALLOC_ERROR;
if (!doc->validData) { delete doc; return MCO_MEM_ALLOC_ERROR;}
addDocument(doc);
#ifndef IN_POLAROID
return openWindow(PrintWindow,0,0L,doc);
#else
return openWindow(TabbedDocWindow,0,0L,doc);
#endif
}


// Create a profile, open a window and load in a data file
McoStatus AllWins::openProfileDoc(void)
{
ProfileDoc *doc;
McoStatus	state;
baseWin 	*aWin;
int32		num_original_doc;

num_original_doc = num_documents;
if (num_documents == 0) 
	{
	state = newProfileDoc();
	if (state != MCO_SUCCESS) return state;
	}
doc = (ProfileDoc *)docs[num_documents-1];
if (doc == NULL) return MCO_FAILURE;
state =  doc->_handle_read_datafile();
if (state == MCO_CANCEL) 
	{
	if (num_original_doc == 0) removeDocument(doc);
	return MCO_SUCCESS;
	}
if (state != MCO_SUCCESS) 
	{
	removeDocument(doc);
	return state;
	}
aWin = doc->getMainWIndow();
if (aWin != NULL) state = aWin->updateWindowData(0);
return state;
}

// Create a profile, open a window and load in a data file
McoStatus AllWins::openProfileDoc(FSSpec *fspec)
{
ProfileDocMac *doc;
McoStatus	state;
baseWin 	*aWin;
int32		num_original_doc;

num_original_doc = num_documents;
state = newProfileDoc();
if (state != MCO_SUCCESS) return state;
doc = (ProfileDocMac *)docs[num_documents-1];
if (doc == NULL) return MCO_FAILURE;
state =  doc->_handle_read_datafile(fspec);
if (state == MCO_CANCEL) 
	{
	if (num_original_doc == 0) removeDocument(doc);
	return MCO_SUCCESS;
	}
if (state != MCO_SUCCESS) 
	{
	removeDocument(doc);
	return state;
	}
aWin = doc->getMainWIndow();
if (aWin != NULL) state = aWin->updateWindowData(0);
return state;
}
#else
// Create a profile and open a window
McoStatus AllWins::newProfileDoc(void)
{
ProfileDocMac *doc;

doc = new ProfileDocMac;
//doc->_read_resource();
if (doc == NULL) return MCO_MEM_ALLOC_ERROR;
if (!doc->validData) { delete doc; return MCO_MEM_ALLOC_ERROR;}
addDocument(doc);
#ifndef IN_POLAROID
return openWindow(PrintWindow,0,0L,doc);
#else
return openWindow(TabbedDocWindow,0,0L,doc);
#endif
}


// Create a profile, open a window and load in a data file
McoStatus AllWins::openProfileDoc(void)
{
ProfileDoc *doc;
McoStatus	state = MCO_SUCCESS;
baseWin 	*aWin;
int32		num_original_doc;
int			oldChange;

num_original_doc = num_documents;
if (num_documents == 0) 
	{
	state = newProfileDoc();
	if (state != MCO_SUCCESS) return state;
	}
doc = (ProfileDoc *)docs[num_documents-1];
if (doc == NULL) return MCO_FAILURE;
oldChange = doc->changedProfile;
state =  doc->handle_read_iccfile();
if (state == MCO_CANCEL) 
	{
	if (num_original_doc == 0) removeDocument(doc);
	doc->changedProfile = oldChange;
	return MCO_SUCCESS;
	}
if (state != MCO_SUCCESS) 
	{
	McoErrorAlert(state);
	removeDocument(doc);
	return MCO_SUCCESS;
	}
aWin = doc->getMainWIndow();
if (aWin != NULL) state = aWin->updateWindowData(0);
return state;
}

// Create a profile, open a window and load in a data file
McoStatus AllWins::openProfileDoc(FSSpec *fspec)
{
ProfileDocMac *doc;
McoStatus	state = MCO_SUCCESS;
baseWin 	*aWin;
int32		num_original_doc;
int			oldChange;

num_original_doc = num_documents;
state = newProfileDoc();
if (state != MCO_SUCCESS) return state;
doc = (ProfileDocMac *)docs[num_documents-1];
if (doc == NULL) return MCO_FAILURE;
oldChange = doc->changedProfile;
state =  doc->handle_read_iccfile(fspec);
if (state == MCO_CANCEL) 
	{
	if (num_original_doc == 0) removeDocument(doc);
	doc->changedProfile = oldChange;
	return MCO_SUCCESS;
	}
if (state != MCO_SUCCESS) 
	{
	McoErrorAlert(state);
	
	removeDocument(doc);
	return MCO_SUCCESS;
	}
aWin = doc->getMainWIndow();
if (aWin != NULL) state = aWin->updateWindowData(0);
return state;
}
#endif


// check to see if the documnet should be closed
Boolean AllWins::checkCloseDocs(Document *doc)
{
int i;

if (doc == NULL)
	{
	for (i=0; i<num_documents; i++)
		{
		if (!docs[i]->checkCloseDoc()) return FALSE;
		}
	return TRUE;
	}
if (!doc->checkCloseDoc()) return FALSE;
return TRUE;
}

// Add a window to a shared data object
McoStatus AllWins::addToShared(baseWin *win,MS_WindowTypes type)
{

int i;

for (i=0; i<num_shared_data; i++)
	{
	if (shared_data[i]->MyType(type))
		{
		shared_data[i]->AddWin(win);
		break;
		}
	}
if ( i == num_shared_data) 
	{
	shared_data[i] = NULL;
	switch (type) {
		case Gammut_Dialog1:
		case Gammut_Dialog_Src:
		case Gammut_Dialog_Dst:
			shared_data[i] = new QD3D_Shared(type);
			num_shared_data++;
			break;
		default: return MCO_BAD_PARAMETER;
		}
	if (shared_data[i] == NULL) return MCO_MEM_ALLOC_ERROR;
	shared_data[i]->AddWin(win);
	}
	
findMaxPriority();	
	
return MCO_SUCCESS;
}



// read a data file so that a gammut surface can be calculated
McoStatus handle_read_datafile(McoHandle patchH,unsigned char *patchFName);
McoStatus handle_read_datafile(McoHandle patchH,unsigned char *patchFName)
{
	McoStatus 			status;
	StandardFileReply 	sinReply;
	SFTypeList			filelist;
	Str255				inname;
	FILE 				*fs;
	short				i,j,k,m;
	double				*patch;

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

	patch = (double*)McoLockHandle(patchH);
	
//load the data into the structure
	for(i = 0; i < NUM_OLD_TOTAL_PATCHES; i++){
		j = i*3;
		fscanf(fs, "%lf \t %lf \t %lf\n", &patch[j], &patch[j+1], &patch[j+2]);
	}
	McoUnlockHandle(patchH);
	
	fclose(fs);
		
	return MCO_SUCCESS;
}




// Set the cursor to the proper state
McoStatus AllWins::setCursor(WinEve_Cursors thecursor)
{
WinEve_Cursors tc;
Cursor		curs;

tc = thecursor;
if (thecursor == WC_SetBack) tc = currentCursor;
if ((thecursor == WC_CrossTemp) && (currentCursor == WC_EyeDrop)) tc = WC_EyeDrop;
if ((thecursor == WC_CrossTemp) && (currentCursor == WC_Box)) tc = WC_Box;

switch (tc) {
	case WC_Arrow: SetCursor(&qd.arrow);
			currentCursor = WC_Arrow;
		break;
	case WC_Watch: 
			curs = **cursors[WC_Nums_WatchStart];
			SetCursor(&curs);
			currentCursor = WC_Watch;
			watch_num = 1;
		break;
	case WC_Watch_Next: 
			curs = **cursors[WC_Nums_WatchStart+watch_num];
			SetCursor(&curs);
			watch_num++;
			if (watch_num >= NumWatchCursor) watch_num = 0;
			currentCursor = WC_Watch_Next;
		break;
	case WC_EyeDrop: 
			curs = **cursors[WC_Nums_Eyedrop];
			SetCursor(&curs);
			currentCursor = WC_EyeDrop;
		break;
	case WC_Box: 
			curs = **cursors[WC_Nums_Box];
			SetCursor(&curs);
			currentCursor = WC_Box;
		break;
	case WC_CrossTemp: 
			curs = **cursors[WC_Nums_Cross];
			SetCursor(&curs);
		break;
	case WC_CropTemp: 
			curs = **cursors[WC_Nums_Crop];
			SetCursor(&curs);
		break;
	case WC_CropFinTemp: 
			curs = **cursors[WC_Nums_CropFin];
			SetCursor(&curs);
		break;
	case WC_CropCancelTemp: 
			curs = **cursors[WC_Nums_CropCancel];
			SetCursor(&curs);
		break;
	}
return MCO_SUCCESS;
}


// Match the window ptr one of the aviabile windows
Boolean AllWins::my_window(WindowPtr win)
{
int i;

for (i=0; i<numWins; i++) if (wins[i]->isMyWindowPtr(win)) return TRUE;
return FALSE;
}

// Return pointer to window class
baseWin *AllWins::get_window(WindowPtr win)
{
int i;

for (i=0; i<numWins; i++) if (wins[i]->isMyWindowPtr(win)) return wins[i];
return NULL;
}

// Return pointer to window class
baseWin *AllWins::get_window(MS_WindowTypes winType, int32 winNum)
{
int i;

for (i=0; i<numWins; i++)
	 if ((wins[i]->isMyWindowType(winType)) && (wins[i]->isMyWindowNum(winNum))) return wins[i];
return NULL;
}


// Set a window to front, put another to back and update preview if neccesary
// Use either winPtr or winType
// Open a window if it is not already opened
McoStatus  AllWins::FrontWindow(WindowPtr winPtr, MS_WindowTypes winType, int32 winNum, Boolean select,void *data,Document *doc)
{
baseWin *aWin;
Boolean Changed = FALSE;
McoStatus status = MCO_SUCCESS;

// Move front window to the back
if (frontWin != NULL) if (frontWin->isMyWindowPtr(winPtr))
	{
	// Move window to back
	Changed = frontWin->MoveToBack();
	}
// Move the window to the front
if (winPtr == NULL) // use win type and win number
	{
	aWin = get_window(winType,winNum);
	if ((aWin == NULL) || (winNum == -1)) status = openWindow(winType,winNum,data,doc);
	else 
		{
		status = aWin->MoveToFront(select);
		frontWin = aWin;
		}
	}
else
	{
	aWin = get_window(winPtr);
	if (aWin != NULL) status = aWin->MoveToFront(select);
	frontWin = aWin;
	}
if (status == MCO_SUCCESS) if (Changed) status = updateWindowData(0);
SetMenu();
return status;
}


// Find the window to move and then move it
McoStatus AllWins::MoveWindow(WindowPtr winPtr,Point where)
{
baseWin *aWin;
McoStatus status = MCO_FAILURE;

aWin = get_window(winPtr);
if (aWin != NULL) status = aWin->MoveWindow(where);
else DragWindow(winPtr, where, &qd.screenBits.bounds);
return status;
}

// find the max priority

McoStatus AllWins::findMaxPriority(void)
{
int i;

max_priority = 0;

for (i=0; i<numWins; i++) 
	if (wins[i]->priority > max_priority) max_priority = wins[i]->priority;
	
for (i=0; i<num_shared_data; i++) 
	if (shared_data[i]->priority > max_priority) max_priority = shared_data[i]->priority;
	
	
process_count = 0;
return MCO_SUCCESS;
}


// remove a specific window based on the type of window

McoStatus AllWins::removeWindow(MS_WindowTypes winType,int32 winNum)
{
int i,j;
MS_WindowTypes wint;

for (i=0; i<numWins; i++) if ((wins[i]->isMyWindowType(winType)) && (wins[i]->isMyWindowNum(winNum)))
	{
	wint = wins[i]->WinType;

	if (wins[i] == frontWin) frontWin = NULL;
	wins[i]->removeWindow();
	for (j=0; j<num_shared_data; j++) if (shared_data[j]->isMyWindow(wins[i]))
		{
		shared_data[j]->RemoveWin(wins[i]);
		}
		
	for (j=0; j<num_documents; j++)
		{
		docs[j]->removeWindow(wins[i]);
		}	
		
	delete wins[i];
	for (j=i; j<numWins; j++)
		{
		wins[j] = wins[j+1];
		}
	numWins--;
//	if (wint == SelectColor_Pallet) setWriteFlag(FALSE);
	}
findMaxPriority();
return MCO_SUCCESS;
}

// remove a specific window based on the window ptr

McoStatus AllWins::removeWindow(WindowPtr win)
{
int i,j;
MS_WindowTypes wint;

for (i=0; i<numWins; i++) if (wins[i]->isMyWindowPtr(win)) 
	{
	wint = wins[i]->WinType;

	if (wins[i] == frontWin) frontWin = NULL;
	wins[i]->removeWindow();
	for (j=0; j<num_shared_data; j++) if (shared_data[j]->isMyWindow(wins[i]))
		{
		shared_data[j]->RemoveWin(wins[i]);
		}
		
	for (j=0; j<num_documents; j++)
		{
		docs[j]->removeWindow(wins[i]);
		}		
		
	delete wins[i];
	for (j=i; j<numWins; j++)
		{
		wins[j] = wins[j+1];
		}
	//if (wint == SelectColor_Pallet) setWriteFlag(FALSE);
	numWins--;
	}
findMaxPriority();
return MCO_SUCCESS;
}

// remove a window based on the window object itself
McoStatus AllWins::removeWindow(baseWin *win)
{
int i,j;
MS_WindowTypes wint;

for (i=0; i<numWins; i++) if (wins[i] == win) 
	{
	wint = wins[i]->WinType;

	if (wins[i] == frontWin) frontWin = NULL;
	wins[i]->removeWindow();
	for (j=0; j<num_shared_data; j++) if (shared_data[j]->isMyWindow(wins[i]))
		{
		shared_data[j]->RemoveWin(wins[i]);
		}
		
	for (j=0; j<num_documents; j++)
		{
		docs[j]->removeWindow(wins[i]);
		}		
	
	delete wins[i];
	for (j=i; j<numWins; j++)
		{
		wins[j] = wins[j+1];
		}
	//if (wint == SelectColor_Pallet) setWriteFlag(FALSE);
	numWins--;
	}
findMaxPriority();
return MCO_SUCCESS;
}


// Remove all of the windows
McoStatus AllWins::removeWindows(void)
{
int i,j;

for (i=0; i<numWins; i++) 
	{
	wins[i]->removeWindow();
	for (j=0; j<num_shared_data; j++) if (shared_data[j]->isMyWindow(wins[i]))
		{
		shared_data[j]->RemoveWin(wins[i]);
		}
		
	for (j=0; j<num_documents; j++)
		{
		docs[j]->removeWindow(wins[i]);
		}	
	delete wins[i];
	wins[i] = NULL;
	}
frontWin = NULL;
numWins = 0;
findMaxPriority();
return MCO_SUCCESS;
}



// Update the data that is within the windows

McoStatus AllWins::updateWindowData(int changed)
{
McoStatus state = MCO_SUCCESS;
int i;

//McoShowMem("\p At update",TRUE);
//setWriteFlag(TRUE);
for (i=0; i<numWins; i++) 
	{
	if (wins[i] != NULL) state = wins[i]->updateWindowData(1);
	else state = MCO_FAILURE;
	if (state != MCO_SUCCESS) return state;
	}
//setWriteFlag(FALSE);
return state;
}

// Update windows that need to always be updated
McoStatus AllWins::DoAlwaysUpdate(Boolean &Changed)
{
McoStatus state = MCO_SUCCESS;
int i;

for (i=0; i<numWins; i++) 
	if ((wins[i]->alwaysUpdate()) && 
		(process_count % (max_priority/wins[i]->priority) == 0))
	{
	state = wins[i]->updateWindowData(0);
	if (state != MCO_SUCCESS) return state;
	}
for (i=0; i<num_shared_data; i++)
	if (process_count % (max_priority/shared_data[i]->priority) == 0)
	{
	state = shared_data[i]->DoAlwaysUpdate(frontWin);
	if (state != MCO_SUCCESS) return state;
	}
// Check to see if data in another window needs to be updated
for (i=0; i<numWins; i++) Changed = wins[i]->checkChanged() | Changed;

// see if process_count is too large
process_count++;
if (process_count > 10000000) process_count = 0;
return state;
}

// see if a window has a timer event
Boolean AllWins::TimerEventPresent(baseWin **win)
{
int i;

for (i=0; i<numWins; i++)
	{
	if (wins[i]->TimerEventPresent()) 
		{
		*win = wins[i];
		return TRUE;
		}
	}
return FALSE;
}

// do nothing
void ProcessEvent(EventRecord *event)
{
;
}

// Do a dialog event

McoStatus AllWins::DoDialogEvent(EventRecord *theEvent,int16 *flag,Boolean &finish,Boolean &Changed, Boolean TimerEvent)
{
int	i,j;
McoStatus state;
baseWin *aWin = NULL;
DialogPtr 	hitdp;
short		item = 0;
Window_Events code;
MS_WindowTypes winType;
int32		winNum;

Handle		dataHandle;
void		*data;
Boolean		MyObject = FALSE;
WindowCode	wcode[100];
long		num_codes;

// check for sliders
if (theEvent->what == mouseDown) if (FindWindow(theEvent->where, &hitdp) == inContent)
	{
	aWin = get_window(hitdp);
	if ((aWin != NULL) && (aWin == frontWin)) MyObject = aWin->isMyObject(theEvent->where,&item);
	}
if ((!MyObject) && (!TimerEvent)) DialogSelect(theEvent,&hitdp,&item);
// if the event is a keyboard event then assume event coresponds to the front window
if (theEvent->what == keyDown) aWin = frontWin;
else if (TimerEvent) 
	{
	TimerEventPresent(&aWin);
	item = TIMER_ITEM;
	}
else aWin = get_window(hitdp);
if (aWin != NULL)
	{
		
	aWin->KeyFilter(theEvent,&item);
	state = aWin->DoEvents(item,theEvent->where,wcode,&num_codes,&data,Changed);
	if (state != MCO_SUCCESS)  { finish = TRUE; return state;}
	return DoCommand(wcode,num_codes,theEvent->where,data,&finish,&Changed);		
	}
finish = FALSE;
return MCO_SUCCESS;	
}

// Process a command for a document
McoStatus AllWins::DoDocumentCommand(WindowCode *wc,WindowCode *wc2,Document *doc,void **data,Boolean *finish,Boolean *changed)
{
Boolean	 WaitDoc;
McoStatus	state;
//first have to find document responsible for the command

if (doc == NULL) doc = FindDocFromWin(wc[0].wintype,wc[0].winnum);
if (doc == NULL)
	{
	wc2[0].code = WE_Do_Nothing;
	return MCO_NOT_IMPLEMENTED;
	}
state = doc->doCommand(wc,wc2,data,changed);
return state;
}


// Process a command
McoStatus AllWins::DoCommand(WindowCode *wc,int num_codes,Point clickPoint,void *data,Boolean *finish,Boolean *changed)
{
McoStatus	state;
int			i,j;
Window_Events	code;		// the code 
MS_WindowTypes 	winType;	// the type of window that the code applies to
int32		  	winNum;		// the number of the window
Document 		*doc;

for (j=0; j<num_codes; j++)
	{
	code = wc[j].code;
	winType = wc[j].wintype;
	winNum = wc[j].winnum;
	doc = wc[j].doc;
	if (code != WE_Do_Nothing)
		{
		if (isCodeDocCode(code)) 
			{
			DoDocumentCommand(&wc[j],&wc[num_codes],doc,&data,finish,changed);
			num_codes++;
			}
		else {
			switch (code) {
				case WE_OK:
				case WE_Cancel:
					if (checkCloseDocs(NULL))
						{
						state = removeWindows();
						*finish = TRUE;
						return state;
						}
				case WE_Open_Window:
					state = FrontWindow(NULL,winType,winNum,TRUE,data,doc);
					if (state != MCO_SUCCESS)  { *finish = TRUE; return state;}
					*changed = TRUE;
					break;
				case WE_Close_Window:
					state = removeWindow(winType,winNum);
					if (state != MCO_SUCCESS)  { *finish = TRUE; return state;}
					*changed = TRUE;
					break;
				case WE_Rotate:
				case WE_Scale:
				case WE_Translate:
					for (i=0; i<num_shared_data; i++) if (shared_data[i]->isMyWindow(winType,winNum))
						{
						shared_data[i]->ProcessMouseDown(clickPoint,code,frontWin);
						}
					break;
				case WE_OpenProfileDoc:
					state = openProfileDoc();
					if (state != MCO_SUCCESS)  { *finish = TRUE; return state;}
					break;
				case WE_NewProfileDoc:
					state = newProfileDoc();
					if (state != MCO_SUCCESS)  { *finish = TRUE; return state;}
					break;
				case WE_CloseDoc:
					if (doc) removeDocument(doc);
					else
						{
						doc = FindDocFromWin(winType,winNum);
						if (doc != NULL) if (checkCloseDocs(doc)) removeDocument(doc);
						}
					break;
				}
			code = WE_Do_Nothing;
			}
		}
	}
return MCO_SUCCESS;
}




Boolean AllWins::DoKeyEvent(EventRecord *theEvent,Boolean *finish,Boolean *changed)
{
int32 menu_choice = 0;
char  theKey;
Boolean didit = FALSE;

if (theEvent->modifiers & 0x0100)
	{
	theKey = theEvent->message & charCodeMask;
	menu_choice = MenuKey( theKey );
	if ( HiWord( menu_choice) != 0 )	/* valid key ? */
		{
		Handle_menu(menu_choice,finish,changed);
		didit = TRUE;
		}
	HiliteMenu( 0 );
	}
return didit;
}

// find the front window and get the region from it 
McoStatus AllWins::getEventRegion(RgnHandle	fullRgn, RgnHandle tempRgn,RgnHandle mouseRgn)
{
CopyRgn(fullRgn,mouseRgn);
if (frontWin != NULL) return frontWin->makeEventRegion(tempRgn,mouseRgn);
return MCO_SUCCESS;
}




// The main event loop
McoStatus AllWins::AllEvents(void)
{
short 		itemType;
short		item = 0;
Handle 		h;
Rect 		r,r1,r2;

EventRecord theEvent;
Boolean		eventOccured;

McoStatus state;
RgnHandle	fullRgn;
RgnHandle	previewRgn;
RgnHandle	mouseRgn;

baseWin		*frontBaseWin;

GrafPtr			oldGraf;

short		the_part;
WindowPtr	which_window;
WindowPtr	oldPort;
McoStatus 	status;
Boolean		Changed = FALSE;
long	   	menu_choice;


int			saveastest = FALSE;
int			saveastest2 = FALSE;
Boolean 	finish = FALSE;
baseWin 	*aWin;
WinEve_Cursors	cursortype;
int16		flag;
Point			aPoint = {100, 100};
Boolean		TimerEvent;
Document 	*doc;
WindowCode	wc[10];
int		nwcode;
Point		where;
	
fullRgn = NewRgn();
previewRgn = NewRgn();
mouseRgn = NewRgn();

RectRgn(fullRgn,&qd.screenBits.bounds);

SetMenu();

while (!finish)
	{
	// Get active region of front window
	//getEventRegion(mouseRgn,previewRgn,mouseRgn);
	// Get the next event
	eventOccured = WaitNextEvent(everyEvent,&theEvent,0,mouseRgn);
	//eventOccured = WaitNextEvent(everyEvent,&theEvent,0,nil);
	// See if point is in active part of front window
	if (frontWin != NULL) 
		{
		cursortype = WC_Arrow;
		state = frontWin->doPointInWindowActiveRgn(theEvent.where,cursortype);
		if (state != MCO_SUCCESS) goto finish;
		setCursor(cursortype);
		} 
	// Get window and part that was hit
	TimerEvent = FALSE;
	if (!(eventOccured)) 
		{ // check for a window timer event
		TimerEvent = TimerEventPresent(&aWin);
		if (TimerEvent)
			{
			state = DoDialogEvent(&theEvent,&flag,finish,Changed,TimerEvent);
			if ((finish) || (state != MCO_SUCCESS)) goto finish;
			}
		// check to see if a window needs to be updated
		state = DoAlwaysUpdate(Changed);
		if (state != MCO_SUCCESS) goto finish;
		}
	else 
	if (eventOccured)
		{
		switch (theEvent.what) {
			case keyDown:
				if (!DoKeyEvent(&theEvent,&finish,&Changed))
					state = DoDialogEvent(&theEvent,&flag,finish,Changed,TimerEvent);
				if ((finish) || (state != MCO_SUCCESS)) goto finish;
				break;
			case mouseDown:
				
		 		the_part = FindWindow(theEvent.where, &which_window);
		 		if (IsDialogEvent(&theEvent)) 
		 			{
		 			state = DoDialogEvent(&theEvent,&flag,finish,Changed,TimerEvent);
		 			if ((finish) || (state != MCO_SUCCESS)) goto finish;
		 			}
				else switch(the_part)
					{
				   case inSysWindow:
						SystemClick (&theEvent, which_window);
						break;
				   case inMenuBar:
						//adjustmenus();	// do I need to do this?
			        	menu_choice = MenuSelect(theEvent.where);
			        	state = Handle_menu(menu_choice,&finish,&Changed);
			        	if ((finish) || (state != MCO_SUCCESS)) goto finish;
			        	HiliteMenu(0);
			        	break;
	 			   case inContent:
	        			if( which_window != getFrontWin())
	        				{
	        				FrontWindow(which_window,No_Dialog,-1,TRUE,0L,0L);
	        				DrawControls(which_window);
	        				}     	
	    				break;					
					case inDrag:
						MoveWindow(which_window,theEvent.where);
						break;
					case inGoAway:
						// first check to see if it is document's main window
						if (isDocumentMain(which_window,&doc))  
							{
							//state = removeDocument(doc);
							nwcode = 1;
							wc[0].code = WE_Doc_Close_Save;
							wc[0].wintype = No_Dialog;
							wc[0].winnum = 0;
							wc[0].doc = doc;
							state = DoCommand(wc,nwcode,where,NULL,&finish,&Changed);
							if (state != MCO_SUCCESS) McoErrorAlert(state);
							}
						else state = removeWindow(which_window);			
						break;
					default:
						ProcessEvent(&theEvent);
					}
				break;
			case updateEvt:
				aWin = get_window((WindowPtr)theEvent.message);
				if (aWin != NULL) aWin->UpdateWindow();
				else if (IsDialogEvent(&theEvent)) 
					{
					which_window = (WindowPtr)theEvent.message;
					GetPort(&oldPort);
					SetPort( which_window ) ;
					BeginUpdate( which_window );
					DrawDialog( which_window );
					EndUpdate( which_window );
					SetPort(oldPort);
					break ;
					}
				break;
			case diskEvt:
				if ( HiWord(theEvent.message) != noErr ) 
					(void) DIBadMount(aPoint, theEvent.message);
				break;
			case activateEvt:
				aWin = get_window((WindowPtr)theEvent.message);
				//if (aWin != NULL) aWin->UpdateWindow();
				if (aWin != NULL)
					{
					frontWin = aWin;
					SetMenu();
					}
			case kHighLevelEvent:
				AEProcessAppleEvent(&theEvent);
				if (DoQuit) finish = TRUE;
				break;
			}
		}
	if (Changed) 
		{
		state = updateWindowData(1);
		Changed = FALSE;
		if (state != MCO_SUCCESS) goto finish;
		SetMenu();
		}
	}
finish:
//We never get here but put this in just to remind ourselves	
DisposeRgn(fullRgn);
DisposeRgn(previewRgn);
DisposeRgn(mouseRgn);
if (state != MCO_SUCCESS) McoErrorAlert(state);
return state;						
}							


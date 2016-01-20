//////////////////////////////////////////////////////////////////////////////
//  printdocwindow.cpp														//
//	The main dialog that has tabs											//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#include "printdocwindow.h"
#include "profiledocmac.h"
#include "calibtab.h"
#include "tonetab.h"
#include "datatab.h"
#include "profiletab.h"
#include "tweaktab.h"
#include "pascal_string.h"
#include "blanktwtab.h"
#include "blanktab.h"

#include "think_bugs.h"
#include "fullname.h"

#define PD_DIAL_ID 		6000
#define PD_NUM_TABS 	5
#define PD_TAB_RECTS 	{2,3,4,5,6}
#define PD_MAX_DITL   	9
#define PD_TOP_BAR		8
#define PD_TOP_BAR_PICT	140
#define PD_TL_CORNER	14
#define PD_TL_CORNER_P	143
#define PD_TR_CORNER	12
#define PD_TR_CORNER_P	141
#define PD_LR_AT		152
#define PD_LL_AT		153
#define PD_LR_RIGHT		154
#define PD_LL_LEFT		155

#define PD_OFF_TAB_START	3000
#define PD_DIS_TAB_START	3010
#define PD_ON_TAB_START		3005


PrintDocWindow::PrintDocWindow(ProfileDoc *dc,int winnum)
{
	short	tab_rects[] = PD_TAB_RECTS;
	int i;
	doc = dc;

	short	iType;
	Handle	iHandle;
	Rect	iRect;


setDialog(PD_DIAL_ID);

priority = 1;
WinType = TabbedDocWindow;
WinNum = winnum;

doingProcess = 0;

numTabs = PD_NUM_TABS;
tabRects = new Rect[numTabs];

enabledTab = new int[numTabs];

for (i=0; i<numTabs; i++)
	{
	GetDItem (dp, tab_rects[i], &iType, (Handle*)&iHandle, &tabRects[i]);
	enabledTab[i] = 1;
	}

// make first tab active
activeTab = 2;
lastactiveTab = 2;
activeWindow = 0L;

// allocate space for the windows
windows = new WindowTab*[numTabs];
for (i=0; i<numTabs; i++) windows[i] = 0L;

// get default data
loadLastPrinter();

setParameters();

// now open the tab

openTab();

updateWindowData(0);
}

PrintDocWindow::~PrintDocWindow(void)
{
if (tabRects) delete tabRects;
if (enabledTab) delete enabledTab;
}


McoStatus PrintDocWindow::loadLastPrinter(void)
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
return printers.Load(&fspec);
}


McoStatus PrintDocWindow::saveLastPrinter(void)
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
return printers.Save(&fspec);
}


McoStatus PrintDocWindow::setParameters(void)
{
	
	
	doc->_condata = printers.data;
	doc->_condata.tabletype = 5;
	
	return printers.setblack(doc->_blacktableH);

}

// copy data into the settings
McoStatus PrintDocWindow::getParameters(void)
{

	printers.data = doc->_condata;
	
		
return MCO_SUCCESS;	
}




// open the tab
McoStatus PrintDocWindow::openTab(void)
{
if (activeWindow) activeWindow->hideWindow();

if ((activeTab >= numTabs) || (activeTab < 0)) return MCO_OUT_OF_RANGE;
activeWindow = 0L;
if (!windows[activeTab])
	{
	if (doc->processing) activeWindow = new BlankTab(doc,dp);
	else switch (activeTab) {
	 	case 0:activeWindow = new CalibTab(doc,dp);
			   break;
		case 1:activeWindow = new ToneTab(doc,dp);
			   break;
		case 2:activeWindow = new DataTab(doc,dp);
			   break;
		case 3:activeWindow = new ProfileTab(doc,dp,WinNum);
			   break;
		case 4:
				if (!doc->didGammutCompression) 
					activeWindow = new BlankTwTab(doc,dp);
				else
					activeWindow = new TweakTab(doc,dp);
			   break; 
		}
	 windows[activeTab] = activeWindow;
	 }
else
	{
	activeWindow = windows[activeTab];
	activeWindow->showWindow();
	}
if (activeWindow == 0L) return MCO_FAILURE;
return MCO_SUCCESS;
}


McoStatus PrintDocWindow::updateWindowData(int change)
{
int	i,n;
short	iType;
Handle	iHandle;
Rect	iRect;
WindowPtr	oldp;
Str255	wintitle;
McoStatus status = MCO_SUCCESS;
int		changed = 0;
	
GetPort(&oldp);
SetPort(dp);	
	
if (doc->processing)
	{
	if (enabledTab[0]) changed = 1;
	if (enabledTab[1]) changed = 1;
	if (enabledTab[2]) changed = 1;
	if (enabledTab[3]) changed = 1;
	if (enabledTab[4]) changed = 1;

	enabledTab[0] = 0;
	enabledTab[1] = 0;
	enabledTab[2] = 0;
	enabledTab[3] = 0;
	enabledTab[4] = 0;
	}
else
	{
	if ((doc->_monacoprint) && (doc->monacoProfile) && (!doc->monacoEdited))
		{
		if (!enabledTab[2]) changed = 1;
		enabledTab[2] = 1;
		}
	else
		{
		if (enabledTab[2]) changed = 1;
		enabledTab[2] = 0;
		}
		
	if (doc->locked)
		{
		
		//Disable(dp,PD_BuildProfile);
		}
	else if ((doc->patchD->type == No_Patches) || (doc->simupatchD->type == No_Patches))
		{
		if (enabledTab[3]) changed = 1;
		if (enabledTab[4]) changed = 1;
		enabledTab[3] = 0;
		enabledTab[4] = 0;
		
		if (doc->createdProfile)
			{
			if (enabledTab[0]) changed = 1;
			if (enabledTab[1]) changed = 1;
			enabledTab[0] = 0;
			enabledTab[1] = 0;
			}		
		else
			{
			if (!enabledTab[0]) changed = 1;
			if (!enabledTab[1]) changed = 1;
			enabledTab[0] = 1;
			enabledTab[1] = 1;
			}			
		//Disable(dp,PD_BuildProfile);
		}
	else
		{
		if (!enabledTab[0]) changed = 1;
		if (!enabledTab[1]) changed = 1;
		enabledTab[0] = 1;
		enabledTab[1] = 1;
		if ((doc->inputStatus() == No_Input) && (doc->_monacoprint) && (doc->validCal()))
			{
			if (!enabledTab[3]) changed = 1;
			if (!enabledTab[4]) changed = 1;
			enabledTab[3] = 1;
			enabledTab[4] = 1;
		//	Enable(dp,PD_BuildProfile);
			}
		else 
			{
			if (enabledTab[3]) changed = 1;
			if (enabledTab[4]) changed = 1;

			enabledTab[3] = 0;
			enabledTab[4] = 0;
			//Disable(dp,PD_BuildProfile);
			}
		}

	if (enabledTab[activeTab] == 0) 
		{
		for (i=0; i<numTabs; i++) if (enabledTab[i]) break;
		if (i<numTabs) 
			{
			lastactiveTab = activeTab;
			activeTab = i;
			openTab();
			}
		}
	}	
	
if (changed) DrawTabs();

// set the window title
if (((ProfileDocMac*)doc)->valid_fsspec) 
	{
	GetWTitle(dp,wintitle);
	if (!cmp_str(wintitle,((ProfileDocMac*)doc)->patchFName)) 
		SetWTitle( dp, ((ProfileDocMac*)doc)->patchFName);
	}
SetPort(oldp);
if (activeWindow) status = activeWindow->updateWindowData(change);
if (status != MCO_SUCCESS) return status;
if (activeWindow) if (n = activeWindow->killTab())
	{
	activeWindow->hideWindow();
	delete activeWindow;
	activeWindow = 0L;
	windows[activeTab] = 0L;
	if (n == 2) activeTab = lastactiveTab;
	openTab();
	}
	
if (activeWindow)
	{	
	if ((activeWindow->WinType != BlankWinTab) && (doc->processing))
		{
		activeWindow->hideWindow();
		delete activeWindow;
		activeWindow = 0L;
		windows[activeTab] = 0L;
		openTab();
		}
	else if ((activeWindow->WinType == BlankWinTab) && (!doc->processing))
		{
		activeWindow->hideWindow();
		delete activeWindow;
		activeWindow = 0L;
		windows[activeTab] = 0L;
		openTab();
		}
	}
	
return status;
}


McoStatus PrintDocWindow::DrawTabs()
{
PicHandle	pict;
Rect 	gr,r,r1;
RGBColor	old,newf,oldb,newb;
int i;
short	iType;
Handle	iHandle;
QDErr		error;
GDHandle	oldGD;
GWorldPtr	oldGW;
GWorldPtr	BigGW;	
PixMapHandle myPixMapHandle;


GetDItem (dp, PD_TOP_BAR, &iType, (Handle*)&iHandle, &r1);


gr = tabRects[0];

gr.bottom = r1.bottom;
gr.right = tabRects[4].right;

GetBackColor(&oldb);
newb.red = 65535;
newb.green = 65535;
newb.blue = 65535;
RGBBackColor(&newb);
error = NewGWorld( &BigGW, 32, &gr, 0, 0, 0 );	// create the gworld
if (error == 0)
	{
	GetGWorld(&oldGW,&oldGD);
	SetGWorld(BigGW,nil);
	myPixMapHandle = GetGWorldPixMap( BigGW );	// 7.0 only 
	LockPixels(myPixMapHandle);
	pict = GetPicture(PD_TOP_BAR_PICT);
	if (pict != 0L) 
		{
		DrawPicture(pict,&r1);
		ReleaseResource((Handle)pict);
		}
		

	for (i=0; i<numTabs; i++)
		{
		if (i != activeTab)
			{
			if (enabledTab[i])
				{
				pict = GetPicture(PD_OFF_TAB_START+i);
				if (pict != 0L) 
					{
					DrawPicture(pict,&tabRects[i]);
					ReleaseResource((Handle)pict);
					}
				}
			else
				{
				pict = GetPicture(PD_DIS_TAB_START+i);
				if (pict != 0L) 
					{
					DrawPicture(pict,&tabRects[i]);
					ReleaseResource((Handle)pict);
					}

				}
			}
		else
			{
			pict = GetPicture(PD_ON_TAB_START+i);
			if (pict != 0L) 
				{
				DrawPicture(pict,&tabRects[i]);
				ReleaseResource((Handle)pict);
				}
			GetForeColor(&old);
			newf.red = 57000;
			newf.green = 57000;
			newf.blue = 57000;
			RGBForeColor(&newf);
			r = tabRects[i];
			//r.left +=3;
			//r.right -=2;
			r.top = r.bottom;
			r.bottom += 5;
			PaintRect(&r);
			RGBForeColor(&old);
			}
			
			
		}
		
	pict = GetPicture(PD_TL_CORNER_P);
	GetDItem (dp, PD_TL_CORNER, &iType, (Handle*)&iHandle, &r1);
	if (pict != 0L) 
		{
		DrawPicture(pict,&r1);
		ReleaseResource((Handle)pict);
		}
		
	pict = GetPicture(PD_TR_CORNER_P);
	GetDItem (dp, PD_TR_CORNER, &iType, (Handle*)&iHandle, &r1);
	if (pict != 0L) 
		{
		DrawPicture(pict,&r1);
		ReleaseResource((Handle)pict);
		}			
		
	if (activeTab == 0)
		{
		r = tabRects[0];
		r.top = r.bottom;
		r.bottom += 5;
		r.right = r.left+5;
		pict = GetPicture(PD_LL_LEFT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}	
	
		r = tabRects[0];
		r.top = r.bottom;
		r.bottom += 5;
		r.left = r.right-5;	
		pict = GetPicture(PD_LR_AT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}						
		}	
	else if (activeTab == 	numTabs-1)
		{
		r = tabRects[activeTab];
		r.top = r.bottom;
		r.bottom += 5;
		r.right = r.left+5;
		pict = GetPicture(PD_LL_AT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}	
	
		r = tabRects[activeTab];
		r.top = r.bottom;
		r.bottom += 5;
		r.left = r.right-5;	
		pict = GetPicture(PD_LR_RIGHT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}						
		}	
	else
		{
		r = tabRects[activeTab];
		r.top = r.bottom;
		r.bottom += 5;
		r.right = r.left+5;
		pict = GetPicture(PD_LL_AT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}	
	
		r = tabRects[activeTab];
		r.top = r.bottom;
		r.bottom += 5;
		r.left = r.right-5;	
		pict = GetPicture(PD_LR_AT);
		if (pict != 0L) 
			{
			DrawPicture(pict,&r);
			ReleaseResource((Handle)pict);
			}						
		}	
				
	SetGWorld(oldGW,oldGD);	
	// draw onto the screen
	CopyBits( (BitMap*)*myPixMapHandle, (BitMap*)&dp->portBits, &gr, &gr, srcCopy, 0 );
	UnlockPixels(myPixMapHandle);
	DisposeGWorld( BigGW );
	}
RGBBackColor(&oldb);
return MCO_SUCCESS;
}

McoStatus PrintDocWindow::UpdateWindow(void)	
{			
	Rect updateRect;
	WindowPtr	oldP;
	
	GetPort(&oldP);
	
	// update the window data	
	updateRect = (**(dp->visRgn)).rgnBBox;
	SetPort( dp ) ;
	BeginUpdate( dp );
	DrawDialog( dp );
	EndUpdate( dp );
	if (activeWindow) activeWindow->UpdateWindow();
	DrawTabs();
	
	SetPort(oldP);
	return MCO_SUCCESS;
}		

Boolean PrintDocWindow::isMyObject(Point where,short *item) 
{
if (activeWindow) return activeWindow->isMyObject(where,item);
return 0;
}


Boolean PrintDocWindow::TimerEventPresent(void)
{
if (activeWindow) return activeWindow->TimerEventPresent();
return 0;
}

McoStatus PrintDocWindow::makeEventRegion(RgnHandle previewRgn,RgnHandle mouseRgn)
{
if (activeWindow) return activeWindow->makeEventRegion(previewRgn,mouseRgn);
return MCO_SUCCESS;
}

McoStatus PrintDocWindow::doPointInWindowActiveRgn(Point where,WinEve_Cursors &cursortype)
{
if (activeWindow) return activeWindow->doPointInWindowActiveRgn(where,cursortype);
return MCO_SUCCESS;
}

// handle keyboard events
Boolean 	PrintDocWindow::KeyFilter(EventRecord *theEvent,short *itemHit )
{
if (activeWindow) return activeWindow->KeyFilter(theEvent,itemHit);
return( FALSE );	// all others
}

// The event handler for the main menu
McoStatus	PrintDocWindow::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data,Boolean &changed)
{
	McoStatus	status;
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	short		tab_rects[] = PD_TAB_RECTS;
	int 		i;
	
	// no codes passed back
	*numwc = 0;
	
	if (((item > PD_MAX_DITL) || (item == -1)) && (activeWindow))
		return activeWindow->DoEvents(item,clickPoint,wc,numwc,data,changed);

	else
		{
		for (i=0; i<numTabs; i++) if (item == tab_rects[i])
			{
			if ((activeTab != i) && (enabledTab[i]))
				{
				lastactiveTab = activeTab;
				activeTab = i;
				status = openTab();
				DrawTabs();
				}
			}
		}
return MCO_SUCCESS;
}

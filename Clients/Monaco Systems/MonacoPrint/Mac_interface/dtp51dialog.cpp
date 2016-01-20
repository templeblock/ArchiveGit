////////////////////////////////////////////////////////////////////////////////////////////////
//	The DTP51 input dialog																	  //
//  inputdialog2.c																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////



#include "dtp51dialog.h"
#include "monacoprint.h"
#include "think_bugs.h"
#include "fullname.h"
#include "mcofile.h"
#include "pascal_string.h"
#include "colortran.h"
//#include "globalValues.h"
#include "recipes.h"

#include "pascal_string.h"
#include "mcocolsyn.h"
#include "errordial.h"
#include <math.h>


#define RGB_PATCHES_8_11 "\pMcoPatchesRGB8-11"
#define RGB_PATCHES_11_17 "\pMcoPatchesRGB11-17"
#define RGB_PATCHES_RGB "\pMcoPatchesRGB8-11-RGB"



#define Input_Dialog		(2015)

#define	DTP51_IDS { 	2,4,5,20,6,7,10,15,16,21,17,-1,19,400,401,21,22,23,24}
/*		2,	\	// Ok_Box
		4,  \	// Save As
		5,  \	// Strip_rect
		8,  \	// Message
		6,  \	// current sheet
		7,  \	// current strip
		10,  \	// patch rect
		15,  \	// new patch rect
		16,  \	// slider
		-1,  \	// redo
		17,  \	// start
		-1,  \	// voice set
		19,  \	// threeDBox
		400,  \	// updateevent
		401,  \	// TimeEvent
		21,  \	// up
		22,  \	// down
		23,  \  // left
		24 	// right
*/

#define Message_List_ID		(1000)
#define Button_Labels (17010)



#define ClickNum 			(50)
#define EndColumnNum		(51)




DTP51Dialog::DTP51Dialog(ProfileDoc *dc,RawData *pD):InputDialog(dc,pD,1)
{
	short		iType;
	Handle		iHandle;
	Handle		sHandle;
	Rect 		iRect, r,pRect,tRect;	
	UserItemUPP  box3D;
	
	QDErr		error;
	GDHandle	oldGD;
	GWorldPtr	oldGW;
	RGBColor	c,oldbackcolor,oldforecolor;
	WindowPtr	oldP;
	OSErr			err;
	int32 		i,wi,hi;
	double w,h;
	short		dtp_ids[] = DTP51_IDS;
	McoStatus  state;
	
	for (i=0; i<NumInputIDS; i++) ids[i] = dtp_ids[i];

	WinType = StripDialog;
	WinNum = 0;

	setDialog(Input_Dialog);
	frame_button(ids[Ok_Box]);	

	
	HideDItem(dp,ids[Start]);
	
	// Added by James, 3D the box
	threeD_box(ids[ThreeDBox]);
	
	Scramble = !(isPressed(0x3a));	
	
	if (Scramble)
		{
		voice_set = 2;
		}	
	else
		{
		Disable(dp,ids[VoiceSet]);
		voice_set = 2;
		}

	//playSound(8193);
	
	// initialize comminications with the DTP51
	if (patchD->patches.rows == 27) 
		{ 
		// initialize comminications with the DTP51
		state = doc->openInputDevice(2,0,0);
		}
	else  state = doc->openInputDevice(1,0,0);
	if (state != MCO_SUCCESS) McoErrorAlert(state);
	
	// initialize the big gworld 
	GetDItem (dp, ids[Strip_Rect], &iType, (Handle*)&iHandle, &iRect);
	tRect = iRect;
	OffsetRect(&tRect,-iRect.left,-iRect.top);
	error = NewGWorld( &BigGW, 32, &tRect, 0, 0, 0 );
	if (error != 0) 
		{
		delete this;
		return;
		} 
	GetGWorld(&oldGW,&oldGD);
	SetGWorld(BigGW,nil);	
	
	
//	GetBackColor(&oldbackcolor);
	GetForeColor(&oldforecolor);
	c.red = 65535;
	c.green = 65535;
	c.blue = 65535;
	RGBBackColor(&c);
	c.red = 0;
	c.green = 0;
	c.blue = 0;
	RGBForeColor(&c);		
//	EraseRect( &tRect );
	
	SetGWorld(oldGW,oldGD);	
//	RGBBackColor(&oldbackcolor);
	RGBForeColor(&oldforecolor);	

//	SetPort(oldP);
		
	GetDItem (dp, ids[Patch_Rect], &iType, (Handle*)&iHandle, &pRect);
	num_display_patch = (iRect.bottom-iRect.top)/(pRect.bottom-pRect.top);
	top_patch = 0;
						
//init the static members
	//current_patch = total_patches-1;
	//current_patch = 0;
	current_patch = 0;
	current_strip = 0;
	current_sheet = 0;
 		
 	_done = 0;
 	_waiting = 0;
	
	SetSheetStripText();
	
	current_top = current_patch - 2*num_display_patch/3;
	current_bottom = current_top + num_display_patch - 1;
	if (current_top < 0) 
		{
		current_top = 0;
		current_bottom = current_top + num_display_patch - 1;
		}
	if (current_bottom >= total_patches-1)
		{
		current_bottom = total_patches - 1;
		current_top = current_bottom - num_display_patch + 1;
		}
	
	current_disp = 2*num_display_patch/3;
	
	init();	
	getpatchRGB();
	
	
	Disable(dp,ids[Redo]);

	
	GetDItem (dp, ids[Slider], &iType, (Handle*)&iHandle, &pRect);
	SetCtlMin((ControlHandle)iHandle,0);
	SetCtlMax((ControlHandle)iHandle,total_patches-1);
	
	GetDItem(dp, ids[VoiceSet], &iType, (Handle*)&iHandle, &pRect);
	SetCtlValue((ControlHandle)iHandle,1);
	//voice_set = 1;
	_waiting = 1;	
	
	checkFinished();
	DrawWindow();
}	

DTP51Dialog::~DTP51Dialog(void)
{
OSErr			err;

	if (doc != 0L) doc->closeInputDevice();
}


McoStatus DTP51Dialog::SetSheetStripText(void)
{
	Rect r;
	short itemType;
	Handle cn1;
	Str255 theString;
	
	
GetDItem ( dp,ids[Cur_sheet], &itemType, (Handle*)&cn1, &r );
NumToString(current_sheet+1,theString);
SetIText(cn1,theString);
GetDItem ( dp,ids[Cur_strip], &itemType, (Handle*)&cn1, &r );
NumToString(current_strip+1,theString);
SetIText(cn1,theString);
return MCO_SUCCESS;
}
				



McoStatus DTP51Dialog::readInPatches(void)
{
int i,count;
short		item;	
short		itemtype;
Handle		cn1,sliderControl;
Rect		r;
McoStatus state;
int32		start_patch;

current_patch = current_patch/patch_per_strip;
current_patch *= patch_per_strip;
start_patch = current_patch;

for (i=0; i<patch_per_strip; i++)
	{
	count = 0;
	do {
		if (doc->tcom != NULL) state = doc->tcom->getNextPatch(&patch_data[current_patch*3]);
		count ++;
		} while ((state != MCO_SUCCESS) && (count < 5));
	if (state != MCO_SUCCESS) 
		{
		//xcom->TurnOnStripRead();
		return state;
		}
	if (doc->patchtype == No_Patches) doc->patchtype = patchtype;
	GetDItem ( dp, ids[Slider], &itemtype, (Handle*)&sliderControl, &r );
	SetCtlValue ( (ControlHandle)sliderControl,current_patch);
	ShowPatch(current_patch);
	current_patch++;
	}
// check for error
checkError(start_patch);
if (current_patch >= total_patches) 
	{
	current_patch = total_patches -1;
	_waiting = FALSE;
	}
ShowPatch(current_patch);
current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;
SetSheetStripText();
//xcom->TurnOnStripRead();
return MCO_SUCCESS;
}





int32 DTP51Dialog::BlackPatch(int32 i)
{
if  (i % patch_per_strip) return 0;
return 1;
}



McoStatus DTP51Dialog::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
{
UserItemUPP myfilter;
Str255		theString;
short		itemtype;
Handle		cn1,sliderControl;
Rect		r;
short		part;
Point		mpoint;
int32		next_patch;
McoStatus	status;
WindowPtr	oldp;
int32		temp;
double 		m=1;

	GetPort(&oldp);
	SetPort(dp);

	Changed = FALSE;
	*numwc = 0;
	status = MCO_SUCCESS;

	if (isPressed(0x38)) m = 10;

	if (item != -1) {
		if ((item == TIMER_ITEM ) && (doc->tcom)) // a timer event has occured
			{   
			_last_tick_time2 = TickCount();
			if (_waiting)
				{
				if (!doc->tcom->IsBufferEmpty())	// has a strip been read in
					{
					changed = TRUE; 
					// check to see if a strip has been read in 
					if (doc->tcom->IsDataPresent()) 
						{
						doc->needsSave = TRUE;
						GetDItem ( dp,ids[Message], &itemtype, (Handle*)&cn1, &r );
						GetIndString(theString,Message_List_ID,4);
						SetIText(cn1,theString);
						readInPatches();
						Changed = TRUE;
						next_patch = findNextPatch();
						if (next_patch == -1)
							{
							GetDItem ( dp,ids[Message], &itemtype, (Handle*)&cn1, &r );
							GetIndString(theString,Message_List_ID,5);
							SetIText(cn1,theString);
							_done == TRUE;
							_waiting = FALSE;
							Disable(dp,ids[Redo]);
							Disable(dp,ids[Start]);
							Disable(dp,ids[Cancel]);
							Enable(dp,ids[Save_as]);	
							Enable(dp,OK);			
							}
						else
							{
							GetDItem ( dp,ids[Message], &itemtype, (Handle*)&cn1, &r );
							GetIndString(theString,Message_List_ID,2);
							SetIText(cn1,theString);
							}
						}
					}
				checkFinished();
				}
			}
		else if (item == ids[Slider])   // the slider has been adjusted
			{
			GetMouse(&mpoint);
			do {
				current_patch = GetSlider(mpoint,ids[Slider]);
				ShowPatchScroll(current_patch);
				SetSheetStripText();
				current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
				current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;	
				GetMouse(&mpoint);
				} while ( StillDown ( ) );
			checkError((current_patch/patch_per_strip)*patch_per_strip);
			//_waiting = 0;
			Enable(dp,ids[Redo]);
			SetSheetStripText();
			}
		else if (item == ids[Start]) // start or stop reading
			{
			_waiting = 1;	
			current_patch = findNextPatch();  // reset the display, current patch, sheet, strip, and text
			ShowPatch(current_patch); // display the window
			current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
			current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;
			Disable(dp,ids[Redo]);
			Disable(dp,ids[Start]);	 
			GetDItem ( dp,ids[Message], &itemtype, (Handle*)&cn1, &r );
			GetIndString(theString,Message_List_ID,2);
			SetIText(cn1,theString);
			SetSheetStripText();
			}
		else if (item == ids[Save_as])
			{
			StorePatches();
			doc->patchtype = patchtype;
			doc->handle_save_datafile(patchD);
			ShowPatch(current_patch);
			}
		else if (item == ids[VoiceSet]) 
			{
			GetDItem(dp, ids[VoiceSet], &itemtype, (Handle*)&cn1, &r );
			voice_set = GetCtlValue((ControlHandle)cn1);
			}
		else if (item == ids[Strip_Rect])
			{
			GlobalToLocal(&clickPoint);
			temp = whichPatch(clickPoint);
			if (temp != -1)
				{
				current_patch = temp;
				current_disp = current_patch-current_top;
				GetDItem ( dp, ids[Slider], &itemtype, (Handle*)&sliderControl, &r );
				SetCtlValue ( (ControlHandle)sliderControl,current_patch);
				ShowPatch(current_patch);
				SetSheetStripText();
				}
			}
		else if (item == ids[Redo]) // Reread a strip
			{
			_waiting = 1;		   // reset the display, current patch, sheet, strip, and text
			Disable(dp,ids[Redo]);
			Disable(dp,ids[Start]);	 
			GetDItem ( dp,ids[Message], &itemtype, (Handle*)&cn1, &r );
			GetIndString(theString,Message_List_ID,3);
			SetIText(cn1,theString);
			}
		else if (item == OK)
			{
			StorePatches();
			doc->copyWaiting(patchD);
			doc->patchtype = patchtype;
			//doc->needsSave = TRUE;
			*numwc = 1;
			wc[0].code = WE_Close_Window;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			}
		else if (item == Cancel)
			{
			*numwc = 1;
			wc[0].code = WE_Close_Window;
			wc[0].wintype = WinType;
			wc[0].winnum = WinNum;
			wc[0].doc = doc;
			}
		}
	SetPort(oldp);
	changed = Changed;
	return status;	    	
}



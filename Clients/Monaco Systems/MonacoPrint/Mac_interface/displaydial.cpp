////////////////////////////////////////////////////////////////////////////////////////////////
//	The DTP51 input dialog																	  //
//  displaydial.cpp																			  //
//  James Vogh																				  //
////////////////////////////////////////////////////////////////////////////////////////////////



#include "displaydial.h"
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



#define Input_Dialog		(2016)

#define	DISPPATCH_IDS { 	-1,-1,1,3,2,-1,4,8,9,-1,-1,-1,11,400,401,-1,-1,-1,-1}
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
#define Patch_Message		(8)
#define Button_Labels (17010)

#define PatchLabel		(8)
#define ColumnLabel		(9)

#define ClickNum 			(50)
#define EndColumnNum		(51)

#define Display_Title 	(2006)


DisplayDialog::DisplayDialog(ProfileDoc *dc,RawData *pD,int wn):InputDialog(dc,pD,1)
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
	short		dtp_ids[] = DISPPATCH_IDS;
	Str255		theString,name;
	
	for (i=0; i<NumInputIDS; i++) ids[i] = dtp_ids[i];

	WinType = DisplayWindow;
	WinNum = wn;

	setDialog(Input_Dialog);

	
	GetIndString(theString,Display_Title,1);
	
	SetWTitle( dp, theString);
	
	// Added by James, 3D the box
	threeD_box(ids[ThreeDBox]);
	

	//playSound(8193);
	
	
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
	
//	GetDItem ( dp,ids[Message], &iType, (Handle*)&iHandle, &pRect );
//	SetIText(iHandle,"\p");
	
	GetDItem ( dp,PatchLabel, &iType, (Handle*)&iHandle, &pRect );
	GetIndString(theString,Message_List_ID,Patch_Message);
	SetIText(iHandle,theString);
	
	strcpy((char*)name,pD->desc);
	ctopstr((char*)name);
	SetWTitle( dp, name);
	
	DrawWindow();
}	



McoStatus DisplayDialog::SetSheetStripText(void)
{
	Rect r;
	short itemType;
	Handle cn1;
	Str255 theString;
	
	
GetDItem ( dp,ids[Cur_sheet], &itemType, (Handle*)&cn1, &r );
NumToString(current_patch+1,theString);
SetIText(cn1,theString);
return MCO_SUCCESS;
}
				

Boolean DisplayDialog::TimerEventPresent(void)
{
return FALSE;
}

McoStatus DisplayDialog::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
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
		if (item == ids[Slider])   // the slider has been adjusted
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
		}
	SetPort(oldp);
	return status;	    	
}



#include "tabledialog.h"

TableDialog::TableDialog(ProfileDoc *dc,RawData *pD): InputDialog(dc,pD,0)
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
	int32 wi,hi;
	double w,h;
	McoColorSync *mon,*lab,*list[2];
	

	//Scramble = !(isPressed(0x3a));
	Scramble = FALSE;

	
	top_patch = 0;
						
//init the static members
	//current_patch = total_patches-1;
	//current_patch = 0;
	current_patch = 0;
	last_patch = 0;
	current_strip = 0;
	current_sheet = 0;
 		
 	_done = 0;
 	_waiting = 0;
 	
 	next_patch = new int32[total_patches];
 	
 int32 i,start_odd_even,odd_even,up_down;	
 	

 	current_patch = 0;
 	start_odd_even =  0;	
 	for (i=0; i<total_patches; i++)
 		{
 		odd_even = (i/patchD->patches.rows)%2;
 		if ((start_odd_even && odd_even) || (!start_odd_even && !odd_even)) up_down = 1;
 		else  up_down = -1;
 		next_patch[i] = i + up_down;
 		if (i%patchD->patches.rows == 0) 
 			if (start_odd_even == 0)
 				{
 				 if (odd_even == 1) next_patch[i] = i + patchD->patches.rows;
 				 }
 			else if (odd_even == 1) next_patch[i] = i + patchD->patches.rows;
 		if ((i+1)%patchD->patches.rows == 0) 
 			if (start_odd_even == 0)
 				{
 				 if (odd_even == 0) next_patch[i] = i + patchD->patches.rows;
 				 }
 			else if (odd_even == 0) next_patch[i] = i + patchD->patches.rows;		
 		if (next_patch[i] < 0) next_patch[i] = total_patches;

 		}
 		

	
}	

TableDialog::~TableDialog(void)
{
OSErr			err;


	if (next_patch) delete next_patch;

}






McoStatus TableDialog::SetSheetStripText(void)
{
	Rect r;
	short itemType;
	Handle cn1;
	Str255 theString;
	int32 current_row;
	
	
GetDItem ( dp,ids[Cur_sheet], &itemType, (Handle*)&cn1, &r );
NumToString(current_strip+1,theString);
SetIText(cn1,theString);
GetDItem ( dp,ids[Cur_strip], &itemType, (Handle*)&cn1, &r );
current_row = current_patch % patch_per_strip;
if (current_row+1 > 26) 
	{
	theString[0] = 2;
	theString[1] = 'A';
	theString[2] = 'A'+current_row-26;
	}
else
	{
	theString[0] = 1;
	theString[1] = 'A'+current_row;
	}
SetIText(cn1,theString);
return MCO_SUCCESS;
}




Boolean TableDialog::KeyFilter(EventRecord *theEvent,short *itemHit )
{

//	if (( theEvent->what != keyDown) && (theEvent->what != autoKey))	// just looking for keystrokes
//		return(FALSE);

	switch ( (theEvent->message) & charCodeMask ) {
		case 0x0d:	// Return pressed or ...
		case 0x03:	// ... Enter pressed
			*itemHit = OK;
			return( TRUE );	// Note: pascal-style TRUE
		case 0x1b:
			*itemHit = Cancel;	// Esc pressed
			return( TRUE );
		case 0x1E:
			*itemHit = Position_Up;
			return( TRUE );
		case 0x1F:
			*itemHit = Position_Down;
			return( TRUE );
		case 0x1C:
			*itemHit = Position_Left;
			return( TRUE );
		case 0x1D:
			*itemHit = Position_Right;
			return( TRUE );
		} 
		
if (isPressed(0x7E)) *itemHit = Position_Up;
if (isPressed(0x7D)) *itemHit = Position_Down;
if (isPressed(0x7B)) *itemHit = Position_Left;
if (isPressed(0x7C)) *itemHit = Position_Right;

		
return( FALSE );	// all others
} 


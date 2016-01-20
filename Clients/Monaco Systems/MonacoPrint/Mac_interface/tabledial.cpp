TableDialog::TableDialog(ProfileDoc *dc,RawData *pD): InputDialog(dc,pD)
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
	
	
	_rgbpatchH = NULL;
	_labpatchrefH = NULL;


	
	top_patch = 0;
						
//init the static members
	//current_patch = total_patches-1;
	//current_patch = 0;
	current_patch = 0;
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
 		odd_even = (i/NUM_ROWS_CMYK)%2;
 		if ((start_odd_even && odd_even) || (!start_odd_even && !odd_even)) up_down = 1;
 		else  up_down = -1;
 		next_patch[i] = i + up_down;
 		if (i%NUM_ROWS_CMYK == 0) 
 			if (start_odd_even == 0)
 				{
 				 if (odd_even == 1) next_patch[i] = i + NUM_ROWS_CMYK;
 				 }
 			else if (odd_even == 1) next_patch[i] = i + NUM_ROWS_CMYK;
 		if ((i+1)%NUM_ROWS_CMYK == 0) 
 			if (start_odd_even == 0)
 				{
 				 if (odd_even == 0) next_patch[i] = i + NUM_ROWS_CMYK;
 				 }
 			else if (odd_even == 0) next_patch[i] = i + NUM_ROWS_CMYK;		
 		if (next_patch[i] < 0) next_patch[i] = total_patches;

 		}
 		

	GetDItem (dp, (short)(Patch_Rect), &iType, (Handle*)&iHandle, &pRect);
	num_display_patch = (iRect.bottom-iRect.top)/(pRect.bottom-pRect.top);
	top_patch = 0;
	
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
	
}	

TableDialog::~TableDialog(void)
{
OSErr			err;


	if (next_patch) delete next_patch;

}



McoStatus TableDialog::readInPatches(void)
{
int i,count;
short		item;	
short		itemtype;
Handle		cn1,sliderControl;
Rect		r;
McoStatus state;
int32		start_patch;

//current_patch = current_patch/patch_per_strip;
//current_patch *= patch_per_strip;
start_patch = current_patch;


count = 0;
do {
	state = tcom->getNextPatch(current_patch,&patch_data[current_patch*3],0);
	if (state == MCO_SUCCESS) 
		{
		 playSound(EndColumnNum);
		}
	count ++;
	} while ((state != MCO_SUCCESS) && (count < 5));
if (state != MCO_SUCCESS) 
	{
	//tcom->TurnOnStripRead();
	return state;
	}
//playSound(ClickNum);
if (doc->patchtype == No_Patches) doc->patchtype = patchtype;
GetDItem ( dp, Slider, &itemtype, (Handle*)&sliderControl, &r );
SetCtlValue ( (ControlHandle)sliderControl,current_patch);
//ShowPatchScroll(current_patch);
current_patch = next_patch[current_patch];
//current_patch++;
	
// check for error
checkError(start_patch);
if (current_patch >= total_patches) 
	{
	current_patch = total_patches -1;
	_waiting = FALSE;
	}
ShowPatchScroll(current_patch);
current_sheet = current_patch/(patch_per_strip*strip_per_sheet);
current_strip = (current_patch - current_sheet*(patch_per_strip*strip_per_sheet))/patch_per_strip;
SetSheetStripText();
//tcom->TurnOnStripRead();
return MCO_SUCCESS;
}



McoStatus TableDialog::SetSheetStripText(void)
{
	Rect r;
	short itemType;
	Handle cn1;
	Str255 theString;
	int32 current_row;
	
	
GetDItem ( dp,Cur_sheet, &itemType, (Handle*)&cn1, &r );
NumToString(current_strip+1,theString);
SetIText(cn1,theString);
GetDItem ( dp,Cur_strip, &itemType, (Handle*)&cn1, &r );
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

int32 TableDialog::findNextPatch(void)
{
int32 i;
for (i=0; i<total_patches; i++)
	{
	if (patch_data[i*3] < 0) return i;
	}
return -1;
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


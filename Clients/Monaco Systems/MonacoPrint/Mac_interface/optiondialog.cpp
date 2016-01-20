////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1996 Monaco Systems Inc.
//
//	optiondialog.c
//
//
//	Create Date:	6/23/96 by James
//
//	Revision History:
//
/////////////////////////////////////////////////////////////////////////////////

#include "optiondialog.h"
#include "basedialog.h"

#define OK	1
#define Cancel 2

#define Option_Dialog		2008

//  Define Open_Warn_Dialog_Ditl item IDs
#define Ok_Box			3                
#define Printer_Sim		4


#define Sat_slider 		5
#define ColBal_silder	6
#define L_Linear_slider 7
#define L_Ink_Liner		21
#define Ink_Neutralize 	24
#define Rev_table_size_menu 23
#define Smooth_slider	28
#define ColBal_Cut_slider 31
#define Smooth_sim_slider 34
#define K_Only_Button	33

#define Sat_Text		38
#define	Col_Bal_Text	39
#define L_Linear_Text	41
#define Smooth_Text		42
#define Smooth_Sim_Text	43
#define	ColBal_Cut_Text	40
#define LockHighlight	51
#define PreserveLight	54

#define	Small_Window_button	53
#define Large_Window_button		52

#define From_CMYK		45
#define To_CMYK			47
#define CMYK_To_CMYK	49

#define SaturationPreview	57
#define SaturationPreviewText 55

#define Rev_table_size_string (1011)
#define Printer_Sim_string	  (1012)
#define Sat_Slid_string		  (1013)
#define Color_Balance_string  (1014)
#define L_linear_string		  (1015)
#define L_ink_string		  (1016)
#define Neut_ink_string		  (1017)
#define Smooth_string		  (1020)
#define Color_Bal_Cut_string  (1021)
#define Smooth_Sim_string	  (1024)
#define K_Only_string		  (1025)

#define Option_Strings			(2005)

#define From_CMYK_String	(1)
#define To_CMYK_String		(2)
#define CMYK_To_CMYK_String	(3)
#define From_RGB_String		(4)
#define To_RGB_String		(5)
#define RGB_To_RGB_String	(6)
#define CMYK_To_RGB_String	(7)
#define RGB_To_CMYK_String	(8)

#define Small_Width				(320)
#define Small_Height			(480)

#define Large_Width				(320)
#define	Large_Height			(564)

/* The PICT ids for the medium slider */
/* disable back, enable unpress back, enable press, disable slide, enable unpress slide, enable press slide */

#define LARGE_PICT_IDS {18013,18012,18012,-32000,18009,18009}

/* The offsets etc. used by the medium sliders */
/* horz offset, horz inset, vert offset, horz text, vert text, horz text, vert text, horz text, vert text, Rect left, rect right, text id */

#define LARGE_OFFSETS {-1,20,0,-32000,-32000,-32000,-32000,-32000,-32000, \
						 1,2,12,13,   86,2,97,13, 1,1, -1}



OptionDialog::OptionDialog(ProfileDoc *dc)
{
	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,num;
	long 	temp;
	Str255	numstr,s;
	double	L_adj;
	
	int16			large_slid_picts[] = LARGE_PICT_IDS;
	int16			large_slid_off[] = LARGE_OFFSETS;

	priority = 1;
	
	doc = (ProfileDoc*)dc;

	setDialog(Option_Dialog);
	frame_button(Ok_Box);	
	
	WinType = OptionWindow;
	WinNum = 0;

// initialize the sliders

	refCon1 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = Sat_Text;
	setSlider(large_slid_picts,large_slid_off,Sat_slider,refCon1);	
	
	refCon2 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = Col_Bal_Text;
	setSlider(large_slid_picts,large_slid_off,ColBal_silder,refCon2);
	
	refCon3 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = L_Linear_Text;
	setSlider(large_slid_picts,large_slid_off,L_Linear_slider,refCon3);
	
	refCon4 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = Smooth_Text;
	setSlider(large_slid_picts,large_slid_off,Smooth_slider,refCon4);
	
	refCon5 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = ColBal_Cut_Text;
	setSlider(large_slid_picts,large_slid_off,ColBal_Cut_slider,refCon5);
	
	refCon6 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = Smooth_Sim_Text;
	setSlider(large_slid_picts,large_slid_off,Smooth_sim_slider,refCon6);
	
	refCon7 = new RefConType;
	large_slid_off[OFFSET_TEXT_ID] = SaturationPreviewText;
	setSlider(large_slid_picts,large_slid_off,SaturationPreview,refCon6);


	CopyIntoDial();
	
	//_read_resource();
	updateWindowData(0);
	
	Disable(dp,Small_Window_button);

}	


OptionDialog::~OptionDialog(void)
{
delete refCon1;
delete refCon2;
delete refCon3;
delete refCon4;
delete refCon5;
delete refCon6;
delete refCon7;
}

McoStatus OptionDialog::setBarText(void)
{
	short itemType;
	DialogTHndl dt;
	Handle cn1,h;
	Str255 theString;
	
switch (doc->_condata.tabletype) {
		case(1):
		case(2):
		case(3):
		case(5):
		case(6):	
	
			GetDItem (dp, From_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,From_CMYK_String);
			SetIText(cn1,theString);
			
			GetDItem (dp, To_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,To_CMYK_String);
			SetIText(cn1,theString);
			
			GetDItem (dp, CMYK_To_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,CMYK_To_CMYK_String);
			SetIText(cn1,theString);
			break;
		case (4):
			GetDItem (dp, From_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,From_RGB_String);
			SetIText(cn1,theString);
			
			GetDItem (dp, To_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,To_RGB_String);
			SetIText(cn1,theString);
			
			GetDItem (dp, CMYK_To_CMYK , &itemType, &cn1, &r);
			GetIndString(theString,Option_Strings,RGB_To_RGB_String);
			SetIText(cn1,theString);
			break;
		}
}


McoStatus OptionDialog::updateWindowData(int changed)
{

setBarText();

switch (doc->_condata.tabletype) {
		case(1):
		// a photoshop table
		Disable(dp,Rev_table_size_menu);
		Disable(dp,L_Ink_Liner);
		Disable(dp,Ink_Neutralize);
		Disable(dp,LockHighlight);
		Enable(dp,Printer_Sim);
		Disable(dp,K_Only_Button);
		Disable(dp,Smooth_sim_slider);
		Disable(dp,PreserveLight);
		break;
		
		case(2): // an icc table
		case(3): // an crd
		Disable(dp,Rev_table_size_menu);
		Enable(dp,L_Ink_Liner);
		Enable(dp,Ink_Neutralize);
		Disable(dp,Printer_Sim);	
		Disable(dp,K_Only_Button);
		Disable(dp,Smooth_sim_slider);
		Enable(dp,LockHighlight);
		Enable(dp,PreserveLight);
		break;

		case(5): // an icc link
		case(6): // an tek CMYK to CMYK
		Enable(dp,Rev_table_size_menu);
		Enable(dp,L_Ink_Liner);
		Enable(dp,Ink_Neutralize);
		Enable(dp,K_Only_Button);
		Enable(dp,Smooth_sim_slider);
		Disable(dp,Printer_Sim);	
		Enable(dp,LockHighlight);
		Enable(dp,PreserveLight);
		break;
					
		case(4):
		// an rgb table
		Disable(dp,Rev_table_size_menu);
		Enable(dp,L_Ink_Liner);
		Enable(dp,Ink_Neutralize);
		Disable(dp,Printer_Sim);
		Disable(dp,K_Only_Button);
		Disable(dp,Smooth_sim_slider);
		Enable(dp,LockHighlight);
		Enable(dp,PreserveLight);
		break;
		}
		
	CopyIntoDial();
return MCO_SUCCESS;
}
		



McoStatus OptionDialog::CopyIntoDial(void)
{

	short	iType;
	Handle	iHandle;
	Rect	iRect;
	short 	i,num;
	long 	temp;
	Str255	numstr,s;


	GetDItem(dp,Sat_slider,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.saturation); 
	GetDItem(dp,Sat_Text,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.saturation, s);
	SetIText (iHandle, s); 
	
	GetDItem(dp, ColBal_silder,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.col_balance); 
	GetDItem(dp,Col_Bal_Text,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.col_balance, s);
	SetIText (iHandle, s); 
	
	GetDItem(dp, L_Linear_slider,&iType,&iHandle,&iRect);
	SetCtlValue((ControlHandle)iHandle, doc->_condata.L_linear); 
	GetDItem(dp,L_Linear_Text,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.L_linear, s);
	SetIText (iHandle, s); 
	
	GetDItem(dp,Ink_Neutralize,&iType,&iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.ink_neutralize); 

	GetDItem(dp,L_Ink_Liner,&iType,&iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.ink_linerize); 	
	
	GetDItem(dp,LockHighlight,&iType,&iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.ink_highlight); 

	GetDItem(dp,PreserveLight,&iType,&iHandle,&iRect);	
	if ((!doc->_condata.ink_linerize) && (!doc->_condata.ink_highlight))
		SetCtlValue ((ControlHandle)iHandle, 1); 
	else SetCtlValue ((ControlHandle)iHandle, 0); 
	
	GetDItem(dp, Rev_table_size_menu, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.rev_table_size); 

	GetDItem(dp, Smooth_slider, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.smooth); 
	GetDItem(dp,Smooth_Text,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.smooth, s);
	SetIText (iHandle, s); 

	
	GetDItem(dp, ColBal_Cut_slider, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, (0.5+(double)doc->_condata.col_bal_cut) ); 
	GetDItem(dp,ColBal_Cut_Text,&iType,&iHandle,&iRect);
	NumToString (((double)doc->_condata.col_bal_cut), s);
	SetIText (iHandle, s); 


	GetDItem(dp, Smooth_sim_slider, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.smooth_sim); 
	GetDItem(dp,Smooth_Sim_Text,&iType,&iHandle,&iRect);
	NumToString (doc->_condata.smooth_sim, s);
	SetIText (iHandle, s); 

	GetDItem(dp, K_Only_Button, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, doc->_condata.K_only); 


	GetDItem(dp, SaturationPreview, &iType, &iHandle,&iRect);
	SetCtlValue ((ControlHandle)iHandle, (doc->_staurationpreview) ); 
	GetDItem(dp,SaturationPreviewText,&iType,&iHandle,&iRect);
	NumToString ((doc->_staurationpreview), s);
	SetIText (iHandle, s); 


	return MCO_SUCCESS;
}

McoStatus OptionDialog::copyOutOfDial(void)
{
	short 		itemType;
	Handle			cn1;	
	ControlHandle	sliderControl;
	Rect			wr;	
	double	L_adj;	
	long			temp;
		
	GetDItem ( dp, Sat_slider, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.saturation = GetCtlValue ( sliderControl);
	
	GetDItem ( dp, ColBal_silder, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.col_balance = GetCtlValue ( sliderControl);
	
	GetDItem ( dp, L_Linear_slider, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.L_linear = GetCtlValue ( sliderControl);
	
	GetDItem(dp, Rev_table_size_menu, &itemType, (Handle*)&sliderControl,&wr);
	temp = (long)GetCtlValue((ControlHandle) sliderControl);

	doc->_condata.rev_table_size = temp; 	
	
	GetDItem ( dp, Smooth_slider, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.smooth = GetCtlValue ( sliderControl);

	GetDItem ( dp, Smooth_sim_slider, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.smooth_sim = GetCtlValue ( sliderControl);


	GetDItem ( dp, ColBal_Cut_slider, &itemType, (Handle*)&sliderControl, &wr );
	doc->_condata.col_bal_cut = GetCtlValue ( sliderControl);
	
	GetDItem ( dp, SaturationPreview, &itemType, (Handle*)&sliderControl, &wr );
	doc->_staurationpreview = GetCtlValue ( sliderControl);

	
	return MCO_SUCCESS;
}

McoStatus OptionDialog::DoEvents(short item, Point clickPoint, WindowCode *wc, int32 *numwc, void **data, Boolean &changed)
{
	short 		itemType, lastitem = 1;
	GrafPtr		oldGraf;
	Handle			cn1;	// added by James
	ControlHandle	sliderControl;
	Rect			wr;		// added by James
	McoStatus		status;
	double	L_adj;
	long			temp;


	Changed = FALSE;
	*numwc = 0;
	status = MCO_SUCCESS;

	if (item == OK)
		{ //Ok;
		
		copyOutOfDial();
		doc->didGammutSurface = 0;
		doc->didGammutCompression = 0;
		*numwc = 1;
		wc[0].code = WE_Close_Window;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		Changed = TRUE;
		}
	else if ((item == Smooth_slider) || (item == Smooth_sim_slider) ||
			(item == Sat_slider) || (item == ColBal_silder) ||
			(item == L_Linear_slider) || (item == ColBal_Cut_slider) ||
			(item == Rev_table_size_menu) || (item ==SaturationPreview))
			{
			copyOutOfDial();
			Changed = TRUE;
			}
	else if (item == Cancel) //Cancel
		{
		*numwc = 1;
		wc[0].code = WE_Close_Window;
		wc[0].wintype = WinType;
		wc[0].winnum = WinNum;
		wc[0].doc = doc;
		}
	else if (item ==Ink_Neutralize)
		{
		GetDItem(dp,item,&itemType,&cn1,&wr);
		doc->_condata.ink_neutralize =1 - doc->_condata.ink_neutralize;
		SetCtlValue ((ControlHandle)cn1, doc->_condata.ink_neutralize);
		Changed = TRUE;
		}
	else if (item == L_Ink_Liner)
		{
		GetDItem(dp,item,&itemType,&cn1,&wr);
		doc->_condata.ink_linerize =1;
		doc->_condata.ink_highlight =0;
		SetCtlValue ((ControlHandle)cn1, doc->_condata.ink_linerize);
		GetDItem(dp,LockHighlight,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, doc->_condata.ink_highlight);
		GetDItem(dp,PreserveLight,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, 0);
		Changed = TRUE;
		}
	else if (item ==LockHighlight)
		{
		GetDItem(dp,item,&itemType,&cn1,&wr);
		doc->_condata.ink_linerize = 0;
		doc->_condata.ink_highlight =1;
		SetCtlValue ((ControlHandle)cn1, doc->_condata.ink_highlight);
		GetDItem(dp,L_Ink_Liner,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, doc->_condata.ink_linerize);
		GetDItem(dp,PreserveLight,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, 0);
		Changed = TRUE;
		}
	else if (item ==PreserveLight)
		{
		GetDItem(dp,PreserveLight,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, 1);
		doc->_condata.ink_linerize = 0;
		doc->_condata.ink_highlight = 0;
		GetDItem(dp,L_Ink_Liner,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, 0);
		GetDItem(dp,PreserveLight,&itemType,&cn1,&wr);
		SetCtlValue ((ControlHandle)cn1, 0);
		Changed = TRUE;
		}
	else if (item == K_Only_Button)
		{
		GetDItem(dp,item,&itemType,&cn1,&wr);
		doc->_condata.K_only =1 - doc->_condata.K_only;
		SetCtlValue ((ControlHandle)cn1, doc->_condata.K_only);
		Changed = TRUE;
		}
	else if (item == Small_Window_button)
		{
		SizeWindow(dp,Small_Width,Small_Height,TRUE);
		Disable(dp,Small_Window_button);
		Enable(dp,Large_Window_button);
		}
	else if (item == Large_Window_button)
		{
		SizeWindow(dp,Large_Width,Large_Height,TRUE);
		Enable(dp,Small_Window_button);
		Disable(dp,Large_Window_button);
		}
	changed = Changed;
	return MCO_SUCCESS;
}		
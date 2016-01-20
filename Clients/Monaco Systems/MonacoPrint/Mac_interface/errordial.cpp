#include "mcostat.h"
#include "errordial.h"
#include "pascal_string.h"
#include "mcotypes.h"
#include "mcomem.h"
#include "basewin.h"

#define OK	1
#define Cancel 2

#define Message_List_ID	17007
#define Error_List_ID	17006
#define DealerD_List_ID 17010
#define AlertID 		14000
#define YesNoID			14003
#define MessID			14004
#define	AlertTextID		2
#define Al_HookItem		3

#define YNTextID		3
#define YN_HookItem		4

#define ALERT_ID 14005

#define ALERT_DILT { 0x0003, 0x0000, 0x0000, 0x010C, 0x0200, \
					 0x0120, 0x0248, 0x0402, 0x4F4B, 0x0000, \
					 0x0000, 0x0008, 0x0008, 0x00C4, 0x0108, \
					 0x4002, 0x3E81, 0x0000, 0x0000, 0x0008, \
				     0x0110, 0x0104, 0x0244, 0x880F, 0x5E30, \
					 0x0D0D, 0x5E31, 0x0D0D, 0x5E32, 0x0D0D, \
					 0x5E33, 0x0D00, 0x0000, 0x0000, 0x010C, \
					 0x0200, 0x0120, 0x0248, 0x0000 }


#define ALERT_DILT_L 78

#define ALERT_AL {  0x0021, 0x0019, 0x0145, 0x0265, 0x0001, \
					0x0100, 0x0100, 0x0000, 0x0000, 0x36B5, \
					0x0000, 0x0000 }


#define ALERT_AL_L 24
#define DD_HookItem 4

#define CloseID 		14001
#define Cl_HookItem		3
#define Cl_dontSave		6
#define Cl_cancel		5

extern UserItemUPP  myOK;

McoStatus McoDealerDial(void)
{
Rect r;
Str255 theString;
short item;
DialogPtr dp;
short itemType;
DialogTHndl dt;
Handle dl;
int16	*dl_pt;
Handle cn1,h;
short wi,hi;
Str255	s;
Handle	a_hand;
Handle	d_hand;
Handle	ac_hand;	// for the color table
Handle	dc_hand;	// for the color table
int16	a_ptr[] = ALERT_AL;
int16	d_ptr[] = ALERT_DILT;
int16	a_rID;
int16	d_rID;
int16	ac_rID;
int		result;
int 	tick;
int i;

while ( (a_rID=UniqueID('DLOG')) < 128 );


PtrToHand(a_ptr,&a_hand,ALERT_AL_L);


AddResource( a_hand, 'DLOG', a_rID, "\pDealerAlert" );
result = ResError();
if ( result ) { return MCO_RSRC_ERROR; }

dl =  GetResource ('DITL', ALERT_ID);
if (dl==nil)
	{
	tick = TickCount();
	while (TickCount() < tick + 3600) SysBeep(1);
	return MCO_SUCCESS;
	}
dl_pt = (int16*)McoLockHandle(dl);

for (i=0; i<ALERT_DILT_L/2; i++) if (dl_pt[i] != d_ptr[i]) 
	{
	tick = TickCount();
	while (TickCount() < tick + 3600) SysBeep(1);
	return MCO_SUCCESS;
	}
McoUnlockHandle(dl);
ReleaseResource((Handle)dl);


dt = (DialogTHndl) GetResource ('DLOG', a_rID);
if (dt==nil) return MCO_RSRC_ERROR;
HNoPurge ((Handle) dt);

r = (**dt).boundsRect;

#define menuHeight 20

wi = qd.screenBits.bounds.right;
hi = qd.screenBits.bounds.bottom;

OffsetRect (&r, -r.left, -r.top);
OffsetRect (&r, (wi - r.right) / 2,
				(hi - r.bottom - menuHeight) / 3 + menuHeight);
(**dt).boundsRect = r;

#undef menuHeight

dp = GetNewDialog (a_rID, nil, (WindowPtr) -1);
if (dp==nil) return MCO_RSRC_ERROR;
GetDItem (dp, DD_HookItem, &itemType, &h, &r);
SetDItem (dp, DD_HookItem,  itemType, (Handle)myOK, &r);

GetIndString(s,DealerD_List_ID,1);

//if (GoodString(s)) ParamText(s,"\p","\p","\p");
//else
 ParamText("\pThis copy of MonacoPRINT is not for resale",
	"\pLogiciel MonacoPRINT de demonstration. Vente Interdite.",
	"\pDiese version von MonacoPRINT  ist night fŸr den verkauf bestimmt!",
	"\pQuesta copia di MonacoPRINT non pu˜ essere rivenduta.                                                     Estas copias (fotocopias) no es para revender");

SysBeep(3);
ShowWindow(dp);
ModalDialog (nil, &item);

DisposDialog (dp);
HPurge ((Handle) dt);
ReleaseResource((Handle)dt);

RmveResource(a_hand);

DisposeHandle(a_hand);

return MCO_SUCCESS;
}

// Convert system errors

McoStatus ConvertError(McoStatus state)
{
switch (state) 
	{
	case -37: return MCO_FILE_OPEN_ERROR;		//bad name
	case -33: return MCO_DISK_FULL;				//directory full
	case -120: return MCO_FILE_OPEN_ERROR;		//directory not found
	case -48: return MCO_FILE_OPEN_ERROR;		//duplicate filename
	case -58: return MCO_FILE_ERROR;			//external file system
	case -36: return MCO_FILE_ERROR;			//i/o error
	case -35: return MCO_FILE_ERROR;			//no such volume
	case -46: return MCO_FILE_LOCKED;			//volume is locked
	case -44: return MCO_FILE_LOCKED;			//disket is write protected
	}
return MCO_SUCCESS;
}

// Display an error alert

void McoErrorAlert(McoStatus state)
{
	Rect r;
	Str255 theString,numSt;
	short item;
	DialogPtr dp;
	short itemType;
	DialogTHndl dt;
	Handle cn1,h;
	short wi,hi;

    if (state == MCO_SUCCESS || state == MCO_CANCEL) return;
    if (state < 0 ) state = ConvertError(state);

	dt = (DialogTHndl) GetResource ('DLOG', AlertID);
	if (dt==nil) return;
	HNoPurge ((Handle) dt);
	
	r = (**dt).boundsRect;
	
	#define menuHeight 20

	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;

	OffsetRect (&r, -r.left, -r.top);
	OffsetRect (&r, (wi - r.right) / 2,
					(hi - r.bottom - menuHeight) / 3 + menuHeight);
	(**dt).boundsRect = r;

	#undef menuHeight
	
	dp = GetNewDialog (AlertID, nil, (WindowPtr) -1);
	if (dp==nil) return;
	GetDItem (dp, Al_HookItem, &itemType, &h				  , &r);
	SetDItem (dp, Al_HookItem,  itemType, (Handle) myOK, &r);
	GetDItem (dp,AlertTextID, &itemType, &cn1, &r);
	switch (state) {
			case MCO_SUCCESS:
				GetIndString(theString,Error_List_ID,1);
				SetIText(cn1,theString);
				break;
			case MCO_FAILURE:
				GetIndString(theString,Error_List_ID,2);
				SetIText(cn1,theString);
				break;
			case MCO_MEM_ALLOC_ERROR:
				GetIndString(theString,Error_List_ID,3);
				SetIText(cn1,theString);
				break;
			case MCO_SYSTEM_ERROR:
				GetIndString(theString,Error_List_ID,4);
				SetIText(cn1,theString);
				break;
			case MCO_UNSUPPORTED_OPERATION:
			case MCO_UNSUPPORTED_MODE:
			case MCO_INVALID_LAYOUT:
			case MCO_UNSUPPORTED_LAYOUT:
			case MCO_UNSUPPORTED_NUM_COMPONENTS:
				GetIndString(theString,Error_List_ID,5);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_ERROR:
				GetIndString(theString,Error_List_ID,6);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_OPEN_ERROR:
				GetIndString(theString,Error_List_ID,7);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_CREATE_ERROR:
				GetIndString(theString,Error_List_ID,8);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_READ_ERROR:
				GetIndString(theString,Error_List_ID,9);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_WRITE_ERROR:
				GetIndString(theString,Error_List_ID,10);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_NOT_OPEN:
				GetIndString(theString,Error_List_ID,11);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_DATA_ERROR:
				GetIndString(theString,Error_List_ID,12);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_EOF_ERROR:
				GetIndString(theString,Error_List_ID,13);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_BOF_ERROR:
				GetIndString(theString,Error_List_ID,14);
				SetIText(cn1,theString);
				break;
			case MCO_NOT_IMPLEMENTED:
				GetIndString(theString,Error_List_ID,15);
				SetIText(cn1,theString);
				break;
			case MCO_BAD_VALUE:
			case MCO_BAD_PARAMETER:
				GetIndString(theString,Error_List_ID,16);
				SetIText(cn1,theString);
				break;
			case MCO_OBJECT_NOT_INITIALIZED:
				GetIndString(theString,Error_List_ID,17);
				SetIText(cn1,theString);
				break;
			case MCO_OBJECT_ALREADY_INITIALIZED:
				GetIndString(theString,Error_List_ID,18);
				SetIText(cn1,theString);
				break;
			case MCO_OUT_OF_RANGE:
				GetIndString(theString,Error_List_ID,19);
				SetIText(cn1,theString);
				break;
			case MCO_WRONG_NUM_COMP:
				GetIndString(theString,Error_List_ID,20);
				SetIText(cn1,theString);
				break;
			case MCO_NOT_CALIB:
				GetIndString(theString,Error_List_ID,21);
				SetIText(cn1,theString);
				break;
			case MCO_BADEVE:
				GetIndString(theString,Error_List_ID,22);
				SetIText(cn1,theString);
				break;
			case MCO_NOUSE:
				GetIndString(theString,Error_List_ID,23);
				SetIText(cn1,theString);
				break;
			case MCO_ABORTED:
				GetIndString(theString,Error_List_ID,24);
				SetIText(cn1,theString);
				break;
			case MCO_RSRC_ERROR:
				GetIndString(theString,Error_List_ID,25);
				SetIText(cn1,theString);
				break;
			case MCO_CMYK_TABEL_NOT_LOADED:
				GetIndString(theString,Error_List_ID,26);
				SetIText(cn1,theString);
				break;
			case MCO_READ_CMYK_TABLE_ERROR:
				GetIndString(theString,Error_List_ID,27);
				SetIText(cn1,theString);
				break;	
			case MCO_OPEN_CMYK_TABLE_ERROR:
				GetIndString(theString,Error_List_ID,28);
				SetIText(cn1,theString);
				break;				
			case MCO_WRITE_TIFF_ERROR:
				GetIndString(theString,Error_List_ID,29);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_LOCKED:
				GetIndString(theString,Error_List_ID,30);
				SetIText(cn1,theString);
				break;
			case MCO_DISK_FULL:
				GetIndString(theString,Error_List_ID,31);
				SetIText(cn1,theString);
				break;
			case MCO_SOURCE_FOLDER_EMPTY:
				GetIndString(theString,Error_List_ID,32);
				SetIText(cn1,theString);
				break;
			case MCO_SOURCE_FOLDER_FULL:
				GetIndString(theString,Error_List_ID,33);
				SetIText(cn1,theString);
				break;
			case MCO_DESTINATION_FOLDER_FULL:
				GetIndString(theString,Error_List_ID,34);
				SetIText(cn1,theString);
				break;
			case MCO_WRITE_EPS_ERROR:
				GetIndString(theString,Error_List_ID,35);
				SetIText(cn1,theString);
				break;
			case MCO_OPEN_JOB_ERROR:
				GetIndString(theString,Error_List_ID,36);
				SetIText(cn1,theString);
				break;
			case MCO_SAVE_JOB_ERROR:
				GetIndString(theString,Error_List_ID,37);
				SetIText(cn1,theString);
				break;
			case MCO_WAIT:
				GetIndString(theString,Error_List_ID,38);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_ALREADY_OPEN:
				GetIndString(theString,Error_List_ID,39);
				SetIText(cn1,theString);
				break;
			case MCO_OPEN_CMYK_ERROR:
				GetIndString(theString,Error_List_ID,40);
				SetIText(cn1,theString);
				break;
			case MCO_OPEN_SRC_ERROR:
				GetIndString(theString,Error_List_ID,41);
				SetIText(cn1,theString);
				break;
			case MCO_OPEN_DEST_ERROR:
				GetIndString(theString,Error_List_ID,42);
				SetIText(cn1,theString);
				break;
			case MCO_NOT_A_GRAY_IMAGE:
				GetIndString(theString,Error_List_ID,43);
				SetIText(cn1,theString);
				break;
			case MCO_OPEN_SET_ERROR:
				GetIndString(theString,Error_List_ID,44);
				SetIText(cn1,theString);
				break;
			case MCO_FILE_NAME_ERROR:
				GetIndString(theString,Error_List_ID,45);
				SetIText(cn1,theString);
				break;
			case MCO_COR_SET_ERROR:
				GetIndString(theString,Error_List_ID,46);
				SetIText(cn1,theString);
				break;
			case MCO_SINGULAR:
				GetIndString(theString,Error_List_ID,47);
				SetIText(cn1,theString);
				break;
			case MCO_NO_QD3D:
				GetIndString(theString,Error_List_ID,48);
				SetIText(cn1,theString);
				break;
			case MCO_QD3D_ERROR:
				GetIndString(theString,Error_List_ID,49);
				SetIText(cn1,theString);
				break;
			case MCO_SERIAL_ERROR:
				GetIndString(theString,Error_List_ID,50);
				SetIText(cn1,theString);
				break;
			case MCO_MAX_MEMORY:
				GetIndString(theString,Error_List_ID,51);
				SetIText(cn1,theString);
				break;	
			case MCO_INVALID_PTR:
				GetIndString(theString,Error_List_ID,52);
				SetIText(cn1,theString);
				break;	
			case MCO_SERIAL_DEVICE_ERROR:
				GetIndString(theString,Error_List_ID,53);
				SetIText(cn1,theString);
				break;
			case MCO_DEVICE_NOT_FOUND:
				GetIndString(theString,Error_List_ID,54);
				SetIText(cn1,theString);
				break;
			case MCO_NOT_ICC_LINK:
				GetIndString(theString,Error_List_ID,55);
				SetIText(cn1,theString);
				break;						
			default:
				GetIndString(theString,Error_List_ID,56);
				NumToString ((int)state, numSt);
				add_string(theString,numSt);
				SetIText(cn1,theString);
				break;
			}
	SysBeep(1);
	ShowWindow(dp);
	ModalDialog (nil, &item);
	DisposDialog (dp);
	HPurge ((Handle) dt);
	ReleaseResource((Handle)dt);

}


// Display a dialog with a message

McoStatus McoMessAlert(McoMess mess,unsigned char *sn)
{
	Rect r;
	Str255 s;
	Str255 theString;
	short item;
	DialogPtr dp;
	short itemType;
	DialogTHndl dt;
	Handle cn1,h;
	short wi,hi;
	int		dialid,hook,text;
	char	ts[800];
	McoStatus state;

	if (mess == MCO_SAFE_MEM) return MCO_SUCCESS;
	
	switch (mess) {
			case MCO_SCAN_NOT_CALIB:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			case MCO_GET_CAL_FN:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;	
			case MCO_SCAN_CALIB:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_MIS_MATCH_CMP:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_MIS_MATCH_CMP_OK:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_UNSUC_SCAN_CALIB:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_NO_SCAN_FILE:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_NO_SCAN_FILE_SUP:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			case MCO_ALMOST_USE:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_NO_USE:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_EVE_ERROR:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_BUFF_SIZE:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_NO_ERROR:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_SAFE_MEM:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_SAVE_FILE_OPT:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			case MCO_SAVE_FILE_NEC:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;			
			case MCO_TABLE_NOT_FOUND:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_ICC_NOT_FOUND:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_UNSPRT_SCAN_TYPE:		
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;	
			case MCO_BAD_PROFILE:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;		
			case MCO_CLOSE_DOC:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			case MCO_FIND_PATCHS:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;			
			case MCO_ALIGN_PATCHES:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			case MCO_CALIB:
			case MCO_CALIB_2:
			case MCO_FAIL_CALIB:
			case MCO_WARN_TCR:
			case MCO_MEAS_PAPER_DENSITY:
			case MCO_CALIB_XRITE:
				dialid = MessID;
				hook = Al_HookItem;
				text = AlertTextID;
				break;
			case MCO_REVERT_FILE:
				dialid = YesNoID;
				hook = YN_HookItem;
				text = YNTextID;
				break;
			}	

	state = MCO_NOT_CALIB;
    if (state == MCO_SUCCESS) return MCO_SUCCESS;

	dt = (DialogTHndl) GetResource ('DLOG', dialid);
	if (dt==nil) return MCO_RSRC_ERROR;
	HNoPurge ((Handle) dt);
	
	r = (**dt).boundsRect;
	
	#define menuHeight 20

	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;

	OffsetRect (&r, -r.left, -r.top);
	OffsetRect (&r, (wi - r.right) / 2,
					(hi - r.bottom - menuHeight) / 3 + menuHeight);
	(**dt).boundsRect = r;

	#undef menuHeight
	
	dp = GetNewDialog (dialid, nil, (WindowPtr) -1);
	if (dp==nil) return MCO_RSRC_ERROR;
	GetDItem (dp, hook , &itemType, &h				  , &r);
	SetDItem (dp, hook,  itemType, (Handle) myOK, &r);
	GetDItem (dp, text , &itemType, &cn1, &r);
	switch (mess) {
			case MCO_SCAN_NOT_CALIB:
				GetIndString(theString,Message_List_ID,1);
				SetIText(cn1,theString);
				break;
			case MCO_GET_CAL_FN:
				GetIndString(theString,Message_List_ID,2);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_SCAN_CALIB:
				GetIndString(theString,Message_List_ID,3);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_MIS_MATCH_CMP:
				GetIndString(theString,Message_List_ID,4);
				SetIText(cn1,theString);
				break;
			case MCO_MIS_MATCH_CMP_OK:
				GetIndString(theString,Message_List_ID,5);
				SetIText(cn1,theString);
				break;
			case MCO_UNSUC_SCAN_CALIB:
				GetIndString(theString,Message_List_ID,6);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_NO_SCAN_FILE:
				GetIndString(theString,Message_List_ID,7);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_NO_SCAN_FILE_SUP:
				GetIndString(theString,Message_List_ID,8);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_ALMOST_USE:
				GetIndString(theString,Message_List_ID,9);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_NO_USE:
				GetIndString(theString,Message_List_ID,10);
				SetIText(cn1,theString);
				break;
			case MCO_EVE_ERROR:
				GetIndString(theString,Message_List_ID,11);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_BUFF_SIZE:
				GetIndString(theString,Message_List_ID,12);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_NO_ERROR:
				GetIndString(theString,Message_List_ID,13);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_SAFE_MEM:
				GetIndString(theString,Message_List_ID,14);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;	
			case MCO_SAVE_FILE_OPT:
				GetIndString(theString,Message_List_ID,15);
				SetIText(cn1,theString);
				break;
			case MCO_SAVE_FILE_NEC:
				GetIndString(theString,Message_List_ID,16);
				SetIText(cn1,theString);
				break;
			case MCO_TABLE_NOT_FOUND:
				GetIndString(theString,Message_List_ID,17);
				SetIText(cn1,theString);
				break;
			case MCO_UNSPRT_SCAN_TYPE:
				GetIndString(theString,Message_List_ID,18);
				add_string(theString,sn);
				SetIText(cn1,theString);
				break;
			case MCO_ICC_NOT_FOUND:
				GetIndString(theString,Message_List_ID,19);
				SetIText(cn1,theString);
				break;
			case MCO_BAD_PROFILE:
				GetIndString(theString,Message_List_ID,20);
				SetIText(cn1,theString);
				break;
			case MCO_CLOSE_DOC:
				GetIndString(theString,Message_List_ID,21);
				SetIText(cn1,theString);
				break;
			case MCO_FIND_PATCHS:
				GetIndString(theString,Message_List_ID,22);
				SetIText(cn1,theString);
				break;
			case MCO_ALIGN_PATCHES:
				GetIndString(theString,Message_List_ID,23);
				SetIText(cn1,theString);
				break;
			case MCO_CALIB:
				GetIndString(theString,Message_List_ID,24);
				SetIText(cn1,theString);
				break;
			case MCO_CALIB_2:
				GetIndString(theString,Message_List_ID,25);
				SetIText(cn1,theString);
				break;
			case MCO_CALIB_XRITE:
				GetIndString(theString,Message_List_ID,26);
				SetIText(cn1,theString);
				break;				
			case MCO_FAIL_CALIB:
				GetIndString(theString,Message_List_ID,27);
				SetIText(cn1,theString);
				break;
			case MCO_WARN_TCR:
				GetIndString(theString,Message_List_ID,28);
				SetIText(cn1,theString);
				break;
			case MCO_MEAS_PAPER_DENSITY:
				GetIndString(theString,Message_List_ID,29);
				SetIText(cn1,theString);
				break;
			case MCO_REVERT_FILE:
				GetIndString(theString,Message_List_ID,30);
				SetIText(cn1,theString);
				break;				
			}
	ShowWindow(dp);
	ModalDialog (nil, &item);
	if (item==OK) state = MCO_SUCCESS;
	else state = MCO_FAILURE;
	DisposDialog (dp);
	HPurge ((Handle) dt);
	ReleaseResource((Handle)dt);
	return state;
}



short McoCloseAlert(int savef)
{
	Rect r;
	Str255 theString,numSt;
	short item;
	DialogPtr dp;
	short itemType;
	DialogTHndl dt;
	Handle cn1,h;
	short wi,hi;

	dt = (DialogTHndl) GetResource ('DLOG', CloseID);
	if (dt==nil) return 0;
	HNoPurge ((Handle) dt);
	
	r = (**dt).boundsRect;
	
	#define menuHeight 20

	wi = qd.screenBits.bounds.right;
	hi = qd.screenBits.bounds.bottom;

	OffsetRect (&r, -r.left, -r.top);
	OffsetRect (&r, (wi - r.right) / 2,
					(hi - r.bottom - menuHeight) / 3 + menuHeight);
	(**dt).boundsRect = r;

	#undef menuHeight
	
	dp = GetNewDialog (CloseID, nil, (WindowPtr) -1);
	if (dp==nil) return 0;
	GetDItem (dp, Cl_HookItem, &itemType, &h				  , &r);
	SetDItem (dp, Cl_HookItem,  itemType, (Handle) myOK, &r);
	
	if (!savef) Disable(dp,1);

	SysBeep(1);
	ShowWindow(dp);
	ModalDialog (nil, &item);
	DisposDialog (dp);
	HPurge ((Handle) dt);
	ReleaseResource((Handle)dt);
	switch (item) {
		case 1: return 1;
		case Cl_dontSave: return 2;
		case Cl_cancel: return 3;
		}
	return 3;
}
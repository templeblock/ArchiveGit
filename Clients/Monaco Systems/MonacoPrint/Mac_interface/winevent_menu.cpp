/******************************************************************/
/* winevent_menu.cpp											  */
/* Copyright(c) 1995 Monaco Systems Inc.						  */
/* The main event handler										  */
/* Handle all events related to the menu						  */
/* May 5, 1995												      */
/* James Vogh													  */
/******************************************************************/


#include "winevent.h"
#include "printmain.h"
#include "profiledocmac.h"
#include "aboutdialog.h"
#include "errordial.h"

// initialize the menues

McoStatus AllWins::InitMenues(void)
{
Handle 		mbar;
MenuHandle 	menuh,apple_handle;


mbar = GetNewMBar(MENUBAR);

if (!mbar) exit(0);
SetMenuBar(mbar);
DisposeHandle(mbar);
menuh = GetMenu(APPLE_MENU);


apple_handle = GetMHandle(APPLE_MENU);
if(apple_handle == NULL) return MCO_MEM_ALLOC_ERROR;		
AddResMenu(apple_handle, 'DRVR'); 	/* Add names of all DAs*/
InsertMenu(apple_handle, 0);
//AddResMenu(menuh,'DRVR');

MenuHandle	DeviceMemuH, DevicesMenuH, PortMenuH;
	
DevicesMenuH = 	GetMenu(DEVICES_MENU);

DeviceMemuH = GetMenu(DEVICE_MENU);
if(DeviceMemuH == NULL) return MCO_MEM_ALLOC_ERROR;		
InsertMenu(DeviceMemuH, -1);
SetItemMark(DevicesMenuH, 1, DEVICE_MENU);

PortMenuH = GetMenu(PORT_MENU);
if(PortMenuH == NULL) return MCO_MEM_ALLOC_ERROR;		
InsertMenu(PortMenuH, -1);
SetItemMark(DevicesMenuH, 2, PORT_MENU);	
	
	
DrawMenuBar();
return MCO_SUCCESS;
}

McoStatus AllWins::ClearMenues(void)
{
Handle 		mbar;


mbar = GetNewMBar(MENUBAR);
if(mbar == NULL) return MCO_MEM_ALLOC_ERROR;
SetMenuBar(mbar);
DisposHandle(mbar);
		
return MCO_SUCCESS;

/*

InsertMenu(GetMenu(FILE_MENU), 0);
		
DrawMenuBar(); */
return MCO_SUCCESS;
}


//enable or disable a menu
static void enable (MenuHandle menu, short item, Boolean enableMenu)
{
	if (enableMenu)
		EnableItem(menu, item);
	else
		DisableItem(menu, item);
}

//check menu is enabled or disabled
static Boolean isenable (MenuHandle menu, short item)
{
	if ((*menu)->enableFlags & ( 1 << item))
		return true;
	else
		return false;
}

McoStatus AllWins::SetMenu(void)
{
int i;

MenuHandle menuh,deviceh,devh,porth;
Document *doc= NULL;

menuh = GetMHandle(FILE_MENU);
if (menuh == NULL) return MCO_FAILURE;

deviceh = GetMHandle(DEVICES_MENU);
if (deviceh == NULL) return MCO_FAILURE;

devh = GetMHandle(DEVICE_MENU);
if (devh == NULL) return MCO_FAILURE;

porth = GetMHandle(PORT_MENU);
if (porth == NULL) return MCO_FAILURE;

enable(menuh,PRNT_PATCHES,FALSE);

if (frontWin != NULL)
	{
	doc = FindDocFromWin(frontWin);
	}

if (num_documents == 0)
	{
	enable(menuh,NEW_PROFILE_ITEM,TRUE);
	enable(menuh,OPEN_PROFILE_ITEM,TRUE);
	enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
	enable(menuh,SAVE_PATCH_DATA,FALSE);
	//enable(menuh,PRNT_PATCHES,FALSE);
	}
else
	{
	if (doc == NULL)
		{
		//enable(menuh,PRNT_PATCHES,FALSE);
		enable(menuh,NEW_PROFILE_ITEM,FALSE);
		enable(menuh,OPEN_PROFILE_ITEM,FALSE);
		enable(menuh,SAVE_PATCH_DATA,FALSE);
		enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
		}
	else
		{
		//enable(menuh,PRNT_PATCHES,TRUE);
		
		enable(menuh,NEW_PROFILE_ITEM,FALSE);
		if (doc->locked) 
			{
			enable(menuh,OPEN_PROFILE_ITEM,FALSE);
			enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
			}
		else 
			{
			if ((((ProfileDoc*)doc)->inputStatus() & Processing_Input1) || 
				(((ProfileDoc*)doc)->inputStatus() & Processing_Input2) ||
				(((ProfileDoc*)doc)->inputStatus() & Tweaking_Input))
				{
				enable(menuh,OPEN_PROFILE_ITEM,FALSE);
				enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
				}
			else
				{
				enable(menuh,OPEN_PROFILE_ITEM,TRUE);
				enable(menuh,CLOSE_PROFILE_ITEM,TRUE);
				}
			if (((ProfileDoc*)doc)->inputStatus() & Getting_Input)
				{
				enable(deviceh,1,FALSE);
				enable(deviceh,2,FALSE);
				}
			else
				{
				enable(deviceh,1,TRUE);
				enable(deviceh,2,TRUE);
				}
			// set device availablity
			// only handhelds and none if in tweaking window
			if (((ProfileDoc*)doc)->inputStatus() & Tweaking_Input)
				{
				enable(devh,DEVICE_NONE,TRUE);
				enable(devh,DEVICE_DTP51,FALSE);
				enable(devh,DEVICE_TECH_CP300,TRUE);
				enable(devh,DEVICE_TECH_SP820,TRUE);
				enable(devh,DEVICE_TECH_TCR_CP300,FALSE);
				enable(devh,DEVICE_TECH_TCR_SP820,FALSE);
				enable(devh,DEVICE_GRE_HAND,TRUE);
				enable(devh,DEVICE_GRE_TABLE,FALSE);
				}
			else
				{
				enable(devh,DEVICE_NONE,TRUE);
				enable(devh,DEVICE_DTP51,TRUE);
				enable(devh,DEVICE_TECH_CP300,TRUE);
				enable(devh,DEVICE_TECH_SP820,TRUE);
				enable(devh,DEVICE_TECH_TCR_CP300,TRUE);
				enable(devh,DEVICE_TECH_TCR_SP820,TRUE);
				enable(devh,DEVICE_GRE_HAND,TRUE);
				enable(devh,DEVICE_GRE_TABLE,TRUE);
				}
			// set checkmarks for current device and port
			for (i=1; i<=8; i++)
				{
				if ((((ProfileDocInf*)doc)->defaults->input_type == DT_None) && (i == 1))  CheckItem(devh,i,1);
				else if (((ProfileDocInf*)doc)->defaults->input_type == (DeviceTypes)(i+1)) CheckItem(devh,i,1);
				else CheckItem(devh,i,0);
				}
			for (i=1; i<=2; i++)
				{
				if (((ProfileDocInf*)doc)->defaults->port == i)  CheckItem(porth,i,1);
				else CheckItem(porth,i,0);
				} 
			}
		
		if (doc->dataPresent()) enable(menuh,SAVE_PATCH_DATA,TRUE);
		else enable(menuh,SAVE_PATCH_DATA,FALSE);
		if (doc->dataPresent()) enable(menuh,DISPLAY_ITEM,TRUE);
		else enable(menuh,DISPLAY_ITEM,FALSE);
		}
	}
if (frontWin != NULL)
	{
		if ((frontWin->WinType == Gammut_Dialog1) || 
		(frontWin->WinType == Gammut_Dialog_Src) || 
		(frontWin->WinType == Gammut_Dialog_Dst))  enable(menuh,CLOSE_G_ITEM,TRUE);
	else enable(menuh,CLOSE_G_ITEM,FALSE);
	}
else enable(menuh,CLOSE_G_ITEM,FALSE);

menuh = GetMHandle(EDIT_MENU);

enable(menuh,EDIT_UNDO,FALSE);
enable(menuh,EDIT_CUT,FALSE);
enable(menuh,EDIT_COPY,FALSE);
enable(menuh,EDIT_PASTE,FALSE);

return MCO_SUCCESS;
}


/****************************************************
*	Handle menu choice.
*****************************************************/
McoStatus AllWins::Handle_menu(int32 menu_choice,Boolean *finish,Boolean *changed)
{
	McoStatus 	status = MCO_SUCCESS;
    int16 		result = FALSE;
    AboutDialog	*aboutdialog;
	short		menuID;
	short		menuItem;
	WindowPeek	wp, jobwp;
	GrafPtr		savePort;
	Str255 		desk_acc_name;
	int16 		hititem, jobresult;
	McoStatus	state = MCO_SUCCESS;
	Document 	*doc;
	ProfileDocInf 	*pdoc;
	Point		where;
	WindowCode	wc[10];
	int		nwcode;
	
	if(menu_choice == 0)
		return 	state;

	menuID = HiWord(menu_choice);
	menuItem = LoWord(menu_choice);
		
	switch (menuID){
		case	APPLE_MENU:
		  	if (menuItem == ABOUT_ITEM){
				state = openWindow(AboutWindow,0,0L,0L);
				if (state != MCO_SUCCESS) McoErrorAlert(state);
				break;
		  	}
	
			GetPort(&savePort);
			GetItem(GetMenu(APPLE_MENU), menuItem, desk_acc_name);
			OpenDeskAcc(desk_acc_name);
			SetPort(savePort);
					
			break;
	
	  	case FILE_MENU:
			switch (menuItem){
				case OPEN_PROFILE_ITEM:
					state = openProfileDoc();
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case NEW_PROFILE_ITEM:
					state = newProfileDoc();
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case DISPLAY_ITEM:
					WaitingDoc = FindDocFromWin(frontWin);
					if (WaitingDoc == 0L) break;
					state = openWindow(DisplayWindow,-1,((ProfileDocInf*)(WaitingDoc))->patchD,WaitingDoc);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case CLOSE_PROFILE_ITEM:
					nwcode = 1;
					wc[0].code = WE_CloseDoc;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case SAVE_PATCH_DATA:
					nwcode = 1;
					wc[0].code = WE_Doc_Save;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;					
			  	case OPEN_G_ITEM:
					state = openWindow(GammutSurfaceTherm1,0,0L,0L);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					state = MCO_SUCCESS;
			  		break;
			  	case CLOSE_G_ITEM:
			  		if (frontWin != NULL)
			  			{
			  			if ((frontWin->WinType == Gammut_Dialog1) || 
							(frontWin->WinType == Gammut_Dialog_Src) || 
							(frontWin->WinType == Gammut_Dialog_Dst)) 
							 	removeWindow(frontWin);
			  			}
			  		break;
			  	case PRNT_PATCHES:
			  		nwcode = 1;
					wc[0].code = WE_Doc_Print;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;			  		
			  	case QUIT_ITEM:
			  		if (checkCloseDocs(NULL)) *finish = TRUE;
			  		break;	  	
			}
			break;  
		case DEVICE_MENU:
			pdoc = (ProfileDocInf*)FindDocFromWin(frontWin);
			if (pdoc == 0L) break;
			if (menuItem == 1) pdoc->defaults->input_type = DT_None;
			else pdoc->defaults->input_type = (DeviceTypes)(menuItem+1);
			WaitingDoc = pdoc;
			nwcode = 1;
			wc[0].code = WE_UpdateInputDevice;
			wc[0].wintype = frontWin->WinType;
			wc[0].winnum = frontWin->WinNum;
			wc[0].doc = pdoc;
			state = DoCommand(wc,nwcode,where,NULL,finish,changed);
			if (state != MCO_SUCCESS) McoErrorAlert(state);
			SetMenu();
			break;
		case PORT_MENU:	
			pdoc = (ProfileDocInf*)FindDocFromWin(frontWin);
			if (pdoc == 0L) break;
			pdoc->defaults->port = menuItem;
			WaitingDoc = pdoc;
			nwcode = 1;
			wc[0].code = WE_UpdateInputDevice;
			wc[0].wintype = frontWin->WinType;
			wc[0].winnum = frontWin->WinNum;
			wc[0].doc = pdoc;
			state = DoCommand(wc,nwcode,where,NULL,finish,changed);
			if (state != MCO_SUCCESS) McoErrorAlert(state);
			SetMenu();
			break;

	}
	return state;
}
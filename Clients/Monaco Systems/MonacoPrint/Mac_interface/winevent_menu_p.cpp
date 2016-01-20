/******************************************************************/
/* winevent_menu_p.cpp											  */
/* Copyright(c) 1995 Monaco Systems Inc.						  */
/* The polaroid version											  */
/* The main event handler										  */
/* Handle all events related to the menu						  */
/* May 5, 1995												      */
/* James Vogh													  */
/******************************************************************/

#include "winevent.h"
#include "printmain_p.h"
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

MenuHandle emenuh = 0L,menuh = 0L,deviceh = 0L,devh = 0L,porth = 0L;
Document *doc= NULL;

menuh = GetMHandle(FILE_MENU);
if (menuh == NULL) return MCO_FAILURE;

#ifndef IN_POLAROID
deviceh = GetMHandle(DEVICES_MENU);
if (deviceh == NULL) return MCO_FAILURE;

devh = GetMHandle(DEVICE_MENU);
if (devh == NULL) return MCO_FAILURE;

porth = GetMHandle(PORT_MENU);
if (porth == NULL) return MCO_FAILURE;
#endif

emenuh = GetMHandle(EDIT_MENU);
if (emenuh == NULL) return MCO_FAILURE;

if (frontWin != NULL)
	{
	doc = FindDocFromWin(frontWin);
	}

if ((numWins > 0) && (frontWin))
	{
	if ((frontWin->WinType == Gammut_Dialog1) || 
		(frontWin->WinType == Gammut_Dialog_Src) || 
		(frontWin->WinType == Gammut_Dialog_Dst)) 
		enable(menuh,CLOSE_PROFILE_ITEM,TRUE);
	else if (doc != 0L) 
		{
		if (doc->locked) enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
		else enable(menuh,CLOSE_PROFILE_ITEM,TRUE);
		}
	else enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
	}
else enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
if (num_documents == 0)
	{
	if (_monacoprint) enable(menuh,NEW_PROFILE_ITEM,TRUE);
	else enable(menuh,NEW_PROFILE_ITEM,FALSE);
	enable(menuh,OPEN_PROFILE_ITEM,TRUE);
	//enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
	enable(menuh,SAVE_PROFILE,FALSE);
	enable(menuh,SAVE_PROFILE_AS,FALSE);
	//enable(menuh,PRNT_PATCHES,FALSE);
	enable(emenuh,EDIT_PREF,FALSE);
	enable(menuh,REVERT_PROFILE,FALSE);
	if (_monacoprint) enable(menuh,OPEN_G_ITEM,TRUE);
	else enable(menuh,OPEN_G_ITEM,FALSE);
	}
else
	{
	if (doc == 0L)
		{
		//enable(menuh,PRNT_PATCHES,FALSE);
		enable(menuh,NEW_PROFILE_ITEM,FALSE);
		enable(menuh,OPEN_PROFILE_ITEM,FALSE);
		enable(menuh,SAVE_PROFILE,FALSE);
		enable(menuh,SAVE_PROFILE_AS,FALSE);
		enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
		enable(emenuh,EDIT_PREF,FALSE);
		enable(menuh,REVERT_PROFILE,FALSE);
		}
	else
		{
		//enable(menuh,PRNT_PATCHES,TRUE);
		if (doc->isMeasuring()) enable(emenuh,EDIT_PREF,FALSE);
		else enable(emenuh,EDIT_PREF,TRUE);
		if (_monacoprint) enable(menuh,OPEN_G_ITEM,TRUE);
		else enable(menuh,OPEN_G_ITEM,FALSE);
		enable(menuh,NEW_PROFILE_ITEM,FALSE);
		if (doc->locked) 
			{
			enable(menuh,OPEN_PROFILE_ITEM,FALSE);
			//enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
			enable(menuh,SAVE_PROFILE,FALSE);
			enable(menuh,SAVE_PROFILE_AS,FALSE);
			}
		else 
			{
			if ((((ProfileDoc*)doc)->inputStatus() & Processing_Input1) || 
				(((ProfileDoc*)doc)->inputStatus() & Processing_Input2) ||
				(((ProfileDoc*)doc)->inputStatus() & Tweaking_Input))
				{
				enable(menuh,OPEN_PROFILE_ITEM,FALSE);
				//enable(menuh,CLOSE_PROFILE_ITEM,FALSE);
				enable(menuh,SAVE_PROFILE,FALSE);
				enable(menuh,SAVE_PROFILE_AS,FALSE);
				}
			else
				{
				enable(menuh,OPEN_PROFILE_ITEM,FALSE);
				//enable(menuh,CLOSE_PROFILE_ITEM,TRUE);
				enable(menuh,SAVE_PROFILE,TRUE);
				enable(menuh,SAVE_PROFILE_AS,TRUE);
				}
			if (((ProfileDoc*)doc)->inputStatus() & Getting_Input)
				{
				if (deviceh) enable(deviceh,1,FALSE);
				if (deviceh) enable(deviceh,2,FALSE);
				}
			else
				{
				if (deviceh) enable(deviceh,1,TRUE);
				if (deviceh) enable(deviceh,2,TRUE);
				}
			// set device availablity
			// only handhelds and none if in tweaking window
			if (((ProfileDoc*)doc)->inputStatus() & Tweaking_Input)
				{
				if (devh) enable(devh,DEVICE_NONE,TRUE);
				if (devh) enable(devh,DEVICE_DTP51,FALSE);
				if (devh) enable(devh,DEVICE_DTP41,FALSE);
				if (devh) enable(devh,DEVICE_XRITE408,FALSE);
				if (devh) enable(devh,DEVICE_TECH_CP300,TRUE);
				if (devh) enable(devh,DEVICE_TECH_SP820,TRUE);
				if (devh) enable(devh,DEVICE_TECH_TCR_CP300,FALSE);
				if (devh) enable(devh,DEVICE_TECH_TCR_SP820,FALSE);
				if (devh) enable(devh,DEVICE_GRE_HAND,TRUE);
				if (devh) enable(devh,DEVICE_GRE_TABLE,FALSE);
				}
			else
				{
				if (devh) enable(devh,DEVICE_NONE,TRUE);
				if (devh) enable(devh,DEVICE_DTP51,TRUE);
				if (devh) enable(devh,DEVICE_DTP41,TRUE);
				if (devh) enable(devh,DEVICE_XRITE408,TRUE);
				if (devh) enable(devh,DEVICE_TECH_CP300,TRUE);
				if (devh) enable(devh,DEVICE_TECH_SP820,TRUE);
				if (devh) enable(devh,DEVICE_TECH_TCR_CP300,TRUE);
				if (devh) enable(devh,DEVICE_TECH_TCR_SP820,TRUE);
				if (devh) enable(devh,DEVICE_GRE_HAND,TRUE);
				if (devh) enable(devh,DEVICE_GRE_TABLE,TRUE);
				} 
			// set checkmarks for current device and port
			if (devh) for (i=1; i<=8; i++)
				{
				if ((((ProfileDocInf*)doc)->defaults->input_type == DT_None) && (i == 1))  CheckItem(devh,i,1);
				else if (((ProfileDocInf*)doc)->defaults->input_type == (DeviceTypes)(i+1)) CheckItem(devh,i,1);
				else CheckItem(devh,i,0);
				}
			if (porth) for (i=1; i<=2; i++)
				{
				if (((ProfileDocInf*)doc)->defaults->port == i)  CheckItem(porth,i,1);
				else CheckItem(porth,i,0);
				} 
			}
		
	//	if (((doc->dataPresent()) || (!((ProfileDocInf*)doc)->monacoProfile) || (((ProfileDocInf*)doc)->monacoEdited)) && 
	//		((ProfileDocInf*)doc)->validCal())
		if (doc->checkDocCanBeSave())
			{
			enable(menuh,SAVE_PROFILE,TRUE);
			if (doc->validFile()) enable(menuh,SAVE_PROFILE_AS,TRUE);
			else enable(menuh,SAVE_PROFILE_AS,FALSE);
			//if ((((ProfileDocInf*)doc)->monacoProfile) && (!((ProfileDocInf*)doc)->createdProfile))
			if (doc->validFile()) enable(menuh,REVERT_PROFILE,TRUE);
			else enable(menuh,REVERT_PROFILE,FALSE);
			//else enable(menuh,REVERT_PROFILE,FALSE);
			}
		else 
			{
			enable(menuh,SAVE_PROFILE,FALSE);
			enable(menuh,SAVE_PROFILE_AS,FALSE);
			enable(menuh,REVERT_PROFILE,FALSE);
			}
		}
	}



enable(emenuh,EDIT_UNDO,FALSE);
enable(emenuh,EDIT_CUT,FALSE);
enable(emenuh,EDIT_COPY,FALSE);
enable(emenuh,EDIT_PASTE,FALSE);

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
					if ((frontWin->WinType == Gammut_Dialog1) || 
						(frontWin->WinType == Gammut_Dialog_Src) || 
						(frontWin->WinType == Gammut_Dialog_Dst)) 
						removeWindow(frontWin);
					else
						{
						nwcode = 1;
						wc[0].code = WE_Doc_Close_Save;
						wc[0].wintype = frontWin->WinType;
						wc[0].winnum = frontWin->WinNum;
						wc[0].doc = FindDocFromWin(frontWin);
						state = DoCommand(wc,nwcode,where,NULL,finish,changed);
						if (state != MCO_SUCCESS) McoErrorAlert(state);
						}
					break;
				case SAVE_PROFILE:
					nwcode = 1;
					wc[0].code = WE_Doc_Save;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case SAVE_PROFILE_AS:	
					nwcode = 1;
					wc[0].code = WE_Doc_Save_As;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
					break;
				case REVERT_PROFILE:
					nwcode = 1;
					wc[0].code = WE_Revert;
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
			  	case QUIT_ITEM:
			  		if (checkCloseDocs(NULL)) *finish = TRUE;
			  		break;	  	
			}
			break; 
		case EDIT_MENU:
			 switch (menuItem){
			 	case EDIT_PREF:
			 		nwcode = 1;
					wc[0].code = WE_Set_Pref;
					wc[0].wintype = frontWin->WinType;
					wc[0].winnum = frontWin->WinNum;
					wc[0].doc = FindDocFromWin(frontWin);
					state = DoCommand(wc,nwcode,where,NULL,finish,changed);
					if (state != MCO_SUCCESS) McoErrorAlert(state);
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
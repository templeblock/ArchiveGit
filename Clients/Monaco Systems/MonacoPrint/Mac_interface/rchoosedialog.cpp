#include "rchoosedialog.h"

#include "pascal_string.h"
#include "fullname.h"
#include "monacoprint.h"
#include "profileDocinf.h"
#include "think_bugs.h"


#define Table_menu	(10000)

RchooseDialog::RchooseDialog(ProfileDocInf *d,int den)
{
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	long		id;
	int 		i;
	MenuHandle menuH;
	int 		which;

	doc = d;
	density = den;
	
	_which = doc->defaults->input_type;
	_which_patch = doc->defaults->patch_format;
	_which_port = doc->defaults->port;
	
	if (_which == DT_None) _which = DT_TechkonSP820;
	
	switch (_which) {
		case DT_TiffFile:
			which = DataFile;
			break;
		case DT_DTP51:
			which = DTP51;
			break;
		case DT_XriteDTP41:
			which = DTP41;
			break;
		case DT_TechkonCP300:
			which = TechkonCP300;
			break;
		case DT_TechkonSP820:
			which = TechkonSP820;
			break;
		case DT_TechkonTCRCP300:
			which = TechkonTCRCP300;
			break;
		case DT_TechkonTCRSP820:
			which = TechkonTCRSP820;
			break;
		case DT_GretagHand:
			which = GretagHand;
			break;
		case DT_GretagTable:
			which = GretagTable;
			break;
		case DT_Xrite408:
			which = XR408;
			break;
		}	
	

	setDialog(Rchoose_Dialog);
	frame_button();	

	for (i=0; i<MAX_PATCH_NAMES; i++) 
		{
		convert[i] = 0;
		convert2[i] = 0;
		}
		
	GetDItem (dialogptr, FileType, &iType, &iHandle, &iRect);
	SetCtlValue((ControlHandle)iHandle,which);
	
	if ((XR408 != -1) && (!density))
		{
		menuH = GetMenu(DeviceMenuID);
		DisableItem(menuH,XR408);
		}

	setDialogState();
}	


RchooseDialog::~RchooseDialog(void)
{
}




void 	RchooseDialog::setDialogState(void)
{
int i,m;
MenuHandle menuH;
short menucount;
	short		item;	
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	
	
switch (_which) {
	case DT_TiffFile:
		if (DataFile < 0) _which = 1;
		break;
	case DT_DTP51:
		if (DTP51 < 0) _which = 1;
		break;
	case DT_XriteDTP41:
		if (DTP41 < 0) _which = 1;
		break;
	case DT_TechkonCP300:
		if (TechkonCP300 < 0) _which = 1;
		break;
	case DT_TechkonSP820:
		if (TechkonSP820 < 0) _which = 1;
		break;
	case DT_TechkonTCRCP300:
		if (TechkonTCRCP300 < 0) _which = 1;
		break;
	case DT_TechkonTCRSP820:
		if (TechkonTCRSP820 < 0) _which = 1;
		break;
	case DT_GretagHand:
		if (GretagHand < 0) _which = 1;
		break;
	case DT_GretagTable:
		if (GretagTable < 0) _which = 1;
		break;
	case DT_Xrite408:
		if (XR408 < 0) _which = 1;
		break;
	}	
	

switch (_which) {
	case (DT_DataFile):
		Disable(dialogptr,PatchFormat);
		Disable(dialogptr,DevicePort);
		menuH = GetMenu(Table_menu);
		menucount = CountMItems(menuH);

		for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);
			
		_which_patch = 1;
		GetDItem (dialogptr, PatchFormat, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,_which_patch);
		DrawControls(dialogptr);
		break;
	case (DT_DTP51):
		Enable(dialogptr,PatchFormat);
		Enable(dialogptr,DevicePort);
		menuH = GetMenu(Table_menu);
		menucount = CountMItems(menuH);

		for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);
		
		m = 1;
		for (i=0; i<doc->patchset->filenum; i++)
			{
			if (doc->patchset->patches[i] != NULL)
				{
				if ( doc->patchset->patches[i]->patches.strips == 1) 
					{
					convert[m] = i;
					convert2[i] = m;
					InsMenuItem(menuH, doc->patchset->patchames[i], m++);
					}
				}
			}
			
		//_which_patch = 1;
		GetDItem (dialogptr, PatchFormat, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,convert2[_which_patch]);
		GetDItem (dialogptr, DevicePort, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,_which_port);
		DrawControls(dialogptr);
		break;
	case (DT_TiffFile):
		Enable(dialogptr,PatchFormat);
		Disable(dialogptr,DevicePort);
		
		menuH = GetMenu(Table_menu);
		menucount = CountMItems(menuH);

		for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);
		
		m = 1;
		for (i=0; i<doc->patchset->filenum; i++)
			{
			if (doc->patchset->patches[i] != NULL)
				{
				if (doc->patchset->patches[i]->patches.strips == 0) 
					{
					convert[m] = i;
					convert2[i] = m;
					InsMenuItem(menuH, doc->patchset->patchames[i], m++);
					}
				}
			}
		//_which_patch = 1;
		GetDItem (dialogptr, PatchFormat, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,convert2[_which_patch]);
		GetDItem (dialogptr, DevicePort, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,_which_port);
		DrawControls(dialogptr);
		break;
	case (DT_TechkonCP300):
	case (DT_TechkonSP820):
	case (DT_TechkonTCRCP300):
	case (DT_TechkonTCRSP820):
	case (DT_GretagHand):
	case (DT_GretagTable):
		Enable(dialogptr,PatchFormat);
		Enable(dialogptr,DevicePort);
		
		menuH = GetMenu(Table_menu);
		menucount = CountMItems(menuH);

		for( i = menucount; i > 0; i-- )
			DelMenuItem(menuH,i);
		
		m = 1;
		for (i=0; i<doc->patchset->filenum; i++)
			{
			if (doc->patchset->patches[i] != NULL)
				{
				if (doc->patchset->patches[i]->patches.strips == 0) 
					{
					convert[m] = i;
					convert2[i] = m;
					InsMenuItem(menuH, doc->patchset->patchames[i], m++);
					}
				}
			}
		//_which_patch = 1;
		GetDItem (dialogptr, PatchFormat, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,convert2[_which_patch]);
		GetDItem (dialogptr, DevicePort, &iType, &iHandle, &iRect);
		SetCtlValue((ControlHandle)iHandle,_which_port);
		DrawControls(dialogptr);
		break;
	case (DT_Xrite408):
		Disable(dialogptr,PatchFormat);
		break;
	}
	
if ((!density) && (_which == DT_Xrite408)) Disable(dialogptr,1);
else Enable(dialogptr,1);	

}


void RchooseDialog::_Savesettings(void)
{


/*
switch (_which) {
	case TiffFile:
		doc->defaults->input_type = DT_TiffFile;
		break;
	case DTP51:
		doc->defaults->input_type = DT_DTP51;
		break;
	case DTP41:
		doc->defaults->input_type = DT_XriteDTP41;
		break;
	case TechkonCP300:
		doc->defaults->input_type = DT_TechkonCP300;
		break;
	case TechkonSP820:
		doc->defaults->input_type = DT_TechkonSP820;
		break;
	case TechkonTCRCP300:
		doc->defaults->input_type = DT_TechkonTCRCP300;
		break;
	case TechkonTCRSP820:
		doc->defaults->input_type = DT_TechkonTCRSP820;
		break;
	case GretagHand:
		doc->defaults->input_type = DT_GretagHand;
		break;
	case GretagTable:
		doc->defaults->input_type = DT_GretagTable;
		break;
	}*/
	
doc->defaults->input_type = (DeviceTypes)_which;	

doc->whichport = _which_port-1;
doc->defaults->patch_format = _which_patch;
doc->defaults->port = _which_port;

}

int		RchooseDialog::convertToDefault(int which)
{
switch (which) {
	case TiffFile:
		_which = DT_TiffFile;
		break;
	case DTP51:
		_which = DT_DTP51;
		break;
	case DTP41:
		_which = DT_XriteDTP41;
		break;
	case TechkonCP300:
		_which = DT_TechkonCP300;
		break;
	case TechkonSP820:
		_which = DT_TechkonSP820;
		break;
	case TechkonTCRCP300:
		_which = DT_TechkonTCRCP300;
		break;
	case TechkonTCRSP820:
		_which = DT_TechkonTCRSP820;
		break;
	case GretagHand:
		_which = DT_GretagHand;
		break;
	case GretagTable:
		_which = DT_GretagTable;
		break;
	case XR408:
		_which = DT_Xrite408;
		break;
	}

}


int16 	RchooseDialog::getchoice(void)
{
return _which;
}

int16	RchooseDialog::hit(void)
{	
	short		item;	
	short		iType;
	Handle		iHandle;
	Rect 		iRect;	
	int 		which;


	BringToFront(dialogptr);
	ShowWindow(dialogptr);

	do{
		ModalDialog(NULL, &item);
		switch(item){
			case FileType:
				GetDItem (dialogptr, FileType, &iType, &iHandle, &iRect);
				which = GetCtlValue((ControlHandle)iHandle);
				convertToDefault(which);
				setDialogState();
				break;
			case PatchFormat:
				GetDItem (dialogptr, PatchFormat, &iType, &iHandle, &iRect);
				which = GetCtlValue((ControlHandle)iHandle);
				_which_patch = convert[which];
				break;
			case DevicePort:
				GetDItem (dialogptr, DevicePort, &iType, &iHandle, &iRect);
				_which_port = GetCtlValue((ControlHandle)iHandle);
				 break;
	    	case 3:
	    	break;
	    	
	    	case 1:
	    	_Savesettings();
			break;	    	
	    }	
	 } while( item != 1 && item != 3);	
	
	if (item == 3) item = 0;
	return item;	    	
}




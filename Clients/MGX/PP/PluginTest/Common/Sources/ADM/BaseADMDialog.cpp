/*
 * Name:
 *	BaseADMDialog.cpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Dialog Base Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.1.0 3/11/1996	DL	Tenth version.
 *		Created by Dave Lazarony.
 */

/*
 * Includes
 */

#include "BaseADMDialog.hpp"
#include "IADMDrawer.hpp"
#include "IADMItem.hpp"
#include "IADMNotifier.hpp"
#include "IADMTracker.hpp"
#include "IASGlobalContext.hpp"

/*
 * Constructor
 */
 
BaseADMDialog::BaseADMDialog()	
: IADMDialog()							
{
	fDialog = nil;
	fUserData = nil;
	fIsModal = false;
}


/*
 * Destructor
 */

BaseADMDialog::~BaseADMDialog()
{
	if (fDialog && !fIsModal)
	{
		/*
		 * Because Destroy() will try to call use through the Destroy Proc
		 * we first have to remove ourselves from the ADM dialog so we cannot be 
		 * called again.
		 */
		sADMDialog->SetUserData(fDialog, nil);
		sADMDialog->SetDrawProc(fDialog, nil);
		sADMDialog->SetNotifyProc(fDialog, nil);
		sADMDialog->SetDestroyProc(fDialog, nil);
		
		/*
		 * Destroy the ADM dialog.
		 */	
		sADMDialog->Destroy(fDialog);
		
		fDialog = nil;
	}
}

ADMDialogRef BaseADMDialog::Create(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style)								
{
	fDialog = sADMDialog->Create(pluginRef, name, dialogID, style, InitProc, this);
	
	return fDialog;	
}


int BaseADMDialog::Modal(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style)								
{
	fIsModal = true;

	int result = sADMDialog->Modal(pluginRef, name, dialogID, style, InitProc, this);

	// Used to set fDialog to nil here, but the destroy method (below) deletes the dialog,
	// so it's gone by this point.
	
	return result;
}


/*
 * Virtual Functions that call Defaults.
 */
	
ASErr BaseADMDialog::Init()
{
	sADMDialog->SetDrawProc(fDialog, DrawProc);
	sADMDialog->SetTrackProc(fDialog, TrackProc);
	sADMDialog->SetNotifyProc(fDialog, NotifyProc);
	sADMDialog->SetDestroyProc(fDialog, DestroyProc);

	return kNoErr;
}

void BaseADMDialog::Draw(IADMDrawer drawer)
{
	sADMDialog->DefaultDraw(fDialog, drawer);
}

ASBoolean BaseADMDialog::Track(IADMTracker tracker)
{
	return sADMDialog->DefaultTrack(fDialog, tracker);
}

void BaseADMDialog::Notify(IADMNotifier notifier)
{
	sADMDialog->DefaultNotify(fDialog, notifier);
}

void BaseADMDialog::Destroy()
{
}

/*
 * Static Procs that call Virtual Functions.
 */
	
ASErr ASAPI BaseADMDialog::InitProc(ADMDialogRef dialog)
{
	IASGlobalContext globalContext;
	BaseADMDialog *admDialog = (BaseADMDialog *)sADMDialog->GetUserData(dialog);
	
	admDialog->fDialog = dialog;
	
	if (admDialog)
		return admDialog->Init();
	else
		return kBadParameterErr;
}

void ASAPI BaseADMDialog::DrawProc(ADMDialogRef dialog, ADMDrawerRef drawer)
{
	IASGlobalContext globalContext;
	BaseADMDialog *admDialog = (BaseADMDialog *)sADMDialog->GetUserData(dialog);
	
	if (admDialog)
		admDialog->Draw(drawer);
}

ASBoolean ASAPI BaseADMDialog::TrackProc(ADMDialogRef dialog, ADMTrackerRef tracker)
{
	IASGlobalContext globalContext;
	BaseADMDialog *admDialog = (BaseADMDialog *)sADMDialog->GetUserData(dialog);
	
	if (admDialog)
		return admDialog->Track(tracker);
	else
		return false;
}

void ASAPI BaseADMDialog::NotifyProc(ADMDialogRef dialog, ADMNotifierRef notifier)
{
	IASGlobalContext globalContext;
	BaseADMDialog *admDialog = (BaseADMDialog *)sADMDialog->GetUserData(dialog);
	
	if (admDialog)
		admDialog->Notify(notifier);
}

void ASAPI BaseADMDialog::DestroyProc(ADMDialogRef dialog)
{
	IASGlobalContext globalContext;
	BaseADMDialog *admDialog = (BaseADMDialog *)sADMDialog->GetUserData(dialog);
	
	if (admDialog) {

		admDialog->Destroy();

		if (admDialog->fIsModal)
		{
			// If it is a modal dialog, we just want to set fDialog to nil
			// so we don't try to reference through it in the BaseADMDialog destructor.
			// The dialog (inside ADM) is deleted right after the destroy proc. is called.
			// If we don't set it to nil here we're hosed...
			// dtomack 1/16/98
			admDialog->fDialog = nil;
		}
		else
		{
			delete admDialog;
		}
	}
}

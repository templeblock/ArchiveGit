/*
 * Name:
 *	IADMDialog.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Dialog Interface Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IADMDialog_hpp__
#define __IADMDialog_hpp__

/*
 * Includes
 */
 
#ifndef __ADMDialog__
#include "ADMDialog.h"
#endif

#ifndef __IASTypes_hpp__
#include "IASTypes.hpp"
#endif

// Stupid Macintosh Headers #define GetItem.
#ifdef GetItem
#undef GetItem
#endif


/*
 * Global Suite Pointer
 */

extern "C" ADMDialogSuite *sADMDialog;

/*
 * Interface Wrapper Class
 */

class IADMDialog
{
protected:
	ADMDialog *fDialog;
	
public:	
	IADMDialog();
	IADMDialog(ADMDialogRef dialog);
	IADMDialog(int nilDialog);
	
	operator ADMDialogRef(void);
	
	ADMDialogRef GetDialogRef(void);
	void SetDialogRef(ADMDialogRef);
	
	void SendNotify(const char *notifierType = nil);
	void DefaultNotify(ADMNotifierRef notifier);
	
	int GetID();
	SPPluginRef GetPluginRef();
	ASWindowRef GetWindowRef();

	char* GetName();
	
	void SetDialogStyle(ADMDialogStyle style);
	ADMDialogStyle GetDialogStyle();
	
	void Show(ASBoolean show = true);
	void Hide();
	ASBoolean IsVisible();
	
	void Enable(ASBoolean enable = true);
	void Disable();
	ASBoolean IsEnabled();
	
	void Activate(ASBoolean activate = true);
	void Deactivate();
	ASBoolean IsActive();
	
	void SetLocalRect(const IASRect &localRect);
	void GetLocalRect(IASRect &localRect);
	
	void SetBoundsRect(const IASRect &boundsRect);
	void GetBoundsRect(IASRect &boundsRect);
	
	void Move(int x, int y);
	void Size(int width, int height);
	
	void LocalToScreenPoint(IASPoint &point);
	void ScreenToLocalPoint(IASPoint &point);
	
	void LocalToScreenRect(IASRect &rect);
	void ScreenToLocalRect(IASRect &rect);
	
	void Invalidate();
	void Update();
	
	void SetCursorID(int cursorResID);
	int GetCursorID();

	void GetCursorID(SPPluginRef *plugin, int *cursorResID);

	ADMFont GetFont();
	void SetFont(ADMFont font);
	
	void SetText(const char *text);
	void GetText(char *text, int maxLength = 0);
	int GetTextLength();
	
	void SetMinWidth(int width);
	int GetMinWidth();
	
	void SetMinHeight(int height);
	int GetMinHeight();
	
	void SetMaxWidth(int width);
	int GetMaxWidth();
	
	void SetMaxHeight(int height);
	int GetMaxHeight();
	
	void SetHorizontalIncrement(int height);
	int GetHorizontalIncrement();
	
	void SetVerticalIncrement(int height);
	int GetVerticalIncrement();
	
	ADMItemRef GetItem(int itemID);
	
	ADMItemRef CreateItem(int itemID, ADMItemType itemType, const IASRect &boundsRect, ADMItemInitProc initProc = nil, ADMUserData data = 0);
	void DestroyItem(ADMItemRef item);
	
	ADMItemRef GetNextItem(ADMItemRef item);
	ADMItemRef GetPreviousItem(ADMItemRef item);

	void SetDefaultItemID(int itemID);
	int GetDefaultItemID();
	void SetCancelItemID(int itemID);
	int GetCancelItemID();
	
	void EndModal(int modalResultID, ASBoolean cancelling = false);

	void LoadToolTips(int stringID);

	void SetHelpID(ASHelpID helpID);
	ASHelpID GetHelpID();
	void Help();

	void SetMask(ADMActionMask mask);
	ADMActionMask GetMask();

	void SetNotifierData(ADMUserData data);
	ADMUserData GetNotifierData();

	ADMTimerRef CreateTimer(unsigned long milliseconds,
							ADMActionMask abortMask,
							ADMDialogTimerProc timerProc,
							ADMDialogTimerAbortProc abortProc);

	void AbortTimer(ADMTimerRef timer);

	ASBoolean IsCollapsed();
};


inline IADMDialog::IADMDialog()								
{
}

inline IADMDialog::IADMDialog(ADMDialogRef dialog)	
{
	fDialog = dialog;
}

inline IADMDialog::IADMDialog(int nilDialog)
{
	if (nilDialog == nil)
		fDialog = (ADMDialog *)nil;
}

inline IADMDialog::operator ADMDialogRef(void)
{
	return fDialog;
}

inline ADMDialogRef IADMDialog::GetDialogRef(void)
{
	return fDialog;
}

inline void IADMDialog::SetDialogRef(ADMDialogRef dialog)
{
	fDialog = dialog;
}


inline void IADMDialog::SendNotify(const char *notifierType)
{
	sADMDialog->SendNotify(fDialog, (char *)notifierType);
}

inline void IADMDialog::DefaultNotify(ADMNotifierRef notifier)
{
	sADMDialog->DefaultNotify(fDialog, notifier);
}

inline int IADMDialog::GetID()
{
	return sADMDialog->GetID(fDialog);
}

inline struct SPPlugin *IADMDialog::GetPluginRef()
{
	return sADMDialog->GetPluginRef(fDialog);
}

inline ASWindowRef IADMDialog::GetWindowRef()
{
	return sADMDialog->GetWindowRef(fDialog);
}


inline char* IADMDialog::GetName()
{
	return sADMDialog->GetDialogName(fDialog);
}


inline void IADMDialog::SetDialogStyle(ADMDialogStyle style)
{
	sADMDialog->SetDialogStyle(fDialog, style);
}

inline ADMDialogStyle IADMDialog::GetDialogStyle()
{
	return sADMDialog->GetDialogStyle(fDialog);
}

inline void IADMDialog::Show(ASBoolean show)
{
	sADMDialog->Show(fDialog, show);
}

inline void IADMDialog::Hide()
{
	sADMDialog->Show(fDialog, false);
}

inline ASBoolean IADMDialog::IsVisible()
{
	return sADMDialog->IsVisible(fDialog);
}

inline void IADMDialog::Enable(ASBoolean enable)
{
	sADMDialog->Enable(fDialog, enable);
}

inline void IADMDialog::Disable()
{
	sADMDialog->Enable(fDialog, false);
}

inline ASBoolean IADMDialog::IsEnabled()
{
	return sADMDialog->IsEnabled(fDialog);
}

inline void IADMDialog::Activate(ASBoolean activate)
{
	sADMDialog->Activate(fDialog, activate);
}

inline void IADMDialog::Deactivate()
{
	sADMDialog->Activate(fDialog, false);
}

inline ASBoolean IADMDialog::IsActive()
{
	return sADMDialog->IsActive(fDialog);
}

inline void IADMDialog::SetLocalRect(const IASRect &localRect)
{
	sADMDialog->SetLocalRect(fDialog, (IASRect *)&localRect);
}

inline void IADMDialog::GetLocalRect(IASRect &localRect)
{
	sADMDialog->GetLocalRect(fDialog, &localRect);
}

inline void IADMDialog::SetBoundsRect(const IASRect &boundsRect)
{
	sADMDialog->SetBoundsRect(fDialog, (IASRect *)&boundsRect);
}

inline void IADMDialog::GetBoundsRect(IASRect &boundsRect)
{
	sADMDialog->GetBoundsRect(fDialog, &boundsRect);
}

inline void IADMDialog::Move(int x, int y)
{
	sADMDialog->Move(fDialog, x, y);
}

inline void IADMDialog::Size(int width, int height)
{
	sADMDialog->Size(fDialog, width, height);
}

inline void IADMDialog::LocalToScreenPoint(IASPoint &point)
{
	sADMDialog->LocalToScreenPoint(fDialog, (ASPoint *)&point);
}

inline void IADMDialog::ScreenToLocalPoint(IASPoint &point)
{
	sADMDialog->ScreenToLocalPoint(fDialog, (ASPoint *)&point);
}

inline void IADMDialog::LocalToScreenRect(IASRect &rect)
{
	sADMDialog->LocalToScreenRect(fDialog, (ASRect *)&rect);
}

inline void IADMDialog::ScreenToLocalRect(IASRect &rect)
{
	sADMDialog->ScreenToLocalRect(fDialog, (ASRect *)&rect);
}
		
inline void IADMDialog::Invalidate()
{
	sADMDialog->Invalidate(fDialog);
}

inline void IADMDialog::Update()
{
	sADMDialog->Update(fDialog);
}



inline void IADMDialog::SetCursorID(int cursorResID)
{
	sADMDialog->SetCursorID(fDialog, sADMDialog->GetPluginRef(fDialog), cursorResID);
}

inline int IADMDialog::GetCursorID()
{
	int cursorID = 0;
	SPPluginRef itsPlugin;
	sADMDialog->GetCursorID(fDialog, &itsPlugin, &cursorID);
	return cursorID;
}

inline void IADMDialog::GetCursorID(SPPluginRef *plugin, int *cursorResID)
{
	*cursorResID = 0;
	sADMDialog->GetCursorID(fDialog, plugin, cursorResID);
}

inline ADMFont IADMDialog::GetFont()
{
	return sADMDialog->GetFont(fDialog);
}

inline void IADMDialog::SetFont(ADMFont font)
{
	sADMDialog->SetFont(fDialog, font);
}
	
inline void IADMDialog::SetText(const char *text)
{
	sADMDialog->SetText(fDialog, (char *)text);
}

inline void IADMDialog::GetText(char *text, int maxLength)
{
	sADMDialog->GetText(fDialog, text, maxLength);
}

inline int IADMDialog::GetTextLength()
{
	return sADMDialog->GetTextLength(fDialog);
}

inline void IADMDialog::SetMinWidth(int width)
{
	sADMDialog->SetMinWidth(fDialog, width);
}

inline int IADMDialog::GetMinWidth()
{
	return sADMDialog->GetMinWidth(fDialog);
}

inline void IADMDialog::SetMinHeight(int height)
{
	sADMDialog->SetMinHeight(fDialog, height);
}

inline int IADMDialog::GetMinHeight()
{
	return sADMDialog->GetMinHeight(fDialog);
}

inline void IADMDialog::SetMaxWidth(int width)
{
	sADMDialog->SetMaxWidth(fDialog, width);
}

inline int IADMDialog::GetMaxWidth()
{
	return sADMDialog->GetMaxWidth(fDialog);
}

inline void IADMDialog::SetMaxHeight(int height)
{
	sADMDialog->SetMaxHeight(fDialog, height);
}

inline int IADMDialog::GetMaxHeight()
{
	return sADMDialog->GetMaxHeight(fDialog);
}

inline void IADMDialog::SetHorizontalIncrement(int increment)
{
	sADMDialog->SetHorizontalIncrement(fDialog, increment);
}

inline int IADMDialog::GetHorizontalIncrement()
{
	return sADMDialog->GetHorizontalIncrement(fDialog);
}

inline void IADMDialog::SetVerticalIncrement(int increment)
{
	sADMDialog->SetVerticalIncrement(fDialog, increment);
}

inline int IADMDialog::GetVerticalIncrement()
{
	return sADMDialog->GetVerticalIncrement(fDialog);
}

inline ADMItemRef IADMDialog::GetItem(int itemID)
{
	return sADMDialog->GetItem(fDialog, itemID);
}

inline ADMItemRef IADMDialog::CreateItem(int itemID, ADMItemType itemType, const IASRect &boundsRect, ADMItemInitProc initProc, ADMUserData data)
{
	return sADMDialog->CreateItem(fDialog, itemID, itemType, (ASRect *)&boundsRect, initProc, data);
}

inline void IADMDialog::DestroyItem(ADMItemRef item)	
{
	sADMDialog->DestroyItem(fDialog, item);
}

inline ADMItemRef IADMDialog::GetNextItem(ADMItemRef item)
{
	return sADMDialog->GetNextItem(fDialog, item);
}

inline ADMItemRef IADMDialog::GetPreviousItem(ADMItemRef item)
{
	return sADMDialog->GetPreviousItem(fDialog, item);
}

inline void IADMDialog::SetDefaultItemID(int itemID)
{
	sADMDialog->SetDefaultItemID(fDialog, itemID);
}

inline int IADMDialog::GetDefaultItemID()
{
	return sADMDialog->GetDefaultItemID(fDialog);
}

inline void IADMDialog::SetCancelItemID(int itemID)
{
	sADMDialog->SetCancelItemID(fDialog, itemID);
}

inline int IADMDialog::GetCancelItemID()
{
	return sADMDialog->GetCancelItemID(fDialog);
}

inline void IADMDialog::EndModal(int modalResultID, ASBoolean cancelling)
{
	sADMDialog->EndModal(fDialog, modalResultID, cancelling);
}
	
inline void IADMDialog::LoadToolTips(int stringID)
{
	sADMDialog->LoadToolTips(fDialog, stringID);
}

inline void IADMDialog::SetHelpID(ASHelpID helpID)
{
	sADMDialog->SetHelpID(fDialog, helpID);
}

inline ASHelpID IADMDialog::GetHelpID()
{
	return sADMDialog->GetHelpID(fDialog);
}

inline void IADMDialog::Help()
{
	sADMDialog->Help(fDialog);
}

inline void IADMDialog::SetMask(ADMActionMask mask)
{
	sADMDialog->SetMask(fDialog, mask);
}

inline ADMActionMask IADMDialog::GetMask()
{
	return sADMDialog->GetMask(fDialog);
}

inline void IADMDialog::SetNotifierData(ADMUserData data)
{
	sADMDialog->SetNotifierData(fDialog, data);
}

inline ADMUserData IADMDialog::GetNotifierData()
{
	return sADMDialog->GetNotifierData(fDialog);
}

inline ADMTimerRef IADMDialog::CreateTimer(unsigned long milliseconds,
		ADMActionMask abortMask, ADMDialogTimerProc timerProc,
		ADMDialogTimerAbortProc abortProc)
{
	return sADMDialog->CreateTimer(fDialog, milliseconds,
		abortMask, timerProc, abortProc);
}

inline void IADMDialog::AbortTimer(ADMTimerRef timer)
{
	sADMDialog->AbortTimer(fDialog, timer);
}

inline ASBoolean IADMDialog::IsCollapsed()
{
	return sADMDialog->IsCollapsed(fDialog);
}

#endif

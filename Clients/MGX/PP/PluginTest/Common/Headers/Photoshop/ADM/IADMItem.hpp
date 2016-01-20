/*
 * Name:
 *	IADMItem.hpp
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	Adobe Dialog Manager Item Interface Class.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 3/6/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __IADMItem_hpp__
#define __IADMItem_hpp__

/*
 * Includes
 */
 
#ifndef __ADMItem__
#include "ADMItem.h"
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

extern "C" ADMItemSuite *sADMItem;


/*
 * Interface Wrapper Class
 */

class IADMItem
{
protected:
	ADMItem *fItem;

public:	
	IADMItem();
	IADMItem(ADMItemRef item);
	IADMItem(int nilItem);

	operator ADMItemRef(void);
		
	void SendNotify(const char *notifierType = nil);
	void SetNotifyProc(ADMItemNotifyProc proc);
	
	int GetID();
	ADMDialogRef GetDialog();
	ASWindowRef GetWindowRef();
	ADMListRef GetList();
	ADMHierarchyListRef GetHierarchyList();

	SPPluginRef GetPluginRef();
	void SetPluginRef(SPPluginRef pluginRef);

	void SetItemType(ADMItemType type);
	ADMItemType GetItemType();
	
	void SetItemStyle(ADMItemStyle style);
	ADMItemStyle GetItemStyle();
	
	void Show(ASBoolean show = true);
	void Hide();
	ASBoolean IsVisible();
	
	void Enable(ASBoolean enable = true);
	void Disable();
	ASBoolean IsEnabled();
	
	void Activate(ASBoolean activate = true);
	void Deactivate();
	ASBoolean IsActive();
	
	void Known(ASBoolean known = true);
	void Unknown();
	ASBoolean IsKnown();
	
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

	void GetCursorID(SPPluginRef *pluginRef, int *cursorID);

	ADMFont GetFont();
	void SetFont(ADMFont font);
	
	void SetPictureID(int pictureResID);
	int GetPictureID();
	
	void SetSelectedPictureID(int pictureResID);
	int GetSelectedPictureID();
	
	void SetDisabledPictureID(int pictureResID);
	int GetDisabledPictureID();
	
	void SetMask(ADMActionMask mask);
	ADMActionMask GetMask();

	void SetUserData(ADMUserData data);
	ADMUserData GetUserData();

	void SetNotifierData(ADMUserData data);
	ADMUserData GetNotifierData();

	ADMTimerRef CreateTimer(unsigned long milliseconds,
							ADMActionMask abortMask,
							ADMItemTimerProc timerProc,
							ADMItemTimerAbortProc abortProc);

	void AbortTimer(ADMTimerRef timer);

	/*
	 * Text
	 */
	 
	void SetText(const char *);
	void GetText(char *, int maxLength = 0);
	int GetTextLength();
	
	void SetMaxTextLength(int length);
	int GetMaxTextLength();
	
	void SelectAll();
	void SetSelectionRange(int selStart, int selEnd);
	void GetSelectionRange(int &selStart, int &selEnd);

	void SetJustify(ADMJustify justify);
	ADMJustify GetJustify();
	
	void SetUnits(ADMUnits units);
	ADMUnits GetUnits();	
	
	void SetAllowUnits(ASBoolean allow);
	ASBoolean GetAllowUnits();
	
	void SetAllowMath(ASBoolean allow);
	ASBoolean GetAllowMath();
	
	void ShowUnits(ASBoolean show);
	ASBoolean GetShowUnits();

	void SetFloatToTextProc(ADMItemFloatToTextProc proc);
	ADMItemFloatToTextProc GetFloatToTextProc();
	ASBoolean DefaultFloatToText(float value, char *text, int textLength);
	
	void SetTextToFloatProc(ADMItemTextToFloatProc proc);
	ADMItemTextToFloatProc GetTextToFloatProc();
	ASBoolean DefaultTextToFloat(char *text, float *value);

	ASBoolean WasPercentageChange();

	/*	

	 * Numerics
	 */
	 
	void SetPrecision(int numberOfDecimalPlaces);
	int GetPrecision();
	
	void SetBooleanValue(ASBoolean value);
	ASBoolean GetBooleanValue();
	
	void SetIntValue(int value);
	int GetIntValue();
		
	void SetFixedValue(ASFixed value);	
	IASFixed GetFixedValue();
	
	void SetFloatValue(float value);	
	float GetFloatValue();
	
	void SetMinIntValue(int value);
	int GetMinIntValue();
		
	void SetMinFixedValue(ASFixed value);	
	IASFixed GetMinFixedValue();
	
	void SetMaxIntValue(int value);
	int GetMaxIntValue();
	
	void SetMinFloatValue(float value);	
	float GetMinFloatValue();
	
	void SetMaxFixedValue(ASFixed value);	
	IASFixed GetMaxFixedValue();
	
	void SetMaxFloatValue(float value);	
	float GetMaxFloatValue();
	
	void SetSmallIncrement(float increment);	
	float GetSmallIncrement();
	
	void SetLargeIncrement(float increment);
	float GetLargeIncrement();	
	
	ADMItemRef GetChildItem(int childID);	

	void SetTipString(const char *tipStr);
	void GetTipString(char *tipStr, int maxLen);
	int GetTipStringLength();
	void EnableTip(ASBoolean enable = true);
	ASBoolean IsTipEnabled();
	void ShowToolTip(ASPoint *where = nil);
	void HideToolTip();

	void SetHelpID(ASHelpID helpID);
	ASHelpID GetHelpID();
	void Help();

	void GetBestSize(ASPoint *size);
};

inline IADMItem::IADMItem()								
{
}

inline IADMItem::IADMItem(ADMItemRef item)	
{
	fItem = item;
}

inline IADMItem::IADMItem(int nilItem)
{
	if (nilItem == nil)
		fItem = (ADMItem *)nilItem;
}

inline IADMItem::operator ADMItemRef(void)
{
	return fItem;
}

inline void IADMItem::SendNotify(const char *notifierType)
{
	sADMItem->SendNotify(fItem, (char *)notifierType);
}
	
inline void IADMItem::SetNotifyProc(ADMItemNotifyProc proc)
{
	sADMItem->SetNotifyProc(fItem, proc);
}
	
inline int IADMItem::GetID()
{
	return sADMItem->GetID(fItem);
}

inline ADMDialogRef IADMItem::GetDialog()
{
	return sADMItem->GetDialog(fItem);
}

inline ASWindowRef IADMItem::GetWindowRef()
{
	return sADMItem->GetWindowRef(fItem);
}

inline ADMListRef IADMItem::GetList()
{
	return sADMItem->GetList(fItem);
}

inline ADMHierarchyListRef IADMItem::GetHierarchyList()
{
	return sADMItem->GetHierarchyList(fItem);
}

inline void IADMItem::SetPluginRef(struct SPPlugin *pluginRef)
{
	sADMItem->SetPluginRef(fItem, pluginRef);
}

inline struct SPPlugin *IADMItem::GetPluginRef()
{
	return sADMItem->GetPluginRef(fItem);
}

inline void IADMItem::SetItemType(ADMItemType type)
{
	sADMItem->SetItemType(fItem, type);
}

inline ADMItemType IADMItem::GetItemType()
{
	return sADMItem->GetItemType(fItem);
}

inline void IADMItem::SetItemStyle(ADMItemStyle style)
{
	sADMItem->SetItemStyle(fItem, style);
}

inline ADMItemStyle IADMItem::GetItemStyle()
{
	return sADMItem->GetItemStyle(fItem);
}

inline void IADMItem::Show(ASBoolean show)
{
	sADMItem->Show(fItem, show);
}

inline void IADMItem::Hide()
{
	sADMItem->Show(fItem, false);
}

inline ASBoolean IADMItem::IsVisible()
{
	return sADMItem->IsVisible(fItem);
}

inline void IADMItem::Enable(ASBoolean enable)
{
	sADMItem->Enable(fItem, enable);
}

inline void IADMItem::Disable()
{
	sADMItem->Enable(fItem, false);
}

inline ASBoolean IADMItem::IsEnabled()
{
	return sADMItem->IsEnabled(fItem);
}

inline void IADMItem::Activate(ASBoolean activate)
{
	sADMItem->Activate(fItem, activate);
}

inline void IADMItem::Deactivate()
{
	sADMItem->Activate(fItem, false);
}

inline ASBoolean IADMItem::IsActive()
{
	return sADMItem->IsActive(fItem);
}

inline void IADMItem::Known(ASBoolean known)
{
	sADMItem->Known(fItem, known);
}

inline void IADMItem::Unknown()
{
	sADMItem->Known(fItem, false);
}

inline ASBoolean IADMItem::IsKnown()
{
	return sADMItem->IsKnown(fItem);
}

inline void IADMItem::SetLocalRect(const IASRect &localRect)
{
	sADMItem->SetLocalRect(fItem, (IASRect *)&localRect);
}

inline void IADMItem::GetLocalRect(IASRect &localRect)
{
	sADMItem->GetLocalRect(fItem, &localRect);
}

inline void IADMItem::SetBoundsRect(const IASRect &boundsRect)
{
	sADMItem->SetBoundsRect(fItem, (IASRect *)&boundsRect);
}

inline void IADMItem::GetBoundsRect(IASRect &boundsRect)
{
	sADMItem->GetBoundsRect(fItem, &boundsRect);
}

inline void IADMItem::Move(int x, int y)
{
	sADMItem->Move(fItem, x, y);
}

inline void IADMItem::Size(int width, int height)
{
	sADMItem->Size(fItem, width, height);
}

inline void IADMItem::LocalToScreenPoint(IASPoint &point)
{
	sADMItem->LocalToScreenPoint(fItem, (ASPoint *)&point);
}

inline void IADMItem::ScreenToLocalPoint(IASPoint &point)
{
	sADMItem->ScreenToLocalPoint(fItem, (ASPoint *)&point);
}

inline void IADMItem::LocalToScreenRect(IASRect &rect)
{
	sADMItem->LocalToScreenRect(fItem, (ASRect *)&rect);
}

inline void IADMItem::ScreenToLocalRect(IASRect &rect)
{
	sADMItem->ScreenToLocalRect(fItem, (ASRect *)&rect);
}
		
inline void IADMItem::Invalidate()
{
	sADMItem->Invalidate(fItem);
}

inline void IADMItem::Update()
{
	sADMItem->Update(fItem);
}

inline void IADMItem::SetCursorID(int cursorResID)
{
	sADMItem->SetCursorID(fItem, sADMItem->GetPluginRef(fItem), cursorResID);
}

inline int IADMItem::GetCursorID()
{
	int cursorID = 0;
	SPPluginRef itsPlugin;
	sADMItem->GetCursorID(fItem, &itsPlugin, &cursorID);
	return cursorID;
}

inline void IADMItem::GetCursorID(SPPluginRef *plugin, int *cursorResID)
{
	*cursorResID = 0;
	sADMItem->GetCursorID(fItem, plugin, cursorResID);
}

inline ADMFont IADMItem::GetFont()
{
	return sADMItem->GetFont(fItem);
}

inline void IADMItem::SetFont(ADMFont font)
{
	sADMItem->SetFont(fItem, font);
}
	
inline void IADMItem::SetPictureID(int pictureResID)
{
	sADMItem->SetPictureID(fItem, pictureResID);
}

inline int IADMItem::GetPictureID()
{
	return sADMItem->GetPictureID(fItem);
}

inline void IADMItem::SetSelectedPictureID(int pictureResID)
{
	sADMItem->SetSelectedPictureID(fItem, pictureResID);
}

inline int IADMItem::GetSelectedPictureID()
{
	return sADMItem->GetSelectedPictureID(fItem);
}

inline void IADMItem::SetDisabledPictureID(int pictureResID)
{
	sADMItem->SetDisabledPictureID(fItem, pictureResID);
}

inline int IADMItem::GetDisabledPictureID()
{
	return sADMItem->GetDisabledPictureID(fItem);
}

/*
 * Text
 */
 
inline void IADMItem::SetText(const char *text)
{
	sADMItem->SetText(fItem, (char *)text);
}

inline void IADMItem::GetText(char *text, int maxLength)
{
	sADMItem->GetText(fItem, text, maxLength);
}

inline int IADMItem::GetTextLength()
{
	return sADMItem->GetTextLength(fItem);
}

inline void IADMItem::SetMaxTextLength(int length)
{
	sADMItem->SetMaxTextLength(fItem, length);
}

inline int IADMItem::GetMaxTextLength()
{
	return sADMItem->GetMaxTextLength(fItem);
}

inline void IADMItem::SelectAll()
{
	sADMItem->SelectAll(fItem);
}

inline void IADMItem::SetSelectionRange(int selStart, int selEnd)
{
	sADMItem->SetSelectionRange(fItem, selStart, selEnd);
}

inline void IADMItem::GetSelectionRange(int &selStart, int &selEnd)
{
	sADMItem->GetSelectionRange(fItem, &selStart, &selEnd);
}

inline void IADMItem::SetJustify(ADMJustify justify)
{
	sADMItem->SetJustify(fItem, justify);
}

inline ADMJustify IADMItem::GetJustify()
{
	return sADMItem->GetJustify(fItem);
}

inline void IADMItem::SetUnits(ADMUnits units)
{
	sADMItem->SetUnits(fItem, units);
}

inline ADMUnits IADMItem::GetUnits()
{
	return sADMItem->GetUnits(fItem);
}

inline void IADMItem::SetAllowUnits(ASBoolean allow)
{
	sADMItem->SetAllowUnits(fItem, allow);
}

inline ASBoolean IADMItem::GetAllowUnits()
{
	return sADMItem->GetAllowUnits(fItem);
}

inline void IADMItem::SetAllowMath(ASBoolean allow)
{
	sADMItem->SetAllowMath(fItem, allow);
}

inline ASBoolean IADMItem::GetAllowMath()
{
	return sADMItem->GetAllowMath(fItem);
}

inline void IADMItem::ShowUnits(ASBoolean show)
{
	sADMItem->ShowUnits(fItem, show);
}

inline ASBoolean IADMItem::GetShowUnits()
{
	return sADMItem->GetShowUnits(fItem);
}

inline void IADMItem::SetFloatToTextProc(ADMItemFloatToTextProc proc)
{
	sADMItem->SetFloatToTextProc(fItem, proc);
}

inline ADMItemFloatToTextProc IADMItem::GetFloatToTextProc()
{
	return sADMItem->GetFloatToTextProc(fItem);
}

inline ASBoolean IADMItem::DefaultFloatToText(float value, char *text, int textLength)
{
	return sADMItem->DefaultFloatToText(fItem, value, text, textLength);
}

inline void IADMItem::SetTextToFloatProc(ADMItemTextToFloatProc proc)
{
	sADMItem->SetTextToFloatProc(fItem, proc);
}

inline ADMItemTextToFloatProc IADMItem::GetTextToFloatProc()
{
	return sADMItem->GetTextToFloatProc(fItem);
}

inline ASBoolean IADMItem::DefaultTextToFloat(char *text, float *value)
{
	return sADMItem->DefaultTextToFloat(fItem, text, value);
}

inline ASBoolean IADMItem::WasPercentageChange()
{
	return sADMItem->WasPercentageChange(fItem);
}

/*
 * Numerics
 */
 
inline void IADMItem::SetPrecision(int numberOfDecimalPlaces)
{
	sADMItem->SetPrecision(fItem, numberOfDecimalPlaces);
}

inline int IADMItem::GetPrecision()
{
	return sADMItem->GetPrecision(fItem);
}

inline void IADMItem::SetBooleanValue(ASBoolean value)
{
	sADMItem->SetBooleanValue(fItem, value);
}

inline ASBoolean IADMItem::GetBooleanValue()
{
	return sADMItem->GetBooleanValue(fItem);
}

inline void IADMItem::SetIntValue(int value)
{
	sADMItem->SetIntValue(fItem, value);
}

inline int IADMItem::GetIntValue()
{
	return sADMItem->GetIntValue(fItem);
}

inline void IADMItem::SetFixedValue(ASFixed value)
{
	sADMItem->SetFixedValue(fItem, value);
}
	
inline IASFixed IADMItem::GetFixedValue()
{
	return sADMItem->GetFixedValue(fItem);
}

inline void IADMItem::SetFloatValue(float value)
{
	sADMItem->SetFloatValue(fItem, value);
}
	
inline float IADMItem::GetFloatValue()
{
	return sADMItem->GetFloatValue(fItem);
}

inline void IADMItem::SetMinIntValue(int value)
{
	sADMItem->SetMinIntValue(fItem, value);
}

inline int IADMItem::GetMinIntValue()
{
	return sADMItem->GetMinIntValue(fItem);
}

inline void IADMItem::SetMinFixedValue(ASFixed value)
{
	sADMItem->SetMinFixedValue(fItem, value);
}
	
inline IASFixed IADMItem::GetMinFixedValue()
{
	return sADMItem->GetMinFixedValue(fItem);
}

inline void IADMItem::SetMaxIntValue(int value)
{
	sADMItem->SetMaxIntValue(fItem, value);
}

inline int IADMItem::GetMaxIntValue()
{
	return sADMItem->GetMaxIntValue(fItem);
}

inline void IADMItem::SetMinFloatValue(float value)
{
	sADMItem->SetMinFloatValue(fItem, value);
}
	
inline float IADMItem::GetMinFloatValue()
{
	return sADMItem->GetMinFloatValue(fItem);
}

inline void IADMItem::SetMaxFixedValue(ASFixed value)
{
	sADMItem->SetMaxFixedValue(fItem, value);
}
	
inline IASFixed IADMItem::GetMaxFixedValue()
{
	return sADMItem->GetMaxFixedValue(fItem);
}

inline void IADMItem::SetMaxFloatValue(float value)
{
	sADMItem->SetMaxFloatValue(fItem, value);
}
	
inline float IADMItem::GetMaxFloatValue()
{
	return sADMItem->GetMaxFloatValue(fItem);
}

inline void IADMItem::SetSmallIncrement(float increment)
{
	sADMItem->SetSmallIncrement(fItem, increment);
}
	
inline float IADMItem::GetSmallIncrement()
{
	return sADMItem->GetSmallIncrement(fItem);
}

inline void IADMItem::SetLargeIncrement(float increment)
{
	sADMItem->SetLargeIncrement(fItem, increment);
}

inline float IADMItem::GetLargeIncrement()
{
	return sADMItem->GetLargeIncrement(fItem);
}

inline ADMItemRef IADMItem::GetChildItem(int childID)
{
	return sADMItem->GetChildItem(fItem, childID);
}

inline void IADMItem::SetTipString(const char *tipStr)
{
	sADMItem->SetTipString(fItem, (char *)tipStr);
}

inline void IADMItem::GetTipString(char *tipStr, int maxLen)
{
	sADMItem->GetTipString(fItem, tipStr, maxLen);
}

inline int IADMItem::GetTipStringLength()
{
	return sADMItem->GetTipStringLength(fItem);
}	

inline void IADMItem::EnableTip(ASBoolean enable)
{
	sADMItem->EnableTip(fItem, enable);
}	

inline ASBoolean IADMItem::IsTipEnabled()
{
	return sADMItem->IsTipEnabled(fItem);
}	

inline void IADMItem::ShowToolTip(ASPoint *where)
{
	sADMItem->ShowToolTip(fItem, where);
}	

inline void IADMItem::HideToolTip()
{
	sADMItem->HideToolTip(fItem);
}	

inline void IADMItem::SetHelpID(ASHelpID helpID)
{
	sADMItem->SetHelpID(fItem, helpID);
}

inline ASHelpID IADMItem::GetHelpID()
{
	return sADMItem->GetHelpID(fItem);
}

inline void IADMItem::Help()
{
	sADMItem->Help(fItem);
}

inline void IADMItem::GetBestSize(ASPoint *size)
{
	sADMItem->GetBestSize(fItem, size);
}	


inline void IADMItem::SetMask(ADMActionMask mask)
{
	sADMItem->SetMask(fItem, mask);
}

inline ADMActionMask IADMItem::GetMask()
{
	return sADMItem->GetMask(fItem);
}

inline void IADMItem::SetUserData(ADMUserData data)
{
	sADMItem->SetUserData(fItem, data);
}

inline ADMUserData IADMItem::GetUserData()
{
	return sADMItem->GetUserData(fItem);
}

inline void IADMItem::SetNotifierData(ADMUserData data)
{
	sADMItem->SetNotifierData(fItem, data);
}

inline ADMUserData IADMItem::GetNotifierData()
{
	return sADMItem->GetNotifierData(fItem);
}

inline ADMTimerRef IADMItem::CreateTimer(unsigned long milliseconds,
		ADMActionMask abortMask, ADMItemTimerProc timerProc,
		ADMItemTimerAbortProc abortProc)
{
	return sADMItem->CreateTimer(fItem, milliseconds,
		abortMask, timerProc, abortProc);
}

inline void IADMItem::AbortTimer(ADMTimerRef timer)
{
	sADMItem->AbortTimer(fItem, timer);
}

#endif

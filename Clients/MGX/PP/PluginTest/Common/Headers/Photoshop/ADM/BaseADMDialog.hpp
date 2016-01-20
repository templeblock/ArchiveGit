/*
 * Name:
 *	BaseADMDialog.hpp
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
 *	1.0.0 3/11/1996	DL	First version.
 *		Created by Dave Lazarony.
 */

#ifndef __BaseADMDialog_hpp__
#define __BaseADMDialog_hpp__

/*
 * Includes
 */
 
#ifndef __IADMDialog_hpp__
#include "IADMDialog.hpp"
#endif

#ifndef __IADMItem_hpp__
#include "IADMItem.hpp"
#endif

class IADMDrawer;
class IADMNotifier;
class IADMTracker;


/*
 * Base Class
 */

class BaseADMDialog : public IADMDialog
{
private:
	static ASErr ASAPI InitProc(ADMDialogRef dialog);
	static void ASAPI DrawProc(ADMDialogRef dialog, ADMDrawerRef drawer);
	static ASBoolean ASAPI TrackProc(ADMDialogRef dialog, ADMTrackerRef tracker);
	static void ASAPI NotifyProc(ADMDialogRef dialog, ADMNotifierRef notifier);
	static void ASAPI DestroyProc(ADMDialogRef dialog);

	ASBoolean fIsModal;

protected:
//	IADMDialog fDialog;
	ADMUserData fUserData;
	
	virtual ASErr Init();
	virtual void Draw(IADMDrawer drawer);
	virtual ASBoolean Track(IADMTracker tracker);
	virtual void Notify(IADMNotifier notifier);
	virtual void Destroy();
	
	IADMItem GetItem(int itemID);
	
public:
	BaseADMDialog();
	
	virtual ~BaseADMDialog();	
	
	virtual ADMDialogRef Create(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style);
	virtual int Modal(SPPluginRef pluginRef, char *name, int dialogID, ADMDialogStyle style = kADMModalDialogStyle);
	
	void SetUserData(ADMUserData userData);
	ADMUserData GetUserData();
	
	IADMDialog GetDialog();
};


inline IADMItem BaseADMDialog::GetItem(int itemID) 
{ 
	return IADMDialog::GetItem(itemID); 
}

	
inline void BaseADMDialog::SetUserData(ADMUserData userData)
{
	fUserData = userData;
}

inline ADMUserData BaseADMDialog::GetUserData()
{
	return fUserData;
}

inline IADMDialog BaseADMDialog::GetDialog()
{
	return IADMDialog::GetDialogRef(); //fDialog;
}


#endif

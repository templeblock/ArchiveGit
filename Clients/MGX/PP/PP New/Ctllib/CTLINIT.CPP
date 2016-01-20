// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "ctllib.h"

#ifndef _WINDLL
/***********************************************************************/
BOOL Ctl_Init( HINSTANCE hInstance, UINT ClassScope, int iLabelFont, LPBLT lpBltScreen,
				HINSTANCE hInstIcon, BOOL fNewLook)
/***********************************************************************/
{
	Ctl_SetLabelFont(iLabelFont);
	Ctl_SetBltScreen(lpBltScreen);
	Ctl_SetNewLook(fNewLook);
	Ctl_SetIconInstance(hInstIcon != NULL ? hInstIcon : hInstance);
	if (!Icon_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!PalPicker_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Grid_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Map_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Stick_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Gradient_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Patch_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Histo_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Tag_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!ListBox_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!Lut_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	if (!StaticText_RegisterClass(hInstance, ClassScope))
		return(FALSE);
	return(TRUE);
}

/***********************************************************************/
void Ctl_Cleanup( HINSTANCE hInstance )
/***********************************************************************/
{
	Icon_UnRegisterClass(hInstance);
	PalPicker_UnRegisterClass(hInstance);
	Grid_UnRegisterClass(hInstance);
	Map_UnRegisterClass(hInstance);
	Stick_UnRegisterClass(hInstance);
	Histo_UnRegisterClass(hInstance);
	Tag_UnRegisterClass(hInstance);
	ListBox_UnRegisterClass(hInstance);
	Lut_UnRegisterClass(hInstance);
	Gradient_UnRegisterClass(hInstance);
	Patch_UnRegisterClass(hInstance);
	StaticText_UnRegisterClass(hInstance);
}
#endif

//************************************************************************
//		Ctl_NoReg_Init
// DESCRIPTION:
//		Same as  Ctl_Init, except doesn't register any classes
//************************************************************************
BOOL Ctl_NoReg_Init( HINSTANCE hInstance, UINT ClassScope, int iLabelFont, LPBLT lpBltScreen,
				HINSTANCE hInstIcon, BOOL fNewLook)
/***********************************************************************/
{
	Ctl_SetLabelFont(iLabelFont);
	Ctl_SetBltScreen(lpBltScreen);
	Ctl_SetNewLook(fNewLook);
	Ctl_SetIconInstance(hInstIcon != NULL ? hInstIcon : hInstance);
	return(TRUE);
}

/***********************************************************************/
//		Same as  Ctl_Cleanup, except doesn't un register any classes
/***********************************************************************/
void Ctl_NoReg_Cleanup( HINSTANCE hInstance )
/***********************************************************************/
{
}




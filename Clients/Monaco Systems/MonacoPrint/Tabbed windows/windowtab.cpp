//////////////////////////////////////////////////////////////////////////////
//  windowtab.cpp															//
//	a tab within a window													//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////


#include "windowtab.h"

WindowTab::WindowTab(DialogPtr dialp,int	Ditl_Id)
{
int temp;

Handle newItems;

dp = dialp;
idDITL = Ditl_Id;

startNum  = CountDITL(dp);


SetPort(dp);
TextFont(systemFont);
TextSize(12);
TextFace(0);

if (idDITL != -1) 
	{

	temp = CountDITL(dp);
		
	newItems = GetResource('DITL',idDITL);
	if (!newItems) return;

	AppendDITL(dp,newItems,overlayDITL);

	ReleaseResource(newItems);
	}

numDITL = CountDITL(dp) - startNum;

WinType = WinTab;

}


WindowTab::~WindowTab(void)
{
//hideWindow();
dp = 0L;
}

McoStatus WindowTab::hideWindow(void)
{
int temp;


//temp = CountDITL(dp);
temp = numDITL;

if ((dp) && (temp)) ShortenDITL(dp,temp);

return MCO_SUCCESS;
}

McoStatus WindowTab::showWindow(void)
{
Handle newItems;

int temp;

	SetPort(dp);
	TextFont(systemFont);
	TextSize(12);
	TextFace(0);

	if (idDITL != -1) 
		{

		temp = CountDITL(dp);
			
		newItems = GetResource('DITL',idDITL);
		if (!newItems) return MCO_RSRC_ERROR;

		AppendDITL(dp,newItems,overlayDITL);

		ReleaseResource(newItems);
		}

	SetUpDial();
	CopyParameters();
	updateWindowData(0);
	
	return MCO_SUCCESS;
}
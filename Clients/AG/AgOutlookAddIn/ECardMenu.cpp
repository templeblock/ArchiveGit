#include "stdafx.h"
#include "resource.h"
#include "ecardmenu.h"

///////////////////////////////////////////////////////////////////////////////
CECardMenu::CECardMenu(HMENU hMenu) : CMenuSuper(hMenu)
{
	//MakeOwnerDrawn();
}
///////////////////////////////////////////////////////////////////////////////
CECardMenu::~CECardMenu(void)
{
	m_ImageList.Destroy();
}
///////////////////////////////////////////////////////////////////////////////
bool CECardMenu::Init()
{
	if (m_ImageList)
		m_ImageList.Destroy();

	if (!m_ImageList.Create(16,16, ILC_COLOR24 | ILC_MASK, 4, 4))
		return false;

	AddImage(IDB_AG_16);
	AddImage(IDB_BLUE_16);
	AddImage(IDB_BEATGREETS_16);
	AddImage(IDB_EGREETS_16);
	SetImageList(&m_ImageList);


	return true;
}
/////////////////////////////////////////////////////////////////////////////
int CECardMenu::AddImage(DWORD dwID)
{
	if (dwID == -1)
		return 0x8000;

	CBitmap bmp;
	bmp.LoadBitmap(dwID);
	HBITMAP hBitmapMask = ReplaceBitmapColor(bmp.m_hBitmap, CLR_DEFAULT, RGB(255,255,255), RGB(0,0,0));

	return m_ImageList.Add(bmp.m_hBitmap, hBitmapMask);
}
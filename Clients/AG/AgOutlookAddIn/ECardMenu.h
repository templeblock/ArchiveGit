#pragma once
#include "MenuSuper.h"

class CECardMenu : public CMenuSuper
{
public:

	CECardMenu(HMENU hMenu = NULL);
	virtual ~CECardMenu(void);
	bool Init();
	int AddImage(DWORD dwID);

private:

	CImageList m_ImageList;

};

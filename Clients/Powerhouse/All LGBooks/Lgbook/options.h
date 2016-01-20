#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "proto.h"
#include "lgbook.h"
#include "lgbookid.h"
#include "sprite.h"

class FAR COptionsScene : public CGBScene
{
public:
	// constructors/destructor
	COptionsScene(int nNextSceneNo);
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	virtual BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);

private:
	int	m_nSelectedSceneID;
};

typedef COptionsScene FAR * LPOPTIONSSCENE;
                            
#endif // _OPTIONS_H_
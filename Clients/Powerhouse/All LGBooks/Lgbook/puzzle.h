#ifndef _PUZZLE_H_
#define _PUZZLE_H_

#include "puzzctrl.h"
#include "lgbook.h"

class FAR CPuzzleScene : public CGBScene
{
public:
	// constructors/destructor
	CPuzzleScene(int nNextSceneNo);
	~CPuzzleScene() {};

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
private:
	int			m_iSkill;
	BOOL		m_bHintMode;
};

typedef CPuzzleScene FAR * LPPUZZLESCENE;

#endif // _PUZZLE_H_

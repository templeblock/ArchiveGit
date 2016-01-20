#ifndef _SOUNDBOX_H_
#define _SOUNDBOX_H_

#include "proto.h"
#include "lgbook.h"
#include "lgbookid.h"

class FAR CSoundBoxScene : public CGBScene
{
public:
	// constructors/destructor
	CSoundBoxScene(int nNextSceneNo);

private:
	// dialog procedure overridables
	virtual BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);

private:        
	static int m_nSoundBoxScene;
};

typedef CSoundBoxScene FAR * LPSOUNDBOXSCENE;
                            
#endif // _SOUNDBOX_H_
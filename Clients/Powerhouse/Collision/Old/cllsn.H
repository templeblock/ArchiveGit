#ifndef _CLLSN_H_
#define _CLLSN_H_

#include "scene.h"
#include "cllsnid.h"

#ifdef MAIN
  #define DECLARE
#else
  #define DECLARE extern
#endif

#define TITLE_SIZE 32

class FAR CCollisionScene : public CScene
{
public:
  // constructors/destructor
  CCollisionScene() {};
  ~CCollisionScene() {};

private:
  // dialog procedure overridables
  BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
  void OnCommand(HWND hWnd, int id, HWND loWord, UINT hiWord);
  void OnClose(HWND hWnd);
  void OnTimer(HWND hWnd, UINT id);
};

typedef CCollisionScene FAR * LPCLLSNSCENE;
                            
class CCollisionApp : public CPHApp
{
public:

  // Constructor
  CCollisionApp(){};
  // Destructor
  ~CCollisionApp(){};

  // Implementation
  BOOL GotoScene( HWND hWndPreviousScene, int iScene );
};

DECLARE CCollisionApp App;


#endif // _CLLSN_H_

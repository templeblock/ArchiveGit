#ifndef LEVELS_H
#define LEVELS_H

#include "GameState.h"

#define MAX_LEVELS 12
#define ALL_OPPS_DEAD 0xffffffff

class CLevel;
typedef class CLevel * LPLEVEL;

class CLevelState;
typedef class CLevelState * LPLEVELSTATE;

class CLevelState
{
public:
	CLevelState();
	~CLevelState();

	BOOL SetLevel(int nLevel);
	int GetLevel(void);
	CLevel *GetLevelPtr(void) {return m_pLevel;}
	BOOL SetState(int nEvent, DWORD dwValue);
	BOOL HaveFireExtinguisher();
	BOOL HaveGateKey();
	int  GetState(void) {return GState.GetState();}

private:
	int m_nLevel;
	CLevel *m_pLevel;
	GameState GState;
	CLevel *Levels[MAX_LEVELS];
};

// Level events
#define LEVENT_GET_COLL		1	// Get collectible events (level specific)
#define LEVENT_LOSE_COLL	2	// Lost collectible event (level specific)
#define LEVENT_GET_FIREEXT	3	// Get the fire extinguisher
#define LEVENT_LOSE_FIREEXT 4	// Lose the fire extinguisher
#define LEVENT_KILL			5	// Opponent killed
#define LEVENT_SWITCH_ON	6	// Switch has been hit
#define LEVENT_GOT_SK		7	// Picked up a spine killer
#define LEVENT_SPINE_KILLED	8	// Spine killed
#define LEVENT_LOSE_ALL		9	// Lose all collectibles

class CLevel : public GameStateObserver
{
public:
	CLevel() { }
	CLevel(int nNum, int *pStateTbl, int nEventMax, int nStateMax);

	int *GetTbl() { return m_pStateTbl; }
	int  GetEventMax() { return m_nEventMax; }
	int  GetStateMax() { return m_nStateMax; }

	virtual BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue) = 0;
	virtual void OnStateChange(int nState) = 0;

	BOOL HaveFireExtinguisher() { return m_bHaveExtinguisher; }
	BOOL HaveGateKey() { return m_bHaveGateKey; }

	int  GetUseData(void) { return m_nUseData1; }
	void ShowText(char *szText);

protected:
	BOOL m_bHaveExtinguisher;
	BOOL m_bHaveGateKey;
	int  m_nUseData1;

private:
	int m_nNum;

	int *m_pStateTbl;
	int  m_nEventMax;
	int  m_nStateMax;
};

class CLevel1 : public CLevel
{
public:
	CLevel1();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

class CLevel2 : public CLevel
{
public:
	CLevel2();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};


class CLevel3 : public CLevel
{
public:
	CLevel3();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

class CLevel4 : public CLevel
{
public:
	CLevel4();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

class CLevel5 : public CLevel
{
public:
	CLevel5();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

class CLevel6 : public CLevel
{
public:
	CLevel6();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

class CLevel7 : public CLevel
{
public:
	CLevel7();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};
class CLevel8 : public CLevel
{
public:
	CLevel8();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};
class CLevel9 : public CLevel
{
public:
	CLevel9();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};
class CLevel10 : public CLevel
{
public:
	CLevel10();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};
class CLevel11 : public CLevel
{
public:
	CLevel11();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};
class CLevel12 : public CLevel
{
public:
	CLevel12();

	BOOL OnEvent(GameState& State, int nEvent, DWORD dwValue);
	void OnStateChange(int nState);
};

#endif

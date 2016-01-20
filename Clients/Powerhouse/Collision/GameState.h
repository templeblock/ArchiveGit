#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <windows.h>

const int nMaxEvents = 200;		// Maximum number of events in the event history array
//const int nLastLevel = 12;		// Last level number

class GameStateObserver
{
public:
	virtual void OnStateChange(int nState) = 0;
};

class GameState
{
public:
	GameState() { m_nState=0; m_nEventCount=0; m_pnStates = NULL; }

	BOOL SetObserver(GameStateObserver *pObserver);

	BOOL SetStateTbl(int *pnTable, int nMaxEvents, int nMaxStates);

	//BOOL SetLevel(int nLevel);
	//int  GetLevel(void) { return m_nLevel; }

	BOOL SetState(int nEvent);
	int  GetState(void) { return m_nState; }

	BOOL DidEventOccur(int nEvent); 

// Private methods
private:
	BOOL RecordEvent(int nEvent);	// Adds event to event history array

// Private data
private:
	//int m_nLevel;				// Current level
	int m_nState;				// Current state

	int m_nEventCount;			// Number of events in event history array
	int m_nEvents[nMaxEvents];	// Event history array

	int *m_pnStates;			// Pointer to current state table
	int  m_nMaxEvents;			// Max number of events for current level
	int  m_nMaxStates;			// Max number of states for current level

	GameStateObserver *m_pObserver;
};


#endif
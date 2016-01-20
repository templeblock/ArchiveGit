#include "gamestate.h"

BOOL GameState::SetObserver(GameStateObserver* pObserver)
{
	m_pObserver = pObserver;
	return TRUE;
}

BOOL GameState::SetStateTbl(int *pnTable, int nMaxEvents, int nMaxStates)
{
	m_pnStates = pnTable;
	m_nMaxEvents = nMaxEvents;
	m_nMaxStates = nMaxStates;
	m_nState = 0;

	return TRUE;
}

BOOL GameState::SetState(int nEvent)
{
	int nNewState;

	// Check event
	if (nEvent < 0 || nEvent > m_nMaxEvents)
		return FALSE;

	// Set new state
	nNewState = *(m_pnStates + (m_nState * m_nMaxEvents) + nEvent);

	// Check new state
	if (nNewState < 0 || nNewState > m_nMaxStates)
		return FALSE;

	m_nState = nNewState;

	// Call observer with new state
	if (m_pObserver)
		m_pObserver->OnStateChange(m_nState);

	// Record the event in the event history array
	RecordEvent(nEvent);

	return TRUE;
}

BOOL GameState::RecordEvent(int nEvent)
{
	// Add event to event history
	if (m_nEventCount < (nMaxEvents-1))
	{
		m_nEvents[m_nEventCount++] = nEvent;		
	}
	else
	{
		// Not enough room in the event history array
		return FALSE;
	}

	return TRUE;
}

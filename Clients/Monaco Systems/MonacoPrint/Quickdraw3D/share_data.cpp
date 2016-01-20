/******************************************************************/
/* share_data.c													  */
/* class that allows sharing of data and events between windows	  */
/* Feb 26, 1996												      */
/* James Vogh													  */
/******************************************************************/

#include "share_data.h"

Shared_Data::Shared_Data(void)
{
priority = 1;
num_wins = 0;
wins = new baseWin*[MaxNumWins];
}

Shared_Data::~Shared_Data()
{
delete wins;
}

McoStatus Shared_Data::AddWin(baseWin *win)
{
wins[num_wins] = win;
num_wins ++;
return MCO_SUCCESS;
}

McoStatus Shared_Data::RemoveWin(baseWin *win)
{
int i,j;

for (i=0; i<num_wins; i++)
	{
	if (wins[i] == win)
		{
		for (j=i; j<num_wins; j++) wins[j] = wins[j+1];
		num_wins--;
		break;
		}
	}
return MCO_SUCCESS;
}

Boolean Shared_Data::isMyWindow(MS_WindowTypes winType,int32 winNum)
{
int i;

for (i=0; i<num_wins; i++)
	{
	if ( (wins[i]->isMyWindowType(winType)) && (wins[i]->isMyWindowNum(winNum))) 
		return TRUE;
	}
return FALSE;
}

Boolean Shared_Data::isMyWindow(baseWin *win)
{
int i;

for (i=0; i<num_wins; i++)
	{
	if  (wins[i] == win) return TRUE;
	}
return FALSE;
}
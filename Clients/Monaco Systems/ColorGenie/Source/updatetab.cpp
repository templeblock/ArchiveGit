//function to enable or disable tabs
BOOL	m_tabs[5];

void UpdateTabs(BOOL *enabledTab, BOOL *changed, int dongle, int monacocreated, 
	int monacoedited, int locked, int sourceexist, int dryjetexist, int calexist, int newcreated)
{

	if (dongle && monacocreated && !monacoedited)
	{
		if (!enabledTab[2]) *changed = 1;
		enabledTab[2] = 1;
	}
	else
	{
		if (enabledTab[2]) *changed = 1;
		enabledTab[2] = 0;
	}
		
	if (locked)
	{
		
	}
	else if ( sourceexist || dryjetexist )
	{
		if (enabledTab[3]) *changed = 1;
		if (enabledTab[4]) *changed = 1;
		enabledTab[3] = 0;
		enabledTab[4] = 0;
		
		if (newcreated)
		{
			if (enabledTab[0]) *changed = 1;
			if (enabledTab[1]) *changed = 1;
			enabledTab[0] = 0;
			enabledTab[1] = 0;
		}		
		else
		{
			if (!enabledTab[0]) *changed = 1;
			if (!enabledTab[1]) *changed = 1;
			enabledTab[0] = 1;
			enabledTab[1] = 1;
		}			
	}
	else
	{
		if (!enabledTab[0]) *changed = 1;
		if (!enabledTab[1]) *changed = 1;
		enabledTab[0] = 1;
		enabledTab[1] = 1;
		if (dongle && calexist)
		{
			if (!enabledTab[3]) *changed = 1;
			if (!enabledTab[4]) *changed = 1;
			enabledTab[3] = 1;
			enabledTab[4] = 1;
		}
		else 
		{
			if (enabledTab[3]) *changed = 1;
			if (enabledTab[4]) *changed = 1;

			enabledTab[3] = 0;
			enabledTab[4] = 0;
		}
	}

}

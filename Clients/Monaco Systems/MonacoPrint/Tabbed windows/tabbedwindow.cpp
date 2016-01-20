//////////////////////////////////////////////////////////////////////////////
//  tabbedwindow.cpp														//
//	a window with tabs														//
//  created 8/25/97															//
//  James Vogh																//
//////////////////////////////////////////////////////////////////////////////

#include "tabbedwindow.h"

TabbedWindow::TabbedWindow(void)
{

}

TabbedWindow::~TabbedWindow(void)
{
int i;

if (windows)
	{
	for (i=0; i<numTabs; i++) if (windows[i]) delete windows[i];
	delete windows;
	}
}
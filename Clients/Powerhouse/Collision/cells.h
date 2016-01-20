#ifndef CELLS_H
#define CELLS_H

#define MAX_GATE 6

#include "worlds.h"

class CCell
{
public:
	CCell();
	~CCell();

	void Setx(int i) {x = i;}
	void Sety(int i) {y = i;}
	int  Getx() { return x; }
	int  Gety() { return y; }
	char Type(void) { return cType; }
	void SetType(char c) {cType = c;}
	char Look(void) { return cLook; }
	void SetLook(char c) {cLook = c;}
	int  IsVisible() { return m_nVisible; }
	void SetVisible(int nVisible) { m_nVisible = nVisible; }
	void SetOff() { m_On = 0; }
	void SetOn() { m_On = 1; }
	int  IsOn() { return m_On; }

	char PortalDir(int iDir);
	void SetPortalDir(int iDir, char cNum);
	char PortalType(int iPortal);
	void SetPortalType(int iPortal, char cNum);
	void ZapPortal(int iDir);
	char OppDir(int iDir);
	void SetOppDir(int iOppDir, char cNum);
	char OppType(int iPortal);
	void SetOppType(int iPortal, char cNum);
	void ClearOpps(void);

private:
	int  x;
	int  y;
	char cType;
	char cLook;
	char cPortal[MAX_GATE];
	char cOpp[MAX_GATE];

	int  m_nVisible;
	int  m_On;

};

#endif

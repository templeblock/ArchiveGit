/*********************** ( First 3D Development ) ***********************/
/*	(c) 1994/5  John Lundy															     						*/
/************************************************************************/
// fduser.cpp : implementation of the CFdView class
//

#include "stdafx.h"
#include <windowsx.h>
#include <mmsystem.h>
#include "fd.h"

#include "fddoc.h"
#include "fdview.h"
#include "fdmm.h"
#include "fdstatus.h"
#include "fdmap.h"

#include "utils.h"
#include <wing.h>
#include "ack\ack3d.h"
#include "ack\ackeng.h"
#include "ack\ackext.h"
#include "ack\acksqr.cpp"		//	inline functions

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define	VIEW_LENGTH		64000

void	TintScreen(UCHAR far *ScreenBuffer, UINT size, int freq, int PalEntry);
long	long_divide(long denom,long divisor);

BOOL			ViewForward=TRUE, fPaused=FALSE;
int				ViewLowRes=0, ViewUpDown=8192, ViewUDCenter=8192;
int				key_shift=0, key_control=0, rel_control=0;
int				rapid_fire=0, fire_shot=0, disp_shot=0;
int				key_up=0, key_down=0, key_left=0, key_right=0;
int				key_ltr_a=0, key_ltr_s=0;
int				rel_up=0, rel_down=0, rel_left=0, rel_right=0;
int				rel_ltr_a=0, rel_ltr_s=0;
int				dcx=0, dcy=0, last_dcx=0, last_dcy=0, ds=0, last_ds=0;
int				jog=0, jumping=0, airborne=0;
int				HeartRate=MIN_HEART_RATE, ObjectHit, newWeapon = -1;
int				PlayerHit=0;
UINT			LastSpecial=0xffff;

CFdStatus		*fds;
int				fdsActive=0;
RECT			fdsRect;
int				fdsWidth=FDS_WIDTH, fdsHeight=FDS_HEIGHT;

CFdMap			*fdm;
int				fdmActive=0;
RECT			fdmRect;
int				fdmWidth=FDM_WIDTH, fdmHeight=FDM_HEIGHT;

int				IdleTicks=0;

extern CFrameWnd		*pFrameWnd;
extern BOOL				fIsIconic;
extern BOOL				fActivateApp;	
extern RECT				frmRect;
extern int				BufferX, BufferY;
extern int				MusicActive, MusicWanted;
extern HMIDIOUT			hmOut;
extern int				SoundActive, SoundWanted;

extern unsigned long	TimerTicks, NumberOfPaints;

extern CPalette			CPalApp;
extern HDC				hdcBuffer;

extern CTime			StartTime, StopTime;
extern CTimeSpan		ElapsedTime;

extern ACKENG		far *ae;

extern int			midiHoldOff;

/////////////////////////////////////////////////////////////////////////////
// Menu Items Interface

void CFdView::OnUpdateOptionsMusic(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (MusicActive)
	{	pCmdUI->Enable(TRUE);
		if (MusicWanted) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
	}
	else pCmdUI->Enable(FALSE);
}

void CFdView::OnOptionsMusic()
{
	// TODO: Add your command handler code here
CString		tmp;
	MusicWanted = 1 - MusicWanted;
	MidiEnable(MusicWanted);
	if (MusicWanted)
	{
		tmp = "Yes";
		if (fds) fds->TextMsg("Music On");
	}
	else
	{
		tmp = "No";
		if (fds) fds->TextMsg("Music Off");
	}
	AfxGetApp()->WriteProfileString("Preferences", "Music", tmp);
}

void CFdView::OnUpdateOptionsSound(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (SoundActive)
	{   pCmdUI->Enable(TRUE);
		if (SoundWanted) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
	}
	else pCmdUI->Enable(FALSE);
}

void CFdView::OnOptionsSound()
{
	// TODO: Add your command handler code here
CString		tmp;
	SoundWanted = 1 - SoundWanted;
	if (SoundWanted)
	{
		tmp = "Yes";
		if (fds) fds->TextMsg("Sound/FX On");
	}
	else
	{
		tmp = "No";
		if (fds) fds->TextMsg("Sound/FX Off");
	}
	AfxGetApp()->WriteProfileString("Preferences", "Sound/FX", tmp);
}

void CFdView::OnUpdateViewForward(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (ViewForward)
		pCmdUI->SetText("Reverse Vie&w\tAlt-W");
	else
		pCmdUI->SetText("Forward Vie&w\tAlt-W");
}

void CFdView::OnViewForward()
{
	// TODO: Add your command handler code here
	ViewForward = (ViewForward) ? FALSE : TRUE;
}

void CFdView::OnUpdateOptionsAutomatic(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (rapid_fire) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}

void CFdView::OnOptionsAutomatic()
{
	// TODO: Add your command handler code here
CString		tmp;
	rapid_fire = 1 - rapid_fire;
	if (rapid_fire)
	{
		tmp = "Yes";
		if (fds) fds->TextMsg("Full Auto On");
	}
	else
	{
		tmp = "No";
		if (fds) fds->TextMsg("Full Auto Off");
	}
	AfxGetApp()->WriteProfileString("Preferences", "Automatic", tmp);
}

void CFdView::OnUpdateOptionsStatus(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (fds) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}

void CFdView::OnOptionsStatus()
{
	// TODO: Add your command handler code here
CString	tmp;
	if (fds)
	{
		fds->DestroyWindow();
		delete fds;
		fds = NULL;
		tmp = "No";
		fdsActive = 0;
	}
	else
	{
		fdsRect.left = AfxGetApp()->GetProfileInt("Preferences", "Status_X", 10);
		fdsRect.top = AfxGetApp()->GetProfileInt("Preferences", "Status_Y", 10);
		fdsRect.right = fdsRect.left + (fdsWidth - 1) + GetSystemMetrics(SM_CXBORDER) * 2;
		fdsRect.bottom = fdsRect.top + (fdsHeight - 1) + GetSystemMetrics(SM_CYBORDER) +
							GetSystemMetrics(SM_CYCAPTION);
		fds = new CFdStatus(this->m_hWnd, fdsRect);
		tmp = "Yes";
		fdsActive = 1;
	}
	AfxGetApp()->WriteProfileString("Preferences", "Status", tmp);
}

void CFdView::OnUpdateOptionsMap(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (fdm) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}

void CFdView::OnOptionsMap()
{
	// TODO: Add your command handler code here
CString	tmp;
	if (fdm)
	{
		fdm->DestroyWindow();
		delete fdm;
		fdm = NULL;
		tmp = "No";
		fdmActive = 0;
	}
	else
	{
		fdmRect.left = AfxGetApp()->GetProfileInt("Preferences", "Map_X", 10);
		fdmRect.top = AfxGetApp()->GetProfileInt("Preferences", "Map_Y", 10);
		fdmRect.right = fdmRect.left + (fdmWidth - 1) + GetSystemMetrics(SM_CXBORDER) * 2;
		fdmRect.bottom = fdmRect.top + (fdmHeight - 1) + GetSystemMetrics(SM_CYBORDER) +
							GetSystemMetrics(SM_CYCAPTION);
		fdm = new CFdMap(this->m_hWnd, fdmRect);
		tmp = "Yes";
		fdmActive = 1;
	}
	AfxGetApp()->WriteProfileString("Preferences", "MapWin", tmp);
}

void CFdView::OnUpdateViewLowRes(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (ViewLowRes)
		pCmdUI->SetText("High &Resolution\tAlt-R");
	else
		pCmdUI->SetText("Low &Resolution\tAlt-R");
}

void CFdView::OnViewLowRes()
{
	// TODO: Add your command handler code here
CString		tmp;
	if (ViewLowRes)
	{
		if (ResizeWindow(1)) return;
		tmp = "No";
		if (fds) fds->TextMsg("High Resolution");
	}
	else
	{
		if (ResizeWindow(2)) return;
		tmp = "Yes";
		if (fds) fds->TextMsg("Low Resolution");
	}
	AfxGetApp()->WriteProfileString("Preferences", "LowResolution", tmp);
}

/////////////////////////////////////////////////////////////////////////////
// PaintIcon

void PaintIcon(void)
{
CDC		*pDC;
	pDC = pFrameWnd->GetDC();
	pDC->SelectPalette(&CPalApp,FALSE);
	pDC->RealizePalette();
	WinGStretchBlt(pDC->m_hDC, 0, 0, frmRect.right, frmRect.bottom, hdcBuffer, 0, 0, ViewXS, ViewYS);
	pFrameWnd->ReleaseDC(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CheckOnDoor

int CheckOnDoor(int x,int y,int Angle)
{
int	doorinfo = AckCheckDoorOpen(ae,x,y,Angle);
	//	TRACE generated too much output when Object movement was added
#ifdef	_DEBUG
	switch (doorinfo)
	{
	case POV_XDOOR:
		//TRACE("XDOOR checked\n");
		break;
	case POV_XDOOR|POV_DOORLOCKED:
		//TRACE("Locked XDOOR checked\n");
		break;
	case POV_YDOOR:
		//TRACE("YDOOR checked\n");
		break;
	case POV_YDOOR|POV_DOORLOCKED:
		//TRACE("Locked YDOOR checked\n");
		break;
	case POV_XSECRETDOOR:
		//TRACE("XSECRETDOOR checked\n");
		break;
	case POV_XSECRETDOOR|POV_DOORLOCKED:
		//TRACE("Locked XSECRETDOOR checked\n");
		break;
	case POV_YSECRETDOOR:
		//TRACE("YSECRETDOOR checked\n");
		break;
	case POV_YSECRETDOOR|POV_DOORLOCKED:
		//TRACE("Locked YDOOR checked\n");
		break;
	case POV_NODOOR:
		//TRACE("Not a door\n");
		break;
	default:
		TRACE("Unknown door type %x\n",doorinfo);
		break;
	}
#endif	//_DEBUG
	if (doorinfo & POV_DOORLOCKED)
	{
char	txt[40]={"Get "};
		if (LastDoorDetail & DOOR_LOCKED_RED) lstrcat(txt,"Red ");
		if (LastDoorDetail & DOOR_LOCKED_GREEN) lstrcat(txt,"Green ");
		if (LastDoorDetail & DOOR_LOCKED_BLUE) lstrcat(txt,"Blue ");
		lstrcat(txt,"key");
		if (fds) fds->TextMsg(txt);
	}
return(doorinfo);
}

/////////////////////////////////////////////////////////////////////////////
// Timer Callback

void CFdView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
MSG		msg;
int		i, j;
	if (GetFocus() != this) return;
	TimerTicks++;
	if (PeekMessage(&msg, NULL, WM_USER_ACTION, WM_USER_ACTION, PM_NOREMOVE) == 0)
	{
		PostMessage(WM_USER_ACTION, 0, TimerTicks);
	}
	if (fActivateApp && !fPaused)
	{
		if (++IdleTicks == 100)	//	force one idle event every 100 ticks
		{
			PostMessage(WM_USER_IDLE, 0, TimerTicks);
		}
		//	Check to see if a door is opening or closing and play a sound
		for (i=0 ; i < MAX_DOORS ; i++)
		{
			j = 64;
			if (!(ae->Door[i].mCode & DOOR_TYPE_SLIDE)) j = 32;
			if (ae->Door[i].ColOffset && ae->Door[i].ColOffset < j)
			{
				PostMessage(WM_USER_SOUND, MACHINE, 20);
				break;
			}
		}
		if (i == MAX_DOORS)
		{
			if (!ae->ObjList[PLAYER_OBJECT].Dead)
			if ((TimerTicks % HeartRate) == 0)
				PostMessage(WM_USER_SOUND, THUMP, 10);
		}
	}
	CView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnKeyDown

void CFdView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
UINT	Counter;
int		result=0;
	//	if Player is dead, return without processing anything
	if (ae->ObjList[PLAYER_OBJECT].Dead) return;
	for (Counter=0 ; Counter < nRepCnt ; Counter++)
	{
		switch (nChar)
		{
		case VK_F2:
			ViewUpDown = 16 << (8 - ViewLowRes);
			break;
		case VK_F3:
			ViewUpDown = 48 << (8 - ViewLowRes);
			break;
		case VK_F4:
			ViewUpDown = 32 << (8 - ViewLowRes);
			break;
		case VK_TAB:
			if (fdm) fdm->InvalidateRect(NULL,FALSE);
			break;
		case 'A':
			key_ltr_a = 1;
			break;
		case 'D':
			if (!jumping) jumping = 1;
			break;
		case 'P':
			if (fPaused)
			{
				if (fds) fds->TextMsg("");
				fPaused = FALSE;
			}
			else
			{
				if (fds) fds->TextMsg("Game Paused");
				fPaused = TRUE;
			}
			break;
		case 'S':
			key_ltr_s = 1;
			break;
		case '1':		//	Fists
		case '2':		//	Pistol
		case '3':		//	Rifle
		case '4':		//	Shotgun
		case '5':		//	Machine Gun
			newWeapon = nChar - '1';
			break;
		case VK_SHIFT:
			key_shift = 1;
			break;
		case VK_CONTROL:
			if (key_control == 0) fire_shot = 1;
			key_control = 1;
			break;
		case VK_SPACE:
			CheckOnDoor(ae->ObjList[PLAYER_OBJECT].x,
						ae->ObjList[PLAYER_OBJECT].y,
						ae->ObjList[PLAYER_OBJECT].Dir);
			break;
		case VK_RETURN:
			break;
		case VK_UP:
			key_up = 1;
			break;
		case VK_DOWN:
			key_down = 1;
			break;
		case VK_RIGHT: 
			key_right = 1;
			break;
		case VK_LEFT:
			key_left = 1;
			break;
		default:
			break;
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnKeyUp

void CFdView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
UINT		Counter;
	for (Counter=0 ; Counter < nRepCnt ; Counter++)
	{
		switch (nChar)
		{
		case 'A':
			rel_ltr_a = -1;
			break;
		case 'P':
			break;
		case 'S':
			rel_ltr_s = -1;
			break;
		case VK_SHIFT:
			key_shift = 0;
			break;
		case VK_CONTROL:
			rel_control = -1;
			break;
		case VK_SPACE:
			break;
		case VK_RETURN:
			break;
		case VK_UP:
			rel_up = -1;
			break;
		case VK_DOWN:
			rel_down = -1;
			break;
		case VK_RIGHT:
			rel_right = -1;
			break;
		case VK_LEFT:
			rel_left = -1;
			break;
		default:
			break;
		}
	}
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnMouseMove

void CFdView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnLButtonDown

void CFdView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnRButtonDown

void CFdView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnLButtonDblClk

void CFdView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CView::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CFdView::OnRButtonDblClk

void CFdView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CheckOnDoor(ae->ObjList[PLAYER_OBJECT].x,
				ae->ObjList[PLAYER_OBJECT].y,
				ae->ObjList[PLAYER_OBJECT].Dir);
	CView::OnRButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// User idle message routine

long CFdView::OnUserIdle(WPARAM wParam, LPARAM iCount)
{
	if (fActivateApp && !fPaused)
	{
		if (!fIsIconic)
		{
//#ifdef	_DEBUG
char		txt[64];
long		l, r;
			ElapsedTime = CTime::GetCurrentTime() - StartTime + ae->ElapsedTime;
			l = ElapsedTime.GetTotalSeconds();
			l = long_divide((NumberOfPaints + ae->NumberOfPaints) * 100L,l);
			r = l % 100L;
			l = long_divide(l,100L);
			wsprintf(txt, "WinAdv - RT=%s - F/S=%ld.%02ld",
				(const char *)ElapsedTime.Format("%D-%H:%M:%S"), l, r);
			GetParentFrame()->SetWindowText(txt);
//#endif
			if (IdleTicks >= 100)
			{
				if (fds) fds->TextMsg("");
				IdleTicks = 0;
			}
		}
	}
	return 0;
}   

/////////////////////////////////////////////////////////////////////////////
// Check health level of Player after a hit

void CheckHealth(int ObjIndex, int Damage)
{
ANIMATION	*ObjPtr;

	ObjPtr = ObjList[ae->ObjList[ObjIndex].bmNum];
	if (!ae->ObjList[PLAYER_OBJECT].Dead)
	{
		if (Damage) PlayerHit = 1;
		ae->ObjList[PLAYER_OBJECT].Health -= Damage;
		if (ae->ObjList[PLAYER_OBJECT].Health <= 0)
		{
			ae->ObjList[PLAYER_OBJECT].Health = -1;			//	DEAD
			ae->ObjList[PLAYER_OBJECT].Dead = 1;
			TRACE("...Player is DEAD...\n");
			ae->ObjList[ObjIndex].Flags &= ~OF_SEEN_PLAYER;
			ObjPtr->CurNum = ObjPtr->IdlMax;
		}
		else
		{
			TRACE("Player health after hit is %ld\n",ae->ObjList[PLAYER_OBJECT].Health);
			if (!(ae->ObjList[ObjIndex].Flags & OF_SEEN_PLAYER))
			{
				ae->ObjList[ObjIndex].Flags |= OF_SEEN_PLAYER;
				ae->ObjList[ObjIndex].Enemy = PLAYER_OBJECT;
				ObjPtr->SkNum = 0;
				ObjPtr->CurNum = ObjPtr->MovMax;
			}
		}
		if (fds) fds->TextHealth();
	}
	else
	{
		ae->ObjList[ObjIndex].Flags &= ~OF_SEEN_PLAYER;
		ObjPtr->CurNum = ObjPtr->IdlMax;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Play the correct weapon sound

void WeaponSound(int Weapon, int Prio)
{
	switch (Weapon)
	{
	case 0:
		wavePlay(GRUNT, Prio);
		break;
	case 1:
		wavePlay(PISTOL_SHOT, Prio);
		break;
	case 2:
		wavePlay(SHOTGUN_SHOT, Prio);
		break;
	case 3:
		wavePlay(GUNFIRE, Prio);
		break;
	case 4:
		wavePlay(MISSLE, Prio);
		break;
	default:
		break;
	}
}

int LaunchMissle(int ObjIndex)
{
int		i;
	do
	{
		for (i=1 ; i < ae->MaxObjects ; i++)
		{
			if (ae->ObjList[i].Flags & OF_ONE_SHOT && !ae->ObjList[i].Active) break;
		}
		if (i == ae->MaxObjects)
		{
			if (ae->MaxObjects == MAX_OBJECTS) continue;	//	No more Object slots
			ae->ObjList[i].Flags		= OF_ANIMATE|OF_SEEN_PLAYER|OF_ONE_SHOT;	// Object Flags (OF_xxxxxx)
			ae->ObjList[i].Health		= Weapons[LAUNCHER].Damage;

			ObjList[i]->SksMax		= 1;			// Number of updates in one Dir before course correction
			ObjList[i]->SkNum			= 0;			// Start after Player, immediately
			ObjList[i]->Speed			= 64;			// Speed of Object movement
			ObjList[i]->Damage		= Weapons[LAUNCHER].Damage;
			ObjList[i]->IdlNum		= 0;
			ObjList[i]->IdlMax		= 0;
			ObjList[i]->MovNum		= 0;
			ObjList[i]->MovMax		= 0;
			ObjList[i]->CurNum		= 0;			//	Starting idle bitmap
			ObjList[i]->bmNum[0]	= 18;			//	Idle bitmaps
			ObjList[i]->mvFlg[0] 	= 1;
			ObjList[i]->bmDsp[0] 	= 1;
			ObjList[i]->CheckDist	= 16;			//	Let missles get close
			ae->MaxObjects++;
		}
		ae->ObjList[i].Enemy	= ObjIndex;
		ae->ObjList[i].Active	= 1;
		ae->ObjList[i].xl			= ae->ObjList[ObjIndex].xl;
		ae->ObjList[i].yl			= ae->ObjList[ObjIndex].yl;
		ae->ObjList[i].mPos		= ae->ObjList[ObjIndex].mPos;
		ae->ObjList[i].Dir		= ae->ObjList[ObjIndex].Dir;
		ae->ObjList[i].bmNum	= i;
		//	This will activate nearby Objects just like we shot a gun
		AckCheckObjHit(ae, ObjIndex,
						ae->ObjList[ObjIndex].x,
						ae->ObjList[ObjIndex].y,
						ae->ObjList[ObjIndex].Dir,
						0,&i);
		return 1;
	}
	while (0);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// User action message routine

long CFdView::OnUserAction(WPARAM wParam, LPARAM lParam)
{
int			i, j, tmp, NewAngle, ObjIndex, Distance, result, hits, recoil;
int			x_tmp, y_tmp;
long		work;
ANIMATION	*ObjPtr;

	if (fActivateApp && !fPaused)
	{
		//	process key input
		if (key_ltr_a && !airborne)
		{
			ds -= 12 + key_shift * 12;
			if (rel_ltr_a) rel_ltr_a = key_ltr_a = 0;
		}
		if (key_ltr_s && !airborne)
		{
			ds = 12 + key_shift * 12;
			if (rel_ltr_s) rel_ltr_s = key_ltr_s = 0;
		}
		if (key_up && !airborne)
		{
			dcy = 12 + key_shift * 12;
			if (rel_up) rel_up = key_up = 0;
		}
		if (key_down && !airborne)
		{
			dcy -= 12 + key_shift * 12;
			if (rel_down) rel_down = key_down = 0;
		}
		if (key_right && !airborne)
		{
			dcx = INT_ANGLE_1 + key_shift * INT_ANGLE_1;
			if (rel_right) rel_right = key_right = 0;
		}
		if (key_left && !airborne)
		{
			dcx -= INT_ANGLE_1 + key_shift * INT_ANGLE_1;
			if (rel_left) rel_left = key_left = 0;
		}
		if (newWeapon >= 0)
		{
			if (!disp_shot)
			{
				ObjList[PLAYER_OBJECT]->Weapon = newWeapon;
				newWeapon = -1;
				if (fds) fds->TextAmmo();
			}
		}
		if (key_control)
		{
			if (fire_shot && !disp_shot)
			{
				if (ae->WeaponsCarried[ObjList[PLAYER_OBJECT]->Weapon].Possessed &&
					*ae->WeaponsCarried[ObjList[PLAYER_OBJECT]->Weapon].Ammo >=
						Weapons[ObjList[PLAYER_OBJECT]->Weapon].Used)
				{
					//	Play the correct sound for this weapon
					dcy -= Weapons[ObjList[PLAYER_OBJECT]->Weapon].Recoil;
					disp_shot = 1; hits = 0;
					if (ObjList[PLAYER_OBJECT]->Weapon == LAUNCHER)
					{
						//	If not enough room in ObjList, don't fire missle or make sound (misfire!?)
						if (LaunchMissle(PLAYER_OBJECT)) WeaponSound(LAUNCHER, 60);
					}
					else
					{
						//	Play the correct sound for this weapon
						WeaponSound(ObjList[PLAYER_OBJECT]->Weapon, 60);
						j = Weapons[ObjList[PLAYER_OBJECT]->Weapon].Rounds;
						tmp = Weapons[ObjList[PLAYER_OBJECT]->Weapon].Spread;
						for (i=0 ; i < j ; i++)
						{
							NewAngle = ae->ObjList[PLAYER_OBJECT].Dir;
							if (i)
							{
								NewAngle += (rand() % tmp) - (rand() % tmp);
								if (NewAngle < 0) NewAngle += INT_ANGLE_360;
								else
								if (NewAngle > INT_ANGLE_360) NewAngle -= INT_ANGLE_360;
							}
							result = AckCheckObjHit(ae, PLAYER_OBJECT,
											ae->ObjList[PLAYER_OBJECT].x,
											ae->ObjList[PLAYER_OBJECT].y,
											NewAngle,tmp,&Distance);
							if (result == POV_OBJECT)
							{
								ObjectHit = AckGetObjectHit();
								if (!(ae->ObjList[ObjectHit].Flags & OF_CAN_PICKUP))
								{
									if (Weapons[ObjList[PLAYER_OBJECT]->Weapon].Range >= Distance)
									{
										if (ae->ObjList[ObjectHit].Health >= 0)
										{
											work =	long_divide((long)Weapons[ObjList[PLAYER_OBJECT]->Weapon].Damage *
																					(long)(Weapons[ObjList[PLAYER_OBJECT]->Weapon].Range -
																						Distance),
																					(long)Weapons[ObjList[PLAYER_OBJECT]->Weapon].Range);
											ae->ObjList[ObjectHit].Health -= (int)work;
											hits++;
											if (ae->ObjList[ObjectHit].Health <= 0)
											{
												ae->ObjList[ObjectHit].bmNum	= GoreObject;
												ae->ObjList[ObjectHit].Dir		= 0;
												ae->ObjList[ObjectHit].Flags	= OF_PASSABLE;
												ae->ObjList[ObjectHit].Health	= 0;
											}
											else
											{
												ae->ObjList[ObjectHit].Flags |= OF_SEEN_PLAYER;
												ae->ObjList[ObjectHit].Enemy = PLAYER_OBJECT;
												ObjList[ObjectHit]->SkNum = 0;
												ObjList[ObjectHit]->CurNum = ObjList[ObjectHit]->MovMax;
											}
										}
										TRACE("Object (%d) health after hit is %d\n", ObjectHit, ae->ObjList[ObjectHit].Health);
									}
								}
							}
						}
						if (hits)
						{
							TRACE("Hit!\n");
							if (fds) fds->TextMsg("Hit!");
						}
						else
						{
							TRACE("Missed!\n");
							if (fds) fds->TextMsg("Missed!");
						}
					}
					*ae->WeaponsCarried[ObjList[PLAYER_OBJECT]->Weapon].Ammo -=
								Weapons[ObjList[PLAYER_OBJECT]->Weapon].Used;
					if (fds) fds->TextAmmo();
				}
				else
					if (fds) fds->TextMsg("Not Enough Ammo");
				if (rapid_fire ^ key_shift)
					fire_shot = 1;
				else
					fire_shot = 0;
			}
			if (rel_control) rel_control = key_control = fire_shot = 0;
		}

		//	Construct Player POV
		if (dcx || dcy || ds || last_dcx || last_dcy || last_ds)
		{
			//	Forward and backward
			tmp = dcy - last_dcy;
			if (((tmp > 0) && (tmp < 3)) || ((tmp < 0) && (tmp > -3)))
				last_dcy += tmp;
			else
				last_dcy += tmp / 3;

			//	Turn left and right
			if (dcx)
			{
				last_dcx += dcx;
				NewAngle = INT_ANGLE_10 * (key_shift + 1);
				if (last_dcx > NewAngle) last_dcx = NewAngle;
				else
				if (last_dcx < -NewAngle) last_dcx = -NewAngle;
			}
			else
				last_dcx = 0;

			//	Strafe left and right
			tmp = ds - last_ds;
			if (((tmp > 0) && (tmp < 3)) || ((tmp < 0) && (tmp > -3)))
				last_ds += tmp;
			else
				last_ds += tmp / 3;
		}
		if (jumping && !ae->ObjList[PLAYER_OBJECT].Dead)
		{
			switch (jumping)
			{
			case 1:
			case 3:
			case 8:
			case 10:
				ViewUpDown = 32 << (8 - ViewLowRes);
				break;
			case 2:
				ViewUpDown = 38 << (8 - ViewLowRes);
				break;
			case 4:
				ViewUpDown = 20 << (8 - ViewLowRes);
				airborne = 1;
				break;
			case 5:
			case 6:
				ViewUpDown = 8 << (8 - ViewLowRes);
				break;
			case 7:
				ViewUpDown = 20 << (8 - ViewLowRes);
				break;
			case 9:
				ViewUpDown = 40 << (8 - ViewLowRes);
				wavePlay(GRUNT, 60);
				airborne = 0;
				break;
			default:
				jumping = 0;
				break;
			}
			if (jumping) jumping++;
		}
		dcx=0, dcy=0; ds = 0;
		if (last_dcx || last_dcy || last_ds)
		{
			//	make the Player jog up/down will moving
			if ((last_dcy || last_ds) && !ae->ObjList[PLAYER_OBJECT].Dead && !jumping)
			{
				switch (jog)
				{
				default:
					jog = 0;
				case 0:
				case 4:
					ViewUpDown = 32 << (8 - ViewLowRes);
					break;
				case 1:
				case 3:
					ViewUpDown = 33 << (8 - ViewLowRes);
					break;
				case 2:
					ViewUpDown = 34 << (8 - ViewLowRes);
					break;
				case 5:
				case 7:
					ViewUpDown = 31 << (8 - ViewLowRes);
					break;
				case 6:
					ViewUpDown = 30 << (8 - ViewLowRes);
					break;
				}
				jog++;
			}
			if (last_dcx)
			{
				ae->ObjList[PLAYER_OBJECT].Dir += last_dcx;
				if (ae->ObjList[PLAYER_OBJECT].Dir >= INT_ANGLE_360)
			    	ae->ObjList[PLAYER_OBJECT].Dir -= INT_ANGLE_360;
			    else
				if (ae->ObjList[PLAYER_OBJECT].Dir < 0)
			    	ae->ObjList[PLAYER_OBJECT].Dir += INT_ANGLE_360;
			}
			NewAngle = ae->ObjList[PLAYER_OBJECT].Dir;
			if (last_dcy > 0)
			{
				tmp = last_dcy;
				Distance = (int)long_sqrt((last_dcy * last_dcy) + (last_ds * last_ds));
				if (last_ds > 0)
					NewAngle += ArcSinTable[(last_ds << 8) / Distance];
				else
				if (last_ds < 0)
					NewAngle -= ArcSinTable[(-last_ds << 8) / Distance];
			}
			else
			if (last_dcy < 0)
			{
				tmp = -last_dcy;
				Distance = (int)long_sqrt((last_dcy * last_dcy) + (last_ds * last_ds));
				NewAngle += INT_ANGLE_180;
				if (last_ds > 0)
					NewAngle -= ArcSinTable[(last_ds << 8) / Distance];
				else
				if (last_ds < 0)
					NewAngle += ArcSinTable[(-last_ds << 8) / Distance];
					
			}
			else
			if (last_ds > 0)
			{
				NewAngle += INT_ANGLE_90;
		    tmp = last_ds;
			}
			else
			if (last_ds < 0)
			{
				NewAngle -= INT_ANGLE_90;
		    tmp = -last_ds;
			}
			if (NewAngle >= INT_ANGLE_360)
			    NewAngle -= INT_ANGLE_360;
			else
			if (NewAngle < 0)
			    NewAngle += INT_ANGLE_360;
			//	save Player's present position
			x_tmp = ae->ObjList[PLAYER_OBJECT].x;
			y_tmp = ae->ObjList[PLAYER_OBJECT].y;
			//	calculate and update Player position
			result = AckMoveObjectPOV(ae,PLAYER_OBJECT,NewAngle,tmp);
			//	check if Player has moved; if not, clear movement registers
			if (abs(x_tmp - ae->ObjList[PLAYER_OBJECT].x) < 3 &&
					abs(y_tmp - ae->ObjList[PLAYER_OBJECT].y) < 3)
			{
				last_dcy = 0; last_ds = 0; jog = 0;
			}
			for (i=0 ; i < TotalSpecial ; i++)
			{
				if (SpecialCodes[i].mPos == ae->ObjList[PLAYER_OBJECT].mPos)
				{
					if (SpecialCodes[i].mCode != LastSpecial)
					{
						switch (SpecialCodes[i].mCode)
						{
						case MAP_STARTCODE:	//	where Player started
							if (fds) fds->TextMsg("Started Here");
							break;
						case MAP_UPCODE:		//	up a level
							if (fds) fds->TextMsg("Up a Level");
							wavePlay(BOOUP, 40);
							break;
						case MAP_DOWNCODE:	//	down a level
							if (fds) fds->TextMsg("Down a Level");
							wavePlay(BOODOWN, 40);
							break;
						case MAP_GOALCODE:	//	game over, man!
							if (fds) fds->TextMsg("You Won!");
							wavePlay(CUCKOO, 50);
							break;
						}
						LastSpecial = SpecialCodes[i].mCode;
					}
					break;
				}
			}
			if (i == TotalSpecial) LastSpecial = 0xffff;
			switch (result)
			{
			case POV_OBJECT:
				//TRACE("Collision with Object\n");
				ObjectHit = AckGetObjectHit();
				if ((tmp = ae->ObjList[ObjectHit].Flags) & OF_CAN_PICKUP)
				{
					//	Get more Health from object
					if (ae->ObjList[ObjectHit].Flags & OF_HEALTH &&
						ae->ObjList[PLAYER_OBJECT].Health < MAX_HEALTH)
					{
						wavePlay(CHIMES, 40);
						ae->ObjList[PLAYER_OBJECT].Health += ae->ObjList[ObjectHit].Health;
						if (ae->ObjList[PLAYER_OBJECT].Health > MAX_HEALTH)
							ae->ObjList[PLAYER_OBJECT].Health = MAX_HEALTH;
						if (fds) fds->TextHealth();
						if (fds) fds->TextMsg("Health points");
						AckDeleteObject(ae,ObjectHit);	//	deactivate object
					}
					//	Get more Ammo from object
					if (ae->ObjList[ObjectHit].Flags & OF_AMMUNITION &&
						*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo < Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo)
					{
						wavePlay(CHIMES, 40);
						*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo += ae->ObjList[ObjectHit].Ammo;
						if (*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo > Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo)
							*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo = Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo;
						if (fds) fds->TextAmmo();
						if (fds) fds->TextMsg("Ammunition");
						AckDeleteObject(ae,ObjectHit);	//	deactivate object
					}
					//	Get new Weapon and/or more ammo from object
					if (ae->ObjList[ObjectHit].Flags & OF_WEAPON)
					{
						if (!ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Possessed ||
							*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo < Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo)
						{
							wavePlay(CHIMES, 40);
							ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Possessed = 1;
							*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo += ae->ObjList[ObjectHit].Ammo;
							if (*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo > Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo)
								*ae->WeaponsCarried[ObjList[ObjectHit]->Weapon].Ammo = Weapons[ObjList[ObjectHit]->Weapon].MaxAmmo;
							if (fds) fds->TextAmmo();
							if (fds) fds->TextMsg(Weapons[ObjList[ObjectHit]->Weapon].Name);
							AckDeleteObject(ae,ObjectHit);	//	deactivate object
						}
					}
					//	unlock doors with key
					if (tmp &= DOOR_KEY_MASK)
					{
						wavePlay(PING, 40);
						//	unlock all correct color keyed doors
						//	some doors may require multiple keys
						for (i=0 ; i < GRID_ARRAY ; i++)
						{
							if ((ae->xGrid[i] & 0xFF) >= DOOR_XCODE)
								if (ae->xGrid[i] & tmp) ae->xGrid[i] &= ~tmp;
							if ((ae->yGrid[i] & 0xFF) >= DOOR_YCODE)
								if (ae->yGrid[i] & tmp) ae->yGrid[i] &= ~tmp;
						}
						if (tmp & OF_KEY_RED)
						{
							ae->Keys |= KEY_RED;
							if (fds)
							{
								fds->TextMsg("RED key");
								fds->DrawKeys();
							}
						}
						else
						if (tmp & OF_KEY_GREEN)
						{
							ae->Keys |= KEY_GREEN;
							if (fds)
							{
								fds->TextMsg("GREEN key");
								fds->DrawKeys();
							}
						}
						else
						if (tmp & OF_KEY_BLUE)
						{
							ae->Keys |= KEY_BLUE;
							if (fds)
							{
								fds->TextMsg("BLUE key");
								fds->DrawKeys();
							}
						}
						AckDeleteObject(ae,ObjectHit);	//	deactivate object
					}
				}
				//	Can do damage when we pick things up
				if (ObjList[ObjectHit]->Damage)
				{
					//	if Object is a Force Field, Player can jump over it!
					if (ae->ObjList[ObjectHit].Flags & OF_FORCE_FIELD)
						if (airborne) break;
					CheckHealth(ObjectHit, ObjList[ObjectHit]->Damage);
				}
				break;
			case POV_XWALL:
			case POV_YWALL:
			case POV_DOORX_REG:
			case POV_DOORY_REG:
			case POV_DOORX:
			case POV_DOORY:
			case POV_NOTHING:
			case POV_PLAYER:	//	Should never happen
				break;
			default:
				TRACE("Collision...\n");
				break;
			}
		}

		//	Construct Object POV
		for (ObjIndex=1,recoil=0 ; ObjIndex < ae->MaxObjects ; ObjIndex++)
		{
			ObjPtr = ObjList[ae->ObjList[ObjIndex].bmNum];
			if (ae->ObjList[ObjIndex].Active && ObjPtr->Speed)
			{
				if (!(ae->ObjList[ObjIndex].Flags & OF_SEEN_PLAYER) && !ae->ObjList[PLAYER_OBJECT].Dead)
				{
					result = AckCheckObjView(ae,ObjIndex,
											ae->ObjList[ObjIndex].Enemy,
											ae->ObjList[ObjIndex].x,
											ae->ObjList[ObjIndex].y,
											ae->ObjList[ObjIndex].Dir,
											ObjPtr->FieldOV);
					if (result == POV_PLAYER)
					{
						ae->ObjList[ObjIndex].Flags |= OF_SEEN_PLAYER;
						ae->ObjList[ObjIndex].Enemy = PLAYER_OBJECT;
						ObjPtr->SkNum = 0;
						ObjPtr->CurNum = ObjPtr->MovMax;
					}
				}
				else
				{
					if (ObjPtr->mvFlg[ObjPtr->CurNum])
					{
						if (recoil)
						{
							NewAngle = ae->ObjList[ObjIndex].Dir - INT_ANGLE_180;
							if (NewAngle < 0) NewAngle += INT_ANGLE_360;
							result = AckMoveObjectPOV(ae,ObjIndex,NewAngle,recoil);
						}
						else
							result = AckMoveObjectPOV(ae,ObjIndex,
															ae->ObjList[ObjIndex].Dir,
															ObjPtr->Speed);

						if (ae->ObjList[ObjIndex].Flags & OF_ONE_SHOT)	//	Rocket
						{
							switch (result)
							{
							case POV_PLAYER:
								//if (ae->ObjList[ObjIndex].Enemy == PLAYER_OBJECT) break;
								wavePlay(EXPLODE, 60);
								if (ObjPtr->Damage)
								{
									CheckHealth(ObjIndex, ObjPtr->Damage);
								}
								ae->ObjList[ObjIndex].Active = 0;
								break;
							case POV_OBJECT:
								ObjectHit = AckGetObjectHit();
								if (ae->ObjList[ObjectHit].Flags & (OF_PASSABLE|OF_CAN_PICKUP|OF_STATIC))
									break;
								wavePlay(EXPLODE, 60);
								if (ae->ObjList[ObjectHit].Flags & OF_ONE_SHOT)
									ae->ObjList[ObjectHit].Active = 0;
								else
								{
									ae->ObjList[ObjectHit].Health -= ObjPtr->Damage;
									if (ae->ObjList[ObjectHit].Health <= 0)
									{
										ae->ObjList[ObjectHit].bmNum	= GoreObject;
										ae->ObjList[ObjectHit].Dir		= 0;
										ae->ObjList[ObjectHit].Flags	= OF_PASSABLE;
										ae->ObjList[ObjectHit].Health	= 0;
									}
								}
								ae->ObjList[ObjIndex].Active = 0;
								break;
							case POV_XWALL:
							case POV_YWALL:
							case POV_DOORX:
							case POV_DOORY:
								wavePlay(EXPLODE, 60);
								ae->ObjList[ObjIndex].Active = 0;
								break;
							default:
								break;
							}
							continue;
						}	//	End Rocket

						switch (result)	//	Object Movement Check
						{
						case POV_NOTHING:
							if (ObjPtr->SkNum == 0)
							{
								NewAngle = AckObjectAngle(ae, ae->ObjList[ObjIndex].Enemy, ae->ObjList[ObjIndex].x, ae->ObjList[ObjIndex].y);
								ae->ObjList[ObjIndex].Dir = NewAngle;
								ObjPtr->SkNum = ObjPtr->SksMax;
							}
							else
								ObjPtr->SkNum--;
							break;
						case POV_DOORX_REG:
						case POV_DOORY_REG:
							CheckOnDoor(ae->ObjList[ObjIndex].x, ae->ObjList[ObjIndex].y, ae->ObjList[ObjIndex].Dir);
							break;
						case POV_PLAYER:
							wavePlay(GRUNT, 60);
							if (ObjPtr->Damage)
							{
								CheckHealth(ObjIndex, ObjPtr->Damage);
							}
							NewAngle = AckObjectAngle(ae, ae->ObjList[ObjIndex].Enemy, ae->ObjList[ObjIndex].x, ae->ObjList[ObjIndex].y);
							NewAngle += (rand() % INT_ANGLE_90) - (rand() % INT_ANGLE_90);
							if (NewAngle >= INT_ANGLE_360) NewAngle -= INT_ANGLE_360;
							else
							if (NewAngle < 0) NewAngle += INT_ANGLE_360;
							ae->ObjList[ObjIndex].Dir = NewAngle;
							ObjPtr->SkNum = ObjPtr->SksMax;
							break;
						case POV_OBJECT:
							ObjectHit = AckGetObjectHit();
							if (ae->ObjList[ObjectHit].Flags & (OF_PASSABLE|OF_CAN_PICKUP))
								break;
						case POV_XWALL:
						case POV_YWALL:
						case POV_DOORX:
						case POV_DOORY:
						default:
							NewAngle = AckObjectAngle(ae, ae->ObjList[ObjIndex].Enemy, ae->ObjList[ObjIndex].x, ae->ObjList[ObjIndex].y);
							NewAngle += (rand() % INT_ANGLE_90) - (rand() % INT_ANGLE_90);
							if (NewAngle >= INT_ANGLE_360) NewAngle -= INT_ANGLE_360;
							else
							if (NewAngle < 0) NewAngle += INT_ANGLE_360;
							ae->ObjList[ObjIndex].Dir = NewAngle;
							ObjPtr->SkNum = ObjPtr->SksMax;
							break;
						}	//	End Object Movement Check
						
						if (recoil)	//	If we are processing a recoil, don't fire again
						{
							recoil = 0;
							continue;
						}
					}

					//	Fire Object Weapon
					if (ae->ObjList[ObjIndex].Flags & OF_SEEN_PLAYER)
					{
						//	Only fire every so often
						if (ObjPtr->FireDly)
						{
							ObjPtr->FireDly--;
							continue;
						}
						//	Need enough ammo to fire
						if (ae->ObjList[ObjIndex].Ammo < Weapons[ObjPtr->Weapon].Used) continue;
						//	A little randomness thrown in
						if (rand() % ObjPtr->FireFreq) continue;
						//	Can we see out enemy?
						result = AckCheckObjView(ae,ObjIndex,
												ae->ObjList[ObjIndex].Enemy,
												ae->ObjList[ObjIndex].x,
												ae->ObjList[ObjIndex].y,
												ae->ObjList[ObjIndex].Dir,
												ObjPtr->FieldOV);
						//	Out of view
						if (result == POV_NOTHING) continue;

						//	Fire our shots
						if (ObjPtr->Weapon == LAUNCHER)
						{
							//	If not enough room in ObjList, don't fire missle or make sound (misfire!?)
							if (LaunchMissle(ObjIndex))
							{
								WeaponSound(LAUNCHER, 60);
								recoil = Weapons[LAUNCHER].Recoil;
							}
						}
						else
						{
							//	Play the correct sound for this weapon
							WeaponSound(ObjPtr->Weapon, 60);
							recoil = Weapons[ObjPtr->Weapon].Recoil;
							ObjPtr->FireDly = Weapons[ObjPtr->Weapon].Delay;
							j = Weapons[ObjPtr->Weapon].Rounds;
							tmp = Weapons[ObjPtr->Weapon].Spread;
							for (i=0 ; i < j ; i++)
							{
								NewAngle = ae->ObjList[ObjIndex].Dir;
								if (i)
								{
									NewAngle += (rand() % tmp) - (rand() % tmp);
									if (NewAngle < 0) NewAngle += INT_ANGLE_360;
									else
									if (NewAngle > INT_ANGLE_360) NewAngle -= INT_ANGLE_360;
								}
								result = AckCheckObjHit(ae, ObjIndex,
														ae->ObjList[ObjIndex].x,
														ae->ObjList[ObjIndex].y,
														NewAngle,tmp,&Distance);
								switch (result)
								{
								case POV_PLAYER:
									if (Weapons[ObjPtr->Weapon].Range >= Distance)
									{
										work =	long_divide((long)Weapons[ObjPtr->Weapon].Damage *
																				(long)(Weapons[ObjPtr->Weapon].Range -
																					Distance),
																				(long)Weapons[ObjPtr->Weapon].Range);
										CheckHealth(ObjIndex, (int)work);
									}
									break;
								case POV_OBJECT:
									ObjectHit = AckGetObjectHit();
									if (!(ae->ObjList[ObjectHit].Flags & OF_CAN_PICKUP))
									{
										if (Weapons[ObjPtr->Weapon].Range >= Distance)
										{
											if (ae->ObjList[ObjectHit].Health >= 0)
											{
												work =	long_divide((long)Weapons[ObjPtr->Weapon].Damage *
																						(long)(Weapons[ObjPtr->Weapon].Range -
																							Distance),
																						(long)Weapons[ObjPtr->Weapon].Range);
												ae->ObjList[ObjectHit].Health -= (int)work;
												if (ae->ObjList[ObjectHit].Health <= 0)
												{
													ae->ObjList[ObjectHit].bmNum	= GoreObject;
													ae->ObjList[ObjectHit].Dir		= 0;
													ae->ObjList[ObjectHit].Flags	= OF_PASSABLE;
													ae->ObjList[ObjectHit].Health	= 0;
													ae->ObjList[ObjIndex].Enemy = PLAYER_OBJECT;
												}
												else
												{
													ObjPtr->SkNum = 0;
													ObjPtr->CurNum = ObjPtr->MovMax;

													ae->ObjList[ObjectHit].Flags |= OF_SEEN_PLAYER;
													ae->ObjList[ObjectHit].Enemy = ObjIndex;
													ae->ObjList[ObjIndex].Enemy = ObjectHit;
												}
											}
											TRACE("Object (%d) health after hit is %d\n", ObjectHit, ae->ObjList[ObjectHit].Health);
										}
									}
									break;
								default:
									break;
								}
							}
						}
						ObjIndex--;	//	Process weapon recoil before continuing
					}
				}
			}
		}
		AckCheckObjectMovement(ae);
		if (!ViewForward)
		{
			NewAngle = ae->ObjList[PLAYER_OBJECT].Dir;
			if (ae->ObjList[PLAYER_OBJECT].Dir >= INT_ANGLE_180)
				ae->ObjList[PLAYER_OBJECT].Dir -= INT_ANGLE_180;
			else
				ae->ObjList[PLAYER_OBJECT].Dir += INT_ANGLE_180;
		}
		if (ae->ObjList[PLAYER_OBJECT].Dead)
		{
			if (ae->ObjList[PLAYER_OBJECT].Dead < 16) ae->ObjList[PLAYER_OBJECT].Dead++;
			ViewUpDown = (32 + ae->ObjList[PLAYER_OBJECT].Dead) << (8 - ViewLowRes);	//	if DEAD, lie down
		}
		AckBuildView(ae);
		if (!ViewForward) ae->ObjList[PLAYER_OBJECT].Dir = NewAngle;
		if (ae->ObjList[PLAYER_OBJECT].Dead || PlayerHit)
		{
			//	screen size, frequency and Palette entry number
			TintScreen(ScreenBuffer,(UINT)(VIEW_LENGTH >> (ViewLowRes << 1)), 6 >> ViewLowRes, 15);
		}
		if (PlayerHit) PlayerHit--;
	}

	if (fIsIconic)
	{
		if (fActivateApp)
		{
			//	Application is Active/Iconic
		}
	}
	else
	{
		if (fActivateApp && !fPaused)
		{
			PaintScreen(0, 0, BufferX, BufferY, 0, 0, ViewXS, ViewYS);
		}
	}
	return 0;
}   

/////////////////////////////////////////////////////////////////////////////
// User sound message routine

long CFdView::OnUserSound(WPARAM SndNumber, LPARAM Prio)
{
	wavePlay(SndNumber, LOWORD(Prio));
	return 0;
}

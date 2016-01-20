static	int		count=0, xxx, yyy;
CPoint	xy;
	if (!CanPaint) return 0;
	if ((count <= 0) || (newX == 0) || (newX == dx) || (newY == 0) || (newY == dy))
	{
		count = rand() & 15;
		xxx = (rand() & 15) - (rand() & 15);
		yyy = (rand() & 15) - (rand() & 15);
	}
	xy.x = newX + xxx;
	if (xy.x < 0) xy.x = 0; else if (xy.x > dx) xy.x = dx;
	xy.y = newY + yyy;
	if (xy.y < 0) xy.y = 0; else if (xy.y > dy) xy.y = dy;
	count--;
	PositionPaint(xy);
//MSG		msg;
//	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//	{
//		TRACE("msg.message = %x\n", msg.message);
//		if (msg.message == WM_QUIT) exit;
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}

#ifdef	_DEBUG
long	l, r;
	if (NumberOfPaints >= 10000)
	{
		StopTime = CTime::GetCurrentTime();
		ElapsedTime = StopTime - StartTime;
		TRACE("Elapsed time was %s\n", ElapsedTime.Format("%H:%M:%S"));
		TRACE("Total number of screen paints was %ld\n", NumberOfPaints);
		l = (NumberOfPaints * 100) / ElapsedTime.GetTotalSeconds();
		r = l - l / 100 * 100;
		l = l / 100;
		TRACE("Number of frames per second was %ld.%02ld\n", l, r);
	}
	else
#endif

	PostMessage(WM_USER_ONE, 0, 0l);	//	Recall ourselves for the fastest updates

	CopySomeDIBBits((BITMAPINFO far *)&DibBufferInfo,
  					(char huge *)pBufferBits,
  					newTopLeftX,
    				newTopLeftY,
    				(BITMAPINFO *)&DibMapInfo,
    				(char huge *)pMapBits,
  					newTopLeftX,
    				newTopLeftY,
    				BitmapX[BitmapNumber],
    				BitmapY[BitmapNumber]);


int		nMapped;
	if (CanPaint)
	{
#ifdef	_DEBUG
long	l, r;
		StopTime = CTime::GetCurrentTime();
		TRACE("Stop time is %s\n", StopTime.Format("%H:%M:%S"));
		ElapsedTime = StopTime - StartTime;
		TRACE("Elapsed time was %s\n", ElapsedTime.Format("%H:%M:%S"));
		TRACE("Total number of screen paints was %ld\n", NumberOfPaints);
		l = (NumberOfPaints * 100) / ElapsedTime.GetTotalSeconds();
		r = l - l / 100 * 100;
		l = l / 100;
		TRACE("Number of frames per second was %ld.%02ld\n", l, r);
#endif
	}
	else
		CanPaint = -1;
	NumberOfPaints = 0;
#ifdef	_DEBUG
	StartTime = CTime::GetCurrentTime();
	TRACE("Start time is %s\n", StartTime.Format("%H:%M:%S"));
#endif
	dc.SelectPalette(&CPalApp, FALSE);
	nMapped = dc.RealizePalette();
	TRACE("Number of new entries mapped in palette = %d\n", nMapped);
	WinGBitBlt(dc.m_hDC, 0, 0, BufferX, BufferY, hdcBuffer, 0, 0);
	NumberOfPaints++;



    pDibBgd[0] = DibOpenFile("bgd.bmp");

	for (Counter=0 ; Counter < NUMBER_OF_BACKGROUNDS ; Counter++)
	{
		BgdX[Counter] = DibWidth(pDibBgd[Counter]);
		BgdY[Counter] = DibHeight(pDibBgd[Counter]);
	}

	for (Counter=0 ; Counter < 21 ; Counter++)
		sysColors[Counter] = GetSysColor(sysNames[Counter]);

    //  create an identity palette from the DIB's color table
    pColorTable = (RGBQUAD far *)((char far *)pDibBgd[0] + pDibBgd[0]->biSize);
    for (Counter=1 ; Counter < 255 ; Counter++)
    {
        // copy from the original color table to the WinGBitmap's
        // color table and the logical palette
		LogicalPalette.palEntry[Counter].peRed = pColorTable[Counter].rgbRed;
		LogicalPalette.palEntry[Counter].peGreen = pColorTable[Counter].rgbGreen;
		LogicalPalette.palEntry[Counter].peBlue = pColorTable[Counter].rgbBlue;
		LogicalPalette.palEntry[Counter].peFlags = PC_NOCOLLAPSE;	//	PC_RESERVED;	//	PC_NOCOLLAPSE;
    }
	LogicalPalette.palEntry[0].peRed = 0;
	LogicalPalette.palEntry[0].peGreen = 0;
	LogicalPalette.palEntry[0].peBlue = 0;
	LogicalPalette.palEntry[0].peFlags = 0;	//	PC_RESERVED;	//	PC_NOCOLLAPSE;

	LogicalPalette.palEntry[255].peRed = 255;
	LogicalPalette.palEntry[255].peGreen = 255;
	LogicalPalette.palEntry[255].peBlue = 255;
	LogicalPalette.palEntry[255].peFlags = 0;	//	PC_RESERVED;	//	PC_NOCOLLAPSE;

    CPalApp.CreatePalette((LOGPALETTE *)&LogicalPalette);
    CPalPtr = &CPalApp;
#ifdef	_DEBUG
int		i, j, diff=0;
	for (i=0 ; i < 255 ; i++)
		for (j=i+1 ; j < 256 ; j++)
			if ((LogicalPalette.palEntry[i].peRed == LogicalPalette.palEntry[j].peRed) &&
			(LogicalPalette.palEntry[i].peGreen == LogicalPalette.palEntry[j].peGreen) &&
			(LogicalPalette.palEntry[i].peBlue == LogicalPalette.palEntry[j].peBlue))
			{
				TRACE("  Duplicate palette entry found at 0x%02x/0x%02x\n", i, j);
				diff++;
			}
	TRACE("Palette created with %d identical entries\n", diff);
#endif



//#include <stdio.h>
//#include <conio.h>
//#include <process.h>
//#include <bios.h>
//#include <malloc.h>
//#include <memory.h>
//#include <fcntl.h>


//#include <stdlib.h>
//#include <stdio.h>
//#include <dos.h>
//#include <io.h>
//#include <fcntl.h>
//#include <time.h>
//#include <string.h>
//#include <sys\stat.h>

HitResult = AckCheckHit(ae->xPlayer,ae->yPlayer,Angle,ae);
if (!HitResult)
{
    if (AckCheckObjPosn(ae,x1,y1,Angle)) return(POV_OBJECT);

    MapPosn = (y1 & 0xFFC0) + (x1 >> 6);
    gCode = Grid[MapPosn] & 0xFF;
    if (gCode > 0 && gCode < DOOR_XCODE)
	{
		if (!(Grid[MapPosn] & DOOR_TYPE_SECRET)) return(POV_XWALL);
	}

    ae->xPlayer = x1;
    ae->yPlayer = y1;
    return(POV_NOTHING);	//	HitResult);
}

if (HitResult == POV_OBJECT) return(HitResult);
if (HitResult == POV_XWALL)
    x1 = ae->xPlayer;
else
    y1 = ae->yPlayer;

NewResult = AckCheckHit(x1,y1,Angle,ae);
if (!NewResult)
{
    if (AckCheckObjPosn(ae,x1,y1,Angle)) return(POV_OBJECT);

    MapPosn = (y1 & 0xFFC0) + (x1 >> 6);
    gCode = Grid[MapPosn] & 0xFF;
    if (gCode > 0 && gCode < DOOR_XCODE)
    {
		if (!(Grid[MapPosn] & DOOR_TYPE_SECRET)) return(POV_XWALL);
	}

    ae->xPlayer = x1;
    ae->yPlayer = y1;
	return(HitResult);
}
if (HitResult == NewResult)
{
    ae->xPlayer = x1;
    ae->yPlayer = y1;
}


handle = fopen("trig.dat","r+b");  /* Does file exist */
if (handle == NULL) return(ERR_BADFILE);

if (LongTanTable == NULL ||
    LongInvTanTable == NULL ||
    CosTable == NULL ||
    SinTable == NULL ||
    InvSinTable == NULL ||
    InvCosTable == NULL ||
    LongCosTable == NULL ||
    xNextTable == NULL ||
    yNextTable == NULL ||
    ViewCosTable == NULL ||
    Grid == NULL ||
    ObjGrid == NULL ||
    AdjustTable == NULL)
{
    fclose(handle);
    return(ERR_NOMEMORY);
}

len = sizeof(long) * INT_ANGLE_360;

fread(SinTable,len,1,handle);
fread(CosTable,len,1,handle);
fread(LongTanTable,len,1,handle);
fread(LongInvTanTable,len,1,handle);
fread(InvCosTable,len,1,handle);
fread(InvSinTable,len,1,handle);
fread(LongCosTable,len,1,handle);
	
fclose(handle);



    /* Remove fixed point and round-up */
    xd = WallDistance >> 14;
    if (WallDistance - (xd << 14) >= 8096) xd++;

    /* Remove initial fixed point and round-up */
    WallDistance = xd >> 6;
    if (xd - (WallDistance << 6) >= 32) WallDistance++;


void CMainFrame::OnViewStatusBar()
{
	// TODO: Add your command handler code here
	StatusBarView = 1 - StatusBarView;
	if (StatusBarView)
		m_wndStatusBar.ShowWindow(SW_NORMAL);
	else
		m_wndStatusBar.ShowWindow(SW_HIDE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (StatusBarView) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewToolbar()
{
	// TODO: Add your command handler code here
	ToolBarView = 1 - ToolBarView;
	if (ToolBarView)
		m_wndToolBar.ShowWindow(SW_NORMAL);
	else
		m_wndToolBar.ShowWindow(SW_HIDE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (ToolBarView) pCmdUI->SetCheck(TRUE); else pCmdUI->SetCheck(FALSE);
}


			if (fdsActive)
			{
				pDC = fds->GetDC();
				pDC->SelectPalette(&CPalApp,FALSE);
				pDC->RealizePalette();
				WinGStretchBlt(pDC->m_hDC, 0, 0, fdsWidth, fdsHeight, hdcBuffer, 0, 0, VIEW_XS, VIEW_YS);
				fds->ReleaseDC(pDC);
			}

							if (NewX > 0)
							{	if (NewY > 0)
									NewAngle = INT_ANGLE_225;
								else
								if (NewY < 0)
									NewAngle = INT_ANGLE_135;
								else
									NewAngle = INT_ANGLE_180;
							}
							else
							if (NewX < 0)
							{	if (NewY > 0)
									NewAngle = INT_ANGLE_315;
								else
								if (NewY < 0)
									NewAngle = INT_ANGLE_45;
								else
									NewAngle = 0;
							}
							else	//	NewX == 0
							{	if (NewY > 0)
									NewAngle = INT_ANGLE_270;
								else
									NewAngle = INT_ANGLE_90;
							}
							NewAngle += (rand() % INT_ANGLE_30) - (rand() % INT_ANGLE_30);
							if (NewAngle >= INT_ANGLE_360) NewAngle -= INT_ANGLE_360;
							else
							if (NewAngle < 0) NewAngle += INT_ANGLE_360;



void TBitBlts(UCHAR huge *pDestBits,UCHAR huge *pSrcBits,
		UINT DRowOff,int DCol,int bmWidth,int bmHeight);



256		128		64		32		16		8		4		2		1
1		2		4		8		16		32		64		128		256


	if (pOpenDocument != NULL)
	{
		POSITION pos = pOpenDocument->GetFirstViewPosition();
		if (pos != NULL)
		{
			CView* pView = pOpenDocument->GetNextView(pos); // get first one
			ASSERT_VALID(pView);
			CFrameWnd* pFrame = pView->GetParentFrame();
			if (pFrame != NULL)
				pFrame->ActivateFrame();
			else
				TRACE0("Error: Can not find a frame for document to activate");
			CFrameWnd* pAppFrame;
			if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
			{
				ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
				pAppFrame->ActivateFrame();
			}
		}
		else
		{
			TRACE0("Error: Can not find a view for document to activate");
		}
		return pOpenDocument;
			// file already open (even if we can't activate it)
	}



if (NewX > NewY)
{
	deltay = ArcCosTable[(labs(NewX) << 8) / distance];
	if (NewX < 0) deltay = -deltay;
}
else
{
	deltay = ArcSinTable[(labs(NewY) << 8) / distance];
	if (NewY < 0) deltay = -deltay;
}



//	if (xpHigh <= 0) continue;
//	yp = (NewY * CosValue) - (NewX * SinValue);
//	distance = long_sqrt(((long)xpHigh * (long)xpHigh) + ((long)ypHigh * (long)ypHigh));

//	distance = (int)((NewX * InvCosTable[PlayerAngle]) >> FP_SHIFT);
//	if (distance < 0) continue;



else
if (NewAmount < Amount)
{
	TmpAngle = NewAngle + INT_ANGLE_180;
	if (TmpAngle >= INT_ANGLE_360) TmpAngle -= INT_ANGLE_360;
	TmpAmount = 0;
	AckCheckHit(ae,ae->xPlayer,ae->yPlayer,TmpAngle,&TmpAmount);
	if (TmpAmount < 0)
	{
		return(HitResult);
	}
	AckCheckObjPosn(ae,ae->xPlayer,ae->yPlayer,TmpAngle,&TmpAmount);
	if (TmpAmount < 0)
    {
    	return(HitResult);
    }
}


int		TmpAngle,TmpAmount;

			TmpAngle = NewAngle + INT_ANGLE_180;
			if (TmpAngle >= INT_ANGLE_360) TmpAngle -= INT_ANGLE_360;
			TmpAmount = 0;
			AckCheckHit(ae,ae->xPlayer,ae->yPlayer,TmpAngle,&TmpAmount);
			if (TmpAmount == 0)
			{
				xl1 = ae->xlPlayer + (CosTable[Angle] * NewAmount);	//	keep all bits
				yl1 = ae->ylPlayer + (SinTable[Angle] * NewAmount);	//	for accuracy
			}


	if (ObjectAngle != INT_ANGLE_90 && ObjectAngle != INT_ANGLE_270)
		XObject = xPlyRay(ae,ObjIndex,xObject,yObject,ObjectAngle);
	
	if (ObjectAngle != 0 && ObjectAngle != INT_ANGLE_180)
		YObject = yPlyRay(ae,ObjIndex,xObject,yObject,ObjectAngle);

	if (XObject < 0 && YObject < 0) break;

	if (ObjectAngle < INT_ANGLE_90 || ObjectAngle > INT_ANGLE_270)
		xd = (iLastX & 0xFFC0) - xObject;
	else
		xd = ((iLastX - GRID_WIDTH) & 0xFFC0) - xObject;

	/* Use the delta X to determine the distance to the wall */
	xDistance = xd * InvCosTable[ObjectAngle];
     
	if (ObjectAngle < INT_ANGLE_180)
		yd = (iLastY & 0xFFC0) - yObject;
	else
		yd = ((iLastY - GRID_HEIGHT) & 0xFFC0) - yObject;

	/* Use the delta Y to determine the distance to the wall */
	yDistance = yd * InvSinTable[ObjectAngle];
    
	if (xDistance < yDistance)
		Object = XObject;
	else
		Object = YObject;


		union
		{   struct
			{	int		xlow;
		    	int		xPlayer;	/* X value from 0 to 4095 */
		    };
		    long	xlPlayer;
		};
		union
		{   struct
			{	int		ylow;
		    	int		yPlayer;	/* Y value from 0 to 4095 */
		    };
		    long	ylPlayer;
		};
		int		PlayerPosn;		//	Where the Player is located
	    int		PlayerAngle;	/* Angle value from 0 to 1919 */


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
							if (POV_NOTHING != AckCheckObjHit(ae,ObjIndex,
															ae->ObjList[ObjIndex].x,
															ae->ObjList[ObjIndex].y,
															NewAngle,
															tmp,
															&Distance))
							{
								WeaponSound(ae->ObjList[ObjIndex].Weapon, 50);
								if (Weapons[ae->ObjList[ObjIndex].Weapon].Range >= Distance)
								{
										work =	(long)Weapons[ae->ObjList[ObjIndex].Weapon].Damage *
												(long)(Weapons[ae->ObjList[ObjIndex].Weapon].Range - Distance) /
												(long)Weapons[ae->ObjList[ObjIndex].Weapon].Range;
										if (ae->ObjList[ObjIndex].Enemy == PLAYER_OBJECT)
											CheckHealth(ObjIndex, (int)work);
								}
								ae->ObjList[ObjIndex].FireDly = Weapons[ae->ObjList[ObjIndex].Weapon].Delay;
							}
						}



if (Angle < INT_ANGLE_45)	NewAngle = INT_ANGLE_90;
else
if (Angle < INT_ANGLE_90)	NewAngle = 0;
else
if (Angle < INT_ANGLE_135)	NewAngle = INT_ANGLE_180;
else
if (Angle < INT_ANGLE_180)	NewAngle = INT_ANGLE_90;
else
if (Angle < INT_ANGLE_225)	NewAngle = INT_ANGLE_270;
else
if (Angle < INT_ANGLE_270)	NewAngle = INT_ANGLE_180;
else
if (Angle < INT_ANGLE_315)	NewAngle = 0;
else
if (Angle < INT_ANGLE_360)	NewAngle = INT_ANGLE_270;




else
if (HitResult == POV_DOORX_REG || HitResult == POV_DOORY_REG)
{	//	must be POV_DOORX_REG or POV_DOORY_REG
	oPtr->xl = xl1;
	oPtr->yl = yl1;
	oPtr->mPos = (y1 & 0xFFC0) + (x1 >> 6);
  return(HitResult);
}



void	draw_col2(int Col,int slice,int dist,
			int width,int ht,
			UCHAR far *Wall,UCHAR far *Screen,UCHAR far *PalTable,
			int LightFlag,int CenterRowOffset);
void	draw_col3(int Col,int slice,int dist,
			UCHAR far *Wall,UCHAR far *Screen,UCHAR far *PalTable,
			UCHAR far *BmScreen,int start,int len,int width,
			int LightFlag,int LightAdj);


/*
  for ( ; Column < ColEnd ; Column++)
	{
		if (distance < Walls[Column].Distance)
		{
			draw_col3(Column,
				xpHigh,
				(int)distance,
				oMaps[ObjNum],
				ScreenBuffer,
				PalTable,
				pBmScrn[distance],
				BS_Start[distance],
				BS_Length[distance],
				BYTES_PER_ROW,
				(int)ae->LightFlag,
				(int)ae->LightMap[ae->ObjList[ObjIndex].mPos]);
		}
		xp += yp;   //	Advance the next column to display (scaling)
	}
*/


/*
for (i = ae->WinStartX ; i <= ae->WinEndX ; i++)
{
	distance = Walls[i].Distance;
	draw_col3(i,
		Walls[i].Column,
		distance,
		bMaps[Walls[i].Number],
		ScreenBuffer,
		PalTable,
		pBmScrn[distance],
		BS_Start[distance],
		BS_Length[distance],
		BYTES_PER_ROW,
		(int)ae->LightFlag,
		Walls[i].LightAdj);
}
*/



for (i = ViewX; i <= ViewX1; i++)
  Walls[i].LightAdj = 0;



#ifndef	_DEBUG
inline UINT xRay(UINT *xGrid,int x,int y,int angle);
inline UINT yRay(UINT *yGrid,int x,int y,int angle);
#else
UINT xRay(UINT *xGrid,int x,int y,int angle);
UINT yRay(UINT *yGrid,int x,int y,int angle);
#endif




offset = BytesPerRow * (ViewHeight - 1) + Column;

i = j = 0;
row = 100;
do
{
	if (i >= xTiles && j >= yTiles) break;
	if (i < xTiles)
	{
		Compare = LongInvTanTable[row] * 29;
		Distance = xFloors[i].Distance;
		if (Compare >= Distance)
		{
			ScreenBuffer[offset] = 0;
			i++;
		}
	}
	if (j < yTiles)
	{
		Compare = LongInvTanTable[row] * 29;
		Distance = yFloors[j].Distance;
		if (Compare >= Distance)
		{
			ScreenBuffer[offset] = 0;
			j++;
		}
	}
	row -= (1 + ViewLowRes);
	offset -= BytesPerRow;
}
while (row > 0);



		if (xFloors[i].Distance > yFloors[j].Distance)
			Floors[Tiles++] = xFloors[i++];
		else
		if (yFloors[i].Distance > xFloors[j].Distance)
			Floors[Tiles++] = yFloors[j++];
		else
		{
			if (xTiles < yTiles)
			{
				Floors[Tiles++] = xFloors[i++];
				j++;
			}
			else
			{
				Floors[Tiles++] = yFloors[j++];
				i++;
			}
		}



void	FloorSlice(int xPlayer,int yPlayer,int ViewAngle,int Column);
	FloorSlice(xPlayer,yPlayer,ViewAngle,i);
/****************************************************************************
* Draw a floor slice for the present column based upon data returned				*
* by the ray cast routines in the AckView routine														*
* Viewer angle from center of screen to bottom row is 18.75 degrees or 100	*
* screen units in high resolution mode (50 units in low resolution mode)		*
****************************************************************************/
inline	void	FloorSlice(int xPlayer,int yPlayer,int ViewAngle,int Column)
{
int			i, j;
UINT		index;
long		xd, yd;
union
{	struct
	{	int		DistanceLow;
		int		DistanceHigh;
	};
	long	Distance;
};

for (i=0 ; i < xTiles ; i++)
{
	xd = xFloors[i].xPos - xPlayer;
	Distance = xd * InvCosTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE) break;

	Distance = DistanceHigh * ViewCosTable[Column];
	index = BS_Floor[DistanceHigh];
	if (index > (UINT)ViewHeight) continue;

	index = index * BytesPerRow + Column;
	ScreenBuffer[index] = 244;
}

for (j=0 ; j < yTiles; j++)
{
	yd = yFloors[j].yPos - yPlayer;
	Distance = yd * InvSinTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE) break;

	Distance = DistanceHigh * ViewCosTable[Column];
	index = BS_Floor[DistanceHigh];
	if (index > (UINT)ViewHeight) continue;

	index = index * BytesPerRow + Column;
	ScreenBuffer[index] = 244;
}

}


;==============================================================================
; long_sqrt(...)
; Created by John Lundy
;==============================================================================

LSop		equ	[bp+ABASE]

LSlow		equ	[bp-4]

PBEGIN	?long_sqrt@@ZAJJ@Z

	push		bp
	mov		bp,sp
	sub		sp,4

	mov		ebx,LSop
	cmp		ebx,1					;	LSop <= 1
	jbe		$100
  
  mov		eax,ebx				;	low = LSop
  xor		ebx,ebx				;	result = 0
  mov		edx,ebx				; high = 0
	mov		cx,16					; times to loop

$10:
	add		ebx,ebx				;	result += result
	shld	edx,eax,2			; the BIG left shift
	mov		LSlow,eax			; save low
	mov		eax,ebx				;	tmp = result + result + 1
	add		eax,eax
	inc		eax
	cmp		edx,eax				; high >= tmp
	jb		$20
	inc		ebx						;	result++
	sub		edx,eax				;	high -= tmp
$20:
	mov		eax,LSlow
	loop	$10

	mov		eax,ebx
	imul	ebx						; result * result
	mov		edx,LSop
	sub		edx,eax				; LSop - (result * result)
	mov		eax,ebx				; result - 1
	dec		eax
	cmp		edx,eax				; LSop - (result * result) >= (result - 1)
	jb		$100
	inc		ebx

$100:
	mov		eax,ebx
	pushd	ebx
	pop		ax
	pop		dx

	mov		sp,bp
	pop		bp
	ret

?long_sqrt@@ZAJJ@Z	endp





/****************************************************************************
* Draw a floor slice for the present column based upon data returned				*
* by the ray cast routines in the AckView routine														*
* Viewer angle from center of screen to bottom row is 18.75 degrees or 100	*
* screen units in high resolution mode (50 units in low resolution mode)		*
****************************************************************************/
void	FloorSlice(ACKENG *ae,int xPlayer,int yPlayer,int ViewAngle,int Column)
{
UCHAR		color;
int			i, j;
UINT		offset, start;
long		xd, yd;
union
{	struct
	{	int		DistanceLow;
		int		DistanceHigh;
	};
	long	Distance;
};

for (i=0 ; i < xTiles ; i++)
{
	xd = xFloors[i].xPos - xPlayer;
	Distance = xd * InvCosTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		xTiles = i;
		break;	//	Everything else is farther away
	}
	Distance = DistanceHigh * ViewCosTable[Column];
	xFloors[i].Distance = DistanceHigh;
	xFloors[i].Offset = BS_Floor[DistanceHigh];
}

for (j=0 ; j < yTiles; j++)
{
	yd = yFloors[j].yPos - yPlayer;
	Distance = yd * InvSinTable[ViewAngle];
	if (DistanceHigh >= MAX_DISTANCE)
	{
		yTiles = j;
		break;	//	Everything else is farther away
	}
	Distance = DistanceHigh * ViewCosTable[Column];
	yFloors[j].Distance = DistanceHigh;
	yFloors[j].Offset = BS_Floor[DistanceHigh];
}

i = j = Tiles = 0;
while (i < xTiles || j < yTiles)
{
	if (i < xTiles)
	{
		if (j < yTiles)
		{
			if (xFloors[i].Distance < yFloors[j].Distance)
				Floors[Tiles++] = xFloors[i++];
			else
			if (yFloors[j].Distance < xFloors[i].Distance)
				Floors[Tiles++] = yFloors[j++];
			else
			{	//	They are equal, which do I choose?
				i++; j++;	//	Neither
			}
		}
		else
			Floors[Tiles++] = xFloors[i++];
	}
	else
	if (j < yTiles)
		Floors[Tiles++] = yFloors[j++];
}

if (Tiles)
{
	start = Floors[0].Offset;
	if (start >= (UINT)ViewHeight) start = ViewHeight - 1;
	start = (start * BytesPerRow) + Column;
	color = Floors[0].Number & 0x41;
	color = (color == 0x41) || (color == 0) ? 247 : 240;
	color -= ae->LightMap[Floors[0].Number];
}

for (i=1 ; i < Tiles ; i++)
{
	offset = Floors[i].Offset;
	if (offset >= (UINT)ViewHeight) offset = ViewHeight - 1;
	offset = (offset * BytesPerRow) + Column;

	for ( ; start > offset ; start -= BytesPerRow)
		ScreenBuffer[start] = color;
	color = Floors[i].Number & 0x41;
	color = (color == 0x41) || (color == 0) ? 247 : 240;
	color -= ae->LightMap[Floors[i].Number];
}
}




			if (xFloors[i].Distance < yFloors[j].Distance)
				Floors[Tiles++] = xFloors[i++];
			else
			if (yFloors[j].Distance < xFloors[i].Distance)
				Floors[Tiles++] = yFloors[j++];
			else
			{	//	They are equal, which do I choose?
				i++; j++;	//	Neither
			}



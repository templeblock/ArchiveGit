/* Map Editor for Animation Construction Kit 3D */
/* Author: Lary Myers */
/* Modified: John Lundy */

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <direct.h>
#include <memory.h>
//#include <alloc.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"
#include "mapedit.h"

void				SetPalette2(char *Palette);
void				smWriteString(int x,int y,char *s);
unsigned char far	*Readiff(char *picname);
void				mouse_set_cursor(int mx, int m);

    unsigned	char far    *BkBuffer;
    unsigned	char far    *smFont;
		int	    ErrorCode;
		int	    GridX;
		int	    GridY;
		int	    LastObjCode;
		int	    LastWallCode;
		int	    CurrentType;
		int	    CurrentCode;
		int	    ModifiedFlag;

    unsigned	int	    MapGrid[GRID_MAX+1];
    unsigned	int	    ObjGrid[GRID_MAX+1];

    unsigned	char	far *WallBitmaps[256];
    unsigned	char	far *ObjBitmaps[256];

    unsigned	char	ObjbmNum[256];
    unsigned	char	Palette[768];

		char	    GridFile[64];
		char	    PalFile[64];

typedef struct
{
	int				x;
	int				y;
	int				MapType;
	unsigned int	MapCode;
	char			*Text;
} SELECTBOX;

SELECTBOX	Squares[] = {
		40,6,-1,0,"Choose one of the following",
		60,20,TYPE_WALL,0					,"Normal square",
		60,30,TYPE_WALL,DOOR_TYPE_SLIDE		,"Sliding door ",
		60,40,TYPE_WALL,WALL_SECRET_DOOR	,"Secret door  ",
		60,50,TYPE_WALL,DOOR_LOCKED			,"Locked door  ",
		60,60,TYPE_WALL,DOOR_LOCKED_RED		,"Locked RED   ",
		60,70,TYPE_WALL,DOOR_LOCKED_GREEN	,"Locked GREEN ",
		60,80,TYPE_WALL,DOOR_LOCKED_BLUE    ,"Locked BLUE  ",
		60,90,TYPE_WALL,MAP_STARTCODE		,"Start square ",
		60,100,TYPE_WALL,MAP_UPCODE			,"Up square    ",
		60,110,TYPE_WALL,MAP_DOWNCODE		,"Down square  ",
		60,120,TYPE_WALL,MAP_GOALCODE		,"Goal square  ",
		70,130,-2,0							,"Cancel",
		-1,-1,-1,0,0
		};

SELECTBOX	Directions[] = {
		40,6,-1,0,"Choose one of the following",
		60,20,0,INT_ANGLE_270			,"North        ",
		60,30,0,INT_ANGLE_315			,"North East   ",
		60,40,0,0						,"East         ",
		60,50,0,INT_ANGLE_45			,"South East   ",
		60,60,0,INT_ANGLE_90			,"South        ",
		60,70,0,INT_ANGLE_135			,"South West   ",
		60,80,0,INT_ANGLE_180			,"West         ",
		60,90,0,INT_ANGLE_225			,"North West   ",
		60,100,-2,0						,"Cancel",
		-1,-1,-1,0,0
		};

SELECTBOX	Shadings[] = {
		40,6,-1,0	,"Choose additional shading",
		60,20,0,1	,"Very Very Light",
		60,30,0,2	,"Very Light     ",
		60,40,0,3	,"Light          ",
		60,50,0,4	,"Light Medium   ",
		60,60,0,5	,"Medium         ",
		60,70,0,6	,"Heavy Medium   ",
		60,80,0,7	,"Heavy          ",
		60,90,0,8	,"Very Heavy     ",
		60,100,-2,0	,"No Additional  ",
		-1,-1,-1,0,0
		};

SELECTBOX	FillBox[] = {
		30,20,-1,0,"Fill will place the current",
		30,30,-1,0,"bitmap in all locations of",
		30,40,-1,0,"         the map.",
		30,50,-1,0," Do you wish to do this? ",
		60,60,-2,1," Yes ",
		110,60,-2,2," No ",
		-1,-1,-1,0,0
		};

SELECTBOX	ClsBox[] = {
		40,20,-1,0,"Clear will erase the entire",
		40,30,-1,0,"           map.",
		40,40,-1,0,"  Do you wish to do this?",
		60,50,-2,1," Yes ",
		110,50,-2,2," No ",
		-1,-1,-1,0,0
		};

SELECTBOX	SaveBox[] = {
		40,20,-1,0,"Map has been modified.",
		40,30,-1,0,"Do you wish to save it?",
		60,50,-2,1," Yes ",
		110,50,-2,2," No ",
		-1,-1,-1,0,0
		};

SELECTBOX	MapSavedBox[] = {
		55,50,-1,0,"Map has been saved.",
		80,70,-2,0," Okay ",
		-1,-1,-1,0,0
		};

SELECTBOX	OverwriteBox[] = {
		50,30,-1,0,"       File exists.",
		40,40,-1,0,"Do you wish to overwrite it?",
		60,60,-2,1," Yes ",
		110,60,-2,2," No ",
		-1,-1,-1,0,0
		};

SELECTBOX	LoadBox[] = {
		40,20,-1,0,"Load map will reload the",
		40,30,-1,0,"original map and erase",
		40,40,-1,0,"     any changes.",
		40,50,-1,0,"Do you wish to do this?",
		60,70,-2,1," Yes ",
		110,70,-2,2," No ",
		-1,-1,-1,0,0
		};

RECT	HotSpots[] = {
		2,2,	    178,143,	/* 0 - Main grid */

		183,3,	    196,14,		/* 1 - Map Up arrow */
		186,134,    196,145,	/* 2 - Map Dn arrow */
		165,150,    181,160,	/* 3 - Map Rt arrow */
		3,150,	    19,160,		/* 4 - Map Lt arrow */

		241,0,	    251,11,		/* 5 - Box Up arrow */
		241,56,	    251,67,		/* 6 - Box Dn arrow */

		209,22,	    231,40,		/* 7 - Show multiple boxes */

		252,70,	    283,94,		/* 8 - Wall button */
		287,70,	    318,94,		/* 9 - Object button */
		252,97,	    283,121,	/* 10 - Fill */
		287,97,	    318,121,	/* 11 - Clear */
		252,124,    283,148,	/* 12 - Save */
		287,124,    318,148,	/* 13 - Load */
		287,151,    318,175,	/* 14 - Exit */

		217,70,	    248,94,		/* 15 - Horizontal door */
		217,97,	    248,121,	/* 16 - Vertical door */

		-1,-1,	    -1,-1		/* End of table */
		};

void delay(int ms)
{
long	far *clock=MK_FP(0x40,0x6c);
long	last;
int		wait=ms/55;
	if (wait <= 0) wait = 1;
	do
	{
		last = *clock;
		while(last == *clock);
	}
	while(wait--);
}
/*****************/
/*  Begin Sound  */
/*****************/
void bgnsd(long snd)
{
snd = 1193280 / snd;
outp(0x43,0xb6);
outp(0x42,(char) (snd & 0xff));
outp(0x42,(char) ((snd >> 8) & 0xff));
outp(0x61,inp(0x61) | 3);
return;
}

/***************/
/*  End Sound  */
/***************/
void endsd(void)
{
outp(0x61,inp(0x61) & 0xfc);
return;
}

/****************************************************************************
****************************************************************************/
void SoundBeep(void)
{
//bgnsd(440);
delay(50);
endsd();
delay(200);
}


/****************************************************************************
****************************************************************************/
void mouse_released(void)
{
    int	    x,y,button;

button = 1;
while (button)
mouse_read_cursor(&button,&y,&x);
}

/****************************************************************************
****************************************************************************/
int GetAction(int mx,int my)
{
int		i = 0;

while (1)
{
    if (HotSpots[i].x < 0)
		return(-1);

    if (mx >= HotSpots[i].x &&
		mx <= HotSpots[i].x1 &&
		my >= HotSpots[i].y &&
		my <= HotSpots[i].y1)
		break;

    i++;
}

return(i);
}

/****************************************************************************
****************************************************************************/
int LoadAndShowScreen(void)
{
unsigned	char far *Video;

BkBuffer = Readiff("medit.lbm");

if (BkBuffer == NULL)
    return(-1);

Video = MK_FP(0xA000,0);

mouse_hide_cursor();
memmove(Video,&BkBuffer[4],(size_t)64000);
mouse_show_cursor();

return(0);
}

/****************************************************************************
****************************************************************************/
int LoadSmallFont(void)
{
int	    ht,wt,len;

smFont = Readiff("smfont.lbm");
if (smFont == NULL)
    return(-1);

ht = (*(int *)smFont);
wt = (*(int *)&smFont[2]);
len = ht * wt;
memmove(smFont,&smFont[4],len);

return(0);
}

/****************************************************************************
****************************************************************************/
void ClearMapArea(void)
{
int	    row;
unsigned char far *Video;

Video = MK_FP(0xA000,(2 * 320) + 2);

mouse_hide_cursor();
for (row = 2; row < 146; row++)
{
    memset(Video,YELLOW,180);
    Video += 320;
}
mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
void DrawBox(int x,int y,int x1,int y1,char color)
{
int	    offset,wt;
unsigned char far *Video;

offset = (y * 320) + x;
Video = MK_FP(0xA000,offset);
wt = x1 - x;
memset(Video,color,wt+1);
Video += 320;

while (++y < y1)
{
    *Video = color;
    Video[wt] = color;
    Video += 320;
}

memset(Video,color,wt+1);
}

/****************************************************************************
****************************************************************************/
void ShowBitmap(int x,int y,unsigned char far *bmp)
{
int      row;
unsigned char far *Video;

Video = MK_FP(0xA000,(y * 320) + x);
mouse_hide_cursor();
for (row = 0; row < 64; row++)
{
    memmove(Video,bmp,64);
    bmp += 64;
    Video += 320;
}
mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
void ShowSelectBox(SELECTBOX *sb)
{
int	    x1,y1;

mouse_hide_cursor();
ClearMapArea();

while (sb->x != -1)
{
    smWriteString(sb->x,sb->y,sb->Text);

    if (sb->MapType == -2)		    /* A button */
	{
		x1 = sb->x + (strlen(sb->Text) * 4);
		y1 = sb->y + 5;
		DrawBox(sb->x - 2,sb->y - 2,x1+1,y1+1,BLACK);
	}
    sb++;
}
mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
char DetermineMapColor(int offset)
{
UCHAR	color;
unsigned int Mcode,Tcode;

color = LIGHTGRAY;
Mcode = MapGrid[offset];
Tcode = Mcode & 0xFF00;
if (!(Mcode & 0xFF))
{
	if ((Tcode < MAP_STARTCODE) && Tcode)
		color = DARKGRAY;
}
if (!(Mcode & 0xFF))
{
    Mcode = ObjGrid[offset];
    if (Mcode & 0xFF)
    {
    	if (Tcode)
			color = BLUE;
		else
			color = LIGHTBLUE;
	}
}
else
{
    if ((Mcode & 0xFF) >= DOOR_XCODE)
		color = RED;

    if (Mcode & (DOOR_LOCKED | DOOR_LOCKED_MASK))
		color = LIGHTRED;

    if (Mcode & WALL_SECRET_DOOR)
		color = GREEN;
}

return(color);
}

/****************************************************************************
****************************************************************************/
void ShowGrid(void)
{
int	    row,col,offset;
int	    x,y;
char    buf[8];
char    blank[3];
char    color;
unsigned int  Mcode;

offset = (GridY * GRID_WIDTH) + GridX;
*blank = ' ';
blank[1] = ' ';
blank[2] = '\0';
y = 2;
mouse_hide_cursor();
for (row = 0; row < 18; row++)
{
    x = 2;
    for (col = 0; col < 18; col++)
	{
		DrawBox(x,y,x+10,y+8,BLACK);
		color = DetermineMapColor(offset);
		Mcode = MapGrid[offset];
		if ((Mcode & 0xFF00) >= MAP_STARTCODE)
		{
	    	sprintf(buf,"%02X",(Mcode & 0xFF00) >> 8);
	    	smWriteString(x+2,y+2,buf);
		}
		else
		if (!(Mcode & 0xFF))
		{
	    	Mcode = ObjGrid[offset];
			if (Mcode & 0xFF)
			{
		    	sprintf(buf,"%02X",Mcode & 0xFF);
		    	smWriteString(x+2,y+2,buf);
			}
			else
		    	smWriteString(x+2,y+2,blank);
		}
		else
		{
	    	sprintf(buf,"%02X",Mcode & 0xFF);
	    	smWriteString(x+2,y+2,buf);
		}

		DrawBox(x+1,y+1,x+9,y+7,color);
		x += 10;
		offset++;
	}
    y += 8;
    offset += GRID_WIDTH - 18;
}

mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
void smWriteChar(int x,int y,unsigned char ch)
{
int	 FontOffset,VidOffset;
int	 row,col;
unsigned	char far *Video;

VidOffset = (y * 320) + x;
Video = MK_FP(0xA000,VidOffset);

row = ch / 32;
col = ch - (row * 32);

FontOffset = ((row * 1344) + 192) + ((col * 6) + 1);

for (row = 0; row < 5; row++)
{
    Video[0] = smFont[FontOffset];
    Video[1] = smFont[FontOffset + 1];
    Video[2] = smFont[FontOffset + 2];
    Video[3] = smFont[FontOffset + 3];
    Video += 320;
    FontOffset += 192;
}
}

/****************************************************************************
****************************************************************************/
void smWriteString(int x,int y,char *s)
{
char    ch;

mouse_hide_cursor();
while (*s)
{
    ch = toupper(*s++);
    smWriteChar(x,y,ch);
    x += 4;
    if (ch == 'M' || ch == 'N' || ch == 'W')
		x++;
}
mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
void ShowCoords(int x,int y)
{
int	    row,col;
char    buf[12];

if (x < 2 || y < 2 || x > 182 || y > 146)
    return;

x -= 2;
y -= 2;

row = (y / 8) + GridY;
col = (x / 10) + GridX;

sprintf(buf,"%02d,%02d",col,row);
smWriteString(108,154,buf);
}

/****************************************************************************
****************************************************************************/
void ShowGridCoords(void)
{
char    buf[10];

sprintf(buf,"%02d,%02d",GridX,GridY);
smWriteString(82,154,buf);
}

/****************************************************************************
****************************************************************************/
int FindSelectHit(SELECTBOX *sb,int mx,int my)
{
int	    index,x1,y1;

index = 0;

while (sb->x != -1)
{
    if (sb->MapType != -1)
	{
		x1 = sb->x + (strlen(sb->Text) * 4);
		y1 = sb->y + 5;
		if (mx >= sb->x && my >= sb->y && mx <= x1 && my <= y1)
	    	return(index);
	}
    sb++;
    index++;
}

return(-1);
}

/****************************************************************************
****************************************************************************/
UINT GetDirection(int x,int y)
{
int	    GridOffset;
int	    mx,my,mbutton;
int	    index,done;
UINT    Dir=0;

x -= 2;
y -= 2;
GridOffset = (((y / 8) + GridY) * GRID_WIDTH) + (x / 10) + GridX;

ShowSelectBox(Directions);
done = 0;
mouse_released();

while (!done)
{
    mouse_read_cursor(&mbutton,&my,&mx);
    if (mbutton & 1)
	{
		index = FindSelectHit(Directions,mx,my);
		if (index != -1)
	    {
	    	if (Directions[index].MapType == -2)	/* Cancel */
			{
				done = 1;
				break;
			}
	    	Dir = Directions[index].MapCode >> 3;
	    	done = 1;
	    }
	}
}

mouse_released();
ClearMapArea();
ShowGrid();
return(Dir);
}

/****************************************************************************
****************************************************************************/
UINT GetShading(int x,int y)
{
int	    GridOffset;
int	    mx,my,mbutton;
int	    index,done;
UINT    Shade=0;

x -= 2;
y -= 2;
GridOffset = (((y / 8) + GridY) * GRID_WIDTH) + (x / 10) + GridX;

ShowSelectBox(Shadings);
done = 0;
mouse_released();

while (!done)
{
    mouse_read_cursor(&mbutton,&my,&mx);
    if (mbutton & 1)
	{
		index = FindSelectHit(Directions,mx,my);
		if (index != -1)
	    {
	    	if (Shadings[index].MapType == -2)	/* Cancel */
			{
				done = 1;
				break;
			}
	    	Shade = Shadings[index].MapCode;
	    	done = 1;
	    }
	}
}

return(Shade);
}

/****************************************************************************
****************************************************************************/
void GetSpecialCode(int x,int y)
{
int	    GridOffset;
int	    mx,my,mbutton;
int	    index,done;
UINT    MapCode;

x -= 2;
y -= 2;
GridOffset = (((y / 8) + GridY) * GRID_WIDTH) + (x / 10) + GridX;

MapCode = MapGrid[GridOffset];
if ((MapCode < MAP_STARTCODE) && !(MapCode & 0xFF))
{
	MapGrid[GridOffset] = GetShading(x,y) << 8;
	mouse_released();
	ClearMapArea();
	ShowGrid();
	return;
}

ShowSelectBox(Squares);
done = 0;
mouse_released();

while (!done)
{
    mouse_read_cursor(&mbutton,&my,&mx);
    if (mbutton & 1)
	{
		index = FindSelectHit(Squares,mx,my);
		if (index != -1)
	    {
	    	if (Squares[index].MapType == -2)	/* Cancel */
			{
				done = 1;
				break;
			}
	    	MapCode = Squares[index].MapCode;
	    	if (Squares[index].MapType == TYPE_WALL)
			{
				if (!MapCode)
		    		MapGrid[GridOffset] &= 0x00FF;
		    	if (MapCode == MAP_STARTCODE)
		    		MapGrid[GridOffset] = MAP_STARTCODE + GetDirection(x,y);
		    	else
				if (MapCode & 0xFF00)
		    		MapGrid[GridOffset] |= MapCode;
				else
		    	if (MapCode & 0xFF)
				{
					MapGrid[GridOffset] &= 0xFF00;
					MapGrid[GridOffset] |= MapCode;
				}
			}
	    	done = 1;
	    }
	}
}

mouse_released();
ClearMapArea();
ShowGrid();
}

/****************************************************************************
****************************************************************************/
int GeneralSelectBox(SELECTBOX *sb,int DoBeep)
{
    int	    done,index;
    int	    result,mx,my,mbutton;

ShowSelectBox(sb);

if (DoBeep) SoundBeep();

result = done = 0;

while (!done)
{
    mouse_read_cursor(&mbutton,&my,&mx);
    if (mbutton & 1)
	{
		index = FindSelectHit(sb,mx,my);
		if (index != -1)
	    {
	    	sb += index;
	    	result = sb->MapCode;
	    	done = 1;
	    }
	}
}

mouse_released();
ClearMapArea();
ShowGrid();

return(result);
}

/****************************************************************************
****************************************************************************/
void PutCode(int x,int y,UINT Mcode)
{
    int	    row,col;
    int	    GridOffset;
    char    color;
    char    buf[4];

x -= 2;
y -= 2;

GridOffset = (((y / 8) + GridY) * GRID_WIDTH) + (x / 10) + GridX;

if (CurrentType == TYPE_WALL)
{
    MapGrid[GridOffset] &= 0xFF00;
    MapGrid[GridOffset] |= Mcode;
}
else
if (CurrentType == TYPE_OBJECT)
{
    ObjGrid[GridOffset] = GetDirection(x,y) << 8;
    ObjGrid[GridOffset] |= Mcode;
}

if (!Mcode)
{
    MapGrid[GridOffset] = 0;
    ObjGrid[GridOffset] = 0;
}

color = DetermineMapColor(GridOffset);

row = ((y / 8) * 8);
col = ((x / 10) * 10);

if (Mcode & 0xFF)
    sprintf(buf,"%02X",Mcode & 0xFF);
else
    strcpy(buf,"  ");

col += 2;
row += 2;
mouse_hide_cursor();
smWriteString(col + 2,row + 2,buf);
DrawBox(col+1,row+1,col+9,row+7,color);
mouse_show_cursor();
ModifiedFlag = 1;
}

/****************************************************************************
****************************************************************************/
void ShowCurrentCode(void)
{
    int	    row;
    UCHAR   color;
    char    buf[4];
    unsigned char far *Video,*bmp;

sprintf(buf,"%02X",CurrentCode & 0xFF);
mouse_hide_cursor();
DrawBox(241,27,251,35,BLACK);
smWriteString(243,29,buf);
if (CurrentType == TYPE_WALL)
{
    color = LIGHTGRAY;
    bmp = WallBitmaps[CurrentCode & 0xFF];
}
else
{
    color = LIGHTBLUE;
    bmp = ObjBitmaps[ObjbmNum[CurrentCode & 0xFF]];
}

DrawBox(242,28,250,34,color);

if (bmp != NULL)
    ShowBitmap(254,2,bmp);
else
{
    Video = MK_FP(0xA000,(2 * 320) + 254);
    for (row = 0; row < 64; row++)
	{
		memset(Video,LIGHTGRAY,64);
		Video += 320;
	}
    SoundBeep();
}

mouse_show_cursor();
}

/****************************************************************************
****************************************************************************/
void ShowButton(int x,int y,char *Text)
{

smWriteString(x,y,Text);
DrawBox(x-2,y-2,x + (strlen(Text) * 4) + 1,y + 6,BLACK);

}

/****************************************************************************
****************************************************************************/
void ShowNextGroup(int StartCode)
{
int					i,x,y;
unsigned int		cCode;
unsigned char far	*bmp,*Video;
char				buf[10];

x = 0;
cCode = StartCode;

mouse_hide_cursor();
for (i = 0; i < 5; i++)
{
    if (CurrentType == TYPE_WALL)
		bmp = WallBitmaps[cCode];
    else
		bmp = ObjBitmaps[ObjbmNum[cCode]];

    if (bmp != NULL)
		ShowBitmap(x,0,bmp);
    else
	{
		Video = MK_FP(0xA000,x);
		for (y = 0; y < 64; y++)
	    {
	    	memset(Video,LIGHTGRAY,64);
	    	Video += 320;
	    }
		DrawBox(x,0,x+63,63,BLACK);
	}

    sprintf(buf,"%02X",cCode);
    ShowButton(x+30,70,buf);

    if (CurrentType == TYPE_WALL)
	{
		smWriteString(x,80,"                ");

		if (cCode == DOOR_XCODE)
	    	smWriteString(x,80,"Vertical door");
		if (cCode == DOOR_YCODE)
	    	smWriteString(x,80,"Horizontal door");
		if (cCode == DOOR_SIDECODE)
	    	smWriteString(x,80,"Door side");
	}

    cCode++;
    if (cCode > 0xFF)	//	(CurrentType == TYPE_OBJECT && cCode >= MAX_OBJECTS)
		cCode = 0;

    x += 64;
}
mouse_show_cursor();

}


/****************************************************************************
****************************************************************************/
void ShowBitmapGroup(void)
{
int					mbutton,mx,my;
int					StartCode, cCode;
unsigned char far	*Video;

Video = MK_FP(0xA000,0);

mouse_released();
mouse_hide_cursor();
memmove(BkBuffer,Video,(size_t)64000);
memset(Video,YELLOW,(size_t)64000);

StartCode = CurrentCode;
ShowNextGroup(StartCode);

ShowButton(6,100,"<- Previous");
ShowButton(150,100,"Cancel");
ShowButton(280,100,"Next ->");

mouse_show_cursor();

while (1)
{
  mouse_read_cursor(&mbutton,&my,&mx);
  if (mbutton & 1)
	{
		if (mx > 5 && mx < 50 && my > 99 && my < 109)
	  {
	  	StartCode -= 1;
	   	if (CurrentType == TYPE_WALL)
				StartCode &= 0xFF;
	    else
			{
				if (StartCode < 0)
		   		StartCode = MAX_OBJECTS - 1;
			}
	    ShowNextGroup(StartCode);
	    delay(200);
	    continue;
	  }

		if (mx > 149 && mx < 174 && my > 99 && my < 109)
	   	break;

		if (mx > 279 && mx < 309 && my > 99 && my < 109)
	  {
	   	StartCode += 1;
	   	if (CurrentType == TYPE_WALL)
				StartCode &= 0xFF;
	   	else
			{
				if (StartCode >= MAX_OBJECTS)
		   		StartCode = 0;
			}
	   	ShowNextGroup(StartCode);
	   	delay(200);
	   	continue;
	  }

		if (my < 64)
    {
	   	mx /= 64;
	   	cCode = StartCode + mx;
	   	if (CurrentType == TYPE_WALL)
	   	{
				cCode &= 0xFF;
				if (WallBitmaps[cCode])	//	must be a bitmap there
				{
					CurrentCode = cCode;
					break;
				}
			}
	   	else
			{
				if (cCode >= MAX_OBJECTS)
					cCode -= MAX_OBJECTS;
		 		if (ObjBitmaps[ObjbmNum[cCode]])
		 		{
	  			CurrentCode = cCode;
	  			break;
				}
			}
	  }
	}
}

mouse_hide_cursor();
memmove(Video,BkBuffer,(size_t)64000);
ShowCurrentCode();
mouse_show_cursor();
mouse_released();

}

/****************************************************************************
****************************************************************************/
char *GetExtent(char *s)
{
char		*e;

e = strrchr(s,'.');
if (e == NULL)
    return(s);
e++;

return(e);
}

/****************************************************************************
****************************************************************************/
int LoadBitmap(int BitmapNumber,char *BitmapName,int BitmapType)
{
int				handle;
unsigned char	*bmp;

if (!(stricmp(GetExtent(BitmapName),"LBM")))
{
    bmp = Readiff(BitmapName);
    if (bmp == NULL)
	{
		ErrorCode = ERR_NOMEMORY;
		return(-1);
	}

    if (BitmapType == TYPE_WALL)
		WallBitmaps[BitmapNumber] = bmp;

    if (BitmapType == TYPE_OBJECT)
		ObjBitmaps[BitmapNumber] = bmp;

    memmove(bmp,&bmp[4],4096);
    return(0);
}

bmp = malloc(4096);
if (bmp == NULL)
{
    ErrorCode = ERR_NOMEMORY;
    return(-1);
}

if (BitmapType == TYPE_WALL)
    WallBitmaps[BitmapNumber] = bmp;

if (BitmapType == TYPE_OBJECT)
    ObjBitmaps[BitmapNumber] = bmp;


handle = open(BitmapName,O_RDWR|O_BINARY);
if (handle < 1)
{
    free(bmp);
    ErrorCode = ERR_BADFILE;
    return(-1);
}

read(handle,bmp,4);	    /* Skip width and height for now */
read(handle,bmp,4096);
close(handle);

return(0);
}

/****************************************************************************
****************************************************************************/
char *StripEndOfLine(char *s)
{
int		len;
char	ch;

len = strlen(s);

while (--len >= 0)
{
    ch = s[len];
    if (ch != ' ' && ch != ';' && ch != '\t' && ch != 13 && ch != 10)
		break;

    s[len] = '\0';
}

return(s);
}

/****************************************************************************
****************************************************************************/
char *SkipSpaces(char *s)
{

while (*s == ' ' || *s == '\t' || *s == ',')
    strcpy(s,&s[1]);

return(s);
}

/****************************************************************************
****************************************************************************/
char *AddExtent(char *s,char *ext)
{
if (strchr(s,'.') == NULL)
    strcat(s,ext);

return(s);
}

/****************************************************************************
****************************************************************************/
char *CopyToComma(char *dest,char *src)
{
char		ch;

while (*src)
{
    ch = *src++;
    if (ch == ' ' || ch == '\t' || ch == ',')
		break;

    *dest++ = ch;
}

*dest = '\0';

return(src);
}



/****************************************************************************
****************************************************************************/
int LoadDescFile(char *fName)
{
FILE	*fp;
int		Mode,fMode,result;
int		bType,value,bNum,ObjIndex;
char	LineBuf[128];
char	fBuf[128];
char	*s;

fp = fopen(fName,"rt");
if (fp == NULL)
{
    printf("Unable to open description file: %s\n",fName);
    return(-1);
}

ObjIndex = 0;
Mode = 0;
result = 0;
*GridFile = '\0';

while (1)
{
    if (feof(fp))
		break;

    *LineBuf = '\0';
    fgets(LineBuf,127,fp);

    if (*LineBuf == ';')
		continue;

    StripEndOfLine(LineBuf);
    SkipSpaces(LineBuf);

    if (!strlen(LineBuf))
		continue;

    if (!stricmp(LineBuf,"WALLS:"))
	{
		bType = TYPE_WALL;
		Mode = 1;
		continue;
	}

    if (!stricmp(LineBuf,"ENDWALLS:"))
	{
		if (Mode != 1)
	    {
	    	printf("Invalid place for command: %s.\n",LineBuf);
	    	result = -1;
	    }

		Mode = 0;
		continue;
	}

    if (!stricmp(LineBuf,"OBJECTS:"))
	{
		bType = TYPE_OBJECT;
		Mode = 2;
		continue;
	}

    if (!stricmp(LineBuf,"FILES:"))
	{
		fMode = 1;
		continue;
	}

    if (!stricmp(LineBuf,"ENDFILES:"))
	{
		fMode = 0;
		continue;
	}

    if (!strnicmp(LineBuf,"PALFILE:",8))
	{
		strcpy(PalFile,SkipSpaces(&LineBuf[8]));
		continue;
	}

    if (!strnicmp(LineBuf,"MAPFILE:",8))
	{
		strcpy(GridFile,SkipSpaces(&LineBuf[8]));
		continue;
	}

    if (Mode == 2)
	{
		if (!strnicmp(LineBuf,"NUMBER:",7))
	    {
	    	value = atoi(&LineBuf[7]);

	    	if (value < 1 || value >= 255)
			{
				printf("Invalid object number:\n%s\n",LineBuf);
				result = -1;
				break;
			}
	    	ObjIndex = value;
	    	continue;
	    }

		if (!strnicmp(LineBuf,"BITMAPS:",8))
	    {
	    	strcpy(LineBuf,SkipSpaces(&LineBuf[8]));
	    	value = 0;
	    	strcpy(LineBuf,CopyToComma(fBuf,LineBuf));
	    	SkipSpaces(fBuf);
	    	bNum = atoi(fBuf);

	    	if (bNum < 1 || bNum > 255)
			{
				printf("Invalid bitmap number for object: %d\n",ObjIndex);
				result = -1;
				break;
			}

	    	ObjbmNum[ObjIndex] = bNum;
	    	continue;
	    }
	}

    if (fMode)
	{
		value = atoi(LineBuf);
		if (value < 1 || value > 255)
	    {
	    	printf("Invalid number for object: %s.\n",LineBuf);
	    	result = -1;
	    	continue;
	    }

		s = strpbrk(LineBuf,", \t");
		if (s == NULL)
	    {
	    	printf("Unable to locate bitmap name for object: %s.\n",LineBuf);
	    	result = -1;
	    	continue;
	    }

		strcpy(fBuf,SkipSpaces(s));
		AddExtent(fBuf,".img");

		if (LoadBitmap(value,fBuf,bType))
	    {
	    	printf("Error loading bitmap \"%s\".\n",fBuf);
	    	delay(500);
	    	result = -1;
	    }
		continue;
	}

}

fclose(fp);
return(result);
}

/****************************************************************************
****************************************************************************/
int LoadGrid(void)
{
int		handle;

handle = open(GridFile,O_RDWR|O_BINARY);
if (handle < 1)
{
    printf("Unable to open MapFile: %s\n",GridFile);
    return(-1);
}

if (read(handle,MapGrid,8192) != 8192)
{
    close(handle);
    printf("Error reading MapFile: %s\n",GridFile);
    return(-1);
}

if (read(handle,ObjGrid,8192) != 8192)
{
    close(handle);
    printf("Error reading MapFile: %s\n",GridFile);
    return(-1);
}

close(handle);
return(0);
}


/****************************************************************************
****************************************************************************/
int SaveGrid(void)
{
int		handle;

handle = open(GridFile,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
if (handle < 1)
{
    return(-1);
}

if (write(handle,MapGrid,8192) != 8192)
{
    close(handle);
    return(-2);
}

if (write(handle,ObjGrid,8192) != 8192)
{
    close(handle);
    return(-2);
}

close(handle);
ModifiedFlag = 0;
GeneralSelectBox(MapSavedBox,0);
return(0);
}

/****************************************************************************
****************************************************************************/
int LoadPalette(char *pName)
{
int		handle;

handle = open(pName,O_RDWR|O_BINARY);
if (handle < 1)
{
    printf("Unable to open PalFile: %s\n",pName);
    ErrorCode = ERR_BADPALFILE;
    return(-1);
}

if (read(handle,Palette,768) != 768)
{
    close(handle);
    printf("Error reading PalFile: %s\n",pName);
    ErrorCode = ERR_BADPALFILE;
    return(-1);
}

close(handle);
return(0);
}


/****************************************************************************
****************************************************************************/
int main(int argc,char *argv[])
{
    int	    done,mx,my,mbutton,lastx,lasty,index,SaveType;
    int	    i;
    unsigned int key;
    struct  _find_t ffblk;
    char    *s;

if (mouse_installed() != -1)
    {
    puts("Mouse required");
    exit(1);
    }

if (LoadSmallFont())
    {
    puts("Unable to load font file");
    exit(1);
    }

if (argc < 2)
    {
    printf("ACK-3D Map Editor Version 2.0\n");
    printf("Usage:\n");
    printf("   medit ascfile.ext\n");
    printf("   where:\n");
    printf("         ascfile.ext is the name of the ASCII file that contains\n");
    printf("                     wall and object bitmap filenames.\n");
    exit(1);
    }

if (LoadDescFile(argv[1]))
    {
    printf("\nError reading ASCII file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

if (LoadGrid())
    {
    printf("\nError loading map file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

if (LoadPalette(PalFile))
    {
    printf("\nError loading palette file - ErrorCode = %d\n",ErrorCode);
    exit(1);
    }

_asm {
    mov ax,19
    int 10h
    };

LoadAndShowScreen();

SetPalette2(&Palette[0]);	//16 * 3]);

GridX = GridY = 0;
CurrentType = TYPE_WALL;
LastWallCode = LastObjCode = CurrentCode = 1;

mouse_show_cursor();
mouse_read_cursor(&mbutton,&my,&mx);
lastx = mx;
lasty = my;

ShowCoords(lastx,lasty);

ShowGrid();
ShowCurrentCode();

smWriteString(4,194,"Press spacebar to select square options");
s = GetExtent(GridFile);
smWriteString(82,166,&s[1]);

smWriteString(82,178,argv[1]);
done = 0;

while (!done)
{
    key = inkey();
    if (key == 0x11B)
		break;

    mouse_read_cursor(&mbutton,&my,&mx);

    if (lastx != mx || lasty != my)
	{
		lastx = mx;
		lasty = my;
		ShowCoords(lastx,lasty);
	}

    if (key == 0x3920)
	{
		if (mx > 1 && mx < 183 && my > 1 && my < 147)
	    {
	    	GetSpecialCode(mx,my);
	    	mouse_set_cursor(my,mx);
	    }
		else
	    	SoundBeep();
	}

    if (mbutton & 2)
    {
    	SaveType = CurrentType;
    	CurrentType = -1;	//	Clear cell
    	PutCode(mx,my,0);
    	CurrentType = SaveType;
    }
    if (mbutton & 1)
	{
	index = GetAction(mx,my);
	switch (index)
	{
	    case 0:
		PutCode(mx,my,CurrentCode);
		if (CurrentType == TYPE_OBJECT)
		    mouse_released();

		if (CurrentCode >= DOOR_XCODE)
		{
		    GetSpecialCode(mx,my);
		    mouse_set_cursor(my,mx);
		}
		break;

	    case 1:		/* Map Up arrow */
		if (GridY)
		{
		    GridY--;
		    ShowGrid();
		    ShowGridCoords();
		    ShowCoords(mx,my);
		}
		break;

	    case 2:		/* Map Dn arrow */
		if (GridY < (GRID_HEIGHT - 18))
		{
		    GridY++;
		    ShowGrid();
		    ShowGridCoords();
		    ShowCoords(mx,my);
		}
		break;


	    case 3:		/* Map Rt arrow */
		if (GridX < (GRID_WIDTH - 18))
		{
		    GridX++;
		    ShowGrid();
		    ShowGridCoords();
		    ShowCoords(mx,my);
		}
		break;

	    case 4:		/* Map Lt arrow */
		if (GridX)
		{
		    GridX--;
		    ShowGrid();
		    ShowGridCoords();
		    ShowCoords(mx,my);
		}
		break;

	    case 5:		/* Bitmap Up arrow */
		CurrentCode -= 1;
		if (CurrentType == TYPE_WALL)
		{
		    CurrentCode &= 0xFF;
		    LastWallCode = CurrentCode;
		}
		else
		{
		    if (CurrentCode < 0)
			CurrentCode = MAX_OBJECTS - 1;
		    LastObjCode = CurrentCode;
		}

		ShowCurrentCode();
		delay(200);
		break;

	    case 6:		/* Bitmap Dn arrow */
		CurrentCode += 1;
		if (CurrentType == TYPE_WALL)
		{
		    CurrentCode &= 0xFF;
		    LastWallCode = CurrentCode;
		}
		else
		{
		    if (CurrentCode >= MAX_OBJECTS)
			CurrentCode = 0;
		    LastObjCode = CurrentCode;
		}
		ShowCurrentCode();
		delay(200);
		break;

	    case 7:		/* Show Bitmap Group */
		ShowBitmapGroup();
		if (CurrentType == TYPE_WALL)
		    LastWallCode = CurrentCode;
		else
		    LastObjCode = CurrentCode;

		break;

	    case 8:		/* Wall button */
		CurrentType = TYPE_WALL;
		CurrentCode = LastWallCode;
		ShowCurrentCode();
		mouse_released();
		break;

	    case 9:		/* Object button */
		CurrentType = TYPE_OBJECT;
		CurrentCode = LastObjCode;
		ShowCurrentCode();
		mouse_released();
		break;

	    case 10:		/* Fill */
		if (GeneralSelectBox(FillBox,1) == 2)
		    break;

		for (i = 0; i < GRID_MAX; i++)
		{
		    MapGrid[i] = CurrentCode;
		    ObjGrid[i] = 0;
		}

		ModifiedFlag = 1;
		GridX = GridY = 0;
		ShowGrid();
		ShowGridCoords();
		ShowCoords(mx,my);
		break;

	    case 11:		/* Clear */
		if (GeneralSelectBox(ClsBox,1) == 2)
		    break;

		for (i = 0; i < GRID_MAX; i++)
		{
		    MapGrid[i] = 0;
		    ObjGrid[i] = 0;
		}

		ModifiedFlag = 1;
		GridX = GridY = 0;
		ShowGrid();
		ShowGridCoords();
		ShowCoords(mx,my);
		break;

	    case 12:		/* Save */
		if (!_dos_findfirst(GridFile,0,&ffblk))
		{
		    if (GeneralSelectBox(OverwriteBox,1) == 2)
			break;
		}

		SaveGrid();
		break;

	    case 13:		/* Load */
		if (ModifiedFlag)
		{
		    if (GeneralSelectBox(LoadBox,1) == 2)
				break;
		}

		if (!LoadGrid())
		{
		    ModifiedFlag = 0;
		    GridX = GridY = 0;
		    CurrentType = TYPE_WALL;
		    CurrentCode = 1;
		    ShowGrid();
		    ShowCurrentCode();
		}
		else
		    SoundBeep();
		break;

	    case 14:		/* Exit */
		done = 1;
		break;

	    case 15:		/* Horiz door */
		CurrentType = TYPE_WALL;
		CurrentCode = DOOR_YCODE;
		ShowCurrentCode();
		mouse_released();
		break;

	    case 16:		/* Vert Door */
		CurrentType = TYPE_WALL;
		CurrentCode = DOOR_XCODE;
		ShowCurrentCode();
		mouse_released();
		break;

	    default:
		break;
	    }
	}
}


if (ModifiedFlag)
{
    if (GeneralSelectBox(SaveBox,1) == 1)
		SaveGrid();
}


_asm
{
    mov ax,3
    int 10h
};

return(0);
}

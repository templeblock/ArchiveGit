/*
// Revision History:
//
// $Log: /PM8/App/SOUNDFX.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     3/21/98 1:18p Fredf
// New menus and toolbar names.
// 
//    Rev 1.0   14 Aug 1997 15:25:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.8   03 Jun 1997 11:49:58   johno
// Clean up, added sound for ID_FILE_OPEN, ID_VIEW_GLOBALBAR
// 
// 
//    Rev 1.7   29 May 1997 18:00:10   johno
// More sounds
// 
// 
//    Rev 1.6   29 May 1997 15:26:56   johno
// Control of interruption of currently playing sound
// 
// 
//    Rev 1.5   28 May 1997 18:09:26   johno
// Took out bad ASSERT
// 
//    Rev 1.4   28 May 1997 15:48:32   johno
// Fixed bug selecting default sound
// (now optional)
// 
// 
//    Rev 1.3   27 May 1997 17:06:36   johno
// Plays default sound if specific sound not found
// 
// 
//    Rev 1.2   27 May 1997 13:10:56   johno
// Now derived from CMciWnd
// 
// 
//    Rev 1.1   23 May 1997 16:50:48   johno
// Ctor allows playing of sounds
// 
// 
//    Rev 1.0   23 May 1997 16:03:02   johno
// Initial revision.
// 
*/

#include "stdafx.h"
#include "pmw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int      CSoundFX::SoundIndex = 0;

LPCSTR   CSoundFX::FileName [] =
{
   "soundfx\\general.wav",    //0   This "slot" reserved for default sound
   "soundfx\\print.wav",      //1
   "soundfx\\printp.wav",     //2
   "soundfx\\cancel.wav",     //3
   "soundfx\\exit.wav",       //4
   "soundfx\\done.wav",       //5
   "soundfx\\save.wav",       //6
   "soundfx\\next.wav",       //7
   "soundfx\\ready.wav",      //8
   "soundfx\\help.wav",       //9
   "soundfx\\brand.wav",      //10
   "soundfx\\art.wav",        //11
   "soundfx\\sent.wav",       //12
   "soundfx\\address.wav",    //13
   "soundfx\\internet.wav",   //14
   "soundfx\\search.wav",     //15
   "soundfx\\your.wav"        //16
};

struct SoundFXElement CSoundFX::Sound [] =
{
   // Control ID                 Window ID               Sound File
   // ID_HUB_WINDOW
   IDC_READY_MADE,               ID_HUB_WINDOW,          8,
   IDC_WITH_HELP,                ID_HUB_WINDOW,          9,
   ID_FILE_NEW,                  ID_HUB_WINDOW,          10,
   IDC_YOUR_PROJECTS,            ID_HUB_WINDOW,          16,
   IDC_ART_GALLERY,              ID_HUB_WINDOW,          11,
   IDC_INTERNET,                 ID_HUB_WINDOW,          14,
   IDC_ADDRESS,                  ID_HUB_WINDOW,          13,
   IDC_HUB_EXIT,                 ID_HUB_WINDOW,          4,

   // ID_VIEW_GLOBALBAR
   ID_FILE_OPEN,                 ID_VIEW_FILEBAR,        8,
   IDC_WITH_HELP,                ID_VIEW_FILEBAR,        9,
   ID_FILE_NEW,                  ID_VIEW_FILEBAR,        10,
   ID_FILE_SAVE,                 ID_VIEW_FILEBAR,        6,
   ID_FILE_PRINT,                ID_VIEW_FILEBAR,        1,
   ID_FILE_PRINT_PREVIEW,        ID_VIEW_FILEBAR,        2,

   // ID_VIEW_TEXTBAR

   // ID_VIEW_DRAWBAR
   ID_PICTURES_IMPORT,           ID_VIEW_EDIT1BAR,       11,

   // ID_VIEW_PLACEBAR

   // ID_VIEW_MODIFYBAR

   IDOK,                         IDD_GENERIC_DIALOG_ID,  7,
   IDCANCEL,                     IDD_GENERIC_DIALOG_ID,  3,
   ID_HELP,                      IDD_GENERIC_DIALOG_ID,  9
};

/////////////////////////////////////////////////////////////////////////////
// CSoundFX

CSoundFX::CSoundFX()
{
}

CSoundFX::~CSoundFX()
{
}


BEGIN_MESSAGE_MAP(CSoundFX, CMciWnd)
	//{{AFX_MSG_MAP(CSoundFX)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void
CSoundFX::PlaySoundFX (UINT IDButton, UINT IDWindow, BOOL UseDefault /*= TRUE*/)
{
   LPCSTR   fn;
   int      i;

   for (i = 0; i < NUM_SOUNDFX; i++)
   {
      if (SoundIndex >= NUM_SOUNDFX)
         SoundIndex = 0;

      if ((Sound [SoundIndex].WindowID == IDWindow) && (Sound [SoundIndex].ControlID == IDButton))
         break;
      
      SoundIndex ++;
   } 

   if (i >= NUM_SOUNDFX)   // Didn't find it, use default sound (maybe)
   {
      if (UseDefault != TRUE)
         return;

      fn = FileName [0];
   }
   else
      fn = FileName [Sound [SoundIndex].FileNameIndex];

   CString csName = GetGlobalPathManager()->ExpandPath((char *)fn);
   play_sound ((LPSTR)(LPCSTR)csName, 0, NULL, FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSoundFX message handlers

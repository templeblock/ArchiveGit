// SoundFX.h : header file
//

struct SoundFXElement
{
   UINT     ControlID;
   UINT     WindowID;
   int      FileNameIndex;  
};

#define  NUM_SOUNDFX    18
#define  NUM_SOUNDFILES 17

#include "mciwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundFX window

class CSoundFX : public CMciWnd
{
// Construction
public:
	CSoundFX();

   void
   PlaySoundFX (UINT IDButton, UINT IDWindow, BOOL UseDefault = TRUE);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundFX)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSoundFX();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSoundFX)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   static int                    SoundIndex;    
   static struct SoundFXElement  Sound [NUM_SOUNDFX];
   static LPCSTR                 FileName [NUM_SOUNDFILES];  
};

/////////////////////////////////////////////////////////////////////////////

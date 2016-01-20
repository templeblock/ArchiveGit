// Source file: storctrl.cpp
long WINPROC EXPORT StoryControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam);

#define GWL_STORY	0
#define STORY_EXTRA (GWL_STORY+sizeof(long))
#define STORY_CLASS "story"

typedef struct
{
	RECT	rArea;
	DWORD	dwFrom;
	DWORD	dwTo;
} WORD_DATA;

typedef WORD_DATA FAR *LPWORD_DATA;

class FAR CStory
{
public:
	CStory(HWND hWindow);
	~CStory();

	BOOL Play();
	void StopPlaying();
	void Reset();
	int FindHotSpot(int x, int y);
	void EnableCursor(BOOL fEnable);

public:
	HWND		m_hWnd;
	int			m_nRects;
	HMCI		m_hMCIfile;
	int			m_iHighlight;
	PDIB		m_pDib;
	LPWORD_DATA	m_lpWordData;
	BOOL		m_fPlaying;
	FNAME		m_szStoryFile;
	FNAME		m_szDibFile;
	HCURSOR		m_hHotSpotCursor;
	BOOL		m_fCursorEnabled;
	BOOL		m_fMappedToPalette;
};
typedef CStory FAR *PSTORY;

PSTORY GetStory(HWND hWindow);
void SetStory(HWND hWindow, PSTORY pStory);

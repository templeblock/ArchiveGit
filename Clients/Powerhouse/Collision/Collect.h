#ifndef _COLLECTIBLE_H
#define _COLLECTIBLE_H

#define MAX_COLLECTIBLES 12

typedef struct collect
{
	ITEMID	id;
	BOOL    bTimedLife;			// Does this have a timed life?
	short   iWarningCount;		// Number of times player is warned 
								//   before collectable expires
	long	lLifeTime;			// Amount of time it lives
	long    lStartTime;			// Time that collectable came to life
} COLLECTITEM, * LPCOLLECTITEM;


class CCollectibles
{
// CONSTRUCTORS
  public:
    CCollectibles(void);

// DESTRUCTOR
  public:
    ~CCollectibles(void);

// ATTRIBUTES
  public:
	COLLECTITEM Collectible[MAX_COLLECTIBLES];
	
// FUNCTIONS
  public:
	void Init(void);
	void ReInit(HWND hWnd);
	void AddItem(HWND hWnd, int iLevel, int iCollectible);
	void Add(HWND hWnd, ITEMID idCollectible, BOOL bFromBottom = FALSE);
	void RemoveItem(HWND hWnd, int iLevel, int iCollectible);
	void Remove(HWND hWnd, ITEMID idCollectible);
	void SaveAll(void);
	ITEMID GetID(int index); 
	void SetUnCollected(short iCollNum);

	// Sets timed life for a collectable (milliseconds)
	void SetTimedLife(short iCollNum,  short iWarningCount, long lTime);
	void AdjustTimedLife(HWND hWnd);
};

#endif

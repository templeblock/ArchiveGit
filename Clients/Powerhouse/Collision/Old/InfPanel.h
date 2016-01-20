typedef struct colltype
{
	LPSTR	szColName;
	short	iBitmapID;
	BOOL	bCollected;	
} COLLTYPE, FAR * LPCOLLTYPE;


class far CCollInfo;
typedef class CCollInfo FAR * LPCOLLINFO;

class far CCollInfo
{
// CONSTRUCTORS
  public:
    CCollInfo(void);

// DESTRUCTOR
  public:
    ~CCollInfo(void);

// ATTRIBUTES
  public:
	COLLTYPE ctColl[16];
	
// FUNCTIONS
  public:
	void SetCollected(short iCollNum);
	void SetUnCollected(short iCollNum);
	BOOL IsCollected(short iCollNum); 
	void SetBitmapIDs(short iLevel);
  private:    
};
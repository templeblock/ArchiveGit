#ifndef _PARSER_H_
#define _PARSER_H_

// 2K buffers for processing
// no line parsed can be longer than 2K
#define BUFFER_SIZE	2048L

// for parsing jump table information
class FAR CParser
{
public:
	// constructors/destructor
	CParser(LPSTR lpTableData);
	~CParser() {}

public:
	// get number of entries in table
	int GetNumEntries() { return m_nEntries; }
	// set number of entries in table
	void SetNumEntries(int nEntries) { m_nEntries = nEntries; }
	// actually do the parsing of the table, HandleKey() will be called
	// for each key=value pair in the table
	BOOL ParseTable(DWORD dwUserData);
	// create a shot id for a string
	SHOTID MakeShotID( LPSTR lpString, BOOL bDupCheck );


	// InitTable() called before any calls to HandleKey()
	virtual BOOL InitTable(DWORD dwUserData);
	// !!!
	// you must override this function to provide handling for entries in table
	// !!!
	virtual BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValue, int nValues, int nIndex, DWORD dwUserData) = NULL;
protected:
	// get the next value, call nValues times for multiple values (see HandleKey)
	LPSTR GetNextValue(LPSTR FAR *lppData) {return (GetKey(lppData)); }
	// get entry in table
	BOOL GetEntryString( HPTR FAR * lppData, LPSTR lpReturn, int iLength );
	// get key from key=value pair
	LPSTR GetKey( LPSTR FAR * lppData );
	// get value from key=value pair
	LPSTR GetValues( LPSTR FAR * lppData, LPINT lpNumValues );
	// get a rectangle value
	BOOL GetRect(LPRECT lpRect, LPSTR lpEntry, LPSTR lpValues, int nValues);
	// get a point value
	BOOL GetPoint(LPPOINT lpPoint, LPSTR lpEntry, LPSTR lpValues, int nValues);
	// get a filename or string value
	BOOL GetFilename(LPSTR lpFilename, LPSTR lpEntry, LPSTR lpValues, int nValues);
	// get string value that includes spaces and validate
	BOOL GetString(LPSTR lpString, LPSTR lpEntry, LPSTR lpValues, int nValues);
	// get int value
	BOOL GetInt(LPINT lpIntValue, LPSTR lpEntry, LPSTR lpValues, int nValues);
	// get shot id from key or value and validate
	SHOTID GetShotID(LPSTR lpEntry, LPSTR lpValue);
	// check to make sure shot id is valid and has been found
	SHOTID ShotCheck( SHOTID lShot );
	// count the number of entries in the table
	int CountNumEntries();

protected:
	// pointer to table data
	HPTR	m_lpTableData;
	// entries in the table
	int		m_nEntries;
};

typedef CParser FAR * LPPARSER;



#endif // _PARSER_H_//
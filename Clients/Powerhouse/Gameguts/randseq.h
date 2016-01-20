#ifndef _RANDSEQ_H_
#define _RANDSEQ_H_

class FAR CRandomSequence
{
public:
	CRandomSequence(DWORD dwElements = 0, DWORD dwSequenceSeed = 0);
	~CRandomSequence() {};

	void Init(	DWORD dwElements, // number of elements desired in sequence (0 -> n-1)
				DWORD dwSequenceSeed = 0); // sequence seed must be from (0 -> n-1)

	// get next number in sequence
	// must call Constructor or Init() with dwElements != 0 before calling this function
	DWORD GetNextNumber();

private:
	int BitWidth(DWORD dwElements);

private:
	DWORD m_dwElements;
	DWORD m_dwMask;
	DWORD m_dwSequenceSeed;
	DWORD m_dwSequence;
};


#endif // _RANDSEQ_H_
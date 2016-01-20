#include <windows.h>
#include "randseq.h"

DWORD randmask[] = {
	0x03,			// 2-bits
	0x03,			// 2-bits
	0x03,			// 2-bits
	0x06,			// 3-bits
	0x0C,			// 4-bits
	0x14,			// 5-bits
	0x30,			// 6-bits
	0x60,			// 7-bits
	0xB8,			// 8-bits
	0x0110,			// 9-bits
	0x0240,			// 10-bits
	0x0500,			// 11-bits
	0x0CA0,			// 12-bits
	0x1B00,			// 13-bits
	0x3500,			// 14-bits
	0x6000,			// 15-bits
	0xB400,			// 16-bits
	0x00012000,		// 17-bits
	0x00020400,		// 18-bits
	0x00072000,		// 19-bits
	0x00090000,		// 20-bits
	0x00140000,		// 21-bits
	0x00300000,		// 22-bits
	0x00400000,		// 23-bits
	0x00D80000,		// 24-bits
	0x01200000,		// 25-bits
	0x03880000,		// 26-bits
	0x07200000,		// 27-bits
	0x09000000,		// 28-bits
	0x14000000,		// 29-bits
	0x32800000,		// 30-bits
	0x48000000,		// 31-bits
	0xA3000000};	// 32-bitsÿ

CRandomSequence::CRandomSequence(DWORD dwElements, DWORD dwSequenceSeed)
{
	Init(dwElements, dwSequenceSeed);
}

void CRandomSequence::Init(DWORD dwElements, DWORD dwSequenceSeed)
{
	m_dwElements = dwElements;
	if (m_dwElements)
	{
		// get number of bits required for this size sequence
		int iBitWidth = BitWidth(m_dwElements);
		// lookup mask to be used for this number of bits
		m_dwMask = randmask[iBitWidth];
		// set seed for sequence
		dwSequenceSeed += 1;
		m_dwSequenceSeed = dwSequenceSeed;
		m_dwSequence = dwSequenceSeed;
	}
}

DWORD CRandomSequence::GetNextNumber()
{
	// get current sequence number
	DWORD dwSequence = m_dwSequence;
	// if not done, calculate a new sequence number
	if (dwSequence == 0xFFFFFFFFL)
		return(dwSequence);

	// calculate next sequence number
	do
	{
		// if low-bit set, shift right and xor with mask
		if (m_dwSequence & 1)
		{
			m_dwSequence >>= 1;
			m_dwSequence ^= m_dwMask;
		}
		// if low-bit not set, just shift right
		else
		{
			m_dwSequence >>= 1;
		}
		// if we have hit the initial seed value then we are done
		if (m_dwSequence == m_dwSequenceSeed)
		{
			m_dwSequence = 0xFFFFFFFFL;	 // flag that we are done
			break;
		}
	} while (m_dwSequence > m_dwElements); // only allow sequence numbers in range

	return(dwSequence-1);
}

int CRandomSequence::BitWidth(DWORD dwLastNum)
{
	int iWidth = 0;
	while (dwLastNum)
	{
		dwLastNum >>= 1;
		++iWidth;
	}
	return(iWidth);
}

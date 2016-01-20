#include <windows.h>
#include <mmsystem.h>
#include <mciavi.h>
#include <digitalv.h>

#define NOAVIFILE
//#define NOCOMPMAN
//#define NODRAWDIB
#define NOVIDEO
//#define NOAVIFMT
#define NOMMREG
#define NOMCIWND
#define NOAVICAP
#define NOMSACM
#include "vfw.h"

typedef unsigned short U16;
typedef unsigned char  U8;

#include "vfw_spec.h"

//***********************************************************************
BOOL SetTransparency( HIC m_hIC )
//***********************************************************************
{
R4_DEC_SEQ_DATA m_DecodeSeqMsg;	// Info for messages which may only be sent before initiating playback

if ( !m_hIC )
	if ((m_hIC = ICOpen(ICTYPE_VIDEO, mmioFOURCC('I','V','4','1'), ICMODE_DECOMPRESS)) == 0)
		return( FALSE );

// Get the current settings
m_DecodeSeqMsg.dwSize = sizeof( R4_DEC_SEQ_DATA );
m_DecodeSeqMsg.dwFourCC = mmioFOURCC('I','V','4','1');
m_DecodeSeqMsg.dwVersion = SPECIFIC_INTERFACE_VERSION;
m_DecodeSeqMsg.oeEnvironment = OE_32; // or = OE_32;
m_DecodeSeqMsg.dwFlags = DECSEQ_VALID|DECSEQ_KEY|DECSEQ_SCALABILITY|DECSEQ_FILL_TRANSPARENT;
m_DecodeSeqMsg.dwKey = 0;
m_DecodeSeqMsg.fEnabledKey = 0;
m_DecodeSeqMsg.fScalability = 0;
m_DecodeSeqMsg.fFillTransparentPixels = 0;
m_DecodeSeqMsg.mtType = MT_DECODE_SEQ_DEFAULT;
long err = ICSendMessage(m_hIC, ICM_GETCODECSTATE, (DWORD)(LPSTR)&m_DecodeSeqMsg, sizeof( R4_DEC_SEQ_DATA ));

// Set Transparency
m_DecodeSeqMsg.mtType = MT_DECODE_SEQ_VALUE;
m_DecodeSeqMsg.fFillTransparentPixels = TRUE;
m_DecodeSeqMsg.dwFlags = DECSEQ_VALID|DECSEQ_FILL_TRANSPARENT;
err = ICSendMessage(m_hIC, ICM_SETCODECSTATE, (DWORD)(LPSTR)&m_DecodeSeqMsg, sizeof( R4_DEC_SEQ_DATA )); 
return( err != 0 );
}

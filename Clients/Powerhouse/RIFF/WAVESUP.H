/** wavesup.h
 *
 *  DESCRIPTION:
 *
 *
 *  HISTORY:
 *
 ** cjp */

#ifndef _WAVESUP_H_
#define _WAVESUP_H_

#define WSUPAPI  FAR PASCAL

//
//  public function prototypes
//
DWORD WSUPAPI wsupBytesToSamples(LPWAVEFORMATEX pwfx, DWORD dwBytes);
DWORD WSUPAPI wsupSamplesToBytes(LPWAVEFORMATEX pwfx, DWORD dwSamples);
DWORD WSUPAPI wsupSamplesToTime(LPWAVEFORMATEX pwfx, DWORD dwSamples);
DWORD WSUPAPI wsupTimeToSamples(LPWAVEFORMATEX pwfx, DWORD dwTime);
void  WSUPAPI wsupGetFormatName(LPWAVEFORMATEX lpwfx, LPSTR lpszBuf);

#endif

/** EOF: wavesup.h **/

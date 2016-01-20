/** msadpcm.c
 *
     (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.

     You have a royalty-free right to use, modify, reproduce and 
     distribute the Sample Files (and/or any modified version) in 
     any way you find useful, provided that you agree that 
     Microsoft has no warranty obligations or liability for any 
     Sample Application Files which are modified. 
	 
     If you did not get this from Microsoft Sources, then it may not be the
     most current version.  This sample code in particular will be updated
     and include more documentation.  

     Sources are:
     	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
	CompuServe: WINSDK forum, MDK section.
	Anonymous FTP from ftp.uu.net vendors\microsoft\multimedia
*/


#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "mmreg.h"
#include "msadpcm.h"


//
//  constants used by the Microsoft 4 Bit ADPCM algorithm
//
//  CAUTION: the code contained in this file assumes that the number of
//  channels will be no greater than 2! this is for minor optimization
//  purposes and would be very easy to change if >2 channels is required.
//  it also assumes that the PCM data will either be 8 or 16 bit.
//
//  the code to look out for looks 'similar' to this:
//
//      PCM.BytesPerSample = (PCM.BitsPerSample >> 3) << (Channels >> 1);
//
#define MSADPCM_NUM_COEF        (7)
#define MSADPCM_MAX_CHANNELS    (2)

#define MSADPCM_CSCALE          (8)
#define MSADPCM_PSCALE          (8)
#define MSADPCM_CSCALE_NUM      (1 << MSADPCM_CSCALE)
#define MSADPCM_PSCALE_NUM      (1 << MSADPCM_PSCALE)

#define MSADPCM_DELTA4_MIN      (16)

#define MSADPCM_OUTPUT4_MAX     (7)
#define MSADPCM_OUTPUT4_MIN     (-8)


//
//  Fixed point Delta adaption table:
//
//      Next Delta = Delta * gaiP4[ this output ] / MSADPCM_PSCALE
//
static short  gaiP4[] = { 230, 230, 230, 230, 307, 409, 512, 614,
                          768, 614, 512, 409, 307, 230, 230, 230 };


/** DWORD FAR PASCAL adpcmDecode4Bit(LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpSrc, LPPCMWAVEFORMAT lpwfPCM, HPSTR hpDst, DWORD dwSrcLen)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpSrc, LPPCMWAVEFORMAT lpwfPCM, HPSTR hpDst, DWORD dwSrcLen)
 *
 *  RETURN (DWORD FAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

DWORD FAR PASCAL adpcmDecode4Bit(LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpSrc, LPPCMWAVEFORMAT lpwfPCM, HPSTR hpDst, DWORD dwSrcLen)
{
    short   iInput;
    short   iNextInput;
    short   iFirstNibble;
    short   iDelta;
    long    lSamp;
    long    lPrediction;
    BYTE    bPredictors;
    BYTE    bChannels;
    BYTE    bBitsPerSample;
    BYTE    m;
    WORD    n;
    WORD    wSamplesPerBlock;
    WORD    wBlockHeaderBytes;
    DWORD   dwTotalPos;
    DWORD   dwDecoded;
    short   aiSamp1[MSADPCM_MAX_CHANNELS];
    short   aiSamp2[MSADPCM_MAX_CHANNELS];
    short   aiCoef1[MSADPCM_MAX_CHANNELS];
    short   aiCoef2[MSADPCM_MAX_CHANNELS];
    short   aiDelta[MSADPCM_MAX_CHANNELS];
    LPADPCMCOEFSET  lpCoefSet;

    //
    //  put some commonly used info in more accessible variables and init
    //  the wBlockHeaderBytes, dwDecoded and dwTotalPos vars...
    //
    lpCoefSet           = &lpwfADPCM->aCoef[0];
    bPredictors         = (BYTE)lpwfADPCM->wNumCoef;
    bChannels           = (BYTE)lpwfADPCM->wfx.nChannels;
    bBitsPerSample      = (BYTE)lpwfPCM->wBitsPerSample;
    wSamplesPerBlock    = lpwfADPCM->wSamplesPerBlock;
    wBlockHeaderBytes   = bChannels * 7;
    dwDecoded           = 0L;
    dwTotalPos          = 0L;


    //
    //  step through each byte of ADPCM data and decode it to the requested
    //  PCM format (8 or 16 bit).
    //
    while (dwTotalPos < dwSrcLen)
    {
        //
        //  the first thing we need to do with each block of ADPCM data is
        //  to read the header which consists of 7 bytes of data per channel.
        //  so our first check is to make sure that we have _at least_
        //  enough input data for a complete ADPCM block header--if there
        //  is not enough data for the header, then exit.
        //
        //  the header looks like this:
        //      1 byte predictor per channel
        //      2 byte delta per channel
        //      2 byte first sample per channel
        //      2 byte second sample per channel
        //
        //  this gives us (7 * bChannels) bytes of header information. note
        //  that as long as there is _at least_ (7 * bChannels) of header
        //  info, we will grab the two samples from the header (and if no
        //  data exists following the header we will exit in the decode
        //  loop below).
        //
        dwTotalPos += wBlockHeaderBytes;
        if (dwTotalPos > dwSrcLen)
            goto adpcmDecode4BitExit;
            
        //
        //  grab and validate the predictor for each channel
        //
        for (m = 0; m < bChannels; m++)
        {
            BYTE    bPredictor;

            bPredictor = (BYTE)(*hpSrc++);
            if (bPredictor >=  bPredictors)
            {
                //
                //  the predictor is out of range--this is considered a
                //  fatal error with the ADPCM data, so we fail by returning
                //  zero bytes decoded
                //
                dwDecoded = 0;
                goto adpcmDecode4BitExit;
            }

            //
            //  get the coefficients for the predictor index
            //
            aiCoef1[m] = lpCoefSet[bPredictor].iCoef1;
            aiCoef2[m] = lpCoefSet[bPredictor].iCoef2;
        }
        
        //
        //  get the starting delta for each channel
        //
        for (m = 0; m < bChannels; m++)
        {
            aiDelta[m] = *(short huge *)hpSrc;
            hpSrc++;
            hpSrc++;
        }

        //
        //  get the sample just previous to the first encoded sample per
        //  channel
        //
        for (m = 0; m < bChannels; m++)
        {
            aiSamp1[m] = *(short huge *)hpSrc;
            hpSrc++;
            hpSrc++;
        }

        //
        //  get the sample previous to aiSamp1[x] per channel
        //
        for (m = 0; m < bChannels; m++)
        {
            aiSamp2[m] = *(short huge *)hpSrc;
            hpSrc++;
            hpSrc++;
        }


        //
        //  write out first 2 samples for each channel.
        //
        //  NOTE: the samples are written to the destination PCM buffer
        //  in the _reverse_ order that they are in the header block:
        //  remember that aiSamp2[x] is the _previous_ sample to aiSamp1[x].
        //
        if (bBitsPerSample == (BYTE)8)
        {
            for (m = 0; m < bChannels; m++)
            {
                *hpDst++ = (char)((aiSamp2[m] >> 8) + 128);
            }
            for (m = 0; m < bChannels; m++)
            {
                *hpDst++ = (char)((aiSamp1[m] >> 8) + 128);
            }
        }
        else
        {
            for (m = 0; m < bChannels; m++)
            {
                *(short huge *)hpDst = aiSamp2[m];
                hpDst++;
                hpDst++;
            }
            for (m = 0; m < bChannels; m++)
            {
                *(short huge *)hpDst = aiSamp1[m];
                hpDst++;
                hpDst++;
            }
        }

        //
        //  we have decoded the first two samples for this block, so add
        //  two to our decoded count
        //
        dwDecoded += 2;


        //
        //  we now need to decode the 'data' section of the ADPCM block.
        //  this consists of packed 4 bit nibbles.
        //
        //  NOTE: we start our count for the number of data bytes to decode
        //  at 2 because we have already decoded the first 2 samples in
        //  this block.
        //
        iFirstNibble = 1;
        for (n = 2; n < wSamplesPerBlock; n++)
        {
            for (m = 0; m < bChannels; m++)
            {
                if (iFirstNibble)
                {
                    //
                    //  we need to grab the next byte to decode--make sure
                    //  that there is a byte for us to grab before continue
                    //
                    dwTotalPos++;
                    if (dwTotalPos > dwSrcLen)
                        goto adpcmDecode4BitExit;

                    //
                    //  grab the next two nibbles and create sign extended
                    //  integers out of them:
                    //
                    //      iInput is the first nibble to decode
                    //      iNextInput will be the next nibble decoded
                    //
                    iNextInput  = (short)*hpSrc++;
                    iInput      = iNextInput >> 4;
                    iNextInput  = (iNextInput << 12) >> 12;

                    iFirstNibble = 0;
                }
                else
                {
                    //
                    //  put the next sign extended nibble into iInput and
                    //  decode it--also set iFirstNibble back to 1 so we
                    //  will read another byte from the source stream on
                    //  the next iteration...
                    //
                    iInput = iNextInput;
                    iFirstNibble = 1;
                }


                //
                //  compute the next Adaptive Scale Factor (ASF) and put
                //  this value in aiDelta for the current channel.
                //
                iDelta = aiDelta[m];
                aiDelta[m] = (short)((gaiP4[iInput & 15] * (long)iDelta) >> MSADPCM_PSCALE);
                if (aiDelta[m] < MSADPCM_DELTA4_MIN)
                    aiDelta[m] = MSADPCM_DELTA4_MIN;

                //
                //  decode iInput (the sign extended 4 bit nibble)--there are
                //  two steps to this:
                //
                //  1.  predict the next sample using the previous two
                //      samples and the predictor coefficients:
                //
                //      Prediction = (aiSamp1[channel] * aiCoef1[channel] + 
                //              aiSamp2[channel] * aiCoef2[channel]) / 256;
                //
                //  2.  reconstruct the original PCM sample using the encoded
                //      sample (iInput), the Adaptive Scale Factor (aiDelta)
                //      and the prediction value computed in step 1 above.
                //
                //      Sample = (iInput * aiDelta[channel]) + Prediction;
                //
                lPrediction = (((long)aiSamp1[m] * aiCoef1[m]) +
                                ((long)aiSamp2[m] * aiCoef2[m])) >> MSADPCM_CSCALE;
                lSamp = ((long)iInput * iDelta) + lPrediction;

                //
                //  now we need to clamp lSamp to [-32768..32767]--this value
                //  will then be a valid 16 bit sample.
                //
                if (lSamp > 32767)
                    lSamp = 32767;
                else if (lSamp < -32768)
                    lSamp = -32768;
        
                //
                //  lSamp contains the decoded iInput sample--now write it
                //  out to the destination buffer
                //
                if (bBitsPerSample == (BYTE)8)
                {
                    *hpDst++ = (char)(((short)lSamp >> 8) + 128);
                }
                else
                {
                    *(short huge *)hpDst = (short)lSamp;
                    hpDst++;
                    hpDst++;
                }
                
                //
                //  ripple our previous samples down making the new aiSamp1
                //  equal to the sample we just decoded
                //
                aiSamp2[m] = aiSamp1[m];
                aiSamp1[m] = (short)lSamp;
            }

            //
            //  we have decoded one more complete sample
            //
            dwDecoded++;
        }
    }

    //
    //  we're done decoding the input data. dwDecoded contains the number
    //  of complete _SAMPLES_ that were decoded. we need to return the
    //  number of _BYTES_ decoded. so calculate the number of bytes per
    //  sample and multiply that with dwDecoded...
    //
adpcmDecode4BitExit:

    return (dwDecoded * ((bBitsPerSample >> (BYTE)3) << (bChannels >> 1)));
} /* adpcmDecode4Bit() */



/** short NEAR PASCAL adpcmEncode4Bit_FirstDelta(short iCoef1, short iCoef2, short iP5, short iP4, short iP3, short iP2, short iP1)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (short iCoef1, short iCoef2, short iP5, short iP4, short iP3, short iP2, short iP1)
 *
 *  RETURN (short NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

short NEAR PASCAL adpcmEncode4Bit_FirstDelta(short iCoef1, short iCoef2, short iP5, short iP4, short iP3, short iP2, short iP1)
{
    long    lTotal;
    short   iRtn;
    long    lTemp;

    //
    //  use average of 3 predictions
    //
    lTemp  = (((long)iP5 * iCoef2) + ((long)iP4 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal = (lTemp > iP3) ? (lTemp - iP3) : (iP3 - lTemp);

    lTemp   = (((long)iP4 * iCoef2) + ((long)iP3 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal += (lTemp > iP2) ? (lTemp - iP2) : (iP2 - lTemp);

    lTemp   = (((long)iP3 * iCoef2) + ((long)iP2 * iCoef1)) >> MSADPCM_CSCALE;
    lTotal += (lTemp > iP1) ? (lTemp - iP1) : (iP1 - lTemp);
    
    //
    //  optimal iDelta is 1/4 of prediction error
    //
    iRtn = (short)(lTotal / 12);
    if (iRtn < MSADPCM_DELTA4_MIN)
        iRtn = MSADPCM_DELTA4_MIN;

    return (iRtn);
} /* adpcmEncode4Bit_FirstDelta() */


/** DWORD FAR PASCAL adpcmEncode4Bit(LPPCMWAVEFORMAT lpwfPCM, HPSTR hpSrc, LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpDst, DWORD dwSrcLen)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPPCMWAVEFORMAT lpwfPCM, HPSTR hpSrc, LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpDst, DWORD dwSrcLen)
 *
 *  RETURN (DWORD FAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

#define ENCODE_DELTA_LOOKAHEAD      5

DWORD FAR PASCAL adpcmEncode4Bit(LPPCMWAVEFORMAT lpwfPCM, HPSTR hpSrc, LPADPCMWAVEFORMAT lpwfADPCM, HPSTR hpDst, DWORD dwSrcLen)
{
    LPSTR   lpSamplesBuf;
    LPSTR   lpSamples;

    BYTE    abBestPredictor[MSADPCM_MAX_CHANNELS];
    DWORD   adwTotalError[MSADPCM_NUM_COEF][MSADPCM_MAX_CHANNELS];

    short   aiSamples[ENCODE_DELTA_LOOKAHEAD][MSADPCM_MAX_CHANNELS];
    short   aiFirstDelta[MSADPCM_NUM_COEF][MSADPCM_MAX_CHANNELS];
    short   aiDelta[MSADPCM_MAX_CHANNELS];
    short   aiSamp1[MSADPCM_MAX_CHANNELS];
    short   aiSamp2[MSADPCM_MAX_CHANNELS];

    short   iCoef1;
    short   iCoef2;
    short   iSamp1;
    short   iSamp2;
    short   iDelta;
    short   iSample;
    short   iOutput;

    long    lSamp;

    DWORD   dw;
    DWORD   dwTotalConverted;
    DWORD   dwInputPos;

    long    lError;
    long    lPrediction;

    WORD    wSamplesPerBlock;
    WORD    cbSample;
    WORD    wBlockHeaderBytes;
    WORD    wBlockSize;
    WORD    wNextWrite;
    WORD    wFirstNibble;
    WORD    n;
    BYTE    m;
    BYTE    i;
    BYTE    bChannels;
    BYTE    bBitsPerSample;

    //
    //  first copy some information into more accessible (cheaper and shorter)
    //  variables--and precompute some stuff...
    //
    wSamplesPerBlock    = lpwfADPCM->wSamplesPerBlock;
    bChannels           = (BYTE)lpwfPCM->wf.nChannels;
    bBitsPerSample      = (BYTE)lpwfPCM->wBitsPerSample;
    wBlockHeaderBytes   = bChannels * 7;
    dwInputPos          = 0L;
    dwTotalConverted    = 0L;

    //
    //  calculate the number of bytes per sample in the PCM data
    //
    //  note: the following code works because we _know_ that we only deal
    //  with 8 or 16 bits per sample PCM and 1 or 2 channels..
    //
    cbSample = (bBitsPerSample >> 3) << (bChannels >> 1);

    //
    //  we allocate and use a small buffer (<64k) to hold an entire block of
    //  samples to be encoded. the reason for this is mainly because we
    //  make 8 passes over the data per block and using huge pointers all
    //  of the time is expensive. so we put it in a <64k chunk and eliminate
    //  the huge pointer stuff
    //
    lpSamplesBuf = GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE, wSamplesPerBlock * cbSample);
    if (!lpSamplesBuf)
        return (0L);

    //
    //  step through each block of PCM data and encode it to 4 bit ADPCM
    //
    for ( ; dwInputPos < dwSrcLen; dwInputPos += (wBlockSize * cbSample))
    {
        //
        //  determine how much data we should encode for this block--this
        //  will be wSamplesPerBlock until we hit the last chunk of PCM
        //  data that will not fill a complete block. so on the last block
        //  we only encode that amount of data remaining...
        //
        dw = (dwSrcLen - dwInputPos) / cbSample;
        wBlockSize = (WORD)min((DWORD)wSamplesPerBlock, dw);

        if (!wBlockSize)
            break;

        //
        //  copy the samples that we will be encoding into our small data
        //  samples buffer (this is faster than doing huge pointer arith
        //  all of the time)
        //
        hmemcpy(lpSamplesBuf, &hpSrc[dwInputPos], wBlockSize * cbSample);

        //
        //  find the optimal predictor for each channel: to do this, we
        //  must step through and encode using each coefficient set (one
        //  at a time) and determine which one has the least error from
        //  the original data. the one with the least error is then used
        //  for the final encode (the 8th pass done below).
        //
        //  NOTE: keeping the encoded data of the one that has the least
        //  error at all times is an obvious optimization that should be
        //  done. in this way, we only need to do 7 passes instead of 8.
        //
        for (i = 0; i < MSADPCM_NUM_COEF; i++)
        {
            //
            //  reset pointer to beginning of our temporary small data
            //  samples buffer
            //
            lpSamples = lpSamplesBuf;

            //
            //  copy the coefficient pair for the current coefficient set
            //  we are using into more convenient/cheaper variables
            //
            iCoef1 = lpwfADPCM->aCoef[i].iCoef1;
            iCoef2 = lpwfADPCM->aCoef[i].iCoef2;

            //
            //  we need to choose the first iDelta--to do this, we need
            //  to look at the first few samples. for convenience, we copy
            //  the first ENCODE_DELTA_LOOKAHEAD samples (converted to 
            //  16 bit samples) into a temporary buffer
            //
            for (n = 0; n < ENCODE_DELTA_LOOKAHEAD; n++)
            {
                for (m = 0; m < bChannels; m++)
                {
                    if (bBitsPerSample != 8)
                    {
                        iSample = *(short far *)lpSamples;
                        lpSamples++;
                        lpSamples++;
                    }
                    else
                        iSample = ((short)*lpSamples++ - 128) << 8;

                    aiSamples[n][m] = iSample;
                }
            }

            //
            //  now choose the first iDelta and setup the first two samples
            //  based on the 16 samples we copied from the source data above
            //  for each channel
            //
            for (m = 0; m < bChannels; m++)
            {
                //
                //  reset total error calculation for new block
                //
                adwTotalError[i][m] = 0L;

                //
                //  first 2 samples will come from real data--compute
                //  starting from there
                //
                aiSamp1[m] = aiSamples[1][m];
                aiSamp2[m] = aiSamples[0][m];
                        
                //
                //  calculate initial iDelta
                //
                iDelta = adpcmEncode4Bit_FirstDelta(iCoef1, iCoef2,
                        aiSamples[0][m], aiSamples[1][m], aiSamples[2][m],
                        aiSamples[3][m], aiSamples[4][m]);
                aiDelta[m] = aiFirstDelta[i][m] = iDelta;
            }

            //
            //  step over first two complete samples--we took care of them
            //  above
            //
            lpSamples = &lpSamplesBuf[cbSample * 2];

            //
            //  now encode the rest of the PCM data in this block--note
            //  we start 2 samples ahead because the first two samples are
            //  simply copied into the ADPCM block header...
            //
            for (n = 2; n < wBlockSize; n++)
            {
                //
                //  each channel gets encoded independently... obviously.
                //
                for (m = 0; m < bChannels; m++)
                {
                    //
                    //  yes, copy into cheaper variables because we access
                    //  them a lot
                    //
                    iSamp1 = aiSamp1[m];
                    iSamp2 = aiSamp2[m];
                    iDelta = aiDelta[m];

                    //
                    //  calculate the prediction based on the previous two
                    //  samples
                    //
                    lPrediction = ((long)iSamp1 * iCoef1 +
                                    (long)iSamp2 * iCoef2) >> MSADPCM_CSCALE;

                    //
                    //  grab the sample (for the current channel) to encode
                    //  from the source and convert it to a 16 bit data if
                    //  necessary
                    //
                    if (bBitsPerSample != 8)
                    {
                        iSample = *(short far *)lpSamples;
                        lpSamples++;
                        lpSamples++;
                    }
                    else
                        iSample = ((short)*lpSamples++ - 128) << 8;

                    //
                    //  encode it
                    //
                    lError = (long)iSample - lPrediction;
                    iOutput = (short)(lError / iDelta);
                    if (iOutput > MSADPCM_OUTPUT4_MAX)
                        iOutput = MSADPCM_OUTPUT4_MAX;
                    else if (iOutput < MSADPCM_OUTPUT4_MIN)
                        iOutput = MSADPCM_OUTPUT4_MIN;

                    lSamp = lPrediction + ((long)iDelta * iOutput);
        
                    if (lSamp > 32767)
                        lSamp = 32767;
                    else if (lSamp < -32768)
                        lSamp = -32768;
        
                    //
                    //  compute the next iDelta
                    //
                    iDelta = (short)((gaiP4[iOutput & 15] * (long)iDelta) >> MSADPCM_PSCALE);
                    if (iDelta < MSADPCM_DELTA4_MIN)
                        iDelta = MSADPCM_DELTA4_MIN;
        
                    //
                    //  save updated values for this channel back into the
                    //  original arrays...
                    //
                    aiDelta[m] = iDelta;
                    aiSamp2[m] = iSamp1;
                    aiSamp1[m] = (short)lSamp;

                    //
                    //  keep a running status on the error for the current
                    //  coefficient pair for this channel
                    //
                    lError = lSamp - iSample;
                    adwTotalError[i][m] += (lError * lError) >> 7;
                }
            }
        }


        //
        //  WHEW! we have now made 7 passes over the data and calculated
        //  the error for each--so it's time to find the one that produced
        //  the lowest error and use that predictor (this is for each
        //  channel of course)
        //
        for (m = 0; m < bChannels; m++)
        {
            abBestPredictor[m] = 0;
            dw = adwTotalError[0][m];
            for (i = 1; i < MSADPCM_NUM_COEF; i++)
            {
                if (adwTotalError[i][m] < dw)
                {
                    abBestPredictor[m] = i;
                    dw = adwTotalError[i][m];
                }
            }
        }    
        

        //
        //  reset pointer to beginning of our temporary samples buffer--
        //  we're going to make our final pass on the data with the optimal
        //  predictor that we found above
        //
        lpSamples = lpSamplesBuf;


        //
        //  grab first iDelta from our precomputed first deltas that we
        //  calculated above
        //
        for (m = 0; m < bChannels; m++)
        {
            i = abBestPredictor[m];
            aiDelta[m] = aiFirstDelta[i][m];
        }

        //
        //  get the first two samples from the source data (so we can write
        //  them into the ADPCM block header and use them in our prediction
        //  calculation when encoding the rest of the data).
        //
        if (bBitsPerSample != 8)
        {
            for (m = 0; m < bChannels; m++)
            {
                aiSamp2[m] = *(short far *)lpSamples;
                lpSamples++;
                lpSamples++;
            }
            for (m = 0; m < bChannels; m++)
            {
                aiSamp1[m] = *(short far *)lpSamples;
                lpSamples++;
                lpSamples++;
            }
        }
        else
        {
            for (m = 0; m < bChannels; m++)
            {
                aiSamp2[m] = ((short)*lpSamples++ - 128) << 8;
            }
            for (m = 0; m < bChannels; m++)
            {
                aiSamp1[m] = ((short)*lpSamples++ - 128) << 8;
            }
        }


        //
        //  write the block header for the encoded data
        //
        //  the block header is composed of the following data:
        //      1 byte predictor per channel
        //      2 byte delta per channel
        //      2 byte first sample per channel
        //      2 byte second sample per channel
        //
        //  this gives us (7 * bChannels) bytes of header information
        //
        //  so first write the 1 byte predictor for each channel into the
        //  destination buffer
        //
        for (m = 0; m < bChannels; m++)
        {
            *hpDst++ = abBestPredictor[m];
        }

        //
        //  now write the 2 byte delta per channel...
        //
        for (m = 0; m < bChannels; m++)
        {
            *(short huge *)hpDst = aiDelta[m];
            hpDst++;
            hpDst++;
        }

        //
        //  finally, write the first two samples (2 bytes each per channel)
        //
        for (m = 0; m < bChannels; m++)
        {
            *(short huge *)hpDst = aiSamp1[m];
            hpDst++;
            hpDst++;
        }
        for (m = 0; m < bChannels; m++)
        {
            *(short huge *)hpDst = aiSamp2[m];
            hpDst++;
            hpDst++;
        }

        //
        //  the number of bytes that we have written to the destination
        //  buffer is (7 * bChannels)--so add this to our total number of
        //  bytes written to the destination.. for our return value.
        //
        dwTotalConverted += wBlockHeaderBytes;
        

        //
        //  we have written the header for this block--now write the data
        //  chunk (which consists of a bunch of encoded nibbles). note that
        //  we start our count at 2 because we already wrote the first
        //  two samples into the destination buffer as part of the header
        //
        wFirstNibble = 1;
        for (n = 2; n < wBlockSize; n++)
        {
            //
            //  each channel gets encoded independently... obviously.
            //
            for (m = 0; m < bChannels; m++)
            {
                //
                //  use our chosen best predictor and grab the coefficient
                //  pair to use for this channel...
                //
                i = abBestPredictor[m];
                iCoef1 = lpwfADPCM->aCoef[i].iCoef1;
                iCoef2 = lpwfADPCM->aCoef[i].iCoef2;

                //
                //  copy into cheaper variables because we access them a lot
                //
                iSamp1 = aiSamp1[m];
                iSamp2 = aiSamp2[m];
                iDelta = aiDelta[m];

                //
                //  calculate the prediction based on the previous two samples
                //
                lPrediction = ((long)iSamp1 * iCoef1 +
                                (long)iSamp2 * iCoef2) >> MSADPCM_CSCALE;

                //
                //  grab the sample to encode--convert it to 16 bit data if
                //  necessary...
                //
                if (bBitsPerSample != 8)
                {
                    iSample = *(short far *)lpSamples;
                    lpSamples++;
                    lpSamples++;
                }
                else
                    iSample = ((short)*lpSamples++ - 128) << 8;

                //
                //  encode the sample
                //
                lError = (long)iSample - lPrediction;
                iOutput = (short)(lError / iDelta);
                if (iOutput > MSADPCM_OUTPUT4_MAX)
                    iOutput = MSADPCM_OUTPUT4_MAX;
                else if (iOutput < MSADPCM_OUTPUT4_MIN)
                    iOutput = MSADPCM_OUTPUT4_MIN;

                lSamp = lPrediction + ((long)iDelta * iOutput);
            
                if (lSamp > 32767)
                    lSamp = 32767;
                else if (lSamp < -32768)
                    lSamp = -32768;

                //
                //  compute the next iDelta
                //
                iDelta = (short)((gaiP4[iOutput&15] * (long)iDelta) >> MSADPCM_PSCALE); 
                if (iDelta < MSADPCM_DELTA4_MIN)
                    iDelta = MSADPCM_DELTA4_MIN;

                //
                //  save updated values for this channel back into the
                //  original arrays...
                //
                aiDelta[m] = iDelta;
                aiSamp2[m] = iSamp1;
                aiSamp1[m] = (short)lSamp;

                //
                //  we have another nibble of encoded data--either combine
                //  this with the previous nibble and write out a full 
                //  byte, or save this nibble for the next nibble to be
                //  combined into a full byte and written to the destination
                //  buffer... uhg!
                //
                if (wFirstNibble)
                {
                    wNextWrite = (iOutput & 15) << 4;
                    wFirstNibble = 0;
                }
                else
                {
                    *hpDst++ = (BYTE)(wNextWrite | (iOutput & 15));
                    dwTotalConverted += 1;
                    wFirstNibble++;
                }
            }
        }
    }

    //
    //  free the memory used for our small data buffer and return the number
    //  of bytes that we wrote into the destination buffer...
    //
    GlobalFreePtr(lpSamplesBuf);

    return (dwTotalConverted);
} /* adpcmEncode4Bit() */



//---------------------------------------------------------------------------
//
//  the code below provides 'support' routines for building/verifying ADPCM
//  headers, etc.
//
//  the coefficient pairs that should be in the wave format header for
//  the Microsoft 4 Bit ADPCM algorithm. the code to copy the coefficients
//  into the wave format header is shown:
//
//      short gaiCoef1[] = { 256,  512,  0, 192, 240,  460,  392 };
//      short gaiCoef2[] = {   0, -256,  0,  64,   0, -208, -232 };
//
//      for (w = 0; w < MSADPCM_NUM_COEF; w++)
//      {
//          lpwfADPCM->aCoef[w].iCoef1 = gaiCoef1[w];
//          lpwfADPCM->aCoef[w].iCoef2 = gaiCoef2[w];
//      }
//
//---------------------------------------------------------------------------

static short gaiCoef1[] = { 256,  512,  0, 192, 240,  460,  392 };
static short gaiCoef2[] = {   0, -256,  0,  64,   0, -208, -232 };


/** BOOL FAR PASCAL adpcmIsValidFormat(LPWAVEFORMATEX lpwfx)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX lpwfx)
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL adpcmIsValidFormat(LPWAVEFORMATEX lpwfx)
{
    LPADPCMWAVEFORMAT   lpwfADPCM;
    WORD                w;

    if (!lpwfx)
        return (FALSE);

    if (lpwfx->wFormatTag != WAVE_FORMAT_ADPCM)
        return (FALSE);

    if (lpwfx->wBitsPerSample != 4)
        return (FALSE);

    if ((lpwfx->nChannels < 1) || (lpwfx->nChannels > MSADPCM_MAX_CHANNELS))
        return (FALSE);


    //
    //  check coef's to see if it is Microsoft's standard 4 Bit ADPCM
    //
    lpwfADPCM = (LPADPCMWAVEFORMAT)lpwfx;

    if (lpwfADPCM->wNumCoef != MSADPCM_NUM_COEF)
        return (FALSE);

    for (w = 0; w < MSADPCM_NUM_COEF; w++)
    {
        if (lpwfADPCM->aCoef[w].iCoef1 != gaiCoef1[w])
            return (FALSE);

        if (lpwfADPCM->aCoef[w].iCoef2 != gaiCoef2[w])
            return (FALSE);
    }

    return (TRUE);
} /* adpcmIsValidFormat() */


/** BOOL FAR PASCAL pcmIsValidFormat(LPWAVEFORMATEX lpwfx)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX lpwfx)
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL pcmIsValidFormat(LPWAVEFORMATEX lpwfx)
{
    if (!lpwfx)
        return (FALSE);

    if (lpwfx->wFormatTag != WAVE_FORMAT_PCM)
        return (FALSE);

    if ((lpwfx->wBitsPerSample != 8) && (lpwfx->wBitsPerSample != 16))
        return (FALSE);

    if ((lpwfx->nChannels < 1) || (lpwfx->nChannels > MSADPCM_MAX_CHANNELS))
        return (FALSE);

    return (TRUE);
} /* pcmIsValidFormat() */


/** BOOL FAR PASCAL adpcmBuildFormatHeader(LPWAVEFORMATEX lpwfxSrc, LPWAVEFORMATEX lpwfxDst)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX lpwfxSrc, LPWAVEFORMATEX lpwfxDst)
 *
 *  RETURN (BOOL FAR PASCAL):
 *
 *
 *  NOTES:
 *
 ** */

BOOL FAR PASCAL adpcmBuildFormatHeader(LPWAVEFORMATEX lpwfxSrc, LPWAVEFORMATEX lpwfxDst)
{
    LPADPCMWAVEFORMAT   lpwfADPCM;
    WORD                wBlockAlign;
    WORD                wChannels;
    WORD                wBitsPerSample;
    WORD                wHeaderBytes;
    DWORD               dw;
    WORD                w;

    //
    //  if the source format is PCM, then build an ADPCM destination format
    //  header... assuming the PCM format header is valid.
    //
    if (lpwfxSrc->wFormatTag == WAVE_FORMAT_PCM)
    {
        if (!pcmIsValidFormat(lpwfxSrc))
            return (FALSE);

        lpwfADPCM = (LPADPCMWAVEFORMAT)lpwfxDst;

        //
        //  fill in destination header with appropriate ADPCM stuff based
        //  on source PCM header...
        //
        lpwfxDst->wFormatTag     = WAVE_FORMAT_ADPCM;
        lpwfxDst->nSamplesPerSec = lpwfxSrc->nSamplesPerSec;
        lpwfxDst->nChannels      = wChannels      = lpwfxSrc->nChannels;
        lpwfxDst->wBitsPerSample = wBitsPerSample = 4;

        //
        //  choose a block alignment that makes sense for the sample rate
        //  that the original PCM data is. basically, this needs to be
        //  some reasonable number to allow efficient streaming, etc.
        //
        //  don't let block alignment get too small...
        //
        wBlockAlign = 256 * wChannels;
        if (lpwfxSrc->nSamplesPerSec > 11025)
            wBlockAlign *= (WORD)(lpwfxSrc->nSamplesPerSec / 11000);

        lpwfxDst->nBlockAlign = wBlockAlign;

        //
        //  compute that 'samples per block' that will be in the encoded
        //  ADPCM data blocks. this is determined by subtracting out the
        //  'other info' contained in each block--a block is composed of
        //  a header followed by the encoded data.
        //
        //  the block header is composed of the following data:
        //      1 byte predictor per channel
        //      2 byte delta per channel
        //      2 byte first sample per channel
        //      2 byte second sample per channel
        //
        //  this gives us (7 * wChannels) bytes of header information that
        //  contains our first two full samples (so we add two below).
        //
        wHeaderBytes = (7 * wChannels);

        w = (wBlockAlign - wHeaderBytes) * 8;
        lpwfADPCM->wSamplesPerBlock = (w / (wBitsPerSample * wChannels)) + 2;

        //
        //  now compute the avg bytes per second (man this code bites!)
        //
        dw = (((DWORD)wBitsPerSample * wChannels * 
                        (DWORD)lpwfxDst->nSamplesPerSec) / 8);
        lpwfxDst->nAvgBytesPerSec = (WORD)(dw + wHeaderBytes + 
                        ((dw / wBlockAlign) * wHeaderBytes));

        //
        //  fill in the cbSize field of the extended wave format header.
        //  this number is the number of _EXTRA BYTES_ *after* the end
        //  of the WAVEFORMATEX structure that are need for the compression
        //  format.
        //
        //  for Microsoft's 4 Bit ADPCM format, this number is 32:
        //
        lpwfxDst->cbSize = sizeof(ADPCMWAVEFORMAT) - sizeof(WAVEFORMATEX) +
                            (MSADPCM_NUM_COEF * sizeof(ADPCMCOEFSET));

        //
        //  copy the Microsoft 4 Bit ADPCM coef's into the header
        //
        lpwfADPCM->wNumCoef = MSADPCM_NUM_COEF;
        for (w = 0; w < MSADPCM_NUM_COEF; w++)
        {
            lpwfADPCM->aCoef[w].iCoef1 = gaiCoef1[w];
            lpwfADPCM->aCoef[w].iCoef2 = gaiCoef2[w];
        }

        return (TRUE);
    }

    //
    //  if the source format is ADPCM, then build an appropriate PCM header
    //
    else if (lpwfxSrc->wFormatTag == WAVE_FORMAT_ADPCM)
    {
        if (!adpcmIsValidFormat(lpwfxSrc))
            return (FALSE);

        //
        //  fill in the info for our destination format...
        //
        lpwfxDst->wFormatTag     = WAVE_FORMAT_PCM;
        lpwfxDst->nSamplesPerSec = lpwfxSrc->nSamplesPerSec;
        lpwfxDst->nChannels      = wChannels = lpwfxSrc->nChannels;

        //
        //  NOTE: bits per sample can be 8 or 16.. most people don't have
        //  16 bit boards (yet!), so default to 8 bit decoding..
        //
#if 0
        lpwfxDst->wBitsPerSample = wBitsPerSample = 16;
#else
        lpwfxDst->wBitsPerSample = wBitsPerSample = 8;
#endif

        //
        //  set nAvgBytesPerSec and nBlockAlign
        //
        lpwfxDst->nBlockAlign     = (wBitsPerSample >> 3) << (wChannels >> 1);
        lpwfxDst->nAvgBytesPerSec = lpwfxDst->nSamplesPerSec * wBitsPerSample;

        return (TRUE);
    }

    return (FALSE);
} /* adpcmBuildFormatHeader() */


/** EOF: msadpcm.c **/

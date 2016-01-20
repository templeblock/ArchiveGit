
	  Indeo(R) Video Interactive R4.1 -- Version 4.11.15.60
		   Microsoft Windows* 95 and Windows NT
		            October 24, 1995

This setup program installs the 32-bit Indeo video drivers on Windows 95
or Windows NT systems.
 

Readme32.txt Topics:
------------------
 o Indeo Video Driver Versions
 o Installation
 o Additional Documentation
 o Known Operating Characteristics of R4.1
 o Software Requirements
 o Minimum System Configuration
 o Technical Support and Updates


Indeo Video Drivers:
--------------------  
  Indeo(R) video interactive R4.1 - V4.11.15.60  [filename: ir41_32.dll]
  Indeo(R) video R3.2             - V3.24.15.02  [filename: ir32_32.dll]
  Indeo(R) video Raw YVU9         - V1.10.15.11  [filename: iyvu9_32.dll]


Installation:
-------------
Windows 95:

The setup program installs the Indeo video drivers in the Windows 95
System subdirectory (e.g. \windows\system).  Setup puts the 
following entries in the system.ini file:

[drivers32]
VIDC.IV41=ir41_32.dll
VIDC.IV32=ir32_32.dll
VIDC.IV31=ir32_32.dll
VIDC.YVU9=iyvu9_32.dll

Setup also places the following entries in the HKEY_LOCAL_MACHINE
section of the system registry:

System\CurrentControlSet\control\MediaResources\icm\vidc.IV41
System\CurrentControlSet\control\MediaResources\icm\vidc.IV32
System\CurrentControlSet\control\MediaResources\icm\vidc.IV31
System\CurrentControlSet\control\MediaResources\icm\vidc.YVU9

Windows NT:

The setup program installs the Indeo video drivers in the Windows NT
System32 subdirectory (e.g. \windows\system32).  Setup puts the
following entries into the HKEY_LOCAL_MACHINE section of the
system registry:

Software\Microsoft\Windows NT\CurrentVersion\Drivers\VIDC.IV41
Software\Microsoft\Windows NT\CurrentVersion\Drivers\VIDC.IV32
Software\Microsoft\Windows NT\CurrentVersion\Drivers\VIDC.IV31
Software\Microsoft\Windows NT\CurrentVersion\Drivers\VIDC.YVU9
Software\Microsoft\Windows NT\CurrentVersion\drivers.desc\IR41_32.DLL
Software\Microsoft\Windows NT\CurrentVersion\drivers.desc\IR32_32.DLL
Software\Microsoft\Windows NT\CurrentVersion\drivers.desc\IYVU9_32.DLL


Additional Documentation:
-------------------------
o OVERVIEW.DOC
  This document presents a high-level overview of Indeo video interactive's new
  features.

o TRAN_IV4.DOC
  This whitepaper describes techniques for successfully encoding transparent   
  Indeo Video Bitstreams using Adobe Premiere* 4.0 for Windows

o R4CAPS.DOC
  Version 2.6 of the "Indeo Video Interactive Features and Capabilities"
  document, includes updates to the video for windows programming
  interface specification.
		
o EDITING.DOC
  A brief explanation on how to use present editing tools to edit
  and compress avi files utilizing the new features of Indeo
  video R4.

o VFW_SPEC.H
  A header file to match the Video for Windows ICM messages and
  structures for the Indeo video interactive codec.

o IVR4PAL.DOC
  An explanation of a new configurable palette mode that is not 
  supported by previous releases of Indeo video.  The codec uses 
  only the first 198 palette entries for video colors; the remaining 
  38 palette entries are reserved for use by the application.

o IV41DFLT.PAL
  Indeo Video Interactive palette. 


Known Operating Characteristics of R4.1:
----------------------------------------	
o Can not use MCIAVI to access interactive features

  In order to access the new interactive features and capabilities
  introduced by Indeo video interactive within the existing Video For Windows
  architecture, the ICM interface must be used.  VfW does not allow
  access to the ICM via the MCIAVI interface.  As a result, an interactive
  application must provide the MCIAVI functionality for decoding such as
  avi file I/O, sound play and synchronization, and keyframe management.

  Linear applications, such as those presently using Indeo Video R3.2,
  can use the MCIAVI interface and obtain the image quality and the 
  automatic scalability capability of Indeo video interactive.

o Does not load on 386 machines

  In order to provide a high quality image with good playback performance, 
  the recommended minimum system configuration for Indeo video interactive
  is a  486DX2/66.  The Indeo video interactive codec is implemented with
  instructions specific to 486 or later processors. If the codec detects a
  386 processor or less, then the codec is not loaded.  It is recommended
  that applications check the processor type and notify the 386 user that
  they have insufficient resources to play the video portions of the
  application.

o Playing Multiple clips

  Multiple clips will play simultaneously; however, playing clips with
  different frame rates disables the scalability mechanism.  During
  concurrent playback, both clips will play all frames and all bands.
  Scalability will be re-enabled as soon as all active clips have the
  same frame rate.

o Transparency Playback and Dropped Frames

  1.  For software vendors using Transparency and drawing directly to
      the on-screen frame buffer:

      If the application requires all frames to be returned, such as in
      an application that uses transparency to an on-screen buffer, then
      the codec should be used without ICM_DRAW_BEGIN and ICM_DRAW_END
      calls surrounding the DECOMPRESS messages for the motion playback.
      This ensures that no frames will be dropped so that the
      non-transparent section is overlayed on the background.
      These DRAW messages cause the decoder to begin scaling back and
      dropping frames when the required decode time is not available.
      The ICM_DRAW_BEGIN message puts the the decoder in "Normal
      Playback" mode, and the ICM_DRAW_END message takes it out and puts
      it back into "Stepping Mode" (The default playback mode of the
      decoder), where all frames are returned.

  2.  For software vendors using Transparency and drawing to an
      off-screen frame buffer:

      The application needs to recognize the codec's "Don't Draw"
      message and not flip screen buffer pages in this case.

o IF09 and Transparency / Local Decode

  IF09 is a data format which can be output by the R4.1 codec.  It is
  similar to the Raw YVU9 format.  IF09 is used as input to DCI enabled
  hardware that handles color space conversion.

  1.  The IF09 output format supported by Indeo video interactive does
      not directly support the Transparency and Local Decode features
      of the algorithm, however they can be used together with
      application support.

  2.  IF09 and Transparency:  The entire IF09 buffer is filled by image
      data regardless of Transparency (or local decode).  Thus,
      transparent pixels ARE written in this buffer.  To use
      transparency, the calling application must request to have the
      transparency mask exposed, and combine it with the IF09 buffer at
      color conversion time.  

  3.  IF09 and Local Decode:  The calling application has 2 ways of
      getting full local decode support with the decoder.  It may
      manually clip view rectangle from the IF09 buffer, as it color
      converts it to the screen, or it may request the transparency
      mask, which is clipped to the view rect, and treat local decode
      as transparency.  In either case, the Decode rectangle will
      still correctly tell the decoder the portions of video to decode.

o New Indeo Video Interactive Palette

  The Indeo video interactive palette has changed relative
  to the Indeo Video R3.2 palette.  Any graphics or video files
  created based on the Indeo Video R3.x palette will need to be 
  re-done to work with the Indeo video interactive palette.

o Advanced Compression Options

  When using VidEdit, you must use 'Save as Default" in order 
  to have the codec use your current settings.  This is due to
  the operating characteristics of VidEdit.

o Frame Sizes

  In increments of 4, the minimum supported frame size is 32x32
  (pixels); and the maximum supported frame size is 8192x8192
  (pixels).

o Concatenating files using an editing application

  When concatenating multiple V4.1 encoded files using an editing 
  application such as Adobe Premiere, you must insure that all the 
  clips are the same size and were encoded with the same features 
  in the Advanced Dialog box (bi-dir, transparency, view port, 
  scalability, ...).  Combining files with different parameters 
  will have undetermined behavior.


Software Requirements:
-----------------------
o Microsoft Windows 95 or Windows NT 
	

Minimum Hardware Configuration:
------------------------------
486DX2/66 Mhz CPU 
8 MB system RAM 
CD ROM drive(300KB/sec)
SVGA display (VESA compliant)
8-bit Sound board

For best frame rate performance, configure the graphics card for 8-bit 
color mode.  For best image quality, use 24-bit color mode.

Off-line encoding is CPU-intensive, i.e. the faster the CPU the faster the 
compression time.


Technical Support and Updates:
------------------------------

Technical information and any future updates to Indeo Video Interactive will be 
posted on the following electronic forums:

  World Wide Web     http://www.intel.com     PC Developers Support, Multimedia
  CompuServe         Go IntelArch             Multimedia Forum
  Internet           ftp.intel.com            ~/pub/IAL/multimedia

For technical assistance with Indeo technology, post a message on the 
IntelArch forum on Compuserve in the Multimedia library #9 or send an 
electronic mail message to developer_support@intel.com.

_____________________________________________________________________
* Other brands and names are the property of their respective owners.

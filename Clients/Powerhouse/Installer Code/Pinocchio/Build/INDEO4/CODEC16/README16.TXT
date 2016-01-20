
	  Indeo(R) video interactive R4.1 -- Version 4.11.01.60
			 Microsoft Windows* 3.x
			    October 24, 1995

This setup program installs the 16-bit Indeo video drivers on Windows 3.x
systems along with Microsoft's Video for Windows 1.1e.
 

Readme16.txt Topics:
------------------
 o Indeo Video Driver Versions
 o Installation
 o Additional Documentation
 o Known Operating Characteristics
 o Differences from Microsoft's VfW 1.1e Update
 o Software Requirements
 o Minimum System Configuration
 o Technical Support and Updates


Indeo Video Driver Versions:
----------------------------  
  Indeo(R) video interactive R4.1 - V4.11.01.60  [filename: ir41.dll]
  Indeo(R) video R3.2             - V3.24.01.02  [filename: ir32.dll]
  Indeo(R) video Raw YVU9         - V1.10.01.11  [filename: iyvu9.dll]


Installation:
-------------
The Setup program installs the Indeo video drivers along with
Microsoft's latest Video for Windows runtime.  It places the
following entries in the system.ini file for the Indeo video
drivers:

[drivers]
VIDC.IV41=ir41.dll
VIDC.IV32=ir32.dll
VIDC.IV31=ir32.dll
VIDC.YVU9=iyvu9.dll


Additional Documentation:
-------------------------
The following articles can be found at the Intel Web site and CompuServe*
forum listed at the end of this document.

o OVERVIEW.DOC
  This document presents a high-level overview of Indeo video interactive's 
  new features.

o R4CAPS.DOC
  The "Indeo video interactive Features and Capabilities" document.
		
o VFW_SPEC.DOC
  Updates the Video for Windows programming interface specification.

o VFW_SPEC.H
  A header file to match the Video for Windows ICM messages and
  structures for the Indeo video interactive codec.

o EDITING.DOC
  A brief explanation on how to use present editing tools to edit
  and compress AVI files utilizing the new features of Indeo
  video interactive.

o TRAN_IV4.DOC
  This whitepaper describes techniques for successfully encoding transparent   
  Indeo Video Bitstreams using Adobe Premiere* 4.0 for Windows

o IVR4PAL.DOC
  An explanation of all the palette modes supported by Indeo video
  interactive.

o IV41DFLT.PAL
  The Indeo video interactive default palette. 

o IV41CNFG.PAL
  The Indeo video interactive configurable palette.


Known Operating Characteristics:
--------------------------------        
These are the known operating characteristics of Indeo video interactive
R4.1 only:

o Low memory conditions for 16-bit driver only

  The 16-bit version of Indeo video interactive may become unstable if the
  amount of memory consumed under Window 3.1 reaches the total available
  amount of virtual memory in the system.  It is recommended that virtual
  memory always be enabled, and the "recommended" amount of virtual memory
  is selected as specified by the virtual memory setup dialog box.  The
  Indeo video driver operates properly as long as there is a sufficient
  amount of virtual memory.

o Can not use MCIAVI to access interactive features

  In order to access the new interactive features and capabilities
  introduced by Indeo video interactive within the existing Video For Windows
  architecture, the ICM interface must be used.  VfW does not allow
  access to the ICM via the MCIAVI interface.  As a result, an interactive
  application must provide functionality for decoding such as AVI file I/O,
  sound play and synchronization, and keyframe management.  Refer to the
  Kplayer applet for a code example.

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
      it back into "Stepping Mode" (the default playback mode of the
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

o New Indeo video interactive Default Palette

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
  (pixels).  The maximum supported frame size is 8192x8192 (pixels).

o Concatenating files using an editing application

  When concatenating multiple V4.1 encoded files using an editing 
  application such as Adobe Premiere, you must insure that all the 
  clips are the same size and were encoded with the same features 
  in the Advanced Dialog box (bi-dir, transparency, view port, 
  scalability, ...).  Combining files with different parameters 
  will result in undetermined behavior.


Differences from Microsoft's VfW 1.1e Update:
---------------------------------------------
The Setup utility used to install this update is a modified version of
the Setup utility Microsoft provides with their runtime package.  It is
possible to make further changes to this program.  The Setup Toolkit
is in file called CP0982.EXE at the following Microsoft FTP site:

   ftp://ftp.microsoft.com/Softlib

All of the Intel and non-Intel codecs are installed by this update.
Listed below are the differences between this update and the one
supplied by Microsoft:

1. Installs the Indeo video interactive R4.1 driver.
2. Installs R3.2 V3.24.01.02 driver rather than V3.22.1.43.
3. The Raw driver (iyvu9.dll) is updated.  The new version fixes a
   memory leak.
4. The R2.1 driver is not installed.
5. The readme16.txt and license.txt files are added.
6. The files in this update do not all fit on one 3.5" diskette
   even when compressed.


Software Requirements:
-----------------------
o Microsoft Windows 3.1
	

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
compression time.  Pentium(R) Processor systems are more than twice as fast
as 486-based systems at doing off-line compression.


Technical Support and Updates:
------------------------------
Technical information and any future updates to Indeo video interactive will 
be posted on the following electronic forums:

  World Wide Web     http://www.intel.com    PC Developers Support, Multimedia
  CompuServe         Go IntelArch            Multimedia Forum
  Internet           ftp.intel.com           ~/pub/IAL/multimedia

For technical assistance with Indeo technology, post a message on the 
IntelArch forum on Compuserve in the Multimedia library #9 or send an 
electronic mail message to developer_support@intel.com.

_____________________________________________________________________
* Other brands and names are the property of their respective owners.

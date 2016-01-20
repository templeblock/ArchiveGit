/****************************************************************************

	File:		SCBEZBLE.C


	$Header: /Projects/Toolbox/ct/SCBEZBLE.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	the blending values for computing beziers

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

****************************************************************************/

/* this contains standard function values, we use tables instead of actually 
 * computing the value
 */

#include "scbezier.h"

scBezBlendValue bezblend[scBezBlendSize] = {

/* this one appears in all */
{
	scBezFactor( 0x0000,	 1.0000000 ),	/*	(0 0) */
	scBezFactor( 0x0000,	 0.0000000 ),	/*	(0 1) */
	scBezFactor( 0x0000,	 0.0000000 ),	/*	(0 2) */
	scBezFactor( 0x0000,	 0.0000000 )	/*	(0 3) */
},

#ifdef SubDiv256
{
	scBezFactor( 0xfd02,	 0.9883270 ),	/*	(1 0) */
	scBezFactor( 0x02fa,	 0.0116274 ),	/*	(1 1) */
	scBezFactor( 0x0002,	 0.0000456 ),	/*	(1 2) */
	scBezFactor( 0x0000,	 0.0000001 )	/*	(1 3) */
},
#endif

#ifdef SubDiv128
{
	scBezFactor( 0xfa0b,	 0.9767451 ),	/*	(2 0) */
	scBezFactor( 0x05e8,	 0.0230727 ),	/*	(2 1) */
	scBezFactor( 0x000b,	 0.0001817 ),	/*	(2 2) */
	scBezFactor( 0x0000,	 0.0000005 )	/*	(2 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xf71a,	 0.9652541 ),	/*	(3 0) */
	scBezFactor( 0x08ca,	 0.0343371 ),	/*	(3 1) */
	scBezFactor( 0x001a,	 0.0004072 ),	/*	(3 2) */
	scBezFactor( 0x0000,	 0.0000016 )	/*	(3 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0xf42f,	 0.9538536 ),	/*	(4 0) */
	scBezFactor( 0x0ba0,	 0.0454216 ),	/*	(4 1) */
	scBezFactor( 0x002f,	 0.0007210 ),	/*	(4 2) */
	scBezFactor( 0x0000,	 0.0000038 )	/*	(4 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xf14a,	 0.9425432 ),	/*	(5 0) */
	scBezFactor( 0x0e6b,	 0.0563273 ),	/*	(5 1) */
	scBezFactor( 0x0049,	 0.0011221 ),	/*	(5 2) */
	scBezFactor( 0x0000,	 0.0000075 )	/*	(5 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xee6b,	 0.9313226 ),	/*	(6 0) */
	scBezFactor( 0x112a,	 0.0670552 ),	/*	(6 1) */
	scBezFactor( 0x0069,	 0.0016093 ),	/*	(6 2) */
	scBezFactor( 0x0000,	 0.0000129 )	/*	(6 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xeb91,	 0.9201913 ),	/*	(7 0) */
	scBezFactor( 0x13de,	 0.0776065 ),	/*	(7 1) */
	scBezFactor( 0x008e,	 0.0021817 ),	/*	(7 2) */
	scBezFactor( 0x0001,	 0.0000204 )	/*	(7 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0xe8be,	 0.9091492 ),	/*	(8 0) */
	scBezFactor( 0x1686,	 0.0879822 ),	/*	(8 1) */
	scBezFactor( 0x00ba,	 0.0028381 ),	/*	(8 2) */
	scBezFactor( 0x0002,	 0.0000305 )	/*	(8 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xe5f0,	 0.8981957 ),	/*	(9 0) */
	scBezFactor( 0x1922,	 0.0981833 ),	/*	(9 1) */
	scBezFactor( 0x00ea,	 0.0035775 ),	/*	(9 2) */
	scBezFactor( 0x0002,	 0.0000435 )	/*	(9 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xe328,	 0.8873305 ),	/*	(10 0) */
	scBezFactor( 0x1bb3,	 0.1082110 ),	/*	(10 1) */
	scBezFactor( 0x0120,	 0.0043988 ),	/*	(10 2) */
	scBezFactor( 0x0003,	 0.0000596 )	/*	(10 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xe065,	 0.8765534 ),	/*	(11 0) */
	scBezFactor( 0x1e39,	 0.1180664 ),	/*	(11 1) */
	scBezFactor( 0x015b,	 0.0053009 ),	/*	(11 2) */
	scBezFactor( 0x0005,	 0.0000793 )	/*	(11 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0xdda9,	 0.8658638 ),	/*	(12 0) */
	scBezFactor( 0x20b4,	 0.1277504 ),	/*	(12 1) */
	scBezFactor( 0x019b,	 0.0062828 ),	/*	(12 2) */
	scBezFactor( 0x0006,	 0.0001030 )	/*	(12 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xdaf2,	 0.8552615 ),	/*	(13 0) */
	scBezFactor( 0x2323,	 0.1372642 ),	/*	(13 1) */
	scBezFactor( 0x01e1,	 0.0073434 ),	/*	(13 2) */
	scBezFactor( 0x0008,	 0.0001310 )	/*	(13 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xd841,	 0.8447461 ),	/*	(14 0) */
	scBezFactor( 0x2588,	 0.1466088 ),	/*	(14 1) */
	scBezFactor( 0x022b,	 0.0084815 ),	/*	(14 2) */
	scBezFactor( 0x000a,	 0.0001636 )	/*	(14 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xd595,	 0.8343173 ),	/*	(15 0) */
	scBezFactor( 0x27e1,	 0.1557854 ),	/*	(15 1) */
	scBezFactor( 0x027b,	 0.0096962 ),	/*	(15 2) */
	scBezFactor( 0x000d,	 0.0002012 )	/*	(15 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0xd2f0,	 0.8239746 ),	/*	(16 0) */
	scBezFactor( 0x2a30,	 0.1647949 ),	/*	(16 1) */
	scBezFactor( 0x02d0,	 0.0109863 ),	/*	(16 2) */
	scBezFactor( 0x0010,	 0.0002441 )	/*	(16 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xd04f,	 0.8137178 ),	/*	(17 0) */
	scBezFactor( 0x2c73,	 0.1736385 ),	/*	(17 1) */
	scBezFactor( 0x0329,	 0.0123509 ),	/*	(17 2) */
	scBezFactor( 0x0013,	 0.0002928 )	/*	(17 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xcdb5,	 0.8035464 ),	/*	(18 0) */
	scBezFactor( 0x2eac,	 0.1823173 ),	/*	(18 1) */
	scBezFactor( 0x0387,	 0.0137887 ),	/*	(18 2) */
	scBezFactor( 0x0016,	 0.0003476 )	/*	(18 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xcb20,	 0.7934602 ),	/*	(19 0) */
	scBezFactor( 0x30da,	 0.1908322 ),	/*	(19 1) */
	scBezFactor( 0x03ea,	 0.0152988 ),	/*	(19 2) */
	scBezFactor( 0x001a,	 0.0004088 )	/*	(19 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0xc890,	 0.7834587 ),	/*	(20 0) */
	scBezFactor( 0x32fd,	 0.1991844 ),	/*	(20 1) */
	scBezFactor( 0x0452,	 0.0168800 ),	/*	(20 2) */
	scBezFactor( 0x001f,	 0.0004768 )	/*	(20 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xc606,	 0.7735416 ),	/*	(21 0) */
	scBezFactor( 0x3516,	 0.2073750 ),	/*	(21 1) */
	scBezFactor( 0x04be,	 0.0185314 ),	/*	(21 2) */
	scBezFactor( 0x0024,	 0.0005520 )	/*	(21 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xc382,	 0.7637086 ),	/*	(22 0) */
	scBezFactor( 0x3724,	 0.2154050 ),	/*	(22 1) */
	scBezFactor( 0x052f,	 0.0202518 ),	/*	(22 2) */
	scBezFactor( 0x0029,	 0.0006347 )	/*	(22 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xc103,	 0.7539592 ),	/*	(23 0) */
	scBezFactor( 0x3928,	 0.2232755 ),	/*	(23 1) */
	scBezFactor( 0x05a4,	 0.0220401 ),	/*	(23 2) */
	scBezFactor( 0x002f,	 0.0007252 )	/*	(23 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0xbe8a,	 0.7442932 ),	/*	(24 0) */
	scBezFactor( 0x3b22,	 0.2309875 ),	/*	(24 1) */
	scBezFactor( 0x061e,	 0.0238953 ),	/*	(24 2) */
	scBezFactor( 0x0036,	 0.0008240 )	/*	(24 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xbc15,	 0.7347102 ),	/*	(25 0) */
	scBezFactor( 0x3d11,	 0.2385423 ),	/*	(25 1) */
	scBezFactor( 0x069b,	 0.0258163 ),	/*	(25 2) */
	scBezFactor( 0x003d,	 0.0009313 )	/*	(25 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xb9a7,	 0.7252097 ),	/*	(26 0) */
	scBezFactor( 0x3ef5,	 0.2459407 ),	/*	(26 1) */
	scBezFactor( 0x071e,	 0.0278020 ),	/*	(26 2) */
	scBezFactor( 0x0044,	 0.0010476 )	/*	(26 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xb73e,	 0.7157915 ),	/*	(27 0) */
	scBezFactor( 0x40d0,	 0.2531839 ),	/*	(27 1) */
	scBezFactor( 0x07a4,	 0.0298514 ),	/*	(27 2) */
	scBezFactor( 0x004c,	 0.0011732 )	/*	(27 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0xb4da,	 0.7064552 ),	/*	(28 0) */
	scBezFactor( 0x42a1,	 0.2602730 ),	/*	(28 1) */
	scBezFactor( 0x082e,	 0.0319633 ),	/*	(28 2) */
	scBezFactor( 0x0055,	 0.0013084 )	/*	(28 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xb27b,	 0.6972005 ),	/*	(29 0) */
	scBezFactor( 0x4467,	 0.2672090 ),	/*	(29 1) */
	scBezFactor( 0x08bd,	 0.0341368 ),	/*	(29 2) */
	scBezFactor( 0x005f,	 0.0014537 )	/*	(29 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xb022,	 0.6880269 ),	/*	(30 0) */
	scBezFactor( 0x4624,	 0.2739930 ),	/*	(30 1) */
	scBezFactor( 0x094f,	 0.0363708 ),	/*	(30 2) */
	scBezFactor( 0x0069,	 0.0016093 )	/*	(30 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xadce,	 0.6789342 ),	/*	(31 0) */
	scBezFactor( 0x47d7,	 0.2806261 ),	/*	(31 1) */
	scBezFactor( 0x09e5,	 0.0386640 ),	/*	(31 2) */
	scBezFactor( 0x0074,	 0.0017757 )	/*	(31 3) */
},
#endif


#ifdef SubDiv8
{
	scBezFactor( 0xab80,	 0.6699219 ),	/*	(32 0) */
	scBezFactor( 0x4980,	 0.2871094 ),	/*	(32 1) */
	scBezFactor( 0x0a80,	 0.0410156 ),	/*	(32 2) */
	scBezFactor( 0x0080,	 0.0019531 )	/*	(32 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xa936,	 0.6609897 ),	/*	(33 0) */
	scBezFactor( 0x4b1f,	 0.2934439 ),	/*	(33 1) */
	scBezFactor( 0x0b1d,	 0.0434244 ),	/*	(33 2) */
	scBezFactor( 0x008c,	 0.0021420 )	/*	(33 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0xa6f2,	 0.6521373 ),	/*	(34 0) */
	scBezFactor( 0x4cb4,	 0.2996306 ),	/*	(34 1) */
	scBezFactor( 0x0bbf,	 0.0458894 ),	/*	(34 2) */
	scBezFactor( 0x0099,	 0.0023427 )	/*	(34 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xa4b3,	 0.6433643 ),	/*	(35 0) */
	scBezFactor( 0x4e40,	 0.3056708 ),	/*	(35 1) */
	scBezFactor( 0x0c64,	 0.0484094 ),	/*	(35 2) */
	scBezFactor( 0x00a7,	 0.0025555 )	/*	(35 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0xa279,	 0.6346703 ),	/*	(36 0) */
	scBezFactor( 0x4fc2,	 0.3115654 ),	/*	(36 1) */
	scBezFactor( 0x0d0d,	 0.0509834 ),	/*	(36 2) */
	scBezFactor( 0x00b6,	 0.0027809 )	/*	(36 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0xa045,	 0.6260549 ),	/*	(37 0) */
	scBezFactor( 0x513b,	 0.3173155 ),	/*	(37 1) */
	scBezFactor( 0x0db9,	 0.0536104 ),	/*	(37 2) */
	scBezFactor( 0x00c5,	 0.0030192 )	/*	(37 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x9e15,	 0.6175179 ),	/*	(38 0) */
	scBezFactor( 0x52ab,	 0.3229222 ),	/*	(38 1) */
	scBezFactor( 0x0e68,	 0.0562892 ),	/*	(38 2) */
	scBezFactor( 0x00d6,	 0.0032706 )	/*	(38 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x9beb,	 0.6090589 ),	/*	(39 0) */
	scBezFactor( 0x5411,	 0.3283866 ),	/*	(39 1) */
	scBezFactor( 0x0f1b,	 0.0590188 ),	/*	(39 2) */
	scBezFactor( 0x00e7,	 0.0035357 )	/*	(39 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x99c6,	 0.6006775 ),	/*	(40 0) */
	scBezFactor( 0x556e,	 0.3337097 ),	/*	(40 1) */
	scBezFactor( 0x0fd2,	 0.0617981 ),	/*	(40 2) */
	scBezFactor( 0x00fa,	 0.0038147 )	/*	(40 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x97a5,	 0.5923733 ),	/*	(41 0) */
	scBezFactor( 0x56c1,	 0.3388926 ),	/*	(41 1) */
	scBezFactor( 0x108b,	 0.0646260 ),	/*	(41 2) */
	scBezFactor( 0x010d,	 0.0041080 )	/*	(41 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x958a,	 0.5841460 ),	/*	(42 0) */
	scBezFactor( 0x580c,	 0.3439364 ),	/*	(42 1) */
	scBezFactor( 0x1147,	 0.0675015 ),	/*	(42 2) */
	scBezFactor( 0x0121,	 0.0044160 )	/*	(42 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x9374,	 0.5759953 ),	/*	(43 0) */
	scBezFactor( 0x594d,	 0.3488422 ),	/*	(43 1) */
	scBezFactor( 0x1207,	 0.0704235 ),	/*	(43 2) */
	scBezFactor( 0x0136,	 0.0047390 )	/*	(43 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x9163,	 0.5679207 ),	/*	(44 0) */
	scBezFactor( 0x5a86,	 0.3536110 ),	/*	(44 1) */
	scBezFactor( 0x12c9,	 0.0733910 ),	/*	(44 2) */
	scBezFactor( 0x014c,	 0.0050774 )	/*	(44 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x8f57,	 0.5599219 ),	/*	(45 0) */
	scBezFactor( 0x5bb5,	 0.3582439 ),	/*	(45 1) */
	scBezFactor( 0x138f,	 0.0764027 ),	/*	(45 2) */
	scBezFactor( 0x0163,	 0.0054315 )	/*	(45 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x8d4f,	 0.5519986 ),	/*	(46 0) */
	scBezFactor( 0x5cdc,	 0.3627419 ),	/*	(46 1) */
	scBezFactor( 0x1457,	 0.0794578 ),	/*	(46 2) */
	scBezFactor( 0x017c,	 0.0058017 )	/*	(46 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x8b4d,	 0.5441504 ),	/*	(47 0) */
	scBezFactor( 0x5dfa,	 0.3671063 ),	/*	(47 1) */
	scBezFactor( 0x1522,	 0.0825550 ),	/*	(47 2) */
	scBezFactor( 0x0195,	 0.0061883 )	/*	(47 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x8950,	 0.5363770 ),	/*	(48 0) */
	scBezFactor( 0x5f10,	 0.3713379 ),	/*	(48 1) */
	scBezFactor( 0x15f0,	 0.0856934 ),	/*	(48 2) */
	scBezFactor( 0x01b0,	 0.0065918 )	/*	(48 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x8757,	 0.5286779 ),	/*	(49 0) */
	scBezFactor( 0x601c,	 0.3754379 ),	/*	(49 1) */
	scBezFactor( 0x16c0,	 0.0888718 ),	/*	(49 2) */
	scBezFactor( 0x01cb,	 0.0070124 )	/*	(49 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x8563,	 0.5210528 ),	/*	(50 0) */
	scBezFactor( 0x6120,	 0.3794074 ),	/*	(50 1) */
	scBezFactor( 0x1793,	 0.0920892 ),	/*	(50 2) */
	scBezFactor( 0x01e8,	 0.0074506 )	/*	(50 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x8374,	 0.5135015 ),	/*	(51 0) */
	scBezFactor( 0x621c,	 0.3832474 ),	/*	(51 1) */
	scBezFactor( 0x1868,	 0.0953445 ),	/*	(51 2) */
	scBezFactor( 0x0206,	 0.0079066 )	/*	(51 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x818a,	 0.5060234 ),	/*	(52 0) */
	scBezFactor( 0x630f,	 0.3869591 ),	/*	(52 1) */
	scBezFactor( 0x1940,	 0.0986366 ),	/*	(52 2) */
	scBezFactor( 0x0225,	 0.0083809 )	/*	(52 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x7fa5,	 0.4986183 ),	/*	(53 0) */
	scBezFactor( 0x63fa,	 0.3905434 ),	/*	(53 1) */
	scBezFactor( 0x1a1a,	 0.1019645 ),	/*	(53 2) */
	scBezFactor( 0x0245,	 0.0088738 )	/*	(53 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x7dc4,	 0.4912858 ),	/*	(54 0) */
	scBezFactor( 0x64dd,	 0.3940015 ),	/*	(54 1) */
	scBezFactor( 0x1af6,	 0.1053271 ),	/*	(54 2) */
	scBezFactor( 0x0267,	 0.0093856 )	/*	(54 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x7be9,	 0.4840255 ),	/*	(55 0) */
	scBezFactor( 0x65b7,	 0.3973344 ),	/*	(55 1) */
	scBezFactor( 0x1bd5,	 0.1087233 ),	/*	(55 2) */
	scBezFactor( 0x0289,	 0.0099167 )	/*	(55 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x7a12,	 0.4768372 ),	/*	(56 0) */
	scBezFactor( 0x668a,	 0.4005432 ),	/*	(56 1) */
	scBezFactor( 0x1cb6,	 0.1121521 ),	/*	(56 2) */
	scBezFactor( 0x02ae,	 0.0104675 )	/*	(56 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x783f,	 0.4697203 ),	/*	(57 0) */
	scBezFactor( 0x6754,	 0.4036290 ),	/*	(57 1) */
	scBezFactor( 0x1d98,	 0.1156123 ),	/*	(57 2) */
	scBezFactor( 0x02d3,	 0.0110384 )	/*	(57 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x7671,	 0.4626746 ),	/*	(58 0) */
	scBezFactor( 0x6816,	 0.4065928 ),	/*	(58 1) */
	scBezFactor( 0x1e7d,	 0.1191030 ),	/*	(58 2) */
	scBezFactor( 0x02fa,	 0.0116296 )	/*	(58 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x74a8,	 0.4556997 ),	/*	(59 0) */
	scBezFactor( 0x68d0,	 0.4094358 ),	/*	(59 1) */
	scBezFactor( 0x1f64,	 0.1226229 ),	/*	(59 2) */
	scBezFactor( 0x0322,	 0.0122415 )	/*	(59 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x72e4,	 0.4487953 ),	/*	(60 0) */
	scBezFactor( 0x6983,	 0.4121590 ),	/*	(60 1) */
	scBezFactor( 0x204c,	 0.1261711 ),	/*	(60 2) */
	scBezFactor( 0x034b,	 0.0128746 )	/*	(60 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x7124,	 0.4419610 ),	/*	(61 0) */
	scBezFactor( 0x6a2d,	 0.4147634 ),	/*	(61 1) */
	scBezFactor( 0x2137,	 0.1297465 ),	/*	(61 2) */
	scBezFactor( 0x0376,	 0.0135291 )	/*	(61 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x6f69,	 0.4351964 ),	/*	(62 0) */
	scBezFactor( 0x6ad0,	 0.4172502 ),	/*	(62 1) */
	scBezFactor( 0x2223,	 0.1333480 ),	/*	(62 2) */
	scBezFactor( 0x03a2,	 0.0142055 )	/*	(62 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x6db2,	 0.4285012 ),	/*	(63 0) */
	scBezFactor( 0x6b6c,	 0.4196203 ),	/*	(63 1) */
	scBezFactor( 0x2310,	 0.1369745 ),	/*	(63 2) */
	scBezFactor( 0x03d0,	 0.0149040 )	/*	(63 3) */
},
#endif


#ifdef SubDiv4
{
	scBezFactor( 0x6c00,	 0.4218750 ),	/*	(64 0) */
	scBezFactor( 0x6c00,	 0.4218750 ),	/*	(64 1) */
	scBezFactor( 0x2400,	 0.1406250 ),	/*	(64 2) */
	scBezFactor( 0x0400,	 0.0156250 )	/*	(64 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x6a52,	 0.4153175 ),	/*	(65 0) */
	scBezFactor( 0x6c8c,	 0.4240152 ),	/*	(65 1) */
	scBezFactor( 0x24f0,	 0.1442984 ),	/*	(65 2) */
	scBezFactor( 0x0430,	 0.0163689 )	/*	(65 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x68a8,	 0.4088283 ),	/*	(66 0) */
	scBezFactor( 0x6d11,	 0.4260421 ),	/*	(66 1) */
	scBezFactor( 0x25e2,	 0.1479936 ),	/*	(66 2) */
	scBezFactor( 0x0463,	 0.0171361 )	/*	(66 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x6704,	 0.4024070 ),	/*	(67 0) */
	scBezFactor( 0x6d8e,	 0.4279566 ),	/*	(67 1) */
	scBezFactor( 0x26d6,	 0.1517095 ),	/*	(67 2) */
	scBezFactor( 0x0496,	 0.0179269 )	/*	(67 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x6563,	 0.3960533 ),	/*	(68 0) */
	scBezFactor( 0x6e04,	 0.4297600 ),	/*	(68 1) */
	scBezFactor( 0x27cb,	 0.1554451 ),	/*	(68 2) */
	scBezFactor( 0x04cc,	 0.0187416 )	/*	(68 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x63c7,	 0.3897669 ),	/*	(69 0) */
	scBezFactor( 0x6e73,	 0.4314532 ),	/*	(69 1) */
	scBezFactor( 0x28c1,	 0.1591993 ),	/*	(69 2) */
	scBezFactor( 0x0503,	 0.0195807 )	/*	(69 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x6230,	 0.3835473 ),	/*	(70 0) */
	scBezFactor( 0x6edb,	 0.4330373 ),	/*	(70 1) */
	scBezFactor( 0x29b8,	 0.1629710 ),	/*	(70 2) */
	scBezFactor( 0x053b,	 0.0204444 )	/*	(70 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x609c,	 0.3773943 ),	/*	(71 0) */
	scBezFactor( 0x6f3c,	 0.4345134 ),	/*	(71 1) */
	scBezFactor( 0x2ab0,	 0.1667592 ),	/*	(71 2) */
	scBezFactor( 0x0576,	 0.0213332 )	/*	(71 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x5f0e,	 0.3713074 ),	/*	(72 0) */
	scBezFactor( 0x6f96,	 0.4358826 ),	/*	(72 1) */
	scBezFactor( 0x2baa,	 0.1705627 ),	/*	(72 2) */
	scBezFactor( 0x05b2,	 0.0222473 )	/*	(72 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x5d83,	 0.3652863 ),	/*	(73 0) */
	scBezFactor( 0x6fe8,	 0.4371459 ),	/*	(73 1) */
	scBezFactor( 0x2ca4,	 0.1743806 ),	/*	(73 2) */
	scBezFactor( 0x05ef,	 0.0231872 )	/*	(73 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x5bfd,	 0.3593307 ),	/*	(74 0) */
	scBezFactor( 0x7034,	 0.4383044 ),	/*	(74 1) */
	scBezFactor( 0x2d9f,	 0.1782117 ),	/*	(74 2) */
	scBezFactor( 0x062e,	 0.0241532 )	/*	(74 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x5a7b,	 0.3534401 ),	/*	(75 0) */
	scBezFactor( 0x7079,	 0.4393592 ),	/*	(75 1) */
	scBezFactor( 0x2e9b,	 0.1820549 ),	/*	(75 2) */
	scBezFactor( 0x066f,	 0.0251457 )	/*	(75 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x58fd,	 0.3476143 ),	/*	(76 0) */
	scBezFactor( 0x70b8,	 0.4403114 ),	/*	(76 1) */
	scBezFactor( 0x2f97,	 0.1859093 ),	/*	(76 2) */
	scBezFactor( 0x06b2,	 0.0261650 )	/*	(76 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x5783,	 0.3418528 ),	/*	(77 0) */
	scBezFactor( 0x70ef,	 0.4411620 ),	/*	(77 1) */
	scBezFactor( 0x3095,	 0.1897736 ),	/*	(77 2) */
	scBezFactor( 0x06f7,	 0.0272115 )	/*	(77 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x560e,	 0.3361554 ),	/*	(78 0) */
	scBezFactor( 0x7121,	 0.4419122 ),	/*	(78 1) */
	scBezFactor( 0x3192,	 0.1936469 ),	/*	(78 2) */
	scBezFactor( 0x073d,	 0.0282855 )	/*	(78 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x549d,	 0.3305216 ),	/*	(79 0) */
	scBezFactor( 0x714b,	 0.4425629 ),	/*	(79 1) */
	scBezFactor( 0x3291,	 0.1975281 ),	/*	(79 2) */
	scBezFactor( 0x0785,	 0.0293874 )	/*	(79 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x5330,	 0.3249512 ),	/*	(80 0) */
	scBezFactor( 0x7170,	 0.4431152 ),	/*	(80 1) */
	scBezFactor( 0x3390,	 0.2014160 ),	/*	(80 2) */
	scBezFactor( 0x07d0,	 0.0305176 )	/*	(80 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x51c7,	 0.3194436 ),	/*	(81 0) */
	scBezFactor( 0x718d,	 0.4435703 ),	/*	(81 1) */
	scBezFactor( 0x348f,	 0.2053097 ),	/*	(81 2) */
	scBezFactor( 0x081b,	 0.0316764 )	/*	(81 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x5062,	 0.3139987 ),	/*	(82 0) */
	scBezFactor( 0x71a5,	 0.4439292 ),	/*	(82 1) */
	scBezFactor( 0x358e,	 0.2092080 ),	/*	(82 2) */
	scBezFactor( 0x0869,	 0.0328641 )	/*	(82 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x4f01,	 0.3086160 ),	/*	(83 0) */
	scBezFactor( 0x71b6,	 0.4441929 ),	/*	(83 1) */
	scBezFactor( 0x368e,	 0.2131099 ),	/*	(83 2) */
	scBezFactor( 0x08b9,	 0.0340812 )	/*	(83 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x4da4,	 0.3032951 ),	/*	(84 0) */
	scBezFactor( 0x71c1,	 0.4443626 ),	/*	(84 1) */
	scBezFactor( 0x378e,	 0.2170143 ),	/*	(84 2) */
	scBezFactor( 0x090b,	 0.0353279 )	/*	(84 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x4c4c,	 0.2980358 ),	/*	(85 0) */
	scBezFactor( 0x71c6,	 0.4444394 ),	/*	(85 1) */
	scBezFactor( 0x388e,	 0.2209201 ),	/*	(85 2) */
	scBezFactor( 0x095e,	 0.0366047 )	/*	(85 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x4af7,	 0.2928376 ),	/*	(86 0) */
	scBezFactor( 0x71c5,	 0.4444242 ),	/*	(86 1) */
	scBezFactor( 0x398e,	 0.2248263 ),	/*	(86 2) */
	scBezFactor( 0x09b4,	 0.0379119 )	/*	(86 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x49a6,	 0.2877002 ),	/*	(87 0) */
	scBezFactor( 0x71be,	 0.4443181 ),	/*	(87 1) */
	scBezFactor( 0x3a8e,	 0.2287318 ),	/*	(87 2) */
	scBezFactor( 0x0a0c,	 0.0392498 )	/*	(87 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x485a,	 0.2826233 ),	/*	(88 0) */
	scBezFactor( 0x71b2,	 0.4441223 ),	/*	(88 1) */
	scBezFactor( 0x3b8e,	 0.2326355 ),	/*	(88 2) */
	scBezFactor( 0x0a66,	 0.0406189 )	/*	(88 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x4711,	 0.2776064 ),	/*	(89 0) */
	scBezFactor( 0x719f,	 0.4438378 ),	/*	(89 1) */
	scBezFactor( 0x3c8d,	 0.2365363 ),	/*	(89 2) */
	scBezFactor( 0x0ac1,	 0.0420194 )	/*	(89 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x45cc,	 0.2726493 ),	/*	(90 0) */
	scBezFactor( 0x7186,	 0.4434657 ),	/*	(90 1) */
	scBezFactor( 0x3d8d,	 0.2404332 ),	/*	(90 2) */
	scBezFactor( 0x0b1f,	 0.0434518 )	/*	(90 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x448b,	 0.2677515 ),	/*	(91 0) */
	scBezFactor( 0x7168,	 0.4430071 ),	/*	(91 1) */
	scBezFactor( 0x3e8c,	 0.2443251 ),	/*	(91 2) */
	scBezFactor( 0x0b7f,	 0.0449163 )	/*	(91 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x434e,	 0.2629128 ),	/*	(92 0) */
	scBezFactor( 0x7145,	 0.4424629 ),	/*	(92 1) */
	scBezFactor( 0x3f8a,	 0.2482109 ),	/*	(92 2) */
	scBezFactor( 0x0be1,	 0.0464134 )	/*	(92 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x4214,	 0.2581326 ),	/*	(93 0) */
	scBezFactor( 0x711c,	 0.4418344 ),	/*	(93 1) */
	scBezFactor( 0x4088,	 0.2520896 ),	/*	(93 2) */
	scBezFactor( 0x0c46,	 0.0479434 )	/*	(93 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x40df,	 0.2534108 ),	/*	(94 0) */
	scBezFactor( 0x70ed,	 0.4411225 ),	/*	(94 1) */
	scBezFactor( 0x4186,	 0.2559600 ),	/*	(94 2) */
	scBezFactor( 0x0cac,	 0.0495067 )	/*	(94 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3fad,	 0.2487469 ),	/*	(95 0) */
	scBezFactor( 0x70b9,	 0.4403284 ),	/*	(95 1) */
	scBezFactor( 0x4283,	 0.2598211 ),	/*	(95 2) */
	scBezFactor( 0x0d15,	 0.0511035 )	/*	(95 3) */
},
#endif


#ifdef SubDiv8
{
	scBezFactor( 0x3e80,	 0.2441406 ),	/*	(96 0) */
	scBezFactor( 0x7080,	 0.4394531 ),	/*	(96 1) */
	scBezFactor( 0x4380,	 0.2636719 ),	/*	(96 2) */
	scBezFactor( 0x0d80,	 0.0527344 )	/*	(96 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3d55,	 0.2395915 ),	/*	(97 0) */
	scBezFactor( 0x7041,	 0.4384977 ),	/*	(97 1) */
	scBezFactor( 0x447b,	 0.2675112 ),	/*	(97 2) */
	scBezFactor( 0x0ded,	 0.0543995 )	/*	(97 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x3c2f,	 0.2350993 ),	/*	(98 0) */
	scBezFactor( 0x6ffd,	 0.4374633 ),	/*	(98 1) */
	scBezFactor( 0x4576,	 0.2713380 ),	/*	(98 2) */
	scBezFactor( 0x0e5c,	 0.0560994 )	/*	(98 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3b0c,	 0.2306636 ),	/*	(99 0) */
	scBezFactor( 0x6fb4,	 0.4363509 ),	/*	(99 1) */
	scBezFactor( 0x4670,	 0.2751512 ),	/*	(99 2) */
	scBezFactor( 0x0ece,	 0.0578343 )	/*	(99 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x39ed,	 0.2262840 ),	/*	(100 0) */
	scBezFactor( 0x6f66,	 0.4351616 ),	/*	(100 1) */
	scBezFactor( 0x4769,	 0.2789497 ),	/*	(100 2) */
	scBezFactor( 0x0f42,	 0.0596046 )	/*	(100 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x38d2,	 0.2219602 ),	/*	(101 0) */
	scBezFactor( 0x6f13,	 0.4338965 ),	/*	(101 1) */
	scBezFactor( 0x4861,	 0.2827325 ),	/*	(101 2) */
	scBezFactor( 0x0fb8,	 0.0614107 )	/*	(101 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x37ba,	 0.2176919 ),	/*	(102 0) */
	scBezFactor( 0x6ebc,	 0.4325566 ),	/*	(102 1) */
	scBezFactor( 0x4957,	 0.2864985 ),	/*	(102 2) */
	scBezFactor( 0x1031,	 0.0632529 )	/*	(102 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x36a6,	 0.2134786 ),	/*	(103 0) */
	scBezFactor( 0x6e5f,	 0.4311431 ),	/*	(103 1) */
	scBezFactor( 0x4a4d,	 0.2902467 ),	/*	(103 2) */
	scBezFactor( 0x10ac,	 0.0651316 )	/*	(103 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x3596,	 0.2093201 ),	/*	(104 0) */
	scBezFactor( 0x6dfe,	 0.4296570 ),	/*	(104 1) */
	scBezFactor( 0x4b42,	 0.2939758 ),	/*	(104 2) */
	scBezFactor( 0x112a,	 0.0670471 )	/*	(104 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3489,	 0.2052159 ),	/*	(105 0) */
	scBezFactor( 0x6d97,	 0.4280993 ),	/*	(105 1) */
	scBezFactor( 0x4c35,	 0.2976850 ),	/*	(105 2) */
	scBezFactor( 0x11a9,	 0.0689998 )	/*	(105 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x337f,	 0.2011657 ),	/*	(106 0) */
	scBezFactor( 0x6d2d,	 0.4264712 ),	/*	(106 1) */
	scBezFactor( 0x4d26,	 0.3013730 ),	/*	(106 2) */
	scBezFactor( 0x122c,	 0.0709901 )	/*	(106 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3279,	 0.1971691 ),	/*	(107 0) */
	scBezFactor( 0x6cbd,	 0.4247738 ),	/*	(107 1) */
	scBezFactor( 0x4e17,	 0.3050389 ),	/*	(107 2) */
	scBezFactor( 0x12b1,	 0.0730183 )	/*	(107 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x3177,	 0.1932259 ),	/*	(108 0) */
	scBezFactor( 0x6c4a,	 0.4230080 ),	/*	(108 1) */
	scBezFactor( 0x4f05,	 0.3086815 ),	/*	(108 2) */
	scBezFactor( 0x1338,	 0.0750847 )	/*	(108 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x3078,	 0.1893355 ),	/*	(109 0) */
	scBezFactor( 0x6bd2,	 0.4211749 ),	/*	(109 1) */
	scBezFactor( 0x4ff2,	 0.3122998 ),	/*	(109 2) */
	scBezFactor( 0x13c2,	 0.0771897 )	/*	(109 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x2f7c,	 0.1854978 ),	/*	(110 0) */
	scBezFactor( 0x6b55,	 0.4192758 ),	/*	(110 1) */
	scBezFactor( 0x50de,	 0.3158927 ),	/*	(110 2) */
	scBezFactor( 0x144f,	 0.0793338 )	/*	(110 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x2e84,	 0.1817122 ),	/*	(111 0) */
	scBezFactor( 0x6ad4,	 0.4173115 ),	/*	(111 1) */
	scBezFactor( 0x51c8,	 0.3194591 ),	/*	(111 2) */
	scBezFactor( 0x14de,	 0.0815172 )	/*	(111 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x2d90,	 0.1779785 ),	/*	(112 0) */
	scBezFactor( 0x6a50,	 0.4152832 ),	/*	(112 1) */
	scBezFactor( 0x52b0,	 0.3229980 ),	/*	(112 2) */
	scBezFactor( 0x1570,	 0.0837402 )	/*	(112 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x2c9e,	 0.1742963 ),	/*	(113 0) */
	scBezFactor( 0x69c6,	 0.4131920 ),	/*	(113 1) */
	scBezFactor( 0x5396,	 0.3265083 ),	/*	(113 2) */
	scBezFactor( 0x1604,	 0.0860034 )	/*	(113 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x2bb0,	 0.1706653 ),	/*	(114 0) */
	scBezFactor( 0x6939,	 0.4110389 ),	/*	(114 1) */
	scBezFactor( 0x547a,	 0.3299890 ),	/*	(114 2) */
	scBezFactor( 0x169b,	 0.0883069 )	/*	(114 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x2ac6,	 0.1670850 ),	/*	(115 0) */
	scBezFactor( 0x68a8,	 0.4088250 ),	/*	(115 1) */
	scBezFactor( 0x555c,	 0.3334388 ),	/*	(115 2) */
	scBezFactor( 0x1734,	 0.0906512 )	/*	(115 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x29de,	 0.1635551 ),	/*	(116 0) */
	scBezFactor( 0x6813,	 0.4065514 ),	/*	(116 1) */
	scBezFactor( 0x563c,	 0.3368568 ),	/*	(116 2) */
	scBezFactor( 0x17d1,	 0.0930367 )	/*	(116 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x28fa,	 0.1600754 ),	/*	(117 0) */
	scBezFactor( 0x677a,	 0.4042191 ),	/*	(117 1) */
	scBezFactor( 0x571a,	 0.3402420 ),	/*	(117 2) */
	scBezFactor( 0x1870,	 0.0954636 )	/*	(117 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x2819,	 0.1566453 ),	/*	(118 0) */
	scBezFactor( 0x66de,	 0.4018292 ),	/*	(118 1) */
	scBezFactor( 0x57f5,	 0.3435931 ),	/*	(118 2) */
	scBezFactor( 0x1912,	 0.0979323 )	/*	(118 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x273c,	 0.1532646 ),	/*	(119 0) */
	scBezFactor( 0x663d,	 0.3993829 ),	/*	(119 1) */
	scBezFactor( 0x58cf,	 0.3469092 ),	/*	(119 2) */
	scBezFactor( 0x19b6,	 0.1004433 )	/*	(119 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x2662,	 0.1499329 ),	/*	(120 0) */
	scBezFactor( 0x659a,	 0.3968811 ),	/*	(120 1) */
	scBezFactor( 0x59a6,	 0.3501892 ),	/*	(120 2) */
	scBezFactor( 0x1a5e,	 0.1029968 )	/*	(120 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x258a,	 0.1466498 ),	/*	(121 0) */
	scBezFactor( 0x64f2,	 0.3943250 ),	/*	(121 1) */
	scBezFactor( 0x5a7a,	 0.3534320 ),	/*	(121 2) */
	scBezFactor( 0x1b08,	 0.1055933 )	/*	(121 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x24b6,	 0.1434150 ),	/*	(122 0) */
	scBezFactor( 0x6447,	 0.3917155 ),	/*	(122 1) */
	scBezFactor( 0x5b4c,	 0.3566365 ),	/*	(122 2) */
	scBezFactor( 0x1bb5,	 0.1082330 )	/*	(122 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x23e5,	 0.1402281 ),	/*	(123 0) */
	scBezFactor( 0x6399,	 0.3890539 ),	/*	(123 1) */
	scBezFactor( 0x5c1b,	 0.3598017 ),	/*	(123 2) */
	scBezFactor( 0x1c65,	 0.1109163 )	/*	(123 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x2318,	 0.1370888 ),	/*	(124 0) */
	scBezFactor( 0x62e7,	 0.3863411 ),	/*	(124 1) */
	scBezFactor( 0x5ce8,	 0.3629265 ),	/*	(124 2) */
	scBezFactor( 0x1d17,	 0.1136436 )	/*	(124 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x224d,	 0.1339967 ),	/*	(125 0) */
	scBezFactor( 0x6232,	 0.3835782 ),	/*	(125 1) */
	scBezFactor( 0x5db2,	 0.3660098 ),	/*	(125 2) */
	scBezFactor( 0x1dcd,	 0.1164153 )	/*	(125 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x2186,	 0.1309514 ),	/*	(126 0) */
	scBezFactor( 0x6179,	 0.3807664 ),	/*	(126 1) */
	scBezFactor( 0x5e7a,	 0.3690505 ),	/*	(126 2) */
	scBezFactor( 0x1e85,	 0.1192317 )	/*	(126 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x20c1,	 0.1279526 ),	/*	(127 0) */
	scBezFactor( 0x60be,	 0.3779066 ),	/*	(127 1) */
	scBezFactor( 0x5f3e,	 0.3720476 ),	/*	(127 2) */
	scBezFactor( 0x1f41,	 0.1220931 )	/*	(127 3) */
},
#endif


#ifdef SubDiv2
{
	scBezFactor( 0x2000,	 0.1250000 ),	/*	(128 0) */
	scBezFactor( 0x6000,	 0.3750000 ),	/*	(128 1) */
	scBezFactor( 0x6000,	 0.3750000 ),	/*	(128 2) */
	scBezFactor( 0x2000,	 0.1250000 )	/*	(128 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1f41,	 0.1220931 ),	/*	(129 0) */
	scBezFactor( 0x5f3e,	 0.3720476 ),	/*	(129 1) */
	scBezFactor( 0x60be,	 0.3779066 ),	/*	(129 2) */
	scBezFactor( 0x20c1,	 0.1279526 )	/*	(129 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x1e85,	 0.1192317 ),	/*	(130 0) */
	scBezFactor( 0x5e7a,	 0.3690505 ),	/*	(130 1) */
	scBezFactor( 0x6179,	 0.3807664 ),	/*	(130 2) */
	scBezFactor( 0x2186,	 0.1309514 )	/*	(130 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1dcd,	 0.1164153 ),	/*	(131 0) */
	scBezFactor( 0x5db2,	 0.3660098 ),	/*	(131 1) */
	scBezFactor( 0x6232,	 0.3835782 ),	/*	(131 2) */
	scBezFactor( 0x224d,	 0.1339967 )	/*	(131 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x1d17,	 0.1136436 ),	/*	(132 0) */
	scBezFactor( 0x5ce8,	 0.3629265 ),	/*	(132 1) */
	scBezFactor( 0x62e7,	 0.3863411 ),	/*	(132 2) */
	scBezFactor( 0x2318,	 0.1370888 )	/*	(132 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1c65,	 0.1109163 ),	/*	(133 0) */
	scBezFactor( 0x5c1b,	 0.3598017 ),	/*	(133 1) */
	scBezFactor( 0x6399,	 0.3890539 ),	/*	(133 2) */
	scBezFactor( 0x23e5,	 0.1402281 )	/*	(133 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x1bb5,	 0.1082330 ),	/*	(134 0) */
	scBezFactor( 0x5b4c,	 0.3566365 ),	/*	(134 1) */
	scBezFactor( 0x6447,	 0.3917155 ),	/*	(134 2) */
	scBezFactor( 0x24b6,	 0.1434150 )	/*	(134 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1b08,	 0.1055933 ),	/*	(135 0) */
	scBezFactor( 0x5a7a,	 0.3534320 ),	/*	(135 1) */
	scBezFactor( 0x64f2,	 0.3943250 ),	/*	(135 2) */
	scBezFactor( 0x258a,	 0.1466498 )	/*	(135 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x1a5e,	 0.1029968 ),	/*	(136 0) */
	scBezFactor( 0x59a6,	 0.3501892 ),	/*	(136 1) */
	scBezFactor( 0x659a,	 0.3968811 ),	/*	(136 2) */
	scBezFactor( 0x2662,	 0.1499329 )	/*	(136 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x19b6,	 0.1004433 ),	/*	(137 0) */
	scBezFactor( 0x58cf,	 0.3469092 ),	/*	(137 1) */
	scBezFactor( 0x663d,	 0.3993829 ),	/*	(137 2) */
	scBezFactor( 0x273c,	 0.1532646 )	/*	(137 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x1912,	 0.0979323 ),	/*	(138 0) */
	scBezFactor( 0x57f5,	 0.3435931 ),	/*	(138 1) */
	scBezFactor( 0x66de,	 0.4018292 ),	/*	(138 2) */
	scBezFactor( 0x2819,	 0.1566453 )	/*	(138 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1870,	 0.0954636 ),	/*	(139 0) */
	scBezFactor( 0x571a,	 0.3402420 ),	/*	(139 1) */
	scBezFactor( 0x677a,	 0.4042191 ),	/*	(139 2) */
	scBezFactor( 0x28fa,	 0.1600754 )	/*	(139 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x17d1,	 0.0930367 ),	/*	(140 0) */
	scBezFactor( 0x563c,	 0.3368568 ),	/*	(140 1) */
	scBezFactor( 0x6813,	 0.4065514 ),	/*	(140 2) */
	scBezFactor( 0x29de,	 0.1635551 )	/*	(140 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1734,	 0.0906512 ),	/*	(141 0) */
	scBezFactor( 0x555c,	 0.3334388 ),	/*	(141 1) */
	scBezFactor( 0x68a8,	 0.4088250 ),	/*	(141 2) */
	scBezFactor( 0x2ac6,	 0.1670850 )	/*	(141 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x169b,	 0.0883069 ),	/*	(142 0) */
	scBezFactor( 0x547a,	 0.3299890 ),	/*	(142 1) */
	scBezFactor( 0x6939,	 0.4110389 ),	/*	(142 2) */
	scBezFactor( 0x2bb0,	 0.1706653 )	/*	(142 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x1604,	 0.0860034 ),	/*	(143 0) */
	scBezFactor( 0x5396,	 0.3265083 ),	/*	(143 1) */
	scBezFactor( 0x69c6,	 0.4131920 ),	/*	(143 2) */
	scBezFactor( 0x2c9e,	 0.1742963 )	/*	(143 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x1570,	 0.0837402 ),	/*	(144 0) */
	scBezFactor( 0x52b0,	 0.3229980 ),	/*	(144 1) */
	scBezFactor( 0x6a50,	 0.4152832 ),	/*	(144 2) */
	scBezFactor( 0x2d90,	 0.1779785 )	/*	(144 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x14de,	 0.0815172 ),	/*	(145 0) */
	scBezFactor( 0x51c8,	 0.3194591 ),	/*	(145 1) */
	scBezFactor( 0x6ad4,	 0.4173115 ),	/*	(145 2) */
	scBezFactor( 0x2e84,	 0.1817122 )	/*	(145 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x144f,	 0.0793338 ),	/*	(146 0) */
	scBezFactor( 0x50de,	 0.3158927 ),	/*	(146 1) */
	scBezFactor( 0x6b55,	 0.4192758 ),	/*	(146 2) */
	scBezFactor( 0x2f7c,	 0.1854978 )	/*	(146 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x13c2,	 0.0771897 ),	/*	(147 0) */
	scBezFactor( 0x4ff2,	 0.3122998 ),	/*	(147 1) */
	scBezFactor( 0x6bd2,	 0.4211749 ),	/*	(147 2) */
	scBezFactor( 0x3078,	 0.1893355 )	/*	(147 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x1338,	 0.0750847 ),	/*	(148 0) */
	scBezFactor( 0x4f05,	 0.3086815 ),	/*	(148 1) */
	scBezFactor( 0x6c4a,	 0.4230080 ),	/*	(148 2) */
	scBezFactor( 0x3177,	 0.1932259 )	/*	(148 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x12b1,	 0.0730183 ),	/*	(149 0) */
	scBezFactor( 0x4e17,	 0.3050389 ),	/*	(149 1) */
	scBezFactor( 0x6cbd,	 0.4247738 ),	/*	(149 2) */
	scBezFactor( 0x3279,	 0.1971691 )	/*	(149 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x122c,	 0.0709901 ),	/*	(150 0) */
	scBezFactor( 0x4d26,	 0.3013730 ),	/*	(150 1) */
	scBezFactor( 0x6d2d,	 0.4264712 ),	/*	(150 2) */
	scBezFactor( 0x337f,	 0.2011657 )	/*	(150 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x11a9,	 0.0689998 ),	/*	(151 0) */
	scBezFactor( 0x4c35,	 0.2976850 ),	/*	(151 1) */
	scBezFactor( 0x6d97,	 0.4280993 ),	/*	(151 2) */
	scBezFactor( 0x3489,	 0.2052159 )	/*	(151 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x112a,	 0.0670471 ),	/*	(152 0) */
	scBezFactor( 0x4b42,	 0.2939758 ),	/*	(152 1) */
	scBezFactor( 0x6dfe,	 0.4296570 ),	/*	(152 2) */
	scBezFactor( 0x3596,	 0.2093201 )	/*	(152 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x10ac,	 0.0651316 ),	/*	(153 0) */
	scBezFactor( 0x4a4d,	 0.2902467 ),	/*	(153 1) */
	scBezFactor( 0x6e5f,	 0.4311431 ),	/*	(153 2) */
	scBezFactor( 0x36a6,	 0.2134786 )	/*	(153 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x1031,	 0.0632529 ),	/*	(154 0) */
	scBezFactor( 0x4957,	 0.2864985 ),	/*	(154 1) */
	scBezFactor( 0x6ebc,	 0.4325566 ),	/*	(154 2) */
	scBezFactor( 0x37ba,	 0.2176919 )	/*	(154 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0fb8,	 0.0614107 ),	/*	(155 0) */
	scBezFactor( 0x4861,	 0.2827325 ),	/*	(155 1) */
	scBezFactor( 0x6f13,	 0.4338965 ),	/*	(155 2) */
	scBezFactor( 0x38d2,	 0.2219602 )	/*	(155 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0f42,	 0.0596046 ),	/*	(156 0) */
	scBezFactor( 0x4769,	 0.2789497 ),	/*	(156 1) */
	scBezFactor( 0x6f66,	 0.4351616 ),	/*	(156 2) */
	scBezFactor( 0x39ed,	 0.2262840 )	/*	(156 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0ece,	 0.0578343 ),	/*	(157 0) */
	scBezFactor( 0x4670,	 0.2751512 ),	/*	(157 1) */
	scBezFactor( 0x6fb4,	 0.4363509 ),	/*	(157 2) */
	scBezFactor( 0x3b0c,	 0.2306636 )	/*	(157 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0e5c,	 0.0560994 ),	/*	(158 0) */
	scBezFactor( 0x4576,	 0.2713380 ),	/*	(158 1) */
	scBezFactor( 0x6ffd,	 0.4374633 ),	/*	(158 2) */
	scBezFactor( 0x3c2f,	 0.2350993 )	/*	(158 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0ded,	 0.0543995 ),	/*	(159 0) */
	scBezFactor( 0x447b,	 0.2675112 ),	/*	(159 1) */
	scBezFactor( 0x7041,	 0.4384977 ),	/*	(159 2) */
	scBezFactor( 0x3d55,	 0.2395915 )	/*	(159 3) */
},
#endif


#ifdef SubDiv8
{
	scBezFactor( 0x0d80,	 0.0527344 ),	/*	(160 0) */
	scBezFactor( 0x4380,	 0.2636719 ),	/*	(160 1) */
	scBezFactor( 0x7080,	 0.4394531 ),	/*	(160 2) */
	scBezFactor( 0x3e80,	 0.2441406 )	/*	(160 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0d15,	 0.0511035 ),	/*	(161 0) */
	scBezFactor( 0x4283,	 0.2598211 ),	/*	(161 1) */
	scBezFactor( 0x70b9,	 0.4403284 ),	/*	(161 2) */
	scBezFactor( 0x3fad,	 0.2487469 )	/*	(161 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0cac,	 0.0495067 ),	/*	(162 0) */
	scBezFactor( 0x4186,	 0.2559600 ),	/*	(162 1) */
	scBezFactor( 0x70ed,	 0.4411225 ),	/*	(162 2) */
	scBezFactor( 0x40df,	 0.2534108 )	/*	(162 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0c46,	 0.0479434 ),	/*	(163 0) */
	scBezFactor( 0x4088,	 0.2520896 ),	/*	(163 1) */
	scBezFactor( 0x711c,	 0.4418344 ),	/*	(163 2) */
	scBezFactor( 0x4214,	 0.2581326 )	/*	(163 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0be1,	 0.0464134 ),	/*	(164 0) */
	scBezFactor( 0x3f8a,	 0.2482109 ),	/*	(164 1) */
	scBezFactor( 0x7145,	 0.4424629 ),	/*	(164 2) */
	scBezFactor( 0x434e,	 0.2629128 )	/*	(164 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0b7f,	 0.0449163 ),	/*	(165 0) */
	scBezFactor( 0x3e8c,	 0.2443251 ),	/*	(165 1) */
	scBezFactor( 0x7168,	 0.4430071 ),	/*	(165 2) */
	scBezFactor( 0x448b,	 0.2677515 )	/*	(165 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0b1f,	 0.0434518 ),	/*	(166 0) */
	scBezFactor( 0x3d8d,	 0.2404332 ),	/*	(166 1) */
	scBezFactor( 0x7186,	 0.4434657 ),	/*	(166 2) */
	scBezFactor( 0x45cc,	 0.2726493 )	/*	(166 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0ac1,	 0.0420194 ),	/*	(167 0) */
	scBezFactor( 0x3c8d,	 0.2365363 ),	/*	(167 1) */
	scBezFactor( 0x719f,	 0.4438378 ),	/*	(167 2) */
	scBezFactor( 0x4711,	 0.2776064 )	/*	(167 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x0a66,	 0.0406189 ),	/*	(168 0) */
	scBezFactor( 0x3b8e,	 0.2326355 ),	/*	(168 1) */
	scBezFactor( 0x71b2,	 0.4441223 ),	/*	(168 2) */
	scBezFactor( 0x485a,	 0.2826233 )	/*	(168 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0a0c,	 0.0392498 ),	/*	(169 0) */
	scBezFactor( 0x3a8e,	 0.2287318 ),	/*	(169 1) */
	scBezFactor( 0x71be,	 0.4443181 ),	/*	(169 2) */
	scBezFactor( 0x49a6,	 0.2877002 )	/*	(169 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x09b4,	 0.0379119 ),	/*	(170 0) */
	scBezFactor( 0x398e,	 0.2248263 ),	/*	(170 1) */
	scBezFactor( 0x71c5,	 0.4444242 ),	/*	(170 2) */
	scBezFactor( 0x4af7,	 0.2928376 )	/*	(170 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x095e,	 0.0366047 ),	/*	(171 0) */
	scBezFactor( 0x388e,	 0.2209201 ),	/*	(171 1) */
	scBezFactor( 0x71c6,	 0.4444394 ),	/*	(171 2) */
	scBezFactor( 0x4c4c,	 0.2980358 )	/*	(171 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x090b,	 0.0353279 ),	/*	(172 0) */
	scBezFactor( 0x378e,	 0.2170143 ),	/*	(172 1) */
	scBezFactor( 0x71c1,	 0.4443626 ),	/*	(172 2) */
	scBezFactor( 0x4da4,	 0.3032951 )	/*	(172 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x08b9,	 0.0340812 ),	/*	(173 0) */
	scBezFactor( 0x368e,	 0.2131099 ),	/*	(173 1) */
	scBezFactor( 0x71b6,	 0.4441929 ),	/*	(173 2) */
	scBezFactor( 0x4f01,	 0.3086160 )	/*	(173 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0869,	 0.0328641 ),	/*	(174 0) */
	scBezFactor( 0x358e,	 0.2092080 ),	/*	(174 1) */
	scBezFactor( 0x71a5,	 0.4439292 ),	/*	(174 2) */
	scBezFactor( 0x5062,	 0.3139987 )	/*	(174 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x081b,	 0.0316764 ),	/*	(175 0) */
	scBezFactor( 0x348f,	 0.2053097 ),	/*	(175 1) */
	scBezFactor( 0x718d,	 0.4435703 ),	/*	(175 2) */
	scBezFactor( 0x51c7,	 0.3194436 )	/*	(175 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x07d0,	 0.0305176 ),	/*	(176 0) */
	scBezFactor( 0x3390,	 0.2014160 ),	/*	(176 1) */
	scBezFactor( 0x7170,	 0.4431152 ),	/*	(176 2) */
	scBezFactor( 0x5330,	 0.3249512 )	/*	(176 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0785,	 0.0293874 ),	/*	(177 0) */
	scBezFactor( 0x3291,	 0.1975281 ),	/*	(177 1) */
	scBezFactor( 0x714b,	 0.4425629 ),	/*	(177 2) */
	scBezFactor( 0x549d,	 0.3305216 )	/*	(177 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x073d,	 0.0282855 ),	/*	(178 0) */
	scBezFactor( 0x3192,	 0.1936469 ),	/*	(178 1) */
	scBezFactor( 0x7121,	 0.4419122 ),	/*	(178 2) */
	scBezFactor( 0x560e,	 0.3361554 )	/*	(178 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x06f7,	 0.0272115 ),	/*	(179 0) */
	scBezFactor( 0x3095,	 0.1897736 ),	/*	(179 1) */
	scBezFactor( 0x70ef,	 0.4411620 ),	/*	(179 2) */
	scBezFactor( 0x5783,	 0.3418528 )	/*	(179 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x06b2,	 0.0261650 ),	/*	(180 0) */
	scBezFactor( 0x2f97,	 0.1859093 ),	/*	(180 1) */
	scBezFactor( 0x70b8,	 0.4403114 ),	/*	(180 2) */
	scBezFactor( 0x58fd,	 0.3476143 )	/*	(180 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x066f,	 0.0251457 ),	/*	(181 0) */
	scBezFactor( 0x2e9b,	 0.1820549 ),	/*	(181 1) */
	scBezFactor( 0x7079,	 0.4393592 ),	/*	(181 2) */
	scBezFactor( 0x5a7b,	 0.3534401 )	/*	(181 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x062e,	 0.0241532 ),	/*	(182 0) */
	scBezFactor( 0x2d9f,	 0.1782117 ),	/*	(182 1) */
	scBezFactor( 0x7034,	 0.4383044 ),	/*	(182 2) */
	scBezFactor( 0x5bfd,	 0.3593307 )	/*	(182 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x05ef,	 0.0231872 ),	/*	(183 0) */
	scBezFactor( 0x2ca4,	 0.1743806 ),	/*	(183 1) */
	scBezFactor( 0x6fe8,	 0.4371459 ),	/*	(183 2) */
	scBezFactor( 0x5d83,	 0.3652863 )	/*	(183 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x05b2,	 0.0222473 ),	/*	(184 0) */
	scBezFactor( 0x2baa,	 0.1705627 ),	/*	(184 1) */
	scBezFactor( 0x6f96,	 0.4358826 ),	/*	(184 2) */
	scBezFactor( 0x5f0e,	 0.3713074 )	/*	(184 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0576,	 0.0213332 ),	/*	(185 0) */
	scBezFactor( 0x2ab0,	 0.1667592 ),	/*	(185 1) */
	scBezFactor( 0x6f3c,	 0.4345134 ),	/*	(185 2) */
	scBezFactor( 0x609c,	 0.3773943 )	/*	(185 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x053b,	 0.0204444 ),	/*	(186 0) */
	scBezFactor( 0x29b8,	 0.1629710 ),	/*	(186 1) */
	scBezFactor( 0x6edb,	 0.4330373 ),	/*	(186 2) */
	scBezFactor( 0x6230,	 0.3835473 )	/*	(186 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0503,	 0.0195807 ),	/*	(187 0) */
	scBezFactor( 0x28c1,	 0.1591993 ),	/*	(187 1) */
	scBezFactor( 0x6e73,	 0.4314532 ),	/*	(187 2) */
	scBezFactor( 0x63c7,	 0.3897669 )	/*	(187 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x04cc,	 0.0187416 ),	/*	(188 0) */
	scBezFactor( 0x27cb,	 0.1554451 ),	/*	(188 1) */
	scBezFactor( 0x6e04,	 0.4297600 ),	/*	(188 2) */
	scBezFactor( 0x6563,	 0.3960533 )	/*	(188 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0496,	 0.0179269 ),	/*	(189 0) */
	scBezFactor( 0x26d6,	 0.1517095 ),	/*	(189 1) */
	scBezFactor( 0x6d8e,	 0.4279566 ),	/*	(189 2) */
	scBezFactor( 0x6704,	 0.4024070 )	/*	(189 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0463,	 0.0171361 ),	/*	(190 0) */
	scBezFactor( 0x25e2,	 0.1479936 ),	/*	(190 1) */
	scBezFactor( 0x6d11,	 0.4260421 ),	/*	(190 2) */
	scBezFactor( 0x68a8,	 0.4088283 )	/*	(190 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0430,	 0.0163689 ),	/*	(191 0) */
	scBezFactor( 0x24f0,	 0.1442984 ),	/*	(191 1) */
	scBezFactor( 0x6c8c,	 0.4240152 ),	/*	(191 2) */
	scBezFactor( 0x6a52,	 0.4153175 )	/*	(191 3) */
},
#endif


#ifdef SubDiv4
{
	scBezFactor( 0x0400,	 0.0156250 ),	/*	(192 0) */
	scBezFactor( 0x2400,	 0.1406250 ),	/*	(192 1) */
	scBezFactor( 0x6c00,	 0.4218750 ),	/*	(192 2) */
	scBezFactor( 0x6c00,	 0.4218750 )	/*	(192 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x03d0,	 0.0149040 ),	/*	(193 0) */
	scBezFactor( 0x2310,	 0.1369745 ),	/*	(193 1) */
	scBezFactor( 0x6b6c,	 0.4196203 ),	/*	(193 2) */
	scBezFactor( 0x6db2,	 0.4285012 )	/*	(193 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x03a2,	 0.0142055 ),	/*	(194 0) */
	scBezFactor( 0x2223,	 0.1333480 ),	/*	(194 1) */
	scBezFactor( 0x6ad0,	 0.4172502 ),	/*	(194 2) */
	scBezFactor( 0x6f69,	 0.4351964 )	/*	(194 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0376,	 0.0135291 ),	/*	(195 0) */
	scBezFactor( 0x2137,	 0.1297465 ),	/*	(195 1) */
	scBezFactor( 0x6a2d,	 0.4147634 ),	/*	(195 2) */
	scBezFactor( 0x7124,	 0.4419610 )	/*	(195 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x034b,	 0.0128746 ),	/*	(196 0) */
	scBezFactor( 0x204c,	 0.1261711 ),	/*	(196 1) */
	scBezFactor( 0x6983,	 0.4121590 ),	/*	(196 2) */
	scBezFactor( 0x72e4,	 0.4487953 )	/*	(196 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0322,	 0.0122415 ),	/*	(197 0) */
	scBezFactor( 0x1f64,	 0.1226229 ),	/*	(197 1) */
	scBezFactor( 0x68d0,	 0.4094358 ),	/*	(197 2) */
	scBezFactor( 0x74a8,	 0.4556997 )	/*	(197 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x02fa,	 0.0116296 ),	/*	(198 0) */
	scBezFactor( 0x1e7d,	 0.1191030 ),	/*	(198 1) */
	scBezFactor( 0x6816,	 0.4065928 ),	/*	(198 2) */
	scBezFactor( 0x7671,	 0.4626746 )	/*	(198 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x02d3,	 0.0110384 ),	/*	(199 0) */
	scBezFactor( 0x1d98,	 0.1156123 ),	/*	(199 1) */
	scBezFactor( 0x6754,	 0.4036290 ),	/*	(199 2) */
	scBezFactor( 0x783f,	 0.4697203 )	/*	(199 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x02ae,	 0.0104675 ),	/*	(200 0) */
	scBezFactor( 0x1cb6,	 0.1121521 ),	/*	(200 1) */
	scBezFactor( 0x668a,	 0.4005432 ),	/*	(200 2) */
	scBezFactor( 0x7a12,	 0.4768372 )	/*	(200 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0289,	 0.0099167 ),	/*	(201 0) */
	scBezFactor( 0x1bd5,	 0.1087233 ),	/*	(201 1) */
	scBezFactor( 0x65b7,	 0.3973344 ),	/*	(201 2) */
	scBezFactor( 0x7be9,	 0.4840255 )	/*	(201 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0267,	 0.0093856 ),	/*	(202 0) */
	scBezFactor( 0x1af6,	 0.1053271 ),	/*	(202 1) */
	scBezFactor( 0x64dd,	 0.3940015 ),	/*	(202 2) */
	scBezFactor( 0x7dc4,	 0.4912858 )	/*	(202 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0245,	 0.0088738 ),	/*	(203 0) */
	scBezFactor( 0x1a1a,	 0.1019645 ),	/*	(203 1) */
	scBezFactor( 0x63fa,	 0.3905434 ),	/*	(203 2) */
	scBezFactor( 0x7fa5,	 0.4986183 )	/*	(203 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0225,	 0.0083809 ),	/*	(204 0) */
	scBezFactor( 0x1940,	 0.0986366 ),	/*	(204 1) */
	scBezFactor( 0x630f,	 0.3869591 ),	/*	(204 2) */
	scBezFactor( 0x818a,	 0.5060234 )	/*	(204 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0206,	 0.0079066 ),	/*	(205 0) */
	scBezFactor( 0x1868,	 0.0953445 ),	/*	(205 1) */
	scBezFactor( 0x621c,	 0.3832474 ),	/*	(205 2) */
	scBezFactor( 0x8374,	 0.5135015 )	/*	(205 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x01e8,	 0.0074506 ),	/*	(206 0) */
	scBezFactor( 0x1793,	 0.0920892 ),	/*	(206 1) */
	scBezFactor( 0x6120,	 0.3794074 ),	/*	(206 2) */
	scBezFactor( 0x8563,	 0.5210528 )	/*	(206 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x01cb,	 0.0070124 ),	/*	(207 0) */
	scBezFactor( 0x16c0,	 0.0888718 ),	/*	(207 1) */
	scBezFactor( 0x601c,	 0.3754379 ),	/*	(207 2) */
	scBezFactor( 0x8757,	 0.5286779 )	/*	(207 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x01b0,	 0.0065918 ),	/*	(208 0) */
	scBezFactor( 0x15f0,	 0.0856934 ),	/*	(208 1) */
	scBezFactor( 0x5f10,	 0.3713379 ),	/*	(208 2) */
	scBezFactor( 0x8950,	 0.5363770 )	/*	(208 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0195,	 0.0061883 ),	/*	(209 0) */
	scBezFactor( 0x1522,	 0.0825550 ),	/*	(209 1) */
	scBezFactor( 0x5dfa,	 0.3671063 ),	/*	(209 2) */
	scBezFactor( 0x8b4d,	 0.5441504 )	/*	(209 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x017c,	 0.0058017 ),	/*	(210 0) */
	scBezFactor( 0x1457,	 0.0794578 ),	/*	(210 1) */
	scBezFactor( 0x5cdc,	 0.3627419 ),	/*	(210 2) */
	scBezFactor( 0x8d4f,	 0.5519986 )	/*	(210 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0163,	 0.0054315 ),	/*	(211 0) */
	scBezFactor( 0x138f,	 0.0764027 ),	/*	(211 1) */
	scBezFactor( 0x5bb5,	 0.3582439 ),	/*	(211 2) */
	scBezFactor( 0x8f57,	 0.5599219 )	/*	(211 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x014c,	 0.0050774 ),	/*	(212 0) */
	scBezFactor( 0x12c9,	 0.0733910 ),	/*	(212 1) */
	scBezFactor( 0x5a86,	 0.3536110 ),	/*	(212 2) */
	scBezFactor( 0x9163,	 0.5679207 )	/*	(212 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0136,	 0.0047390 ),	/*	(213 0) */
	scBezFactor( 0x1207,	 0.0704235 ),	/*	(213 1) */
	scBezFactor( 0x594d,	 0.3488422 ),	/*	(213 2) */
	scBezFactor( 0x9374,	 0.5759953 )	/*	(213 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0121,	 0.0044160 ),	/*	(214 0) */
	scBezFactor( 0x1147,	 0.0675015 ),	/*	(214 1) */
	scBezFactor( 0x580c,	 0.3439364 ),	/*	(214 2) */
	scBezFactor( 0x958a,	 0.5841460 )	/*	(214 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x010d,	 0.0041080 ),	/*	(215 0) */
	scBezFactor( 0x108b,	 0.0646260 ),	/*	(215 1) */
	scBezFactor( 0x56c1,	 0.3388926 ),	/*	(215 2) */
	scBezFactor( 0x97a5,	 0.5923733 )	/*	(215 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x00fa,	 0.0038147 ),	/*	(216 0) */
	scBezFactor( 0x0fd2,	 0.0617981 ),	/*	(216 1) */
	scBezFactor( 0x556e,	 0.3337097 ),	/*	(216 2) */
	scBezFactor( 0x99c6,	 0.6006775 )	/*	(216 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x00e7,	 0.0035357 ),	/*	(217 0) */
	scBezFactor( 0x0f1b,	 0.0590188 ),	/*	(217 1) */
	scBezFactor( 0x5411,	 0.3283866 ),	/*	(217 2) */
	scBezFactor( 0x9beb,	 0.6090589 )	/*	(217 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x00d6,	 0.0032706 ),	/*	(218 0) */
	scBezFactor( 0x0e68,	 0.0562892 ),	/*	(218 1) */
	scBezFactor( 0x52ab,	 0.3229222 ),	/*	(218 2) */
	scBezFactor( 0x9e15,	 0.6175179 )	/*	(218 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x00c5,	 0.0030192 ),	/*	(219 0) */
	scBezFactor( 0x0db9,	 0.0536104 ),	/*	(219 1) */
	scBezFactor( 0x513b,	 0.3173155 ),	/*	(219 2) */
	scBezFactor( 0xa045,	 0.6260549 )	/*	(219 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x00b6,	 0.0027809 ),	/*	(220 0) */
	scBezFactor( 0x0d0d,	 0.0509834 ),	/*	(220 1) */
	scBezFactor( 0x4fc2,	 0.3115654 ),	/*	(220 2) */
	scBezFactor( 0xa279,	 0.6346703 )	/*	(220 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x00a7,	 0.0025555 ),	/*	(221 0) */
	scBezFactor( 0x0c64,	 0.0484094 ),	/*	(221 1) */
	scBezFactor( 0x4e40,	 0.3056708 ),	/*	(221 2) */
	scBezFactor( 0xa4b3,	 0.6433643 )	/*	(221 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0099,	 0.0023427 ),	/*	(222 0) */
	scBezFactor( 0x0bbf,	 0.0458894 ),	/*	(222 1) */
	scBezFactor( 0x4cb4,	 0.2996306 ),	/*	(222 2) */
	scBezFactor( 0xa6f2,	 0.6521373 )	/*	(222 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x008c,	 0.0021420 ),	/*	(223 0) */
	scBezFactor( 0x0b1d,	 0.0434244 ),	/*	(223 1) */
	scBezFactor( 0x4b1f,	 0.2934439 ),	/*	(223 2) */
	scBezFactor( 0xa936,	 0.6609897 )	/*	(223 3) */
},
#endif


#ifdef SubDiv8
{
	scBezFactor( 0x0080,	 0.0019531 ),	/*	(224 0) */
	scBezFactor( 0x0a80,	 0.0410156 ),	/*	(224 1) */
	scBezFactor( 0x4980,	 0.2871094 ),	/*	(224 2) */
	scBezFactor( 0xab80,	 0.6699219 )	/*	(224 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0074,	 0.0017757 ),	/*	(225 0) */
	scBezFactor( 0x09e5,	 0.0386640 ),	/*	(225 1) */
	scBezFactor( 0x47d7,	 0.2806261 ),	/*	(225 2) */
	scBezFactor( 0xadce,	 0.6789342 )	/*	(225 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0069,	 0.0016093 ),	/*	(226 0) */
	scBezFactor( 0x094f,	 0.0363708 ),	/*	(226 1) */
	scBezFactor( 0x4624,	 0.2739930 ),	/*	(226 2) */
	scBezFactor( 0xb022,	 0.6880269 )	/*	(226 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x005f,	 0.0014537 ),	/*	(227 0) */
	scBezFactor( 0x08bd,	 0.0341368 ),	/*	(227 1) */
	scBezFactor( 0x4467,	 0.2672090 ),	/*	(227 2) */
	scBezFactor( 0xb27b,	 0.6972005 )	/*	(227 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0055,	 0.0013084 ),	/*	(228 0) */
	scBezFactor( 0x082e,	 0.0319633 ),	/*	(228 1) */
	scBezFactor( 0x42a1,	 0.2602730 ),	/*	(228 2) */
	scBezFactor( 0xb4da,	 0.7064552 )	/*	(228 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x004c,	 0.0011732 ),	/*	(229 0) */
	scBezFactor( 0x07a4,	 0.0298514 ),	/*	(229 1) */
	scBezFactor( 0x40d0,	 0.2531839 ),	/*	(229 2) */
	scBezFactor( 0xb73e,	 0.7157915 )	/*	(229 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0044,	 0.0010476 ),	/*	(230 0) */
	scBezFactor( 0x071e,	 0.0278020 ),	/*	(230 1) */
	scBezFactor( 0x3ef5,	 0.2459407 ),	/*	(230 2) */
	scBezFactor( 0xb9a7,	 0.7252097 )	/*	(230 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x003d,	 0.0009313 ),	/*	(231 0) */
	scBezFactor( 0x069b,	 0.0258163 ),	/*	(231 1) */
	scBezFactor( 0x3d11,	 0.2385423 ),	/*	(231 2) */
	scBezFactor( 0xbc15,	 0.7347102 )	/*	(231 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x0036,	 0.0008240 ),	/*	(232 0) */
	scBezFactor( 0x061e,	 0.0238953 ),	/*	(232 1) */
	scBezFactor( 0x3b22,	 0.2309875 ),	/*	(232 2) */
	scBezFactor( 0xbe8a,	 0.7442932 )	/*	(232 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x002f,	 0.0007252 ),	/*	(233 0) */
	scBezFactor( 0x05a4,	 0.0220401 ),	/*	(233 1) */
	scBezFactor( 0x3928,	 0.2232755 ),	/*	(233 2) */
	scBezFactor( 0xc103,	 0.7539592 )	/*	(233 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0029,	 0.0006347 ),	/*	(234 0) */
	scBezFactor( 0x052f,	 0.0202518 ),	/*	(234 1) */
	scBezFactor( 0x3724,	 0.2154050 ),	/*	(234 2) */
	scBezFactor( 0xc382,	 0.7637086 )	/*	(234 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0024,	 0.0005520 ),	/*	(235 0) */
	scBezFactor( 0x04be,	 0.0185314 ),	/*	(235 1) */
	scBezFactor( 0x3516,	 0.2073750 ),	/*	(235 2) */
	scBezFactor( 0xc606,	 0.7735416 )	/*	(235 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x001f,	 0.0004768 ),	/*	(236 0) */
	scBezFactor( 0x0452,	 0.0168800 ),	/*	(236 1) */
	scBezFactor( 0x32fd,	 0.1991844 ),	/*	(236 2) */
	scBezFactor( 0xc890,	 0.7834587 )	/*	(236 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x001a,	 0.0004088 ),	/*	(237 0) */
	scBezFactor( 0x03ea,	 0.0152988 ),	/*	(237 1) */
	scBezFactor( 0x30da,	 0.1908322 ),	/*	(237 2) */
	scBezFactor( 0xcb20,	 0.7934602 )	/*	(237 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0016,	 0.0003476 ),	/*	(238 0) */
	scBezFactor( 0x0387,	 0.0137887 ),	/*	(238 1) */
	scBezFactor( 0x2eac,	 0.1823173 ),	/*	(238 2) */
	scBezFactor( 0xcdb5,	 0.8035464 )	/*	(238 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0013,	 0.0002928 ),	/*	(239 0) */
	scBezFactor( 0x0329,	 0.0123509 ),	/*	(239 1) */
	scBezFactor( 0x2c73,	 0.1736385 ),	/*	(239 2) */
	scBezFactor( 0xd04f,	 0.8137178 )	/*	(239 3) */
},
#endif


#ifdef SubDiv16
{
	scBezFactor( 0x0010,	 0.0002441 ),	/*	(240 0) */
	scBezFactor( 0x02d0,	 0.0109863 ),	/*	(240 1) */
	scBezFactor( 0x2a30,	 0.1647949 ),	/*	(240 2) */
	scBezFactor( 0xd2f0,	 0.8239746 )	/*	(240 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x000d,	 0.0002012 ),	/*	(241 0) */
	scBezFactor( 0x027b,	 0.0096962 ),	/*	(241 1) */
	scBezFactor( 0x27e1,	 0.1557854 ),	/*	(241 2) */
	scBezFactor( 0xd595,	 0.8343173 )	/*	(241 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x000a,	 0.0001636 ),	/*	(242 0) */
	scBezFactor( 0x022b,	 0.0084815 ),	/*	(242 1) */
	scBezFactor( 0x2588,	 0.1466088 ),	/*	(242 2) */
	scBezFactor( 0xd841,	 0.8447461 )	/*	(242 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0008,	 0.0001310 ),	/*	(243 0) */
	scBezFactor( 0x01e1,	 0.0073434 ),	/*	(243 1) */
	scBezFactor( 0x2323,	 0.1372642 ),	/*	(243 2) */
	scBezFactor( 0xdaf2,	 0.8552615 )	/*	(243 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0006,	 0.0001030 ),	/*	(244 0) */
	scBezFactor( 0x019b,	 0.0062828 ),	/*	(244 1) */
	scBezFactor( 0x20b4,	 0.1277504 ),	/*	(244 2) */
	scBezFactor( 0xdda9,	 0.8658638 )	/*	(244 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0005,	 0.0000793 ),	/*	(245 0) */
	scBezFactor( 0x015b,	 0.0053009 ),	/*	(245 1) */
	scBezFactor( 0x1e39,	 0.1180664 ),	/*	(245 2) */
	scBezFactor( 0xe065,	 0.8765534 )	/*	(245 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0003,	 0.0000596 ),	/*	(246 0) */
	scBezFactor( 0x0120,	 0.0043988 ),	/*	(246 1) */
	scBezFactor( 0x1bb3,	 0.1082110 ),	/*	(246 2) */
	scBezFactor( 0xe328,	 0.8873305 )	/*	(246 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0002,	 0.0000435 ),	/*	(247 0) */
	scBezFactor( 0x00ea,	 0.0035775 ),	/*	(247 1) */
	scBezFactor( 0x1922,	 0.0981833 ),	/*	(247 2) */
	scBezFactor( 0xe5f0,	 0.8981957 )	/*	(247 3) */
},
#endif


#ifdef SubDiv32
{
	scBezFactor( 0x0002,	 0.0000305 ),	/*	(248 0) */
	scBezFactor( 0x00ba,	 0.0028381 ),	/*	(248 1) */
	scBezFactor( 0x1686,	 0.0879822 ),	/*	(248 2) */
	scBezFactor( 0xe8be,	 0.9091492 )	/*	(248 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0001,	 0.0000204 ),	/*	(249 0) */
	scBezFactor( 0x008e,	 0.0021817 ),	/*	(249 1) */
	scBezFactor( 0x13de,	 0.0776065 ),	/*	(249 2) */
	scBezFactor( 0xeb91,	 0.9201913 )	/*	(249 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0000,	 0.0000129 ),	/*	(250 0) */
	scBezFactor( 0x0069,	 0.0016093 ),	/*	(250 1) */
	scBezFactor( 0x112a,	 0.0670552 ),	/*	(250 2) */
	scBezFactor( 0xee6b,	 0.9313226 )	/*	(250 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0000,	 0.0000075 ),	/*	(251 0) */
	scBezFactor( 0x0049,	 0.0011221 ),	/*	(251 1) */
	scBezFactor( 0x0e6b,	 0.0563273 ),	/*	(251 2) */
	scBezFactor( 0xf14a,	 0.9425432 )	/*	(251 3) */
},
#endif


#ifdef SubDiv64
{
	scBezFactor( 0x0000,	 0.0000038 ),	/*	(252 0) */
	scBezFactor( 0x002f,	 0.0007210 ),	/*	(252 1) */
	scBezFactor( 0x0ba0,	 0.0454216 ),	/*	(252 2) */
	scBezFactor( 0xf42f,	 0.9538536 )	/*	(252 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0000,	 0.0000016 ),	/*	(253 0) */
	scBezFactor( 0x001a,	 0.0004072 ),	/*	(253 1) */
	scBezFactor( 0x08ca,	 0.0343371 ),	/*	(253 2) */
	scBezFactor( 0xf71a,	 0.9652541 )	/*	(253 3) */
},
#endif


#ifdef SubDiv128
{
	scBezFactor( 0x0000,	 0.0000005 ),	/*	(254 0) */
	scBezFactor( 0x000b,	 0.0001817 ),	/*	(254 1) */
	scBezFactor( 0x05e8,	 0.0230727 ),	/*	(254 2) */
	scBezFactor( 0xfa0b,	 0.9767451 )	/*	(254 3) */
},
#endif


#ifdef SubDiv256
{
	scBezFactor( 0x0000,	 0.0000001 ),	/*	(255 0) */
	scBezFactor( 0x0002,	 0.0000456 ),	/*	(255 1) */
	scBezFactor( 0x02fa,	 0.0116274 ),	/*	(255 2) */
	scBezFactor( 0xfd02,	 0.9883270 )	/*	(255 3) */
},
#endif


/* this one is needed by all sub divisions */
{
	scBezFactor( 0x0000,	 0.0000000 ),	/*	(256 0) */
	scBezFactor( 0x0000,	 0.0000000 ),	/*	(256 1) */
	scBezFactor( 0x0000,	 0.0000000 ),	/*	(256 2) */
	scBezFactor( 0x0000,	 1.0000000 )	/*	(256 3) */
}

};

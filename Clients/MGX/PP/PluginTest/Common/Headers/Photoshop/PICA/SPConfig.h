/*
 * Name:
 *	SPConfig.h
 *
 * Copyright 1986-1998 Adobe Systems Incorporated.
 * All Rights Reserved.
 *
 * Purpose:
 *	SPConfig.h is the environment configuration file for PICA. It
 *	defines MAC_ENV or WIN_ENV. These are used to control platform-specific
 *	sections of code.
 *
 * Distribution:
 *	PUBLIC
 *
 * Version history:
 *	1.0.0 4/1/1995	DL	First version.
 *		Created by Adobe Systems Incorporated.
 */

#ifndef __SPCnfig__
#define __SPCnfig__

/*
 *	Metrowerks MacOS 68K and PPC
 */
#ifdef __MWERKS__
#if !defined(__INTEL__)		/* mjf was- #if defined(__MC68K__) || defined(__POWERPC__) */
#ifndef MAC_ENV
#define MAC_ENV 1
#endif
#endif
#endif

/*
 *	Metrowerks Windows
 */
#ifdef __MWERKS__
#ifdef __INTEL__
#ifndef WIN_ENV
#define WIN_ENV 1
#include <x86_prefix.h>
#endif
#endif
#endif

/*
 *	Windows
 */
#if defined(_WINDOWS) || defined(_MSC_VER)		// PSMod, better compiler check
#ifndef WIN_ENV
#define WIN_ENV 1
#endif
#endif


/*
 *	Make certain that one and only one of the platform constants is defined.
 */

#if !defined(WIN_ENV) && !defined(MAC_ENV)
#error
#endif

#if defined(WIN_ENV) && defined(MAC_ENV)
#error
#endif

#endif

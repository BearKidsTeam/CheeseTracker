/***************************************************************************
    This file is part of the CheeseTronic Music Tools
    url                  : http://reduz.com.ar/cheesetronic
    copyright            : (C) 2003 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
                          typedefs.h  -  description
                             -------------------
    begin                : Tue Jan 8 2002
    copyright            : (C) 2002 by Juan Linietsky
    email                : coding@reduz.com.ar
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <iostream>
#include <cstdio>
#include <string>

#include <math.h>

#define FUNCTION_SUCCESS 0
#define FUNCTION_FAILURE -1

//#define DIR_SEPARATOR "/"
//

// A useful macro for testing if the current build environment
// matches certain critical assumptions made about it.

#define XYZ(X,Y,Z) X##Y##Z
#define COMPILER_ASSERT(exp) extern char _CompilerAssert[(exp)?1:-1]

#ifdef HAVE_STDINT_H

// Use the system's fixed-size integers if possible. These integers
// are guaranteed to have the specified number of bits.

#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS 
#endif
#	include <stdint.h>

	typedef uint8_t 	Uint8;
	typedef int8_t		Sint8;
	typedef uint16_t	Uint16;
	typedef int16_t		Sint16;
	typedef uint32_t	Uint32;
	typedef int32_t		Sint32;
	typedef int64_t		Sint64;
	typedef uint64_t	Uint64;

#	define SINT8_MAX INT8_MAX
#	define SINT16_MAX INT16_MAX
#	define SINT32_MAX INT32_MAX
#	define SINT64_MAX INT64_MAX

// Unsigned upper limits already defined correctly by stdint.h

#elif defined(HAVE_MSINT)

	#include <limits.h>

	// Ugh. We seem to be in a Micro$hit environment.

	typedef unsigned __int8		Uint8;
	typdef __int8			Sint8;
	typedef unsigned __int16	Uint16;
	typedef __int16			Sint16;
	typedef unsigned __int32	Uint32;
	typedef __int32			Sint32;
	typedef unsigned __int64	Uint64;
	typedef __int64			Sint64;

#	define SINT8_MAX	_I8_MAX
#	define SINT16_MAX	_I16_MAX
#	define SINT32_MAX	_I32_MAX
#	define SINT64_MAX	_I64_MAX

#	define UINT8_MAX    _UI8_MAX
#	define UINT16_MAX   _UI16_MAX
#	define UINT32_MAX   _UI32_MAX
#	define UINT64_MAX   _UI64_MAX

#else

// If <stdint.h> is not available (or doesn't define all the required
// types), and to add insult to injury Microsoft-style exact-width
// integers aren't available either, then the old CT 0.9.9 code is
// preserved below. It only works if certain assumptions hold (for
// example, "int" is assumed to be 32 bits, but there might be systems
// where "int" is actually 64 bits).
//
// As of 2007, the COMPILER_ASSERT macro will cause this code not to
// compile if these typedefs don't turn out to have the number of
// bytes that they should have if all bytes are 8 bits wide.
//

#	include <limits.h>

	typedef unsigned char   Uint8;
	typedef signed char     Sint8;
	typedef unsigned short  Uint16;
	typedef signed short    Sint16;
	typedef unsigned int    Uint32;
	typedef signed int      Sint32;
	typedef long long 	Sint64;
	typedef unsigned long long Uint64;

#	define SINT8_MAX	SCHAR_MAX
#	define SINT16_MAX	SHRT_MAX
#	define SINT32_MAX	INT_MAX
#	define SINT64_MAX	LLONG_MAX

#	define UINT8_MAX	UCHAR_MAX
#	define UINT16_MAX	USHRT_MAX
#	define UINT32_MAX	UINT_MAX
#	define UINT64_MAX	ULLONG_MAX

	extern "C" {

	// If any of these cause a compiler error, then the
	// typedefs above need to be redefined so that they
	// have the correct number of bytes.

	COMPILER_ASSERT(sizeof(Uint8)*BITS_PER_BYTE == 8);
	COMPILER_ASSERT(sizeof(Sint8)*BITS_PER_BYTE == 8);
	COMPILER_ASSERT(sizeof(Uint16)*BITS_PER_BYTE == 16);
	COMPILER_ASSERT(sizeof(Sint16)*BITS_PER_BYTE == 16);
	COMPILER_ASSERT(sizeof(Uint32)*BITS_PER_BYTE == 32);
	COMPILER_ASSERT(sizeof(Sint32)*BITS_PER_BYTE == 32);
	COMPILER_ASSERT(sizeof(Uint64)*BITS_PER_BYTE == 64);
	COMPILER_ASSERT(sizeof(Sint64)*BITS_PER_BYTE == 64);

	}

#endif

// Build only on two's complement machines. Disable this at your own
// risk.

COMPILER_ASSERT((Uint8)(-1) == (Uint8)(~0));

typedef unsigned char byte;

// FIXME: 
// These escape sequences, used by the WARN macro, produce color on certain
// terminals. The original author has assumed that Qt requires X11, and this
// program requires Qt, so therefore stdout and stderr will always be either
// /dev/null, or an xterm (which understands these escape sequences).
//
// The assumption fails if this program is compiled with Qt under Windows,
// where it might be run from a Microsoft CMD.EXE window. It also fails if
// an obsolete version of xterm is in use.
//
// Furthermore, light blue is unreadable against the default white background
// of the xterm program.
//
// I have shortened the sequences, as LIGHT_BLUE previously contained redundant
// terminal commands, and DEFAULT_COLOR produced gray, regardless of the
// default color passed with the -fg option to xterm (or the default color
// selected through the menus of Konsole and other such "user-friendly"
// xterms).
//
// Define ANSI_COLORS to use an ANSI color sequence in the WARN() macro.

#ifndef ANSI_COLORS
	const char DEFAULT_COLOR[] =	"";
	const char LIGHT_BLUE[] =	"";
#else
	const char DEFAULT_COLOR[] =	"\033[0m";
	const char LIGHT_BLUE[] =	"\033[01;34m";
#endif

#define ERROR(reason) do { \
	std::cout << "Error: "<< __FILE__ <<':'<< __LINE__ << ": " << reason << std::endl;\
} while (0)
#define WARN(reason) do { \
	std::cout << LIGHT_BLUE << "Warning: "<< __FILE__ << ':' << __LINE__ << ": " << reason << DEFAULT_COLOR << std::endl;\
} while(0)


	using std::string;

static inline string itostr(int m_val) {

	char cacabuffer[100];
	snprintf(cacabuffer, sizeof(cacabuffer)-1, "%i",m_val);
 	return cacabuffer;
}

	
static inline int near2power(int m_val) {

	int bit=0;
	while (m_val)
		m_val&=~(1<<(bit++));
	return(1<<bit)>>1;
}


#define BOUND(bvar,bmin,bmax) ((bvar>bmax)?bmax:(bvar<bmin)?bmin:bvar)
#define NUM_2_THIRD_CHAR(num) ('0'+((num)/100) % 10)
#define NUM_2_SECOND_CHAR(num) ('0'+((num)/10) % 10)
#define NUM_2_FIRST_CHAR(num) ('0'+(num) % 10)
#define BITBOOL(m_exp) ((m_exp)?1:0)
#define BITSIZEOF(m_exp) (sizeof(m_exp)*BITS_PER_BYTE)



#define OCTAVE 12

#endif


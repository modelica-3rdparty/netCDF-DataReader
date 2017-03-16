/*  msvc_compatibility.h - MSVC compatibility header
    Copyright (C) 2015-2017, tbeu

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#if !defined(MSVC_COMPATIBILITY_H)
#define MSVC_COMPATIBILITY_H

#if defined(_MSC_VER)
/* MSVC compatibility handling for linking static libraries compiled by Visual Studio 2010 */

#if _MSC_VER >= 1900

#include <stdio.h>
#include <time.h>
#include "gconstructor.h"

extern long timezone = 0;

#if defined(G_HAS_CONSTRUCTORS)
#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(Timezone_initialize)
#endif
G_DEFINE_CONSTRUCTOR(Timezone_initialize)
static void Timezone_initialize(void) {
	_get_timezone(&timezone);
}
#endif

/* Dymola 2017 FD01 defines __iob_func in dsutil.h */
#if !defined(HACK_SUPPORT_VS2015)
extern FILE _iob[3];

#undef stdin
#undef stdout
#undef stderr
#define stdin _iob
#define stdout (_iob+1)
#define stderr (_iob+2)

FILE* __iob_func(void) {
	return _iob;
}
#endif

#pragma comment( lib, "legacy_stdio_definitions.lib" )

#endif

#endif

#endif

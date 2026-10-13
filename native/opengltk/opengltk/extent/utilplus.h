/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is "Java-Python Extension libplus (JPE-libplus)".
 * 
 * The Initial Developer of the Original Code is Frederic Bruno Giacometti.
 * Portions created by Frederic Bruno Giacometti are
 * Copyright (C) 2001-2002 Frederic Bruno Giacometti. All Rights Reserved.
 * 
 * Contributor(s): frederic.giacometti@arakne.com
 *
 * Acknowledgments:
 */

#ifndef UTILPLUS_H
#define UTILPLUS_H

#ifndef NOT
#define NOT !
#endif

typedef char const* string_t;

#ifndef DLLimport
# ifdef _MSC_VER
#   define DLLimport __declspec( dllimport)
# else
#   define DLLimport
# endif
#endif

#ifndef DLLexport
# ifdef _MSC_VER
#   define DLLexport __declspec( dllexport)
# else
#   define DLLexport
# endif
#endif


#ifdef _MSC_VER
#   define WSTDCALL __stdcall
#   define WVARCALL __cdecl
#else
#   define WSTDCALL
#   define WVARCALL
#endif

#ifdef LIBPLUS_DEBUG
# define LogTrace ((void)fprintf( stderr, "Trace@%s:%i\n",	\
				  __FILE__, __LINE__))
#else
# define LogTrace
#endif

#endif /* UTILPLUS_H */

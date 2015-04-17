/*
# Copyright (c) 2010, Joerg Raedler <joerg@j-raedler.de>
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the following disclaimer
#   in the documentation and/or other materials provided with the distribution.
# * Neither the name of the dezentral gbr nor the names of
#   its contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef STRINGHASHTABLE_H
#define STRINGHASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CTOOLS_BOOL_DEFINED
typedef enum {false=0, true=1} bool;
#define CTOOLS_BOOL_DEFINED 1
#endif /* CTOOLS_BOOL_DEFINED */

/* If defined, SHT_EXTERNAL_TYPEDEFS should be a
   valid header filename, for example
   gcc -c -DSHT_EXTERNAL_TYPEDEFS="\"foobar.h\""
*/
#ifndef SHT_EXTERNAL_TYPEDEFS
typedef unsigned long  SHT_size;
typedef void          *SHT_val;
#else
#include SHT_EXTERNAL_TYPEDEFS
#endif /* SHT_EXTERNAL_TYPEDEFS */

typedef struct SHT_Table SHT_Table;
typedef void(*SHT_iterfunc)(char *key, SHT_val value, const void *arg);

SHT_Table *shtNew(SHT_size capacity);
void       shtFree(SHT_Table *table);
SHT_val    shtGet(const SHT_Table *table, const char *key);
bool       shtPut(SHT_Table *table, const char *key, SHT_val value);
bool       shtExists(const SHT_Table *table, const char *key);
SHT_size   shtCount(const SHT_Table *table);
bool       shtIterate(const SHT_Table *table, SHT_iterfunc iFunc, void *arg);

#ifdef  __cplusplus
}
#endif

#endif /* STRINGHASHTABLE_H */

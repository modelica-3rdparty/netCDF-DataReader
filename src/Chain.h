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

#ifndef CHAIN_H
#define CHAIN_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif /* _GNU_SOURCE */

#ifndef CTOOLS_BOOL_DEFINED
typedef enum {false=0, true=1} bool;
#define CTOOLS_BOOL_DEFINED 1
#endif /* CTOOLS_BOOL_DEFINED */

/* basic structure of a chain item */
struct _item {
    struct _item *next; /* points to next item or NULL */
    struct _item *prev; /* points to previous item or NULL */
    void *data;         /* generic pointer to item data */
};
typedef struct _item Item;

Item *chainItemNew(void *data);
Item *chainNew(int len, bool closed, void *(*func)(long));
Item *chainAppend(Item *prev, Item *item);
Item *chainCutBehind(Item *c);
Item *chainItem(Item *c, int n);
Item *chainFirst(Item *c);
Item *chainLast(Item *c);
Item *chainCat(Item *c0, Item *c1);
void  chainInsertAfter(Item *c, Item *i);
void  chainInsertBefore(Item *c, Item *i);
void  chainDelete(Item *i);
int   chainLen(Item *c);
int   chainPos(Item *c);
bool  chainIsClosed(Item *c);
void  chainFree(Item *c, void (*func)(void*));
/* char *chainItemString(Item *c); */
void  chainItemPrint(FILE *f, Item *c);
void  chainApply(Item *c, void (*func)(Item*));
Item *chainApplyExtended(Item *c, bool (*func)(Item*), bool forward);
Item *chainSearch(Item *c, void *val);

#ifndef NDEBUG
Item *chainNext(Item *i);
Item *chainPrev(Item *i);
void *chainData(Item *i);
#else
#define chainNext(I) ((I)->next)
#define chainPrev(I) ((I)->prev)
#define chainData(I) ((I)->data)
#endif /* NDEBUG */

#ifdef  __cplusplus
}
#endif

#endif /* CHAIN_H */

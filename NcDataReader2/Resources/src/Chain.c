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

You may use the following command to test this file if tcc is installed:
    tcc -Wall -DCHAIN_TEST -run Chain.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "Chain.h"

#ifndef NDEBUG
#include <assert.h>
#endif /* NDEBUG */

/* create a new standalone item with data block set to data */
Item *chainItemNew(void *data) {
    Item *item = (Item *)malloc(sizeof(Item));
    if (item) {
        item->data = data;
        item->next = NULL;
        item->prev = NULL;
    }
    return item;
}

/* append item to prev, create new item if NULL */
Item *chainAppend(Item *prev, Item *item) {
    if (!item) {
        item = (Item *)malloc(sizeof(Item));
        item->data = NULL;
        item->next = NULL;
    }
    prev->next = item;
    item->prev = prev;
    return item;
}

/* create a new chain, possibly closed, using func to create data */
Item *chainNew(int len, bool closed, void *(*func)(long)) {
    Item *start, *item;
    long i = 0;
    if (func) {
        start = item = chainItemNew(func(i));
        for (i = 1; i < len; i++) 
            item = chainAppend(item, chainItemNew(func(i)));
        if (closed > 0) {
            item->next = start;
            start->prev = item;
        }
    } else {
        start = item = chainItemNew(NULL);
        for (i = 1; i < len; i++)
            item = chainAppend(item, NULL);
        if (closed > 0) {
            item->next = start;
            start->prev = item;
        }
    }
    return start;
}

/* cut chain behind item c, return second part */
Item *chainCutBehind(Item *c) {
    Item *next = c->next;
    if (next) {
        next->prev = NULL;
        c->next = NULL;
    }
    return next;
}

/* return number of items in chain c */
int chainLen(Item *c) {
    Item *tmp = chainFirst(c);
    int i = 1;
    if (tmp) { /* not closed */
        while (tmp->next) {
            tmp = tmp->next;
            i++;
        }
    } else { /* closed */
        tmp = c;
        while (tmp->next != c) {
            tmp = tmp->next;
            i++;
        }
    }
    return i;
}

/* return item number n relative to item c */
Item *chainItem(Item *c, int n) {
    int i = 0, d = (n > 0 ? 1 : -1);
    Item *tmp;
    while (i != n) {
        tmp = (d == 1 ? c->next : c->prev);
        if (! tmp)
            return NULL;
        c = tmp;
        i += d;
    }
    return c;
}

/* concatenate chain c1 to c0 */
Item *chainCat(Item *c0, Item *c1) {
    if ((c0=chainLast(c0)) && (c1=chainFirst(c1))) {
        c0->next = c1;
        c1->prev = c0;
        return chainFirst(c0);
    }
    return NULL;
}

/* return first item or NULL for closed chains */
Item *chainFirst(Item *c) {
    Item *start = c;
    while (chainPrev(c)) {
        if (c->prev == start)
            return NULL; /* closed chain has no first item */
        c = chainPrev(c);
    }
    return c;
}

/* return last item or NULL for closed chains */
Item *chainLast(Item *c) {
    Item *start = c;
    while (chainNext(c)) {
        if (c->next == start)
            return NULL; /* closed chain has no last item */
        c = chainNext(c);
    }
    return c;
}

/* give position of item in chain, -1 for closed chain */
int chainPos(Item *c) {
    Item *tmp = chainFirst(c);
    int i = 0;
    if (!tmp) return -1;
    while (tmp != c) {
        tmp = chainNext(tmp);
        i++;
    }
    return i;
}

/* insert item i in chain after element c */
void chainInsertAfter(Item *c, Item *i) {
    Item *prev=c, *next=chainNext(c);
    prev->next = i;
    i->prev = prev;
    i->next = next;
    if (next)
        next->prev = i;
}

/* insert item i in chain before element c */
void chainInsertBefore(Item *c, Item *i) {
    Item *prev=chainPrev(c), *next=c;
    next->prev = i;
    i->next = next;
    i->prev = prev;
    if (prev)
        prev->next = i;
}

/* delete item in chain and close gap */
void chainDelete(Item *i) {
    Item *prev=chainPrev(i), *next=chainNext(i);
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
}

/* check if chain is closed */
bool chainIsClosed(Item *c) {
    Item *tmp = c;
    while (chainNext(tmp)) {
        if (tmp->next == c)
            return true;
        tmp = chainNext(tmp);
    }
    return false;
}

/* give a characteristic string for item */
/*
char *chainItemString(Item *c) {
    char *s= (char *)malloc(81);
    snprintf(s, 80, "<item memAdr=\"%p\" prev=\"%p\" next=\"%p\" data=\"%p\"/>\n",
        c, chainPrev(c), chainNext(c), chainData(c));
    return s;
}
*/

/* print a characteristic string for item */
void chainItemPrint(FILE *f, Item *c) {
    fprintf(f, "<item memAdr=\"%p\" prev=\"%p\" next=\"%p\" data=\"%p\"/>\n",
        c, chainPrev(c), chainNext(c), chainData(c));
}

/* call function func with all chain items in c */
void chainApply(Item *c, void (*func)(Item*)) {
    Item *start = c;
    do {
        func(c);
        c = chainNext(c);
    } while (c && (c != start));
}

/* call function func with all chain items in c.
   as soon as func returns nonzero, return corresponding Item,
   works backwards and forward */
Item *chainApplyExtended(Item *c, bool (*func)(Item*), bool forward) {
    Item *start = c;
    do {
        if (func(c))
            return c;
        c = ((forward) ? chainNext(c) : chainPrev(c));
    } while (c && (c != start));
    return NULL;
}

/* search for first Item containing addr as data */
Item *chainSearch(Item *c, void *val) {
    Item *start = c;
    do {
        if (chainData(c) == val)
            return c;
        c = chainNext(c);
    } while (c && (c != start));
    return NULL;
}

/* free chain memory, using func to free the data memory */
void chainFree(Item *c, void (*func)(void*)) {
    if (!c)
        return;
    if (chainIsClosed(c))
        chainCutBehind(c->prev);
    else
        c = chainFirst(c);
    while (c->next != NULL) {
        c = c->next;
        if (func) func(chainData(chainPrev(c)));
        free(c->prev);
    }
    free(c);
}

#ifndef NDEBUG
Item *chainNext(Item *i) {
    assert(i);
    return i->next;
}

Item *chainPrev(Item *i){
    assert(i);
    return i->prev;
}

void *chainData(Item *i){
    assert(i);
    return i->data;
}
#endif /* NDEBUG */

#ifdef CHAIN_TEST
#include <stdio.h>

void *enumerate(long i) {
    long *l = (long *)malloc(sizeof(long));
    *l = i;
    return (void*)l;
}

void freeEnum(void *p) {
    free((long*)p);
}

void printNum(Item *i) {
    printf("%ld ", *((long*)(i->data)));
}

/* test a closed chain */
int test1(void) {
    Item *c = chainNew(20, 1, enumerate);
    chainApply(chainItem(c, 30), printNum);
    printf("\n");
    chainFree(c, freeEnum);
    return 1;
}

/* test an open chain */
int test2(void) {
    Item *a, *b; 
    a = b = chainNew(15, 0, enumerate);

    chainApply(a, printNum);
    printf("\n");

    a = chainFirst(b);
    b = chainCutBehind(chainItem(a, 7));
    b = chainCat(b, a);
    a = b;

    chainApply(a, printNum);
    printf("\n");

    chainFree(b, freeEnum);
    return 1;
}

void _tmp_(Item *a) { chainItemPrint(stdout, a); }
        
/* test printing of items */
int test3(void) {
    Item *a = chainNew(13, 0, enumerate);
    chainApply(a, _tmp_);
    chainFree(a, freeEnum);
    return 1;
}

/* test huge chains */
int test4(void) {
    Item *a = chainNew(5000000, 1, NULL), *b;
    a = chainItem(a, chainLen(a)/2);
    chainCutBehind(a);
    a = chainLast(a);
    while (a->prev) {
        b = a->prev;
        chainDelete(a);
        free(a);
        a = b;
    }
    free(a);
    return 1;
}

bool search42(Item *i) {
    return (*(long *)(i->data) == 42);
}

/* test chainApplyExtended */
int test5(void) {
    Item *a = chainNew(100, 0, enumerate), *tmp;
    if ((tmp = chainApplyExtended(a, search42, 1))) {
        printf("%d\n", chainPos(tmp));
        chainItemPrint(stdout, tmp);
    }
    chainFree(a, freeEnum);
    return 1;
}

/* test chainSearch */
int test6(void) {
    Item *a = chainNew(100, 0, enumerate), *tmp;
    void *s = chainItem(a, 42)->data;
    if ((tmp = chainSearch(a, s))) {
        printf("%d\n", chainPos(tmp));
        chainItemPrint(stdout, tmp);
    }
    chainFree(a, freeEnum);
    return 1;
}


int main(void) {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 1;
}
#endif /* CHAIN_TEST */

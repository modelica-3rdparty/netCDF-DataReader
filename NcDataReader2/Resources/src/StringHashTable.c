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
tcc -Wall -DSTRINGHASHTABLE_TEST -run StringHashTable.c
*/

#include <string.h>
#include <stdlib.h>
#include "StringHashTable.h"
#define uthash_fatal(msg) return false
#include "uthash.h"

struct SHT_Table {
    char *key;
    SHT_val value;
    UT_hash_handle hh; /* Hashable structure */
};

SHT_Table * shtNew(SHT_size capacity) {
    return NULL;
}

void _shtFree(SHT_Table **table) {
    SHT_Table* s;
    SHT_Table* tmp;
    HASH_ITER(hh, *table, s, tmp) {
        free(s->key);
        HASH_DEL(*table, s);
        free(s);
    }
}

SHT_val shtGet(const SHT_Table *table, const char *key) {
    if (key) {
        SHT_Table* s;
        HASH_FIND_STR(table, key, s);
        if (s) {
            return s->value;
        }
    }
    return NULL;
}

bool shtExists(const SHT_Table *table, const char *key) {
    if (key) {
        SHT_Table* s;
        HASH_FIND_STR(table, key, s);
        if (s) {
            return true;
        }
    }
    return false;
}

bool _shtPut(SHT_Table **table, const char *key, SHT_val value) {
    SHT_Table* s;
    if (key == NULL) {
        return false;
    }
    s = malloc(sizeof(SHT_Table));
    if (!s) {
        return false;
    }
    s->key = _strdup(key);
    s->value = value;
    HASH_ADD_KEYPTR(hh, *table, s->key, strlen(s->key), s);
    return true;
}

SHT_size shtCount(const SHT_Table *table) {
    return HASH_COUNT(table);
}

bool shtIterate(const SHT_Table *table, SHT_iterfunc iFunc, void *arg) {
    if (table && iFunc) {
        const SHT_Table* s;
        for (s = table; s != NULL; s = s->hh.next) {
            iFunc(s->key, s->value, arg);
        }
        return true;
    }
    return false;
}

#ifdef STRINGHASHTABLE_TEST

#include <stdio.h>

void visit(const char *key, void *value, void *arg) {
    printf("iter: %s maps to %s\n", key, (char *)value);
}

int main(void) {
    SHT_Table * t;
    char *obj0 = "HELLO_00";
    char *obj1 = "HELLO_01";
    char *obj2 = "HELLO_02";
    char *res;

    t = shtNew(1000);
    shtPut(t, "key0", obj0);
    shtPut(t, "key1", obj1);
    shtPut(t, "key2", obj2);

    res = shtGet(t, "key0");
    printf("key0 -> %s\n", res);
    res = shtGet(t, "key1");
    printf("key1 -> %s\n", res);
    res = shtGet(t, "key2");
    printf("key2 -> %s\n", res);

    shtIterate(t, visit, NULL);

    shtFree(t);

    return 0;
}

#endif /* STRINGHASHTABLE_TEST */

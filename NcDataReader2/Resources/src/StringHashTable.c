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

typedef struct Pair {
    char    *key;
    SHT_val  value;
} Pair;

typedef struct PairList {
    SHT_size  count;
    Pair     *pairs;
} PairList;

struct SHT_Table {
    SHT_size  count;
    PairList *pairlist;
};


static Pair *getPair(PairList *plist, const char *key) {
    SHT_size i, n;
    Pair *pair;
    n = plist->count;
    if (n == 0) {
        return NULL;
    }
    pair = plist->pairs;
    i = 0;
    while (i < n) {
        if (pair->key != NULL && pair->value != NULL) {
            if (strcmp(pair->key, key) == 0) {
                return pair;
            }
        }
        pair++;
        i++;
    }
    return NULL;
}


static SHT_size strHash(const char *str) {
    SHT_size hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}


SHT_Table * shtNew(SHT_size capacity) {
    SHT_Table *table;
    
    table = malloc(sizeof(SHT_Table));
    if (table == NULL) {
        return NULL;
    }
    table->count = capacity;
    table->pairlist = malloc(table->count * sizeof(PairList));
    if (table->pairlist == NULL) {
        free(table);
        return NULL;
    }
    memset(table->pairlist, 0, table->count * sizeof(PairList));
    return table;
}


void shtFree(SHT_Table *table) {
    SHT_size i, j, n, m;
    PairList *plist;
    Pair *pair;
    if (table == NULL) return;
    n = table->count;
    plist = table->pairlist;
    i = 0;
    while (i < n) {
        m = plist->count;
        pair = plist->pairs;
        j = 0;
        while(j < m) {
            free(pair->key);
            pair++;
            j++;
        }
        free(plist->pairs);
        plist++;
        i++;
    }
    free(table->pairlist);
    free(table);
}


SHT_val shtGet(const SHT_Table *table, const char *key) {
    SHT_size index;
    PairList *plist;
    Pair *pair;
    if ((table == NULL) || (key == NULL)) return NULL;
    index = strHash(key) % table->count;
    plist = &(table->pairlist[index]);
    pair = getPair(plist, key);
    if (pair == NULL) return NULL;
    return pair->value;
}

bool shtExists(const SHT_Table *table, const char *key) {
    SHT_size index;
    PairList *plist;
    Pair *pair;
    if ((table == NULL) || (key == NULL)) return false;
    index = strHash(key) % table->count;
    plist = &(table->pairlist[index]);
    pair = getPair(plist, key);
    if (pair == NULL) {
        return false;
    }
    return true;
}


bool shtPut(SHT_Table *table, const char *key, SHT_val value) {
    SHT_size key_len, index;
    PairList *plist;
    Pair *tmp_pairs, *pair;
    char *new_key;
    if ((table == NULL) || (key == NULL)) return false;
    key_len = strlen(key);
    index = strHash(key) % table->count;
    plist = &(table->pairlist[index]);
    if ((pair = getPair(plist, key)) != NULL) {
        pair->value = value;
        return true;
    }
    new_key = malloc((key_len + 1) * sizeof(char));
    if (new_key == NULL) return false;
    if (plist->count == 0) {
        plist->pairs = malloc(sizeof(Pair));
        if (plist->pairs == NULL) {
            free(new_key);
            return false;
        }
        plist->count = 1;
    }
    else {
        tmp_pairs = realloc(plist->pairs, (plist->count + 1) * sizeof(Pair));
        if (tmp_pairs == NULL) {
            free(new_key);
            return false;
        }
        plist->pairs = tmp_pairs;
        plist->count++;
    }
    pair = &(plist->pairs[plist->count - 1]);
    pair->key = new_key;
    strcpy(pair->key, key);
    pair->value = value;
    return true;
}


SHT_size shtCount(const SHT_Table *table) {
    SHT_size i, j, n, m, count;
    PairList *plist;
    Pair *pair;
    if (table == NULL) return 0;
    plist = table->pairlist;
    n = table->count;
    i = 0;
    count = 0;
    while (i < n) {
        pair = plist->pairs;
        m = plist->count;
        j = 0;
        while (j < m) {
            count++;
            pair++;
            j++;
        }
        plist++;
        i++;
    }
    return count;
}


bool shtIterate(const SHT_Table *table, SHT_iterfunc iFunc, void *arg) {
    SHT_size i, j, n, m;
    PairList *plist;
    Pair *pair;
    if ((table == NULL) || (iFunc == NULL)) return false;
    plist = table->pairlist;
    n = table->count;
    i = 0;
    while (i < n) {
        pair = plist->pairs;
        m = plist->count;
        j = 0;
        while (j < m) {
            iFunc(pair->key, pair->value, arg);
            pair++;
            j++;
        }
        plist++;
        i++;
    }
    return true;
}


#ifdef STRINGHASHTABLE_TEST

#include <stdio.h>

void visit(const char *key, void *value, const void *arg) {
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
    
    return 0;
}

#endif /* STRINGHASHTABLE_TEST */


/*  ncDataReader2 - interpolating reading of 1D data sets
    Copyright (C) 2004  Joerg Raedler <joerg@dezentral.de>

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

#include "Lookup.h"

struct _lookupCacheData {
    bool valid;
    double min;
    double max;
    size_t i;
};
typedef struct _lookupCacheData LookupCacheData;

static void *lookupCacheItemNew(long i) {
    LookupCacheData *d = (LookupCacheData *)malloc(sizeof(LookupCacheData));
    if (! d) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    d->valid = false;
    return (void *)d;
}

/* create new lookup cache */
Item *lookupCacheNew(int len) {
    return chainNew(len, 1, lookupCacheItemNew);
}

static void lookupCacheItemFree(void *i) {
    free((LookupCacheData *)i);
}

/* delete lookup cache */
void lookupCacheFree(Item *c) {
    chainFree(c, lookupCacheItemFree);
}

static void lookupCacheItemReset(Item *i) {
    ((LookupCacheData *)(i->data))->valid = false;
}

/* reset (wipeout) lookup cache */
void lookupCacheReset(Item *c) {
    chainApply(c, lookupCacheItemReset);
}

/* search a value cached */
size_t ncDataSet1DLookupValue(NcDataSet1D *dataSet, double x) {
    size_t i, hi, lo;
    if (x <= dataSet->min) return 0;
    if (x >= dataSet->max) return dataSet->dim-1;
    if (dataSet->lookupCache) {
        /* search in cache - search backwards */
        LookupCacheData *lc;
        Item *s, *c = (Item *)(dataSet->lookupCache);
        s = c;
        while(c->prev != s) {
            lc = (LookupCacheData *)(c->data);
            if (lc->valid)
                if ((lc->min <= x) && (lc->max > x)) {
                    dataSet->lCacheStat[0]++;
                    return lc->i; 
                }
            c = c->prev;
        }
    }
    /* not found -> do full search */
    lo = 0; hi = dataSet->dim-1;
    while (hi > (lo + 1)) {
        if (ncDataSet1DGetItem(dataSet, i=(hi + lo)/2) > x)
            hi = i;
        else
            lo = i;
    }
    i = lo;
    if (dataSet->lookupCache) {
        /* save data to the next cache element */
        Item *c = ((Item *)(dataSet->lookupCache))->next;
        LookupCacheData *lc = (LookupCacheData *)(c->data);
        lc->valid = true;
        lc->min = ncDataSet1DGetItem(dataSet, i);
        lc->max = ncDataSet1DGetItem(dataSet, i+1);
        lc->i   = i;
        /* rotate cache one step */
        dataSet->lookupCache = (void*)(c);
    }
    dataSet->lCacheStat[1]++;
    return i;
}

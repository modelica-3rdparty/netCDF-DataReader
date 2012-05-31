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

#include <stdlib.h>
#include <assert.h>
#include "IpLinear.h"


struct _linearCacheData {
    bool valid;
    size_t i;
    double par[2];
};
typedef struct _linearCacheData LinearCacheData;

static void *linearCacheItemNew(long i) {
    LinearCacheData *d = (LinearCacheData *)malloc(sizeof(LinearCacheData));
    if (! d) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    d->valid = false;
    d->i     = 0;
    return (void *)d;
}

/* create new linear cache */
Item *linearCacheNew(int len) {
    return chainNew(len, 1, linearCacheItemNew);
}

static void linearCacheItemFree(void *i) {
    free((LinearCacheData *)i);
}

/* delete linear cache */
void linearCacheFree(Item *c) {
    chainFree(c, linearCacheItemFree);
}

static void linearCacheItemReset(Item *i) {
    ((LinearCacheData *)(i->data))->valid = false;
}

/* reset (wipeout) linear cache */
void linearCacheReset(Item *c) {
    chainApply(c, linearCacheItemReset);
}

static bool linearCacheSearch(Item *cache, size_t i, double *p) {
    Item *s = cache, *c = cache;
    LinearCacheData *ac;
    if (!c)
        return false;
    while(c->prev != s) {
        ac = (LinearCacheData *)(c->data);
        if ((ac->valid) && (ac->i == i)) {
            p[0] = ac->par[0];
            p[1] = ac->par[1];
            return true;
        }
        c = c->prev;
    }
    return false;
}

static INLINE void linearCacheSet(Item **cache, size_t i, double *p) {
    LinearCacheData *ac = (LinearCacheData *)((*cache)->data);
    ac->valid = true;
    ac->i = i;
    ac->par[0] = p[0];
    ac->par[1] = p[1];
    /* rotate cache one step */
    *cache = (*cache)->next;
}

double ncVar1DGetLinear(NcVar1D *var, double x) {
    size_t i;
    double xi, xj, yi, yj, par[2];
    i = ncDataSet1DSearch(var->dataSet, &x);
    if (i == var->dataSet->dim-1) i--;
    if (! linearCacheSearch((Item *)(var->parameterCache), i, par)) {
        var->pCacheStat[1]++;
        xi = ncDataSet1DGetItem(var->dataSet, i);
        yi = ncVar1DGetItem(var, i);
        xj = ncDataSet1DGetItem(var->dataSet, ++i);
        yj = ncVar1DGetItem(var, i);
        assert((xi != xj));
        par[0] = (yj-yi) / (xj-xi);
        par[1] = yi-par[0]*xi;
        if (var->parameterCache)
            /* store values in cache */
            linearCacheSet((Item **)(&(var->parameterCache)), --i, par);
    } else
        var->pCacheStat[0]++;
    return par[0]*x+par[1];
}

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
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "IpAkima.h"

static AkimaData *AD = NULL;

/* cache handling */

struct _akimaCacheData {
    bool valid;
    size_t i;
    double par[5];
};
typedef struct _akimaCacheData AkimaCacheData;

static void *akimaCacheItemNew(long i) {
    AkimaCacheData *d = (AkimaCacheData *)malloc(sizeof(AkimaCacheData));
    if (! d) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    d->valid = false;
    d->i     = 0;
    return (void *)d;
}

/* create new akima cache */
Item *akimaCacheNew(int len) {
    return chainNew(len, 1, akimaCacheItemNew);
}

static void akimaCacheItemFree(void *i) {
    free((AkimaCacheData *)i);
}

/* delete akima cache */
void akimaCacheFree(Item *c) {
    chainFree(c, akimaCacheItemFree);
}

static void akimaCacheItemReset(Item *i) {
    ((AkimaCacheData *)(i->data))->valid = false;
}

/* reset (wipeout) akima cache */
void akimaCacheReset(Item *c) {
    chainApply(c, akimaCacheItemReset);
}

static bool akimaCacheSearch(Item *cache, size_t i, double *p) {
    Item *s = cache, *c = cache;
    AkimaCacheData *ac;
    if (!c)
        return 0;
    while(c->prev != s) {
        ac = (AkimaCacheData *)(c->data);
        if ((ac->valid) && (ac->i == i)) {
            p[0] = ac->par[0];
            p[1] = ac->par[1];
            p[2] = ac->par[2];
            p[3] = ac->par[3];
            p[4] = ac->par[4];
            return 1;
        }
        c = c->prev;
    }
    return 0;
}

static INLINE void akimaCacheSet(Item **cache, size_t i, double *p) {
    AkimaCacheData *ac = (AkimaCacheData *)(((*cache)->next)->data);
    ac->valid = true;
    ac->i = i;
    ac->par[0] = p[0];
    ac->par[1] = p[1];
    ac->par[2] = p[2];
    ac->par[3] = p[3];
    ac->par[4] = p[4];
    /* rotate cache one step */
    *cache = (*cache)->next;
}


/* akima implementation */

#define  M(j) ((Y[j+1]-Y[j])/(X[j+1]-X[j]))
void akimaCalc(AkimaData *a) {
    size_t i, n;
    double *X, *Y, *B, *C, *D, tmp, mm1, mm2, mp1, mp2;
    double xm1, xm2, xp1, xp2, ym1, ym2, yp1, yp2;
    assert(a->dim > 0);
    n = a->dim; X = a->X; Y = a->Y; B = a->B; C = a->C; D = a->D;
    xm1 = X[0] + X[1] - X[2];
    ym1 = (X[0]-xm1) * (M(1) - 2.0 * M(0)) + Y[0];
    mm1 = (Y[0]-ym1)/(X[0]-xm1);
    xm2 = 2.0 * X[0] - X[2];
    ym2 = (xm1-xm2) * (M(0) - 2.0 * mm1) + ym1;
    mm2 = (ym1-ym2)/(xm1-xm2);
    xp1 = X[n-1] + X[n-2] - X[n-3];
    yp1 = (2.0 * M(n-2) - M(n-3)) * (xp1 - X[n-1]) + Y[n-1];
    mp1 = (yp1-Y[n-1])/(xp1-X[n-1]);
    xp2 = 2.0 * X[n-1] - X[n-3];
    yp2 = (2.0 * mp1 - M(n-2)) * (xp2 - xp1) + yp1;
    mp2 = (yp2-yp1)/(xp2-xp1);
    if ((tmp = fabs(M(1) - M(0)) + fabs(mm1 - mm2)) != 0.0)
        B[0] = (fabs(M(1) - M(0)) * mm1 + fabs(mm1 - mm2) * M(0))/tmp;
    else B[0] = 0.0;
    if ((tmp = fabs(M(2) - M(1)) + fabs(M(0) - mm1)) != 0.0)
        B[1] = (fabs(M(2) - M(1)) * M(0) + fabs(M(0) - mm1) * M(1))/tmp;
    else B[1] = 0.0;
    for (i=2; i < n-2; i++) {
        if ((tmp = fabs(M(i+1) - M(i)) + fabs(M(i-1) - M(i-2))) != 0.0)
            B[i] = (fabs(M(i+1) - M(i)) * M(i-1) + fabs(M(i-1) - M(i-2)) * M(i))/tmp;
        else B[i] = 0.0;
    }
    if ((tmp = fabs(mp1 - M(n-2)) + fabs(M(n-3) - M(n-4))) != 0.0)
        B[n-2] = (fabs(mp1 - M(n-2)) * M(n-3) + fabs(M(n-3) - M(n-4)) * M(n-2))/tmp;
    else B[n-2] = 0.0;
    if ((tmp = fabs(mp2 - mp1) + fabs(M(n-2) - M(n-3))) != 0.0)
        B[n-1] = (fabs(mp2 - mp1) * M(n-2) + fabs(M(n-2) - M(n-3)) * mp1)/tmp;
    else B[n-1] = 0.0;
    for (i=0; i < n-1; i++) {
        tmp = X[i+1]-X[i];
        C[i] = (3.0 * M(i) - 2.0 * B[i] - B[i+1]) / tmp;
        D[i] = (B[i] + B[i+1] - 2.0 * M(i)) / (tmp * tmp);
    }
}
#undef  M


double ncVar1DGetAkima(NcVar1D *var, double x) {
    size_t i, d, k;
    NcDataSet1D *dataSet;
    int j;
    long int li;
    double par[5], tmp;
    if (! AD) {
        AD = malloc(sizeof(AkimaData));
        AD->dim = 6;
        AD->X = malloc(6*sizeof(double));
        AD->Y = malloc(6*sizeof(double));
        AD->B = malloc(6*sizeof(double));
        AD->C = malloc(6*sizeof(double));
        AD->D = malloc(6*sizeof(double));
    }
    dataSet = var->dataSet;
    i = ncDataSet1DSearch(dataSet, &x);
    if (! akimaCacheSearch((Item *)(var->parameterCache), i, par)) {
        var->pCacheStat[1]++;
        d = dataSet->dim;
        if ((dataSet->extra == EpPeriodic) || ((i >= 2) && (i+4 <= d))) {
            for (j = 0; j < 6; j++) {
                li = i - 2 + j;
                if (li < 0) {
                    li += d - 1;
                    AD->X[j] = ncDataSet1DGetItem(dataSet, li) + dataSet->min - dataSet->max;
                    AD->Y[j] = ncVar1DGetItem(var, li);
                } else if (li >= d) {
                    li = (li % d) + 1;
                    AD->X[j] = ncDataSet1DGetItem(dataSet, li) - dataSet->min + dataSet->max;
                    AD->Y[j] = ncVar1DGetItem(var, li);
                } else {
                    AD->X[j] = ncDataSet1DGetItem(dataSet, li);
                    AD->Y[j] = ncVar1DGetItem(var, li);
                }
            }
            k = 2;
        } else if (i < 2) {
            for (j = 0; j < 6; j++) {
                li = j;
                AD->X[j] = ncDataSet1DGetItem(dataSet, li);
                AD->Y[j] = ncVar1DGetItem(var, li);
            }
            k = i;
        } else if (i+4 > d) {
            for (j = 0; j < 6; j++) {
                li = d - 6 + j;
                AD->X[j] = ncDataSet1DGetItem(dataSet, li);
                AD->Y[j] = ncVar1DGetItem(var, li);
            }
            k = 6 + i - d;
        }
        akimaCalc(AD);
        par[0] = AD->X[k];
        par[1] = AD->Y[k];
        par[2] = AD->B[k];
        par[3] = AD->C[k];
        par[4] = AD->D[k];
        if (var->parameterCache)
            /* store values in cache */
            akimaCacheSet((Item **)(&(var->parameterCache)), i, par);
    } else
        var->pCacheStat[0]++;
    tmp = x - par[0];
    return par[1] + par[2]*tmp + par[3]*tmp*tmp + par[4]*tmp*tmp*tmp;
}


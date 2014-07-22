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

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "netcdf.h"
#include "ncDataReader2.h"
#include "config.h"
#include "IpLinear.h"
#include "IpSinSteps.h"
#include "IpAkima.h"
#include "IpCosWin.h"

struct _valueCacheData {
    bool valid;
    double x, y;
};
typedef struct _valueCacheData ValueCacheData;


static void *valueCacheItemNew(long i) {
    ValueCacheData *d = (ValueCacheData *)malloc(sizeof(ValueCacheData));
    if (! d) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    d->valid = false;
    d->x = d->y = 0.0;
    return (void *)d;
}


Item *valueCacheNew(int len) {
    return chainNew(len, 1, valueCacheItemNew);
}


static void valueCacheItemFree(void *i) {
    free((ValueCacheData *)i);
}


void valueCacheFree(Item *c) {
    chainFree(c, valueCacheItemFree);
}


static void valueCacheItemReset(Item *i) {
    ((ValueCacheData *)(i->data))->valid = false;
}


void valueCacheReset(Item *c) {
    chainApply(c, valueCacheItemReset);
}


static bool valueCacheSearch(Item *cache, double x, double *y) {
    Item *s = cache, *c = cache;
    ValueCacheData *ac;
    if (!c)
        return false;
    while(c->prev != s) {
        ac = (ValueCacheData *)(c->data);
        if ((ac->valid) && (ac->x == x)) {
            *y = ac->y;
            return true;
        }
        c = c->prev;
    }
    return false;
}


static INLINE void valueCacheSet(Item **cache, double x, double y) {
    ValueCacheData *ac = (ValueCacheData *)((*cache)->data);
    ac->valid = true;
    ac->x = x;
    ac->y = y;
    /* rotate cache one step */
    *cache = (*cache)->next;
}


NcVar1D DLL_EXPORT *ncVar1DNew(NcDataSet1D *dataSet, const char *varName, Interpolation inter, LoadType loadType) {
    NcVar1D *var = NULL;
    size_t l;
    int ncV, ncD;
    assert(dataSet);
    if (ncError(nc_inq_varid(dataSet->fileId, varName, &ncV))) return var;
    if (ncError(nc_inq_varndims(dataSet->fileId, ncV, &ncD))) return var;
    if (ncD != 1) return var;
    nc_inq_vardimid(dataSet->fileId, ncV, &ncD);
    nc_inq_dimlen(dataSet->fileId, ncD, &l);
    if (l != dataSet->dim) return var;
    var = (NcVar1D *)malloc(sizeof(NcVar1D));
    if (! var) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    var->dataSet = dataSet;
    dataSet->refCount++;
    var->varId = ncV;
    if (loadType == LtAuto) {
        char *l = ncGetAttributeTextDefault(dataSet->fileId, ncV, NCATT_LOAD_TYPE, NCATT_LT_AUTO);
        if (strcmp(l, NCATT_LT_AUTO) == 0)
            loadType = ((dataSet->dim < LARGE_DATASET) ? LtFull : LtNone);
        else if (strcmp(l, NCATT_LT_FULL) == 0)  loadType = LtFull;
        else if (strcmp(l, NCATT_LT_NONE) == 0)  loadType = LtNone;
        else if (strcmp(l, NCATT_LT_CHUNK) == 0) loadType = LtChunk;
        else loadType = LtFull;
        free(l);
    }
    var->loadType = loadType;
    var->cache = NULL;
    var->loadCount = 0;
    switch (var->loadType) {
        case LtFull:
            var->cache = (double *)malloc(dataSet->dim * sizeof(double));
            if (var->cache) {
                nc_get_var_double(dataSet->fileId, ncV, var->cache);
                var->cacheIndex[0] = 0;
                var->cacheIndex[1] = dataSet->dim;
                var->loadCount++;
            } else {
                ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
                var->loadType = LtNone; /* fall back */
            }
            break;
        case LtChunk:
            l = (LARGE_DATASET < dataSet->dim ? LARGE_DATASET : dataSet->dim);
            var->chunkSize = ncGetAttributeLongDefault(dataSet->fileId, ncV, NCATT_CHUNK_SIZE, l);
            var->cacheIndex[0] = 0;
            var->cacheIndex[1] = 0;
            break;
        case LtNone:
            break;
        default:
            ncdrError(NCDR_EINVAL, NCDR_EINVAL_TXT);
    }
    if (inter == IpAuto) {
        char *i = ncGetAttributeTextDefault(dataSet->fileId, ncV, NCATT_INTERPOLATION, NCATT_IP_AKIMA);
        if (strcmp(i, NCATT_IP_DISCRETE) == 0)
            inter = IpDiscrete;
        else if (strcmp(i, NCATT_IP_LINEAR) == 0)
            inter = IpLinear;
        else if (strcmp(i, NCATT_IP_SINSTEPS) == 0)
            inter = IpSinSteps;
        else if (strcmp(i, NCATT_IP_COSWIN) == 0)
            inter = IpCosWin;
        else
            inter = IpAkima;
        free(i);
    }
    /* read default values for scaling and smoothing */
    var->inter = inter;
    var->scale[0]  = ncGetAttributeDoubleDefault(dataSet->fileId, ncV, NCATT_SCALE_FACTOR, 1.0);
    var->scale[1]  = ncGetAttributeDoubleDefault(dataSet->fileId, ncV, NCATT_ADD_OFFSET, 0.0);
    if (inter == IpSinSteps)
        var->smoothing = ncGetAttributeDoubleDefault(dataSet->fileId, ncV, NCATT_SMOOTHING, 0.0);
    if (inter == IpCosWin)
        var->smoothing = ncGetAttributeDoubleDefault(dataSet->fileId, ncV, NCATT_WINDOW_SIZE, 0.01);
    /* initialize value cache */
    var->valueCache = NULL;
    if ((l = ncGetAttributeLongDefault(dataSet->fileId, ncV, NCATT_VALUE_CACHE, 0)) > 0)
        var->valueCache = (void *)valueCacheNew(l);
    var->vCacheStat[0] = 0; var->vCacheStat[1] = 0;
    /* initialize parameter cache */
    var->parameterCache = NULL;
    if ((l = ncGetAttributeLongDefault(dataSet->fileId, ncV, NCATT_PARAMETER_CACHE, 0)) > 0) {
        switch (var->inter) {
            case IpAkima:
                var->parameterCache = (void *)akimaCacheNew(l);
                break;
            case IpLinear:
                var->parameterCache = (void *)linearCacheNew(l);
                break;
            default:
                ;
        }
    }
    var->pCacheStat[0] = 0; var->pCacheStat[1] = 0;
    /* adjust ends for periodic usage !*/
    if (var->dataSet->extra == EpPeriodic) {
        double a, b;
        l = 0;
        nc_get_var1_double(var->dataSet->fileId, var->varId, &l, &a);
        a = var->scale[0]* a + var->scale[1];
        l = var->dataSet->dim-1;
        nc_get_var1_double(var->dataSet->fileId, var->varId, &l, &b);
        b = var->scale[0]* b + var->scale[1];
        var->transition = 0.5 * (a+b);
    }
    return var;
}


void DLL_EXPORT ncVar1DFree(NcVar1D *var) {
    assert(var);
    assert(var->dataSet);
    assert(var->dataSet->refCount > 0);
    var->dataSet->refCount--;
    if (var->cache) free(var->cache);
    if (var->valueCache) {
        valueCacheFree((Item *)(var->valueCache));
    }
    if (var->parameterCache)
        switch (var->inter) {
            case IpAkima:
                akimaCacheFree((Item *)(var->parameterCache));
                break;
            case IpLinear:
                linearCacheFree((Item *)(var->parameterCache));
                break;
            default:
                /* no other interpolation with parameter cache for now */ ;
        }
    free(var);
}


double DLL_EXPORT ncVar1DGet(NcVar1D *var, double x) {
    double y;
    if (! valueCacheSearch((Item *)(var->valueCache), x, &y)) {
        var->vCacheStat[1]++;
        switch (var->inter) {
            case IpDiscrete:
                y = ncVar1DGetItem(var, ncDataSet1DSearch(var->dataSet, &x)); break;
            case IpLinear: 
                y = ncVar1DGetLinear(var, x); break;
            case IpAkima:
                y = ncVar1DGetAkima(var, x); break;
            case IpSinSteps:
                y = ncVar1DGetSinSteps(var, x); break;
            case IpCosWin:
                y = ncVar1DGetCosWin(var, x); break;
            default:
                /* should never happen! */
                y = x;
        }
        if (var->valueCache)
            valueCacheSet((Item **)(&(var->valueCache)), x, y);
    } else
        var->vCacheStat[0]++;
    return y;
}


static void loadChunk(NcVar1D *var, size_t start) {
    size_t d = var->dataSet->dim;
    if (! var->cache) /* allocate memory */
        var->cache = (double *)malloc(var->chunkSize * sizeof(double));
    else if ((var->cacheIndex[1] - var->cacheIndex[0]) != var->chunkSize)
        var->cache = (double *)realloc(var->cache, var->chunkSize * sizeof(double));
    if (! var->cache) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    if ((start + var->chunkSize) > d) /* adjust start */
        start = d - var->chunkSize;
    nc_get_vara_double(var->dataSet->fileId, var->varId, &start, &(var->chunkSize), var->cache);
    var->loadCount++;
    var->cacheIndex[0] = start;
    var->cacheIndex[1] = start + var->chunkSize - 1;
}


double DLL_EXPORT ncVar1DGetItem(NcVar1D *var, size_t i) {
    double d;
    size_t st;
    assert((i < var->dataSet->dim));
    if ((var->dataSet->extra == EpPeriodic) && 
        ((i == 0) || (i == var->dataSet->dim-1)))
        return var->transition;
    switch (var->loadType) {
        case LtFull:
            d = var->cache[i];
            break;
        case LtChunk:
            if ((! var->cache) || (i < var->cacheIndex[0]) || (i > var->cacheIndex[1])) {
                /* load another chunk, try to center around i */
                size_t tmp = (var->chunkSize) / 2;
                loadChunk(var, (i>tmp ? i-tmp : 0));
            }
            st = i - var->cacheIndex[0];
            d = var->cache[st];
            break;
        case LtNone:
            ncError(nc_get_var1_double(var->dataSet->fileId, var->varId, &i, &d));
            var->loadCount++;
            break;
    }
    return var->scale[0]* d + var->scale[1];
}


int DLL_EXPORT ncVar1DSetOption(NcVar1D *var, VarOption option, ...) {
    int i, res = 0;
    va_list ap;
    va_start(ap, option);
    switch (option) {
        case OpVarParameterCacheSize:
            i = va_arg(ap, int);
            switch (var->inter) {
                case IpAkima:
                    if (var->parameterCache)
                        akimaCacheFree((Item *)(var->parameterCache));
                    if (i > 0)
                        var->parameterCache = (void *)akimaCacheNew(i);
                    else
                        var->parameterCache = NULL;
                    res = 1;
                    break;
                case IpLinear:
                    if (var->parameterCache)
                        linearCacheFree((Item *)(var->parameterCache));
                    if (i > 0)
                        var->parameterCache = (void *)linearCacheNew(i);
                    else
                        var->parameterCache = NULL;
                    res = 1;
                    break;
                default:
                    ;
            }
            break;
        case OpVarValueCacheSize:
            i = va_arg(ap, int);
            if (var->valueCache)
                valueCacheFree((Item *)(var->valueCache));
            if (i > 0)
                var->valueCache = (void *)valueCacheNew(i);
            else
                var->valueCache = NULL;
            res = 1;
            break;
        case OpVarSmoothing:
            if (var->inter == IpSinSteps) {
                var->smoothing = va_arg(ap, double);
                res = 1;
            }
            break;
        case OpVarWindowSize:
            if (var->inter == IpCosWin) {
                var->smoothing = va_arg(ap, double);
                res = 1;
            }
            break;
        case OpVarScaling:
            var->scale[0] = va_arg(ap, double);
            var->scale[1] = va_arg(ap, double);
            valueCacheReset(var->valueCache);
            switch (var->inter) {
                case IpAkima:
                    akimaCacheReset(var->parameterCache);
                    break;
                case IpLinear:
                    linearCacheReset(var->parameterCache);
                    break;
                default:
                    ;
            }
            if (var->dataSet->extra == EpPeriodic) {
                double a, b;
                size_t l = 0;
                nc_get_var1_double(var->dataSet->fileId, var->varId, &l, &a);
                a = var->scale[0]* a + var->scale[1];
                l = var->dataSet->dim-1;
                nc_get_var1_double(var->dataSet->fileId, var->varId, &l, &b);
                b = var->scale[0]* b + var->scale[1];
                var->transition = 0.5 * (a+b);
            }
            res = 1;
            break;
        case OpVarChunkSize:
            var->chunkSize = va_arg(ap, size_t);
            if (var->chunkSize > var->dataSet->dim)
                var->chunkSize = var->dataSet->dim;
            if (var->chunkSize <= 2)
                var->chunkSize = 3;
            res = 1;
            break;
        default:
            ncdrError(NCDR_EINVAL, NCDR_EINVAL_TXT);
    }
    va_end(ap);
    return res;
}


void DLL_EXPORT ncVar1DDumpStatistics(NcVar1D *var, FILE *f) {
    char ctmp[1024];
    if (f==NULL) f=stdout;
    nc_inq_varname(var->dataSet->fileId, var->varId, ctmp);
    fprintf(f, "Var1D: %s\n", ctmp);
    nc_inq_varname(var->dataSet->fileId, var->dataSet->varId, ctmp);
    fprintf(f, "  DataSet:              %s\n", ctmp);
    fprintf(f, "  LoadType:             ");
    switch (var->loadType) {
        case LtFull:
            fprintf(f, "full (at initialization time)\n");
            break;
        case LtNone:
            fprintf(f, "none (every value on demand)\n");
            break;
        case LtChunk:
            fprintf(f, "chunks ("SIZET_FMT" values on demand)\n", var->chunkSize);
    }
    fprintf(f, "  Interpolation:        ");
    switch (var->inter) {
        case IpDiscrete:
            fprintf(f, "discrete\n");
            break;
        case IpLinear:
            fprintf(f, "linear\n");
            break;
        case IpSinSteps:
            fprintf(f, "sinsteps\n");
            break;
        case IpCosWin:
            fprintf(f, "coswin\n");
            break;
        case IpAkima:
            fprintf(f, "akima\n");
            break;
    }
    fprintf(f, "  LoadCount:            "SIZET_FMT"\n", var->loadCount);
    fprintf(f, "  ValueCalc./Cache:     "SIZET_FMT"/"SIZET_FMT"\n", var->vCacheStat[1], var->vCacheStat[0]);
    fprintf(f, "  ParameterCalc./Cache: "SIZET_FMT"/"SIZET_FMT"\n\n", var->pCacheStat[1], var->pCacheStat[0]);
}

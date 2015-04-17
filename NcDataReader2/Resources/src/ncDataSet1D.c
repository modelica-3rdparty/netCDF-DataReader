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
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include "netcdf.h"
#include "ncDataReader2.h"
#include "config.h"
#include "Lookup.h"


NcDataSet1D DLL_EXPORT *ncDataSet1DNew(const char *fileName, const char *varName, Extrapolation extra, LoadType loadType, int lookupCacheSize) {
    int ncF, ncV, ncD;
    size_t l;
    double d;
    NcDataSet1D *dataSet = NULL;
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) return dataSet;
    if (ncError(nc_inq_varid(ncF, varName, &ncV))) return dataSet;
    if (ncError(nc_inq_varndims(ncF, ncV, &ncD))) return dataSet;
    if (ncD != 1) return dataSet;
    dataSet = (NcDataSet1D *)malloc(sizeof(NcDataSet1D));
    if (! dataSet) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    dataSet->fileId = ncF;
    dataSet->varId = ncV;
    dataSet->refCount = 0;
    dataSet->scale[0] = ncGetAttributeDoubleDefault(ncF, ncV, NCATT_SCALE_FACTOR, 1.0);
    dataSet->scale[1] = ncGetAttributeDoubleDefault(ncF, ncV, NCATT_ADD_OFFSET, 0.0);
    
    nc_inq_vardimid(ncF, ncV, &ncD);
    nc_inq_dimlen(ncF, ncD, &(dataSet->dim));
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
    dataSet->loadType = loadType;
    if (extra == EpAuto) {
        char *e = ncGetAttributeTextDefault(ncF, ncV, NCATT_EXTRAPOLATION, NCATT_EP_DEFAULT);
        if (strcmp(e, NCATT_EP_PERIODIC) == 0)
            extra = EpPeriodic;
        else if (strcmp(e, NCATT_EP_CONSTANT) == 0)
            extra = EpConstant;
        else
            extra = EpDefault;
        free(e);
    }
    dataSet->extra = extra;
    dataSet->cache = NULL;
    dataSet->loadCount = 0;
    switch (dataSet->loadType) {
        case LtFull:
            dataSet->cache = (double *)malloc(dataSet->dim * sizeof(double));
            if (dataSet->cache) {
                nc_get_var_double(ncF, ncV, dataSet->cache);
                dataSet->cacheIndex[0] = 0;
                dataSet->cacheIndex[1] = dataSet->dim;
                dataSet->loadCount++;
            } else {
                ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
                dataSet->loadType = LtNone; /* fall back */
            }
            break;
        case LtChunk:
            l = (LARGE_DATASET < dataSet->dim ? LARGE_DATASET : dataSet->dim);
            dataSet->chunkSize = ncGetAttributeLongDefault(ncF, ncV, NCATT_CHUNK_SIZE, l);
            dataSet->cacheIndex[0] = 0;
            dataSet->cacheIndex[1] = 0;
            break;
        case LtNone:
            break;
        default:
            ncdrError(NCDR_EINVAL, NCDR_EINVAL_TXT);
    }    
    l = 0;
    nc_get_var1_double(ncF, ncV, &l, &d);
    dataSet->min = dataSet->scale[0]* d + dataSet->scale[1];
    l = dataSet->dim-1;
    nc_get_var1_double(ncF, ncV, &l, &d);
    dataSet->max = dataSet->scale[0]* d + dataSet->scale[1];
    if (lookupCacheSize < 0)
        lookupCacheSize = ncGetAttributeLongDefault(dataSet->fileId, ncV, NCATT_LOOKUP_CACHE, 0);
    if (lookupCacheSize > 0)
        dataSet->lookupCache = (void *)lookupCacheNew(lookupCacheSize);
    else
        dataSet->lookupCache = NULL;
    dataSet->lCacheStat[0] = 0; dataSet->lCacheStat[1] = 0;
    
    return dataSet;
}


void DLL_EXPORT ncDataSet1DFree(NcDataSet1D *dataSet) {
    assert(dataSet);
    assert(dataSet->refCount == 0);
    if (dataSet->cache) free(dataSet->cache);
    if (dataSet->lookupCache) lookupCacheFree((Item *)(dataSet->lookupCache));
    ncError(nc_close(dataSet->fileId));
    free(dataSet);
}


static INLINE double _adjustPeriodic(double x, double s, double e) {
    return (((x >= s) && (x <= e)) ? x : s + fmod(x-s, e-s) + ((x >= s)? 0.0 : e-s));
}


static INLINE double _adjustRange(double x, double s, double e) {
    return ((x < s) ? s : ((x > e) ? e : x));
}


size_t DLL_EXPORT ncDataSet1DSearch(NcDataSet1D *dataSet, double *x) {
    if (dataSet->extra == EpPeriodic)
        *x = _adjustPeriodic(*x, dataSet->min, dataSet->max);
    else if (dataSet->extra == EpConstant)
        *x = _adjustRange(*x, dataSet->min, dataSet->max);
    return ncDataSet1DLookupValue(dataSet, *x);
}


static void loadChunk(NcDataSet1D *dataSet, size_t start) {
    if (! dataSet->cache) /* allocate memory */
        dataSet->cache = (double *)malloc(dataSet->chunkSize * sizeof(double));
    else if ((dataSet->cacheIndex[1] - dataSet->cacheIndex[0]) != dataSet->chunkSize)
        dataSet->cache = (double *)realloc(dataSet->cache, dataSet->chunkSize * sizeof(double));
    if (! dataSet->cache) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    if ((start + dataSet->chunkSize) > dataSet->dim) /* adjust start */
        start = dataSet->dim - dataSet->chunkSize;
    nc_get_vara_double(dataSet->fileId, dataSet->varId, &start, &(dataSet->chunkSize), dataSet->cache);
    dataSet->loadCount++;
    dataSet->cacheIndex[0] = start;
    dataSet->cacheIndex[1] = start + dataSet->chunkSize - 1;
}


double DLL_EXPORT ncDataSet1DGetItem(NcDataSet1D *dataSet, size_t i) {
    double d;
    size_t st;
    assert((i < dataSet->dim));
    switch (dataSet->loadType) {
        case LtFull:
            d = dataSet->cache[i];
            break;
        case LtNone:
            ncError(nc_get_var1_double(dataSet->fileId, dataSet->varId, &i, &d));
            dataSet->loadCount++;
            break;
        case LtChunk:
            if ((! dataSet->cache) || (i < dataSet->cacheIndex[0]) || (i > dataSet->cacheIndex[1])) {
                /* load another chunk, try to center around i */
                size_t tmp = (dataSet->chunkSize) / 2;
                loadChunk(dataSet, (i>tmp ? i-tmp : 0));
            }
            st = i - dataSet->cacheIndex[0];
            d = dataSet->cache[st];
            break;
    }
    return dataSet->scale[0]* d + dataSet->scale[1];
}


int DLL_EXPORT ncDataSet1DSetOption(NcDataSet1D *dataSet, DataSetOption option, ...) {
    int i, res = 0;
    size_t l;
    double d;
    va_list ap;
    va_start(ap, option);
    switch (option) {
        case OpDataSetLookupCacheSize:
            i = va_arg(ap, int);
            if (dataSet->lookupCache) 
                lookupCacheFree((Item *)(dataSet->lookupCache));
            if (i > 0) 
                dataSet->lookupCache = lookupCacheNew(i);
            else
                dataSet->lookupCache = NULL;
            res = 1;
            break;
        case OpDataSetScaling:
            dataSet->scale[0] = va_arg(ap, double);
            dataSet->scale[1] = va_arg(ap, double);
            lookupCacheReset(dataSet->lookupCache);
            l = 0;
            nc_get_var1_double(dataSet->fileId, dataSet->varId, &l, &d);
            dataSet->min = dataSet->scale[0]* d + dataSet->scale[1];
            l = dataSet->dim-1;
            nc_get_var1_double(dataSet->fileId, dataSet->varId, &l, &d);
            dataSet->max = dataSet->scale[0]* d + dataSet->scale[1];
            res = 1;
            break;
        case OpDataSetChunkSize:
            dataSet->chunkSize = va_arg(ap, size_t);
            if (dataSet->chunkSize > dataSet->dim)
                dataSet->chunkSize = dataSet->dim;
            if (dataSet->chunkSize <= 2)
                dataSet->chunkSize = 3;
            res = 1;
            break;
        default:
            ncdrError(NCDR_EINVAL, NCDR_EINVAL_TXT);
    }
    va_end(ap);
    return res;
}


void DLL_EXPORT ncDataSet1DDumpStatistics(NcDataSet1D *dataSet, FILE *f) {
    char ctmp[1024];
    if (f == NULL) f=stdout;
    nc_inq_varname(dataSet->fileId, dataSet->varId, ctmp);
    fprintf(f, "DataSet1D: %s\n", ctmp);
    fprintf(f, "  Size:          "SIZET_FMT"\n", dataSet->dim);
    fprintf(f, "  LoadType:      ");
    switch (dataSet->loadType) {
        case LtFull:
            fprintf(f, "full (at initialization time)\n");
            break;
        case LtNone:
            fprintf(f, "none (every value on demand)\n");
            break;
        case LtChunk:
            fprintf(f, "chunks ("SIZET_FMT" values on demand)\n", dataSet->chunkSize);
    }
    fprintf(f, "  Extrapolation: ");
    switch (dataSet->extra) {
        case EpPeriodic:
            fprintf(f, "periodic\n");
            break;
        case EpConstant:
            fprintf(f, "constant\n");
            break;
        case EpDefault:
            fprintf(f, "default (depends on interpolation)\n");
            break;
    }
    fprintf(f, "  LoadCount:     "SIZET_FMT"\n", dataSet->loadCount);
    fprintf(f, "  Lookups/Cache: "SIZET_FMT"/"SIZET_FMT"\n\n", dataSet->lCacheStat[1], dataSet->lCacheStat[0]);
}

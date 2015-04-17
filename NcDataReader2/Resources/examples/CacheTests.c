#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <ncDataReader2.h>

#define RAND6() ceil((double)rand()/RAND_MAX * 6.0)
#define RAND01() (rand()/(double)RAND_MAX)

char *ncFileName;

void lookupCacheTest(void) {
    NcDataSet1D *dset;
    NcVar1D *vaki;
    int i, sizes[] = {0, 2, 4, 8, 16, 32, 64};
    double x, y, start, end, step, noise;
    clock_t cstart;
    long int ch, cf;

    dset = ncDataSet1DNew(ncFileName, "time", EpPeriodic, LtFull, 0);
    vaki = ncVar1DNew(dset, "test1D", IpDiscrete, LtFull);
    ncVar1DSetOption(vaki, OpVarParameterCacheSize, 0);
    ncVar1DSetOption(vaki, OpVarValueCacheSize, 0);
    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.000001 *(end - start);

    printf("\n### testing size of lookup cache\n");
    printf("size | calc. time | cache hit ratio\n");
    for (i=0; i < 7; i++) {
        ncDataSet1DSetOption(dset, OpDataSetLookupCacheSize, sizes[i]);
        cstart = clock();
        for (x = start; x < end; x+= step) {
            noise = 0.5*(RAND01()-0.5);
            y = ncVar1DGet(vaki, x+noise);
        }
        ch = dset->lCacheStat[0];
        cf = dset->lCacheStat[1];
        dset->lCacheStat[0] = 0;
        dset->lCacheStat[1] = 0;
        printf("% 4d | % 10ld | %6.2f%% (%7ld / %ld)\n", sizes[i], clock()-cstart,
               100.0*ch/(ch+cf), ch, ch+cf);
    }
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


void parameterCacheTest(void) {
    NcDataSet1D *dset;
    NcVar1D *vaki;
    int i, sizes[] = {0, 2, 4, 8, 16, 32, 64};
    double x, y, start, end, step, noise;
    clock_t cstart;
    long int ch, cf;
    dset = ncDataSet1DNew(ncFileName, "time", EpDefault, LtFull, 0);
    vaki = ncVar1DNew(dset, "test1D", IpAkima, LtFull);

    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.000001 *(end - start);

    printf("\n### testing size of parameter cache (Akima)\n");
    printf("size | calc. time | cache hit ratio\n");
    /* x goes straight from start to end, noise adds a little noise to x:
       - without noise even a small cache is much better than no cache,
       - with a lot of noise the success depends on the cache size,
       sometimes no cache at all is even better...

       With IpLinear instead of IpAkima the speed gain because of the cache 
       is much smaller because the calculation of parameters is much faster
    */
    for (i=0; i < 7; i++) {
        cstart = clock();
        ncVar1DSetOption(vaki, OpVarParameterCacheSize, sizes[i]);
        for (x = start; x < end; x+= step) {
            noise = 0.3*(RAND01()-0.5);
            y = ncVar1DGet(vaki, x+noise);
        }
        ch = vaki->pCacheStat[0];
        cf = vaki->pCacheStat[1];
        vaki->pCacheStat[0] = 0;
        vaki->pCacheStat[1] = 0;
        printf("% 4d | % 10ld | %6.2f%% (%7ld / %ld)\n", sizes[i], clock()-cstart,
               100.0*ch/(ch+cf), ch, ch+cf);
    }
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


void valueCacheTest(void) {
    NcDataSet1D *dset;
    NcVar1D *vaki;
    int i, sizes[] = {0, 2, 4, 8, 16, 32, 64};
    double x, y, start, end, step, noise;
    clock_t cstart;
    long int ch, cf;
    dset = ncDataSet1DNew(ncFileName, "time", EpDefault, LtFull, 0);
    vaki = ncVar1DNew(dset, "test1D", IpAkima, LtFull);
    ncVar1DSetOption(vaki, OpVarParameterCacheSize, 0);

    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.000001 *(end - start);

    printf("\n### testing size of value cache (Akima)\n");
    printf("size | calc. time | cache hit ratio\n");
    for (i=0; i < 7; i++) {
        cstart = clock();
        ncVar1DSetOption(vaki, OpVarValueCacheSize, sizes[i]);
        for (x = start; x < end; x+= step) {
            noise = RAND6()*step;
            y = ncVar1DGet(vaki, x+noise);
        }
        ch = vaki->vCacheStat[0];
        cf = vaki->vCacheStat[1];
        vaki->vCacheStat[0] = 0;
        vaki->vCacheStat[1] = 0;
        printf("% 4d | % 10ld | %6.2f%% (%7ld / %ld)\n", sizes[i], clock()-cstart,
               100.0*ch/(ch+cf), ch, ch+cf);
    }
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


int main(int argc, char *argv[]) {
    if (argc > 1) ncFileName = argv[1];
    else          ncFileName = "testfile.nc";

    lookupCacheTest();
    parameterCacheTest();
    valueCacheTest();
    return 0;
}
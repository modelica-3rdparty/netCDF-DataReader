#include "ncDataReader2.h"

#define FILENAME "/var/tmp/testfilebig.nc"

static NcDataSet1D *dset = NULL;
static NcVar1D *v1, *v2;

static void myInit() {
    dset = ncDataSet1DNew(FILENAME, "time", EpPeriodic, LtFull, 10);
    
    v1 = ncVar1DNew(dset, "big_var_01", IpAkima, LtChunk);
    ncVar1DSetOption(v1, OpVarParameterCacheSize, 10);
    ncVar1DSetOption(v1, OpVarChunkSize, 100);
    
    v2 = ncVar1DNew(dset, "big_var_03", IpSinSteps, LtChunk);
    ncVar1DSetOption(v2, OpVarSmoothing, 0.001);
    ncVar1DSetOption(v2, OpVarChunkSize, 100);
}

double getV1(double x){
    if (dset == NULL)
        myInit();
    return ncVar1DGet(v1, x);
};

double getV2(double x){
    if (dset == NULL)
        myInit();
    return ncVar1DGet(v2, x);
};

#ifdef TEST_ME
#include <stdio.h>

int main(void) {
    double t = 42.0;
    printf("%g\t%g\t%g\n", t, getV1(t), getV2(t));
    return 0;
}
#endif

#ifndef NCFULLAPIFUNCTIONS_H
#define NCFULLAPIFUNCTIONS_H 

#include "ncDataReader2.h"

#define FILENAME "../../testfilebig.nc"

/* dataset and variable information */
static NcDataSet1D *dset = NULL;
static NcVar1D *v1, *v2;

/* initialization function */
static void myInit() {
    dset = ncDataSet1DNew(FILENAME, "time", EpPeriodic, LtFull, 10);
    
    v1 = ncVar1DNew(dset, "big_var_01", IpAkima, LtChunk);
    ncVar1DSetOption(v1, OpVarParameterCacheSize, 10);
    ncVar1DSetOption(v1, OpVarChunkSize, 100);
    
    v2 = ncVar1DNew(dset, "big_var_03", IpSinSteps, LtChunk);
    ncVar1DSetOption(v2, OpVarSmoothing, 0.001);
    ncVar1DSetOption(v2, OpVarChunkSize, 100);
}

/* functions to return the interpolated values -
 * myInit() is executed at the first call to one of the functions */

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


/* just a simple main routine to test the functions standalone,
 * not needed for Modelica */

#ifdef TEST_ME
#include <stdio.h>

int main(void) {
    double t = 42.0;
    printf("%g\t%g\t%g\n", t, getV1(t), getV2(t));
    return 0;
}
#endif

#endif

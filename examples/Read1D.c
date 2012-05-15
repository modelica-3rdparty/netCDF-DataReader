#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncDataReader2.h>

#define NCFILE "testfile.nc"

void testAuto(void) {
    NcDataSet1D *dset;
    NcVar1D *v;
    double x, start, end, step;
    FILE *outf = fopen("data_1D_auto.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return;
    }
    fprintf(outf, "# you may plot this file with gnuplot\n");
    dset = ncDataSet1DNew(NCFILE, "time", EpAuto, LtAuto, 0);
    v    = ncVar1DNew(dset, "test1D", IpAuto, LtAuto);

    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.0001 *(end - start);
    for (x = start; x < end; x+= step) {
        fprintf(outf, "%g\t%g\n", x, ncVar1DGet(v, x));
    }
    fclose(outf);
    ncVar1DFree(v);
    ncDataSet1DFree(dset);
}


void testDefaultEP(void) {
    NcDataSet1D *dset;
    NcVar1D *vdis, *vlin, *vsin, *vaki;
    double x, start, end, step;
    FILE *outf = fopen("data_1D_default.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return;
    }
    fprintf(outf, "# you may plot this file with gnuplot\n");
    dset = ncDataSet1DNew(NCFILE, "time", EpDefault, LtFull, 10);
    vdis = ncVar1DNew(dset, "test1D", IpDiscrete, LtFull);
    vlin = ncVar1DNew(dset, "test1D", IpLinear, LtFull);
    vsin = ncVar1DNew(dset, "test1D", IpSinSteps, LtFull);
    vaki = ncVar1DNew(dset, "test1D", IpAkima, LtFull);

    ncVar1DSetOption(vsin, OpVarSmoothing, 0.01);
    ncVar1DSetOption(vaki, OpVarParameterCacheSize, 10);

    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.0001 *(end - start);
    for (x = start; x < end; x+= step) {
        fprintf(outf, "%g\t%g\t%g\t%g\t%g\n", x, ncVar1DGet(vdis, x), 
               ncVar1DGet(vlin, x), ncVar1DGet(vsin, x), ncVar1DGet(vaki, x));
    }
    fclose(outf);
    ncVar1DFree(vdis);
    ncVar1DFree(vlin);
    ncVar1DFree(vsin);
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


void testPeriodicEP(void) {
    NcDataSet1D *dset;
    NcVar1D *vdis, *vlin, *vsin, *vaki;
    double x, start, end, step;
    FILE *outf = fopen("data_1D_periodic.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return;
    }
    fprintf(outf, "# you may plot this file with gnuplot\n");
    dset = ncDataSet1DNew(NCFILE, "time", EpPeriodic, LtFull, 10);
    vdis = ncVar1DNew(dset, "test1D", IpDiscrete, LtFull);
    vlin = ncVar1DNew(dset, "test1D", IpLinear, LtFull);
    vsin = ncVar1DNew(dset, "test1D", IpSinSteps, LtFull);
    vaki = ncVar1DNew(dset, "test1D", IpAkima, LtFull);

    ncVar1DSetOption(vsin, OpVarSmoothing, 0.01);
    ncVar1DSetOption(vaki, OpVarParameterCacheSize, 10);

    start = dset->min - 3.0 * (dset->max - dset->min);
    end   = dset->max + 3.0 * (dset->max - dset->min);
    step  = 0.0001 *(end - start);
    for (x = start; x < end; x+= step) {
        fprintf(outf, "%g\t%g\t%g\t%g\t%g\n", x, ncVar1DGet(vdis, x), 
               ncVar1DGet(vlin, x), ncVar1DGet(vsin, x), ncVar1DGet(vaki, x));
    }
    fclose(outf);
    ncVar1DFree(vdis);
    ncVar1DFree(vlin);
    ncVar1DFree(vsin);
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


int main(void) {
  testAuto();
  testDefaultEP();
  testPeriodicEP();
  return 0;
}
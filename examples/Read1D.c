#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncDataReader2.h>

char *ncFileName;

void testEPAuto(void) {
    NcDataSet1D *dset;
    NcVar1D *v;
    double x, start, end, step;
    FILE *outf = fopen("data_1D_auto.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return;
    }
    fprintf(outf, "# you may plot this file with gnuplot\n");
    dset = ncDataSet1DNew(ncFileName, "time", EpAuto, LtAuto, 0);
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


void testEP(Extrapolation extra, char *fileName) {
    NcDataSet1D *dset;
    NcVar1D *vdis, *vlin, *vsin, *vcos, *vaki;
    double x, start, end, step;
    FILE *outf = fopen(fileName, "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return;
    }
    fprintf(outf, "# you may plot this file with gnuplot\n");
    dset = ncDataSet1DNew(ncFileName, "time", extra, LtFull, 10);
    vdis = ncVar1DNew(dset, "test1D", IpDiscrete, LtFull);
    vlin = ncVar1DNew(dset, "test1D", IpLinear, LtFull);
    vsin = ncVar1DNew(dset, "test1D", IpSinSteps, LtFull);
    vcos = ncVar1DNew(dset, "test1D", IpCosWin, LtFull);
    vaki = ncVar1DNew(dset, "test1D", IpAkima, LtFull);

    ncVar1DSetOption(vsin, OpVarSmoothing, 0.01);
    ncVar1DSetOption(vcos, OpVarWindowSize, 0.1);
    ncVar1DSetOption(vaki, OpVarParameterCacheSize, 10);

    start = dset->min - 0.3 * (dset->max - dset->min);
    end   = dset->max + 0.3 * (dset->max - dset->min);
    step  = 0.00001 *(end - start);
    for (x = start; x < end; x+= step) {
        fprintf(outf, "%g\t%g\t%g\t%g\t%g\t%g\n", x, ncVar1DGet(vdis, x), 
               ncVar1DGet(vlin, x), ncVar1DGet(vsin, x), ncVar1DGet(vcos, x),
               ncVar1DGet(vaki, x));
    }
    fclose(outf);
    ncVar1DFree(vdis);
    ncVar1DFree(vlin);
    ncVar1DFree(vsin);
    ncVar1DFree(vcos);
    ncVar1DFree(vaki);
    ncDataSet1DFree(dset);
}


int main(int argc, char *argv[]) {
    if (argc > 1) ncFileName = argv[1];
    else          ncFileName = "testfile.nc";
    
    testEPAuto();
    testEP(EpDefault,  "data_1D_default.dat");
    testEP(EpConstant, "data_1D_constant.dat");
    testEP(EpPeriodic, "data_1D_periodic.dat");
    return 0;
}

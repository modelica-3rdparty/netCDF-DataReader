#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ncDataReader2.h>

#define NCFILE "testfilebig.nc"

void testChunk(void) {
    NcDataSet1D *dset;
    NcVar1D *v;
    double ix, istart, iend, istep;
    FILE *outf = fopen("data_1D_chunk.dat", "w");
    fprintf(outf, "# you may plot this file with gnuplot\n");
    
    dset = ncDataSet1DNew(NCFILE, "time", EpAuto, LtFull, 0);

    v = ncVar1DNew(dset, "big_var_00", IpAkima, LtAuto);
    
    istart = dset->min;
    iend   = dset->max - 0.1 * (dset->max - dset->min);
    istep  = (iend - istart) / 20;
    for (ix = istart; ix < iend; ix += istep) {
        double x, start, end, step;
        start = ix;
        end = start + istep / 300;
        step = 0.0001 * (end-start);
        for (x = start; x < end; x+= step) {
            fprintf(outf, "%10f\t%10f\n", x, ncVar1DGet(v, x));
        }
        fprintf(outf, "\n");
    }
    fclose(outf);
    ncVar1DFree(v);
    ncDataSet1DFree(dset);
}


int main(void) {
  testChunk();
  return 0;
}
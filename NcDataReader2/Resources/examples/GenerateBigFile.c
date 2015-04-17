#include <math.h>
#include <stdio.h>
#include <string.h>
#include "ncDataReader2.h"
#include "netcdf.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define RAND01() (rand()/(double)RAND_MAX)

void handleError(int status) {
if (status != NC_NOERR) {
   fprintf(stderr, "%s\n", nc_strerror(status));
   exit(-1);
   }
}


int numDigits (long n) {
    if (n == 0) return 1;
    return floor (log10 (abs (n))) + 1;
}


void createBigFile(char *fileName, long nVars, long nValues) {
    int ncF, ncD, *ncV, ncT;
    char c[101], f[101];
    size_t i, j;
    int v;
    double t, *tBuf, *vBuf;

    printf("Starting: "); fflush(stdout);

    ncV = malloc(nVars*sizeof(long));
    snprintf(f, 100, "big_var_%%0%dd", numDigits(nVars));

    handleError(nc_create(fileName, NC_CLOBBER | NC_64BIT_OFFSET, &ncF));
    ncSetAttributeText(ncF, NC_GLOBAL, "doc", "big test file for ncdataReader2");

    nc_redef(ncF);
    handleError(nc_def_dim(ncF, "time", nValues, &ncD));
    handleError(nc_def_var(ncF, "time", NC_DOUBLE, 1, &ncD, &ncT));
    ncSetAttributeText(ncF, ncT, NCATT_LOAD_TYPE, NCATT_LT_FULL);
    ncSetAttributeLong(ncF, ncT, NCATT_LOOKUP_CACHE, 16);
    for (v = 0; v < nVars; v++) {
        snprintf(c, 100, f, v);
        handleError(nc_def_var(ncF, c, NC_DOUBLE, 1, &ncD, ncV+v));
        ncSetAttributeText(ncF, ncV[v], NCATT_LOAD_TYPE, NCATT_LT_CHUNK);
        ncSetAttributeText(ncF, ncV[v], NCATT_INTERPOLATION, NCATT_IP_AKIMA);
        ncSetAttributeLong(ncF, ncV[v], NCATT_CHUNK_SIZE, 1024);
        ncSetAttributeLong(ncF, ncV[v], NCATT_PARAMETER_CACHE, 16);
    }

    handleError(nc_enddef(ncF));
    printf("N"); fflush(stdout);

    tBuf = malloc(nValues*sizeof(double));
    vBuf = malloc(nValues*sizeof(double));

    if ((tBuf == NULL) || (vBuf == NULL)) {
        printf("Out of memory!\n");
        return;
    }

    for (i = 0; i < nValues; i ++)
        tBuf[i] = 0.01 * (i - 0.5 + 0.4 * RAND01());
    printf("T"); fflush(stdout);
    handleError(nc_put_var_double(ncF, ncT, tBuf));
    printf("t"); fflush(stdout);
    for (j = 0; j < nVars; j++) {
        for (i = 0; i < nValues; i ++) {
            t = tBuf[i];
            vBuf[i] = 100 * sin(t/100) + 10 * RAND01()*sin(t/10) + RAND01()*sin(t);
        }
        printf("V"); fflush(stdout);
        handleError(nc_put_var_double(ncF, ncV[j], vBuf));
        printf("v"); fflush(stdout);
    }
    free(ncV);
    free(tBuf);
    free(vBuf);
    handleError(nc_sync(ncF));
    handleError(nc_close(ncF));
    printf("\n");
}

int main(int argc, char *argv[]) {
    long nVars=10, nValues=10000000;
    char *outFile=NULL;
    if (argc > 3) nValues = atol(argv[3]);
    if (argc > 2) nVars   = atol(argv[2]);
    if (argc > 1)
        outFile = argv[1];
    else 
        outFile = "testfilebig.nc";
    printf("Usage: %s [outfile [number_of_variables [number_of_values]]]\n\n", argv[0]);
    printf("This generates a file with faked time series\n");
    printf("Total number of values: %ld = (%ld variables + time) * %ld values\n\n", nValues*(nVars+1), nVars, nValues);
    printf("Make sure you have enough disk space and some time.\n");
    printf("Enter 'y' to proceed, any other to stop now: ");
    if (getchar() == 'y')
        createBigFile(outFile, nVars, nValues);
    return 0;
}

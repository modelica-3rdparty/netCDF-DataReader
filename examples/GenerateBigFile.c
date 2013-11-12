#include <math.h>
#include <stdio.h>
#include <string.h>
#include "ncDataReader2.h"
#include "netcdf.h"
#include "../config.h"

#define DIM 10000000
#define VARS 10

#define RAND01() (rand()/(double)RAND_MAX)

void handle_error(int status) {
if (status != NC_NOERR) {
   fprintf(stderr, "%s\n", nc_strerror(status));
   exit(-1);
   }
}


void createBigFile(char *fileName) {
    int ncF, ncD, ncV[VARS], ncT;
    char c[101];
    size_t i, j;
    double t, *tBuf, *vBuf;
    
    printf("Starting: "); fflush(stdout);

    handle_error(nc_create(fileName, NC_CLOBBER, &ncF));
    ncSetAttributeText(ncF, NC_GLOBAL, "doc", "big test file for ncdataReader2");

    nc_redef(ncF);
    handle_error(nc_def_dim(ncF, "time", DIM, &ncD));
    handle_error(nc_def_var(ncF, "time", NC_DOUBLE, 1, &ncD, &ncT));
    ncSetAttributeText(ncF, ncT, NCATT_LOAD_TYPE, NCATT_LT_FULL);
    ncSetAttributeLong(ncF, ncT, NCATT_LOOKUP_CACHE, 16);
    for (j = 0; j < VARS; j++) {
        snprintf(c, 100, "big_var_%02ld", j);
        handle_error(nc_def_var(ncF, c, NC_DOUBLE, 1, &ncD, ncV+j));
        ncSetAttributeText(ncF, ncV[j], NCATT_LOAD_TYPE, NCATT_LT_CHUNK);
        ncSetAttributeText(ncF, ncV[j], NCATT_INTERPOLATION, NCATT_IP_AKIMA);
        ncSetAttributeLong(ncF, ncV[j], NCATT_CHUNK_SIZE, 1024);
        ncSetAttributeLong(ncF, ncV[j], NCATT_PARAMETER_CACHE, 16);
    }
    
    handle_error(nc_enddef(ncF));
    printf("N"); fflush(stdout);

    tBuf = malloc(DIM*sizeof(double));
    vBuf = malloc(DIM*sizeof(double));
    
    if ((tBuf == NULL) || (vBuf == NULL)) {
        printf("Out of memory!\n");
        return;
    }
    
    for (i = 0; i < DIM; i ++)
        tBuf[i] = 0.01 * (i - 0.5 + 0.4 * RAND01());
    printf("T"); fflush(stdout);
    handle_error(nc_put_var_double(ncF, ncT, tBuf));
    printf("t"); fflush(stdout);
    for (j = 0; j < VARS; j++) {
        for (i = 0; i < DIM; i ++) {
            t = tBuf[i];
            vBuf[i] = 100 * sin(t/100) + 10 * RAND01()*sin(t/10) + RAND01()*sin(t);
        }
        printf("V"); fflush(stdout);
        handle_error(nc_put_var_double(ncF, ncV[j], vBuf));
        printf("v"); fflush(stdout);
    }
    
    handle_error(nc_sync(ncF));
    handle_error(nc_close(ncF));
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("This generates a big file with %d data values.\n", DIM*(VARS+1));
    printf("Make sure you have enough disk space and some time.\n");
    printf("Enter 'y' to proceed, any other to stop now: ");
    if (getchar() == 'y')
        if (argc > 1) createBigFile(argv[1]);
        else          createBigFile("testfilebig.nc");
    return 0;
}

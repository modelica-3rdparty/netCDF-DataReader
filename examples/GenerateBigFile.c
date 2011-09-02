#include <math.h>
#include <stdio.h>
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
    double d;
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
    for (i = 0; i < DIM; i ++) {
        /* give some visual feedback on progress */
        if (i % (DIM/100) == 0) {
            printf(".");
            fflush(stdout);
        }
        d = 0.01 * (i - 0.5 + 0.4 * RAND01());
        handle_error(nc_put_var1_double(ncF, ncT, &i, &d));
        for (j = 0; j < VARS; j++) {
            d = 100 * sin(d/100) + 10 * RAND01()*sin(d/10) + RAND01()*sin(d);
            handle_error(nc_put_var1_double(ncF, ncV[j], &i, &d));
        }
    }
    handle_error(nc_sync(ncF));
    handle_error(nc_close(ncF));
    printf("\n");
}



int main(void) {
    printf("This generates a very big file with %d data points.\n", DIM);
    printf("Make sure you have enough disk space and some time.\n");
    printf("Enter 'y' to proceed, any other to stop now: ");
    if (getchar() == 'y') {
        printf("Starting: ");
        createBigFile("testfilebig.nc");
    }
    return 0;
}

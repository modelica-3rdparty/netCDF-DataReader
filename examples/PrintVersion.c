#include <stdio.h>
#include <netcdf.h>
#include "ncDataReader2.h"

#ifndef NC_NOCHECK
void myHandler(int e, char *s) {
    printf("\n");
    printf("      ---------- Message from custom error handler -----------\n");
    printf("       Error %d: %s\n", e, s);
    printf("      --------------------------------------------------------\n");
    printf("\n");
}
#endif /* NC_NOCHECK */

int main(void) {
    printf("NcDataReader2  version: %s\n", ncDataReader2Version());
    printf("netCDF library version: %s\n", nc_inq_libvers());

#ifndef NC_NOCHECK
    ncSetErrorHandler(myHandler);
    printf("\nTesting customized error handler, an error message should appear below:\n");
    // should raise an error
    ncDataSet1DNew("foobarbaz.nc", "hugo_egon", EpDefault, LtFull, 0);
#endif /* NC_NOCHECK */

    return 0;
}

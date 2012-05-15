#include <stdlib.h>
#include <stdio.h>
#include <ncDataReaderEA.h>

#define NCFILE  "testfile.nc"

int main(void) {
    double x, start, step, end;
    FILE *outf = fopen("data_1D_EA.dat", "w");
    start = 0.0;
    end   = 100.0;
    step  = 0.00001 *(end - start);
    fprintf(outf, "# you may plot this file with gnuplot\n");
    for (x = start; x < end; x+= step)
        fprintf(outf, "%g\t%g\n", x, ncEasyGet1D(NCFILE, "test1D", x));
    fclose(outf);
    ncEasyDumpStatistics("Read1DEasyAPI.log");
    ncEasyFree();
    printf("attributes:\n");
    printf("                 doc: %s\n", ncEasyGetAttributeString(NCFILE, "", "doc"));
    printf("             version: %s\n", ncEasyGetAttributeString(NCFILE, "", "version"));
    printf("                 foo: %g\n", ncEasyGetAttributeDouble(NCFILE, "", "foo"));
    printf("                 bar: %ld\n", ncEasyGetAttributeLong(NCFILE, "", "bar"));
    printf("  time|extrapolation: %s\n", ncEasyGetAttributeString(NCFILE, "time", "extrapolation"));
    printf("   time|scale_factor: %g\n", ncEasyGetAttributeDouble(NCFILE, "time", "scale_factor"));
    printf("     time|add_offset: %g\n", ncEasyGetAttributeDouble(NCFILE, "time", "add_offset"));
    return 0;
}

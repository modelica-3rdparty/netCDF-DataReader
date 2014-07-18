#include <stdlib.h>
#include <stdio.h>
#include <ncDataReaderEA.h>

int main(int argc, char *argv[]) {
    double x, start, step, end;
    char *ncFileName;
    FILE *outf = fopen("data_1D_EA.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return 10;
    }
    if (argc > 1) ncFileName = argv[1];
    else          ncFileName = "testfile.nc";

    start = 0.0;
    end   = 100.0;
    step  = 0.00001 *(end - start);
    fprintf(outf, "# you may plot this file with gnuplot\n");
    for (x = start; x < end; x+= step)
        fprintf(outf, "%g\t%g\n", x, ncEasyGet1D(ncFileName, "test1D", x));
    fclose(outf);
    ncEasyDumpStatistics("Read1DEasyAPI.log");
    ncEasyFree();
    printf("attributes:\n");
    printf("                 doc: %s\n", ncEasyGetAttributeString(ncFileName, "", "doc"));
    printf("             version: %s\n", ncEasyGetAttributeString(ncFileName, "", "version"));
    printf("                 foo: %g\n", ncEasyGetAttributeDouble(ncFileName, "", "foo"));
    printf("                 bar: %ld\n", ncEasyGetAttributeLong(ncFileName, "", "bar"));
    printf("  time|extrapolation: %s\n", ncEasyGetAttributeString(ncFileName, "time", "extrapolation"));
    printf("   time|scale_factor: %g\n", ncEasyGetAttributeDouble(ncFileName, "time", "scale_factor"));
    printf("     time|add_offset: %g\n", ncEasyGetAttributeDouble(ncFileName, "time", "add_offset"));
    return 0;
}

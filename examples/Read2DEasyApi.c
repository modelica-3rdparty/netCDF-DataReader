#include <stdlib.h>
#include <stdio.h>
#include <ncDataReaderEA.h>

int main(int argc, char *argv[]) {
    double x, y;
    char *ncFileName;
    FILE *outf = fopen("data_2D_EA.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return 10;
    }
    if (argc > 1) ncFileName = argv[1];
    else          ncFileName = "testfile.nc";

    fprintf(outf, "# you may plot this file with gnuplot using splot\n");
    for (x = -4.9; x <  4.9; x += 0.2) {
        for (y = -4.9; y < 4.9; y+= 0.2)
            fprintf(outf, "%g\t%g\t%g\n", x, y, 
                    ncEasyGetScattered2D(ncFileName, "points", x, y));
        fprintf(outf, "\n");
    }
    fclose(outf);
    ncEasyFree();
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <ncDataReaderEA.h>

int main(void) {
    double x, y;
    FILE *outf = fopen("data_2D_EA.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return 10;
    }
    fprintf(outf, "# you may plot this file with gnuplot using splot\n");
    
    for (x = -4.9; x <  4.9; x += 0.2) {
        for (y = -4.9; y < 4.9; y+= 0.2)
            fprintf(outf, "%g\t%g\t%g\n", x, y, 
                    ncEasyGetScattered2D("testfile.nc", "points", x, y));
        fprintf(outf, "\n");
    }
    fclose(outf);
    ncEasyFree();
    return 0;
}

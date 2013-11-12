#include <stdlib.h>
#include <stdio.h>
#include <ncDataReader2.h>

int main(int argc, char *argv[]) {
    NcScattered2D *data;
    double x, y, xstep, ystep;
    FILE *outf;

    if (argc > 1) data = ncScattered2DNew(argv[1], "points");
    else          data = ncScattered2DNew("testfile.nc", "points");
    
    ncScattered2DSetOption(data, OpScattered2DPointsPerCell, 20);
    ncScattered2DInit(data);
    outf = fopen("data_2D.dat", "w");
    if (!outf) {
        printf("error: could not open output file for writing\n");
        return 10;
    }
    
    fprintf(outf, "# you may plot this file with gnuplot using splot\n");
    fprintf(outf, "# x-range %g - %g\n", data->xRange[0], data->xRange[1]);
    fprintf(outf, "# y-range %g - %g\n", data->yRange[0], data->yRange[1]);
    fprintf(outf, "# z-range %g - %g\n", data->zRange[0], data->zRange[1]);
    
    xstep = 0.02 * (data->xRange[1] - data->xRange[0]);
    ystep = 0.02 * (data->yRange[1] - data->yRange[0]);
    
    for (x = data->xRange[0]; x <  data->xRange[1]; x += xstep) {
        for (y = data->yRange[0]; y < data->yRange[1]; y+= ystep)
            fprintf(outf, "%g\t%g\t%g\n", x, y, ncScattered2DGet(data, x, y));
        fprintf(outf, "\n");
    }
    fclose(outf);
    ncScattered2DFree(data);
    return 0;
}

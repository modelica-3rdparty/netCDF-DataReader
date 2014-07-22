#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include "getopt.h"
#include "config.h"
#include "ncDataReader2.h"


#define OPTSTR "dghv:a:s:e:n:i:x:m:w:l:c:k:p:t:o:"
#ifdef _WIN32
#define TEMPLATE "%e;%e;\r\n"
#define TEMPLATE_GP "%e\t%e\r\n"
#else /* _WIN32 */
#define TEMPLATE "%e;%e;\n"
#define TEMPLATE_GP "%e\t%e\n"
#endif /* _WIN32 */


#define HELPTEXT ""\
"Dump interpolated values from a netCDF file or DAP-Server using ncDataReader2\n"\
"Version: "NCDR_VERSION"\n"\
"Usage: ncdr2Dump1D [parameter] filename\n"\
"       filename    name of the netCDF file or DAP-URI\n"\
"   mandatory parameters:\n"\
"       -v string   name of the variable\n"\
"       -a string   name of the abscissa / data set\n"\
"   optional parameters:\n"\
"       -o string   name of output file (default: stdout)\n"\
"       -s float    start of data range\n"\
"       -e float    end of data range\n"\
"       -n int      number of points\n"\
"       -i char     interpolation:\n"\
"                   [a]kima, [l]inear, [d]iscrete, [s]insteps, [c]oswin\n"\
"       -x char     extrapolation:\n"\
"                   [d]efault, [p]eriodic, [c]onstant\n"\
"       -l char     load type:\n"\
"                   [f]ull, [n]one, [c]hunks (see -h)\n"\
"       -w float    window size for coswin interpolation\n"\
"       -m float    smoothing radius for sinsteps interpolation (default is 0)\n"\
"       -k int      size of lookup cache\n"\
"       -p int      size of parameter cache\n"\
"       -c int      size of chunks for chunk loading\n"\
"       -t string   template string for output (used with printf())\n"\
"       -g          use gnuplot-compatible output (default is CSV)\n"\
"       -d          dump timing information and access statistics to stderr\n"\
"       -h          print this help and exit\n"


#define DDEF DBL_MAX

int export1D(int argc, char **argv) {
    int d=0, tmp;
    double s=DDEF, e=DDEF, m=0, w=DDEF, xi, step;
    size_t n=100, k=0, p=0, c=0;
    char *f=NULL, *v=NULL, *a=NULL, *t=TEMPLATE, *o=NULL;
    Extrapolation x = EpDefault;
    Interpolation i = IpAuto;
    LoadType l = LtAuto;
    NcDataSet1D *dataSet;
    NcVar1D *var;
    FILE *ofile;
    clock_t cstart;
    
    while((tmp = getopt(argc, argv, OPTSTR)) != -1) 
        switch(tmp) {
            case 'd': d = 1; break;
            case 'g': t = TEMPLATE_GP; break;
            case 's': s = atof(optarg); break;
            case 'e': e = atof(optarg); break;
            case 'm': m = atof(optarg); break;
            case 'w': w = atof(optarg); break;
            case 'n': n = atol(optarg); break;
            case 'k': k = atol(optarg); break;
            case 'p': p = atol(optarg); break;
            case 'c': c = atol(optarg); break;
            case 'v': v = optarg; break;
            case 'a': a = optarg; break;
            case 't': t = optarg; break;
            case 'o': o = optarg; break;
            case 'x':
                switch (optarg[0]) {
                    case 'c': x = EpConstant; break;
                    case 'd': x = EpDefault; break;
                    case 'p': x = EpPeriodic; break;
                }
                break;
            case 'i':
                switch (optarg[0]) {
                    case 'a': i = IpAkima; break;
                    case 'l': i = IpLinear; break;
                    case 'd': i = IpDiscrete; break;
                    case 's': i = IpSinSteps; break;
                    case 'c': i = IpCosWin; break;
                }
                break;
            case 'l':
                switch (optarg[0]) {
                    case 'f': l = LtFull; break;
                    case 'c': l = LtChunk; break;
                    case 'n': l = LtNone; break;
                }
                break;
            case 'h':
                fprintf(stderr, HELPTEXT);
                return 0;                   
            default:
                fprintf(stderr, HELPTEXT);
                return 10;
        }
    f = argv[optind];
    if (f==NULL) {
        fprintf(stderr, "error: need a valid name of the netcdf file\n");
        return 1;
    }
    if (v==NULL) {
        fprintf(stderr, "error: need a valid variable name (-v)\n");
        return 2;
    }
    if (a==NULL) {
        fprintf(stderr, "error: need a valid name for the abscissa / data set (-a)\n");
        return 3;
    }
    
    if (d) cstart = clock();
    
    dataSet = ncDataSet1DNew(f, a, x, LtFull, k);
    if (s == DDEF) s = dataSet->min;
    if (e == DDEF) e = dataSet->max;
    
    var = ncVar1DNew(dataSet, v, i, l);
    if (w != 0)     ncVar1DSetOption(var, OpVarSmoothing, m);
    if (w != DDEF)  ncVar1DSetOption(var, OpVarWindowSize, w);
    if (p != 0)     ncVar1DSetOption(var, OpVarParameterCacheSize, p);
    if (c != 0)     ncVar1DSetOption(var, OpVarChunkSize, c);
    
    step = (e-s)/(n-1);
    if (o == NULL)
        ofile = stdout;
    else
        ofile = fopen(o, "w");
    for (xi = s; xi <= e; xi += step)
        fprintf(ofile, t, xi, ncVar1DGet(var, xi));
    if (o != NULL)
        fclose(ofile);
    
    if (d) {
        fprintf(stderr, "Needed clock cycles:  %ld\n\n", clock()-cstart);
        ncDataSet1DDumpStatistics(dataSet, stderr);
        ncVar1DDumpStatistics(var, stderr);
    }
    
    ncVar1DFree(var);
    ncDataSet1DFree(dataSet);

    return 0;
}

int main(int argc, char **argv) {
    return export1D(argc, argv);
}

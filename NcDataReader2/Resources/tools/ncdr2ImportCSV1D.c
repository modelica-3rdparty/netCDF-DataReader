#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include "getopt.h"
#include "csv.h"
#include "config.h"
#include "ncDataReader2.h"
#include "netcdf.h"


#define OPTSTR "ho:i:x:m:w:l:v:c:k:p:t:"


#define HELPTEXT ""\
"Generate a ncDataReader2-compatible netCDF file from CSV input\n"\
"Version: "NCDR_VERSION"\n"\
"Usage: ncdr2ImportCSV1D [parameter] filename\n"\
"       filename    name of the CSV input file\n"\
"   optional parameters:\n"\
"       -o string   name of output file\n"\
"       -t string   comment to include in file\n"\
"       -h          print this help and exit\n"\
"   the following is stored as attributes of the variables:\n"\
"       -i char     interpolation:\n"\
"                   [a]kima, [l]inear, [d]iscrete, [s]insteps, [c]oswin\n"\
"       -x char     extrapolation:\n"\
"                   [d]efault, [p]eriodic, [c]onstant\n"\
"       -l char     load type:\n"\
"                   [f]ull, [n]one, [c]hunks (see -h)\n"\
"       -w float    window size for coswin interpolation\n"\
"       -m float    smoothing radius for sinsteps interpolation\n"\
"       -k int      size of lookup cache\n"\
"       -p int      size of parameter cache\n"\
"       -v int      size of value cache\n"\
"       -c int      size of chunks for chunk loading\n"\
"   the input file is a simple CSV file with the following structure:\n"\
"       - fields delimited by comma ','\n"\
"       - every row has the same number of fields\n"\
"       - optional header row with the variable names\n"\
"       - all other fields contain just numbers\n"\
"       - the decimal separator for numbers is a point '.'\n"\
"       - every column contains one variable\n"\
"       - first column is used as the abscissa for all variables\n"

#define DDEF DBL_MAX


void handle_error(int status) {
if (status != NC_NOERR) {
   fprintf(stderr, "%s\n", nc_strerror(status));
   exit(-1);
   }
}


int importCSV1D(int argc, char **argv) {
    char *f=NULL, *o=NULL, *t=NULL, ctmp[301], *xStr, *iStr, *lStr;
    const char *name;
    unsigned int k=0, p=0, v=0, c=0;
    size_t j;
    int width, height, tmp, type, ncFile, ncDim, ncVar;
    double w=DDEF, m=DDEF, dtmp;
    Extrapolation x = EpAuto;
    Interpolation i = IpAuto;
    LoadType l = LtAuto;
    CSV *csv;
    
    while((tmp = getopt(argc, argv, OPTSTR)) != -1) 
        switch(tmp) {
            case 'm': m = atof(optarg); break;
            case 'w': w = atof(optarg); break;
            case 'k': k = (unsigned int)atol(optarg); break;
            case 'p': p = (unsigned int)atol(optarg); break;
            case 'v': v = (unsigned int)atol(optarg); break;
            case 'c': c = (unsigned int)atol(optarg); break;
            case 't': t = optarg; break;
            case 'o': o = optarg; break;
            case 'x':
                switch (optarg[0]) {
                    case 'c': 
                        x = EpConstant;
                        xStr = NCATT_EP_CONSTANT;
                        break;
                    case 'd': 
                        x = EpDefault; 
                        xStr = NCATT_EP_DEFAULT;
                        break;
                    case 'p': 
                        x = EpPeriodic;
                        xStr = NCATT_EP_PERIODIC;
                        break;
                }
                break;
            case 'i':
                switch (optarg[0]) {
                    case 'a': 
                        i = IpAkima;
                        iStr = NCATT_IP_AKIMA;
                        break;
                    case 'l': 
                        i = IpLinear; 
                        iStr = NCATT_IP_LINEAR;
                        break;
                    case 'd': 
                        i = IpDiscrete; 
                        iStr = NCATT_IP_DISCRETE;
                        break;
                    case 's': 
                        i = IpSinSteps;
                        iStr = NCATT_IP_SINSTEPS;
                        break;
                    case 'c': 
                        i = IpCosWin; 
                        iStr = NCATT_IP_COSWIN;
                        break;
                }
                break;
            case 'l':
                switch (optarg[0]) {
                    case 'f': 
                        l = LtFull;
                        lStr = NCATT_LT_FULL;
                        break;
                    case 'c': 
                        l = LtChunk;
                        lStr = NCATT_LT_CHUNK;
                        break;
                    case 'n': 
                        l = LtNone;
                        lStr = NCATT_LT_NONE;
                        break;
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
        fprintf(stderr, "error: need a name of the CSV file\n");
        return 1;
    }
    
    csv = loadcsv(f);
    if (csv==NULL) {
        fprintf(stderr, "error: parsing of CSV file failed\n");
        return 2;
    }
    csv_getsize(csv, &width, &height);

    if (o==NULL) {
        tmp = strlen(f)+4;
        o = malloc(tmp+1);
        snprintf(o, tmp, "%s.nc", f);
    }
    handle_error(nc_create(o, NC_CLOBBER, &ncFile));
    handle_error(nc_put_att_text(ncFile, NC_GLOBAL, "original_file", strlen(f), f));
    snprintf(ctmp, 300, "generated with ncdr2ImportCSV1D (version %s)" , ncDataReader2Version());
    handle_error(nc_put_att_text(ncFile, NC_GLOBAL, "doc", strlen(ctmp), ctmp));
    if (t != NULL)
        handle_error(nc_put_att_text(ncFile, NC_GLOBAL, "comment", strlen(t), t));
    
    name = csv_column(csv, 0, &type);
    if (! name) name = "var_00";
    if (type != CSV_REAL) {
        fprintf(stderr, "error: wrong data type in column 0\n");
        return 3;
    }
    handle_error(nc_def_dim(ncFile, name, height, &ncDim));
    handle_error(nc_def_var(ncFile, name, NC_DOUBLE, 1, &ncDim, &ncVar));
    if (x != EpAuto)
        handle_error(nc_put_att_text(ncFile, ncVar, NCATT_EXTRAPOLATION, strlen(xStr), xStr));
    if (k != 0)
        handle_error(nc_put_att_uint(ncFile, ncVar, NCATT_LOOKUP_CACHE, NC_LONG, 1, &k));
    handle_error(nc_enddef(ncFile));
    for (j = 0; j < height; j++) {
        dtmp = csv_get(csv, 0, j);
        handle_error(nc_put_var1_double(ncFile, ncVar, &j, &dtmp));
    }
    
    for(tmp=1; tmp < width; tmp++) {
        name = csv_column(csv, tmp, &type);
        if (! name) {
            snprintf(ctmp, 300, "var_%02d", tmp);
            name = ctmp;
        }
        if (type != CSV_REAL) {
            fprintf(stderr, "warning: wrong data type in column %d, skipping!\n", tmp);
        } else {
            handle_error(nc_redef(ncFile));
            handle_error(nc_def_var(ncFile, name, NC_DOUBLE, 1, &ncDim, &ncVar));
            if (i != IpAuto)
                handle_error(nc_put_att_text(ncFile, ncVar, NCATT_INTERPOLATION, strlen(iStr), iStr));
            if (l != LtAuto)
                handle_error(nc_put_att_text(ncFile, ncVar, NCATT_LOAD_TYPE, strlen(lStr), lStr));
            if (m != DDEF)
                handle_error(nc_put_att_double(ncFile, ncVar, NCATT_SMOOTHING, NC_DOUBLE, 1, &m));
            if (w != DDEF)
                handle_error(nc_put_att_double(ncFile, ncVar, NCATT_WINDOW_SIZE, NC_DOUBLE, 1, &w));
            if (p != 0)
                handle_error(nc_put_att_uint(ncFile, ncVar, NCATT_PARAMETER_CACHE, NC_LONG, 1, &p));
            if (v != 0)
                handle_error(nc_put_att_uint(ncFile, ncVar, NCATT_VALUE_CACHE, NC_LONG, 1, &v));
            if (c != 0)
                handle_error(nc_put_att_uint(ncFile, ncVar, NCATT_CHUNK_SIZE, NC_LONG, 1, &c));
            handle_error(nc_enddef(ncFile));
            for (j = 0; j < height; j++) {
                dtmp = csv_get(csv, tmp, j);
                handle_error(nc_put_var1_double(ncFile, ncVar, &j, &dtmp));
            }
        }
    }

    killcsv(csv);
    handle_error(nc_sync(ncFile));
    handle_error(nc_close(ncFile));
    return 0;
}

int main(int argc, char **argv) {
    return importCSV1D(argc, argv);
}

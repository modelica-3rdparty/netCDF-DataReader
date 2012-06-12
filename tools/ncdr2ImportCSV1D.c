#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include "getopt.h"
#include "csv.h"
#include "../config.h"
#include "ncDataReader2.h"


#define OPTSTR "ho:i:x:m:w:l:c:k:p:t:"


#define HELPTEXT ""\
"Generate a ncDatareader2-compatible netCDF file from CSV\n"\
"Version: "NCDR_VERSION"\n"\
"Usage: ncdr2ImportCSV1D [parameter] filename\n"\
"       filename    name of the CSV file\n"\
"   optional parameters:\n"\
"       -o string   name of output file\n"\
"       -i char     interpolation for all variable:\n"\
"                   [a]kima, [l]inear, [d]iscrete, [s]insteps, [c]oswin\n"\
"       -x char     extrapolation for all variables:\n"\
"                   [d]efault, [p]eriodic, [c]onstant\n"\
"       -l char     load type for all variables:\n"\
"                   [f]ull, [n]one, [c]hunks (see -h)\n"\
"       -w float    window size for coswin interpolation\n"\
"       -m float    smoothing radius for sinsteps interpolation (default is 0)\n"\
"       -k int      size of lookup cache\n"\
"       -p int      size of parameter cache\n"\
"       -c int      size of chunks for chunk loading\n"\
"       -t string   comment to inlcude in file\n"\
"       -h          print this help and exit\n"


int importCSV1D(int argc, char **argv) {
    return 1;
}

int main(int argc, char **argv) {
    return importCSV1D(argc, argv);
}

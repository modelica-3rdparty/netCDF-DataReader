/*  ncDataReader2 - interpolating reading of 1D data sets
    Copyright (C) 2004  Joerg Raedler <joerg@dezentral.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "netcdf.h"
#include "ncDataReader2.h"
#include "ncDataReaderEA.h"
#include "config.h"
#include "StringHashTable.h"

typedef struct {
    SHT_Table *dataSets;
    SHT_Table *vars;
} EasyFileData1D;

static SHT_Table *easyFiles1D = NULL;


/* ************ 1D ************ */

static NcVar1D *createEasyVar1D(EasyFileData1D *fileData, const char *fileName, 
                                const char *varName) {
    NcDataSet1D *dataSet = NULL;
    NcVar1D     *var = NULL;
    int ncF, ncV, ncD;
    char dimName[NC_MAX_NAME+1];
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) return var;
    if (ncError(nc_inq_varid(ncF, varName, &ncV))) return var;
    if (ncError(nc_inq_varndims(ncF, ncV, &ncD))) return var;
    assert(ncD == 1);
    nc_inq_vardimid(ncF, ncV, &ncD);
    nc_inq_dimname(ncF, ncD, dimName);
    if (! (dataSet = shtGet(fileData->dataSets, dimName))) {
        dataSet = ncDataSet1DNew(fileName, dimName, EpAuto, LtAuto, 8);
        shtPut(fileData->dataSets, dimName, dataSet);
    }
    var = ncVar1DNew(dataSet, varName, IpAuto, LtAuto);
    shtPut(fileData->vars, varName, var);
    return var;
}


double DLL_EXPORT ncEasyGet1D(const char *fileName, const char *varName, double x) {
    EasyFileData1D *fileData;
    NcVar1D        *var;
    if (easyFiles1D == NULL) 
        easyFiles1D = shtNew(100);
    if (!(fileData = shtGet(easyFiles1D, fileName))) {
        fileData = (EasyFileData1D *)malloc(sizeof(EasyFileData1D));
        if (! fileData) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
        fileData->dataSets = shtNew(100);
        fileData->vars     = shtNew(1000);
        shtPut(easyFiles1D, fileName, fileData);
    }
    if (!(var = shtGet(fileData->vars, varName))) {
        var = createEasyVar1D(fileData, fileName, varName);
    }
    return ncVar1DGet(var, x);
}


static void _freeDataSet(char *name, void *d, void *arg) { ncDataSet1DFree(d); }

static void _freeVar(char *name, void *v, void *arg) { ncVar1DFree(v); }

static void _freeFileData(char *name, void *f, void *arg) {
    EasyFileData1D *fileData;
    fileData = f;
    shtIterate(fileData->vars, (SHT_iterfunc)_freeVar, NULL);
    shtFree(fileData->vars);
    shtIterate(fileData->dataSets, (SHT_iterfunc)_freeDataSet, NULL);
    shtFree(fileData->dataSets);
    free(fileData);
}


/* ************ Scattered 2D ************ */

static SHT_Table *easyScattered2D = NULL;


static void _freeScattered2D(char *name, void *v, void *arg) {
    ncScattered2DFree(v); 
}


double DLL_EXPORT ncEasyGetScattered2D(const char *fileName, const char *varName, 
                                       double x, double y) {
    char key[1025];
    NcScattered2D *scattered2D;
    if (easyScattered2D == NULL) 
        easyScattered2D = shtNew(100);
    snprintf(key, 1024, "%s.%s", fileName, varName);
    if (!(scattered2D = shtGet(easyScattered2D, key))) {
        scattered2D = ncScattered2DNew(fileName, varName);
        ncScattered2DInit(scattered2D);
        shtPut(easyScattered2D, key, scattered2D);
    }
    return ncScattered2DGet(scattered2D, x, y);
}



/* ************ General ************ */

void DLL_EXPORT ncEasyFree() {
    shtIterate(easyFiles1D, (SHT_iterfunc)_freeFileData, NULL);
    shtFree(easyFiles1D);
    shtIterate(easyScattered2D, (SHT_iterfunc)_freeScattered2D, NULL);
    shtFree(easyScattered2D);
    easyFiles1D = NULL;
}


double DLL_EXPORT ncEasyGetAttributeDouble(const char *fileName, const char *varName, 
                                           const char *attName) {
    int ncF, ncV;
    double d;
    size_t n;
    nc_type t;
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) return NC_DOUBLE_NOVAL;
    if (strlen(varName) > 0) {
        if (ncError(nc_inq_varid(ncF, varName, &ncV))) return NC_DOUBLE_NOVAL;
    } else 
        ncV = NC_GLOBAL;
    if (ncError(nc_inq_att(ncF, ncV, attName, &t, &n))) return NC_DOUBLE_NOVAL;
    if (n != 1) return NC_DOUBLE_NOVAL;
    if (ncError(nc_get_att_double(ncF, ncV, attName, &d))) return NC_DOUBLE_NOVAL;
    ncError(nc_close(ncF));
    return d;
}


long DLL_EXPORT ncEasyGetAttributeLong(const char *fileName, const char *varName, 
                                       const char *attName) {
    int ncF, ncV;
    long l;
    size_t n;
    nc_type t;
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) return NC_LONG_NOVAL;
    if (strlen(varName) > 0) {
        if (ncError(nc_inq_varid(ncF, varName, &ncV))) return NC_LONG_NOVAL;
    } else 
        ncV = NC_GLOBAL;
    if (ncError(nc_inq_att(ncF, ncV, attName, &t, &n))) return NC_LONG_NOVAL;
    if (n != 1) return NC_LONG_NOVAL;
    if (ncError(nc_get_att_long(ncF, ncV, attName, &l))) return NC_LONG_NOVAL;
    ncError(nc_close(ncF));
    return l;
}


static INLINE char *cloneStr(char *c) {
    char *n = malloc(strlen(c)+1);
    if (! n) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    strcpy(n, c);
    return n;
}


char DLL_EXPORT *ncEasyGetAttributeString(const char *fileName, const char *varName, 
                                          const char *attName) {
    int ncF, ncV;
    char *c;
    size_t n;
    nc_type t;
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) 
        return cloneStr(NC_STRING_NOVAL);
    if (strlen(varName) > 0) {
        if (ncError(nc_inq_varid(ncF, varName, &ncV))) 
            return cloneStr(NC_STRING_NOVAL);
    } else 
        ncV = NC_GLOBAL;
    if (ncError(nc_inq_att(ncF, ncV, attName, &t, &n))) 
        return cloneStr(NC_STRING_NOVAL);
    c = (char *)malloc(n+1);
    if (! c) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    if (ncError(nc_get_att_text(ncF, ncV, attName, c))) {
        free(c);
        return cloneStr(NC_STRING_NOVAL);
    }
    ncError(nc_close(ncF));
    c[n] = '\0';
    return c;
}

static void dumpDataSet1DStatistics(char *key, SHT_val val, const void *tmp) {
    ncDataSet1DDumpStatistics((NcDataSet1D *)val, (FILE *)tmp);
}

static void dumpVar1DStatistics(char *key, SHT_val val, const void *tmp) {
    ncVar1DDumpStatistics((NcVar1D *)val, (FILE *)tmp);
}

static void dumpFile1DStatistics(char *key, SHT_val val, const void *tmp) {
    EasyFileData1D *data = (EasyFileData1D *)val;
    fprintf((FILE *)tmp, ">>> File: %s\n", key);
    shtIterate(data->dataSets, dumpDataSet1DStatistics, (void *)tmp);
    shtIterate(data->vars, dumpVar1DStatistics, (void *)tmp);
}

int DLL_EXPORT ncEasyDumpStatistics(const char *fileName) {
    FILE *f = fopen(fileName, "w");
    if (!f) {
        ncdrError(NCDR_EFILEW, NCDR_EFILEW_TXT);
        return 1;
    }
    fprintf(f, ">>> Access statistics for ncDataReader2/EA <<<\n");
    shtIterate(easyFiles1D, dumpFile1DStatistics, (void *)f);
    fflush(f);
    fclose(f);
    return 0;
}


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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "netcdf.h"
#include "ncDataReader2.h"
#include "config.h"
#if defined(MODELICA_ERROR_HANDLER)
#include "ModelicaUtilities.h"
#endif

/* version string */
char ncDataReader2_version[] = NCDR_VERSION;

/* text of last error */
char DLL_EXPORT ncLastErrorText[101];

char DLL_EXPORT *ncDataReader2Version() {
    return ncDataReader2_version;
}


#ifndef NC_NOCHECK
#include <stdio.h>
#ifdef WIN32_MSGBOX
#include <Windows.h>
#endif /* WIN32_MSGBOX */

/* default error handler
   WIN32_MSGBOX: show message dialog box and exit
   other systems: write msg to stderr and exit */
void _defaultHandler(int n, char *msg) {
#if defined(MODELICA_ERROR_HANDLER)
#ifdef WIN32_MSGBOX
    char TMP[101];
    snprintf(TMP, 100, "ERROR | %d | %s\n", n, msg);
    MessageBox(NULL, TMP, "Fatal exception in ncDataReader2", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
    ModelicaFormatError("ncDataReader2 | ERROR | %d | %s\n", n, msg);
#else
#ifdef WIN32_MSGBOX
    char TMP[101];
    snprintf(TMP, 100, "ERROR | %d | %s\n", n, msg);
    MessageBox(NULL, TMP, "Fatal exception in ncDataReader2", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
#endif
    fprintf(stderr, "ncDataReader2 | ERROR | %d | %s\n", n, msg);
    fflush(stderr);
    exit(n);
#endif
}

static NcErrorHandler error_handler = _defaultHandler;

NcErrorHandler DLL_EXPORT ncSetErrorHandler(NcErrorHandler newHandler) {
    NcErrorHandler tmp = error_handler;
    error_handler = newHandler;
    return tmp;
}

/* catch and handle netCDF's return values */
int DLL_EXPORT ncError(int status) {
    if (status == NC_NOERR) return 0;
    if (error_handler) {
        snprintf(ncLastErrorText, 100, "netcdf | %s", nc_strerror(status));
        (*error_handler)(status, ncLastErrorText);
    }
    return 1; /* got some error -> true */
}
#endif /* not NC_NOCHECK */


/* handle internal errors of this library */
int ncdrError(int status, char *msg) {
    if (status == 0) return 0;
    if (error_handler) {
        snprintf(ncLastErrorText, 100, "ncDataReader2 | %s", msg);
        (*error_handler)(status, ncLastErrorText);
    }
    return 1;
}


double DLL_EXPORT ncGetAttributeDoubleDefault(int ncF, int ncV, const char *name, double def) {
    nc_type t;
    size_t n;
    double d;
    if (nc_inq_att(ncF, ncV, name, &t, &n) != NC_NOERR) return def;
    if (n != 1) return def;
    if (nc_get_att_double(ncF, ncV, name, &d) != NC_NOERR) return def;
    return d;
}


long DLL_EXPORT ncGetAttributeLongDefault(int ncF, int ncV, const char *name, long def) {
    nc_type t;
    size_t n;
    long l;
    if (nc_inq_att(ncF, ncV, name, &t, &n) != NC_NOERR) return def;
    if (n != 1) return def;
    if (nc_get_att_long(ncF, ncV, name, &l) != NC_NOERR) return def;
    return l;
}


static INLINE char *cloneStr(const char *c) {
    char *n = malloc(strlen(c)+1);
    if (! n) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    strcpy(n, c);
    return n;
}


char DLL_EXPORT *ncGetAttributeTextDefault(int ncF, int ncV, const char *name, const char *def) {
    nc_type t;
    size_t n;
    char *c;
    if (nc_inq_att(ncF, ncV, name, &t, &n) != NC_NOERR) return cloneStr(def);
    c = (char *)malloc(n+1);
    if (nc_get_att_text(ncF, ncV, name, c) != NC_NOERR) {
        free(c);
        return cloneStr(def);
    }
    c[n] = '\0';
    return c;
}


void DLL_EXPORT ncSetAttributeDouble(int ncF, int ncV, const char *att, double val) {
    ncError(nc_put_att_double(ncF, ncV, att, NC_DOUBLE, 1, &val));
}


void DLL_EXPORT ncSetAttributeLong(int ncF, int ncV, const char *att, long val) {
    ncError(nc_put_att_long(ncF, ncV, att, NC_LONG, 1, &val));
}


void DLL_EXPORT ncSetAttributeText(int ncF, int ncV, const char *att, const char *val) {
    char c[101];
    strncpy(c, val, 100);
    ncError(nc_put_att_text(ncF, ncV, att, strlen(c), c));
}

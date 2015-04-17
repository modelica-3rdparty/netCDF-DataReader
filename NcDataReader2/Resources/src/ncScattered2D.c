/*  ncDataReader2 - interpolating reading of data sets
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
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* csa.h is missing support for C++, we have to
   protect this here */
#ifdef __cplusplus
extern "C" {
#endif
#include "csa.h"
#ifdef __cplusplus
}
#endif

#include "netcdf.h"
#include "ncDataReader2.h"
#include "config.h"

NcScattered2D DLL_EXPORT *ncScattered2DNew(const char *fileName, const char *varName) {
    int ncF, ncV, ncD[2];
    size_t l, i;
    NcScattered2D *data = NULL;
    double *d, tmp;
    if (ncError(nc_open(fileName, NC_NOWRITE, &ncF))) return data;
    if (ncError(nc_inq_varid(ncF, varName, &ncV))) return data;
    if (ncError(nc_inq_varndims(ncF, ncV, ncD))) return data;
    if (ncD[0] != 2) return data;
    nc_inq_vardimid(ncF, ncV, ncD);
    nc_inq_dimlen(ncF, ncD[1], &l);
    if (l != 3) return NULL;
    nc_inq_dimlen(ncF, ncD[0], &l);
    if (l <= 4) return NULL;
    data = (NcScattered2D *)malloc(sizeof(NcScattered2D));
    if (! data) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    data->nPoints = l;
    data->_csa = csa_create();
    tmp = ncGetAttributeDoubleDefault(ncF, ncV, NCATT_SCALE_FACTOR, 1.0);
    data->xScale[0] = tmp; 
    data->yScale[0] = tmp; 
    data->zScale[0] = tmp; 
    tmp = ncGetAttributeDoubleDefault(ncF, ncV, NCATT_ADD_OFFSET, 0.0);
    data->xScale[1] = tmp;
    data->yScale[1] = tmp;
    data->zScale[1] = tmp;
    data->isInitialized = false;
    data->points = (double *)malloc(3*l*sizeof(double));
    if (! data->points) ncdrError(NCDR_ENOMEM, NCDR_ENOMEM_TXT);
    nc_get_var_double(ncF, ncV, data->points);
    csa_addpoints((csa *)(data->_csa), l, (point *)(data->points));
    d = data->points;
    data->xRange[0] = data->xRange[1] = *d;
    data->yRange[0] = data->yRange[1] = *(++d);
    data->zRange[0] = data->zRange[1] = *(++d);
    for (i=1; i<l; i++) {
        d++;
        if (*d < data->xRange[0]) data->xRange[0] = *d;
        if (*d > data->xRange[1]) data->xRange[1] = *d;
        d++;
        if (*d < data->yRange[0]) data->yRange[0] = *d;
        if (*d > data->yRange[1]) data->yRange[1] = *d;
        d++;
        if (*d < data->zRange[0]) data->zRange[0] = *d;
        if (*d > data->zRange[1]) data->zRange[1] = *d;
    }
    l = ncGetAttributeLongDefault(ncF, ncV, NCATT_CSA_NPMIN, 3);
    csa_setnpmin((csa*)(data->_csa), l);
    l = ncGetAttributeLongDefault(ncF, ncV, NCATT_CSA_NPMAX, 40);
    csa_setnpmax((csa*)(data->_csa), l);
    l = ncGetAttributeLongDefault(ncF, ncV, NCATT_CSA_K, 140);
    csa_setk((csa*)(data->_csa), l);
    l = ncGetAttributeLongDefault(ncF, ncV, NCATT_CSA_NPPC, 5);
    csa_setnppc((csa*)(data->_csa), l);
    ncError(nc_close(ncF));
    return data;
}


void DLL_EXPORT ncScattered2DInit(NcScattered2D *data) {
    if (data->isInitialized == false) {
        csa_calculatespline((csa*)(data->_csa));
        /* now we can forget the points */
        assert(data->points);
        free(data->points);
        data->points = NULL;
        data->isInitialized = true;
    }
}


void DLL_EXPORT ncScattered2DFree(NcScattered2D *data) {
    assert(data);
    assert(data->_csa);
    csa_destroy((csa*)(data->_csa));
    if ((data->points) && (data->isInitialized == false))
        free(data->points);
    free(data);
}


double DLL_EXPORT ncScattered2DGet(NcScattered2D *data, double x, double y) {
    point p;
    assert(data);
    if (data->isInitialized == false) ncScattered2DInit(data);
    p.x = x * data->xScale[0] + data->xScale[1];
    p.y = y * data->yScale[0] + data->yScale[1];
    csa_approximatepoint((csa*)(data->_csa), &p);
    return p.z * data->zScale[0] + data->zScale[1];
}


int DLL_EXPORT ncScattered2DSetOption(NcScattered2D *data,
                                       Scattered2DOption option, ...) {
    int res = 0;
    va_list ap;
    va_start(ap, option);
    assert(data);
    assert(data->isInitialized == false);
    switch (option) {
        case OpScattered2DK:
            csa_setk((csa*)(data->_csa), va_arg(ap, int));
            res = 1;
            break;
        case OpScattered2DPointsMin:
            csa_setnpmin((csa*)(data->_csa), va_arg(ap, int));
            res = 1;
            break;
        case OpScattered2DPointsMax:
            csa_setnpmax((csa*)(data->_csa), va_arg(ap, int));
            res = 1;
            break;
        case OpScattered2DPointsPerCell:
            csa_setnppc((csa*)(data->_csa), va_arg(ap, int));
            res = 1;
            break;
        case OpScattered2DScalingX:
            data->xScale[0] = va_arg(ap, double);
            data->xScale[1] = va_arg(ap, double);
            res = 1;
            break;
        case OpScattered2DScalingY:
            data->yScale[0] = va_arg(ap, double);
            data->yScale[1] = va_arg(ap, double);
            res = 1;
            break;
        case OpScattered2DScalingZ:
            data->zScale[0] = va_arg(ap, double);
            data->zScale[1] = va_arg(ap, double);
            res = 1;
            break;
        case OpScattered2DScaling:
            data->xScale[0] = va_arg(ap, double);
            data->xScale[1] = va_arg(ap, double);
            data->yScale[0] = data->xScale[0];
            data->yScale[1] = data->xScale[1];
            data->zScale[0] = data->xScale[0];
            data->zScale[1] = data->xScale[1];
            res = 1;
            break;
    }
    return res;
}


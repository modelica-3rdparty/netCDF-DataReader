/*  ncDataReader2 - interpolating reading of 1D data sets
    Copyright (C) 2012  Joerg Raedler <joerg@j-raedler.de>

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

/* #include <stdlib.h> */
#include <math.h>
#include "../config.h"
#include "IpSinSteps.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static double *xBuf=NULL;
static double *yBuf=NULL;
static size_t bufSize=0;

static INLINE double cosWinIntegral(double a, double b, double x) {
  return 0.5 * (sin(x)*(b + a*x) + a*cos(x) + 0.5*a*x*x + b*x);
}


static INLINE void resizeBuffer(size_t dim) {
    if (bufSize < dim) {
        xBuf = realloc(xBuf, dim*sizeof(double));
        yBuf = realloc(yBuf, dim*sizeof(double));
    }
}


double ncVar1DGetCosWin(NcVar1D *var, double x) {
    size_t i, j, k;
    double x0, x1, xstart, xend, y, a, b, ta, tb, xk, yk, xk1, yk1, ws;
    NcDataSet1D *dataSet;

    dataSet = var->dataSet;
    ws = var->smoothing;
    assert(ws < (dataSet->max - dataSet->min));

    xstart = x - 0.5*ws;
    xend   = x + 0.5*ws;
    
    i = ncDataSet1DSearch(dataSet, &xstart);
    if (i == dataSet->dim-1) i--;
    j = ncDataSet1DSearch(dataSet, &xend);
    if (j >= dataSet->dim-1) j--;
    if (dataSet->extra == EpPeriodic) x = 0.5 * (xstart+xend);
    if (dataSet->extra == EpConstant) {
        /* reset borders to original value */
        xstart = x - 0.5*ws;
        xend   = x + 0.5*ws;
    }
    
    if ((xstart >= dataSet->min) && (xend <= dataSet->max)) {
        /* simple case: completely inside */
        xk1 = ncDataSet1DGetItem(dataSet, i);
        yk1 = ncVar1DGetItem(var, i);
        y = 0.0;
        for (k=i; k <= j; k++){
            xk  = xk1;
            yk  = yk1;
            xk1 = ncDataSet1DGetItem(dataSet, k+1);
            yk1 = ncVar1DGetItem(var, k+1);
            x0 = ((xstart <= xk) ? xk : xstart);
            x1 = ((xend <= xk1) ? xend : xk1);
            a = ((xk == xk1) ? 0.0 : (yk1-yk) / (xk1 - xk)); 
            b  = yk - a * xk;
            ta = a * ws / (2.0 * M_PI);
            tb = b + a * x;
            y += cosWinIntegral(ta, tb, ((x1-x)*2.0*M_PI/ws));
            y -= cosWinIntegral(ta, tb, ((x0-x)*2.0*M_PI/ws));
        }
        return y / M_PI;
    }
    if (xend < dataSet->min) {
        /* completely left outside of interval */
        switch (dataSet->extra) {
            case EpConstant:
                return ncVar1DGetItem(var, 0);
            case EpDefault:
                xk = ncDataSet1DGetItem(dataSet, 0);
                yk = ncVar1DGetItem(var, 0);
                xk1 = ncDataSet1DGetItem(dataSet, 1);
                yk1 = ncVar1DGetItem(var, 1);
                assert(xk != xk1);
                a = (yk1-yk)/(xk1 - xk);
                return a*x + yk-a*xk;
        }
    }
    if (xstart > dataSet->max) {
        /* completely right outside of interval */
        switch (dataSet->extra) {
            case EpConstant:
                return ncVar1DGetItem(var, dataSet->dim-1);
            case EpDefault:
                k = dataSet->dim-2;
                xk = ncDataSet1DGetItem(dataSet, k);
                yk = ncVar1DGetItem(var, k);
                k++;
                xk1 = ncDataSet1DGetItem(dataSet, k);
                yk1 = ncVar1DGetItem(var, k);
                assert(xk != xk1);
                a = (yk1-yk)/(xk1 - xk);
                return a*x + yk-a*xk;
        }
    }
    /* so we overlap the borders: use temporary buffers */
    switch (dataSet->extra) {
        /* FIXME! */
        case EpPeriodic:
            break;
        case EpDefault:
            break;
        case EpConstant:
            break;
    }
    return 0;
}

/*

double ncVar1DGetCosWin(NcVar1D *var, double x) {
    size_t i, j, k;
    double x0, x1, xstart, xend, y, a, b, ta, tb, xk, yk, xk1, yk1, ws;
    NcDataSet1D *dataSet;

    dataSet = var->dataSet;
    ws = var->smoothing;
    assert(ws < (dataSet->max - dataSet->min));

    a = xstart = x - 0.5*ws;
    b = xend   = x + 0.5*ws;
    
    i = ncDataSet1DSearch(dataSet, &xstart);
    if (i == dataSet->dim-1) i--;
    j = ncDataSet1DSearch(dataSet, &xend);
    if (j >= dataSet->dim-1) j--;

    if (dataSet->extra != EpPeriodic) {
        xstart = a;
        xend = b;
    }
    if (xstart < dataSet->min) {
        if (xend <= dataSet->mn) {
            switch (dataSet->extra) {
                case EpConstant:
                    return ncVar1DGetItem(var, 0);
                case EpDefault:
                    xk = ncDataSet1DGetItem(dataSet, 0);
                    yk = ncVar1DGetItem(var, 0);
                    xk1 = ncDataSet1DGetItem(dataSet, 1);
                    yk1 = ncVar1DGetItem(var, 1);
                    assert(xk != xk1);
                    a = (yk1-yk)/(xk1 - xk);
                    return a*x + yk-a*xk;
            }
        }
        xk1 = xstart;
        switch (dataSet->extra) {
            case EpConstant:
                xk1 = ncVar1DGetItem(var, i);
                break;
            case EpDefault:
                xk = ncDataSet1DGetItem(dataSet, 0);
                yk = ncVar1DGetItem(var, 0);
                xk1 = ncDataSet1DGetItem(dataSet, 1);
                yk1 = ncVar1DGetItem(var, 1);
                assert(xk != xk1);
                a = (yk1-yk)/(xk1 - xk);
                return a*x + yk-a*xk;
        }
    } else {
        if (xstart > dataSet->max) {
            switch (dataSet->extra) {
                case EpConstant:
                    return ncVar1DGetItem(var, dataSet->dim-1);
                case EpDefault:
                    k = dataSet->dim-2;
                    xk = ncDataSet1DGetItem(dataSet, k);
                    yk = ncVar1DGetItem(var, k);
                    k++;
                    xk1 = ncDataSet1DGetItem(dataSet, k);
                    yk1 = ncVar1DGetItem(var, k);
                    assert(xk != xk1);
                    a = (yk1-yk)/(xk1 - xk);
                    return a*x + yk-a*xk;
        } else {
            xk1 = ncDataSet1DGetItem(dataSet, i);
            yk1 = ncVar1DGetItem(var, i);
        }
    }
    y = 0.0;
    for (k=i; k <= j; k++){
        xk  = xk1;
        yk  = yk1;
        xk1 = ncDataSet1DGetItem(dataSet, k+1);
        yk1 = ncVar1DGetItem(var, k+1);
        x0 = ((xstart <= xk) ? xk : xstart);
        x1 = ((xend <= xk1) ? xend : xk1);
        a = ((xk == xk1) ? 0.0 : (yk1-yk) / (xk1 - xk)); 
        b  = yk - a * xk;
        ta = a * ws / (2.0 * M_PI);
        tb = b + a * x;
        y += cosWinIntegral(ta, tb, ((x1-x)*2.0*M_PI/ws));
        y -= cosWinIntegral(ta, tb, ((x0-x)*2.0*M_PI/ws));
    }
    return y / M_PI;
}

*/
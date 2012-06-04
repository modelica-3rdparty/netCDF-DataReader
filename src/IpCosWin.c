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
        printf(">>> %ld\n", dim);
        xBuf = realloc(xBuf, dim*sizeof(double));
        yBuf = realloc(yBuf, dim*sizeof(double));
        bufSize = dim;
    }
}


double ncVar1DGetCosWin(NcVar1D *var, double x) {
    size_t i, j, k, l, m, n;
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
    if (dataSet->extra == EpConstant) {
        /* reset borders to original value */
        xstart = x - 0.5*ws;
        xend   = x + 0.5*ws;
    }
    
    if ((xstart >= dataSet->min) && (xend <= dataSet->max) && (i <= j)) {
        /* simple case: completely inside */
        if (dataSet->extra == EpPeriodic)
            /* x has to be adjusted as well */
            x = 0.5 * (xstart+xend);
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
    
    /* still here? So we overlap the borders: use temporary buffers */
    switch (dataSet->extra) {
        case EpPeriodic:
            l = dataSet->dim - i + j + 1;
            resizeBuffer(l);
            m = 0;
            n = ((x-dataSet->min) < (dataSet->max-x) ? 0 : 1); // x is right or left of transition
            for (k=i; k < dataSet->dim; k++) {
                xBuf[m] = ncDataSet1DGetItem(dataSet, k);
                if (! n) {
                    xBuf[m] += dataSet->min;
                    xBuf[m] -= dataSet->max;
                }
                yBuf[m++] = ncVar1DGetItem(var, k);
            }
            for (k=1; k<=j+1; k++) {
                xBuf[m] = ncDataSet1DGetItem(dataSet, k);
                if (n) {
                    xBuf[m] += dataSet->max;
                    xBuf[m] -= dataSet->min;
                }
                yBuf[m++] = ncVar1DGetItem(var, k);
            }
            if (n) xend += dataSet->max - dataSet->min;
            else xstart += dataSet->min - dataSet->max;
            break;
        case EpDefault:
            l = 0;
            /* FIXME! */
            break;
        case EpConstant:
            l = 0;
            /* FIXME! */
            break;
    }
    printf("--- %g: [%g %g] (%ld)\n", x, xstart, xend, l);
    for (k=0; k < l; k++)
        printf(" %g ", xBuf[k]);
    printf("\n");
    for (k=0; k < l; k++)
        printf(" %g ", yBuf[k]);
    printf("\n");
    y = 0;
    for (k=0; k<l; k++){
        x0 = ((xstart <= xBuf[k]) ? xBuf[k] : xstart);
        x1 = ((xend <= xBuf[k+1]) ? xend : xBuf[k+1]);
        a = ((xBuf[k] == xBuf[k+1]) ? 0.0 : (yBuf[k+1]-yBuf[k]) / (xBuf[k+1] - xBuf[k]));
        b  = yBuf[k] - a * xBuf[k];
        ta = a * ws / (2.0 * M_PI);
        tb = b + a * x;
        y += cosWinIntegral(ta, tb, ((x1-x)*2.0*M_PI/ws));
        y -= cosWinIntegral(ta, tb, ((x0-x)*2.0*M_PI/ws));
    }
    return y / M_PI;
}

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


static INLINE double cosWinIntegral(double a, double b, double x) {
  return 0.5 * (sin(x)*(b + a*x) + a*cos(x) + 0.5*a*x*x + b*x);
}

double ncVar1DGetCosWin(NcVar1D *var, double x) {
    size_t i, j, k;
    double x0, x1, xstart, xend, y, a, b, ta, tb, xk, yk, xk1, yk1, ws;
    
    ws = var->windowSize;
    xstart = x - 0.5*ws;
    xend   = x + 0.5*ws;
    
    i = ncDataSet1DSearch(var->dataSet, &xstart);
    j = ncDataSet1DSearch(var->dataSet, &xend);
    
    xk1 = ncDataSet1DGetItem(var->dataSet, i);
    yk1 = ncVar1DGetItem(var, i);
    y = 0.0;
    for (k=i; k <= j; k++){
        xk  = xk1;
        yk  = yk1;
        xk1 = ncDataSet1DGetItem(var->dataSet, k+1);
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

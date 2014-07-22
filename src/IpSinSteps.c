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

/* #include <stdlib.h> */
#include <math.h>
#include "IpSinSteps.h"

double ncVar1DGetSinSteps(NcVar1D *var, double x) {
    size_t i, j;
    double xi, xj, yi, yj, dtmp;
    i = ncDataSet1DSearch(var->dataSet, &x);
    j = i+1;
    if (j >= var->dataSet->dim)
        j = ((var->dataSet->extra == EpPeriodic) ? 0 : i);
    xi = ncDataSet1DGetItem(var->dataSet, i);
    xj = ncDataSet1DGetItem(var->dataSet, j);
    if ((x >= (xi + var->smoothing)) && (x <= (xj - var->smoothing))) {
        return ncVar1DGetItem(var, i);
    }
    else {
        if (x < xi+var->smoothing) {
            j = i; 
            if (i == 0)
                i = ((var->dataSet->extra == EpPeriodic) ? var->dataSet->dim-2 : j);
            else
                i = i-1;
            xj = xi;
            xi = ncDataSet1DGetItem(var->dataSet, i);
        }
        yi = ncVar1DGetItem(var, i);
        yj = ncVar1DGetItem(var, j);
    }
    dtmp = sin(0.5 * M_PI * (x - xj) / var->smoothing);
    return yi + (0.5 + 0.5 * dtmp) * (yj - yi);
}

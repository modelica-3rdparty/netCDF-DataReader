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

#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdlib.h>
#include "ncDataReader2.h"
#include "Chain.h"

size_t  ncDataSet1DLookupValue(NcDataSet1D *dataSet, double x);

/* cache handling */
Item   *lookupCacheNew(int len);
void    lookupCacheFree(Item *c);
void    lookupCacheReset(Item *c);
#endif /* LOOKUP_H */

#!/usr/bin/env python
# This does exactly the same as GenerateFile.c .
# There are several netCDF interfaces for python available,
# this script will try use the one from scipy and fall back to pupynere.
# (http://pypi.python.org/pypi/pupynere/).

try:
    from scipy.io import *
except:
    from pupynere import *
    
from math import pi, sin, tanh
from random import random

DIM = 100
NPOINTS = 1000

def createFile(fileName):
    f = netcdf_file(fileName, 'w')
    f.doc = 'test file for ncDataReader2, created by GenerateFile.py'
    f.version = '2.3.0'
    f.foo = 42.42
    f.bar = 1234
    return f

def create1DVar(f):
    f.createDimension('time', DIM)
    time = f.createVariable('time', 'd', ('time', ))
    time.scale_factor = 2.5
    time.add_offset = 0.0
    time.extrapolation = 'periodic'

    test = f.createVariable('test1D', 'd', ('time', ))
    test.scale_factor = 2.0
    test.add_offset = 3.0
    test.interpolation = 'akima'
    test.smoothing = 500.0

    step = 2.0 * pi / (DIM - 1.0)
    for i in range(DIM):
        x = pi + i*step + 0.3*random()*step
        time[i] = x
        test[i] = sin(x) + 0.1*random()

def create2DVar(f):
    f.createDimension('npoints', NPOINTS)
    f.createDimension('xyz', 3)
    points = f.createVariable('points', 'd', ('npoints', 'xyz'))
    for i in range(NPOINTS):
        x = 10.0 * random() - 5.0
        y = 10.0 * random() - 5.0
        points[i] = (x, y, tanh(x*y))

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        f = createFile(sys.argv[1])
    else:
        f = createFile('testfile.nc')
    create1DVar(f)
    create2DVar(f)
    f.sync()
    f.close()

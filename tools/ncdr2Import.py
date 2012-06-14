#!/usr/bin/env python

import sys, argparse, csv

has_netcdf   = False
has_pupynere = -1
try:
    import netCDF4
    has_netcdf4 = True
    has_netcdf = True
except:
    has_netcdf4 = False

if not has_netcdf:
    try:
        from scipy.io import netcdf_file
        has_pupynere = True
        has_netcdf = True
    except:
        has_pupynere = False
 
if not has_netcdf:
    try:
        from pupynere import netcdf_file
        has_pupynere = True
        has_netcdf = True
    except:
        has_pupynere = False

try:
    from scipy.io import loadmat
    has_mat = True
except:
    has_mat = False
        
try:
    import xlrd
    has_xlrd = True
except:
    has_xlrd = False

try:
    import odf
    has_odf = True
except:
    has_odf = False

print 'netCDF  : ', has_netcdf
print 'netCDF4 : ', has_netcdf4
print 'pupynere: ', has_pupynere
print 'MAT     : ', has_mat
print 'Excel   : ', has_xlrd
print 'ODF     : ', has_odf


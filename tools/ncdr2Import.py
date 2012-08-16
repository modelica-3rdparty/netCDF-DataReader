#!/usr/bin/env python

import sys
if sys.version_info < (2, 7, 0):
    print('error: python version >= 2.7 required')
    sys.exit(27)

import argparse, csv

epilog = "Please read the ncDataReader2 manula for additional information."
arg = argparse.ArgumentParser(description='Generate a ncDataReader2-compatible netCDF file', epilog=epilog)
arg.add_argument('-n', '--info', action='store_true', help='show some information on the file')
arg.add_argument('-o', '--outfile', nargs=1, help='write exported data to this file')
arg.add_argument('-f', '--format', nargs=1, choices=('csv', 'xls', 'odc', 'mat', 'dymat'), help='import data from this format')
arg.add_argument('-s', '--variables', nargs=1, metavar='VARS', help='export only these variables (default: all)')
arg.add_argument('-a', '--abscissa', nargs=1, help='use this variable as abscissa')
arg.add_argument('-x', '--extrapolation', nargs=1, choices=('default', 'constant', 'periodic'), help='extrapolation attribute')
arg.add_argument('-i', '--interpolation', nargs=1, choices=('akima', 'linear', 'discrete', 'sinsteps', 'coswin'), help='interpolation attribute')
arg.add_argument('-l', '--load-type', nargs=1, choices=('full', 'chunks', 'none'), help='load type attribute')
arg.add_argument('-w', '--window-size', nargs=1, type=float, help='window size for coswin interpolation')
arg.add_argument('-m', '--smoothing-radius', nargs=1, type=float, help='smoothing radius for sinsteps interpolation')
arg.add_argument('-k', '--lookup-cache', nargs=1, type=int, help='size of lookup cache')
arg.add_argument('-p', '--parameter-cache', nargs=1, type=int, help='size of parameter cache')
arg.add_argument('-v', '--value-cache', nargs=1, type=int, help='size of value cache')
arg.add_argument('-c', '--chunk-size', nargs=1, type=int, help='size of chunks for chunk loading')
arg.add_argument('-t', '--comment', nargs=1, help='comment to include in file')
arg.add_argument('inputfile', nargs=1, help='input file')
pargs = arg.parse_args()


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

if not has_netcdf:
    print('error: no package with netCDF support for python found')
    print('please install one of python-netCDF4, scipy or pupynere')
    sys.exit(10)
    
        

class Import(object):
    
    def writeNetCDF(self, ncFile):
        pass
    
    def initNC(self, ncFile):
        return None
        
    def writeAbscissa(self, aName, values):
        return None
       
    def writeVariable(self, vName, values):
        return None
        
    def listVariables(self):
        return []


class ImportCSV(Import):
    def __init__(self, inFile):
        from csv import Sniffer, reader
        csvFile = open(inFile, 'r')
        sample = csvFile.read(1024)
        csvFile.seek(0)
        self.reader = reader(csvFile, Sniffer.sniff(sample))
        if Sniffer.has_header(sample):
            self.varNames = next(self.reader)
        else:
            self.varNames = None
        
    def writeNetCDF(self, ncFile):
        self.initNC(ncFile)
        self.writeAbscissa(spam, foo)
        for v in self.varNames:
            self.writeVariable(v, bar)
            
    def listVariables(self):
        return self.varNames


class ImportMat(Import):
    def __init__(self, inFile):
        from scipy.io import loadmat


class ImportDyMat(Import):
    def __init__(self, inFile):
        import DyMat


class ImportXLS(Import):
    def __init__(self, inFile):
        import xlrd

        
class ImportODC(Import):
    def __init__(self, inFile):
        import odf

importer = {
        'csv'   : ImportCSV,
        'mat'   : ImportMat,
        'dymat' : ImportDyMat,
        'xls'   : ImportXLS,
        'odc'   : ImportODC,
    }

print pargs

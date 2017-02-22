# NcDataReader2
Modelica library for interfacing [netCDF](https://github.com/Unidata/netcdf-c)
files using different interpolation and extrapolation methods.

## Build status
[![Build Status](https://travis-ci.org/tbeu/netCDF-DataReader.svg?branch=master)](https://travis-ci.org/tbeu/netCDF-DataReader)

## Library description
NcDataReader2 is a library of C functions to access data stored in netCDF files
using different interpolation and extrapolation methods. The aim of this library
is to provide access from simulation systems like Modelica to data sets like
weather data or measured time rows. As such systems usually require strictly
continuity of functions and their derivatives, smooth spline interpolation is
included.

For more information and installation instructions please have look at the
[doc](NcDataReader2/Resources/doc) folder.

The original repository is hosted on [Bitbucket](https://bitbucket.org/jraedler/ncdatareader2)
by [Jörg Rädler](http://www.j-raedler.de/projects/ncdatareader2).

## License
NcDataReader2 is released under the terms of the LGPL v2.

## Development and contribution
* [Jörg Rädler](https://github.com/jraedler)
* [tbeu](https://github.com/tbeu)

You may report any issues with using the [Issues](../../issues) button.

Contributions in shape of [Pull Requests](../../pulls) are always welcome.

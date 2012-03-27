% This does exactly the same as GenerateFile.c or GenerateFile.py

DIM = 100;
NPOINTS = 1000;

f = netcdf('testfile.nc','c');
f.doc = 'test file for ncDataReader2, created by GenerateFile.m';
f.version = '2.2.0';
f.foo = 42.42;
f.bar = 1234;

% create dimension and variables in 1D
f('time') = DIM;

f{'time'} = ncdouble('time');
f{'time'}.scale_factor = 2.5;
f{'time'}.add_offset = 0.0;
f{'time'}.extrapolation = 'periodic';

f{'test1D'} = ncdouble('time');
f{'test1D'}.scale_factor = 2.0;
f{'test1D'}.add_offset = 3.0;
f{'test1D'}.interpolation = 'akima';
f{'test1D'}.smoothing = 500.0;

step = 2.0 * pi / (DIM - 1.0);
for i = 1:1:DIM
    x = pi + i*step + 0.3*rand()*step;
    f{'time'}(i) = x;
    f{'test1D'}(i) = sin(x) + 0.1*rand();
end

% create dimension and variables for 2D
f('npoints') = NPOINTS;
f('xyz') = 3;

f{'points'} = ncdouble('npoints', 'xyz');

for i = 1:1:NPOINTS
    x = 10.0 * rand() - 5.0;
    y = 10.0 * rand() - 5.0;
    f{'points'}(i,1) = x;
    f{'points'}(i,2) = y;
    f{'points'}(i,3) = tanh(x*y);
end

close(f)

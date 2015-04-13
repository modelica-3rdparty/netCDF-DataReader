within NcDataReader2.Examples;

model Simple "Simple test model for ncDataReader2"
    import nc = NcDataReader2.Functions;
    parameter String fileName = "/var/tmp/testfile.nc";
    Real x "dummy variable to integrate";
    Real a = nc.ncEasyGetAttributeDouble(fileName, "", "foo") "dummy";
    Real y "dummy variable";
    Real T "temperature";
    
    equation
    y = x * a;
    der(x) = T;
    T = nc.ncEasyGet1D(fileName, "test1D", time / 1000);
    annotation(uses(Modelica(version = "3.2")), experiment(StartTime = 0, StopTime = 10000, NumberOfIntervals = 5000), experimentSetupOutput);
end Simple;

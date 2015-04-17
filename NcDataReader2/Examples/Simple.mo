within NcDataReader2.Examples;

model Simple "Simple test model for ncDataReader2"
    import nc = NcDataReader2.Functions;
    parameter String fileName = Modelica.Utilities.Files.loadResource("modelica://NcDataReader2/Resources/examples/testfile.nc");
    Real x "dummy variable to integrate";
    Real a = nc.ncEasyGetAttributeDouble(fileName, "", "foo") "dummy";
    Real y "dummy variable";
    Real T "temperature";
    
    equation
    y = x * a;
    der(x) = T;
    T = nc.ncEasyGet1D(fileName, "test1D", time / 1000);
    annotation(uses(Modelica(version = "3.2.1")), experiment(StartTime = 0, StopTime = 10000, Interval = 2));
end Simple;

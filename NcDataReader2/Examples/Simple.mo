within NcDataReader2.Examples;

model Simple "Simple test model for NcDataReader2"
    extends Modelica.Icons.Example;
    import nc = NcDataReader2.Functions;
    parameter String fileName = Modelica.Utilities.Files.loadResource("modelica://NcDataReader2/Resources/examples/testfile.nc") "File where external data is stored"
      annotation(Dialog(
        loadSelector(filter="netCDF files (*.nc)",
        caption="Open file")));
    Real x(start=0.0, fixed=true) "Dummy variable to integrate";
    Real a = nc.ncEasyGetAttributeDouble(fileName, "", "foo") "Dummy";
    Real y "Dummy variable";
    Real T "Temperature";

equation
    y = x * a;
    der(x) = T;
    T = nc.ncEasyGet1D(fileName, "test1D", time / 1000);
    annotation(experiment(StartTime = 0, StopTime = 10000, Interval = 2));
end Simple;

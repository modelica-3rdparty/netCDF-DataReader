within NcDataReader2.Examples;

model FullAPI "Test model for NcDataReader2 using the full API"
    extends Modelica.Icons.Example;
    parameter String fileName = Modelica.Utilities.Files.loadResource("modelica://NcDataReader2/Resources/examples/testfilebig.nc") "File where external data is stored"
      annotation(Dialog(
        loadSelector(filter="netCDF files (*.nc)",
        caption="Open file")));

    // this model needs FullAPIFunctions.h,
    // if in doubt, copy the file to the working directory
    // where the compiler can find it

    function getV1 "Get v1 from file"
        extends Modelica.Icons.Function;
        input String fileName;
        input Real x;
        output Real y;
        external "C" y=getV1(fileName, x) annotation(__iti_dll = "ITI_ncDataReader2.dll", __iti_dllNoExport = true, Include = "#include <FullAPIFunctions.h>", Library = {"ncDataReader2", "netcdf"});
    end getV1;

    function getV2 "Get v2 from file"
        extends Modelica.Icons.Function;
        input String fileName;
        input Real x;
        output Real y;
        external "C" y=getV2(fileName, x) annotation(__iti_dll = "ITI_ncDataReader2.dll", __iti_dllNoExport = true, Include = "#include \"FullAPIFunctions.h\"", Library = {"ncDataReader2", "netcdf"});
    end getV2;

    Real x(start=0.0, fixed=true) "Dummy variable";
    Real T1 "Temperature 1";
    Real T2 "Temperature 2";

equation
    der(x) = T1 + T2;
    T1 = getV1(fileName, time);
    T2 = getV2(fileName, time);
    annotation(experiment(StartTime = 100, StopTime = 150, Interval = 0.0001));
end FullAPI;

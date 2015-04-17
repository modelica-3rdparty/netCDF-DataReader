within NcDataReader2.Examples;

model FullAPI "Test model for ncDataReader2 using the full API"

    // this model needs FullAPIFunctions.c,
    // if in doubt, copy the file to the working directory
    // where the compiler can find it

    function getV1 "get v1 from file"
        input Real x;
        output Real y;
        annotation(Include = "#include <FullAPIFunctions.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end getV1;

    function getV2 "get v2 from file"
        input Real x;
        output Real y;
        annotation(Include = "#include <FullAPIFunctions.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end getV2;

    Real x "dummy variable";
    Real T1 "temperature 1";
    Real T2 "temperature 2";

    equation 
    der(x) = T1 + T2;
    T1 = getV1(time);
    T2 = getV2(time);
    annotation(uses(Modelica(version = "3.2")), experiment(StartTime = 100, StopTime = 150, NumberOfIntervals = 500000), experimentSetupOutput);

end FullAPI;

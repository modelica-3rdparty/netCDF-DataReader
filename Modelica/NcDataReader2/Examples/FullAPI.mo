within NcDataReader2.Examples;

model FullAPI "Test model for ncDataReader2 using the full API"
    import FullAPIFunctions;
    Real y "dummy variable to integrate";
    Real x "dummy variable";
    Real t1 "temperature 1";
    Real t2 "temperature 2";

    equation 
    y = x;
    der(x) = t1 + t2;
    t1 = getV1(time);
    t2 = getV2(time);
    annotation(uses(Modelica(version = "3.1")), experiment(StartTime = 1000, StopTime = 1100, NumberOfIntervals = 500000), experimentSetupOutput);
end FullAPI;

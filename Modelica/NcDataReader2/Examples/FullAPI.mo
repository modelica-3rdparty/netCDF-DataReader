within NcDataReader2.Examples;

model FullAPI "Test model for ncDataReader2 using the full API"
    // this model needs FullAPIFunctions.c
    import FullAPIFunctions;
    Real x "dummy variable";
    Real t1 "temperature 1";
    Real t2 "temperature 2";

    equation 
    der(x) = t1 + t2;
    t1 = getV1(time);
    t2 = getV2(time);
    annotation(uses(Modelica(version = "3.1")), experiment(StartTime = 100, StopTime = 150, NumberOfIntervals = 500000), experimentSetupOutput);
end FullAPI;

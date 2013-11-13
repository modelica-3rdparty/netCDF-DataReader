within NcDataReader2.Examples;

model BigFile "Test model for ncDataReader2 with large data sets"
    import nc = NcDataReader2.Functions;
    Real y "dummy variable to integrate";
    Real x "dummy variable";
    Real t1 "temperature 1";
    Real t2 "temperature 2";
    String dataFile = "/var/tmp/testfilebig.nc";
    String logFile  = "/var/tmp/ncdr.log";

    algorithm
    when terminal() then
        nc.ncEasyDumpStatistics(logFile);
    end when;
    
    equation 
    y = x;
    der(x) = t1 + t2;
    t1 = nc.ncEasyGet1D(dataFile, "big_var_00", time);
    t2 = nc.ncEasyGet1D(dataFile, "big_var_01", time);
    annotation(uses(Modelica(version = "3.2")), experiment(StartTime = 1000, StopTime = 1100, NumberOfIntervals = 500000), experimentSetupOutput);
end BigFile;

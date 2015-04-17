within NcDataReader2.Examples;

model BigFile "Test model for ncDataReader2 with large data sets"
    import nc = NcDataReader2.Functions;
    Real y "dummy variable to integrate";
    Real x "dummy variable";
    Real T1 "temperature 1";
    Real T2 "temperature 2";
    String dataFile = Modelica.Utilities.Files.loadResource("modelica://NcDataReader2/Resources/examples/testfilebig.nc");
    String logFile  = "ncdr.log";

    algorithm
    when terminal() then
        nc.ncEasyDumpStatistics(logFile);
    end when;
    
    equation 
    y = x;
    der(x) = T1 + T2;
    T1 = nc.ncEasyGet1D(dataFile, "big_var_00", time);
    T2 = nc.ncEasyGet1D(dataFile, "big_var_01", time);
    annotation(uses(Modelica(version = "3.2.1")), experiment(StartTime = 1000, StopTime = 1100, Interval = 0.0002));
end BigFile;

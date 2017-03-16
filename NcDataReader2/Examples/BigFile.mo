within NcDataReader2.Examples;

model BigFile "Test model for NcDataReader2 with large data sets"
    extends Modelica.Icons.Example;
    import nc = NcDataReader2.Functions;
    parameter String dataFile = Modelica.Utilities.Files.loadResource("modelica://NcDataReader2/Resources/examples/testfilebig.nc") "File where external data is stored"
      annotation(Dialog(
        loadSelector(filter="netCDF files (*.nc)",
        caption="Open file")));
    parameter String logFile = "ncdr.log" "Logfile (empty string is the terminal)"
      annotation(Dialog(
        saveSelector(filter="Logfiles (*.log)",
        caption="Save logfile")));
    Real y "dummy variable to integrate";
    Real x(start=0.0, fixed=true) "dummy variable";
    Real T1 "temperature 1";
    Real T2 "temperature 2";

    algorithm
    when terminal() then
        nc.ncEasyDumpStatistics(logFile);
    end when;
    
    equation 
    y = x;
    der(x) = T1 + T2;
    T1 = nc.ncEasyGet1D(dataFile, "big_var_00", time);
    T2 = nc.ncEasyGet1D(dataFile, "big_var_01", time);
    annotation(experiment(StartTime = 1000, StopTime = 1100, Interval = 0.0002));
end BigFile;

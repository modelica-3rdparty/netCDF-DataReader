package NcDataReader2

    package Functions
    
    function ncEasyGet1D "get an interpolated value from a netCDF file"
        input String fileName;
        input String varName;
        input Real x;
        output Real y;
        annotation(Icon(graphics = {Line(points = {{60,-60},{-60,-60},{-60,20}}, rotation = 0, color = {0,0,255}, pattern = LinePattern.Solid, thickness = 0.25),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{-58,-50},{-54,-54}}),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{-20,-10},{-16,-14}}),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{56,-32},{52,-36}}),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{-40,2},{-44,6}}),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{34,-8},{30,-12}}),Line(points = {{-56,-52},{-40,30},{4,-56},{26,0},{54,-34}}, rotation = 0, color = {255,0,0}, pattern = LinePattern.Solid, thickness = 0.25, smooth = Smooth.Bezier),Rectangle(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25, extent = {{2,-36},{6,-40}}),Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, rotation = 0, color = {0,0,0}, pattern = LinePattern.Solid, thickness = 0.25),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, rotation = 0, lineColor = {0,0,0}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25),Text(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.None, lineThickness = 0.25, extent = {{-20.8044,63.8003},{68.2386,-1.10957}}, textString = "1D", fontName = "Arial")}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end ncEasyGet1D;
    
    function ncEasyGetScattered2D "get an interpolated value from a netCDF file"
        input String fileName;
        input String varName;
        input Real x;
        input Real y;
        output Real z;
        annotation(Icon(graphics = {Line(points = {{60,-60},{-60,-60},{-60,20}}, color = {0,0,255}, smooth = Smooth.None),Rectangle(extent = {{-58,-50},{-54,-54}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{-20,-10},{-16,-14}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{56,-32},{52,-36}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{-40,2},{-44,6}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{34,-8},{30,-12}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{8,-18},{12,-22}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Text(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.None, lineThickness = 0.25, extent = {{-20.8044,63.8003},{68.2386,-1.10957}}, textString = "2D", fontName = "Arial"),Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, color = {0,0,0}, smooth = Smooth.None),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, lineColor = {0,0,0}, smooth = Smooth.None, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Line(points = {{-60,-60},{14,0}}, color = {0,0,255}, smooth = Smooth.None),Rectangle(extent = {{28,-34},{32,-38}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{-22,-40},{-18,-44}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{-2,0},{2,-4}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Rectangle(extent = {{-40,-18},{-36,-22}}, lineColor = {0,0,255}, fillColor = {0,0,0}, fillPattern = FillPattern.Solid)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end ncEasyGetScattered2D;
    
    function ncEasyGetAttributeDouble "get an attribute from a netCDF file as Real"
        input String fileName;
        input String varName;
        input String attName;
        output Real a;
        annotation(Icon(graphics = {Line(points = {{-40,90.2774},{-90,40.2774},{-90,-89.7226},{90,-89.7226},{90,90.2774},{-40,90.2774}}, rotation = 0, color = {0,0,0}, pattern = LinePattern.Solid, thickness = 0.25),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, rotation = 0, lineColor = {0,0,0}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25),Line(points = {{-60,-60},{0,60},{60,-60}}, rotation = 0, color = {0,0,255}, pattern = LinePattern.Solid, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, rotation = 0, color = {0,0,255}, pattern = LinePattern.Solid, thickness = 0.5)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end ncEasyGetAttributeDouble;
    
    function ncEasyGetAttributeLong "get an attribute from a netCDF file as Integer"
        input String fileName;
        input String varName;
        input String attName;
        output Integer a;
        annotation(Icon(graphics = {Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, color = {0,0,0}, smooth = Smooth.None),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, lineColor = {0,0,0}, smooth = Smooth.None, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Line(points = {{-60,-60},{0,60},{60,-60}}, color = {0,127,0}, smooth = Smooth.None, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, color = {0,127,0}, smooth = Smooth.None, thickness = 0.5)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end ncEasyGetAttributeLong;
    
    function ncEasyGetAttributeString "get an attribute from a netCDF file as String"
        input String fileName;
        input String varName;
        input String attName;
        output String a;
        annotation(Icon(graphics = {Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, color = {0,0,0}, smooth = Smooth.None),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, lineColor = {0,0,0}, smooth = Smooth.None, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Line(points = {{-60,-60},{0,60},{60,-60}}, color = {127,0,0}, smooth = Smooth.None, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, color = {127,0,0}, smooth = Smooth.None, thickness = 0.5)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
        external "C";
    end ncEasyGetAttributeString;
    
    annotation(Icon, uses(Modelica(version = "3.1")), Diagram(graphics = {Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, rotation = 0, color = {0,0,0}, pattern = LinePattern.Solid, thickness = 0.25),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, rotation = 0, lineColor = {0,0,0}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25),Text(rotation = 0, lineColor = {0,0,255}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.None, lineThickness = 0.25, extent = {{-64.918,-10.4918},{77.9273,-53.8696}}, textString = "netCDF")}));

    end Functions;

    package Examples
    
        model NcTestSimple "Very simple test model for ncDataReader2"
            import nc = NcDataReader2.Functions;
            parameter String fileName = "/var/tmp/testfile.nc";
            Real x "dummy variable to integrate";
            Real a = nc.ncEasyGetAttributeDouble(fileName, "", "foo") "dummy";
            Real y "dummy variable";
            Real t "temperature";
            
            equation
            y = x * a;
            der(x) = t;
            t = nc.ncEasyGet1D(fileName, "test1D", time / 1000);
            annotation(uses(Modelica(version = "3.1")), experiment(StartTime = 0, StopTime = 10000, NumberOfIntervals = 5000), experimentSetupOutput);
        end NcTestSimple;

        model NcTestBig "Test model for ncDataReader2 with very large data sets"
            import nc = NcDataReader2.Functions;
            Real y "dummy variable to integrate";
            Real x "dummy variable";
            Real t1 "temperature 1";
            Real t2 "temperature 2";
            String dataFile = "/var/tmp/testfilebig.nc";
            // String dataFile = "http://192.168.56.100/test/testfilebig.nc";

            equation 
            y = x;
            der(x) = t1 + t2;
            t1 = nc.ncEasyGet1D(dataFile, "big_var_00", time);
            t2 = nc.ncEasyGet1D(dataFile, "big_var_01", time);
            annotation(uses(Modelica(version = "3.1")), experiment(StartTime = 1000, StopTime = 1100, NumberOfIntervals = 500000), experimentSetupOutput);
        end NcTestBig;

    end Examples;
  
end NcDataReader2;


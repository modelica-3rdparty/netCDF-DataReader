within NcDataReader2.Functions;

function ncEasyGetAttributeDouble "get an attribute from a netCDF file as Real"
    input String fileName;
    input String varName;
    input String attName;
    output Real a;
    annotation(Icon(graphics = {Line(points = {{-40,90.2774},{-90,40.2774},{-90,-89.7226},{90,-89.7226},{90,90.2774},{-40,90.2774}}, rotation = 0, color = {0,0,0}, pattern = LinePattern.Solid, thickness = 0.25),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, rotation = 0, lineColor = {0,0,0}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25),Line(points = {{-60,-60},{0,60},{60,-60}}, rotation = 0, color = {0,0,255}, pattern = LinePattern.Solid, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, rotation = 0, color = {0,0,255}, pattern = LinePattern.Solid, thickness = 0.5)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
    external "C";
end ncEasyGetAttributeDouble;

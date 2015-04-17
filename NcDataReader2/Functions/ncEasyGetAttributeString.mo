within NcDataReader2.Functions;

function ncEasyGetAttributeString "get an attribute from a netCDF file as String"
    input String fileName;
    input String varName;
    input String attName;
    output String a;
    annotation(Icon(graphics = {Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, color = {0,0,0}, smooth = Smooth.None),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, lineColor = {0,0,0}, smooth = Smooth.None, fillColor = {0,0,0}, fillPattern = FillPattern.Solid),Line(points = {{-60,-60},{0,60},{60,-60}}, color = {127,0,0}, smooth = Smooth.None, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, color = {127,0,0}, smooth = Smooth.None, thickness = 0.5)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
    external "C";
end ncEasyGetAttributeString;

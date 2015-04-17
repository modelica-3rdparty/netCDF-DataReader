within NcDataReader2.Functions;

function ncEasyDumpStatistics "dumps statistics on data loading and caches"
    input String fileName;
    output Integer res;
    annotation(Icon(graphics = {Line(points = {{-40,90.2774},{-90,40.2774},{-90,-89.7226},{90,-89.7226},{90,90.2774},{-40,90.2774}}, rotation = 0, color = {0,0,0}, pattern = LinePattern.Solid, thickness = 0.25),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, rotation = 0, lineColor = {0,0,0}, fillColor = {0,0,0}, pattern = LinePattern.Solid, fillPattern = FillPattern.Solid, lineThickness = 0.25)}), Include = "#include <ncDataReaderEA.h>", Library = {"ncDataReader2","netcdf"});
    external "C";
end ncEasyDumpStatistics;

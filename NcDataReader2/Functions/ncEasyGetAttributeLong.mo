within NcDataReader2.Functions;

function ncEasyGetAttributeLong "get an attribute from a netCDF file as Integer"
    input String fileName;
    input String varName;
    input String attName;
    output Integer a;
    external "C" a=ncEasyGetAttributeLong(fileName, varName, attName) annotation(__iti_dll = "ITI_ncDataReader2.dll",
     __iti_dllNoExport = true, __iti_dllDirectory="modelica://NcDataReader2/Resources/Library",
     Include = "#include <ncDataReaderEA.h>", IncludeDirectory = "modelica://NcDataReader2/Resources/Include",
     Library = {"ncDataReader2", "netcdf"}, LibraryDirectory="modelica://NcDataReader2/Resources/Library");
    annotation(Icon(graphics={  Line(points = {{-40,90},{-90,40},{-90,-90},{90,-90},{90,90},{-40,90}}, color = {0,0,0}, smooth = Smooth.None),Polygon(points = {{-40,90},{-40,40},{-90,40},{-40,90}}, lineColor = {0,0,0}, smooth = Smooth.None, fillColor = {0,0,0},
            fillPattern =                                                                                                                                                                                                        FillPattern.Solid),Line(points = {{-60,-60},{0,60},{60,-60}}, color = {0,127,0}, smooth = Smooth.None, thickness = 0.5),Line(points = {{-36,-12},{36,-12}}, color = {0,127,0}, smooth = Smooth.None, thickness = 0.5)}));
end ncEasyGetAttributeLong;

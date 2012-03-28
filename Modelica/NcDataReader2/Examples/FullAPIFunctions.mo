
function getV1 "get v1 from file"
    input Real x;
    output Real y;
    annotation(Include = "#include <FullAPIFunctions.c>", Library = {"ncDataReader2","netcdf"});
    external "C";
end getV1;

function getV2 "get v2 from file"
    input Real x;
    output Real y;
    annotation(Include = "#include <FullAPIFunctions.c>", Library = {"ncDataReader2","netcdf"});
    external "C";
end getV2;

# ncDataReader2 library - the main target
add_library(ncDataReader2 ${LIB_BUILD_TYPE}
  Misc.c Lookup.c Chain.c ncVar1D.c ncDataSet1D.c
  IpLinear.c IpSinSteps.c IpAkima.c IpCosWin.c 
  ncScattered2D.c csa.c svd.c
  ncDataReaderEA.c StringHashTable.c
)
target_link_libraries(ncDataReader2 ${NC_LIB})

# change some flags in the header files
configure_file("ncDataReader2.h.in" "ncDataReader2.h")
configure_file("ncDataReaderEA.h.in" "ncDataReaderEA.h")

# what to install
install(FILES ncDataReader2.h ncDataReaderEA.h DESTINATION include)
install(TARGETS ncDataReader2
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

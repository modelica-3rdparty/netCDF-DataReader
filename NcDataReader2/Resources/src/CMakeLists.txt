# ncDataReader2 library - the main target
if(BUILD_SHARED_LIB AND MSVC)
  add_library(ITI_ncDataReader2 ${LIB_BUILD_TYPE}
    Misc.c Lookup.c Chain.c ncVar1D.c ncDataSet1D.c
    IpLinear.c IpSinSteps.c IpAkima.c IpCosWin.c 
    ncScattered2D.c csa.c svd.c
    ncDataReaderEA.c StringHashTable.c
  )
else(BUILD_SHARED_LIB AND MSVC)
  add_library(ncDataReader2 ${LIB_BUILD_TYPE}
    Misc.c Lookup.c Chain.c ncVar1D.c ncDataSet1D.c
    IpLinear.c IpSinSteps.c IpAkima.c IpCosWin.c 
    ncScattered2D.c csa.c svd.c
    ncDataReaderEA.c StringHashTable.c
  )
endif(BUILD_SHARED_LIB AND MSVC)
if(BUILD_SHARED_LIB AND MSVC)
  target_link_libraries(ITI_ncDataReader2 ${NC_LIB} ${MODELICA_EXTERNALC_LIB})
else(BUILD_SHARED_LIB AND MSVC)
  target_link_libraries(ncDataReader2 ${NC_LIB})
endif(BUILD_SHARED_LIB AND MSVC)

# change some flags in the header files
configure_file("ncDataReader2.h.in" "ncDataReader2.h")
configure_file("ncDataReaderEA.h.in" "ncDataReaderEA.h")

# what to install
install(FILES ncDataReader2.h ncDataReaderEA.h DESTINATION include)
if(BUILD_SHARED_LIB AND MSVC)
  install(TARGETS ITI_ncDataReader2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
else(BUILD_SHARED_LIB AND MSVC)
  install(TARGETS ncDataReader2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif(BUILD_SHARED_LIB AND MSVC)

set (LIBS ncDataReader2 ${NC_LIB} ${MATH_LIB})

set (T ncdr2Dump1D )
add_executable (${T} ${T}.c getopt.c)
target_link_libraries (${T} ${LIBS})
install(TARGETS ${T} RUNTIME DESTINATION bin)

set (T ncdr2ImportCSV1D )
add_executable (${T} ${T}.c getopt.c csv.c)
target_link_libraries (${T} ${LIBS})
install(TARGETS ${T} RUNTIME DESTINATION bin)

set (LIBS ncDataReader2 ${NC_LIB} ${MATH_LIB})

set (TOOLS ncdr2Dump1D)

foreach (T ${TOOLS})
    add_executable (${T} ${T}.c getopt.c)
    target_link_libraries (${T} ${LIBS})
    install(TARGETS ncdr2Dump1D RUNTIME DESTINATION bin)
endforeach(T)

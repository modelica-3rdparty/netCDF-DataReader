set (LIBS ncDataReader2 ${NC_LIB} ${MATH_LIB})

set (TOOLS ncdr2Dump1D)

foreach (T ${EXAMPLES})
    add_executable (${T} ${T}.c)
    target_link_libraries (${T} ${LIBS})
endforeach(T)

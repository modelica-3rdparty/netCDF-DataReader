set (LIBS ncDataReader2 ${NC_LIB} ${MATH_LIB})

set (EXAMPLES GenerateFile GenerateBigFile CacheTests PrintVersion Read1D 
    Read1DChunk Read1DEasyAPI Read2D Read2DEasyApi)

foreach (EX ${EXAMPLES})
    add_executable (${EX} ${EX}.c)
    target_link_libraries (${EX} ${LIBS})
    if(UNIX)
        install(FILES ${EX}.c DESTINATION share/doc/${NCDR_PACKAGE_NAME}/examples)
    endif(UNIX)
endforeach(EX)

if(UNIX)
    install(FILES GenerateFile.m GenerateFile.py 00_README DESTINATION share/doc/${NCDR_PACKAGE_NAME}/examples)
endif(UNIX)

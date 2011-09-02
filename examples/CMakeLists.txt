set (LIBS ncDataReader2 ${NC_LIB} ${MATH_LIB})

set (EXAMPLES GenerateFile GenerateBigFile CacheTests PrintVersion Read1D 
    Read1DChunk Read1DEasyAPI Read2D Read2DEasyApi)

foreach (EX ${EXAMPLES})
    add_executable (${EX} ${EX}.c)
    target_link_libraries (${EX} ${LIBS})
endforeach(EX)

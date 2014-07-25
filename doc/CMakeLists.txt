
add_custom_command (OUTPUT ncDataReader2_Manual.pdf
  COMMAND rst2pdf ncDataReader2_Manual.txt
  DEPENDS ncDataReader2_Manual.txt
)


set (DOCFILES ncDataReader2_Manual.pdf ncDataReader2_Manual.txt 
     CosWin_Approximation_German.pdf CosWin.png Interpolation.png 
     Logo.odg Logo.pdf Logo.png README.csa)

if(UNIX)
    install(FILES ${DOCFILES} DESTINATION share/doc/${NCDR_PACKAGE_NAME}/)
endif(UNIX)

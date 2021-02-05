set(target CSRES)
add_library(${target} STATIC
        SRES
        RandomNumberGenerator
        OptItem
        OptItems
        Error
        )


add_subdirectory(test)


set(target CCSRES)
add_library(${target} SHARED CSRES.cpp CSRES.h)
target_link_libraries(${target} PUBLIC CSRES)


install(
        TARGETS CSRES CCSRES
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)










set(target SRES)
add_library(${target} STATIC
        Optimizer
        EvolutionaryOptimizer
        SRES
        RandomNumberGenerator
        OptItem
        OptItems
        Error
        )
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
include(GoogleTest)
add_subdirectory(test)


set(target SRESC)
add_library(${target} SHARED CSRES.cpp CSRES.h)
target_link_libraries(${target} PUBLIC SRES)


install(
        TARGETS SRES SRESC
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)










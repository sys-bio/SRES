set(target SRES)

add_library(${target}-static STATIC ESES ESSRSort sharefunc)
target_compile_definitions(${target}-static PUBLIC SRES_STATIC_DEFINE)
target_link_libraries(${target}-static PUBLIC linux2win)
target_include_directories(${target}-static PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_library(${target} SHARED ESES ESSRSort sharefunc)
target_compile_definitions(${target}-static PUBLIC SRES_EXPORT)
target_link_libraries(${target} PUBLIC linux2win)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

include(GenerateExportHeader)
generate_export_header(${target})

install(
        TARGETS ${target} ${target}-static
        EXPORT ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_subdirectory(test)



if (BUILD_PYTHON)
    add_subdirectory(python)
endif ()


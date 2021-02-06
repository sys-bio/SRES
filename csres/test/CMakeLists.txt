set(target CSRES-BealeFunction)
add_executable(${target} BealeFunction.cpp)
target_link_libraries(${target} PRIVATE CSRES)

set(target CSRES-BealeFunctionC)
add_executable(${target} BealeFunctionC.cpp)
target_link_libraries(${target} PRIVATE CCSRES)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(${target} PRIVATE ${PARENT_DIR})
add_dependencies(${target} CSRES)


install(
        TARGETS ${target}
        EXPORT ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



set(target OptItemTests)
add_executable(${target} TestOptItem.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main CSRES)


set(target OptItemsTests)
add_executable(${target} TestOptItems.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main CSRES)


set(target CSRESTest)
add_executable(${target} CSRESTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main CSRES)



set(target CCSRESTest)
add_executable(${target} CCSRESTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main CCSRES)















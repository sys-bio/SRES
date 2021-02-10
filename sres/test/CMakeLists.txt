include(GoogleTest)

set(TESTS)
set(target SRES-BealeFunction)
add_executable(${target} BealeFunction.cpp)
target_link_libraries(${target} PRIVATE SRES)
set(TESTS "${TESTS}" "${target}")

set(target SRES-BealeFunctionC)
add_executable(${target} BealeFunctionC.cpp)
target_link_libraries(${target} PRIVATE SRESC)
set(TESTS "${TESTS}" "${target}")

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(${target} PRIVATE ${PARENT_DIR})
add_dependencies(${target} SRESC)
set(TESTS "${TESTS}" "${target}")


install(
        TARGETS ${target}
        EXPORT ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



set(target RNGTests)
add_executable(${target} RNGTests.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main SRES)
gtest_add_tests(TARGET ${target})
set(TESTS "${TESTS}" "${target}")

set(target OptItemTests)
add_executable(${target} OptItemTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main SRES)
gtest_add_tests(TARGET ${target})
set(TESTS "${TESTS}" "${target}")


set(target OptItemsTests)
add_executable(${target} OptItemsTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main SRES)
gtest_add_tests(TARGET ${target})
set(TESTS "${TESTS}" "${target}")


set(target CSRESTest)
add_executable(${target} CSRESTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main SRES)
gtest_add_tests(TARGET ${target})
set(TESTS "${TESTS}" "${target}")



set(target SRESCTest)
add_executable(${target} CCSRESTest.cpp)
target_link_libraries(${target} PRIVATE gtest gtest_main SRESC)
gtest_add_tests(TARGET ${target})
set(TESTS "${TESTS}" "${target}")

install(TARGETS ${TESTS})
#set(target EvolutionaryOptimizerTests)
#add_executable(${target} EvolutionaryOptimizerTests)
#target_link_libraries(${target} PRIVATE gtest gtest_main SRES)
#target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})















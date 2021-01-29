get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

set(test_target EstimateMuAndSigma)
add_executable(${test_target} EstimateMuAndSigma.cpp)
target_link_libraries(${test_target} PRIVATE gtest gtest_main SRES-static)
target_include_directories(${test_target} PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${PARENT_DIR}>"
        )


set(test_target EstimateMuAndSigmaModified)
add_executable(${test_target} EstimateMuAndSigmaModified.cpp)
target_link_libraries(${test_target} PRIVATE gtest gtest_main SRES-static)
target_include_directories(${test_target} PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${PARENT_DIR}>"
        )


set(test_target SresTest)
add_executable(${test_target} test.cpp)
target_link_libraries(${test_target} PRIVATE gtest gtest_main SRES-static)
target_include_directories(${test_target} PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${PARENT_DIR}>"
        )

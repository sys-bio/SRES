cmake_minimum_required(VERSION 3.17)

# Set version.
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_MICRO 0)

set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO})

project(SRES VERSION ${PROJECT_VERSION})

set(SOVERSION ${VERSION_MAJOR})

# standard install locations
include(GNUInstallDirs)


############################################################################
#       Settings
#

# pic - needed on linux ignored on other platform
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# put all runtime targets (exe and dll) into bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# put libraries into lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

# archives
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

# export all on windows. Ignored on other.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

set(CMAKE_VERBOSE_MAKEFILE ON)

#set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
#
#find_package(Sanitizers)
#
#if (${Sanitizers_FOUND})
#    message(STATUS "Found Sanitizers")
#else ()
#    message(STATUS "Sanitizers not found")
#endif ()

# setup some paths that will be used for the INSTALL_RPATH paths on systems that
# support it. Adapt the "ORIGIN" to the APPLE use case
if (APPLE)
    set(base @loader_path)
else ()
    set(base $ORIGIN)
endif ()

file(RELATIVE_PATH INSTALL_RPATH_DIRECTORIES
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
        ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
        )
# make all shared libraries have the same directory, the bin and the lib dirs in RPATH
set(CMAKE_INSTALL_RPATH ${base} ${base}/${INSTALL_RPATH_DIRECTORIES})


# Wierd bug on windows:
# the __cplusplus macro always defaults to 199711L regardless of which version of
# c++ you are using. This is fixed: https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
# but requires that we define the Zc:__cplusplus switch for the duration of MSVC version 19 compilers.
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif ()

############################################################################
#       Options
#

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD}")
if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD} -lstdc++fs")
endif ()

option(BUILD_PYTHON "build the python API (for SRES only)" ON)

set(README_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Readme.md")
set(REQUIREMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt")

# Configure the sphinx docs conf.py
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/source/conf.py.in
        ${CMAKE_CURRENT_SOURCE_DIR}/docs/source/conf.py
        @ONLY
)

############################################################################
#       include src
#


add_subdirectory(external)

add_subdirectory(sres)


option(COPY_BINARY_TO_PYTHON_DIR " when true, find the SRES binary and copy to sres python folder" off)

if (COPY_BINARY_TO_PYTHON_DIR)
    install(TARGETS SRESC DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/sres")
endif ()

find_package(Python)

# configure the python setup.py
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in"
        "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
)

install(
        FILES
        "${REQUIREMENTS_FILE}"
        "${README_FILE}"
        "${CMAKE_CURRENT_BINARY_DIR}/setup.py"
        DESTINATION .
)

install(
        FILES
        sres/__init__.py
        sres/sres.py
        $<TARGET_FILE:SRESC>
        DESTINATION
        site-packages/sres
)

install(
        FILES
        sres/test/__init__.py
        sres/test/BealeFunctionSRES.py
        sres/test/LargeRRModel.py
        sres/test/RoadrunnerProblem.py
        DESTINATION site-packages/sres/test
)













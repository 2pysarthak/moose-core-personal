set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0004 OLD)
    if(POLICY CMP0050)
        cmake_policy(SET CMP0050 OLD)
    endif(POLICY CMP0050)
endif(COMMAND cmake_policy)

project(MOOSE)

include(CheckCXXCompiler.cmake)
include(CheckIncludeFileCXX)
include(FindPkgConfig)

# Set moose version
# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git describe --tags --long
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION_OUTPUT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions( -DMOOSE_VERSION="${GIT_VERSION_OUTPUT}")


# This snippet is from LLVM project.
# Sanity check our source directory to make sure that we are not trying to
# generate an in-tree build (unless on MSVC_IDE, where it is ok), and to make
# sure that we don't have any stray generated files lying around in the tree
# (which would end up getting picked up by header search, instead of the correct
# versions).

message(STATUS "CMAKE_INSTALL_PREFIX : ${CMAKE_INSTALL_PREFIX}")
if( CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
    message(FATAL_ERROR 
        "======================================================================\n"
        "In-source builds are not allowed. Remove CMakeCache.txt and CMakeFiles\n"
        "directory and do something like this inside this directory \n"
        "    $ mkdir _build_dir \n"
        "    $ cd _build_dir  \n"
        "    $ cmake ..  \n"
        "===================================================================== \n"
        )
endif()

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
    )

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )

# This is for testing purpose.
link_directories(${CMAKE_CURRENT_BINARY_DIR})

################################# OS Specific ##################################
message(STATUS "Operating system: ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    CMAKE_POLICY(SET CMP0042 NEW)
    set(MACOSX TRUE)
else()
    set(MACOSX FALSE)
endif()

################################ CMAKE OPTIONS ##################################

option(VERBOSITY "Set MOOSE verbosity level (deprecated)" 0)
## Unit testing and debug mode.
option(DEBUG "Build with debug support" OFF)
option(GPROF "Build for profiling using gprof" OFF)
option(ENABLE_UNIT_TESTS "Enable unit tests (DEBUG should also be ON)" OFF)
option(WITH_PYTHON "Build native python extension" ON)
option(WITH_CYTHON "Build python extension using Cython. (experimental)" OFF)
option(WITH_MPI  "Enable Openmpi support" OFF)
option(WITH_BOOST "Use boost library instead of GSL" OFF)
option(WITH_GSL  "Use gsl-library. Alternative is WITH_BOOST" ON)

# If SBML is available, it will automaticall enable the support. If ON, then
# libSBML must be present.
option(WITH_SBML  "Enable SBML support. Automatically detected." OFF)

# If GSL_STATIC_HOME is set, we use it to search for static gsl libs.
option(GSL_STATIC_HOME 
    "Path prefix where static gsl library can be found e.g. /opt/sw/gsl116 " 
    OFF
    )
option(SBML_STATIC_HOME 
    "Path prefix where static sbml library can be found e.g. /opt/sbml/"
    OFF
    )
option(HDF5_STATIC_HOME
    "Path prefix where static hdf5 library can be found e.g /opt/sw/hdf5 "
    OFF
    )

################################# CMKAE MACROS #################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

############################ BUILD CONFIGURATION #################################

# VERBOSITY OF OUTPUT
if(VERBOSITY)
    message(STATUS "Verbosity of output is ${VERBOSITY}. This is not implemented
    yet!")
    add_definitions(-DVERBOSITY=${VERBOSITY})
else()
    message(STATUS "Moose will be quiet")
    add_definitions(-DQUIET_MODE)
endif()

# Default macros
add_definitions(-DUSE_GENESIS_PARSER)

if(DEBUG)
    message(STATUS "Building for Debug/Unit testing")
    add_definitions(-DDO_UNIT_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
elseif(ENABLE_UNIT_TESTS)
    MESSAGE(STATUS "Enabled Unit tests")
    add_definitions(-DDO_UNIT_TESTS)
    set(CMAKE_BUILD_TYPE Debug)
else()
    message(STATUS "Building for Release/No unit tests.")
    set(CMAKE_BUILD_TYPE Release)
    add_definitions(-UDO_UNIT_TESTS -O3)
endif()

if(GPROF AND DEBUG)
    message(STATUS "Compiling with profiling with gprof")
    add_definitions(-pg)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
endif()

IF(NOTIFY_PROGRESS)
    IF(COMPILER_SUPPORTS_CXX11 OR COMPILER_SUPPORTS_CXX0X)
        MESSAGE(STATUS "MOOSE will occasionally write to __moose__progress__ file "
            " status of simulation (done time/total time)"
            )
    ELSE()
        MESSAGE(FATAL_ERROR "No c++11 or c++0x support in compiler."
            "You can not use -DNOTIFY_PROGRESS=ON without it"
            )
    ENDIF()
ENDIF()
################################### TARGETS ####################################

add_library(libmoose SHARED basecode/main.cpp)
add_executable(moose.bin basecode/main.cpp)

################################### SETUP BUILD ################################

# default include paths.
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

if(WITH_BOOST)
    set(WITH_GSL OFF)
endif(WITH_BOOST)

# If this variable is used while packaging; we build this project using another
# cmake script and pass the location of SBML static library using this variable.
# Otherwise, this is useless. FIXME: This is not a clean solution.
if(SBML_STATIC_HOME)
    include_directories(${SBML_STATIC_HOME}/include)
    find_library(LIBSBML_LIBRARIES
        NAMES libsbml-static.a libsbml.a
        PATHS "${SBML_STATIC_HOME}/lib" "${SBML_STATIC_HOME}/lib64"
        NO_DEFAULT_PATH
        )
    message(STATUS "- Using static version of LIBSBML_LIBRARIES: ${LIBSBML_LIBRARIES}")
    if(LIBSBML_LIBRARIES)
        message(STATUS "- Successfully located SBML static library: ${LIBSBML_LIBRARIES}")
    ELSE()
        message(FATAL_ERROR 
            "Can't find static libsbml libraries at path ${SBML_STATIC_HOME}"
            )
    endif()
    set(LIBSBML_FOUND ON)
else(SBML_STATIC_HOME)
    pkg_check_modules(LIBSBML libsbml)
    if(NOT LIBSBML_FOUND)
        message(STATUS "pkg-config could not find sbml. Fallback to default")
        find_package(LIBSBML)
    endif()
endif(SBML_STATIC_HOME)

if(LIBSBML_FOUND)
    message(STATUS "LIBSBML found ${LIBSBML_LIBRARIES}")
    include_directories(${LIBSBML_INCLUDE_DIRS})
    pkg_check_modules(LibXML2 libxml-2.0)
    if (!LibXML2_FOUND)
        find_package(LibXML2 REQUIRED)
    endif()
    include_directories(${LibXML2_INCLUDE_DIRS})
else()
    message(
        "======================================================================\n"
        "libsbml NOT found. \n\n"
        "If you want to compile with SBML support, download and install \n"
        "libsbml-5.9.0 from: \n"
        "http://sourceforge.net/projects/sbml/files/libsbml/5.9.0/stable/ and\n"
        "rerun cmake.\n\n"
        "If you don't want SBML support then continue with `make`.\n\n"
        "If you install libsbml to non-standard place, let the cmake know by\n"
        "exporting environment variable SBML_DIR to the location.\n"
        "=====================================================================\n"
        )
    SET(WITH_SBML OFF)
endif()

include_directories(msg basecode)

set_target_properties(libmoose PROPERTIES COMPILE_DEFINITIONS  "MOOSE_LIB")
set_target_properties(libmoose PROPERTIES PREFIX "")

if(WITH_GSL)
    if(GSL_STATIC_HOME)
        SET(GSL_INCLUDE_DIRS "${GSL_STATIC_HOME}/include")
        find_library(GSL_LIBRARY 
            NAME libgsl.a 
            PATHS ${GSL_STATIC_HOME}/lib ${GSL_STATIC_HOME}/lib64
            NO_DEFAULT_PATH
            )
        find_library(GSLCBLAS_LIBRARY
            NAME libgslcblas.a 
            PATHS ${GSL_STATIC_HOME}/lib ${GSL_STATIC_HOME}/lib64 
            NO_DEFAULT_PATH
            )
        SET(GSL_LIBRARIES ${GSL_LIBRARY} ${GSLCBLAS_LIBRARY})
        message(STATUS "STATIC GSL_LIBRARIES ${GSL_LIBRARIES}")
        set(GSL_FOUND TRUE)
    ELSE()
        find_package(GSL 1.16)
        if(NOT GSL_FOUND)
            message(FATAL_ERROR 
                "=====================================================================\n"
                " FATAL gsl(>1.16) not found.\n\n"
                " MOOSE requires Gnu Scientific Library (GSL) 1.16 or higher. \n"
                " Please install the `dev` or `devel` package e.g. \n"
                "     $ sudo apt-get install libgsl0-dev \n"
                "     $ sudo yum install libgsl-devel \n"
                "     $ brew install gsl \n\n"
                " Or build and install gsl from source code \n"
                "     https://www.gnu.org/software/gsl/ \n"
                " After installing gsl, rerun cmake.\n\n"
                " If you install gsl in non-standard place, set the GSL_HOME environment \n"
                " variable. CMAKE use this to search for required files. \n"
                "====================================================================\n"
                )
        endif(NOT GSL_FOUND)
    endif()
    add_definitions(-DUSE_GSL)
    # GSL is also used in RNG (whenever applicable), therefore include paths are
    # top level.
    include_directories( ${GSL_INCLUDE_DIRS} )
elseif(WITH_BOOST)
<<<<<<< HEAD
    find_package(Boost 1.44 COMPONENTS filesystem REQUIRED)
=======
    find_package(Boost 1.44 COMPONENTS filesystem random REQUIRED)
>>>>>>> bhallalab/master
    find_package( LAPACK REQUIRED )
    add_definitions( -DUSE_BOOST -UUSE_GSL )
    include_directories( ${Boost_INCLUDE_DIRS} )
    check_include_file_cxx(
        ${Boost_INCLUDE_DIRS}/boost/random/random_device.hpp
        BOOST_RANDOM_DEVICE_EXISTS
        )
    if(BOOST_RANDOM_DEVICE_EXISTS)
        add_definitions(-DBOOST_RANDOM_DEVICE_EXISTS)
    endif(BOOST_RANDOM_DEVICE_EXISTS)
endif()

## Setup hdf5
if(HDF5_STATIC_HOME)
    SET(HDF5_INCLUDE_DIRS "${HDF5_STATIC_HOME}/include")
    find_library(HDF5_LIBRARY 
        NAME libhdf5-static.a
        PATHS ${HDF5_STATIC_HOME}/lib ${HDF5_STATIC_HOME}/lib64
        NO_DEFAULT_PATH
        )
    find_library(HDF5_HL_LIBRARY
        NAME libhdf5_hl-static.a
        PATHS ${HDF5_STATIC_HOME}/lib ${HDF5_STATIC_HOME}/lib64
        NO_DEFAULT_PATH
        )
    SET(HDF5_LIBRARIES ${HDF5_LIBRARY} ${HDF5_HL_LIBRARY})
    SET(HDF5_FOUND TRUE)
ELSE(HDF5_STATIC_HOME)
    pkg_check_modules(HDF5 hdf5)
    if(NOT HDF5_FOUND)
        message(STATUS "pkg-config could not find hdf5. fallback to default")
        find_package(HDF5 COMPONENTS CXX HL)
     endif()
endif(HDF5_STATIC_HOME)

if(NOT HDF5_FOUND)
    message(
        "==================================================================\n"
        " HDF5 not found. Disabling support. Required for nsdf. \n\n"
        " If you need hdf5 support, please install hdf5-dev or hdf5-devel\n"
        " package or equivalent.\n\n"
        "     $ sudo apt-get install libhdf5-dev \n"
        "     $ sudo yum install libhdf5-devel \n"
        "     $ brew install hdf5 \n\n"
        " Otherwise, continue with 'make' and 'make install' \n"
        " If you install hdf5 to non-standard path, export environment \n"
        " variable HDF5_ROOT to the location. Rerun cmake \n"
        "================================================================ \n"
        )
ELSEIF(HDF5_FOUND AND (NOT HDF5_STATIC_HOME))
    if(HDF5_HL_LIBRARIES)
        set(HDF5_LIBRARIES ${HDF5_HL_LIBRARIES})
    else()
        set(HDF5_LIBRARIES "-lhdf5 -lhdf5_hl")
    endif()
    include_directories(${HDF5_INCLUDE_DIRS})
endif(NOT HDF5_FOUND)

if(HDF5_FOUND)
    add_definitions( -DUSE_HDF5 )
    include_directories( ${HDF5_INCLUDE_DIRS} )
endif( HDF5_FOUND )

# This is a fix for new HDF5 package on Debian/Ubuntu which installs hdf5
# headers in non-standard path. issue #80.
if(HDF5_LIBRARY_DIRS)
    set_target_properties( libmoose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
endif()

find_package(Termcap)
find_package(Readline)

if(READLINE_FOUND AND TERMCAP_FOUND)
    add_definitions(-DUSE_READLINE)
    include_directories(${Readline_INCLUDE_DIR})
endif()

# Openmpi
if(WITH_MPI)
    find_package(MPI REQUIRED)
    if(MPI_CXX_FOUND)
        message(STATUS "Using MPI from ${MPI_CXX_INCLUDE_PATH}")
        include_directories(${MPI_CXX_INCLUDE_PATH})
        set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
        add_definitions(-DUSE_MPI)
        SET(CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
        SET(CMAKE_C_COMPILER ${MPI_C_COMPILER})
    else()
        message(STATUS "Cound not find MPI")
        add_definitions(-UUSE_MPI)
    endif()
endif(WITH_MPI)

# Add subdirectroeis
add_subdirectory(basecode)
add_subdirectory(msg)
add_subdirectory(shell)
add_subdirectory(randnum)
add_subdirectory(scheduling)
add_subdirectory(biophysics)
add_subdirectory(builtins)
add_subdirectory(utility)
add_subdirectory(mesh)
add_subdirectory(sbml)
add_subdirectory(mpi)
add_subdirectory(signeur)
add_subdirectory(ksolve)
add_subdirectory(hsolve)
add_subdirectory(diffusion)
add_subdirectory(device)
add_subdirectory(benchmarks)
add_subdirectory(kinetics)
add_subdirectory(synapse)
add_subdirectory(intfire)

# These are always shared libraries.
set(SYSTEM_SHARED_LIBS ${LibXML2_LIBRARIES})
if(WITH_BOOST)
    list(APPEND SYSTEM_SHARED_LIBS ${LAPACK_LIBRARIES})
    list(APPEND SYSTEM_SHARED_LIBS ${Boost_LIBRARIES})
endif(WITH_BOOST)

# These libraries could be static of dynamic.
# When building packages on OBS or launchpad, we build static library of a
# dependencies if a correct version is not found. 
# Here we make sure that the correct static version is linked; if a correct
# shared version is found then link the shared version in SYSTEM_SHARED_LIBS.

# If CMAKE_STATIC_LIBRARIES are mentiond on command line, use it as list.
set(STATIC_LIBRARIES  "")

if(HDF5_STATIC_HOME)
    list(APPEND STATIC_LIBRARIES ${HDF5_LIBRARIES})
elseif(HDF5_FOUND)
    list(APPEND SYSTEM_SHARED_LIBS ${HDF5_LIBRARIES})
endif()

LIST(APPEND STATIC_LIBRARIES moose_sbml)
if(SBML_STATIC_HOME)
    list(APPEND STATIC_LIBRARIES ${LIBSBML_LIBRARIES})
elseif(LIBSBML_FOUND)
    list(APPEND SYSTEM_SHARED_LIBS ${LIBSBML_LIBRARIES})
endif()

if(WITH_GSL)
    if(GSL_STATIC_HOME)
        message(STATUS "Using STATIC gsl libraries: ${GSL_LIBRARIES}")
        list(APPEND STATIC_LIBRARIES ${GSL_LIBRARIES})
    else()
        message(STATUS "Using system gsl : ${GSL_LIBRARIES}")
        list(APPEND SYSTEM_SHARED_LIBS ${GSL_LIBRARIES})
    endif()
endif()

# Deprecated.
#if(READLINE_FOUND AND TERMCAP_FOUND)
#    list(APPEND SYSTEM_SHARED_LIBS ${Readline_LIBRARY} ${TERMCAP_LIBRARY})
#endif()

if(WITH_MPI)
    if(MPI_CXX_FOUND)
        list(APPEND SYSTEM_SHARED_LIBS ${MPI_CXX_LIBRARIES})
    endif()
endif(WITH_MPI)

pkg_check_modules(MUPARSER muparser)
if(MUPARSER_FOUND)
    message(STATUS "Using system muparser ${MUPARSER_VERSION}")
    list(APPEND SYSTEM_SHARED_LIBS ${MUPARSER_LIBRARIES})
else()
    message(STATUS "Using private muparser")
    add_subdirectory(external/muparser)
    list(APPEND MOOSE_LIBRARIES muparser)
endif()

#pkg_check_modules(TINYXML tinyxml)
#if(TINYXML_FOUND)
    #message(STATUS "Using system tinyxml ${TINYXML_VERSION}")
    #list(APPEND SYSTEM_SHARED_LIBS ${TINYXML_LIBRARIES})
#else()
    #message(STATUS "Using private tinyxml")
    #add_subdirectory(external/tinyxml)
    #list(APPEND MOOSE_LIBRARIES tinyxml)
#endif()

###################################### LINKING #################################
list(APPEND MOOSE_LIBRARIES
    moose_builtins
    msg
    benchmarks
    shell
    randnum
    scheduling
    moose_mpi
    biophysics 
    utility 
    kinetics 
    synapse
    intfire
    hsolve 
    mesh
    signeur
    diffusion 
    ksolve
    device
    basecode
    )


# MAC linker does not understand many of gnu-ld options.
message( STATUS "Shared libs: ${SYSTEM_SHARED_LIBS}")
if(MACOSX)
    target_link_libraries(libmoose
        "-Wl,-all_load"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )

    target_link_libraries(libmoose
        ${SYSTEM_SHARED_LIBS}
        )
ELSE(MACOSX)
    target_link_libraries(libmoose 
        "-Wl,--whole-archive"
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${SYSTEM_SHARED_LIBS}
        )
endif(MACOSX)

add_dependencies(moose.bin libmoose)

# This target is only for testing purpose. It links to libmoose.
if(CMAKE_VERSION VERSION_LESS "2.8.0")
    message(STATUS "Using a fix for cmake ( < 2.8)")
    target_link_libraries(moose.bin PUBLIC moose)
ELSE()
    target_link_libraries(moose.bin LINK_PUBLIC moose)
ENDIF()

######################### BUILD PYMOOSE ########################################
# Root of all python module.
if(WITH_PYTHON)
<<<<<<< HEAD
    message(STATUS "Building native python extension of MOOSE")
    set(PYMOOSE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python/moose")

    find_package(NumPy REQUIRED)
    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    message(STATUS "Python include dir: ${PYTHON_INCLUDE_DIRS}")
    message(STATUS "Python libraries: ${PYTHON_LIBRARIES}")
    message(STATUS "Python executable: ${PYTHON_EXECUTABLE}")

    EXEC_PROGRAM(${PYTHON_EXECUTABLE}
        ARGS "-c 'try: import importlib.machinery; print(importlib.machinery.EXTENSION_SUFFIXES[0])
except Exception: pass'"
        OUTPUT_VARIABLE PYTHON_SO_EXTENSION
        )
    message( STATUS "Python so extension ${PYTHON_SO_EXTENSION}" )

    find_package(NumPy REQUIRED)
    include_directories(${NUMPY_INCLUDE_DIRS})
    add_definitions(-DUSE_NUMPY)

    add_definitions(-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION)

    execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --includes
        OUTPUT_VARIABLE PYTHON_INCLUDE_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process( COMMAND ${PYTHON_EXECUTABLE}-config --libs
        OUTPUT_VARIABLE PYTHON_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    message( STATUS "Python include flags: ${PYTHON_INCLUDE_FLAGS}" )
    add_subdirectory(pymoose)

    set_target_properties(_moose PROPERTIES 
        COMPILE_DEFINITIONS "PYMOOSE"
        COMPILE_FLAGS "${PYTHON_INCLUDE_FLAGS}"
        LIBRARY_OUTPUT_DIRECTORY ${PYMOOSE_OUTPUT_DIRECTORY}
        PREFIX ""
        SUFFIX ".so"
        )

    #if(NOT(PYTHON_SO_EXTENSION STREQUAL ""))
    #    set_target_properties(_moose PROPERTIES
    #    SUFFIX ${PYTHON_SO_EXTENSION})
    #endif()

    # see issue #80
    if(HDF5_LIBRARY_DIRS)
        set_target_properties( _moose PROPERTIES LINK_FLAGS "-L${HDF5_LIBRARY_DIRS}" )
    endif()


if(MACOSX)
    set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup")
    message(STATUS "ADDING some linker flags ${CMAKE_EXE_LINKER_FLAGS}")
endif(MACOSX)

if(MACOSX)
    target_link_libraries( _moose 
        "-Wl,-all_load"
        moosepython  
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        )
    target_link_libraries(_moose
        ${SYSTEM_SHARED_LIBS}
        )
ELSE(MACOSX)
    target_link_libraries(_moose 
        "-Wl,--whole-archive"
        moosepython  
        ${MOOSE_LIBRARIES}
        ${STATIC_LIBRARIES}
        "-Wl,--no-whole-archive"
        ${PYTHON_LIBRARIES}
        ${SYSTEM_SHARED_LIBS}
        )
endif(MACOSX)

# Make this target dependant on all python files in python folder.
add_custom_target(copy_python_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/python
    ${CMAKE_CURRENT_BINARY_DIR}/python
    COMMENT "Copying python files to build directory"
    VERBATIM
    )
add_dependencies(_moose copy_python_files)

if(WITH_DOC)
    # Also generate the documents
    FIND_PROGRAM(Sphinx REQUIRED)
    add_custom_command(TARGET _moose
        POST_BUILD
        COMMAND make html
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Docs/user/py
        COMMENT "Building user doc using python-sphinx"
        VERBATIM
        )
endif(WITH_DOC)
=======
    add_subdirectory( pymoose )
>>>>>>> bhallalab/master
endif(WITH_PYTHON)

######################### INSTALL ##############################################

install(TARGETS moose.bin
    DESTINATION bin
    )

install(TARGETS libmoose
    DESTINATION lib
    )

install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/moose
    DESTINATION bin
    )

if(WITH_PYTHON)
    install(CODE 
        "
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} setup.py install --prefix=${CMAKE_INSTALL_PREFIX}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python
            )
        "
        )

endif(WITH_PYTHON)

# Print message to start build process
if(${CMAKE_BUILD_TOOL} MATCHES "make")
    message(
        "=======================================\n"
        "If cmake did not report any error, run \n"
	" 'make' to build  MOOSE \n"
        "=======================================\n"
        )
endif()


############################ CTEST ######################################

include(${CMAKE_CURRENT_SOURCE_DIR}/MooseTests.cmake)

############################ CPACK ######################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_moose_cpack.cmake)

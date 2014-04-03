cmake_minimum_required (VERSION 2.8)

project (Guamps)
set(PROJECT_VERSION, "1.0.0")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# use C99 standard
set(CMAKE_C_FLAGS "-std=c99")

# debugging
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")

# find external dependencies
# find_package(GROMACS COMPONENTS libgmx REQUIRED)
set(GROMACS_LIBRARY "GROMACS_LIBRARY-NOT_FOUND" CACHE FILEPATH "path/to/libgmx.a")
set(GROMACS_INCLUDE_DIR "GROMACS_INCLUDE_DIR-NOT_FOUND" CACHE FILEPATH "path/to/include")
# SET(GROMACS_LIBRARIES "/afs/crc.nd.edu/user/c/cabdulwa/Public/apps/gromacs/x86_64/4.5.3/lib/libgmx.a")
# set(GROMACS_INCLUDE_DIR "/afs/crc.nd.edu/user/c/cabdulwa/Public/apps/gromacs/x86_64/4.5.3/include")


include_directories(${GROMACS_INCLUDE_DIR})
set(LIBS ${LIBS} ${GROMACS_LIBRARIES})

# make sure the compiler can find header files for guamps
include_directories(include)

set(LinkType STATIC)

# # static linking works on OS X but not Linux at the moment
# if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#     set(LinkType STATIC)
# else()
#     set(LinkType SHARED)
# endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# recurese in the the subdirectories
add_subdirectory(include)
add_subdirectory(src)

# installation
set(BIN_INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}bin)
set(LIB_INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}lib)
set(INCL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}include)

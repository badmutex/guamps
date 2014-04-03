cmake_minimum_required (VERSION 2.8)

project (Guamps)
set(PROJECT_VERSION, "1.0.0")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/bin)

# find external dependencies
# find_package(GROMACS COMPONENTS libgmx REQUIRED)
set(GROMACS_LIBRARY "GROMACS_LIBRARY-NOT_FOUND" CACHE FILEPATH "path/to/libgmx.a")
set(GROMACS_INCLUDE_DIR "GROMACS_INCLUDE_DIR-NOT_FOUND" CACHE FILEPATH "path/to/include")

### statically link to libraries.
### The mechanism is hacked together
### see github.com/badi/guamps.git/issues/37
### and github.com/badi/guamps.git/pulls/38
set(LinkType STATIC)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# -static is not supported on darwin
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_EXE_LINKER_FLAGS)
  set(LinkLibs)
else()
  set(CMAKE_EXE_LINKER_FLAGS "-static")
  set(LinkLibs m pthread dl)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)
message(INFO " linker flags:  ${CMAKE_EXE_LINKER_FLAGS}")
message(INFO " linker libs:   ${LinkLibs}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# use C99 standard
set(CMAKE_C_FLAGS "-std=c99")

# debugging
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")

include_directories(${GROMACS_INCLUDE_DIR})
set(LIBS ${LIBS} ${GROMACS_LIBRARIES})

# make sure the compiler can find header files for guamps
include_directories(include)

# recurese in the the subdirectories
add_subdirectory(include)
add_subdirectory(src)

# installation
set(BIN_INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}bin)
set(LIB_INSTALL_DIR  ${CMAKE_INSTALL_PREFIX}lib)
set(INCL_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}include)

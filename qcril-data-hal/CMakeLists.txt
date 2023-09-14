cmake_minimum_required(VERSION 2.8.9)

project (qcril-data-hal)

include(GNUInstallDirs)
include(FindPkgConfig)

pkg_check_modules(GLIB REQUIRED glib-2.0)
add_compile_options(${GLIB_CFLAGS} -include glib.h)
add_definitions(-Dstrlcpy=g_strlcpy)
add_definitions(-Dstrlcat=g_strlcat)

# Supported configurations
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
   )

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Available build configurations." FORCE)

add_definitions(-DRIL_FOR_MDM_LE)
add_definitions(-DQCRIL_DATA_MODULE_DEV)
add_definitions(-DFEATURE_QCRIL_USE_NETCTRL)
add_definitions(-DDRIL_REQUEST_SET_INITIAL_ATTACH_APN)
add_definitions(-DFEATURE_DATA_EMBMS)
add_definitions(-DFEATURE_DATA_LQE)
add_definitions(-DFEATURE_DATA_LOG_QXDM)
add_definitions(-DRIL_FOR_LOW_RAM)
if (RIL_SHARED_LIB)
add_definitions(-DRIL_FOR_DYNAMIC_LOADING)
endif(RIL_SHARED_LIB)

macro(SYSR_INCLUDE_DIR subdir)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I =/usr/include/${subdir}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I =/usr/include/${subdir}")
endmacro()

add_subdirectory( datamodule/module )

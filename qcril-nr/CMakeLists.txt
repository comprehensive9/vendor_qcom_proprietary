cmake_minimum_required(VERSION 2.8.9)

project (qcril-nr)

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
#add_definitions(-DRIL_FOR_MDM_DISABLE_DATA)
add_definitions(-DRIL_SHLIB)

#This is temporarily to compile qtimutex
add_definitions(-DQTIMUTEX_HOST_BUILD)

if (RIL_FOR_EXTERNAL_AP)
    add_definitions(-DRIL_FOR_EXTERNAL_AP)
    # Feature flag used by some data header files
    add_definitions(-DFEATURE_EXTERNAL_AP)
else()
    add_definitions(-DQC_DIAG_EXISTS)
endif (RIL_FOR_EXTERNAL_AP)

macro(SYSR_INCLUDE_DIR subdir)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I =/usr/include/${subdir}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I =/usr/include/${subdir}")
endmacro()

add_subdirectory( qcril-common/qtimutex )
add_subdirectory( qcril-common/framework )
add_subdirectory( qcril-common/logger)
add_subdirectory( qcril-common/qtibus )
add_subdirectory( qcril-common/interfaces )
add_subdirectory( qcril-common/internal-msgs )
add_subdirectory( qcril-common/qtiwakelock )
add_subdirectory( modules/qmi )
add_subdirectory( modules/uim )
add_subdirectory( modules/sms )
add_subdirectory( cmas )
add_subdirectory( modules/pbm )
add_subdirectory( modules/cellinfo )
add_subdirectory( modules/voice )
add_subdirectory( modules/ims )
add_subdirectory( modules/nas )
add_subdirectory( modules/dms )
add_subdirectory( modules/android )
add_subdirectory( mocks/utils )
add_subdirectory( mocks/cutils )
add_subdirectory( qcril-common/utilities )
add_subdirectory( translators/android )
add_subdirectory( qcrild/librilutils )
add_subdirectory( qcril_qmi )
add_subdirectory( qcril-common/marshalling )
add_subdirectory( qcrild/libril )
add_subdirectory( qcrild/qcrild )
add_subdirectory( qcril_client )
add_subdirectory( qcril-common/PeripheralMgr )
add_subdirectory( qcril-common/qcril_database )
add_subdirectory( test/cli )

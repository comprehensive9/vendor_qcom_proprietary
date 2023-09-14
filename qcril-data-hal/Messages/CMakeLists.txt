cmake_minimum_required(VERSION 2.8.9)

project (qcril-data-hal-messages)

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
add_definitions(-DRIL_FOR_LOW_RAM)

macro(SYSR_INCLUDE_DIR subdir)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I =/usr/include/${subdir}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I =/usr/include/${subdir}")
endmacro()

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)
SYSR_INCLUDE_DIR(event)
SYSR_INCLUDE_DIR(sync)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(dsutils)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(data)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-macro-redefined -fPIC")

####qcrilDataInterfaces

# Set target library name as libqcrilDataInterfaces
set(TARGET_LIBQCRILDATAINTERFACES libqcrilDataInterfaces)

# Library source files
file(GLOB libqcrilDataInterfaces_src "src/Interfaces/request/*.cpp"
    "src/Interfaces/UnSolMessages/*.cpp"
    )

# Build qcrilDataInterfaces static library
add_library (${TARGET_LIBQCRILDATAINTERFACES} STATIC
    "${libqcrilDataInterfaces_src}"
)


target_link_libraries(${TARGET_LIBQCRILDATAINTERFACES}
    log
    cutils
    dl
    base
    utils
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBQCRILDATAINTERFACES} PROPERTIES
                      LINK_FLAGS "-Wl,-z,defs"
                      VERSION 1.0.0
                      SOVERSION 1)



install ( TARGETS ${TARGET_LIBQCRILDATAINTERFACES}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

####qcrilDataInternal

# Set target library name as libqcrilDataInternal
set(TARGET_LIBQCRILDATAINTERN libqcrilDataInternal)

# Library source files
file(GLOB libqcrilDataintern_src "src/Internal/DataInternal/request/*.cpp"
    "src/Internal/DataInternal/UnSolMessages/*.cpp" "src/Internal/DataInternal/event/*.cpp"
    "src/Internal/DataInternal/sync/*.cpp" "src/Internal/InterModule/request/*.cpp"
    "src/Internal/InterModule/UnSolMessages/*.cpp" "src/Internal/InterModule/sync/*.cpp"
    )

# Build libqcrilDataModule static library
add_library (${TARGET_LIBQCRILDATAINTERN} STATIC
    "${libqcrilDataintern_src}"
)

target_link_libraries(${TARGET_LIBQCRILDATAINTERN}
    log
    cutils
    dl
    base
    utils
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBQCRILDATAINTERN} PROPERTIES
                      LINK_FLAGS "-Wl,-z,defs"
                      VERSION 1.0.0
                      SOVERSION 1)


install ( TARGETS ${TARGET_LIBQCRILDATAINTERN}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

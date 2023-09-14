include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(libcutils)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqtibus
set(TARGET_LIBQCRILINTERFACES qcrilInterfaces)

# Library header files
file(GLOB  qcrilInterfaces_headers "include/")

# Library source files
file(GLOB qcrilInterfaces_src "sms/src/*.cpp")

# Build libqtibus shared library
add_library (${TARGET_LIBQCRILINTERFACES} SHARED
    "${qcrilInterfaces_src}"
)

set_target_properties(${TARGET_LIBQCRILINTERFACES} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilInterfaces_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/interfaces
)

#target_include_directories(${TARGET_LIBQCRILINTERFACES} PUBLIC ${qcrilInterfaces_headers})

target_link_libraries(${TARGET_LIBQCRILINTERFACES} qtimutex qcrilFramework qcrilNrLogger)

install ( TARGETS ${TARGET_LIBQCRILINTERFACES}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


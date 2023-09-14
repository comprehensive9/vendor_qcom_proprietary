include_directories(BEFORE
#    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(libcutils)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqtibus
set(TARGET_LIBQTIBUS qtibus)

# Library source files
file(GLOB libqtibus_src "src/*.cpp")

# Build libqtibus shared library
add_library (${TARGET_LIBQTIBUS} SHARED
    "${libqtibus_src}"
)

set_target_properties(${TARGET_LIBQTIBUS} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qtibus_headers
    ${CMAKE_SOURCE_DIR}/include/qtibus
)

#target_include_directories(${TARGET_LIBQTIBUS} PUBLIC ${qtibus_headers})

target_link_libraries(${TARGET_LIBQTIBUS} pthread qtimutex qcrilFramework qcrilNrLogger)

install ( TARGETS ${TARGET_LIBQTIBUS}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


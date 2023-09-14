include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

SYSR_INCLUDE_DIR(qcril-nr)

# Set target library name as libqcrilMarshal
set(TARGET_LIBQCRIL_MARSHAL qcrilMarshal)

# Library source files
file(GLOB libqcrilMarshal_src "src/*.cpp")

# Build libqcrilMarshal shared library
add_library (${TARGET_LIBQCRIL_MARSHAL} SHARED
    "${libqcrilMarshal_src}"
)

set_target_properties(${TARGET_LIBQCRIL_MARSHAL} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilMarshal_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/inc
)

#target_include_directories(${TARGET_LIBQCRIL_MARSHAL} PUBLIC ${qcrilMarshal_headers})

target_link_libraries(${TARGET_LIBQCRIL_MARSHAL} qtimutex pthread qcrilNrLogger)

install ( TARGETS ${TARGET_LIBQCRIL_MARSHAL}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

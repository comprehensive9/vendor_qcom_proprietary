include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(diag)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqcrilNrLogger
set(TARGET_LIBQCRIL_LOGGER qcrilNrLogger)

# Library source files
file(GLOB qcrilNrLogger_src "DiagLogger.cpp" "Logger.cpp")

# Build libqtimutex shared library
add_library (${TARGET_LIBQCRIL_LOGGER} SHARED
    "${qcrilNrLogger_src}"
)

set_target_properties(${TARGET_LIBQCRIL_LOGGER} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilNrLogger_hdrs
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/mocks
)

#target_include_directories(${TARGET_LIBQCRIL_LOGGER} PUBLIC ${qcrilNrLogger_hdrs})

add_definitions(-DTAG=\"libqcrilNrLogger\")

# Additional LDFLAGS.
set(LDFLAGS "${LDFLAGS} --no-allow-shlib-undefined,--unresolved-symbols=report-all")

target_link_libraries(${TARGET_LIBQCRIL_LOGGER} pthread diag)

install ( TARGETS ${TARGET_LIBQCRIL_LOGGER}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

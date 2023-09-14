include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

SYSR_INCLUDE_DIR(qcril-nr)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_FRAMEWORK qcrilFramework)

# Library source files
file(GLOB libqcrilFramework_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_FRAMEWORK} SHARED
    "${libqcrilFramework_src}"
)

set_target_properties(${TARGET_LIBQCRIL_FRAMEWORK} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilFramework_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/framework
)

#target_include_directories(${TARGET_LIBQCRIL_FRAMEWORK} PUBLIC ${qcrilFramework_headers})

target_link_libraries(${TARGET_LIBQCRIL_FRAMEWORK} qtimutex pthread qcrilNrLogger)

install ( TARGETS ${TARGET_LIBQCRIL_FRAMEWORK}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

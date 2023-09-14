set( CMAKE_CONFIGURATION_TYPES
        "Debug"
        "Release"
        )
set( CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} CACHE STRING "Available build configurations." FORCE )

SYSR_INCLUDE_DIR(qcril-nr)

include_directories(BEFORE
        ${CMAKE_SOURCE_DIR}/mocks )

set( TARGET_MOCK_LIBCUTILS mockqcrilNrcutils )

set( MOCK_CUTILS_SOURCES
        mockproperties.c
        )

# build library
add_library ( ${TARGET_MOCK_LIBCUTILS} SHARED
        ${MOCK_CUTILS_SOURCES}
        )

target_link_libraries(${TARGET_MOCK_LIBCUTILS}
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_MOCK_LIBCUTILS} PROPERTIES
        LINK_FLAGS "-Wl,-z,defs"
        VERSION 1.0.0
        SOVERSION 1)

# install to target
install ( TARGETS ${TARGET_MOCK_LIBCUTILS}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set( MOCK_CUTILS_HEADERS
        mockproperties.h
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

install( FILES
        ${MOCK_CUTILS_HEADERS}
        DESTINATION include/qcril-nr/cutils )

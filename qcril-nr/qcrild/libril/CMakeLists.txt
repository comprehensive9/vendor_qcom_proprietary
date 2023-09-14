include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/utilities/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qtiwakelock/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/marshalling/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/marshalling/platform/linux/inc
    ${CMAKE_SOURCE_DIR}/mocks
)

SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

# Set target library name as libril
set(TARGET_LIBRIL qcrilNrril)

# Library source files
file(GLOB libril_src "ril.cpp"
    "ril_event.cpp"
    "ril_socket_api.cpp"
    "Parcel2.cpp"
    "ril_utils.cpp"
    "RilSocketIndicationModule.cpp"
    "ril_socket/*.cpp")

# Build libril shared library
add_library (${TARGET_LIBRIL} SHARED
    "${libril_src}"
)

set_target_properties(${TARGET_LIBRIL} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(ril_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/framework
)

#target_include_directories(${TARGET_LIBRIL} PUBLIC ${ril_headers})

target_link_libraries(${TARGET_LIBRIL}
    qtimutex
    pthread
    qcrilNrLogger
    qtiwakelock
    qcrildNrUtils
    qcrilMarshal
    qcrilNr
    qcrilFramework
    ${GLIB_LIBRARIES})

install ( TARGETS ${TARGET_LIBRIL}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

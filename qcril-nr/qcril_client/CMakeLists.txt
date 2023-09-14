include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qcrild/include/telephony
)

SYSR_INCLUDE_DIR(qcril-nr)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --unresolved-symbols=report-all")

# Set target library name as libqcrilClient
set(TARGET_LIBQCRIL_CLIENT qcrilClient)

# Library source files
file(GLOB libqcrilClient_src "Parcel2.cpp"
    "record_stream.c"
    "RilApiSession.cpp"
    "RequestManager.cpp")

# Build libqcrilClient shared library
add_library (${TARGET_LIBQCRIL_CLIENT} SHARED
    "${libqcrilClient_src}"
)

set_target_properties(${TARGET_LIBQCRIL_CLIENT} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qcrilClient_headers
    ${CMAKE_CURRENT_SOURCE_DIR}
)

###target_include_directories(${TARGET_LIBQCRIL_CLIENT} PUBLIC ${qcrilClient_headers})

target_link_libraries(${TARGET_LIBQCRIL_CLIENT} pthread
    qcrilMarshal
    qcrilNrLogger
    qcrilFramework
    qcrildNrUtils
    ${GLIB_LIBRARIES})

install ( TARGETS ${TARGET_LIBQCRIL_CLIENT}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set(TARGET_RIL_SAMPLE ril_sample)
set(TARGET_SAMPLE_SOURCES sample.cpp)
add_executable(${TARGET_RIL_SAMPLE} ${TARGET_SAMPLE_SOURCES})

target_link_libraries(${TARGET_RIL_SAMPLE} qcrilClient)

install ( TARGETS ${TARGET_RIL_SAMPLE}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

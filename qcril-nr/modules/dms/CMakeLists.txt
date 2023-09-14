include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/nanopb
    ${CMAKE_SOURCE_DIR}/qcril_qmi
    ${CMAKE_SOURCE_DIR}/qcril_qmi/radio_config
    ${CMAKE_SOURCE_DIR}/qcril_qmi/utilities
    ${CMAKE_SOURCE_DIR}/qcril_qmi/nanopb_utils
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/modules/ims/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi-client-helper)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(qmi)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as libqcrilNrDmsModule
set(TARGET_LIBQCRIL_DMS_MODULE qcrilNrDmsModule)

# Library source files
file(GLOB libqcrilNrDmsModule_src "src/*.cpp")

# Build libqcrilNrDmsModule static library
add_library (${TARGET_LIBQCRIL_DMS_MODULE} STATIC
    "${libqcrilNrDmsModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_DMS_MODULE}
    qtimutex
    qcrilFramework
    qcrilInterfaces
    qcrilInternalMsgs
    qcrilNrLogger)

include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/nanopb
    ${CMAKE_SOURCE_DIR}/qcril_qmi
    ${CMAKE_SOURCE_DIR}/qcril_qmi
    ${CMAKE_SOURCE_DIR}/qcril_qmi/utilities
    ${CMAKE_SOURCE_DIR}/qcril_qmi/nanopb_utils
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
    ${CMAKE_SOURCE_DIR}/qcrild/include
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

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_VOICE_MODULE qcrilNrvoiceModule)

# Library source files
file(GLOB libqcrilNrvoiceModule_src "src/*.cpp" "src/ims/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_VOICE_MODULE} STATIC
    "${libqcrilNrvoiceModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_VOICE_MODULE}
    qtimutex
    qtiwakelock
    qcrilFramework
    qcrilInterfaces
    qcrilInternalMsgs
    qcrilNrLogger)


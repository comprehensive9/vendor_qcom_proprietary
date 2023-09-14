include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/nanopb
    ${CMAKE_SOURCE_DIR}/qcril_qmi
    ${CMAKE_SOURCE_DIR}/qcril_qmi
    ${CMAKE_SOURCE_DIR}/qcril_qmi/nanopb_utils
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(qmi)

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_UTILS qcrilNrUtils)

# Library source files
file(GLOB libqcrilNrUtils_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_UTILS} STATIC
    "${libqcrilNrUtils_src}"
)

set(qcrilNrUtils_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

#target_include_directories(${TARGET_LIBQCRIL_UTILS} PUBLIC ${qcrilNrUtils_headers})

target_link_libraries(${TARGET_LIBQCRIL_UTILS} qtimutex qcrilFramework qcrilInterfaces)

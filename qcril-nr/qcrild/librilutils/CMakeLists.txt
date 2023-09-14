include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi-client-helper)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qcril-data-hal)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as qcrilNrUtils
set(TARGET_LIBQCRILD_UTILS qcrildNrUtils)

# Library source files
file(GLOB libqcrildNrUtils_src "*.c")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRILD_UTILS} STATIC
    "${libqcrildNrUtils_src}"
)

target_link_libraries(${TARGET_LIBQCRILD_UTILS})

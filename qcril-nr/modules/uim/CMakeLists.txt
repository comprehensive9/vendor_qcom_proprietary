include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/qcril_qmi/utilities/
    ${CMAKE_SOURCE_DIR}/qcril_qmi/utilities/timer
    ${CMAKE_SOURCE_DIR}/qcril-common/interfaces/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/internal-msgs/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/framework/inc
    ${CMAKE_SOURCE_DIR}/qcril-common/qtimutex/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_UIM_MODULE qcrilNrUimModule)

# Library source files
file(GLOB libqcrilNrUimModule_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_UIM_MODULE} STATIC
    "${libqcrilNrUimModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_UIM_MODULE} qtimutex
    qcrilFramework qcrilInterfaces qcrilInternalMsgs ${GLIB_LIBRARIES})

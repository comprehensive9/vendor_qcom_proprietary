include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcril-common/logger
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
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

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as libqcrilFramework
set(TARGET_LIBQCRIL_PBM_MODULE qcrilNrPbmModule)

# Library source files
file(GLOB libqcrilNrPbmModule_src "src/*.cpp")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_PBM_MODULE} STATIC
    "${libqcrilNrPbmModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_PBM_MODULE} qtimutex qcrilFramework qcrilInterfaces)

include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

SYSR_INCLUDE_DIR(qcril-nr)

# Set target library name as libqcrilNrCellInfoModule
set(TARGET_LIBQCRIL_CELLINFO_MODULE qcrilNrCellInfoModule)

# Library source files
file(GLOB libqcrilNrCellInfoModule_src "src/*.cpp")

# Build libqcrilNrCellInfoModule static library
add_library (${TARGET_LIBQCRIL_CELLINFO_MODULE} STATIC
    "${libqcrilNrCellInfoModule_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_CELLINFO_MODULE} qcrilFramework)

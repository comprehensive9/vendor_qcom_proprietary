include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(qmi-framework)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as qcrilNrQmiModule
set(TARGET_LIBQCRILNR_QMI_MODULE qcrilNrQmiModule)

# Library source files
file(GLOB libqcrilNrQmiModule_src "src/*.cpp")
file (GLOB exclude_srcs "src/UimHttpModemEndPoint.cpp"
    "src/UimHttpModemEndPointModule.cpp")
list(REMOVE_ITEM libqcrilNrQmiModule_src ${exclude_srcs})

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRILNR_QMI_MODULE} STATIC
    "${libqcrilNrQmiModule_src}"
)

set(qtimutex_headers
    ${CMAKE_SOURCE_DIR}/include/modules/qmi
)

#target_include_directories(${TARGET_LIBQCRILNR_QMI_MODULE} PUBLIC ${qmimodule_headers})

target_link_libraries(${TARGET_LIBQCRILNR_QMI_MODULE} qtimutex qcrilFramework qmi_cci
    qmiservices qmiidl ${GLIB_LIBRARIES})

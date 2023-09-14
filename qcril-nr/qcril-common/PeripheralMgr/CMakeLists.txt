include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/le
)

SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

set(TARGET_LIBQTIPERIPHERALMGR qtiPeripheralMgr)

file(GLOB libqtiPeripheralMgr_src "src/common/*.cpp" "src/le/*.cpp")


set(qtiPeripheralMgr-headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_library (${TARGET_LIBQTIPERIPHERALMGR} STATIC
    "${libqtiPeripheralMgr_src}"
)

target_include_directories(${TARGET_LIBQTIPERIPHERALMGR} PUBLIC ${qtiPeripheralMgr-headers})

target_link_libraries(${TARGET_LIBQTIPERIPHERALMGR} qtimutex qcrilNrLogger)

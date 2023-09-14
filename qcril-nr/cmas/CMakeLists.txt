include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

SYSR_INCLUDE_DIR(qcril-nr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

set(TARGET_LIBQTIQCRILNRCMAS qcrilNrCmas)

file(GLOB libqcrilNrCmas_src "src/*.cpp")


set(qtiqcrilnrcmas-headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

add_library (${TARGET_LIBQTIQCRILNRCMAS} STATIC
    "${libqcrilNrCmas_src}"
)

target_include_directories(${TARGET_LIBQTIQCRILNRCMAS} PUBLIC ${qtiqcrilnrcmas-headers})

target_link_libraries(${TARGET_LIBQTIQCRILNRCMAS} qtimutex qcrilNrLogger)

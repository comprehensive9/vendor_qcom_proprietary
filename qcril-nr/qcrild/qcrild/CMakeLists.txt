include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/mocks
)

SYSR_INCLUDE_DIR(qcril-nr)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Set target library name as libril
set(TARGET_QCRILNRD qcrilNrd)

# Library source files
set(SOURCE_FILES "rild.c")

add_executable (${TARGET_QCRILNRD} ${SOURCE_FILES})

target_link_libraries(${TARGET_QCRILNRD}
    qcrilNrril
    qcrildNrUtils
    qcrilNr
    dl
    ${GLIB_LIBRARIES})

install ( TARGETS ${TARGET_QCRILNRD}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

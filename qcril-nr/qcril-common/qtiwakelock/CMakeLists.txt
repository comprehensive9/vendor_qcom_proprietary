include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/le
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqtiwakelock
set(TARGET_LIBQTIWAKELOCK qtiwakelock)

# Library source files
file(GLOB libqtiwakelock_src "src/common/*.cpp" "src/le/*.cpp")

# Build libqtiwakelock shared library
add_library (${TARGET_LIBQTIWAKELOCK} SHARED
    "${libqtiwakelock_src}"
)

set_target_properties(${TARGET_LIBQTIWAKELOCK} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

set(qtiwakelock_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

#target_include_directories(${TARGET_LIBQTIWAKELOCK} PUBLIC ${qtiwakelock_headers})

target_link_libraries(${TARGET_LIBQTIWAKELOCK} qtimutex)

install ( TARGETS ${TARGET_LIBQTIWAKELOCK}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


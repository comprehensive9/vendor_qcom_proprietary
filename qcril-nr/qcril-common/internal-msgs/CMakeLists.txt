include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/qcrild/include
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(libcutils)
SYSR_INCLUDE_DIR(qmi)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqtibus
set(TARGET_LIBQCRILINTERNALMSGS qcrilInternalMsgs)

# Library source files
file(GLOB qcrilInternalMsgs_src "nas/src/*.cpp" "ims/src/*.cpp")

# Build libqtibus shared library
add_library (${TARGET_LIBQCRILINTERNALMSGS} SHARED
    "${qcrilInternalMsgs_src}"
)

set_target_properties(${TARGET_LIBQCRILINTERNALMSGS} PROPERTIES
    LINK_FLAGS "-Wl,-z,defs"
    VERSION 1.0.0
    SOVERSION 1)

target_link_libraries(${TARGET_LIBQCRILINTERNALMSGS} qtimutex qcrilFramework qcrilNrLogger)

install ( TARGETS ${TARGET_LIBQCRILINTERNALMSGS}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

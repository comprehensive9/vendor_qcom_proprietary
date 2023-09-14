include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}/nanopb
    ${CMAKE_SOURCE_DIR}/qcrild/include
    ${CMAKE_SOURCE_DIR}/modules/ims/src
    ${CMAKE_SOURCE_DIR}/qcril_qmi/services
    ${CMAKE_SOURCE_DIR}/qcril_qmi/oem_socket
    ${CMAKE_SOURCE_DIR}/qcril_qmi/nanopb_utils
    ${CMAKE_SOURCE_DIR}/qcril_qmi/radio_config
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi-client-helper)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(libxml2)
SYSR_INCLUDE_DIR(qcril-data-hal)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)

add_definitions(-DRIL_FOR_LOW_RAM)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Set target library name as libqcrilNr
set(TARGET_LIBQCRIL_NR qcrilNr)

# Library source files
file(GLOB libqcrilNr_src "*.cpp"
    "*.cc"
    "*.c"
    "../nanopb/*.c"
    "nanopb_utils/*.cc"
    "oem_socket/*.cc"
    "services/*.c")

# Build libqcrilFramework shared library
add_library (${TARGET_LIBQCRIL_NR} SHARED
    "${libqcrilNr_src}"
)

target_link_libraries(${TARGET_LIBQCRIL_NR}
    qtimutex
    qtibus
    qcrilFramework
    qcrilInterfaces
    qcrilInternalMsgs
    qcrilNrNasModule
    -Wl,--whole-archive
    qcrilNrvoiceModule
    qcrilNrImsModule
    qcrilNrCellInfoModule
    qcrilNrDmsModule
    qcrilNrPbmModule
    qcrilNrSmsModule
    qcrilNrUimModule
    qcrilNrAndroidTranslators
    qcrilNrAndroidModule
    qcrilDataModule
    qtiPeripheralMgr
    qcrilNrQmiModule
    qcrildNrUtils
    qcrilNrUtils
    libqcrilDataInterfaces
    libqcrilDataInternal
    dsi_netctrl
    ril-db
    -Wl,--no-whole-archive
    qdp
    qmi
    qmi_cci
    qmi_client_helper
    qmi_client_qmux
    qmiservices
    qmi_encdec
    sqlite3
    xml2
    pthread
    dsutils
    utils
    mockqcrilNrcutils
    rt
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBRILQCQMI1} PROPERTIES
                      LINK_FLAGS "-Wl,-z,defs"
                      VERSION 1.0.0
                      SOVERSION 1)

# Export include/framework to usr/include/qcril-nr/framework
install ( TARGETS ${TARGET_LIBQCRIL_NR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

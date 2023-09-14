# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(qcril-nr)
SYSR_INCLUDE_DIR(request)
SYSR_INCLUDE_DIR(UnSolMessages)
SYSR_INCLUDE_DIR(event)
SYSR_INCLUDE_DIR(sync)
SYSR_INCLUDE_DIR(qmi-framework)
SYSR_INCLUDE_DIR(qmi-client-helper)
SYSR_INCLUDE_DIR(dsutils)
SYSR_INCLUDE_DIR(qmi)
SYSR_INCLUDE_DIR(data)
SYSR_INCLUDE_DIR(diag)
SYSR_INCLUDE_DIR(libxml2)

# Set various c/cpp flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

# Set target library name as qcrilDataModule
set(TARGET_LIBQCRILDATA qcrilDataModule)

# Library source files
file(GLOB libqcrilData_src "src/legacy/*"
    "../../util/src/*.cpp"
    "../../endpoints/src/*.cpp"
    "../../authmanager/src/*.cpp"
    "../../profilehandler/src/*.cpp"
    "../../qdphandler/legacy/*.cpp"
    "../../qdphandler/src/*.cpp"
    "../../callmanager/src/*.cpp"
    "../../lcehandler/src/*.cpp"
    "../../KeepAliveManager/src/*.cpp"
    "../../networkservicehandler/src/*.cpp"
    "../../networkavailabilityhandler/src/*.cpp"
    "../../lqe/src/*.c"
    "src/*.cpp"
    )

file (GLOB exclude_srcs "../../util/src/DsiWrapper.cpp"
    "../../callmanager/src/EmbmsCallHandler.cpp"
    "../../endpoints/src/VoiceCallModemEndPoint.cpp"
    "../../endpoints/src/VoiceCallModemEndPointModule.cpp"
    "src/legacy/qcril_data_embms.c"
    )

list(REMOVE_ITEM libqcrilData_src ${exclude_srcs})

if (RIL_SHARED_LIB)
# Build libqcrilDataModule shared library
add_library (${TARGET_LIBQCRILDATA} SHARED
    "${libqcrilData_src}"
)
else ()
# Build libqcrilDataModule static library
add_library (${TARGET_LIBQCRILDATA} STATIC
    "${libqcrilData_src}"
)
endif (RIL_SHARED_LIB)

target_link_libraries(${TARGET_LIBQCRILDATA}
    qdp
    dsi_netctrl
    log
    utils
    libqcrilDataInterfaces
    libqcrilDataInternal
    qcrilFramework
    qcrilNrQmiModule
    qcrilNrUtils
    qcrilNr
    ril-db
    sqlite3
    qmi
    qmi_cci
    qmi_client_qmux
    qmiservices
    qmi_encdec
    qcrilInternalMsgs
    qtimutex
    dsutils
    diag
    qcrilNrLogger
    qtibus
    qmi_client_helper
    rt
    ${GLIB_LIBRARIES})

set_target_properties(${TARGET_LIBQCRILDATA} PROPERTIES
                      LINK_FLAGS "-Wl,-z,defs"
                      VERSION 1.0.0
                      SOVERSION 1)


install ( TARGETS ${TARGET_LIBQCRILDATA}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )


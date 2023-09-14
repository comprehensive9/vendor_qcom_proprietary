LOCAL_PATH:= $(call my-dir)
#Generate Diag shared library
include $(CLEAR_VARS)

LOCAL_VENDOR_MODULE := true
LOCAL_COPY_HEADERS_TO := diag/include
LOCAL_COPY_HEADERS := ../include/diagi.h
LOCAL_COPY_HEADERS += ../include/diaglogi.h
LOCAL_COPY_HEADERS += ../include/diag_lsm.h
LOCAL_COPY_HEADERS += ../include/diag_lsm_dci.h
LOCAL_COPY_HEADERS += ../include/diag.h
LOCAL_COPY_HEADERS += ../include/diagcmd.h
LOCAL_COPY_HEADERS += ../include/diagpkt.h
LOCAL_COPY_HEADERS += ../include/event_defs.h
LOCAL_COPY_HEADERS += ../include/event.h
LOCAL_COPY_HEADERS += ../include/log_codes.h
LOCAL_COPY_HEADERS += ../include/log.h
LOCAL_COPY_HEADERS += ../include/msgcfg.h
LOCAL_COPY_HEADERS += ../include/msg.h
LOCAL_COPY_HEADERS += ../include/msg_pkt_defs.h
LOCAL_COPY_HEADERS += ../include/msgtgt.h
LOCAL_COPY_HEADERS += ../include/msg_qsr.h
LOCAL_COPY_HEADERS += ../include/msg_arrays_i.h
LOCAL_COPY_HEADERS += diag_logger.h
LOCAL_COPY_HEADERS += diag_lsm_dci_i.h
LOCAL_COPY_HEADERS += diag_lsm_event_i.h
LOCAL_COPY_HEADERS += diag_lsm_log_i.h
LOCAL_COPY_HEADERS += diag_lsm_msg_i.h
LOCAL_COPY_HEADERS += diag_lsm_pkt_i.h
LOCAL_COPY_HEADERS += diag_lsmi.h
LOCAL_COPY_HEADERS += diag_shared_i.h
LOCAL_COPY_HEADERS += diagdiag.h
LOCAL_COPY_HEADERS += diagsvc_malloc.h
LOCAL_COPY_HEADERS += eventi.h
LOCAL_COPY_HEADERS += ts_linux.h

include $(BUILD_COPY_HEADERS)
#CC := $(ATH_CROSS_COMPILE_TYPE)gcc
CC = gcc

CFLAGS +=  -D_HOST_SIM_TESTING \
           -D_FOR_HOST_TESTING \
           -I ../include \
           -I templete \
           -I common \
           -I defParm

LDFLAGS += -shared

TARGET_LIB = libtlv1util.so
SRCS =      templete/_cmd_txParmTxtTemplate.c \
            templete/_cmd_txParmBinTemplate.c \
            templete/_cmd_rxParmTxtTemplate.c \
            templete/_cmd_rxParmBinTemplate.c \
            templete/_cmd_calParmTxtTemplate.c \
            templete/_cmd_calParmBinTemplate.c \
            templete/_cmd_calDoneParmTxtTemplate.c \
            templete/_cmd_calDoneParmBinTemplate.c \
            templete/_cmd_rspGenericParmTxtTemplate.c \
            templete/_cmd_rspGenericParmBinTemplate.c \
            templete/_cmd_submitreportParmTxtTemplate.c \
            templete/_cmd_submitreportParmBinTemplate.c \
            templete/_cmd_pmParmTxtTemplate.c \
            templete/_cmd_pmParmBinTemplate.c \
            templete/_cmd_nartCmdParmTxtTemplate.c \
            templete/_cmd_nartCmdParmBinTemplate.c \
            templete/_cmd_nartRspParmTxtTemplate.c \
            templete/_cmd_nartRspParmBinTemplate.c \
            templete/_cmd_txStatusParmTxtTemplate.c \
            templete/_cmd_txStatusParmBinTemplate.c \
            templete/_cmd_rxStatusParmTxtTemplate.c \
            templete/_cmd_rxStatusParmBinTemplate.c \
            hex_dump.c \
            testUtil.c \
            parseRxBinCmdTlv.c \
            genTxBinCmdTlv.c \
            cmdParmsTemplate.c \
            CliTlvCmd_if.c

OBJS = $(SRCS:.c=.o)

.PHONY=: all

all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

.PHONY: clean

clean:
	rm -f ${TARGET_LIB} ${OBJS}

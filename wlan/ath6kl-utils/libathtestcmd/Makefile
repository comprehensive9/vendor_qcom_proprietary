#CC := $(ATH_CROSS_COMPILE_TYPE)gcc
CC = gcc

CFLAGS += -DWLAN_API_NL80211 -DLIBNL_2
LDFLAGS += -shared

TARGET_LIB = libtestcmd6174.so
SRCS = athtestcmdlib.c
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

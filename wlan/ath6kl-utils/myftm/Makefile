#CC := $(ATH_CROSS_COMPILE_TYPE)gcc
TARGET_TYPE ?= AR9888
TARGET_VERS ?= v2

#Sources to compile
CSRCS := myftm_main.c myftm_wlan.c myftm_qtip.c

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -g3 -Wall  \
		$(CSRCS) -o myftm
clean:
	rm -f myftm

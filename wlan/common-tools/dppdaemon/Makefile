ALL= dppdaemon
# install
all: ${ALL}

clean:
	rm -f *.so *.o *.d ${ALL}

LDFLAGS += -rdynamic

ifndef CXX
CXX=g++
endif

ifndef CXXSTD
CXXSTD=c++14
endif

ifndef CFLAGS
CFLAGS = -MMD -O2 -Wall -g
CFLAGS+= -std=${CXXSTD}
endif

OBJ=dppCommonUtils.o dppManager.o dppEnrollee.o dppWpaCtrlIface.o dppdaemon.o xmlUtil.o wpa_ctrl_utils.o

%.o: %.cpp
	$(CXX) -c -fPIC $(CFLAGS) ${COPTS} $< -o $@
	@echo "  CXX " $<

libdpp_manager.so: $(OBJ)
	$(Q)$(CXX) $(LDFLAGS) -o $@ $(CFLAGS) -shared -fPIC $^

dppdaemon: libdpp_manager.so
	$(CXX) -o $@ ${CXXFLAGS} ${CFLAGS} -Wall  $^ -L. -ldpp_manager

install:
	cp -a -f dppdaemon $(INSTALL_ROOT)/usr/bin/

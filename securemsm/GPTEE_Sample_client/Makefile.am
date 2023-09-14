ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG  \
    -I$(SRC_TOP)/security/securemsm/GPTEE/inc \
    -DOE

requiredlibs =
library_includedir =
library_include_HEADERS =

c_sources = GPTEE_Sample_client.cpp
h_sources = GPTEE_Sample_client.h
bin_PROGRAMS = GPTEE_Sample_client
GPTEE_Sample_client_CC = @CC@

pkgconfigdir = $(libdir)/pkgconfig

GPTEE_Sample_client_LDFLAGS = -lGPTEE_vendor -lpthread
GPTEE_Sample_client_SOURCES = $(c_sources) $(h_sources)

all-local: $(bin_PROGRAMS)

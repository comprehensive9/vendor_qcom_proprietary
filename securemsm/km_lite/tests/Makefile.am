AM_CFLAGS = -Wall -g \
        -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        $(SECUREMSMNOSHIP_CFLAGS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D__packed__= \
    -DFEATURE_LE_DIAG \
    -I$(WORKSPACE)/security/securemsm/km_lite \
    -DOE \
    $(SECUREMSMNOSHIP_CFLAGS)

bin_PROGRAMS = km_lite_sample_client
h_sources = sample_utils.h
km_lite_sampleclient_CC = @CC@
pkgconfigdir = $(libdir)/pkgconfig
km_lite_sample_client_SOURCES = keymaster_aes_sample.c keymaster_delete_sample.c keymaster_ecdsa_sample.c keymaster_hmac_sample.c keymaster_impexp_sample.c keymaster_key_upgrade_sample.c keymaster_rsa_sample.c main.cpp sample_utils.c keymaster_multithreading.c $(h_sources)
km_lite_sample_client_CPPFLAGS = $(AM_CPPFLAGS)
km_lite_sample_client_CFLAGS = $(AM_CFLAGS) $(OPENSSL_CFLAGS)
km_lite_sample_client_LDFLAGS = -lpthread -ldl -lm -llog -lion -lKeyMaster


# Makefile.am - Automake script for sscrpcd

ACLOCAL_AMFLAGS = -I m4

root_systemdir = "/system"
root_persistdir = "/persist"

common_list = $(shell find  $(srcdir)/config/common/ -type f)

if SDM845_TARGET
common_jsondir   = $(root_persistdir)/etc/sensors/config
else
common_jsondir   = $(root_systemdir)/etc/sensors/config
endif

common_json_DATA = $(common_list)

sns_reg_config_list = $(shell find  $(srcdir)/sns_le_config/ -type f)
sns_reg_configdir = $(root_systemdir)/etc/sensors
sns_reg_config_DATA = $(sns_reg_config_list)

if SDM845_TARGET
sdm845_list = $(shell find  $(srcdir)/config/sdm845/ -type f)
sdm845_jsondir = $(root_persistdir)/sensors/registry/config
sdm845_json_DATA = $(sdm845_list)

sdm845_reg_configdir  = $(root_persistdir)/sensors/registry/registry

endif

if QCS605_TARGET
qcs605_list = $(shell find  $(srcdir)/config/qcs605/ -type f)
qcs605_jsondir = $(root_systemdir)/etc/sensors/config
qcs605_json_DATA = $(qcs605_list)
endif

if QCS40X_TARGET
qcs405_list = $(shell find  $(srcdir)/config/qcs405/ -type f)
qcs405_jsondir = $(root_systemdir)/etc/sensors/config
qcs405_json_DATA = $(qcs405_list)
endif

if SDMSTEPPE_TARGET
sdmsteppe_list = $(shell find  $(srcdir)/config/sm6150/ -type f)
sdmsteppe_jsondir = $(root_systemdir)/etc/sensors/config
sdmsteppe_json_DATA = $(sdmsteppe_list)
endif

if KONA_TARGET
kona_list = $(shell find  $(srcdir)/config/kona/ -type f)
kona_jsondir = $(root_systemdir)/etc/sensors/config
kona_json_DATA = $(kona_list)
endif

if SXR2130_TARGET
sxr2130_list = $(shell find  $(srcdir)/config/sxr2130/ -type f)
sxr2130_jsondir = $(root_systemdir)/etc/sensors/config
sxr2130_json_DATA = $(sxr2130_list)

sxr2130_kona_list = $(shell find  $(srcdir)/config/sxr2130_mtp/ -type f)
sxr2130_kona_jsondir = $(root_systemdir)/etc/sensors/config
sxr2130_kona_json_DATA = $(sxr2130_kona_list)
endif

if QRB5165_TARGET
qrb5165_list = $(shell find  $(srcdir)/config/qrb5165/ -type f)
qrb5165_jsondir = $(root_systemdir)/etc/sensors/config
qrb5165_json_DATA = $(qrb5165_list)
endif

if QRBX210_TARGET
qrbx210_list = $(shell find  $(srcdir)/config/qrbx210/ -type f)
qrbx210_jsondir = $(root_systemdir)/etc/sensors/config
qrbx210_json_DATA = $(qrbx210_list)
endif

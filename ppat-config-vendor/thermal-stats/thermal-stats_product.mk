LIMITS_STAT_PRODUCT_LIST += lahaina

ifeq ($(call is-board-platform-in-list,$(LIMITS_STAT_PRODUCT_LIST)),true)
	PRODUCT_PACKAGES_DEBUG += limits_stat.ko
endif

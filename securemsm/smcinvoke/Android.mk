ifeq ($(call is-board-platform-in-list,msmnile sdmshrike sdm845 sdm660 sdm710 $(MSMSTEPPE) kona lito atoll bengal monaco $(TRINKET) lahaina holi),true)

include $(call all-subdir-makefiles)

endif

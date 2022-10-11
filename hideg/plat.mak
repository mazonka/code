PLAT=unx

ifdef OS

ifeq ($(OS),Windows_NT)
PLAT=win
endif

endif

ifneq (,$(wildcard macos))
PLAT=mac
endif

ifneq (,$(wildcard mingw))
PLAT=mgw
endif


BINN=bn_$(PLAT)
BINT=bt_$(PLAT)

$(info PLAT=$(PLAT))

MPIRD0=../../../3p/mpir_$(PLAT)

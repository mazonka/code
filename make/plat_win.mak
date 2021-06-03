
CL=cl -EHsc -nologo -Ox -D_USING_V110_SDK71_=1 -bigobj /std:c++17 $(INCS)

EOUT=-Fe
OOUT=-Fo
OEXT=obj
EEXT=exe

LDFS=WS2_32.Lib advapi32.lib


ifneq ($(MPIR),0)
MPIRD1 = $(RT)/../3p/mpir_$(PLAT)/native
LDF1 += $(MPIRD1)/mpir.lib $(MPIRD1)/mpirxx.lib
endif

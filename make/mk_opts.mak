
INCS = -I$(RT) -I.
HEAD =

ifneq ($(MPIR),0)
INCS += -I$(RT)/$(MPIRD0)
HEAD += $(RT)/$(MPIRD0)/mpir.h
endif


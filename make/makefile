
MPIR=0

RT=./
OD=$(RT)/bt_$(PLAT)/

include mk_plat.mak


#INCS = -I$(RT) -I. -DE3NOABORT=1
#HEAD =

#ifneq ($(MPIR),0)
#INCS += -I$(RT)/$(MPIRD0)
#HEAD += $(RT)/$(MPIRD0)/mpir.h
#endif

include mk_opts.mak

include mk_$(PLAT).mak

src1 = a.cpp
obj1 := $(src1:%.cpp=o/%.$(OEXT))

include mk_rules.mak


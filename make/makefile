
MPIR=1

RT=../..
OD=$(RT)/bt_$(PLAT)/

include plat.mak


#INCS = -I$(RT) -I. -DE3NOABORT=1
#HEAD =

#ifneq ($(MPIR),0)
#INCS += -I$(RT)/$(MPIRD0)
#HEAD += $(RT)/$(MPIRD0)/mpir.h
#endif

include mk_opts.mak

include plat_$(PLAT).mak

src1 = a_im1c.cpp main.cpp bitfield.cpp a_base.cpp a_barr.cpp a_fulmul.cpp \
	a_modmul.cpp a_flex.cpp a_kara.cpp perform.cpp a_shoup.cpp a_koch.cpp
obj1 := $(src1:%.cpp=o/%.$(OEXT))

include mk_rules.mak




CL=g++ -O3 -std=c++17 $(INCS)

EOUT=-o 
OOUT=-o 
OEXT=o
EEXT=exe

#LDFS=-lpthread -ldl
LDFS=

ifneq ($(MPIR),0)
MPIRD1=$(RT)/../3p/mpir_$(PLAT)/native
LDF1+=$(MPIRD1)/libmpir.a $(MPIRD1)/libcxx.a
endif

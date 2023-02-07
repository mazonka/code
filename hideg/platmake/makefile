

include ../plat.mak

O = o_$(PLAT)

include ../mk_opts.mak

include ../plat_$(PLAT).mak

src1 = hash.cpp sha2.cpp bzcf.cpp olu.cpp hidegf.cpp fcls.cpp sync.cpp gfu.cpp

obj1 := $(src1:%.cpp=$(O)/%.$(OEXT))

include mk_rules.mak

key:
	./gf_$(PLAT).exe bzc genkey .gf.key < ctest/gkey.txt

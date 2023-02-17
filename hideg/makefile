

include plat.mak


include mk_opts.mak

include plat_$(PLAT).mak

src1 = hash.cpp bzcf.cpp olu.cpp hidegf.cpp fcls.cpp sync.cpp gfu.cpp sha256.cpp jadd.cpp

obj1 := $(src1:%.cpp=o/%.$(OEXT))

include mk_rules.mak

key:
	./gf.exe bzc genkey .gf.key < ctest/gkey.txt

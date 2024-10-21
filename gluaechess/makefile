

TCPINCDIR=tcp/inc

include plat.mak


include mk_opts.mak

include plat_$(PLAT).mak

src1 = gluae.cpp
obj1 := $(src1:%.cpp=o/%.$(OEXT))

src2 = echo.cpp
obj2 := $(src2:%.cpp=o/%.$(OEXT))

src3 = fdback.cpp
obj3 := $(src3:%.cpp=o/%.$(OEXT))

src4 = feedin.cpp
obj4 := $(src4:%.cpp=o/%.$(OEXT))

htlibs = tcp/libs/htgllib.lib tcp/libs/htoslib.lib tcp/libs/htsglib.lib

include mk_rules.mak


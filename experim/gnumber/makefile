

ifeq ($(OS),Windows_NT)
PLAT=w
else
PLAT=u
endif

ifeq ($(PLAT),w)
CL = cl
O = o_msc
OEXT = .obj
OPT=-WX -W4 -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4355 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1

# unref var
#OPT+= /wd4189

OOUT=-Fo
EOUT=-Fe
ARCR=lib /OUT:
endif

ifeq ($(PLAT),u)
CL = g++
O = o_unx
OEXT = .o
OPT=-Wall -O2 -std=c++11 -fno-strict-aliasing -ffloat-store -pthread
ifeq ($(OS),Windows_NT)
OPT=-Wall -O2 -std=c++11 -fno-strict-aliasing -ffloat-store -lpthread
endif
OOUT=-o 
EOUT=-o 
ARCR=ar crs #
endif

EEXT = .$(PLAT).exe

src = main.cpp
obj := $(src:%.cpp=$O/%$(OEXT))

all: $O gn$(EEXT)

$O:
	mkdir $O


gn$(EEXT): $O/gn$(OEXT) cpp/*.h
	$(CL) $(OPT) $< $(EOUT)$@

$O/gn$(OEXT): cpp/gn.cpp
	$(CL) -c $(OPT) $< $(OOUT)$@

#$(obj): $O/%$(OEXT):%.cpp *.h $(ALGL)
$(obj): $O/%$(OEXT):cpp/%.cpp cpp/*.h
	$(CL) -c $(OPT) $< $(OOUT)$@

clean:
	rm -rf $O *.exe *.obj *.o o_* out/*.dat

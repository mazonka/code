

all: $(HEAD) bzc.exe gf.exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1

bzc.exe: o/bzc.$(OEXT) $(obj1)
	$(CL) $< $(obj1) $(LDFS) $(EOUT)$@

gf.exe: o/gf.$(OEXT) $(obj1)
	$(CL) $< $(obj1) $(LDFS) $(EOUT)$@


o/bzc.$(OEXT): bzc.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

o/gf.$(OEXT): gf.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(obj1): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@


c clean:
	rm -f *.$(OEXT) *.exe bzc.key
	rm -rf o

s:
	cmd /C "$(APP)/run/style.bat *.cpp *.h"

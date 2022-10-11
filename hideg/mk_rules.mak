

all: $(HEAD) bzc.exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1

bzc.exe: o/bzc.$(OEXT) $(obj1)
	$(CL) $< $(obj1) $(LDFS) $(EOUT)bob.exe


o/bzc.$(OEXT): bzc.cpp *.h
	@mkdir -p o
	$(CL) -c bzc.cpp $(OOUT)$@

$(obj1): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(objm): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@


$(obj2): o/%.$(OEXT):../%.cpp ../*.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@



c clean:
	rm -f *.$(OEXT) *.exe
	rm -rf o

s:
	cmd /C "$(APP)/run/style.bat *.cpp *.h"

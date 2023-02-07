

#all: $(HEAD) bzc.exe gf.exe
all: $(HEAD) gf_$(PLAT).exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1

#bzc.exe: o/bzc.$(OEXT) $(obj1)
#	$(CL) $< $(obj1) $(LDFS) $(EOUT)$@

gf_$(PLAT).exe: $(O)/gf.$(OEXT) $(obj1)
	$(CL) $< $(obj1) $(LDFS) $(EOUT)$@


#o/bzc.$(OEXT): bzc.cpp *.h
#	@mkdir -p o
#	$(CL) -c $< $(OOUT)$@

$(O)/gf.$(OEXT): ../gf.cpp ../*.h
	@mkdir -p $(O)
	$(CL) -c $< $(OOUT)$@

$(obj1): $(O)/%.$(OEXT):../%.cpp ../*.h
	@mkdir -p $(O)
	$(CL) -c $< $(OOUT)$@


c clean:
	rm -f *.$(OEXT) *.exe .gf.key
	rm -rf o_*

#s:
#	cmd /C "$(APP)/run/style.bat *.cpp *.h"

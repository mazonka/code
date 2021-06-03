

all: $(HEAD) bob.exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1

bob.exe: $(OD)secint.$(OEXT) $(OD)cgtshared.$(OEXT) $(RT)/cgtshared.h $(obj1) $(obj2) $(LDF1)
	$(CL) $(obj1) $(obj2) $(OD)secint.$(OEXT) $(OD)cgtshared.$(OEXT) $(LDF1) $(LDFS) $(EOUT)bob.exe

bob: bob.exe
	./bob.exe

$(obj1): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(obj2): o/%.$(OEXT):../%.cpp ../*.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(OD)secint.$(OEXT) $(OD)cgtshared.$(OEXT) $(RT)/cgtshared.h:
	@echo build bob in standard make: $(OD)secint.$(OEXT) $(OD)cgtshared.$(OEXT) $(RT)/cgtshared.h
	exit 1




c clean:
	rm -f *.$(OEXT) *.exe
	rm -rf o

s:
	cmd /C "c:/ap20/run/style.bat *.cpp *.h"

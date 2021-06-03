

all: $(HEAD) bob.exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1

bob.exe: $(obj1) $(obj2) $(LDF1)
	$(CL) $(obj1) $(obj2) $(LDF1) $(LDFS) $(EOUT)bob.exe

bob: bob.exe
	./bob.exe

$(obj1): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(obj2): o/%.$(OEXT):../%.cpp ../*.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@


c clean:
	rm -f *.$(OEXT) *.exe
	rm -rf o

s:
	cmd /C "c:/apg/run/style.bat *.cpp *.h"



all: $(HEAD) gluae.exe echo.exe fdback.exe feedin.exe

$(HEAD):
	@echo "Error: build $(HEAD)"
	@exit 1


gluae.exe: $(obj1) $(LDF1)
	$(CL) $(obj1) $(LDF1) $(LDFS) $(EOUT)gluae.exe

echo.exe: $(obj2)
	$(CL) $(obj2) $(LDFS) $(EOUT)echo.exe

fdback.exe: $(obj3) $(LDF1)
	$(CL) $(obj3) $(LDF1) $(LDFS) $(EOUT)fdback.exe


feedin.exe: $(obj4) $(htlibs) $(LDF1)
	$(CL) $(obj4) $(htlibs) $(LDF1) $(LDFS) $(EOUT)feedin.exe

gluae: gluae.exe
	./gluae.exe

echo: echo.exe
	./echo.exe

fdback: fdback.exe
	./fdback.exe



$(obj1): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

#$(obj2): o/%.$(OEXT):../%.cpp ../*.h

$(obj2): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(obj3): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

$(obj4): o/%.$(OEXT):%.cpp *.h
	@mkdir -p o
	$(CL) -c $< $(OOUT)$@

c clean:
	rm -f *.$(OEXT) *.exe
	rm -rf o
	cd tcp && make clean

s:
	style.bat *.cpp *.h


r run:
	./gluae.exe | ./echo.exe | ./fdback.exe


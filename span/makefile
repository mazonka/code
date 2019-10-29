# PLAT: w u

SRC=./
include ./mk_all.mak

O=$(TRG)

src = gen/util.cpp gen/quest.cpp gen/gl_except.cpp \
gen/gl_utils.cpp gen/os_filesys.cpp

obj := $(src:gen/%.cpp=$O/%$(OEXT))

srp = $(PLAT)/osfun.cpp $(PLAT)/os_file2.cpp $(PLAT)/os_timer2.cpp
obp := $(srp:$(PLAT)/%.cpp=$O/%$(OEXT))


srctrg = span.cpp
trg := $(srctrg:%.cpp=%$(EEXT))


#all: $O $(trg)
all: $O span$(EEXT)

$O:
	mkdir -p $O

span$(EEXT): $(obj) $O/span$(OEXT) $(obp)
	$(COMPILER) $(OPT) $(INC) $O/span$(OEXT) $(obj) $(obp) $(LDF) $(EOUT)$@

#$(trg): %$(EEXT):%.cpp $(obj)
#	$(COMPILER) $(OPT) $(INC) $< $(obj) $(LDF) $(EOUT)$@

$(obj): $O/%$(OEXT):gen/%.cpp gen/*.h
	$(COMPILER) -c $(OPT) $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

$(obp): $O/%$(OEXT):$(PLAT)/%.cpp
	$(COMPILER) -c $(OPT) $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

$O/span$(OEXT): span.cpp
	$(COMPILER) -c $(OPT) $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@


clean:
	rm -rf $(TRG) *.obj *.exe

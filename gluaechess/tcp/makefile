HQDIR=../../../../../hasq/dev/hasqd/src
BIND=_bin_msc

all: testnet.exe

#a:
#	dir $(HQDIR)

testnet.exe: testnet.cpp libs/htgllib.lib libs/htoslib.lib libs/htsglib.lib
	cl -EHsc -Iinc testnet.cpp \
	libs/htgllib.lib libs/htoslib.lib libs/htsglib.lib \
	WS2_32.Lib advapi32.lib

libs/htgllib.lib: $(HQDIR)/gl/$(BIND)/htgllib.lib
	cp $< $@

libs/htoslib.lib: $(HQDIR)/os/$(BIND)/htoslib.lib
	cp $< $@

libs/htsglib.lib: $(HQDIR)/sg/$(BIND)/htsglib.lib
	cp $< $@

c clean:
	rm -f *.obj *.exe libs/*.lib
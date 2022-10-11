
CL=cl -EHsc -nologo -Ox -D_USING_V110_SDK71_=1 /std:c++17 $(INCS)

EOUT=-Fe
OOUT=-Fo
OEXT=obj
EEXT=exe

LDFS=WS2_32.Lib advapi32.lib
LDFS=


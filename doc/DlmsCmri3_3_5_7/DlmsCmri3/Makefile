#################   Build start Test APP    #################################
# Build Revision    1.0
# Build Date        31-MAY-2010
# ChangeLog         Initial version

EXE =  	bin/msedcl_Dlms

SRCS = 	src/AES128.cpp \
   src/autn.cpp \
   src/comm.cpp \
   src/Daksh.cpp \
   src/display.cpp \
   src/FUNCTH.cpp \
   src/global.cpp \
   src/hhuLcd.cpp \
   src/pdsequivalent.cpp \
   src/PROGRAM.cpp\
   src/rs232.cpp \
   src/TestMode.cpp\
   src/timer.cpp \
   src/utility.cpp\
   src/UPLOAD.cpp\
   src/viewMode.cpp\

OBJ =  src/msedcl_Dlms.o

INCLUDES = -I. -I includes
CC = /opt/analogics/at91sam9261_ek/toolchain/bin/armv5l-analogics-linux-gnueabi-g++	 

EXTRAOPT = /opt/analogics/at91sam9261_ek/toolchain/bin/armv5l-analogics-linux-gnueabi-strip
#SO = /usr/local/lib/lib9260_lotus_ver1.0
#SO = /usr/local/lib/lib9260_hhc3t.so.5 
SO = /usr/local/lib/lib9260_lotus.so.4
LIBS = -lssl -lsqlite3 -lssl -lcurl -lssl -lpthread

all: 
	$(CC) $(SRCS) $(SO) $(INCLUDES) $(LIBS) -o $(EXE) 
	$(EXTRAOPT) $(EXE)
	

## Clean the target
clean: 
	rm -f *.o *.so $(EXE)

###################################################################
#
#   EadkDemo.mk
#
#	Makefile to build EADK Demo application that will use 
#   already built EADK library.
# 
#   To build EADK Demo application:
#
#   - Execute "make -f EadkDemo.mk all" command
#    
#	This will build EadkDemo executable
#
###################################################################

#     
#
###################################################################

#################################### MAKE COMMANDS
AR =		ar
CC =		cc
MV =		mv
LD =		ld
CP =		cp

#################################### BASE DIRECTORY
BASEDIR =	.
SRCDIR =	$(BASEDIR)
INCDIR =	$(BASEDIR)
LIBDIR =	/usr/bin

#################################### COMPILE and LINK FLAGS

CFLG =		-c -I$(INCDIR) -DPLATFORM_HEADER=\"./Platform/Linux/platform.h\" -DET_IP_MODBUS \
				-DMBUS_PLATFORM_HEADER=\"../Platform/Linux/mbspltfm.h\"
LFLG =		-lpthread 

#################################### LINUX APPLICATION OBJECT FILES
EADK_DEMO_OBJ = EadkDemo.o \

EADK_LIB = EADK.a \

EadkDemo: $(EADK_DEMO_OBJ) $(EADK_LIB)
	$(CC) -o EadkDemo $(LFLG) $(EADK_DEMO_OBJ) $(EADK_LIB)

EadkDemo.o: $(SRCDIR)/EadkDemo.c 
	$(CC) $(CFLG) $(SRCDIR)/EadkDemo.c

$(EADK_LIB): 
	$(CP) $(SRCDIR)/$(EADK_LIB) $(LIBDIR)

clean:
	rm *.o

all:	EadkDemo

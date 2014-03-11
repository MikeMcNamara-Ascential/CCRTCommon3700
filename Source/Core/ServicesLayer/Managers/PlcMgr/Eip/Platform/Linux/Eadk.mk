###################################################################
#
#   EADK.mk
#
#	Makefile to build EADK library.
# 
#   To build EADK library:
#   
#   
#   - Execute "make -f Eadk.mk all" command
#
#	This will build EADK.a static library
#
#   To build EADK Demo application:
#   
#   - Execute "make -f EadkDemo.mk all" command
#    
#	This will build EadkDemo executable
#
###################################################################

#################################### MAKE COMMANDS
AR =		ar
CC =		cc
MV =		mv
LD =		ld

#################################### BASE DIRECTORY
BASEDIR =	../..
SRCDIR =	$(BASEDIR)
INCDIR =	$(BASEDIR)
MODBUSDIR = $(SRCDIR)/Modbus
MODBUSINCDIR = $(INCDIR)/Modbus

#################################### COMPILE and LINK FLAGS

CFLG =		-c -I$(INCDIR) -DPLATFORM_HEADER=\"./Platform/Linux/platform.h\" -DET_IP_MODBUS -DEIP_QOS \
			-DMBUS_PLATFORM_HEADER=\"./Platform/Linux/mbspltfm.h\"
LFLG =		-lpthread 

#################################### LINUX APPLICATION OBJECT FILES
EADK_OBJ = eiputil.o eipclien.o eiprqst.o eiprqgrp.o eiproutr.o eipcnct.o eipsessn.o eipsckt.o eipucmm.o eipio.o eipcnmgr.o \
			eipasm.o eipnotfy.o eippdu.o eiptrace.o eipid.o eiptcpip.o eipetlnk.o eippccc.o platform.o eipport.o eipdatbl.o eipqos.o \
			eipmbscm.o eipmbus.o mbsclien.o mbsnotfy.o mbsrqst.o mbssessn.o mbstcp.o mbstrace.o mbsutil.o mbspltfm.o

EADK: $(EADK_OBJ)
	$(AR) rcs EADK.a $(EADK_OBJ)

eiputil.o: $(SRCDIR)/eiputil.c $(INCDIR)/eiputil.h
	$(CC) $(CFLG) $(SRCDIR)/eiputil.c

eipclien.o: $(SRCDIR)/eipclien.c $(INCDIR)/eipclien.h
	$(CC) $(CFLG) $(SRCDIR)/eipclien.c

eipdatbl.o: $(SRCDIR)/eipdatbl.c $(INCDIR)/eipdatbl.h
	$(CC) $(CFLG) $(SRCDIR)/eipdatbl.c

eiprqst.o: $(SRCDIR)/eiprqst.c $(INCDIR)/eiprqst.h
	$(CC) $(CFLG) $(SRCDIR)/eiprqst.c

eiprqgrp.o: $(SRCDIR)/eiprqgrp.c $(INCDIR)/eiprqgrp.h
	$(CC) $(CFLG) $(SRCDIR)/eiprqgrp.c

eiproutr.o: $(SRCDIR)/eiproutr.c $(INCDIR)/eiproutr.h
	$(CC) $(CFLG) $(SRCDIR)/eiproutr.c

eipcnct.o: $(SRCDIR)/eipcnct.c $(INCDIR)/eipcnct.h
	$(CC) $(CFLG) $(SRCDIR)/eipcnct.c

eipsessn.o: $(SRCDIR)/eipsessn.c $(INCDIR)/eipsessn.h
	$(CC) $(CFLG) $(SRCDIR)/eipsessn.c

eipsckt.o: $(SRCDIR)/eipsckt.c $(INCDIR)/eipsckt.h
	$(CC) $(CFLG) $(SRCDIR)/eipsckt.c

eipucmm.o: $(SRCDIR)/eipucmm.c $(INCDIR)/eipucmm.h
	$(CC) $(CFLG) $(SRCDIR)/eipucmm.c

eipio.o: $(SRCDIR)/eipio.c $(INCDIR)/eipio.h
	$(CC) $(CFLG) $(SRCDIR)/eipio.c

eipcnmgr.o: $(SRCDIR)/eipcnmgr.c $(INCDIR)/eipcnmgr.h
	$(CC) $(CFLG) $(SRCDIR)/eipcnmgr.c

eipasm.o: $(SRCDIR)/eipasm.c $(INCDIR)/eipasm.h
	$(CC) $(CFLG) $(SRCDIR)/eipasm.c

eipnotfy.o: $(SRCDIR)/eipnotfy.c $(INCDIR)/eipnotfy.h
	$(CC) $(CFLG) $(SRCDIR)/eipnotfy.c

eippdu.o: $(SRCDIR)/eippdu.c $(INCDIR)/eippdu.h
	$(CC) $(CFLG) $(SRCDIR)/eippdu.c

eiptrace.o: $(SRCDIR)/eiptrace.c $(INCDIR)/eiptrace.h
	$(CC) $(CFLG) $(SRCDIR)/eiptrace.c

eipid.o: $(SRCDIR)/eipid.c $(INCDIR)/eipid.h
	$(CC) $(CFLG) $(SRCDIR)/eipid.c

eiptcpip.o: $(SRCDIR)/eiptcpip.c $(INCDIR)/eiptcpip.h
	$(CC) $(CFLG) $(SRCDIR)/eiptcpip.c

eipetlnk.o: $(SRCDIR)/eipetlnk.c $(INCDIR)/eipetlnk.h
	$(CC) $(CFLG) $(SRCDIR)/eipetlnk.c

eippccc.o: $(SRCDIR)/eippccc.c $(INCDIR)/eippccc.h
	$(CC) $(CFLG) $(SRCDIR)/eippccc.c

platform.o: $(SRCDIR)/Platform/Linux/platform.c $(INCDIR)/Platform/Linux/platform.h
	$(CC) $(CFLG) $(SRCDIR)/Platform/Linux/platform.c

eipport.o: $(SRCDIR)/eipport.c $(INCDIR)/eipport.h
	$(CC) $(CFLG) $(SRCDIR)/eipport.c

eipqos.o: $(SRCDIR)/eipqos.c $(INCDIR)/eipqos.h
	$(CC) $(CFLG) $(SRCDIR)/eipqos.c

#################################### CIP-Modbus functionality
eipmbscm.o: $(MODBUSDIR)/eipmbscm.c $(MODBUSINCDIR)/eipmbscm.h
	$(CC) $(CFLG) $(MODBUSDIR)/eipmbscm.c

eipmbus.o: $(MODBUSDIR)/eipmbus.c $(MODBUSINCDIR)/eipmbus.h
	$(CC) $(CFLG) $(MODBUSDIR)/eipmbus.c

mbsclien.o: $(MODBUSDIR)/mbsclien.c $(MODBUSINCDIR)/mbsclien.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbsclien.c

mbsnotfy.o: $(MODBUSDIR)/mbsnotfy.c $(MODBUSINCDIR)/mbsnotfy.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbsnotfy.c

mbsrqst.o: $(MODBUSDIR)/mbsrqst.c $(MODBUSINCDIR)/mbsrqst.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbsrqst.c

mbssessn.o: $(MODBUSDIR)/mbssessn.c $(MODBUSINCDIR)/mbssessn.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbssessn.c

mbstcp.o: $(MODBUSDIR)/mbstcp.c $(MODBUSINCDIR)/mbstcp.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbstcp.c

mbstrace.o: $(MODBUSDIR)/mbstrace.c $(MODBUSINCDIR)/mbstrace.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbstrace.c

mbsutil.o: $(MODBUSDIR)/mbsutil.c $(MODBUSINCDIR)/mbsutil.h
	$(CC) $(CFLG) $(MODBUSDIR)/mbsutil.c

mbspltfm.o: $(SRCDIR)/Platform/Linux/mbspltfm.c $(INCDIR)/Platform/Linux/mbspltfm.h
	$(CC) $(CFLG) $(SRCDIR)/Platform/Linux/mbspltfm.c

clean:
	rm *.o

all:	EADK

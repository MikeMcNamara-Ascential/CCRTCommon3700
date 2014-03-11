###################################################################
#
#   EADKDemo.mk
#
#	Makefile to build EADK Demo.
# 
#   To build EADK Demo application:
#   
#   - Execute "make -f EADKDemo.mk" command
#    
#	This will build EADKDemo executable
#
###################################################################

#################################### MAKE COMMANDS
AR =		ar
CC =		m68k-elf-gcc
CXX =		m68k-elf-g++
OBJCOPY =	m68k-elf-objcopy

#################################### BASE DIRECTORY
BASEDIR =	../..
SRCDIR =	$(BASEDIR)
INCDIR =	$(BASEDIR)
MODBUSDIR = $(SRCDIR)/Modbus
MODBUSINCDIR = $(INCDIR)/Modbus
NAME   = EADKDemo
TARGET	= $(NAME).s19
TARGETAP= $(NAME)_APP.s19
TARGET_DIR = $(BASEDIR)/Platform/Netburner

NBLIBDIR = $(NBROOT)/lib/
NBLIBS = $(NBLIBDIR)NetBurner.a $(NBROOT)/MOD5282/original/lib/MOD5282.a
NBINCLUDE=/include
NBCOMPCODE=compcode
PLATFORM=$(DEFPLATFORM)

#################################### COMPILE and LINK FLAGS

CFLG =		-c -m5206e -gdwarf-2 -Wall -O2 -include "Netburner.pre" -I$(INCDIR) -I$(NBROOT)$(NBINCLUDE)
LFLG =		-m5206e -Wl -Wl,-n -T$(NBROOT)/MOD5282/lib/MOD5282.ld -Wl,-R$(NBROOT)/MOD5282/lib/sys.ld

#################################### NETBURNER APPLICATION OBJECT FILES
EADK_OBJ = eiputil.o eipclien.o eiprqst.o eiprqgrp.o eiproutr.o eipcnct.o eipsessn.o eipsckt.o eipucmm.o eipio.o \
			eipcnmgr.o eipasm.o eipnotfy.o eippdu.o eiptrace.o eipid.o eiptcpip.o eippccc.o eipetlnk.o platform.o \
			eipport.o eipdatbl.o eipnbsoc.o EADK.o \
			eipmbscm.o eipmbus.o mbsclien.o mbsnotfy.o mbsrqst.o mbssessn.o mbstcp.o mbstrace.o mbsutil.o mbspltfm.o

$(TARGETAP): $(TARGET) 
	$(NBCOMPCODE) $(TARGET) $(TARGETAP) $(COMPCODEFLAGS) -P$(PLATFORM)

$(TARGET) : $(EADK_OBJ) $(NBLIBS)
	$(CXX) $(LFLG) -Wl,-Map=$(NAME).map -o $(NAME).elf  $(EADK_OBJ)  -Wl,--start-group,$(NBLIBS) -Wl,--end-group 
	$(OBJCOPY) --strip-all --output-target=srec $(NAME).elf $(TARGET)

EADK.o: $(SRCDIR)/Platform/Netburner/EADK.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/Platform/Netburner/EADK.c $(CFLG)

eiputil.o: $(SRCDIR)/eiputil.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiputil.c $(CFLG)

eipclien.o: $(SRCDIR)/eipclien.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipclien.c $(CFLG)

eipdatbl.o: $(SRCDIR)/eipdatbl.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipdatbl.c $(CFLG)

eiprqst.o: $(SRCDIR)/eiprqst.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiprqst.c $(CFLG) 

eiprqgrp.o: $(SRCDIR)/eiprqgrp.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiprqgrp.c $(CFLG) 

eiproutr.o: $(SRCDIR)/eiproutr.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiproutr.c $(CFLG)

eipcnct.o: $(SRCDIR)/eipcnct.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipcnct.c $(CFLG) 

eipsessn.o: $(SRCDIR)/eipsessn.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipsessn.c $(CFLG) 

eipsckt.o: $(SRCDIR)/eipsckt.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipsckt.c $(CFLG) 

eipnbsoc.o: $(SRCDIR)/Platform/Netburner/eipnbsoc.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/Platform/Netburner/eipnbsoc.c $(CFLG) 

eipucmm.o: $(SRCDIR)/eipucmm.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipucmm.c $(CFLG) 

eipio.o: $(SRCDIR)/eipio.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipio.c $(CFLG) 

eipcnmgr.o: $(SRCDIR)/eipcnmgr.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipcnmgr.c $(CFLG) 

eipasm.o: $(SRCDIR)/eipasm.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipasm.c $(CFLG) 

eipnotfy.o: $(SRCDIR)/eipnotfy.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipnotfy.c $(CFLG) 

eippdu.o: $(SRCDIR)/eippdu.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eippdu.c $(CFLG) 

eiptrace.o: $(SRCDIR)/eiptrace.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiptrace.c $(CFLG) 

eipid.o: $(SRCDIR)/eipid.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipid.c $(CFLG) 

eippccc.o: $(SRCDIR)/eippccc.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eippccc.c $(CFLG) 

eiptcpip.o: $(SRCDIR)/eiptcpip.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eiptcpip.c $(CFLG) 

eipetlnk.o: $(SRCDIR)/eipetlnk.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipetlnk.c $(CFLG) 

platform.o: $(SRCDIR)/Platform/Netburner/platform.c $(INCDIR)/Platform/Netburner/platform.h
	$(CXX) $(SRCDIR)/Platform/Netburner/platform.c $(CFLG) 

eipport.o: $(SRCDIR)/eipport.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/eipport.c $(CFLG)

eipscnr.o: $(SRCDIR)/Scanner/eipscnr.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/Scanner/eipscnr.c $(CFLG) 

eipscmgr.o: $(SRCDIR)/Scanner/eipscmgr.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/Scanner/eipscmgr.c $(CFLG) 

eipcfg.o: $(SRCDIR)/Scanner/eipcfg.c $(INCDIR)/eipinc.h
	$(CXX) $(SRCDIR)/Scanner/eipcfg.c $(CFLG) 

#################################### CIP-Modbus functionality
eipmbscm.o: $(MODBUSDIR)/eipmbscm.c $(MODBUSINCDIR)/eipmbscm.h
	$(CXX) $(MODBUSDIR)/eipmbscm.c $(CFLG) 

eipmbus.o: $(MODBUSDIR)/eipmbus.c $(MODBUSINCDIR)/eipmbus.h
	$(CXX) $(MODBUSDIR)/eipmbus.c $(CFLG) 

mbsclien.o: $(MODBUSDIR)/mbsclien.c $(MODBUSINCDIR)/mbsclien.h
	$(CXX) $(MODBUSDIR)/mbsclien.c $(CFLG) 

mbsnotfy.o: $(MODBUSDIR)/mbsnotfy.c $(MODBUSINCDIR)/mbsnotfy.h
	$(CXX) $(MODBUSDIR)/mbsnotfy.c $(CFLG) 

mbsrqst.o: $(MODBUSDIR)/mbsrqst.c $(MODBUSINCDIR)/mbsrqst.h
	$(CXX) $(MODBUSDIR)/mbsrqst.c $(CFLG) 

mbssessn.o: $(MODBUSDIR)/mbssessn.c $(MODBUSINCDIR)/mbssessn.h
	$(CXX) $(MODBUSDIR)/mbssessn.c $(CFLG) 

mbstcp.o: $(MODBUSDIR)/mbstcp.c $(MODBUSINCDIR)/mbstcp.h
	$(CXX) $(MODBUSDIR)/mbstcp.c $(CFLG) 

mbstrace.o: $(MODBUSDIR)/mbstrace.c $(MODBUSINCDIR)/mbstrace.h
	$(CXX) $(MODBUSDIR)/mbstrace.c $(CFLG) 

mbsutil.o: $(MODBUSDIR)/mbsutil.c $(MODBUSINCDIR)/mbsutil.h
	$(CXX) $(MODBUSDIR)/mbsutil.c $(CFLG) 

mbspltfm.o: $(SRCDIR)/Platform/Netburner/mbspltfm.c $(INCDIR)/Platform/Netburner/mbspltfm.h
	$(CXX) $(SRCDIR)/Platform/Netburner/mbspltfm.c $(CFLG) 

$(NBROOT)/lib/NetBurner$(NBGROUP).a:
	$(MAKE) -C $(NBROOT)/system$(NBGROUP)

clean:
	rm *.o
	rm *.map
	rm *.elf
	rm -f $(TARGET)
	rm -f $(TARGETAP)


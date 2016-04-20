Siemens S7 comm interface source code library (snap 7)
---------------------------------------------------------------------------------------------------
- The library was built using the linux x86/x64 configuration.
- The library is contained in the S7Lib.tgz file.  It needs to go into the following places:
	- in Bins/Core/ServicesLayer/Managers/PlcMgr
	- in /usr/lib on the target machine
	
- If mods are required for the snap 7 library, it can be built using the following command from 
  Source/Core/ServicesLayer/Managers/PlcMgr/S7/build/unix:
	make -f x86_64_linux.mk

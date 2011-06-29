#------------------------------------------------------------------------------------------
# Set default value for Core release if not specified on command line (with -D option)
#------------------------------------------------------------------------------------------
SET(CORE_REL 00000000 CACHE STRING "Core Release")

#------------------------------------------------------------------------------------------
# Set default value for Cust release if not specified on command line (with -D option)
#------------------------------------------------------------------------------------------
SET(CUST_REL 00000000 CACHE STRING "Customer Release")
 
#------------------------------------------------------------------------------------------
# Setup Default for Tools if not specified on command line (with -D option)
#------------------------------------------------------------------------------------------
SET(TOOLS FALSE CACHE BOOL "Build Tools")

#------------------------------------------------------------------------------------------
# Setup Default for Tests if not specified on command line (with -D option)
#------------------------------------------------------------------------------------------
SET(TESTS FALSE CACHE BOOL "Build Tests")


#------------------------------------------------------------------------------------------
# Tell CMake where to install files. These paths are RELATIVE to the base install dir which 
# is set in the $CCRT_ROOT/CMakeCache.txt file.
#------------------------------------------------------------------------------------------
SET(CUSTOMER_SERVICES_BIN_DEPLOY_DIR /ServicesLayer/Bin)
SET(CUSTOMER_SERVICES_CFG_DEPLOY_DIR /Configuration/Servers)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Managers)
ADD_SUBDIRECTORY(Servers)

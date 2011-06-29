#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libmoduleinterfaces.a library
#------------------------------------------------------------------------------------------
SET(CORE_SERVICES_LAYER_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH
	"Path to the root of the core ServicesLayer tree"
	FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake where to install files. These paths are RELATIVE to the base install dir which
# is set in the $CCRT_ROOT/CMakeCache.txt file.
#------------------------------------------------------------------------------------------
SET(SERVICES_LAYER_INSTALL_DIR /ServicesLayer/Bin)
SET(CORE_SL_CFG_DEPLOY_DIR /Configuration/Servers)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)
ADD_SUBDIRECTORY(Managers)
ADD_SUBDIRECTORY(Servers)


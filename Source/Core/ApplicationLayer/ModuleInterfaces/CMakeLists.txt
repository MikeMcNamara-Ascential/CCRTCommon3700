#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the root of the module interface tree
#------------------------------------------------------------------------------------------
SET(CORE_MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the root of the core module interfaces tree" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libmoduleinterfaces.a library
#------------------------------------------------------------------------------------------
SET(CORE_MODULES_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core module interfaces library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the module interface header files
#------------------------------------------------------------------------------------------
SET(MODULE_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common
		      ${CMAKE_CURRENT_SOURCE_DIR}/Include
			${CMAKE_CURRENT_SOURCE_DIR}/Brake
			${CMAKE_CURRENT_SOURCE_DIR}/EngineController
			${CMAKE_CURRENT_SOURCE_DIR}/Transmission)
SET(CORE_MODULES_INC_PATH ${MODULE_PATHS} CACHE STRING
	"Path to the core module interfaces include files" FORCE)

#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE MODULE_INTERFACES *.cpp)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(moduleinterfaces STATIC ${MODULE_INTERFACES})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the library
#------------------------------------------------------------------------------------------
IF(INSTALL_LIBS)
	INSTALL_TARGETS(${LIB_DEPLOY_DIR}/ApplicationLayer/ModuleInterfaces moduleinterfaces)
ENDIF(INSTALL_LIBS)


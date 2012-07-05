#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libprotocolfilters.a library
#------------------------------------------------------------------------------------------
SET(CORE_PROTOCOLS_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core protocol filters library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path(s) to the protocol filter header files
#------------------------------------------------------------------------------------------
SET(PROTOCOLS_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include
	${CMAKE_CURRENT_SOURCE_DIR}/ISO ${CMAKE_CURRENT_SOURCE_DIR}/J2190
	${CMAKE_CURRENT_SOURCE_DIR}/KeyWord2000 ${CMAKE_CURRENT_SOURCE_DIR}/SCI
	${CMAKE_CURRENT_SOURCE_DIR}/GMClassII
	${CMAKE_CURRENT_SOURCE_DIR}/Serial)
SET(CORE_PROTOCOLS_INC_PATH ${PROTOCOLS_PATHS} CACHE STRING
	"Path to the core protocol filter include files" FORCE)

#------------------------------------------------------------------------------------------
# Define the source files needed to build the protocol filter library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE PROTOCOL_FILTERS *.cpp)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(protocolfilters STATIC ${PROTOCOL_FILTERS})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the library
#------------------------------------------------------------------------------------------
IF(INSTALL_LIBS)
	INSTALL_TARGETS(${LIB_DEPLOY_DIR}/ApplicationLayer/ProtocolFilters protocolfilters)
ENDIF(INSTALL_LIBS)


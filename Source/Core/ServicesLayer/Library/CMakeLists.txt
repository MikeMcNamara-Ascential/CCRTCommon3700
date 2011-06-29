#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libserviceslayer.a library
#------------------------------------------------------------------------------------------
SET(CORE_SERVICES_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the libserviceslayer.a library" FORCE)

#------------------------------------------------------------------------------------------
# We need to build the license library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(License)

#------------------------------------------------------------------------------------------
# make sure the parser can find our expat headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(BEFORE
        ${CMAKE_CURRENT_SOURCE_DIR}/SoftwareId
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlProcessor
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlProcessor/xmltok
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlProcessor/xmlwf)

#------------------------------------------------------------------------------------------
# Define some shortcuts for the sub-directory source files
#------------------------------------------------------------------------------------------
FILE(GLOB BEPOS_SOURCES Bepos/*.c)
FILE(GLOB SYSCOMM_COURCES SystemCommunication/*.cpp)
FILE(GLOB SW_ID_COURCES SoftwareId/*.cpp)
FILE(GLOB XML_SOURCES XmlProcessor/*.cpp
			XmlProcessor/*.c
			XmlProcessor/xmlparse/*.c
			XmlProcessor/xmltok/xmltok.c
			XmlProcessor/xmltok/xmlrole.c)
FILE(GLOB SL_LIB_SOURCES *.cpp)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(serviceslayer STATIC ${BEPOS_SOURCES} ${SYSCOMM_COURCES} ${SW_ID_COURCES} ${XML_SOURCES} ${SL_LIB_SOURCES})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
IF(INSTALL_LIBS)
	INSTALL_TARGETS(${LIB_DEPLOY_DIR}/ServicesLayer/Library serviceslayer)
ENDIF(INSTALL_LIBS)

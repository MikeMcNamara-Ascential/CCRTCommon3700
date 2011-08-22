#------------------------------------------------------------------------------------------
# Set some cache variables that point to the link and include directories for the licensing
# code
#------------------------------------------------------------------------------------------
SET(LICENSE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Lib Source Dir" FORCE)
SET(LICENSE_LIB_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "Lib Bin Dir" FORCE)

#------------------------------------------------------------------------------------------
# If the license lib has not been extracted yet, extract it
#------------------------------------------------------------------------------------------
IF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Common/liblicense.a)
	EXEC_PROGRAM(tar ARGS -zxvf ${CMAKE_CURRENT_SOURCE_DIR}/License.tgz -C ${CMAKE_CURRENT_SOURCE_DIR}/
				OUTPUT_VARIABLE IGNORE_ME)
ENDIF(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Common/liblicense.a)

#------------------------------------------------------------------------------------------
# make sure the parser can find our headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(BEFORE ${LICENSE_LIB_DIR}/Include)

#LINK_DIRECTORIES(${LICENSE_LIB_DIR}/Common)
#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(cmlicense STATIC CCRTLicense.cpp)

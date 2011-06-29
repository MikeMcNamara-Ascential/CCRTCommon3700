#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libtcasecomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_TCASE_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core t-case component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the t-case component headers
#------------------------------------------------------------------------------------------
SET(TCASE_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_TCASE_TC_INC_PATH ${TCASE_INC_PATHS} CACHE STRING
	"Path(s) to the core t-case component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_TCASE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_TCASE_TC_LIB_PATH})
LINK_LIBRARIES(tcasecomponent m)

#------------------------------------------------------------------------------------------
# Build the executable after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(GenericTransferCase)
